#include <iostream>
#include <fstream>
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

    // Strip http:// if present
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

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    std::string request =
        "GET " + path + " HTTP/1.0\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n";

    write(sockfd, request.c_str(), request.length());

    // Read headers first
    std::string headers;
    char buffer[BUFFER_SIZE];
    int bytes;

    while (headers.find("\r\n\r\n") == std::string::npos) {
        bytes = read(sockfd, buffer, BUFFER_SIZE);
        if (bytes <= 0) {
            std::cerr << "Failed to read HTTP headers\n";
            close(sockfd);
            return 1;
        }
        headers.append(buffer, bytes);
    }

    size_t header_end = headers.find("\r\n\r\n");
    std::string header_part = headers.substr(0, header_end);
    std::string body = headers.substr(header_end + 4);

    // Parse status line
    std::istringstream header_stream(header_part);
    std::string status_line;
    getline(header_stream, status_line);

    // Parse Content-Length
    std::string line;
    int content_length = -1;
    while (getline(header_stream, line)) {
        if (line.rfind("Content-Length:", 0) == 0) {
            content_length = atoi(line.substr(15).c_str());
            break;
        }
    }

    if (content_length < 0) {
        std::cerr << "No Content-Length found\n";
        close(sockfd);
        return 1;
    }

    // Read rest of body
    while ((int)body.size() < content_length) {
        bytes = read(sockfd, buffer, BUFFER_SIZE);
        if (bytes <= 0) break;
        body.append(buffer, bytes);
    }

    close(sockfd);

    // If not 200 OK, print error body
    if (status_line.find("200") == std::string::npos) {
        std::cout << body;
        return 0;
    }

    // Determine output filename
    std::string filename = path;
    if (filename == "/") filename = "index.html";
    else if (filename[0] == '/') filename = filename.substr(1);

    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(body.c_str(), content_length);
    outfile.close();

    std::cout << "Saved file: " << filename << std::endl;
    return 0;
}
