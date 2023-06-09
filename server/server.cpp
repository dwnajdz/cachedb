#include <iostream>  // For input/output operations
#include <cstring>
#include <sys/socket.h>  // For socket related functions
#include <netinet/in.h>  // For address structures
#include <unistd.h>  // For close function

// port address of server
#define PORT 3000
// buffer size of message
#define BUFFER_SIZE 1024
// number of maximum connections query if server is busy
#define BACKLOG 10

int failedError(std::string message) {
    std::cerr << "Failed to " << message << std::endl;
    return 1;
}

int hostServer(int port, int buffer_size, int backlog) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        return failedError("create socket");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int bindResult = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1) {
        return failedError("bind socket");
    }

    int listenResult = listen(serverSocket, backlog);
    if (listenResult == -1) {
        close(serverSocket);
        return failedError("listen for connections");
    }

    std::cout << "server started successfuly\n";
    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            failedError("accept connection");
            continue;
        }

        char buffer[buffer_size];
        memset(buffer, 0, buffer_size);

        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            close(clientSocket);
            failedError("receive data from client");
            continue;
        }

        std::cout << "Received data from client: " << buffer << std::endl;

        const char* response = "accepted";
        int bytesSent = send(clientSocket, response, sizeof(response), 0);
        if (bytesSent == -1) {
            close(clientSocket);
            failedError("send data to client");
            continue;
        }
        
        close(clientSocket);
    }
}

int main(int argc, char const* argv[])
{
    hostServer(PORT, BUFFER_SIZE, BACKLOG);
}
