#include "gesture_extraction.h"
#include "math_utils.h"
#include "STD_TYPES.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "TCA9548A_Interface.h"
#include "TCA9548A_Integration.h"
#include "MPU6050_Interface.h"
#include "../HAL/MPU6050/mpu6050_types.h"
#include "UART_Interface.h"

// Global gesture buffer
static gesture_buffer_t g_gesture_buffer;

// Previous velocity values for acceleration calculation
static f32 prev_velocity_roll[NUM_FINGERS];
static f32 prev_velocity_pitch[NUM_FINGERS];

// System initialization flag
static bool system_initialized = false;

void gesture_extraction_init(void) {
    // Clear buffers and state
    memset(&g_gesture_buffer, 0, sizeof(gesture_buffer_t));
    memset(prev_velocity_roll, 0, sizeof(prev_velocity_roll));
    memset(prev_velocity_pitch, 0, sizeof(prev_velocity_pitch));
    g_gesture_buffer.buffer_index = 0;
    g_gesture_buffer.buffer_full = false;
    system_initialized = false;
    
    // Initialize the complete smart glove system
    TCA9548A_Status_t status = TCA9548A_InitializeSmartGloveSystem();
    if (status == TCA9548A_OK) {
        system_initialized = true;
        
        // Test all sensors
        u8 sensors_found = TCA9548A_TestAllSensors();
        
        // Send diagnostic information via UART
        UART_voidSendString("Gesture system initialized. Sensors found: ");
        UART_voidSendNumber(sensors_found);
        UART_voidSendString("/5\r\n");
        
        if (sensors_found < 4) {  // At least 4 sensors should be working
            UART_voidSendString("WARNING: Not all sensors detected!\r\n");
        }
    } else {
        UART_voidSendString("ERROR: Failed to initialize gesture system!\r\n");
    }
}

void add_sensor_data(finger_t finger, f32 roll, f32 pitch) {
    if (finger >= NUM_FINGERS || !system_initialized) return;
    
    u8 idx = g_gesture_buffer.buffer_index;
    
    // Get current timestamp (assuming you have a system timer function)
    u32 current_time = get_system_timestamp(); // You'll need to implement this
    
    // Add new data
    g_gesture_buffer.finger_data[finger][idx].roll = roll;
    g_gesture_buffer.finger_data[finger][idx].pitch = pitch;
    g_gesture_buffer.finger_data[finger][idx].timestamp = current_time;
    
    // Update buffer state only once per complete reading cycle
    if (finger == PINKY) {  // Last finger in sequence
        g_gesture_buffer.buffer_index = (idx + 1) % FEATURE_WINDOW_SIZE;
        if (!g_gesture_buffer.buffer_full && g_gesture_buffer.buffer_index == 0) {
            g_gesture_buffer.buffer_full = true;
        }
        g_gesture_buffer.last_update_time = current_time;
    }
}

bool read_all_sensors_data(void) {
    if (!system_initialized) return false;
    
    // Read data from all finger sensors
    for (finger_t finger = 0; finger < NUM_FINGERS; finger++) {
        mpu6050_raw_data_t raw_data;
        
        // Read sensor data using MPU6050 multi-sensor interface
        if (MPU6050_readSensorData(finger, &raw_data) == MPU6050_OK) {
            // Convert raw accelerometer data to roll/pitch angles
            f32 roll = calculate_roll_from_accel(raw_data.ax, raw_data.ay, raw_data.az);
            f32 pitch = calculate_pitch_from_accel(raw_data.ax, raw_data.ay, raw_data.az);
            
            // Add to gesture buffer
            add_sensor_data(finger, roll, pitch);
        } else {
            // Handle sensor read error
            UART_voidSendString("Sensor ");
            UART_voidSendNumber(finger);
            UART_voidSendString(" read error\r\n");
            return false;
        }
    }
    
    return true;
}

f32 calculate_roll_from_accel(s16 ax, s16 ay, s16 az) {
    // Convert to g's (assuming ±2g range, 16-bit resolution)
    (void)ax;  // Suppress unused parameter warning - ax not needed for roll calculation
    f32 ay_g = (f32)ay / 16384.0f;
    f32 az_g = (f32)az / 16384.0f;
    
    // Calculate roll angle in degrees
    f32 roll = atan2f(ay_g, az_g) * 180.0f / M_PI;
    return roll;
}

f32 calculate_pitch_from_accel(s16 ax, s16 ay, s16 az) {
    // Convert to g's
    f32 ax_g = (f32)ax / 16384.0f;
    f32 ay_g = (f32)ay / 16384.0f;
    f32 az_g = (f32)az / 16384.0f;
    
    // Calculate pitch angle in degrees
    f32 pitch = atan2f(-ax_g, sqrtf(ay_g * ay_g + az_g * az_g)) * 180.0f / M_PI;
    return pitch;
}

