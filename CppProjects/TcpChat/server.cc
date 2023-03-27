#include <chrono>
#include <thread>

#include "TcpChat/tcp_server/tcp_server.h"

int main() {
  constexpr uint32_t port = 8799;

  auto& server = tcp_chat::TcpServer::GetInstance();
  server.Start(port);

  // run for 30 minutes
  std::this_thread::sleep_for(std::chrono::minutes(30));
  server.Stop();

  return 0;
}