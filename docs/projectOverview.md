Smart Glove for Sign Language Recognition and Speech Output using MPU6050, ATmega32, and ESP32

Overview:
This project aims to design and implement a wearable smart glove that enables mute individuals to communicate through hand gesture recognition. The glove uses multiple MPU6050 IMU sensors mounted on the fingers to capture finger movements. An ATmega32 microcontroller acts as a sensor hub, collecting and preprocessing data (Roll and Pitch angles) from the sensors. The processed data is sent to an ESP32, which runs a lightweight machine learning model to classify gestures into letters, words, or phrases. Recognized gestures are then mapped to audio outputs using a sound module and displayed on an LCD/OLED screen for user feedback.

Core Features:

Gesture Recognition System

Hardware:

5× MPU6050 IMU sensors (one per finger).

TCA9548A I²C multiplexer for connecting multiple sensors.

ATmega32 microcontroller for sensor fusion and preprocessing.

Software:

Collect raw accelerometer and gyroscope data.

Apply complementary filter to extract Roll and Pitch.

Build a feature vector (10 values = 5 fingers × 2 angles).

Send feature vector to ESP32 via UART.

Machine Learning Classification

Offline:

Collect dataset of predefined sign language gestures.

Train model (Random Forest / MLP) in Python (Scikit-learn/TensorFlow).

Export lightweight version (TinyML/TFLite Micro or optimized C code).

On ESP32:

Load the trained model.

Perform inference in real-time.

Output recognized gesture as a label (word/letter).

Audio Output

Hardware: DFPlayer Mini (MP3 player module) + Speaker.

Software:

ESP32 sends commands to the DFPlayer via UART.

Play the audio file corresponding to the recognized gesture.

Implement volume control and error handling.

User Interface

Hardware: 16x2 LCD or OLED display + optional LEDs.

Software:

Display the recognized word or phrase.

Provide real-time feedback for correct/incorrect gestures.

Gesture Training Mode

Allow the user to add new gestures.

Calibration mode to capture and save new gesture feature vectors.

Store mappings (gesture → audio file) in EEPROM or SD card.

System Flow:

MPU6050 sensors capture finger motion.

ATmega32 reads IMUs (via TCA9548A), computes Roll & Pitch, and forms a feature vector.

Feature vector is transmitted to ESP32 over UART.

ESP32 runs ML inference → predicts gesture label.

ESP32:

Sends command to DFPlayer Mini to play corresponding audio.

Displays recognized gesture on LCD/OLED.

Applications:

Assistive technology for mute individuals.

Educational tool for learning sign language.

Gesture-based human-computer interaction.