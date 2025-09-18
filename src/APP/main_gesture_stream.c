// ===== Smart Glove Main (TX-only UI + letter RX) =====
// IMT LCD API + gesture_stream + gesture_stream_platform
//
// UI behavior:
//   - No per-finger roll/pitch display
//   - After sending a frame: show "Frame sent"
//   - Wait briefly for a predicted letter from ESP32 and display it
//
// Packet to ESP32 (little-endian):
//   [0xAA55][0x01][ts_ms(uint32)][10 x int16 centideg][crc16-ccitt]
//
// Build flags example:
//   avr-gcc -mmcu=atmega32 -DF_CPU=8000000UL ...

#include "STD_TYPES.h"
#include "I2C_Interface.h"
#include "LCD_Interface.h"
#include "UART_Interface.h"
#include "UART_Register.h"          // UDR/UCSRA/RXC
#include "SOFT_UART.h"              // Soft UART for DFPlayer (TX=PD2, RX=PD3/INT1)

#include "gesture_stream.h"           // smart_glove_init()
#include "gesture_stream_platform.h"  // read_all_sensors_angles(), gstream_get_timestamp_ms()
// REMOVED for memory: #include "sensor_debug.h" - DEBUG: Sensor orientation debugging

#include <util/delay.h>
#include <string.h>

#ifndef NUM_FINGERS
#define NUM_FINGERS 5
#endif

#define WAIT_LETTER_TIMEOUT  800   // ms to wait for predicted letter after sending (increased for DFPlayer)
#define BETWEEN_FRAMES_MS    100   // inter-frame idle (increased to allow audio playback)
#define DEBUG_MODE            0    // MEMORY OPTIMIZATION: Disabled to save RAM (was 1)

/* ================= UART helpers ================= */
static void uart_send_bytes(const u8 *buf, u16 len) {
    for (u16 i = 0; i < len; ++i) UART_voidSendChar((char)buf[i]);
}

// Non-blocking read of one byte from UART (using interrupt buffer)
static u8 uart_try_read_byte(u8 *out) {
    return UART_u8ReceiveByteNonBlocking(out);
}

// Circular buffer for UART input to prevent race conditions
// Using the same buffer size as defined in UART_Interface.h
#undef UART_BUFFER_SIZE  // Undefine first to avoid redefinition warnings
static struct {
    char buffer[32];  // Use the value from UART_Interface.h
    u8 read_index;
    u8 write_index;
    u8 count;
} uart_buffer = {0};

// Called from UART interrupt or polling to add bytes to buffer
void uart_buffer_add_byte(u8 b) {
    if (uart_buffer.count < 32) { // Use literal value instead of macro
        uart_buffer.buffer[uart_buffer.write_index] = b;
        uart_buffer.write_index = (uart_buffer.write_index + 1) % 32; // Use literal value
        uart_buffer.count++;
    }
}

// Read a byte from the buffer (returns 1 if byte available)
static u8 uart_buffer_get_byte(u8 *out) {
    if (uart_buffer.count == 0) {
        return 0;
    }
    
    *out = uart_buffer.buffer[uart_buffer.read_index];
    uart_buffer.read_index = (uart_buffer.read_index + 1) % 32; // Use literal value instead of macro
    uart_buffer.count--;
    return 1;
}

// Process incoming UART data (call this regularly)
static void uart_process_incoming(void) {
    u8 b;
    while (uart_try_read_byte(&b)) {
        uart_buffer_add_byte(b);
    }
}

// Collect a single ASCII letter (terminated by '\n') coming from ESP32
static u8 uart_try_read_letter(char *outLetter) {
    static u8 has_pending = 0;
    static char pending = 0;
    
    // Process any new bytes
    uart_process_incoming();
    
    // Check if there's any data in the buffer
    u8 b;
    if (!uart_buffer_get_byte(&b)) return 0;
    
    if (b == '\n' || b == '\r') {
        if (has_pending) {
            *outLetter = pending;
            has_pending = 0;
            return 1;
        }
        return 0;
    }
    
    // Store the character
    pending = (char)b;
    has_pending = 1;
    return 0;
}

