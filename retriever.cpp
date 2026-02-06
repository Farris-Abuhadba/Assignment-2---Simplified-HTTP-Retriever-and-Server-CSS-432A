#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <url>\n";
        return 1;
    }

    std::string url = argv[1];
    std::string host;
    std::string path = "/";
    int port = 80;

    // Remove http:// if present
    if (url.find("http://") == 0) {
        url = url.substr(7);
    }

    // Split host and path
    size_t slash = url.find('/');
    if (slash != std::string::npos) {
        host = url.substr(0, slash);
        path = url.substr(slash);
    } else {
        host = url;
    }

    // DNS lookup
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        perror("gethostbyname");
        return 1;
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Build server address
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);

    // Connect
    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // Build HTTP GET request
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.0\r\n" << "Host: " << host << "\r\n" << "Connection: close\r\n\r\n";

    // Send request
    write(sockfd, request.str().c_str(), request.str().length());

    // Read and print response
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = read(sockfd, buffer, BUFFER_SIZE)) > 0) {
        std::cout.write(buffer, bytes);
    }

    close(sockfd);
    return 0;
}
