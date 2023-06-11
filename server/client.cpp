#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>

// port address of server
#define PORT 3000
// default buffer size of message
#define BUFFER_SIZE 1024
// number of maximum connections query if server is busy
#define BACKLOG 10

int failedToError(std::string message) {
  std::cerr << "Failed to " << message << std::endl;
  return 1;
}

void receive_response(int socket, int buffer_size = BUFFER_SIZE) {
  char buffer[buffer_size] = {0};

  while (true) {
    // receive message from server
    int recv_len = recv(socket, buffer, buffer_size, 0);
    if (recv_len == -1) {
      failedToError("receive message from server");
      break;
    }

    std::cout << "response: " << buffer << '\n';

    // clear buffer
    memset(buffer, 0, sizeof(buffer));
  }
}

int main() {
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    failedToError("create socket");
    return 1;
  }

  // connect to server
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  // connect to localhost address
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  // connect to defined port
  server_address.sin_port = htons(PORT);

  int connect_status = connect(client_socket, (struct sockaddr*)&server_address,
                               sizeof(server_address));
  if (connect_status == -1) {
    failedToError("connect to server");
    close(client_socket);
    return 1;
  }

  std::thread t(receive_response, client_socket, BUFFER_SIZE);
  t.detach();

  // send messages to server
  std::string message;

  while (true) {
    std::getline(std::cin, message);
    int send_len = send(client_socket, message.c_str(), message.length(), 0);
    if (send_len == -1) {
      failedToError("send message to server");
      break;
    }
  }

  close(client_socket);
  return 0;
}