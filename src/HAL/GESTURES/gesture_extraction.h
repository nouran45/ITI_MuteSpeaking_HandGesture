#ifndef GESTURE_EXTRACTION_H
#define GESTURE_EXTRACTION_H

#include <stdint.h>
#include <stdbool.h>

#include "../../LIB/STD_TYPES.h"

// Configuration constants
#define NUM_FINGERS 5
#define NUM_AXIS 2  // Roll and Pitch
#define FEATURE_WINDOW_SIZE 10
#define MAX_FEATURES 20

// Finger indices
typedef enum {
    THUMB = 0,
    INDEX_FINGER = 1,
    MIDDLE_FINGER = 2,
    RING_FINGER = 3,
    PINKY = 4
} finger_t;

// Raw sensor data structure
typedef struct {
    float roll;
    float pitch;
    uint32_t timestamp;
} imu_data_t;

// Feature vector structure
typedef struct {
    float mean_roll[NUM_FINGERS];
    float mean_pitch[NUM_FINGERS];
    float std_roll[NUM_FINGERS];
    float std_pitch[NUM_FINGERS];
    float max_roll[NUM_FINGERS];
    float min_roll[NUM_FINGERS];
    float max_pitch[NUM_FINGERS];
    float min_pitch[NUM_FINGERS];
    float range_roll[NUM_FINGERS];
    float range_pitch[NUM_FINGERS];
    float velocity_roll[NUM_FINGERS];
    float velocity_pitch[NUM_FINGERS];
    float acceleration_roll[NUM_FINGERS];
    float acceleration_pitch[NUM_FINGERS];
    float correlation_matrix[NUM_FINGERS][NUM_FINGERS];
} feature_vector_t;

// Gesture data buffer
typedef struct {
    imu_data_t finger_data[NUM_FINGERS][FEATURE_WINDOW_SIZE];
    uint8_t buffer_index;
    bool buffer_full;
    uint32_t last_update_time;
} gesture_buffer_t;

// Function prototypes
u8 is_gesture_complete(void);
u8 extract_features(feature_vector_t* features);
void add_sensor_data(finger_t finger, f32 roll, f32 pitch);
bool read_all_sensors_data(void);
f32 calculate_roll_from_accel(s16 ax, s16 ay, s16 az);
f32 calculate_pitch_from_accel(s16 ax, s16 ay, s16 az);
void calculate_statistical_features(feature_vector_t* features);
void calculate_temporal_features(feature_vector_t* features);
void calculate_correlation_features(feature_vector_t* features);
void normalize_features(feature_vector_t* features);
void reset_gesture_buffer(void);
bool run_gesture_recognition_cycle(feature_vector_t* features);
void print_sensor_diagnostics(void);
u32 get_system_timestamp(void);


#endif // GESTURE_EXTRACTION_H