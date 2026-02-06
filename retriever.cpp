#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <url>\n";
        return 1;
    }

    std::string url = argv[1];

    if (url.find("http://") == 0) {
        url = url.substr(7);
    }

    std::string host = url;
    size_t slash = url.find('/');
    if (slash != std::string::npos) {
        host = url.substr(0, slash);
    }

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
    serv_addr.sin_port = htons(80);
    memcpy(&serv_addr.sin_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    std::cout << "Connected to " << host << std::endl;
    close(sockfd);
    return 0;
}
