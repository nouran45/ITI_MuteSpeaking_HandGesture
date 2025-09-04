#!/bin/bash
# Simple serial monitor for MPU6050 data
# Usage: ./serial_monitor.sh [port] [baudrate]

PORT=${1:-/dev/ttyUSB0}
BAUD=${2:-9600}

echo "Connecting to $PORT at $BAUD baud..."
echo "Press Ctrl+C to stop"
echo "----------------------------------------"

# Check if port exists
if [ ! -e "$PORT" ]; then
    echo "Error: Port $PORT not found!"
    echo "Available ports:"
    ls /dev/tty* | grep -E "(USB|ACM)" 2>/dev/null || echo "No USB serial ports found"
    exit 1
fi

# Use screen if available, otherwise stty+cat
if command -v screen >/dev/null 2>&1; then
    screen $PORT $BAUD
else
    # Alternative method using stty and cat
    stty -F $PORT $BAUD raw -echo
    cat $PORT
fi
