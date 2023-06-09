#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int failedToError(std::string message) {
    std::cerr << "Failed to " << message << std::endl;
    return 1;
}

int newSocket(std::string serverIP, int port) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        return failedToError("create socket");
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    
    int addressStatus = inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr));
    if (addressStatus <= 0) {
        return failedToError("invalid address/address not supported address=" + serverIP);
    }

    int socketConnectStatus = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));  
    if (socketConnectStatus < 0) {
        return failedToError("connect to the server");
    }

    std::cout << "successfuly connected to the server\n";
    return clientSocket;
}

// sends message to the server and returns server response
char* sendSocketMessage(int clientSocket, char *message, int buffer_size=512) {
    if (clientSocket == -1) {
        failedToError("send message, invalid client socket");
        return NULL;
    }

    int messageStatus = send(clientSocket, message, strlen(message), 0);
    if (messageStatus < 0) {
        failedToError("send message");
        return NULL;
    }

    // process response from server
    char buffer[buffer_size];

    // clear buffer
    memset(buffer, 0, buffer_size);

    std::cout << "SIZEOF: " << sizeof(buffer) << '\n';
    std::cout << "BUFFER_SIZE: " << buffer_size << '\n';
    int receiveStatus = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    if (receiveStatus < 0) {
        failedToError("receive response from server");
    }

    //close(clientSocket);

    //char response[sizeof(buffer)-1];
    //memcpy(response, buffer, sizeof(buffer));

    return buffer;
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
    std::string serverIP = "0.0.0.0";
    int port = 3000;

    int my_socket = newSocket(serverIP, port);
    for (int i=0; i<10; i++) {
        std::string line;
        std::getline(std::cin, line);

        char* message = line.data();

        //char* responseFromServer;
        sendSocketMessage(my_socket, message);
        //std::cout << "response: " << responseFromServer << std::endl;
    }
    close(my_socket);
}
