#include "gesture_stream_platform.h"  // NEW
#include "axis_mapping_fix.h"          // ⭐ AXIS MAPPING FIX
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdlib.h>                   // For abs() function
#include <stdbool.h>                  // For bool type
#include <util/delay.h>               // For _delay_ms()
#include "I2C_Interface.h"
#include "UART_Interface.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Integration.h"     // For initialization functions
#include "MPU6050_INTERFACE.h"        // For MPU6050_Status_t definition
#include "fixed_point.h"              // Fixed point math for improved performance

/* Explicit handling for isnan/isinf if math.h doesn't provide them */
#ifndef isnan
#define isnan(x) ((x) != (x))
#endif
#ifndef isinf
#define isinf(x) (((x) == INFINITY) || ((x) == -INFINITY))
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* timestamp source, ticked from ISR */
static volatile uint32_t g_ms = 0;
void gesture_stream_tick_ms(uint16_t delta_ms) { g_ms += delta_ms; }
uint32_t gstream_get_timestamp_ms(void) { return g_ms; }

/* UART bridge (binary write) */
int uart_write_bytes(const uint8_t *data, uint16_t len)
{
    if (!data || !len) return -1;
    for (uint16_t i = 0; i < len; ++i) {
        UART_voidSendByte((u8)data[i]);
    }
    return (int)len;
}

/* Map finger index -> (mux channel, mpu id) */
typedef struct { u8 ch; u8 id; } finger_map_t;
static const finger_map_t k_fmap[NUM_FINGERS] = { {0,0},{1,1},{2,2},{3,3},{4,4} };

/* Complementary Filter Parameters - FIXED for consistency */
#define ALPHA 0.93f                    // Blending factor (93% gyro, 7% accel) - matches documentation
#define DT_MS 40.0f                    // Sampling time in milliseconds (40ms = 25Hz)
#define DT_SEC (DT_MS / 1000.0f)       // Converted to seconds (0.04s)
/* MPU6050 Scaling Constants */
#define ACCEL_SCALE (1.0f / 16384.0f)  // Convert accel raw to G (for ±2g range: 16384 LSB/g)
#define GYRO_SCALE (1.0f / 131.0f)     // Convert gyro raw to deg/s (for ±250°/s range: 131 LSB/°/s)

/* Fixed-point equivalents - defined in fixed_point.c */
extern const fixed_t FP_ALPHA;          // 0.93 in fixed-point (16.16)
extern const fixed_t FP_ONE_MINUS_ALPHA; // 0.07 in fixed-point (16.16)
extern const fixed_t FP_GYRO_SCALE_DT;   // GYRO_SCALE * DT_SEC in fixed-point

/* Fixed Point Versions for Improved Performance */
#define FP_ALPHA FP_FROM_FLOAT(0.93f)       // Alpha in fixed point
#define FP_ONE_MINUS_ALPHA FP_FROM_FLOAT(0.07f) // (1-alpha) in fixed point
#define FP_DT_SEC FP_FROM_FLOAT(0.04f)      // 40ms in seconds as fixed point
// Pre-scale gyro to avoid multiple multiplications
#define FP_GYRO_SCALE_DT FP_FROM_FLOAT((1.0f/131.0f) * 0.04f)

/* Previous angle states for complementary filter */
static float prev_roll[NUM_FINGERS] = {0};
static float prev_pitch[NUM_FINGERS] = {0};

/* Calculate angles from accelerometer only (gravity reference) - FIXED for consistency */
static void compute_angles_from_accel(s16 ax, s16 ay, s16 az,
                                      float* out_roll_deg, float* out_pitch_deg,
                                      u8 finger_idx)
{
    // Input validation
    if (!out_roll_deg || !out_pitch_deg) {
        return;
    }
    
    // Use the fixed compute_angles function from axis_mapping_fix.h
    // This handles both standard mapping and finger-specific axis swapping
    compute_angles_fixed(ax, ay, az, out_roll_deg, out_pitch_deg, finger_idx);
}

