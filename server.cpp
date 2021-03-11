#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    // creating an endpoint and descriptor
    int listening = socket(AF_INET, SOCK_STREAM, 0);

    // socket description
    sockaddr_in server;
    // address family
    server.sin_family = AF_INET; // IPv4
    server.sin_port = htons(5400); // convert between host and network byte order
    // convert string into a network address (IPv4) and copies it to server.sin_addr
    inet_pton(AF_INET, "0.0.0.0", &server.sin_addr);
    // assigning address to the descriptor
    bind(listening, (sockaddr *) &server, sizeof(server));

    // listening for connection on a socket
    listen(listening, 1);

    // accept a connection on a socket
    int clientSocket = accept(listening, NULL, NULL);
    // close a file descriptor
    close(listening);

    char buf[256];
    // receive a message from a clientSocket
    int bytesRecv = recv(clientSocket, buf, 256, 0);
    std::cout << std::string(buf, 0, bytesRecv) << std::endl;
    close(clientSocket);
    return 0;
}