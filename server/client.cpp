#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int failedToError(std::string message) {
    std::cerr << "Failed to " << message << std::endl;
    return 1;
}

int connectToServer(std::string serverIP, int port, int buffer_size, char *message) {
    std::cout << "connecting " << serverIP << ':' << port << std::endl;

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        return failedToError("create socket");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        return failedToError("connect, invalid address/address not supported address=" + serverIP);
    }

    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        return failedToError("connect to the server");
    }

    int messageStatus = send(clientSocket, message, strlen(message), 0);
    if (messageStatus < 0) {
        return failedToError("send data to the server");
    }

    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    int receiveStatus = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    if (receiveStatus < 0) {
        return failedToError("receive data from server");
    }

    std::cout << "Server response: \n" << buffer << std::endl;
    close(clientSocket);
    return 0;
}

int main() {
    std::string serverIP = "127.0.0.1";
    int port = 3333;

    while (true) {
        std::string line;
        std::getline(std::cin, line);

        char* message = line.data();

        connectToServer(serverIP, port, 4096, message);
    }
    // char *message = "STORE key value";

    // connectToServer(serverIP, port, 4096, message);
}