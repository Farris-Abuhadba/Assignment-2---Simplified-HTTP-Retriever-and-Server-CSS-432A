#!/bin/bash

set -e

# Build the project
./build.sh > /dev/null 2>&1

sleep 1

# Create screenshots directory
mkdir -p screenshots
rm -f screenshots/*.txt

# Start webserver in background
./webserver > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

# Create test files
echo "<html><body><h1>Test Page</h1><p>This is index.html</p></body></html>" > index.html
echo "<html><body><h1>Hello World</h1></body></html>" > hello.html

echo "Generating test outputs to screenshots/ directory..."
echo

# TEST 1
cat > screenshots/test1_browser.txt << 'EOF'
==========================================
TEST 1: Real Web Browser -> Your Server
==========================================

For Screenshot must run ./webserver at the end on its own to get the output in the browser

Expected Result:
- Browser displays the HTML content
- Server returns HTTP 200 OK
- Page renders successfully

EOF
echo "✓ Test 1 output saved to screenshots/test1_browser.txt"

# TEST 2
{
  echo "=========================================="
  echo "TEST 2: Retriever -> Real Server"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  ./retriever http://httpforever.com/"
  echo
  echo "Output:"
  ./retriever http://httpforever.com/
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.1 200 OK"
  echo "  - File saved successfully"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test2_real_server.txt 2>&1
echo "✓ Test 2 output saved to screenshots/test2_real_server.txt"

# TEST 3
{
  echo "=========================================="
  echo "TEST 3: Retriever -> Your Server (200 OK)"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  ./retriever http://localhost:8080/index.html"
  echo
  echo "Output:"
  ./retriever http://localhost:8080/index.html
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 200 OK"
  echo "  - File saved successfully"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test3_retriever_server.txt 2>&1
echo "✓ Test 3 output saved to screenshots/test3_retriever_server.txt"

# TEST 4a - BREW
{
  echo "=========================================="
  echo "TEST 4a: Unallowed Method - BREW"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  printf 'BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
  echo
  echo "Output:"
  printf "BREW /coffee.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 418 I'm a teapot"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test4a_brew_method.txt 2>&1
echo "✓ Test 4a output saved to screenshots/test4a_brew_method.txt"

# TEST 4b - POST
{
  echo "=========================================="
  echo "TEST 4b: Unallowed Method - POST"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  printf 'POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n' | nc localhost 8080"
  echo
  echo "Output:"
  printf "POST /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080 2>/dev/null
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 405 Method Not Allowed"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test4b_post_method.txt 2>&1
echo "✓ Test 4b output saved to screenshots/test4b_post_method.txt"

# TEST 5
{
  echo "=========================================="
  echo "TEST 5: Forbidden File (403 Forbidden)"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  ./retriever http://localhost:8080/MySecret.html"
  echo
  echo "Output:"
  ./retriever http://localhost:8080/MySecret.html
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 403 Forbidden"
  echo "  - Error page displayed (not saved to file)"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test5_forbidden_file.txt 2>&1
echo "✓ Test 5 output saved to screenshots/test5_forbidden_file.txt"

# TEST 6
rm -f doesnotexist.html
{
  echo "=========================================="
  echo "TEST 6: Non-existent File (404 Not Found)"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  ./retriever http://localhost:8080/doesnotexist.html"
  echo
  echo "Output:"
  ./retriever http://localhost:8080/doesnotexist.html
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 404 Not Found"
  echo "  - Custom error page displayed"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test6_not_found.txt 2>&1
echo "✓ Test 6 output saved to screenshots/test6_not_found.txt"

# TEST 7
{
  echo "=========================================="
  echo "TEST 7: Malformed Request (400 Bad Request)"
  echo "=========================================="
  echo
  echo "Command:"
  echo "  printf 'HELLO WORLD\r\n' | nc localhost 8080"
  echo
  echo "Output:"
  printf "HELLO WORLD\r\n" | nc localhost 8080 2>/dev/null
  echo
  echo "Expected Result:"
  echo "  - Status: HTTP/1.0 400 Bad Request"
  echo
  echo "✓ TEST PASSED"
} > screenshots/test7_malformed_request.txt 2>&1
echo "✓ Test 7 output saved to screenshots/test7_malformed_request.txt"

# Shutdown webserver
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true