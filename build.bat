@echo off
REM ============================================================
REM  Hospital Triage Management System - Windows build script
REM  Usage:  build.bat          compile everything
REM          build.bat run      compile then start the program
REM          build.bat test     compile then run the test suites
REM ============================================================

if not exist bin mkdir bin

echo [1/3] Building main application...
g++ src\main.cpp -I include -o bin\triage.exe
if errorlevel 1 goto failed

echo [2/3] Building Member 1 standalone module...
g++ src\triage_module.cpp -I include -o bin\triage_module.exe
if errorlevel 1 goto failed

echo [3/3] Building test suites...
g++ tests\test_queue.cpp   -I include -o bin\test_queue.exe
if errorlevel 1 goto failed
g++ tests\test_modules.cpp -I include -o bin\test_modules.exe
if errorlevel 1 goto failed

echo.
echo Build succeeded. Executables are in bin\
echo.

if "%1"=="run"  bin\triage.exe
if "%1"=="test" (
  bin\test_queue.exe
  echo.
  bin\test_modules.exe
)
goto :eof

:failed
echo.
echo BUILD FAILED - fix the first error reported above.
exit /b 1
