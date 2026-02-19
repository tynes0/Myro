#!/bin/bash

echo "=========================================="
echo "          Myro - Auto Builder             "
echo "=========================================="
echo ""

cd "$(dirname "$0")/.."

if [ ! -d "build" ]; then
    echo "[INFO] Build directory not found! Generating first..."
    mkdir build
    cd build
    cmake ..
else
    cd build
fi

echo ""
echo "[INFO] Compiling Myro Engine (Debug mode)..."
cmake --build . --config Debug 

echo ""
echo "=========================================="
echo "[SUCCESS] Build process finished!"
echo "Executable is located in build/bin/ or build/bin/Debug/"
echo "=========================================="