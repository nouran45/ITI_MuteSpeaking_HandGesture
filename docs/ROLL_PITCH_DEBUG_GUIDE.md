# 🔧 ROLL/PITCH DEBUGGING GUIDE

## 🎯 PROBLEM
Your roll values are larger than pitch values, which is unexpected for finger movement.

## 🛠️ SOLUTION STEPS

### **STEP 1: Test Current Implementation**
1. Flash the current firmware to your device
2. Place your hand flat with fingers straight
3. Observe the roll/pitch values via UART
4. Slowly bend one finger up/down (this should increase PITCH more than ROLL)

### **STEP 2: Enable Debug Mode (if needed)**
If you want detailed debugging:

1. **Edit `main_gesture_stream.c`**:
   ```c
   #define DEBUG_MODE 1  // Change from 0 to 1
   ```

2. **Rebuild and flash**:
   ```bash
   make clean && make all && make flash
   ```

3. **Connect UART** and observe detailed output showing:
   - Raw accelerometer values (ax, ay, az)
   - Current mapping results
   - Alternative mapping results

   **UART Connection Guide:**
   - **Baud Rate**: 9600 bps
   - **ATmega32 UART Pins**: 
     - **TX (Pin 14)** → Connect to your USB-Serial adapter RX
     - **RX (Pin 15)** → Connect to your USB-Serial adapter TX  
     - **GND** → Connect to your USB-Serial adapter GND
   - **Terminal Software**: Use PuTTY, Arduino Serial Monitor, or `screen /dev/ttyUSB0 9600`
   - **Settings**: 8 data bits, No parity, 1 stop bit

### **STEP 3: Fix Axis Mapping (if roll/pitch are swapped)**
If you find that the axis mapping is wrong:

1. **Edit `axis_mapping_fix.h`**:
   ```c
   #define SWAP_AXES 1  // Change from 0 to 1
   ```

2. **Rebuild and test**:
   ```bash
   make clean && make all && make flash
   ```

### **STEP 4: Verify Results**
Test with known finger positions:
- **Straight finger**: Both roll and pitch ≈ 0°
- **Bent finger (up/down)**: Pitch should change more than roll
- **Tilted finger (left/right)**: Roll should change more than pitch

## 📊 EXPECTED BEHAVIOR

### **Normal Finger Movement**:
```
Finger straight:     Roll ≈ 0°,    Pitch ≈ 0°
Finger bent up:      Roll ≈ 0°,    Pitch ≈ 30-90°
Finger tilted left:  Roll ≈ 30°,   Pitch ≈ 0°
```

### **If Values Are Swapped**:
```
Finger bent up:      Roll ≈ 30-90°, Pitch ≈ 0°    ← WRONG!
```

## 🔍 FILES INVOLVED

### **Key Files**:
- `axis_mapping_fix.h` - Main fix for axis swapping
- `main_gesture_stream.c` - Debug mode control
- `sensor_debug.c` - Detailed debugging functions
- `gesture_stream_platform.c` - Uses the axis fix

### **Quick Settings**:
- `DEBUG_MODE` in `main_gesture_stream.c` - Enable/disable debug
- `SWAP_AXES` in `axis_mapping_fix.h` - Fix axis mapping

## 🚀 TESTING COMMANDS

```bash
# Normal build
make clean && make all

# Enable debug mode and rebuild  
# (Edit DEBUG_MODE = 1 first)
make clean && make all

# Enable axis swapping and rebuild
# (Edit SWAP_AXES = 1 first)  
make clean && make all

# Flash to device
make flash
```

## 📝 UART OUTPUT EXAMPLES

### **Normal Operation**:
```
Frame sent
Waiting...
```

### **Debug Mode**:
```
=== SENSOR ORIENTATION DEBUG ===
--- FINGER 0 ---
Raw Accel: AX=1024 AY=512 AZ=15872
Current:   Pitch=3.7° Roll=1.8°
Alt Map:   Pitch=1.8° Roll=3.7°
```

## ✅ SUCCESS CRITERIA

After fixing, you should see:
- **Pitch > Roll** when bending fingers up/down
- **Roll > Pitch** when tilting fingers left/right
- **Both ≈ 0°** when fingers are straight
- **Stable, consistent readings** from the complementary filter

## 🎛️ FINAL RECOMMENDATION

1. **Try current version first** - it might work correctly
2. **Enable debug mode** if you need detailed analysis  
3. **Enable axis swapping** if roll/pitch are clearly swapped
4. **Test thoroughly** with different finger positions

The system now has robust error handling and should give you accurate angle measurements! 🎯
