@echo off
REM ===== Windows Build Script for ATmega32 Smart Glove =====
REM Double-click this file to build the project easily

echo.
echo ===== ATmega32 Smart Glove Build Script =====
echo.

REM Check if make is available
where make >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: 'make' command not found!
    echo.
    echo Please install one of the following:
    echo   1. Git for Windows ^(includes make^)
    echo   2. MinGW/MSYS2
    echo   3. Use Git Bash instead of Command Prompt
    echo.
    pause
    exit /b 1
)

REM Check if avr-gcc is available
where avr-gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: 'avr-gcc' not found!
    echo.
    echo Please install Arduino IDE or WinAVR and add to PATH:
    echo   C:\Program Files ^(x86^)\Arduino\hardware\tools\avr\bin
    echo.
    pause
    exit /b 1
)

echo Tools check: OK
echo.

:menu
echo Choose an option:
echo   1. Build firmware
echo   2. Clean build files
echo   3. Build and flash
echo   4. Show memory usage
echo   5. Exit
echo.
set /p choice=Enter your choice (1-5): 

if "%choice%"=="1" goto build
if "%choice%"=="2" goto clean  
if "%choice%"=="3" goto flash
if "%choice%"=="4" goto size
if "%choice%"=="5" goto exit
echo Invalid choice. Please try again.
goto menu

:build
echo.
echo Building firmware...
make -f Makefile.windows all
if %errorlevel% neq 0 (
    echo.
    echo Build FAILED!
    pause
    goto menu
)
echo.
echo Build SUCCESS!
pause
goto menu

:clean
echo.
echo Cleaning build files...
make -f Makefile.windows clean
echo Clean complete.
pause
goto menu

:flash
echo.
echo Building and flashing firmware...
make -f Makefile.windows all
if %errorlevel% neq 0 (
    echo Build FAILED!
    pause
    goto menu
)

echo.
echo Make sure your programmer is connected...
pause

make -f Makefile.windows flash
if %errorlevel% neq 0 (
    echo.
    echo Flash FAILED! Check programmer connection.
    pause
    goto menu
)
echo.
echo Flash SUCCESS!
pause
goto menu

:size
echo.
echo Checking memory usage...
make -f Makefile.windows size
pause
goto menu

:exit
echo.
echo Goodbye!
exit /b 0
