#include "STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../LIB/math_utils.h"
#include "feature_vectors.h"
#include "../../MCAL/UART/UART_Interface.h"
#include <string.h>
#include <math.h>



void serialize_feature_vector(const feature_vector_t* features, f32* output_array, u8 array_size) {
    if (!features || !output_array || array_size < TOTAL_FEATURE_COUNT) {
        return;
    }
    u8 index = 0;
    // Statistical features
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        output_array[FEATURE_MEAN_ROLL_START + finger] = features->mean_roll[finger];
        output_array[FEATURE_MEAN_PITCH_START + finger] = features->mean_pitch[finger];
        output_array[FEATURE_STD_ROLL_START + finger] = features->std_roll[finger];
        output_array[FEATURE_STD_PITCH_START + finger] = features->std_pitch[finger];
        output_array[FEATURE_MAX_ROLL_START + finger] = features->max_roll[finger];
        output_array[FEATURE_MIN_ROLL_START + finger] = features->min_roll[finger];
        output_array[FEATURE_MAX_PITCH_START + finger] = features->max_pitch[finger];
        output_array[FEATURE_MIN_PITCH_START + finger] = features->min_pitch[finger];
        output_array[FEATURE_RANGE_ROLL_START + finger] = features->range_roll[finger];
        output_array[FEATURE_RANGE_PITCH_START + finger] = features->range_pitch[finger];
    }
    // Temporal features
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        output_array[FEATURE_VELOCITY_ROLL_START + finger] = features->velocity_roll[finger];
        output_array[FEATURE_VELOCITY_PITCH_START + finger] = features->velocity_pitch[finger];
        output_array[FEATURE_ACCELERATION_ROLL_START + finger] = features->acceleration_roll[finger];
        output_array[FEATURE_ACCELERATION_PITCH_START + finger] = features->acceleration_pitch[finger];
    }
    // Correlation matrix (upper triangle)
    index = FEATURE_CORRELATION_START;
    for (u8 i = 0; i < NUM_FINGERS; i++) {
        for (u8 j = i; j < NUM_FINGERS; j++) {
            if (index < array_size) {
                output_array[index++] = features->correlation_matrix[i][j];
            }
        }
    }
}

void deserialize_feature_vector(const f32* input_array, feature_vector_t* features, u8 array_size) {
    if (!input_array || !features || array_size < TOTAL_FEATURE_COUNT) {
        return;
    }
    u8 index = 0;
    // Statistical features
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        features->mean_roll[finger] = input_array[FEATURE_MEAN_ROLL_START + finger];
        features->mean_pitch[finger] = input_array[FEATURE_MEAN_PITCH_START + finger];
        features->std_roll[finger] = input_array[FEATURE_STD_ROLL_START + finger];
        features->std_pitch[finger] = input_array[FEATURE_STD_PITCH_START + finger];
        features->max_roll[finger] = input_array[FEATURE_MAX_ROLL_START + finger];
        features->min_roll[finger] = input_array[FEATURE_MIN_ROLL_START + finger];
        features->max_pitch[finger] = input_array[FEATURE_MAX_PITCH_START + finger];
        features->min_pitch[finger] = input_array[FEATURE_MIN_PITCH_START + finger];
        features->range_roll[finger] = input_array[FEATURE_RANGE_ROLL_START + finger];
        features->range_pitch[finger] = input_array[FEATURE_RANGE_PITCH_START + finger];
    }
    // Temporal features
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        features->velocity_roll[finger] = input_array[FEATURE_VELOCITY_ROLL_START + finger];
        features->velocity_pitch[finger] = input_array[FEATURE_VELOCITY_PITCH_START + finger];
        features->acceleration_roll[finger] = input_array[FEATURE_ACCELERATION_ROLL_START + finger];
        features->acceleration_pitch[finger] = input_array[FEATURE_ACCELERATION_PITCH_START + finger];
    }
    // Correlation matrix
    index = FEATURE_CORRELATION_START;
    for (u8 i = 0; i < NUM_FINGERS; i++) {
        for (u8 j = i; j < NUM_FINGERS; j++) {
            if (index < array_size) {
                features->correlation_matrix[i][j] = input_array[index];
                if (i != j) {
                    features->correlation_matrix[j][i] = input_array[index];
                }
                index++;
            }
        }
    }
}

u8 validate_feature_vector(const feature_vector_t* features) {
    if (!features) return 0;
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        if (isnan(features->mean_roll[finger]) || isinf(features->mean_roll[finger])) return 0;
        if (isnan(features->mean_pitch[finger]) || isinf(features->mean_pitch[finger])) return 0;
        if (isnan(features->std_roll[finger]) || isinf(features->std_roll[finger])) return 0;
        if (isnan(features->std_pitch[finger]) || isinf(features->std_pitch[finger])) return 0;
        if (features->mean_roll[finger] < 0.0f || features->mean_roll[finger] > 1.0f) return 0;
        if (features->mean_pitch[finger] < 0.0f || features->mean_pitch[finger] > 1.0f) return 0;
    }
    for (u8 i = 0; i < NUM_FINGERS; i++) {
        for (u8 j = 0; j < NUM_FINGERS; j++) {
            if (features->correlation_matrix[i][j] < -1.0f || features->correlation_matrix[i][j] > 1.0f) {
                return 0;
            }
        }
    }
    return 1;
}

