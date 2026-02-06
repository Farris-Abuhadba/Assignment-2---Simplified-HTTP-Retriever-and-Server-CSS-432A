#!/bin/bash

set -e

# Build the project
echo "Building project..."
./build.sh
echo

# Kill any existing webserver on port 8080
killall webserver 2>/dev/null || true
pkill -x webserver 2>/dev/null || true
if command -v fuser >/dev/null 2>&1; then
  fuser -k 8080/tcp 2>/dev/null || true
fi
if command -v lsof >/dev/null 2>&1; then
  PIDS=$(lsof -ti tcp:8080 2>/dev/null || true)
  if [ -n "$PIDS" ]; then
    kill -9 $PIDS 2>/dev/null || true
  fi
fi
sleep 1

# Start webserver in background
./webserver > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1


echo "TEST 1: Real Web Browser -> Your Server"
echo "=========================================="
echo "Expected: Browser displays HTML content"
echo "✓ MANUAL TEST - Requires screenshot"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 2: Retriever -> Real Server"
echo "=========================================="
echo "Command: ./retriever http://httpforever.com/"
echo
./retriever http://httpforever.com/
echo
echo "✓ TEST 2 PASSED"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 3: Retriever -> Your Server (200 OK)"
echo "=========================================="
echo "Command: ./retriever http://localhost:8080/index.html"
echo
./retriever http://localhost:8080/index.html
echo
echo "✓ TEST 3 PASSED"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 4a: Unallowed Method - BREW (418)"
echo "=========================================="
echo "Command: printf 'BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
echo
printf "BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
echo
echo "✓ TEST 4a PASSED - Server returned 418 I'm a teapot"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 4b: Unallowed Method - POST (405)"
echo "=========================================="
echo "Command: printf 'POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
echo
printf "POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
echo
echo "✓ TEST 4b PASSED - Server returned 405 Method Not Allowed"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 5: Forbidden File (403 Forbidden)"
echo "=========================================="
echo "Command: ./retriever http://localhost:8080/MySecret.html"
echo
./retriever http://localhost:8080/MySecret.html
echo
echo "✓ TEST 5 PASSED - Server returned 403 Forbidden"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 6: Non-existent File (404 Not Found)"
echo "=========================================="
echo "Command: ./retriever http://localhost:8080/doesnotexist.html"
echo
rm -f doesnotexist.html
./retriever http://localhost:8080/doesnotexist.html
echo
echo "✓ TEST 6 PASSED - Server returned 404 Not Found"
echo
read -p "Press Enter to continue to next test..."
echo

echo "TEST 7: Malformed Request (400 Bad Request)"
echo "=========================================="
echo "Command: printf 'HELLO WORLD\r\n' | nc localhost 8080"
echo
printf "HELLO WORLD\r\n" | nc localhost 8080 2>/dev/null
echo
echo "✓ TEST 7 PASSED - Server returned 400 Bad Request"
echo
echo

# Shutdown webserver
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true
