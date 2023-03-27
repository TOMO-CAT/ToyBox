#include <chrono>

#include "TcpChat/tcp_client/tcp_client.h"

int main() {
  tcp_chat::TcpClient client;
  client.Connect("127.0.0.1", 8799);

  // run for 30 minutes
  std::this_thread::sleep_for(std::chrono::minutes(30));
  client.Disconnect();

  return 0;
}