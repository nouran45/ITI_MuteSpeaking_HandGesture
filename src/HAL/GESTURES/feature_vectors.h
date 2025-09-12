#ifndef FEATURE_VECTORS_H
#define FEATURE_VECTORS_H

#include "STD_TYPES.h"      // For u8, f32, etc.
#include "gesture_extraction.h"       // For feature_vector_t, NUM_FINGERS

// Feature vector serialization constants
#define FEATURES_PER_FINGER_STATISTICAL 10  // mean, std, max, min, range for roll and pitch
#define FEATURES_PER_FINGER_TEMPORAL 4      // velocity and acceleration for roll and pitch
#define FEATURES_CORRELATION_MATRIX 25      // 5x5 correlation matrix (upper triangle)
#define TOTAL_FEATURE_COUNT (FEATURES_PER_FINGER_STATISTICAL * NUM_FINGERS + \
                           FEATURES_PER_FINGER_TEMPORAL * NUM_FINGERS + \
                           FEATURES_CORRELATION_MATRIX)

// Feature indices for serialization
typedef enum {
    // Statistical features (10 per finger = 50 total)
    FEATURE_MEAN_ROLL_START = 0,
    FEATURE_MEAN_PITCH_START = 5,
    FEATURE_STD_ROLL_START = 10,
    FEATURE_STD_PITCH_START = 15,
    FEATURE_MAX_ROLL_START = 20,
    FEATURE_MIN_ROLL_START = 25,
    FEATURE_MAX_PITCH_START = 30,
    FEATURE_MIN_PITCH_START = 35,
    FEATURE_RANGE_ROLL_START = 40,
    FEATURE_RANGE_PITCH_START = 45,

    // Temporal features (4 per finger = 20 total)
    FEATURE_VELOCITY_ROLL_START = 50,
    FEATURE_VELOCITY_PITCH_START = 55,
    FEATURE_ACCELERATION_ROLL_START = 60,
    FEATURE_ACCELERATION_PITCH_START = 65,

    // Correlation matrix (25 total)
    FEATURE_CORRELATION_START = 70
} feature_index_t;

// Gesture class definitions
typedef enum {
    GESTURE_A = 0, GESTURE_B, GESTURE_C, GESTURE_D, GESTURE_E,
    GESTURE_F, GESTURE_G, GESTURE_H, GESTURE_I, GESTURE_J,
    GESTURE_K, GESTURE_L, GESTURE_M, GESTURE_N, GESTURE_O,
    GESTURE_P, GESTURE_Q, GESTURE_R, GESTURE_S, GESTURE_T,
    GESTURE_U, GESTURE_V, GESTURE_W, GESTURE_X, GESTURE_Y,
    GESTURE_Z,
    GESTURE_UNKNOWN = 255
} gesture_class_t;

// Feature processing functions
void serialize_feature_vector(const feature_vector_t* features, f32* output_array, u8 array_size);
void deserialize_feature_vector(const f32* input_array, feature_vector_t* features, u8 array_size);
u8 validate_feature_vector(const feature_vector_t* features);
void print_feature_vector(const feature_vector_t* features);
gesture_class_t letter_to_gesture_class(char letter);
char gesture_class_to_letter(gesture_class_t gesture);

// Feature scaling functions
void scale_features_to_range(f32* features, u8 size, f32 min_val, f32 max_val);
void apply_feature_normalization(feature_vector_t* features);
f32 calculate_feature_magnitude(const feature_vector_t* features);

// Debug and utility functions
void log_feature_statistics(const feature_vector_t* features);
u8 compare_feature_vectors(const feature_vector_t* feat1, const feature_vector_t* feat2, f32 threshold);

#endif // FEATURE_VECTORS_H