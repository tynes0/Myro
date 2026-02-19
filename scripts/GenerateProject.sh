#!/bin/bash

echo "=========================================="
echo "      Myro - Project Generator            "
echo "=========================================="
echo ""

cd "$(dirname "$0")/.."

if [ ! -d "build" ]; then
    echo "[INFO] Creating build directory..."
    mkdir build
fi

cd build
echo "[INFO] Generating CMake cache and project files..."
cmake ..

echo ""
echo "=========================================="
echo "[SUCCESS] Build files are ready!"
echo "You can now run 'cd build && make' or use your preferred IDE."
echo "=========================================="