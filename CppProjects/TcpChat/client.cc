#include "TcpChat/tcp_client/tcp_client.h"

int main() {
  tcp_chat::TcpClient client;
  client.Connect("127.0.0.1", 8799);
}