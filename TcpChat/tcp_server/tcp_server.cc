#include "TcpChat/tcp_server/tcp_server.h"

namespace {

constexpr uint32_t kBacklog = 10;

}

namespace tcp_chat {

bool TcpServer::ListenOn() {
  server_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sockfd_ == -1) {
    ELOG << "socket() fail: " << strerror(errno);
    return false;
  }

  int32_t opt = -1;
  setsockopt(server_sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_sockfd_, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
    ELOG << "bind() fail: " << strerror(errno);
    return false;
  }

  if (listen(server_sockfd_, kBacklog) == -1) {
    ELOG << "listen() fail: " << strerror(errno);
    return false;
  }

  LOG << "start to listen on port: " << port_;
  return true;
}

}  // namespace tcp_chat
