// https://blog.csdn.net/weixin_45646601/article/details/127824635
// https://blog.csdn.net/m0_48660921/article/details/122382490

#include "TcpChat/log.h"

class TcpServer {
 public:
  // Meyers's singleton 单例设计模式: https://zhuanlan.zhihu.com/p/476220724
  static TcpServer& GetInstance() {
    static TcpServer instance;
    return instance;
  }
  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;

 private:
  TcpServer() {
  }

 public:
  int32_t server_sockfd_ = -1;
};

int main() {
  LOG_INFO("print something...");
  // int listen_socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  // if (listen_socket_fd == -1) {

  // }
  return 0;
}