/* Complementary filter fusion with fixed-point optimization */
static void apply_complementary_filter(u8 finger_idx, 
                                       float accel_roll, float accel_pitch,
                                       float gyro_x, float gyro_y,
                                       float* out_roll, float* out_pitch)
{
    // Input validation with explicit NULL checks
    if (finger_idx >= NUM_FINGERS || !out_roll || !out_pitch) {
        if (out_roll) *out_roll = 0.0f;
        if (out_pitch) *out_pitch = 0.0f;
        return;
    }
    
    // Check for NaN values in input (protect against bad math)
    if (isnan(accel_roll) || isnan(accel_pitch) || 
        isnan(gyro_x) || isnan(gyro_y)) {
        *out_roll = 0.0f;
        *out_pitch = 0.0f;
        return;
    }
    
    // Initialize each finger individually on first use
    static bool finger_initialized[NUM_FINGERS] = {false, false, false, false, false};
    
    if (!finger_initialized[finger_idx]) {
        // First time for this finger: initialize with accelerometer values
        prev_roll[finger_idx] = accel_roll;
        prev_pitch[finger_idx] = accel_pitch;
        finger_initialized[finger_idx] = true;
        
        // For first reading, just return accelerometer values
        *out_roll = accel_roll;
        *out_pitch = accel_pitch;
        return;
    }
    
    // Convert to fixed-point for faster calculations on AVR
    fixed_t fp_accel_roll = FP_FROM_FLOAT(accel_roll);
    fixed_t fp_accel_pitch = FP_FROM_FLOAT(accel_pitch);
    fixed_t fp_prev_roll = FP_FROM_FLOAT(prev_roll[finger_idx]);
    fixed_t fp_prev_pitch = FP_FROM_FLOAT(prev_pitch[finger_idx]);
    
    // Optimize gyro calculation by doing it all in fixed point
    // This combines the conversion to deg/s and the integration in one step
    fixed_t fp_gyro_roll_delta = fp_mul(FP_FROM_INT(gyro_x), FP_GYRO_SCALE_DT);
    fixed_t fp_gyro_pitch_delta = fp_mul(FP_FROM_INT(gyro_y), FP_GYRO_SCALE_DT);
    
    // Integrate the gyro values
    fixed_t fp_gyro_roll = fp_add(fp_prev_roll, fp_gyro_roll_delta);
    fixed_t fp_gyro_pitch = fp_add(fp_prev_pitch, fp_gyro_pitch_delta);
    
    // Clamp the integrated values
    fp_gyro_roll = fp_clamp(fp_gyro_roll, FP_FROM_FLOAT(-180.0f), FP_FROM_FLOAT(180.0f));
    fp_gyro_pitch = fp_clamp(fp_gyro_pitch, FP_FROM_FLOAT(-90.0f), FP_FROM_FLOAT(90.0f));
    
    // Apply complementary filter in fixed point
    // result = alpha * gyro + (1-alpha) * accel
    fixed_t fp_roll = fp_add(fp_mul(FP_ALPHA, fp_gyro_roll), 
                           fp_mul(FP_ONE_MINUS_ALPHA, fp_accel_roll));
                           
    fixed_t fp_pitch = fp_add(fp_mul(FP_ALPHA, fp_gyro_pitch), 
                            fp_mul(FP_ONE_MINUS_ALPHA, fp_accel_pitch));
    
    // Final clamping
    fp_roll = fp_clamp(fp_roll, FP_FROM_FLOAT(-180.0f), FP_FROM_FLOAT(180.0f));
    fp_pitch = fp_clamp(fp_pitch, FP_FROM_FLOAT(-90.0f), FP_FROM_FLOAT(90.0f));
    
    // Convert back to float for interface compatibility
    *out_roll = FP_TO_FLOAT(fp_roll);
    *out_pitch = FP_TO_FLOAT(fp_pitch);
    
    // Update previous states with filtered results
    prev_roll[finger_idx] = *out_roll;
    prev_pitch[finger_idx] = *out_pitch;
}

