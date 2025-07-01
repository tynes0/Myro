@echo off
setlocal

:: Go to the directory above (project root)
cd /d "%~dp0.."

echo [Myro Build] Generating Visual Studio 2022 project files...

:: Path to premake5.exe relative to root
set PREMAKE=vendor\premake\premake5.exe
set LOGFILE=build\build_log.txt

if not exist %PREMAKE% (
    echo [Error] %PREMAKE% not found.
    echo Please make sure premake5.exe exists in vendor\premake5.
    pause
    exit /b 1
)

:: Run premake and redirect output to log file
echo [Info] Running premake...
%PREMAKE% vs2022 > %LOGFILE% 2>&1

if %errorlevel% neq 0 (
    echo [Error] Premake execution failed.
    echo ---------- Build Log ----------
    type %LOGFILE%
    echo --------------------------------
    echo See full log in %LOGFILE%
    pause
    exit /b %errorlevel%
)

echo [Success] Project files generated successfully.
del %LOGFILE% >nul 2>&1
pause
endlocal
