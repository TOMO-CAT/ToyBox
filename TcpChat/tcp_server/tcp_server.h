#pragma once

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>

#include "thirdparty/coost/include/co/log.h"

namespace tcp_chat {

class TcpServer {
 public:
  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;

 private:
  TcpServer() {
  }

 public:
  int32_t port() {
    return port_;
  }
  void set_port(int32_t port) {
    port_ = port;
  }

 public:
  void Start(int32_t port);
  void Stop();

 private:
  bool ListenOn();

 public:
  // Meyers's singleton 单例设计模式: https://zhuanlan.zhihu.com/p/476220724
  static TcpServer& GetInstance() {
    static TcpServer instance;
    return instance;
  }

 private:
  int32_t port_ = -1;
  int32_t server_sockfd_ = -1;

  std::atomic<bool> is_exit_ = false;

  std::thread send_thread_;
};

}  // namespace tcp_chat