void print_feature_vector(const feature_vector_t* features) {
    if (!features) return;
    UART_voidSendString("=== Feature Vector ===\r\n");
    const char* finger_names[5] = {"THUMB", "INDEX", "MIDDLE", "RING", "PALM"};
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        UART_voidSendString(finger_names[finger]);
        UART_voidSendString(":\r\n  Mean R/P: ");
        // Implement float-to-string conversion as needed
        // UART_voidSendFloat(features->mean_roll[finger]);
        UART_voidSendString(" / ");
        // UART_voidSendFloat(features->mean_pitch[finger]);
        UART_voidSendString("\r\n");
    }
}

gesture_class_t letter_to_gesture_class(char letter) {
    if (letter >= 'A' && letter <= 'Z') return (gesture_class_t)(letter - 'A');
    if (letter >= 'a' && letter <= 'z') return (gesture_class_t)(letter - 'a');
    return GESTURE_UNKNOWN;
}

char gesture_class_to_letter(gesture_class_t gesture) {
    if (gesture >= GESTURE_A && gesture <= GESTURE_Z) return 'A' + (char)gesture;
    return '?';
}

void scale_features_to_range(f32* features, u8 size, f32 min_val, f32 max_val) {
    if (!features || size == 0) return;
    f32 range = max_val - min_val;
    if (range == 0.0f) return;
    for (u8 i = 0; i < size; i++) {
        features[i] = ((features[i] - min_val) / range);
        if (features[i] < 0.0f) features[i] = 0.0f;
        if (features[i] > 1.0f) features[i] = 1.0f;
    }
}

void apply_feature_normalization(feature_vector_t* features) {
    if (!features) return;
    f32 temp_array[NUM_FINGERS];
    for (u8 i = 0; i < NUM_FINGERS; i++) temp_array[i] = features->mean_roll[i];
    f32 mean = math_calculate_mean(temp_array, NUM_FINGERS);
    f32 std = math_calculate_std(temp_array, NUM_FINGERS, mean);
    if (std > 0.0f) {
        for (u8 i = 0; i < NUM_FINGERS; i++) {
            features->mean_roll[i] = (features->mean_roll[i] - mean) / std;
        }
    }
    // Repeat for other feature groups as needed
}

f32 calculate_feature_magnitude(const feature_vector_t* features) {
    if (!features) return 0.0f;
    f32 magnitude = 0.0f;
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        magnitude += features->mean_roll[finger] * features->mean_roll[finger];
        magnitude += features->mean_pitch[finger] * features->mean_pitch[finger];
        magnitude += features->std_roll[finger] * features->std_roll[finger];
        magnitude += features->std_pitch[finger] * features->std_pitch[finger];
        magnitude += features->velocity_roll[finger] * features->velocity_roll[finger];
        magnitude += features->velocity_pitch[finger] * features->velocity_pitch[finger];
    }
    return sqrtf(magnitude);
}

void log_feature_statistics(const feature_vector_t* features) {
    if (!features) return;
    // f32 magnitude = calculate_feature_magnitude(features);  // Currently unused
    u8 is_valid = validate_feature_vector(features);
    UART_voidSendString("Feature Stats:\r\n  Valid: ");
    UART_voidSendByte(is_valid ? '1' : '0');  // Use UART_voidSendByte instead of UART_voidSendChar
    UART_voidSendString("\r\n  Magnitude: ");
    // UART_voidSendFloat(magnitude);
    UART_voidSendString("\r\n");
}

u8 compare_feature_vectors(const feature_vector_t* feat1, const feature_vector_t* feat2, f32 threshold) {
    if (!feat1 || !feat2) return 0;
    f32 diff_sum = 0.0f;
    u8 feature_count = 0;
    for (u8 finger = 0; finger < NUM_FINGERS; finger++) {
        diff_sum += fabsf(feat1->mean_roll[finger] - feat2->mean_roll[finger]);
        diff_sum += fabsf(feat1->mean_pitch[finger] - feat2->mean_pitch[finger]);
        diff_sum += fabsf(feat1->std_roll[finger] - feat2->std_roll[finger]);
        diff_sum += fabsf(feat1->std_pitch[finger] - feat2->std_pitch[finger]);
        feature_count += 4;
    }
    f32 average_diff = diff_sum / feature_count;
    return (average_diff < threshold) ? 1 : 0;
}

// --- Static helpers if not provided elsewhere ---
static f32 calculate_mean(const f32* data, u8 size) __attribute__((unused));
static f32 calculate_mean(const f32* data, u8 size) {
    if (!data || size == 0) return 0.0f;
    f32 sum = 0.0f;
    for (u8 i = 0; i < size; i++) sum += data[i];
    return sum / size;
}

static f32 calculate_std(const f32* data, u8 size, f32 mean) __attribute__((unused));
static f32 calculate_std(const f32* data, u8 size, f32 mean) {
    if (!data || size == 0) return 0.0f;
    f32 sum = 0.0f;
    for (u8 i = 0; i < size; i++) {
        f32 diff = data[i] - mean;
        sum += diff * diff;
    }
    return sqrtf(sum / size);
}