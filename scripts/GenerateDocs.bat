@echo off
title Myro - Generate Documentation
echo ==========================================
echo       Myro - Doxygen Generator
echo ==========================================
echo.

cd /d "%~dp0.."

where doxygen >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] Doxygen is not installed or not in PATH!
    echo Please install Doxygen from https://www.doxygen.nl/download.html
    echo.
    pause
    exit /b 1
)

echo [INFO] Generating API documentation...
doxygen Doxyfile

echo.
echo ==========================================
echo [SUCCESS] Documentation generated successfully!
echo You can open: docs\api\html\index.html
echo ==========================================
pause