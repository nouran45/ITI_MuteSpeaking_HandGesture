# Smart Glove Gesture Recognition System Documentation

## Overview

This documentation covers the gesture recognition module for a smart
glove system that uses MPU6050 sensors and a TCA9548A I2C multiplexer to
capture and interpret hand gestures. The system processes orientation
data from multiple fingers to extract features suitable for machine
learning classification.

## Directory Structure

    gesture/
    ├── gesture_extraction.c
    ├── gesture_extraction.h
    ├── feature_vectors.c
    ├── feature_vectors.h
    ├── ml_interface.c
    └── ml_interface.h

## Module Overview

### 1. Gesture Extraction (gesture_extraction.c/h)

**Core Functions:**

**Data Collection & Processing** - `gesture_extraction_init()`:
Initialize gesture buffer and sensor system\
- `add_sensor_data()`: Add finger orientation data to buffer\
- `read_all_sensors_data()`: Read all finger sensors and populate
buffer\
- `calculate_roll_from_accel()`: Compute roll angle from accelerometer
data\
- `calculate_pitch_from_accel()`: Compute pitch angle from accelerometer
data

**Feature Extraction** - `extract_features()`: Main function to extract
all feature types\
- `calculate_statistical_features()`: Compute mean, std, min, max,
range\
- `calculate_temporal_features()`: Compute velocity and acceleration\
- `calculate_correlation_features()`: Compute finger movement
correlations\
- `normalize_features()`: Normalize features to standard range

**System Control** - `is_gesture_complete()`: Check if buffer contains a
complete gesture\
- `reset_gesture_buffer()`: Clear buffer for new gesture\
- `run_gesture_recognition_cycle()`: Main processing cycle\
- `print_sensor_diagnostics()`: Output sensor status via UART

**Utility Functions** - `calculate_mean()`, `calculate_std()`:
Statistical calculations\
- `calculate_velocity()`, `calculate_acceleration()`: Motion
calculations\
- `get_system_timestamp()`: Get current time reference

------------------------------------------------------------------------

### 2. Feature Vector Management (feature_vectors.c/h)

**Core Functions:**

**Serialization** - `serialize_feature_vector()`: Convert struct to flat
array\
- `deserialize_feature_vector()`: Convert array back to struct

**Validation & Processing** - `validate_feature_vector()`: Check for
valid data (NaN, Inf, range)\
- `scale_features_to_range()`: Scale features to \[0,1\] range\
- `apply_feature_normalization()`: Apply Z-score normalization\
- `calculate_feature_magnitude()`: Compute L2 norm of features

**Conversion & Comparison** - `letter_to_gesture_class()`: Convert ASCII
to gesture enum\
- `gesture_class_to_letter()`: Convert enum to ASCII\
- `compare_feature_vectors()`: Compare similarity of two vectors

**Debugging** - `print_feature_vector()`: Output feature details via
UART\
- `log_feature_statistics()`: Output feature statistics

------------------------------------------------------------------------

### 3. Machine Learning Interface (ml_interface.c/h)

**Core Functions:**

**Data Preparation** - `prepare_ml_packet()`: Package features for
transmission\
- `serialize_features()`: Convert features to transmission format

**Communication** - `send_to_esp32()`: Transmit data to ESP32\
- `receive_from_esp32()`: Receive ML response

**Validation & Processing** - `calculate_checksum()`: Generate data
integrity check\
- `validate_response()`: Verify response validity\
- `class_to_letter()`: Convert ML class ID to character

------------------------------------------------------------------------

## Data Structures

### Feature Vector (`feature_vector_t`)

Contains extracted features from gesture data: - Statistical features
(mean, std, min, max, range)\
- Temporal features (velocity, acceleration)\
- Correlation matrix between fingers

### Gesture Buffer (`gesture_buffer_t`)

Circular buffer storing historical sensor data: - Orientation data for
each finger\
- Timestamps for temporal analysis\
- Buffer management indices

### ML Packet (`ml_packet_t`)

Structured data package for ML transmission: - Serialized feature data\
- Checksum for integrity verification\
- Packet metadata

------------------------------------------------------------------------

## Usage Flow

### Initialization

``` c
gesture_extraction_init();
```

### Data Collection Cycle

``` c
while (1) {
    if (run_gesture_recognition_cycle(&features)) {
        // Gesture detected and features extracted
    }
}
```

### Feature Processing

``` c
if (extract_features(&features)) {
    serialize_feature_vector(&features, output_array, SIZE);
    prepare_ml_packet(&features, &packet);
}
```

### ML Communication

``` c
if (send_to_esp32(&packet)) {
    if (receive_from_esp32(&response)) {
        char gesture = class_to_letter(response.class_id);
    }
}
```

------------------------------------------------------------------------

## Configuration Constants

-   **NUM_FINGERS**: Number of fingers being tracked (default: 5)\
-   **FEATURE_WINDOW_SIZE**: Samples stored for analysis (default: 10)\
-   **GESTURE_CLASSES**: Number of recognizable gestures

------------------------------------------------------------------------

## Integration Points

### Hardware Dependencies

-   MPU6050 sensors for orientation data\
-   TCA9548A I2C multiplexer for sensor management\
-   UART interface for diagnostics and ML communication

### System Dependencies

-   Timer service for timestamp generation\
-   I2C driver for sensor communication\
-   UART driver for data transmission

------------------------------------------------------------------------

## Example Usage

``` c
// Initialize the gesture system
gesture_extraction_init();

// Main processing loop
feature_vector_t features;
while (1) {
    if (run_gesture_recognition_cycle(&features)) {
        // Process the detected gesture
        ml_packet_t packet;
        if (prepare_ml_packet(&features, &packet)) {
            send_to_esp32(&packet);
        }
    }
    
    // Add delay between cycles
    _delay_ms(50);
}
```

------------------------------------------------------------------------

## Error Handling

All functions return status codes indicating success or failure.
Critical functions include: - Data validation checks\
- Sensor communication error handling\
- Buffer overflow protection\
- Checksum verification for data integrity

------------------------------------------------------------------------

## Performance Considerations

-   Circular buffer design minimizes memory usage\
-   Feature normalization ensures consistent ML input\
-   Efficient serialization reduces transmission overhead\
-   Modular design allows for optimization of critical paths

------------------------------------------------------------------------

This gesture recognition system provides a complete pipeline from sensor
data collection to machine learning classification, enabling real-time
hand gesture recognition for smart glove applications.
