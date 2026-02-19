#!/bin/bash

echo "=========================================="
echo "       Myro - Doxygen Generator           "
echo "=========================================="
echo ""

cd "$(dirname "$0")/.."

if ! command -v doxygen &> /dev/null
then
    echo "[ERROR] Doxygen could not be found!"
    echo "Please install it (e.g., 'sudo apt install doxygen' or 'brew install doxygen')"
    exit 1
fi

echo "[INFO] Generating API documentation..."
doxygen Doxyfile

echo ""
echo "=========================================="
echo "[SUCCESS] Documentation generated successfully!"
echo "You can open: docs/api/html/index.html"
echo "=========================================="