#ifndef GESTURE_STREAM_PLATFORM_H
#define GESTURE_STREAM_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include "STD_TYPES.h"
#include "gesture_stream.h"   /* for NUM_FINGERS and public API types */

/* Tick this from a 1 ms timer ISR (or pass delta in ms). */
void gesture_stream_tick_ms(uint16_t delta_ms);

/* Millisecond timestamp used in gesture packets. */
uint32_t gstream_get_timestamp_ms(void);

/* Low-level UART bridge used by the streamer (binary bytes). 
 * Must return number of bytes written, or <0 on error.
 */
int uart_write_bytes(const uint8_t *data, uint16_t len);

/* Fill roll_deg[i], pitch_deg[i] in degrees for i = 0..NUM_FINGERS-1.
 * Return false on failure (e.g., mux select or read error).
 */
bool read_all_sensors_angles(float *roll_deg, float *pitch_deg);

/* MEMORY OPTIMIZED: Minimal smart glove system initialization */
u8 smart_glove_init(void);

#endif /* GESTURE_STREAM_PLATFORM_H */
