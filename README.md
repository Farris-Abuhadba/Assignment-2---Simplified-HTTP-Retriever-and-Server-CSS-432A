[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=22480411&assignment_repo_type=AssignmentRepo)
# Assignment 2 - Simplified HTTP Retriever and Server

You will write two programs that exercise a simplified version of HTTP.  The retriever will work in conjunction with any web server and the server will work in conjunction with any web browser.  This way, you can test your software independently of each other.  You may use HTTP 1.0 or 1.1.

1)  Your retriever takes in an input from the command line and parses the server address and file that is being requested.  The program then issues a GET request to the server for the requested file.  When the file is returned by the server, the retriever outputs the file to the file system.  If the server returns an error code instead of a OK code, then the retriever should not save the file and should display on the screen whatever error page was sent with the error.  Your retriever should exit after receiving all the responses.

2)  Your server waits for a connection and an HTTP GET request (you may do this single threaded or multi-threaded).  After receiving the GET request, the server opens the file that is requested and sends it (along with the HTTP 200 OK code, of course).  If the file requested does not exist, the server should return a 404 Not Found code along with a custom File Not Found page.  If the HTTP request is for MySecret.html then the web server should return a 403 Forbidden.  If the request is for a file that is above the directory structure where your web server is running ( for example, "GET ../../../etc/passwd" ), you should return a 403 Forbidden.  If the client issues a BREW method instead of GET return a 418 I'm a teapot response.  Since you only have to support GET requests, if the request is anything besides GET or BREW, return a 405 Method Not Allowed. Finally, if your server cannot understand the request, return a 400 Bad Request.  After you handle the request, your server should return to waiting for the next request. 

**for your demo script, you can create a list of "unauthorized files" and "forbidden files" and check that list rather than actually trying to determine if you don't have the permissions **

You will submit the code for both assignments with a build and demo script.  The demo script should run through all of the following test cases.  I have provided you a starter for the script, but you'll need to fill in your specific cases.  You do not need a test case for the real webbrowser. 

You will also submit screen shots of your programs executing

1) Real Web browser accessing your server -- 10 points

      (screen shot and  test case included in demo script)

2) Your retriever accessing a real server  -- 10 points  

3) Your retriever accessing a file from your server -- 10 points

4) Your retriever requesting sending an unallowed method to your server (includes both 405 and 418 errors) -- 10 points

5) Your retriever requesting a forbidden file from your server -- 10 points

6) Your retriever requesting a non-existent file from your server -- 10 points

7) Your retriever sending a malformed request to your server -- 10 points 

Make sure your screenshots are labeled.

Additional points  -- 

Basic Networking Code for client and server -- 30 points
      -Sockets, TCP connection, DNS lookup, etc.

----

Failure to include required files, poor comments, poor code structure/naming, memory leaks, and git etiquette will be penalized up to -10 points each depending on how bad it is.


