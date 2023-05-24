#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define PACKET_SIZE 4096
#define DEST_IP "127.0.0.1"
#define DEST_PORT 7777

int main() {
  // 打开 TUN/TAP 设备
  int fd = ::open("/dev/net/tun", O_RDWR);
  if (fd < 0) {
    std::cerr << "Failed to open TUN/TAP device: " << std::strerror(errno) << std::endl;
    return -1;
  }

  // 设置 TUN/TAP 设备参数
  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  if (::ioctl(fd, TUNSETIFF, reinterpret_cast<void*>(&ifr)) < 0) {
    std::cerr << "Failed to set TUN/TAP device parameters: " << std::strerror(errno) << std::endl;
    ::close(fd);
    return -1;
  }

  // 构造 UDP 数据包
  char packet[PACKET_SIZE];
  std::memset(packet, 0, sizeof(packet));
  struct iphdr* ip_header = (struct iphdr*)packet;
  ip_header->version = 4;
  ip_header->ihl = 5;
  ip_header->tos = 0;
  ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr));
  ip_header->id = htons(getpid());
  ip_header->frag_off = 0;
  ip_header->ttl = 64;
  ip_header->protocol = IPPROTO_UDP;
  ip_header->check = 0;
  ip_header->saddr = inet_addr("192.168.1.100");
  ip_header->daddr = inet_addr(DEST_IP);
  struct udphdr* udp_header = (struct udphdr*)(packet + sizeof(struct iphdr));
  udp_header->source = htons(1234);
  udp_header->dest = htons(DEST_PORT);
  udp_header->len = htons(sizeof(struct udphdr));
  udp_header->check = 0;
  std::strncpy(packet + sizeof(struct iphdr) + sizeof(struct udphdr), "Hello, server!",
               sizeof(packet) - sizeof(struct iphdr) - sizeof(struct udphdr));

  // 向 TUN/TAP 设备写入数据包
  ssize_t n_sent = ::write(
      fd, packet, sizeof(struct iphdr) + sizeof(struct udphdr) + std::strlen("Hello, server!"));
  if (n_sent < 0) {
    std::cerr << "write() fail: " << std::strerror(errno) << std::endl;
    ::close(fd);
    return 1;
  }

  ::close(fd);
  return 0;
}
