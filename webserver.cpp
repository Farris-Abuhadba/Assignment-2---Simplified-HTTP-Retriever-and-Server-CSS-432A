#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main()
{
    int port = 8080;

    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
        perror("socket");
        return 1;
    }

    int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    listen(serverSd, 5);

    std::cout << "Web server listening on port " << port << std::endl;

    while (true) {
        int client = accept(serverSd, nullptr, nullptr);
        if (client >= 0) {
            close(client);
        }
    }
}
