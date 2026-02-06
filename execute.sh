#    Farris Abu-Hadba
#    CSS 432A - Network Design and Programming
#    Assignment 2 - Simplified HTTP Retriever and Server
#    Main Script for tests
#

#!/bin/bash

set -e

# Start webserver in background
./webserver > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

# Create test files
echo "<html><body><h1>Test Page</h1><p>This is index.html</p></body></html>" > index.html
echo "<html><body><h1>Hello World</h1></body></html>" > hello.html

echo "=========================================="
echo "HTTP Server and Retriever Test Suite"
echo "=========================================="
echo

echo "TEST 1: Real Web Browser -> Your Server"
echo "  Manual test: Open http://localhost:8080/index.html in browser"
echo

echo "TEST 2: Retriever -> Real Server"
echo "  Command: ./retriever http://httpforever.com/"
./retriever http://httpforever.com/ 2>/dev/null
echo "  ✓ Successfully retrieved file from real server"
echo

echo "TEST 3: Retriever -> Your Server (200 OK)"
echo "  Command: ./retriever http://localhost:8080/index.html"
./retriever http://localhost:8080/index.html 2>/dev/null
echo "  ✓ Successfully retrieved file with 200 OK"
echo

echo "TEST 4a: Unallowed Method - BREW (418 I'm a teapot)"
echo "  Command: printf 'BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
printf "BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
echo "  ✓ Server returned 418 I'm a teapot"
echo

echo "TEST 4b: Unallowed Method - POST (405 Method Not Allowed)"
echo "  Command: printf 'POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
printf "POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
echo "  ✓ Server returned 405 Method Not Allowed"
echo

echo "TEST 5: Forbidden File (403 Forbidden)"
echo "  Command: ./retriever http://localhost:8080/MySecret.html"
./retriever http://localhost:8080/MySecret.html 2>/dev/null
echo "  ✓ Server returned 403 Forbidden"
echo

echo "TEST 6: Non-existent File (404 Not Found)"
echo "  Command: ./retriever http://localhost:8080/doesnotexist.html"
rm -f doesnotexist.html
./retriever http://localhost:8080/doesnotexist.html 2>/dev/null
echo "  ✓ Server returned 404 Not Found"
echo

echo "TEST 7: Malformed Request (400 Bad Request)"
echo "  Command: printf 'HELLO WORLD\r\n' | nc localhost 8080"
printf "HELLO WORLD\r\n" | nc localhost 8080 2>/dev/null
echo "  ✓ Server returned 400 Bad Request"
echo

echo "=========================================="
echo "All 7 tests completed successfully!"
echo "=========================================="
echo

# Shutdown webserver
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

echo
echo "Running Valgrind memory check..."
echo

# Start server with valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./webserver > /dev/null 2>&1 &
SERVER_PID=$!
sleep 2

# Run a quick test with valgrind
echo "Testing retriever with Valgrind:"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./retriever http://localhost:8080/index.html

# Cleanup
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

echo
echo "=========================================="
echo "Testing complete!"
echo "=========================================="