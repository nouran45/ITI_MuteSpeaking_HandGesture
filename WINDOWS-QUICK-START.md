# 🪟 **Windows User Quick Setup Guide**

## 📦 **Files for Windows Users:**
- `Makefile.windows` - Windows-specific Makefile
- `Makefile.cross-platform` - Works on both Windows & Linux  
- `build.bat` - Double-click to build (Windows GUI)
- `README-Windows.md` - Detailed setup instructions

## ⚡ **Super Quick Setup (5 minutes):**

### **1. Install Arduino IDE**
- Download: https://www.arduino.cc/en/software
- Install with default settings
- **Done!** (This gives you all the AVR tools)

### **2. Install Git for Windows** 
- Download: https://git-scm.com/download/win
- Install with default settings  
- **Important**: This gives you the `make` command

### **3. Clone & Build**
Open **Git Bash** (not Command Prompt) and run:
```bash
git clone <your-repo-url>
cd IMU_testing_LCD

# Build firmware
make -f Makefile.windows all

# Flash to microcontroller (USB programmer connected)
make -f Makefile.windows flash
```

## 🖱️ **Super Easy GUI Method:**
1. **Double-click `build.bat`** in Windows Explorer
2. **Choose option 1** to build
3. **Choose option 3** to build and flash
4. **Done!** 

## 🔧 **Available Commands:**
```bash
# Using Makefile
make -f Makefile.windows all      # Build firmware
make -f Makefile.windows flash    # Upload to MCU
make -f Makefile.windows clean    # Clean build files
make -f Makefile.windows help     # Show help

# Using GUI
build.bat                         # Double-click for menu
```

## ⚙️ **Programmer Settings:**
Edit `Makefile.windows` to change programmer:

**For USBasp (default):**
```makefile
PROGRAMMER = usbasp
```

**For Arduino as ISP:**
```makefile
PROGRAMMER = arduino
PORT = COM3
BAUD = 57600
```

## 🐛 **Common Issues & Solutions:**

| Problem | Solution |
|---------|----------|
| "make not found" | Install Git for Windows |
| "avr-gcc not found" | Install Arduino IDE, add to PATH |
| "Permission denied" | Run Git Bash as Administrator |
| "Device not found" | Check USB programmer connection |

## 🎯 **Project Memory Usage:**
```
Program: 4962 bytes (15.1% of 32KB)
Data:    679 bytes (33.2% of 2KB)
✅ Plenty of space remaining!
```

## 📞 **Need Help?**
1. **Check Arduino IDE installation**
2. **Use Git Bash (not Command Prompt)**  
3. **Run as Administrator when flashing**
4. **Read `README-Windows.md` for details**

---
**🚀 Happy coding! The project is ready to build on Windows! 🚀**
