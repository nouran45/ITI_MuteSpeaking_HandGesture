# 🔌 UART CONNECTION GUIDE

## 🎯 **UART SETUP FOR ATmega32**

Your ATmega32 is configured for **9600 baud rate** UART communication.

## 🔧 **Hardware Connections**

### **ATmega32 UART Pins**:
```
ATmega32 Pin Layout:
┌─────────────────────────┐
│  1 │ PB0   VCC │ 40     │
│  2 │ PB1   PA0 │ 39     │
│  3 │ PB2   PA1 │ 38     │
│  ...              ...   │
│ 14 │ TXD   RXD │ 15     │  ← UART PINS
│ 16 │ GND   VCC │ 31     │
└─────────────────────────┘
```

### **Pin Connections**:
- **Pin 14 (TXD)** → USB-Serial adapter **RX**
- **Pin 15 (RXD)** → USB-Serial adapter **TX**  
- **Pin 11 or 31 (VCC)** → USB-Serial adapter **5V** (if needed)
- **Pin 16 or 30 (GND)** → USB-Serial adapter **GND**

## 📱 **USB-Serial Adapter Options**

### **Common Adapters**:
1. **FTDI FT232RL** (recommended)
2. **CH340G** (cheap option)
3. **CP2102** (reliable)
4. **Arduino as USB-Serial** (if available)

### **Wiring Example (FTDI)**:
```
ATmega32    →    FTDI Adapter
────────         ─────────────
Pin 14 (TX) →    RX
Pin 15 (RX) →    TX
Pin 16 (GND) →   GND
```

## 💻 **Terminal Software Setup**

### **Option 1: PuTTY (Windows)**
1. Download PuTTY
2. Select "Serial" connection
3. **Serial port**: COM3, COM4, etc.
4. **Speed**: 9600
5. Click "Open"

### **Option 2: Arduino IDE Serial Monitor**
1. Open Arduino IDE
2. Tools → Serial Monitor
3. **Baud rate**: 9600
4. **Port**: Select your USB-Serial port

### **Option 3: Linux Terminal**
```bash
# Find your device
ls /dev/ttyUSB*    # Usually /dev/ttyUSB0

# Connect with screen
screen /dev/ttyUSB0 9600

# Or with minicom
minicom -D /dev/ttyUSB0 -b 9600

# Exit screen: Ctrl+A, then K, then Y
```

### **Option 4: Windows Command Line**
```cmd
# Using built-in terminal
mode COM3 BAUD=9600 PARITY=n DATA=8 STOP=1
copy con COM3
```

## ⚙️ **UART Settings**
- **Baud Rate**: 9600 bps
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None

## 📊 **Expected Output**

### **Normal Mode (DEBUG_MODE = 0)**:
```
Frame sent
Waiting...
Frame sent  
Waiting...
```

### **Debug Mode (DEBUG_MODE = 1)**:
```
=== SENSOR ORIENTATION DEBUG ===
--- FINGER 0 ---
Raw Accel: AX=1024 AY=512 AZ=15872
Current:   Pitch=3.7° Roll=1.8°
Alt Map:   Pitch=1.8° Roll=3.7°
--- FINGER 1 ---
Raw Accel: AX=2048 AY=256 AZ=15360
Current:   Pitch=7.4° Roll=0.9°
Alt Map:   Pitch=0.9° Roll=7.4°
...
```

## 🔍 **Troubleshooting**

### **No Output**:
- Check wiring (TX/RX might be swapped)
- Verify baud rate (9600)
- Check power connections
- Ensure ATmega32 is running

### **Garbled Text**:
- Wrong baud rate
- Check terminal settings
- Verify ground connection

### **Connection Issues**:
- Check USB driver for adapter
- Try different USB port
- Verify device manager (Windows)

## ✅ **Quick Test**

1. **Connect hardware** as described above
2. **Open terminal** at 9600 baud
3. **Power on ATmega32**
4. **You should see**: "Frame sent" messages
5. **For debug**: Set `DEBUG_MODE = 1` and rebuild

## 🚀 **Ready to Debug!**

Once connected, you can:
- Monitor roll/pitch values in real-time
- See raw accelerometer data
- Compare current vs. alternative axis mapping
- Determine if you need to enable `SWAP_AXES`

Your UART is now ready for debugging! 🎯
