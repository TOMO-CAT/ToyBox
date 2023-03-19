#pragma once

#include <sys/epoll.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>
#include <thread>
#include <unordered_map>

#include "thirdparty/coost/include/co/log.h"

namespace tcp_chat {

class TcpServer {
 public:
  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;
  ~TcpServer();

 private:
  TcpServer();

 public:
  bool Start(int32_t port);
  void Stop();

 private:
  void ListenOn();
  void SetNonBlocking(int32_t fd);
  void CreateEpoll();
  void AddListenSocketToEpoll();
  void HandleEpollEvent();

 public:
  // Meyers's singleton 单例设计模式: https://zhuanlan.zhihu.com/p/476220724
  static TcpServer& GetInstance() {
    static TcpServer instance;
    return instance;
  }

 private:
  static constexpr uint32_t kMaxEpollEvents = 10;

 private:
  int32_t port_ = -1;
  int32_t listen_sockfd_ = -1;
  int32_t epoll_fd_ = -1;

  std::atomic<bool> is_stop_ = false;
  std::atomic<bool> is_stop_gracefully_ = false;

  struct epoll_event epoll_events_[kMaxEpollEvents];
  std::unordered_map<int32_t, std::string> sock_fd_to_client_ip_;

  std::thread epoll_thread_;
};

}  // namespace tcp_chat