// Blocking with timeout (ms) - returns 1 if letter received
static u8 wait_for_letter(char *outLetter, u16 timeout_ms) {
    for (u16 t = 0; t < timeout_ms; ++t) {
        if (uart_try_read_letter(outLetter)) return 1;
        _delay_ms(1);
    }
    return 0;
}

/* ================= CRC16-CCITT (0x1021, init=0xFFFF) ================= */
static u16 crc16_ccitt(const u8 *data, u16 len) {
    u16 crc = 0xFFFFu;
    for (u16 i = 0; i < len; ++i) {
        crc ^= (u16)data[i] << 8;
        for (u8 b = 0; b < 8; ++b) {
            if (crc & 0x8000u) crc = (u16)((crc << 1) ^ 0x1021u);
            else               crc <<= 1;
        }
    }
    return crc;
}

/* ================= Packet builder ================= */
typedef struct __attribute__((packed)) {
    u16 header;     // 0xAA55
    u8  version;    // 0x01
    u32 ts_ms;      // timestamp
    s16 angles[NUM_FINGERS * 2];  // roll0,pitch0,...,roll4,pitch4 in centideg
    u16 crc;        // CRC16 over [version..angles]
} gpacket_t;

static void send_angles_packet(const float roll_deg[NUM_FINGERS],
                               const float pitch_deg[NUM_FINGERS]) {
    // Input validation
    if (!roll_deg || !pitch_deg) {
        return;
    }
    
    gpacket_t pkt;
    pkt.header  = 0xAA55;
    pkt.version = 0x01;
    pkt.ts_ms   = gstream_get_timestamp_ms();

    // Maximum value for centidegrees in s16 type
    // s16 range: -32768 to +32767
    // Centidegrees at ±180° would be ±18000, which fits in s16
    // Using 180° as absolute max (±180° range for roll, ±90° for pitch)
    const float MAX_ANGLE_DEG = 180.0f;
    
    for (u8 i = 0; i < NUM_FINGERS; ++i) {
        // Check for NaN/infinity values
        float roll_clamped = isnan(roll_deg[i]) || isinf(roll_deg[i]) ? 0.0f : roll_deg[i];
        float pitch_clamped = isnan(pitch_deg[i]) || isinf(pitch_deg[i]) ? 0.0f : pitch_deg[i];
        
        // Clamp to safe ranges with explicit min/max functions
        roll_clamped = fmaxf(-MAX_ANGLE_DEG, fminf(MAX_ANGLE_DEG, roll_clamped));
        pitch_clamped = fmaxf(-90.0f, fminf(90.0f, pitch_clamped));
        
        // Convert to centidegrees with careful conversion to avoid overflow
        s32 roll_centideg = (s32)(roll_clamped * 100.0f);
        s32 pitch_centideg = (s32)(pitch_clamped * 100.0f);
        
        // Final bounds check before casting to s16
        if (roll_centideg > 32767) roll_centideg = 32767;
        if (roll_centideg < -32768) roll_centideg = -32768;
        if (pitch_centideg > 32767) pitch_centideg = 32767;
        if (pitch_centideg < -32768) pitch_centideg = -32768;
        
        pkt.angles[i * 2 + 0] = (s16)roll_centideg;
        pkt.angles[i * 2 + 1] = (s16)pitch_centideg;
    }

    const u8 *crc_start = (const u8 *)&pkt.version;
    u16 crc_len = (u16)(sizeof(pkt.version) + sizeof(pkt.ts_ms) + sizeof(pkt.angles));
    pkt.crc = crc16_ccitt(crc_start, crc_len);

    uart_send_bytes((const u8 *)&pkt, sizeof(pkt));
}

/* ================= Function forward declarations ================= */
static uint16_t letter_to_track(char L);

/* ================= LCD helpers - FIXED timing issues ================= */
static void lcd_show_status(const char* line1, const char* line2) {
    LCD_Clear();
    _delay_ms(5);  // FIXED: Add delay after clear for LCD stability
    LCD_vidGotoxy(0, 0);
    LCD_voidSendString((char*)line1);
    LCD_vidGotoxy(0, 1);
    LCD_voidSendString((char*)line2);
}

