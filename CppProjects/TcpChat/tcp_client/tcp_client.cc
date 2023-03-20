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
  struct sockaddr_in server_addr;
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_ == -1) {
    ELOG << "socket fail(): " << std::strerror(errno);
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

  if (connect(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    ELOG << "connect() fail: " << std::strerror(errno);
    exit(EXIT_FAILURE);
  }

  // 将文件设置为非阻塞模式
  int flags = fcntl(sockfd_, F_GETFL, 0);
  fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);

  recv_thread_ = std::thread([this]() {
    // 设置超时时间为 1 秒
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (!is_stop_) {
      // 等待数据可读
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(sockfd_, &readfds);
      int ret = select(sockfd_ + 1, &readfds, nullptr, nullptr, &tv);
      LOG << "DEBUG: select return " << ret;

      static char buffer[kBufferSize];
      memset(buffer, 0, sizeof(buffer));

      if (ret == -1) {
        // 出错
        ELOG << "select() fail: " << std::strerror(errno);
        exit(EXIT_FAILURE);
      } else if (ret == 0) {
        // 超时
        WLOG << "read timeout";
      } else {
        // 数据可读
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

}  // namespace tcp_chat