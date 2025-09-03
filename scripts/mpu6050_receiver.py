#!/usr/bin/env python3
"""
MPU6050 Live Data Receiver
Receives live sensor data from ATmega32a via UART

Data format: AX,AY,AZ,GX,GY,GZ,Motion_Status
Baud rate: 9600
"""

import serial
import time
import csv
from datetime import datetime

# Configuration
SERIAL_PORT = '/dev/ttyUSB0'  # Change this to your serial port (COM port on Windows)
BAUD_RATE = 9600
CSV_FILENAME = f'mpu6050_data_{datetime.now().strftime("%Y%m%d_%H%M%S")}.csv'

def main():
    try:
        # Open serial connection
        print(f"Connecting to {SERIAL_PORT} at {BAUD_RATE} baud...")
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Give time for connection to establish
        
        print("Connected! Receiving live data...")
        print("Format: AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, Motion")
        print("Press Ctrl+C to stop\n")
        
        # Create CSV file
        with open(CSV_FILENAME, 'w', newline='') as csvfile:
            csv_writer = csv.writer(csvfile)
            csv_writer.writerow(['Timestamp', 'AccelX', 'AccelY', 'AccelZ', 
                               'GyroX', 'GyroY', 'GyroZ', 'Motion'])
            
            while True:
                line = ser.readline().decode('utf-8').strip()
                
                if line:
                    # Display live data
                    timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    print(f"[{timestamp}] {line}")
                    
                    # Parse and save to CSV
                    try:
                        data = line.split(',')
                        if len(data) >= 7:
                            csv_writer.writerow([timestamp] + data)
                            csvfile.flush()  # Ensure data is written immediately
                    except:
                        pass  # Skip malformed lines
                        
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        print("Make sure your USB-to-serial adapter is connected and the port is correct.")
        print("Common ports:")
        print("  Linux: /dev/ttyUSB0, /dev/ttyACM0")
        print("  Windows: COM1, COM2, COM3, etc.")
        print("  macOS: /dev/cu.usbserial-*")
        
    except KeyboardInterrupt:
        print(f"\nStopping... Data saved to {CSV_FILENAME}")
        
    except Exception as e:
        print(f"Error: {e}")
        
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()