static void lcd_show_prediction_letter(char letter) {
    if (letter == '_') {
        lcd_show_status("Prediction: Rest", "File: STOP");
    } else if (letter == '?') {
        lcd_show_status("Prediction: Unkn", "File: 099.mp3");
    } else {
        char display_line1[17];  // 16 chars + null terminator for LCD
        char display_line2[17];
        uint16_t track = letter_to_track(letter);
        
        // Format: "Pred: A" on line 1
        display_line1[0] = 'P'; display_line1[1] = 'r'; display_line1[2] = 'e'; 
        display_line1[3] = 'd'; display_line1[4] = ':'; display_line1[5] = ' ';
        display_line1[6] = letter; 
        display_line1[7] = ' '; display_line1[8] = 'T'; display_line1[9] = ':';
        // Add track number to display for debugging
        if (track >= 10) {
            display_line1[10] = '0' + (track / 10);
            display_line1[11] = '0' + (track % 10);
            display_line1[12] = '\0';
        } else {
            display_line1[10] = '0' + track;
            display_line1[11] = '\0';
        }
        
        // Format: "File: 001.mp3" on line 2
        display_line2[0] = 'F'; display_line2[1] = 'i'; display_line2[2] = 'l'; 
        display_line2[3] = 'e'; display_line2[4] = ':'; display_line2[5] = ' ';
        
        if (track == 99) {
            display_line2[6] = '0'; display_line2[7] = '9'; display_line2[8] = '9';
        } else {
            display_line2[6] = '0';  // First digit always 0
            display_line2[7] = '0' + (track / 10);  // Second digit
            display_line2[8] = '0' + (track % 10);  // Third digit
        }
        display_line2[9] = '.'; display_line2[10] = 'm'; display_line2[11] = 'p';
        display_line2[12] = '3'; display_line2[13] = '\0';
        
        LCD_Clear();
        _delay_ms(5);  // FIXED: Add delay after clear
        LCD_vidGotoxy(0, 0);
        LCD_voidSendString(display_line1);
        LCD_vidGotoxy(0, 1);
        LCD_voidSendString(display_line2);
    }
}

/* ================= DFPlayer Mini via Soft UART (TX=PD2) ================= */
/* Wire: PD2 (SOFT UART TX) -> DFPlayer RX, GND↔GND. DFPlayer default baud 9600. */

// Send a 10-byte DFPlayer frame: 7E FF 06 CMD 00 HH LL CKH CKL EF
static void DF_send(uint8_t cmd, uint16_t param) {
    uint8_t f[10];
    f[0]=0x7E; f[1]=0xFF; f[2]=0x06; f[3]=cmd; f[4]=0x00;
    f[5]=(uint8_t)(param>>8); f[6]=(uint8_t)(param&0xFF);
    uint16_t sum=0; for(uint8_t i=1;i<=6;i++) sum+=f[i];
    uint16_t chk=0 - sum;
    f[7]=(uint8_t)(chk>>8); f[8]=(uint8_t)(chk&0xFF); f[9]=0xEF;

    for(uint8_t i=0;i<10;i++){
        SOFT_UART_voidPutChar(f[i]);   // send via Soft UART
        _delay_us(100);  // Increased delay between bytes for better reliability
    }
    _delay_ms(50);  // Wait for DFPlayer to process command
}

static inline void DF_setVolume(uint8_t v){ if(v>30)v=30; DF_send(0x06, v); }
static inline void DF_playTrack(uint16_t t){ DF_send(0x03, t); }   // 001.mp3 -> 1
static inline void DF_stop(void){ DF_send(0x16, 0); }

