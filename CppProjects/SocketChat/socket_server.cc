#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>

namespace {
constexpr uint32_t kPort = 7777;
constexpr uint32_t kBufferSize = 1024;
}  // namespace

int main() {
  int server_socket = ::socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in server_address;
  std::memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = ::htonl(INADDR_ANY);
  server_address.sin_port = ::htons(kPort);
  if (::bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_address),
             sizeof(server_address)) != 0) {
    std::cout << "connect() fail: " << std::strerror(errno) << std::endl;
    return -1;
  }

  ::listen(server_socket, 1);
  std::cout << "start echo server, listen on " << kPort << " ..." << std::endl;

  while (true) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[kBufferSize] = {0};
    ssize_t n = ::recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr*)&client_address, &client_address_len);
    std::ostringstream oss;
    oss << "[" << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port)
        << "]";
    std::string client_addr = oss.str();
    std::cout << "receive message from client " << client_addr << ": " << buffer << std::endl;
    ::sendto(server_socket, buffer, n, 0, (struct sockaddr*)&client_address, client_address_len);
  }

  return 0;
}