bool read_all_sensors_angles(float *roll_deg, float *pitch_deg)
{
    // Input validation
    if (!roll_deg || !pitch_deg) {
        return false;
    }

    // Initialize output arrays with zeros (in case of partial failures)
    for (u8 i = 0; i < NUM_FINGERS; ++i) {
        roll_deg[i] = 0.0f;
        pitch_deg[i] = 0.0f;
    }

    // Track successful sensor reads for graceful degradation
    u8 successful_reads = 0;

    for (u8 i = 0; i < NUM_FINGERS; ++i) {
        const u8 ch = k_fmap[i].ch;
        const u8 id = k_fmap[i].id;

        // Select multiplexer channel with error handling
        if (TCA9548A_SelectChannel(ch) != TCA9548A_OK) {
            continue;  // Skip this sensor but try others
        }

        // Add delay for channel settling
        _delay_ms(2);  // 2ms delay for I2C mux stabilization

        // Read both accelerometer and gyroscope data with error checking
        s16 ax = 0, ay = 0, az = 0;
        s16 gx = 0, gy = 0, gz = 0;
        
        // Use unified read function with error checking
        MPU6050_Status_t status = MPU6050_enumReadAll(id, &ax, &ay, &az, NULL, &gx, &gy, &gz);
        if (status != MPU6050_OK) {
            // Skip this sensor on error, but continue with others
            continue;
        }
        
        // Calculate accelerometer-based angles (gravity reference)
        float accel_roll = 0.0f, accel_pitch = 0.0f;
        compute_angles_from_accel(ax, ay, az, &accel_roll, &accel_pitch, i);
        
        // Apply complementary filter fusion
        apply_complementary_filter(i, accel_roll, accel_pitch, 
                                  (float)gx, (float)gy, 
                                  &roll_deg[i], &pitch_deg[i]);
        
        successful_reads++;
    }
    
    // Always disable all channels when done (cleanup)
    TCA9548A_DisableAllChannels();
    
    // Return true if at least 3 fingers were read successfully
    // (graceful degradation - allows operation with some failed sensors)
    return (successful_reads >= 3);
}

/* Smart glove initialization with robust error checking */
u8 smart_glove_init(void) {
    // Step 1: Initialize TCA9548A multiplexer
    TCA9548A_Status_t mux_status = TCA9548A_InitializeSmartGloveSystem();
    if (mux_status != TCA9548A_OK) {
        // Log specific error
        if (mux_status == TCA9548A_ERROR_I2C) {
            // I2C bus error - hardware problem
            return 0;
        } else {
            // Other multiplexer initialization error
            return 0;
        }
    }
    
    // Wait for multiplexer to stabilize
    _delay_ms(100);
    
    // Step 2: Test multiplexer communication
    if (TCA9548A_TestCommunication() != TCA9548A_OK) {
        // Multiplexer responsive but communication test failed
        return 0;
    }
    
    // Step 3: Initialize all MPU6050 sensors
    MPU6050_voidInitAllSensors();
    
    // Allow sensors to fully initialize
    _delay_ms(200);
    
    // Step 4: Test sensor communication with comprehensive error reporting
    u8 working_sensors = TCA9548A_TestAllSensors();
    
    // Only accept if at least 3 sensors work (minimum for usable gesture detection)
    if (working_sensors >= 3) {
        // Reset all angle tracking variables for a clean start
        for (u8 i = 0; i < NUM_FINGERS; i++) {
            prev_roll[i] = 0.0f;
            prev_pitch[i] = 0.0f;
        }
        return 1; // Success: System ready with 3+ working sensors
    }

    return 0; // Too few sensors working
}
