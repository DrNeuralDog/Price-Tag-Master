@echo off
cd /d "%~dp0"
cd build
cmake --build . --config Debug
pause