// Map 'A'..'Z' => track numbers based on actual SD card file order
// FILES ARE SHUFFLED ON SD CARD! DFPlayer reads by copy order, not filename order
// Based on your tested mapping:
static const uint16_t letter_track_map[26] = {
    // A, B, C, D, E, F, G, H, I, J,  K,  L,  M,  N,  O,  P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z
      15, 8,23,43, 1,17, 3, 6, 1, 1, 26, 20, 19,42, 16, 1, 1,32,41,33,37,34, 1,21, 9,14
    // Updated mappings:
    // A=15, B=8, C=23, D=43, G=3, H=6, K=26, L=20, M=19, N=42, O=16, R=32, S=41, T=33, U=37, V=34, X=21, Y=9, Z=14
    // Still unknown: E, F, I, J, P, Q, W (set to track 1 temporarily)
};

// TEST MODE: Uncomment this to enable sequential track testing
//#define TEST_MODE_ENABLED 0

// MODE 2: Word creation mode - collects 3 letters to form words
//#define MODE_2_ENABLED 1

// ESP32 sends char data type: ASCII values for letters
static uint16_t letter_to_track(char L) {
    // Handle special cases first
    if (L == '_' || L == 0)  return 0;      // Rest/stop
    if (L == '?')   return 99;              // Unknown/error
    
    // Handle uppercase letters A-Z (ASCII 65-90) with custom mapping
    if (L >= 'A' && L <= 'Z') {
        uint8_t index = L - 'A';  // 0-25
        return letter_track_map[index];
    }
    
    // Handle lowercase letters a-z (ASCII 97-122) with custom mapping
    if (L >= 'a' && L <= 'z') {
        uint8_t index = L - 'a';  // 0-25
        return letter_track_map[index];
    }
    
    // Any other character -> error sound
    return 99;
}

static void play_by_letter(char L) {
    uint16_t tr = letter_to_track(L);
    if (tr == 0) {
        DF_stop();
    } else {
        // Some DFPlayer modules might need different track numbering
        // Try both standard numbering and 0-based indexing if issues persist
        DF_playTrack(tr);
        _delay_ms(100);  // Give DFPlayer time to start playback
        
        // Debug: Send track info via UART for verification
        UART_voidSendString("Playing: ");
        UART_voidSendChar(L);
        UART_voidSendString(" -> Track ");
        // Simple number to string conversion for track number
        if (tr >= 10) {
            UART_voidSendChar('0' + (tr / 10));
        }
        UART_voidSendChar('0' + (tr % 10));
        UART_voidSendString("\r\n");
    }
}

#ifdef MODE_2_ENABLED
/* ================= MODE 2: Word Creation Functions ================= */

// Display current word progress on LCD
static void lcd_show_word_progress(const char* word, uint8_t position) {
    char line1[17] = "Word: ";
    char line2[17] = "Pos: ";
    
    // Show current word (with underscores for missing letters)
    for (uint8_t i = 0; i < 3; i++) {
        if (i < position) {
            line1[6 + i] = word[i];
        } else {
            line1[6 + i] = '_';
        }
    }
    line1[9] = '\0';
    
    // Show position
    line2[5] = '0' + (position + 1);
    line2[6] = '/';
    line2[7] = '3';
    line2[8] = '\0';
    
    LCD_Clear();
    _delay_ms(5);
    LCD_vidGotoxy(0, 0);
    LCD_voidSendString(line1);
    LCD_vidGotoxy(0, 1);
    LCD_voidSendString(line2);
}

// Display completed word
static void lcd_show_completed_word(const char* word) {
    char line1[17] = "Word: ";
    char line2[17] = "Complete!";
    
    // Copy word to display
    for (uint8_t i = 0; i < 3; i++) {
        line1[6 + i] = word[i];
    }
    line1[9] = '\0';
    
    LCD_Clear();
    _delay_ms(5);
    LCD_vidGotoxy(0, 0);
    LCD_voidSendString(line1);
    LCD_vidGotoxy(0, 1);
    LCD_voidSendString(line2);
}

// Play all letters in the word sequentially
static void play_word_sequence(const char* word) {
    UART_voidSendString("Playing word: ");
    UART_voidSendString((char*)word);
    UART_voidSendString("\r\n");
    
    for (uint8_t i = 0; i < 3; i++) {
        if (word[i] != 0) {
            play_by_letter(word[i]);
            _delay_ms(800); // Wait between letters
        }
    }
}

