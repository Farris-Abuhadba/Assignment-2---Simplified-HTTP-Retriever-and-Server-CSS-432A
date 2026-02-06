/*
    Farris Abu-Hadba
    CSS 432A - Network Design and Programming
    Assignment 2 - Simplified HTTP Retriever and Server
    Retriever Program (retriever.cpp)
*/


#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/**
 * Main function - HTTP Retriever Client
 * Parses URL, connects to server, sends GET request, saves file or displays error.
 * 
 * @param argc Command line argument count (expects 2)
 * @param argv Command line arguments - argv[1] is URL (http://host:port/path)
 * @return 0 on success, 1 on error
 */
int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        std::cerr << "Example: " << argv[0] << " http://localhost:8080/index.html" << std::endl;
        return 1;
    }

    std::string url = argv[1];

    // Parse the URL to extract server address and file path
    std::string hostname;
    std::string filepath;
    int port = 80; // Default HTTP port

    // Remove "http://" if present
    size_t start = 0;
    if (url.find("http://") == 0) {
        start = 7;
    }

    // Find the first '/' after the hostname
    size_t pathStart = url.find('/', start);
    if (pathStart == std::string::npos) {
        hostname = url.substr(start);
        filepath = "/";
    } else {
        hostname = url.substr(start, pathStart - start);
        filepath = url.substr(pathStart);
    }

    // Check if port is specified in hostname
    size_t colonPos = hostname.find(':');
    if (colonPos != std::string::npos) {
        port = atoi(hostname.substr(colonPos + 1).c_str());
        hostname = hostname.substr(0, colonPos);
    }

    std::cout << "Connecting to server: " << hostname << ":" << port << std::endl;
    std::cout << "Requesting file: " << filepath << std::endl;

    // Look up the servers IP address using DNS
    struct hostent* host = gethostbyname(hostname.c_str());
    if (host == NULL) {
        std::cerr << "Error: Cannot resolve hostname " << hostname << std::endl;
        return 1;
    }

    // Set up the server address structure
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    serverAddr.sin_port = htons(port);

    // Create a TCP socket
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSd < 0) {
        perror("socket");
        return 1;
    }

    // Connect to the server
    if (connect(clientSd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        close(clientSd);
        return 1;
    }

    // Construct the HTTP GET request
    std::string request = "GET " + filepath + " HTTP/1.0\r\n";
    request += "Host: " + hostname + "\r\n";
    request += "\r\n";

    // Send the GET request to the server
    int bytesSent = write(clientSd, request.c_str(), request.length());
    if (bytesSent < 0) {
        perror("write");
        close(clientSd);
        return 1;
    }

    std::cout << "Request sent, waiting for response..." << std::endl;

    // Read the response from the server
    std::string response;
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = read(clientSd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    close(clientSd);

    if (response.empty()) {
        std::cerr << "Error: No response received from server" << std::endl;
        return 1;
    }

    // Parse the HTTP response to separate headers and body
    size_t headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        std::cerr << "Error: Malformed HTTP response" << std::endl;
        return 1;
    }

    std::string headers = response.substr(0, headerEnd);
    std::string body = response.substr(headerEnd + 4);

    // Extract the status code from the first line
    size_t firstLineEnd = headers.find("\r\n");
    std::string statusLine = headers.substr(0, firstLineEnd);
    
    // Parse status line
    size_t firstSpace = statusLine.find(' ');
    size_t secondSpace = statusLine.find(' ', firstSpace + 1);
    
    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        std::cerr << "Error: Cannot parse status line" << std::endl;
        return 1;
    }

    std::string statusCodeStr = statusLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    int statusCode = atoi(statusCodeStr.c_str());

    std::cout << "Status: " << statusLine << std::endl;

    // If status code is 200 OK, save the file
    if (statusCode == 200) {
        // Extract filename from filepath
        std::string filename;
        size_t lastSlash = filepath.find_last_of('/');
        if (lastSlash != std::string::npos && lastSlash + 1 < filepath.length()) {
            filename = filepath.substr(lastSlash + 1);
        } else {
            filename = "index.html";
        }

        // Save the body to a file
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error: Cannot create file " << filename << std::endl;
            return 1;
        }

        outFile.write(body.c_str(), body.length());
        outFile.close();

        std::cout << "File saved as: " << filename << std::endl;
    } else {
        // If error code, display the error page on screen
        std::cout << "Server returned error code " << statusCode << std::endl;
        std::cout << "Error page content:" << std::endl;
        std::cout << body << std::endl;
    }

    return 0;
}