@echo off
title Myro Project Generator
echo ==========================================
echo       Myro - Visual Studio Generator
echo ==========================================
echo.

cd ..

if not exist "build" (
    echo [INFO] Creating build directory...
    mkdir build
)

cd build
echo [INFO] Generating CMake cache and project files...
cmake ..

echo.
echo ==========================================
echo [SUCCESS] Visual Studio files are ready!
echo You can now open build\Myro.sln
echo ==========================================
pause