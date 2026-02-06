#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

int main()
{
    int port = 8080;

    // Create TCP socket
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
        perror("socket");
        return 1;
    }

    // Allow the server to restart quickly
    int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Set up server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind socket to port
    if (bind(serverSd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    // Start listening for connections
    listen(serverSd, 5);
    std::cout << "Server listening on port " << port << std::endl;

    // Main server loop
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

        // Parse the request line: METHOD PATH VERSION
        std::istringstream request(buffer);
        std::string method, path, version;
        request >> method >> path >> version;

        // If request line is malformed, return 400
        if (method.empty() || path.empty() || version.empty()) {
            std::string response =
                "HTTP/1.0 400 Bad Request\r\n"
                "Connection: close\r\n\r\n";

            write(clientSd, response.c_str(), response.length());
            close(clientSd);
            continue;
        }

        // For Phase 4, we only confirm the request was understood
        std::string response =
            "HTTP/1.0 200 OK\r\n"
            "Connection: close\r\n\r\n";

        write(clientSd, response.c_str(), response.length());
        close(clientSd);
    }

    return 0;
}
