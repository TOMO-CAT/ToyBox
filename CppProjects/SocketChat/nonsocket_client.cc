#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap.h>

#include <cstring>
#include <iostream>
#include <memory>

// Define the UDP header struct
struct udpheader {
  u_short sport;  // Source port
  u_short dport;  // Destination port
  u_short len;    // Length of UDP header + payload
  u_short crc;    // Checksum
};

namespace {
constexpr uint32_t kPort = 7777;
}

#include <netinet/in.h>

#include <cstdint>

// Calculate the checksum for a given buffer
uint16_t checksum(const uint16_t* buf, uint32_t size, uint32_t src_addr, uint32_t dest_addr,
                  uint8_t proto) {
  uint32_t sum = 0;
  const uint16_t* buf_end = buf + size / 2;

  // Add the pseudo-header (source and destination IP addresses, protocol, and UDP length)
  sum += (src_addr >> 16) & 0xFFFF;
  sum += src_addr & 0xFFFF;
  sum += (dest_addr >> 16) & 0xFFFF;
  sum += dest_addr & 0xFFFF;
  sum += proto;
  sum += htons(size);

  // Add the data in the buffer
  while (buf < buf_end) {
    sum += *buf++;
  }

  // If the size is odd, add the last byte as well
  if (size % 2) {
    sum += *reinterpret_cast<const uint8_t*>(buf_end);
  }

  // Fold the 32-bit sum to 16 bits
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  // Take the one's complement of the sum
  return static_cast<uint16_t>(~sum);
}

int main() {
  // Open a pcap handle for the network interface
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t* handle = ::pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
    fprintf(stderr, "pcap_open_live error: %s\n", errbuf);
    return 1;
  }

  // Create the UDP packet
  const char payload[] = "Hello, world!";
  int payload_len = ::strlen(payload);

  udpheader udp;
  udp.sport = ::htons(1234);                           // Source port
  udp.dport = ::htons(kPort);                          // Destination port
  udp.len = ::htons(sizeof(udpheader) + payload_len);  // Length of header + payload
  udp.crc = 0;                                         // Checksum (will be calculated later)

  size_t packet_len = sizeof(struct ether_header) + sizeof(struct ip) + sizeof(udp) + payload_len;
  std::unique_ptr<char[]> packet(new char[packet_len]);

  // Construct the Ethernet header
  struct ether_header* eth = (struct ether_header*)packet.get();
  eth->ether_shost[0] = 0x00;
  eth->ether_shost[1] = 0x11;
  eth->ether_shost[2] = 0x22;
  eth->ether_shost[3] = 0x33;
  eth->ether_shost[4] = 0x44;
  eth->ether_shost[5] = 0x55;
  eth->ether_dhost[0] = 0x00;
  eth->ether_dhost[1] = 0x66;
  eth->ether_dhost[2] = 0x77;
  eth->ether_dhost[3] = 0x88;
  eth->ether_dhost[4] = 0x99;
  eth->ether_dhost[5] = 0xaa;
  eth->ether_type = htons(ETHERTYPE_IP);

  // Construct the IP header
  struct ip* ip = (struct ip*)(packet.get() + sizeof(struct ether_header));
  ip->ip_v = 4;    // IPv4
  ip->ip_hl = 5;   // Header length (in 32-bit words)
  ip->ip_tos = 0;  // Type of service
  ip->ip_len = htons(sizeof(struct ip) + sizeof(udp) + payload_len);  // Total length
  ip->ip_id = htons(0);                                               // Identification
  ip->ip_off = htons(IP_DF);                                          // Don't fragment
  ip->ip_ttl = 64;                                                    // Time to live
  ip->ip_p = IPPROTO_UDP;                                             // Protocol (UDP)
  ip->ip_sum = 0;                              // Checksum (will be calculated later)
  ip->ip_src.s_addr = inet_addr("127.0.0.1");  // Source IP address
  ip->ip_dst.s_addr = inet_addr("127.0.0.1");  // Destination IP address

  // Construct the UDP header
  udpheader* udp_ptr = (udpheader*)(packet.get() + sizeof(struct ether_header) + sizeof(struct ip));
  *udp_ptr = udp;

  // Append the payload to the UDP header
  char* payload_ptr = (char*)(udp_ptr + 1);
  memcpy(payload_ptr, payload, payload_len);

  // Calculate the UDP checksum
  udp.crc = checksum((unsigned short*)udp_ptr, sizeof(udpheader) + payload_len, ip->ip_src.s_addr,
                     ip->ip_dst.s_addr, IPPROTO_UDP);
  udp_ptr->crc = udp.crc;

  // Calculate the IP checksum
  ip->ip_sum = checksum((unsigned short*)ip, sizeof(struct ip), 0, 0, IPPROTO_IP);

  // Send the packet
  if (pcap_sendpacket(handle, (u_char*)packet.get(), packet_len) != 0) {
    fprintf(stderr, "pcap_sendpacket error: %s\n", pcap_geterr(handle));
    return 1;
  }

  std::cout << "send message success, going to quit" << std::endl;

  // Close the pcap handle
  ::pcap_close(handle);

  return 0;
}
