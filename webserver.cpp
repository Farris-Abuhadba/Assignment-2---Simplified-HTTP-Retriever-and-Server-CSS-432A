#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

// Send a simple HTTP response with no body
void sendSimpleResponse(int client, const std::string& status)
{
    std::ostringstream resp;
    resp << "HTTP/1.0 " << status << "\r\n" << "Connection: close\r\n\r\n";

    write(client, resp.str().c_str(), resp.str().length());
}

int main()
{
    int port = 8080;

    // Create socket
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
        perror("socket");
        return 1;
    }

    // Allow quick restart
    int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Bind
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // Listen
    listen(serverSd, 5);
    std::cout << "Server listening on port " << port << std::endl;

    // Main accept loop
    while (true) {
        int client = accept(serverSd, nullptr, nullptr);
        if (client < 0) {
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytes = read(client, buffer, BUFFER_SIZE - 1);
        if (bytes <= 0) {
            close(client);
            continue;
        }

        buffer[bytes] = '\0';

        // Parse request line
        std::istringstream request(buffer);
        std::string method, path, version;
        request >> method >> path >> version;

        // If we cannot parse the request line → 400 Bad Request
        if (method.empty() || path.empty() || version.empty()) {
            sendSimpleResponse(client, "400 Bad Request");
            close(client);
            continue;
        }

        // For Phase 4, we stop here — just acknowledge request
        // (Phase 5+ will add real logic)
        sendSimpleResponse(client, "200 OK");

        close(client);
    }

    return 0;
}
