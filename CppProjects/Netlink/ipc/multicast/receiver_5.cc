#include <linux/netlink.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <string>

#define printf2console(fmt, args...)                                                           \
  do {                                                                                         \
    struct timeval now;                                                                        \
    ::gettimeofday(&now, nullptr);                                                             \
    struct tm tm_now;                                                                          \
    ::localtime_r(&now.tv_sec, &tm_now);                                                       \
    printf("[%04d-%02d-%02d %02d:%02d:%02d.%06ld][%s:%d][%s]" fmt "\n", tm_now.tm_year + 1900, \
           tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,    \
           now.tv_usec, __FILE__, __LINE__, __FUNCTION__, ##args);                             \
    fflush(stdout);                                                                            \
  } while (0)

#define perror2console(str)                                                                \
  do {                                                                                     \
    struct timeval now;                                                                    \
    ::gettimeofday(&now, nullptr);                                                         \
    struct tm tm_now;                                                                      \
    ::localtime_r(&now.tv_sec, &tm_now);                                                   \
    char buff[200];                                                                        \
    snprintf(buff, sizeof(buff), "[%04d-%02d-%02d %02d:%02d:%02d.%06ld][%s:%d][%s]",       \
             tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour,     \
             tm_now.tm_min, tm_now.tm_sec, now.tv_usec, __FILE__, __LINE__, __FUNCTION__); \
    std::string new_str = std::string(buff) + str;                                         \
    perror(new_str.c_str());                                                               \
  } while (0)

#define NLINK_MSG_LEN 1024

int main() {
  int fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  printf2console("receiver is ready!");

  if (fd < 0) {
    perror2console("create Netlink socket failed!");
    return -1;
  }

  struct sockaddr_nl src_addr;
  struct sockaddr_nl dest_addr;
  // allocate buffer for netlink message which
  // is message header + message payload
  struct nlmsghdr* nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(NLINK_MSG_LEN));
  // fill the iovec structure
  struct iovec iov;
  // define the message header for message
  struct msghdr msg;

  nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);
  nlh->nlmsg_pid = getpid();  // src application unique id
  nlh->nlmsg_flags = 0;

  src_addr.nl_family = AF_NETLINK;  // AF_NETLINK socket protocol
  src_addr.nl_pid = getpid();       // application unique id
  src_addr.nl_groups = (1 << 5);    // specify the address which the process want to receive
                                    // if two or more address then turn on those bits
                                    // subscribe to 5 multicast address
  printf("Listening to the MCAST address (5): %d\n", src_addr.nl_groups);
  // attach socket to unique id or address
  bind(fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

  iov.iov_base = (void*)nlh;     // netlink message header base address
  iov.iov_len = nlh->nlmsg_len;  // netlink message length

  msg.msg_name = (void*)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  /* Listen forever in a while loop */
  while (1) {
    ssize_t bytes_received = ::recvmsg(fd, &msg, 0);
    if (bytes_received == -1) {
      perror2console("receive message fail");
    } else if (bytes_received == 0) {
      printf2console("connection closed by remote peer");
    } else {
      printf2console("recived message: %s", (char*)NLMSG_DATA(nlh));
    }
  }
  close(fd);  // close the socket
}
