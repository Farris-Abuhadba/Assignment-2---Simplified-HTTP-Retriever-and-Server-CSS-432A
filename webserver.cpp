/*
    Farris Abu-Hadba
    CSS 432A - Network Design and Programming
    Assignment 2 - Simplified HTTP Retriever and Server
    Webserver Program (webserver.cpp)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

/**
 * Main function - HTTP Web Server
 * Listens on port 8080, handles GET requests, serves files or returns error codes.
 * 
 * @return 0 on success, 1 on error
 * 
 * Handles: 200 OK, 400 Bad Request, 403 Forbidden, 404 Not Found, 
 *          405 Method Not Allowed, 418 I'm a teapot
 */
int main() {
    int port = 8080;

    // Create TCP socket
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
        perror("socket");
        return 1;
    }

    // Allow quick restart
    int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Set up server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind socket
    if (bind(serverSd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    // Listen for connections
    listen(serverSd, 5);
    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        int clientSd = accept(serverSd, nullptr, nullptr);
        if (clientSd < 0) {
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytes = read(clientSd, buffer, BUFFER_SIZE - 1);
        if (bytes <= 0) {
            close(clientSd);
            continue;
        }

        buffer[bytes] = '\0';

        // Parse request line
        std::istringstream request(buffer);
        std::string method, path, version;
        request >> method >> path >> version;

        // Malformed request
        if (method.empty() || path.empty() || version.empty()) {
            std::string response =
                "HTTP/1.0 400 Bad Request\r\n"
                "Connection: close\r\n\r\n";
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // BREW method
        if (method == "BREW") {
            std::string response =
                "HTTP/1.0 418 I'm a teapot\r\n"
                "Connection: close\r\n\r\n";
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // Only GET is allowed
        if (method != "GET") {
            std::string response =
                "HTTP/1.0 405 Method Not Allowed\r\n"
                "Connection: close\r\n\r\n";
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // Forbidden file name
        if (path == "/MySecret.html") {
            std::string response =
                "HTTP/1.0 403 Forbidden\r\n"
                "Connection: close\r\n\r\n";
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // Directory traversal attempt
        if (path.find("..") != std::string::npos) {
            std::string response =
                "HTTP/1.0 403 Forbidden\r\n"
                "Connection: close\r\n\r\n";
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // Remove leading '/' from path to get filename
        std::string filename = path.substr(1);
        if (filename.empty()) {
            filename = "index.html";
        }

        // Try to open the file
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            // File does not exist - send 404
            std::string errorPage = 
                "<html><body>"
                "<h1>404 Not Found</h1>"
                "<p>The requested file was not found on this server.</p>"
                "</body></html>";
            
            std::string response =
                "HTTP/1.0 404 Not Found\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "\r\n" + errorPage;
            
            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // File exists - read the entire file
        std::string fileContent;
        char fileBuffer[BUFFER_SIZE];
        while (file.read(fileBuffer, BUFFER_SIZE)) {
            fileContent.append(fileBuffer, file.gcount());
        }
        fileContent.append(fileBuffer, file.gcount());
        file.close();

        // Send 200 OK with file content
        std::string response =
            "HTTP/1.0 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/html\r\n"
            "\r\n" + fileContent;
        
        write(clientSd, response.c_str(), response.length());
        close(clientSd);
    }

    return 0;
}