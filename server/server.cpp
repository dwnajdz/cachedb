#include <netinet/in.h> // For address structures
#include <sys/socket.h> // For socket related functions
#include <unistd.h>     // For close function

#include <cstring>
#include <iostream> // For input/output operations
#include <thread>
#include <vector>

// port address of server
#define PORT 3000
// default buffer size of message
#define BUFFER_SIZE 1024
// number of maximum connections query if server is busy
#define BACKLOG 10

void failedToError(std::string message) {
  std::cerr << "Failed to " << message << std::endl;
}

void handle_client(int client_socket, int buffer_size = BUFFER_SIZE) {
  char buffer[buffer_size] = {0};

  while (true) {
    // receive message
    int recv_len = recv(client_socket, buffer, buffer_size, 0);
    if (recv_len == -1) {
      failedToError("receive message from client");
      break;
    }

    std::cout << "Message from client: " << buffer << '\n';

    const std::string message = "accepted";
    int send_len = send(client_socket, message.c_str(), message.length(), 0);
    if (send_len == -1) {
      failedToError("send message");
      continue;
    }

    memset(buffer, 0, buffer_size);
  }

  close(client_socket);
}

// default port: 3000, buffer_size: 1024, backlog: 10
int host_server(int port = PORT,
                int buffer_size = BUFFER_SIZE,
                int backlog = BACKLOG) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    failedToError("create socket");
    return 1;
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  int bind_status = bind(
      server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
  if (bind_status == -1) {
    failedToError("bind socket");
    close(server_socket);
    return 1;
  }

  int listen_status = listen(server_socket, backlog);
  if (listen_status == -1) {
    failedToError("listen for incoming connections");
    return 1;
  }

  while (true) {
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    // accept an connection
    int client_socket =
        accept(server_socket, (struct sockaddr*)&client_address, &addrlen);
    if (client_socket == -1) {
      failedToError("accept incoming client connection");
      continue;
    }

    std::thread t(handle_client, client_socket, buffer_size);
    t.detach();
  }
  close(server_socket);
  return 0;
}

int main(int argc, char const* argv[]) {
  host_server();
}
