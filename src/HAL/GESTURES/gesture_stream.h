
#ifndef GESTURE_STREAM_H
#define GESTURE_STREAM_H

/*
 * Smart Glove — Raw Gesture Streaming (no features, no ML)
 *
 * This module reads roll/pitch angles for all fingers and sends a compact
 * binary packet to the ESP32 over UART. It does not compute feature vectors
 * or run ML locally.
 */
#include "STD_TYPES.h"   // for f32, s16

#include <stdint.h>
#include <stdbool.h>

#ifndef NUM_FINGERS
#define NUM_FINGERS 5
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Packet markers */
#define GSTREAM_HDR 0xAAu
#define GSTREAM_TAIL 0x55u

/* Packet type/version */
#define GSTREAM_TYPE_RAW 0x01u
#define GSTREAM_VER      0x01u

typedef struct {
    f32 roll;
    f32 pitch;
    s16 ax_raw, ay_raw, az_raw;
    f32 ax_g, ay_g, az_g;
} FingerSensorData_t;

/* Public API */
void gesture_stream_init(void);

/* Smart glove system initialization */
u8 smart_glove_init(void);

/* Print sensor data via UART */
void print_sensor_data(void);

/* One-shot: read sensors -> build packet -> send to ESP32.
 * Returns true if a packet was sent.
 */
bool gesture_cycle_and_send(void);

/* Build a packet from roll/pitch arrays and timestamp.
 * out_len returns the number of bytes in out_buf.
 * out_buf must have at least gesture_stream_packet_maxlen() bytes capacity.
 */
bool gesture_build_packet(const float *roll_deg, const float *pitch_deg, uint32_t ts_ms,
                          uint8_t *out_buf, uint16_t *out_len);

/* Maximum possible packet length for current NUM_FINGERS. */
uint16_t gesture_stream_packet_maxlen(void);

/* ================= New Complementary Filter Functions ================= */

/* Read both accelerometer and gyroscope data from all sensors */
u8 read_all_sensors_accel_gyro_raw(s16 ax[5], s16 ay[5], s16 az[5],
                                   s16 gx[5], s16 gy[5], s16 gz[5]);

/* Calculate roll and pitch using complementary filter (accelerometer + gyroscope) */
void calculate_roll_pitch_complementary(s16 ax[5], s16 ay[5], s16 az[5],
                                        s16 gx[5], s16 gy[5], s16 gz[5],
                                        f32 roll[5], f32 pitch[5]);

/* Complete sensor reading and processing with complementary filter */
u8 read_and_process_all_sensors_cf(void);

/* Legacy functions for backward compatibility */
u8 read_and_process_all_sensors(void);
void calculate_roll_pitch_all(s16 ax[5], s16 ay[5], s16 az[5], f32 roll[5], f32 pitch[5]);

/* ================= Platform Hooks (provide real impls in your project) ================= */

/* Fill roll_deg[p], pitch_deg[p] for p in [0..NUM_FINGERS-1]. Return false on failure. */
bool read_all_sensors_angles(float *roll_deg, float *pitch_deg);

/* Millisecond timestamp. */
uint32_t gstream_get_timestamp_ms(void);

/* UART write: return number of bytes written (or <0 on error). */
int uart_write_bytes(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* GESTURE_STREAM_H */
