#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

namespace {
constexpr uint32_t kBufferSize = 1024;
}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " <server ip> <port>" << std::endl;
    return -1;
  }
  const char* server_ip = argv[1];
  uint16_t server_port = std::atoi(argv[2]);
  std::cout << "set server address: " << server_ip << ":" << server_port << std::endl;

  int client_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (client_socket <= 0) {
    std::cout << "socket() fail: " << std::strerror(errno) << std::endl;
    return -1;
  }
  struct sockaddr_in server_address;
  std::memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = ::inet_addr(server_ip);
  server_address.sin_port = htons(server_port);

  std::cout << "Please enter the message to send:" << std::endl;

  while (true) {
    std::string message;
    std::getline(std::cin, message);
    ::sendto(client_socket, message.c_str(), message.length(), 0, (struct sockaddr*)&server_address,
             sizeof(server_address));

    struct sockaddr_in server_reply_address;
    socklen_t server_reply_address_len = sizeof(server_reply_address);
    char buffer[kBufferSize] = {0};
    ssize_t n = ::recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr*)&server_reply_address, &server_reply_address_len);
    if (n <= 0) {
      std::cout << "recvfrom() fail: " << std::strerror(errno) << std::endl;
      return -1;
    }

    std::cout << "Received reply from server: " << buffer << std::endl;
  }

  return 0;
}
