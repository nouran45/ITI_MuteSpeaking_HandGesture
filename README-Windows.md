# Windows Setup Guide for ATmega32 Smart Glove Project

## 🖥️ **Windows Development Setup**

### **Option 1: Arduino IDE (Easiest)**
1. **Download Arduino IDE**: https://www.arduino.cc/en/software
2. **Install Arduino IDE** with default settings
3. **Add AVR tools to PATH**:
   - Open System Properties → Advanced → Environment Variables
   - Add to PATH: `C:\Program Files (x86)\Arduino\hardware\tools\avr\bin`
4. **Verify installation**: Open Command Prompt and run:
   ```cmd
   avr-gcc --version
   ```

### **Option 2: WinAVR (Alternative)**
1. **Download WinAVR**: http://winavr.sourceforge.net/
2. **Install WinAVR** (adds tools to PATH automatically)
3. **Verify installation**: Open Command Prompt and run:
   ```cmd
   avr-gcc --version
   ```

### **Option 3: MSYS2 (Advanced)**
1. **Download MSYS2**: https://www.msys2.org/
2. **Install MSYS2** and run:
   ```bash
   pacman -S mingw-w64-x86_64-avr-gcc
   pacman -S mingw-w64-x86_64-avr-binutils  
   pacman -S mingw-w64-x86_64-avrdude
   ```

## 🔧 **Build Instructions**

### **Using Windows Makefile:**
```cmd
# Clone the project
git clone <repository-url>
cd IMU_testing_LCD

# Build the firmware
make -f Makefile.windows all

# Clean build files
make -f Makefile.windows clean

# Flash to microcontroller
make -f Makefile.windows flash
```

### **Using Cross-Platform Makefile:**
```cmd
# This works on both Windows and Linux
make -f Makefile.cross-platform all
make -f Makefile.cross-platform flash
```

## ⚡ **Quick Start (Windows)**
1. **Install Arduino IDE** (easiest option)
2. **Open Command Prompt** as Administrator
3. **Navigate to project folder:**
   ```cmd
   cd C:\path\to\your\project
   ```
4. **Build:**
   ```cmd
   make -f Makefile.windows all
   ```
5. **Flash (with USB programmer connected):**
   ```cmd  
   make -f Makefile.windows flash
   ```

## 🔌 **Programmer Configuration**

### **For USBasp (Default):**
```makefile
PROGRAMMER = usbasp
```

### **For Arduino as ISP:**
```makefile
PROGRAMMER = arduino
PORT = COM3
BAUD = 57600
```

### **For other programmers, edit these lines in Makefile.windows:**
```makefile
PROGRAMMER = your_programmer_type
PORT = COM_PORT  # e.g., COM1, COM2, etc.
BAUD = baud_rate # if needed
```

## 🛠️ **Troubleshooting**

### **"avr-gcc not found"**
- Make sure Arduino IDE is installed
- Add AVR tools to Windows PATH:
  `C:\Program Files (x86)\Arduino\hardware\tools\avr\bin`
- Restart Command Prompt

### **"make not found"**
- Install Git for Windows (includes make): https://git-scm.com/
- Use Git Bash instead of Command Prompt
- Or install Make for Windows: http://gnuwin32.sourceforge.net/packages/make.htm

### **"Permission denied" when flashing**
- Run Command Prompt as Administrator
- Check if programmer is connected properly
- Try different USB port

### **"Device not found"**
- Check programmer connection
- Verify correct COM port in Makefile
- Install programmer drivers if needed

## 📁 **Project Structure**
```
IMU_testing_LCD/
├── Makefile.windows      ← Use this for Windows
├── Makefile.cross-platform ← Alternative
├── include/              ← All header files
├── src/                  ← Source code
│   ├── APP/main.c
│   ├── HAL/              ← Hardware drivers
│   └── MCAL/             ← Microcontroller drivers
└── tests/                ← Test programs
```

## 🎯 **Available Make Targets**
```cmd
make -f Makefile.windows help    # Show all available commands
make -f Makefile.windows all     # Build firmware
make -f Makefile.windows clean   # Remove build files  
make -f Makefile.windows flash   # Upload to MCU
make -f Makefile.windows size    # Show memory usage
```

## 💡 **Tips for Windows Users**
1. **Use Git Bash** instead of Command Prompt for better compatibility
2. **Run as Administrator** when flashing firmware
3. **Check Device Manager** for COM port numbers
4. **Install programmer drivers** before first use
5. **Use full paths** if tools are not in PATH

## 🔍 **Testing Your Setup**
```cmd
# Test if tools are installed
avr-gcc --version
avr-objcopy --version  
avr-size --version
avrdude -?

# Test build process
make -f Makefile.windows test-build
```

## 📞 **Need Help?**
- Check if Arduino IDE is properly installed
- Verify PATH environment variable
- Try using Git Bash instead of Command Prompt
- Run commands as Administrator
- Make sure USB programmer is connected and recognized
