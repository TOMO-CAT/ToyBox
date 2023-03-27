#pragma once

#include <atomic>
#include <string>
#include <thread>

namespace tcp_chat {

class TcpClient final {
 public:
  TcpClient(const std::string& name);
  ~TcpClient();

 public:
  void Connect(const std::string& server_ip, uint32_t server_port);
  void Disconnect();
  void Send(const std::string& message);

 private:
  std::string name_;
  int32_t sockfd_ = -1;
  std::atomic<bool> is_stop_ = {false};
  std::thread recv_thread_;
};

}  // namespace tcp_chat