#include "TcpChat/tcp_server/tcp_server.h"

#include <asm-generic/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace {

constexpr uint32_t kBacklog = 10;

}

namespace tcp_chat {

bool TcpServer::ListenOn() {
  server_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sockfd_ == -1) {
    ELOG << "socket() fail: " << std::strerror(errno);
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
    ELOG << "bind() fail: " << std::strerror(errno);
    return false;
  }

  if (listen(server_sockfd_, kBacklog) == -1) {
    ELOG << "listen() fail: " << std::strerror(errno);
    return false;
  }

  LOG << "start to listen on port: " << port_;
  return true;
}

/**
 * @brief 设置非阻塞模式
 *        IO 模型: https://www.yuque.com/tomocat/fryenb/pcis7w16rtc2yxgg
 *
 * @param fd 文件描述符
 * @return true 设置成功
 * @return false 设置失败
 */
bool TcpServer::SetNonBlocking(int32_t fd) {
  // 获取文件状态标志:
  //     fcntl: 系统调用函数, 用于对已打开的文件描述符进行各种控制操作
  //     F_GETFL: 获取打开文件的文件描述符的 flag status flags, 它是控制文件 IO 行为的一组 bit mask
  int32_t flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    ELOG << "fcntl() fail: " << std::strerror(errno);
    return false;
  }

  // 设置非阻塞模式
  //     F_SETFL: 设置文件状态标志
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1) {
    ELOG << "fcntl() fail: " << std::strerror(errno);
    return false;
  }
  return true;
}

}  // namespace tcp_chat
