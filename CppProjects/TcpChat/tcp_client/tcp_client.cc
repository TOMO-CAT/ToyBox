#include "TcpChat/tcp_client/tcp_client.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "co/log.h"

namespace tcp_chat {

namespace {
constexpr uint32_t kBufferSize = 1024;
}

TcpClient::TcpClient() {
}

TcpClient::~TcpClient() {
  if (recv_thread_.joinable()) {
    recv_thread_.join();
  }
}

void TcpClient::Connect(const std::string& server_ip, uint32_t server_port) {
  sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd_ == -1) {
    ELOG << "socket fail(): " << std::strerror(errno);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
  server_addr.sin_port = htons(server_port);

  if (connect(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    ELOG << "connect() fail: " << std::strerror(errno);
    close(sockfd_);
    exit(EXIT_FAILURE);
  }

  recv_thread_ = std::thread([this]() {
    while (!is_stop_) {
      // 等待数据可读
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(sockfd_, &read_fds);

      // 设置超时时间为 1 秒
      struct timeval timeout;
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      int ret = select(sockfd_ + 1, &read_fds, nullptr, nullptr, &timeout);

      // 出错
      if (ret == -1) {
        ELOG << "select() fail: " << std::strerror(errno);
        exit(EXIT_FAILURE);
      }

      // 超时
      if (ret == 0) {
        WLOG << "read timeout";
        continue;
      }

      // 数据可读
      static char buffer[kBufferSize];
      memset(buffer, 0, sizeof(buffer));
      auto nbytes = read(sockfd_, buffer, kBufferSize);
      if (nbytes == -1) {
        // 出错
        ELOG << "read() fail: " << std::strerror(errno);
        exit(EXIT_FAILURE);
      } else if (nbytes == 0) {
        // 对端关闭连接
        LOG << "peer closed";
        close(sockfd_);
        exit(EXIT_FAILURE);
      } else {
        // 成功读到数据, 将收到的消息打印到标准输出
        std::cout << buffer << std::endl;
      }
    }
  });
}

void TcpClient::Disconnect() {
  is_stop_ = true;
  LOG << "TcpClient is going to quit, please wait";
  recv_thread_.join();
  LOG << "TcpClient quit successfully";
}

void TcpClient::Send(const std::string& message) {
  static char buffer[kBufferSize];
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, kBufferSize, "%s", message.c_str());
  if (write(sockfd_, buffer, strlen(buffer)) == -1) {
    ELOG << "write() fail: " << std::strerror(errno);
    exit(EXIT_FAILURE);
  }
}

// /**
//  * @brief 设置非阻塞模式
//  *        IO 模型: https://www.yuque.com/tomocat/fryenb/pcis7w16rtc2yxgg
//  *
//  * @param fd 文件描述符
//  */
// void TcpClient::SetNonBlocking(int32_t fd) {
//   // 获取文件状态标志:
//   //     fcntl: 系统调用函数, 用于对已打开的文件描述符进行各种控制操作
//   //     F_GETFL: 获取打开文件的文件描述符的 flag status flags, 它是控制文件 IO 行为的一组 bit
//   mask int32_t flags = fcntl(fd, F_GETFL, 0); if (flags == -1) {
//     ELOG << "fcntl() fail: " << std::strerror(errno);
//     exit(EXIT_FAILURE);
//   }

//   // 设置非阻塞模式
//   //     F_SETFL: 设置文件状态标志
//   flags |= O_NONBLOCK;
//   if (fcntl(fd, F_SETFL, flags) == -1) {
//     ELOG << "fcntl() fail: " << std::strerror(errno);
//     exit(EXIT_FAILURE);
//   }
// }

}  // namespace tcp_chat