u8 extract_features(feature_vector_t* features) {
    if (!g_gesture_buffer.buffer_full || features == NULL || !system_initialized) {
        return false;
    }
    
    // Clear feature vector
    memset(features, 0, sizeof(feature_vector_t));
    
    // Calculate different types of features
    calculate_statistical_features(features);
    calculate_temporal_features(features);
    calculate_correlation_features(features);
    
    // Normalize features
    normalize_features(features);
    
    return true;
}



void calculate_statistical_features(feature_vector_t* features) {
    for (finger_t finger = 0; finger < NUM_FINGERS; finger++) {
        f32 roll_data[FEATURE_WINDOW_SIZE];
        f32 pitch_data[FEATURE_WINDOW_SIZE];

        for (u8 i = 0; i < FEATURE_WINDOW_SIZE; i++) {
            roll_data[i] = g_gesture_buffer.finger_data[finger][i].roll;
            pitch_data[i] = g_gesture_buffer.finger_data[finger][i].pitch;
        }

        features->mean_roll[finger] = math_calculate_mean(roll_data, FEATURE_WINDOW_SIZE);
        features->mean_pitch[finger] = math_calculate_mean(pitch_data, FEATURE_WINDOW_SIZE);

        features->std_roll[finger] = math_calculate_std(roll_data, FEATURE_WINDOW_SIZE, features->mean_roll[finger]);
        features->std_pitch[finger] = math_calculate_std(pitch_data, FEATURE_WINDOW_SIZE, features->mean_pitch[finger]);

        features->min_roll[finger] = math_find_min(roll_data, FEATURE_WINDOW_SIZE);
        features->max_roll[finger] = math_find_max(roll_data, FEATURE_WINDOW_SIZE);
        features->min_pitch[finger] = math_find_min(pitch_data, FEATURE_WINDOW_SIZE);
        features->max_pitch[finger] = math_find_max(pitch_data, FEATURE_WINDOW_SIZE);

        features->range_roll[finger] = math_calculate_range(roll_data, FEATURE_WINDOW_SIZE);
        features->range_pitch[finger] = math_calculate_range(pitch_data, FEATURE_WINDOW_SIZE);
    }
}

void calculate_temporal_features(feature_vector_t* features) {
    for (finger_t finger = 0; finger < NUM_FINGERS; finger++) {
        f32 velocities_roll[FEATURE_WINDOW_SIZE - 1];
        f32 velocities_pitch[FEATURE_WINDOW_SIZE - 1];
        f32 accelerations_roll[FEATURE_WINDOW_SIZE - 2];
        f32 accelerations_pitch[FEATURE_WINDOW_SIZE - 2];

        for (u8 i = 1; i < FEATURE_WINDOW_SIZE; i++) {
            u32 dt = g_gesture_buffer.finger_data[finger][i].timestamp -
                     g_gesture_buffer.finger_data[finger][i-1].timestamp;
            if (dt == 0) dt = 1;

            velocities_roll[i-1] = math_calculate_velocity(
                g_gesture_buffer.finger_data[finger][i].roll,
                g_gesture_buffer.finger_data[finger][i-1].roll,
                dt
            );

            velocities_pitch[i-1] = math_calculate_velocity(
                g_gesture_buffer.finger_data[finger][i].pitch,
                g_gesture_buffer.finger_data[finger][i-1].pitch,
                dt
            );
        }

        for (u8 i = 1; i < FEATURE_WINDOW_SIZE - 1; i++) {
            u32 dt = g_gesture_buffer.finger_data[finger][i+1].timestamp -
                     g_gesture_buffer.finger_data[finger][i].timestamp;
            if (dt == 0) dt = 1;

            accelerations_roll[i-1] = math_calculate_acceleration(
                velocities_roll[i], velocities_roll[i-1], dt
            );

            accelerations_pitch[i-1] = math_calculate_acceleration(
                velocities_pitch[i], velocities_pitch[i-1], dt
            );
        }

        features->velocity_roll[finger] = math_calculate_mean(velocities_roll, FEATURE_WINDOW_SIZE - 1);
        features->velocity_pitch[finger] = math_calculate_mean(velocities_pitch, FEATURE_WINDOW_SIZE - 1);
        features->acceleration_roll[finger] = math_calculate_mean(accelerations_roll, FEATURE_WINDOW_SIZE - 2);
        features->acceleration_pitch[finger] = math_calculate_mean(accelerations_pitch, FEATURE_WINDOW_SIZE - 2);
    }
}