// Reset word creation state
static void reset_word_state(char* word, uint8_t* position) {
    word[0] = word[1] = word[2] = word[3] = 0;
    *position = 0;
}

// Check if letter is valid (A-Z or a-z)
static uint8_t is_valid_letter(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

#endif

/* ================= main ================= */
int main(void) {
    I2C_voidInit();
    UART_voidInitWithInterrupt();  // Use interrupt-based UART for better performance
    LCD_Init();
    
    // Soft UART for DFPlayer (TX=PD2, RX=PD3/INT1 @ 9600)
    SOFT_UART_voidInit();
    _delay_ms(500);         // Wait for DFPlayer to fully boot
    DF_setVolume(22);       // 0..30
    _delay_ms(100);         // Wait for volume command to process
    
    // FIXED: Proper LCD initialization sequence with delays
    _delay_ms(100);  // Wait for LCD to fully initialize
    LCD_Clear();
    _delay_ms(10);   // Ensure clear operation completes
    LCD_vidGotoxy(0, 0);
    LCD_voidSendString("Smart Glove Boot");
    _delay_ms(1000); // Show boot message for 1 second

    if (!smart_glove_init()) {
        lcd_show_status("Init failed", "Check sensors");
        while (1) { 
            _delay_ms(1000); // Prevent busy waiting
        }
    }

    lcd_show_status("Init OK", "Starting...");
    _delay_ms(1000); // Show init success for 1 second

#ifdef TEST_MODE_ENABLED
    // TEST MODE: Play tracks 1-44 sequentially to map SD card order
    UART_voidSendString("\r\n=== SD CARD MAPPING TEST (44 FILES) ===\r\n");
    lcd_show_status("TEST MODE", "Mapping 44 files");
    
    for (uint16_t track = 1; track <= 44; track++) {
        char track_str[12];
        track_str[0] = 'T'; track_str[1] = 'r'; track_str[2] = ':';
        
        // Handle numbers 1-44 (up to 2 digits)
        if (track >= 10) {
            track_str[3] = '0' + (track / 10);
            track_str[4] = '0' + (track % 10);
            track_str[5] = '\0';
        } else {
            track_str[3] = '0'; track_str[4] = '0' + track;
            track_str[5] = '\0';
        }
        
        lcd_show_status("TEST MODE", track_str);
        
        // UART output with proper 2-digit formatting
        UART_voidSendString("Track ");
        if (track < 10) {
            UART_voidSendChar('0');
            UART_voidSendChar('0' + track);
        } else {
            UART_voidSendChar('0' + (track / 10));
            UART_voidSendChar('0' + (track % 10));
        }
        UART_voidSendString(" playing...\r\n");
        
        DF_playTrack(track);
        _delay_ms(2500);  // Play for 2.5 seconds (shorter for 44 files)
        DF_stop();
        _delay_ms(500);   // Shorter pause between tracks
    }
    
    lcd_show_status("Test complete", "Check UART log");
    UART_voidSendString("=== TEST COMPLETE (44 FILES) ===\r\n");
    _delay_ms(5000);
#endif

#ifdef MODE_2_ENABLED
    // MODE 2: Word creation mode
    UART_voidSendString("\r\n=== MODE 2: WORD CREATION ===\r\n");
    UART_voidSendString("Collecting 3-letter words from gestures\r\n");
    lcd_show_status("MODE 2 Active", "3-Letter Words");
    _delay_ms(2000);
#endif

#if DEBUG_MODE
    // DEBUG MODE: Run sensor orientation debugging
    UART_voidSendString("\r\n🔧 DEBUG MODE ENABLED 🔧\r\n");
    UART_voidSendString("Running sensor orientation analysis...\r\n");
    
    run_sensor_debug_menu();
    
    UART_voidSendString("\r\nDebug complete. Starting normal operation in 5 seconds...\r\n");
    lcd_show_status("Debug Mode", "Check UART");
    _delay_ms(5000);
#endif

    float roll[NUM_FINGERS];
    float pitch[NUM_FINGERS];
    char letter = 0;
    static char last_letter = 0;

    // MODE 2: Word creation state variables (declared always, used conditionally)
    static char current_word[4] = {0}; // 3 letters + null terminator
    static uint8_t word_position = 0;  // 0, 1, or 2 for current letter position
    static uint32_t last_letter_time = 0;
    const uint32_t LETTER_TIMEOUT_MS = 3000; // 3 seconds to complete a word

#ifdef MODE_2_ENABLED
    // Initialize word state only in MODE_2
    reset_word_state(current_word, &word_position);
    lcd_show_word_progress(current_word, word_position);
    last_letter_time = gstream_get_timestamp_ms();
#endif

    while (1) {
        // FIXED: Update timestamp each cycle (40ms intervals)
        gesture_stream_tick_ms(BETWEEN_FRAMES_MS);
        
        // 1) Read angles (no LCD print)
        if (!read_all_sensors_angles(roll, pitch)) {
            lcd_show_status("Read fail", "");
            _delay_ms(300);
            continue;
        }

        // 2) Send framed packet to ESP32
        send_angles_packet(roll, pitch);

        // 3) Notify user and wait for reply letter
        lcd_show_status("Frame sent", "Waiting...");
        if (wait_for_letter(&letter, WAIT_LETTER_TIMEOUT)) {
#ifdef MODE_2_ENABLED
            // MODE 2: Word creation logic
            uint32_t current_time = gstream_get_timestamp_ms();
            
            // Check for timeout (reset word if too much time passed)
            if (current_time - last_letter_time > LETTER_TIMEOUT_MS) {
                reset_word_state(current_word, &word_position);
                lcd_show_word_progress(current_word, word_position);
                UART_voidSendString("Word timeout - reset\r\n");
            }
            
            // Process the received letter
            if (is_valid_letter(letter) && letter != last_letter) {
                // Add letter to current word
                current_word[word_position] = letter;
                word_position++;
                last_letter_time = current_time;
                last_letter = letter;
                
                UART_voidSendString("Added letter: ");
                UART_voidSendChar(letter);
                UART_voidSendString(" at position ");
                UART_voidSendChar('0' + word_position);
                UART_voidSendString("\r\n");
                
                if (word_position >= 3) {
                    // Word complete - play it and reset
                    current_word[3] = '\0'; // Ensure null termination
                    lcd_show_completed_word(current_word);
                    _delay_ms(1000); // Show completed word
                    
                    play_word_sequence(current_word);
                    
                    // Reset for next word
                    reset_word_state(current_word, &word_position);
                    _delay_ms(2000); // Pause between words
                    lcd_show_word_progress(current_word, word_position);
                } else {
                    // Show progress
                    lcd_show_word_progress(current_word, word_position);
                    play_by_letter(letter); // Still play individual letter
                }
            } else if (letter == '_') {
                // Rest gesture - could be used to reset word
                reset_word_state(current_word, &word_position);
                lcd_show_word_progress(current_word, word_position);
                UART_voidSendString("Word reset by rest gesture\r\n");
                last_letter = letter;
            }
#else
            // Original MODE: Single letter processing
            // Play only on change to avoid repeated triggers
            if (letter != last_letter) {
                play_by_letter(letter);          // ---- DFPlayer trigger ----
                last_letter = letter;
            }
            lcd_show_prediction_letter(letter);
#endif
            _delay_ms(200);  // Give time to read prediction and hear audio
        } else {
#ifdef MODE_2_ENABLED
            lcd_show_status("Frame sent", "No reply");
            // Check timeout in no-reply case too
            uint32_t current_time = gstream_get_timestamp_ms();
            if (current_time - last_letter_time > LETTER_TIMEOUT_MS && word_position > 0) {
                reset_word_state(current_word, &word_position);
                lcd_show_word_progress(current_word, word_position);
                UART_voidSendString("Word timeout - reset (no reply)\r\n");
            }
#else
            lcd_show_status("Frame sent", "No reply");
#endif
        }

        _delay_ms(BETWEEN_FRAMES_MS);
    }

    return 0;
}
