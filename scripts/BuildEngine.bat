@echo off
title Myro Engine Builder
echo ==========================================
echo          Myro - Auto Builder
echo ==========================================
echo.
cd /d "%~dp0.."

if not exist "build" (
    echo [INFO] Build directory not found! Generating first...
    mkdir build
    cd build
    cmake ..
) else (
    cd build
)

echo.
echo [INFO] Compiling Myro Engine (Debug mode)...
cmake --build . --config Debug

echo.
echo ==========================================
echo [SUCCESS] Build process finished!
echo Executable is located in build\bin\Debug\
echo ==========================================
pause