void calculate_correlation_features(feature_vector_t* features) {
    // Calculate cross-correlation between fingers
    for (finger_t finger1 = 0; finger1 < NUM_FINGERS; finger1++) {
        for (finger_t finger2 = 0; finger2 < NUM_FINGERS; finger2++) {
            if (finger1 == finger2) {
                features->correlation_matrix[finger1][finger2] = 1.0f;
                continue;
            }
            
            f32 sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
            
            for (u8 i = 0; i < FEATURE_WINDOW_SIZE; i++) {
                f32 x = g_gesture_buffer.finger_data[finger1][i].roll;
                f32 y = g_gesture_buffer.finger_data[finger2][i].roll;
                
                sum_x += x;
                sum_y += y;
                sum_xy += x * y;
                sum_x2 += x * x;
                sum_y2 += y * y;
            }
            
            f32 n = FEATURE_WINDOW_SIZE;
            f32 numerator = n * sum_xy - sum_x * sum_y;
            f32 denominator = sqrtf((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
            
            if (denominator != 0) {
                features->correlation_matrix[finger1][finger2] = numerator / denominator;
            } else {
                features->correlation_matrix[finger1][finger2] = 0.0f;
            }
        }
    }
}

void normalize_features(feature_vector_t* features) {
    // Simple min-max normalization for angle values (typically -180 to 180)
    const f32 angle_range = 360.0f;
    const f32 angle_offset = 180.0f;
    
    for (finger_t finger = 0; finger < NUM_FINGERS; finger++) {
        // Normalize angle features to [0, 1]
        features->mean_roll[finger] = (features->mean_roll[finger] + angle_offset) / angle_range;
        features->mean_pitch[finger] = (features->mean_pitch[finger] + angle_offset) / angle_range;
        
        // Normalize range features
        features->range_roll[finger] = features->range_roll[finger] / angle_range;
        features->range_pitch[finger] = features->range_pitch[finger] / angle_range;
        
        // Normalize std features
        features->std_roll[finger] = features->std_roll[finger] / (angle_range / 4); // Assume max std is range/4
        features->std_pitch[finger] = features->std_pitch[finger] / (angle_range / 4);
    }
}

u8 is_gesture_complete(void) {
    // Check if buffer is full and recent data is available
    if (!g_gesture_buffer.buffer_full || !system_initialized) return false;
    
    // Check if all fingers have recent data (within last 100ms)
    u32 current_time = get_system_timestamp(); // You'll need to implement this
    return (current_time - g_gesture_buffer.last_update_time) < 100;
}

void reset_gesture_buffer(void) {
    g_gesture_buffer.buffer_index = 0;
    g_gesture_buffer.buffer_full = false;
    g_gesture_buffer.last_update_time = 0;
    memset(prev_velocity_roll, 0, sizeof(prev_velocity_roll));
    memset(prev_velocity_pitch, 0, sizeof(prev_velocity_pitch));
}

bool run_gesture_recognition_cycle(feature_vector_t* features) {
    if (!system_initialized || !features) {
        return false;
    }
    
    // Read all sensor data
    if (!read_all_sensors_data()) {
        UART_voidSendString("ERROR: Failed to read sensor data\r\n");
        return false;
    }
    
    // Check if we have enough data for feature extraction
    if (!is_gesture_complete()) {
        return false; // Not enough data yet
    }
    
    // Extract features
    if (!extract_features(features)) {
        UART_voidSendString("ERROR: Feature extraction failed\r\n");
        return false;
    }
    
    return true;
}

void print_sensor_diagnostics(void) {
    if (!system_initialized) {
        UART_voidSendString("System not initialized\r\n");
        return;
    }
    
    UART_voidSendString("=== Sensor Diagnostics ===\r\n");
    
    // Test each sensor individually
    for (finger_t finger = 0; finger < NUM_FINGERS; finger++) {
        mpu6050_raw_data_t raw_data;
        
        if (MPU6050_readSensorData(finger, &raw_data) == MPU6050_OK) {
            UART_voidSendString("Finger ");
            UART_voidSendNumber(finger);
            UART_voidSendString(": AX=");
            UART_voidSendNumber(raw_data.ax);
            UART_voidSendString(", AY=");
            UART_voidSendNumber(raw_data.ay);
            UART_voidSendString(", AZ=");
            UART_voidSendNumber(raw_data.az);
            UART_voidSendString(", GX=");
            UART_voidSendNumber(raw_data.gx);
            UART_voidSendString(", GY=");
            UART_voidSendNumber(raw_data.gy);
            UART_voidSendString(", GZ=");
            UART_voidSendNumber(raw_data.gz);
            UART_voidSendString("\r\n");
        } else {
            UART_voidSendString("Finger ");
            UART_voidSendNumber(finger);
            UART_voidSendString(": SENSOR ERROR\r\n");
        }
    }
    
    // Print buffer status
    UART_voidSendString("Buffer: Index=");
    UART_voidSendNumber(g_gesture_buffer.buffer_index);
    UART_voidSendString(", Full=");
    UART_voidSendString(g_gesture_buffer.buffer_full ? "YES" : "NO");
    UART_voidSendString("\r\n");
    
    UART_voidSendString("========================\r\n");
}






// You'll need to implement this function based on your timer system
u32 get_system_timestamp(void) {
    // This should return current system time in milliseconds
    // Implementation depends on your timer/RTC setup
    static u32 dummy_time = 0;
    dummy_time += 10; // Simulate 10ms increments
    return dummy_time;
}