#!/bin/sh
# Hospital Triage Management System - Linux / macOS build script
# Usage: ./build.sh | ./build.sh run | ./build.sh test
set -e
mkdir -p bin
echo "[1/3] Building main application..."
g++ src/main.cpp           -I include -o bin/triage
echo "[2/3] Building Member 1 standalone module..."
g++ src/triage_module.cpp  -I include -o bin/triage_module
echo "[3/3] Building test suites..."
g++ tests/test_queue.cpp   -I include -o bin/test_queue
g++ tests/test_modules.cpp -I include -o bin/test_modules
echo "Build succeeded. Executables are in bin/"
case "$1" in
  run)  ./bin/triage ;;
  test) ./bin/test_queue; echo; ./bin/test_modules ;;
esac
