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
  int fd = ::socket(PF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  printf2console("sender is ready!");

  if (fd < 0) {
    perror2console("create Netlink socket failed!");
    return -1;
  }

  /* Declare for src NL sockaddr, dest NL sockaddr, nlmsghdr, iov, msghr */
  struct sockaddr_nl src_addr;
  struct sockaddr_nl dest_addr;
  // allocate buffer for netlink message which is message header + message payload
  struct nlmsghdr* nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(NLINK_MSG_LEN));
  // fill the iovec structure
  struct iovec iov;
  // define the message header for message
  // sending
  struct msghdr msg;

  ::memset(nlh, 0, NLMSG_SPACE(NLINK_MSG_LEN));
  ::memset(&src_addr, 0, sizeof(src_addr));

  src_addr.nl_family = AF_NETLINK;           // AF_NETLINK socket protocol
  src_addr.nl_pid = getpid();                // application unique id
  src_addr.nl_groups = (1 << 3) | (1 << 5);  // specify not a multicast communication

  // attach socket to unique id or address
  ::bind(fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

  nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);  // netlink message length
  nlh->nlmsg_pid = 900;                         // src application unique id
  nlh->nlmsg_flags = 0;

  ::strcpy((char*)(NLMSG_DATA(nlh)), "Hello World !");  // copy the payload to be sent

  iov.iov_base = (void*)nlh;     // netlink message header base address
  iov.iov_len = nlh->nlmsg_len;  // netlink message length

  dest_addr.nl_family = AF_NETLINK;  // protocol family
  dest_addr.nl_pid = 0;              // destination process id
  dest_addr.nl_groups = (1 << 3) | (1 << 5);

  msg.msg_name = (void*)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // 发送消息
  ssize_t bytes_sent = ::sendmsg(fd, &msg, 0);

  // 检查是否发送成功
  if (bytes_sent == -1) {
    perror2console("send message fail");
  } else {
    printf2console("send message [%s]", (char*)NLMSG_DATA(nlh));
  }

  ::close(fd);  // close the socket
}
