#include <chrono>
#include <cstdlib>
#include <iostream>

#include "TcpChat/tcp_client/tcp_client.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " client_name" << std::endl;
    exit(EXIT_FAILURE);
  }

  tcp_chat::TcpClient client(argv[1]);
  client.Connect("127.0.0.1", 8799);

  // wait all clients to start
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // send message to other clients
  client.Send("hello everyone!");
  client.Send(std::string("my name is ") + argv[1]);
  client.Send("goodbye, see you next time");

  std::this_thread::sleep_for(std::chrono::seconds(2));
  client.Disconnect();

  return 0;
}