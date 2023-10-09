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
  // 创建 Netlink Socket
  int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  printf2console("sender is ready!");

  if (fd < 0) {
    perror2console("create Netlink socket failed!");
    return -1;
  }

  // 声明发送地址和接收地址
  struct sockaddr_nl src_addr;
  struct sockaddr_nl dest_addr;

  // 给 Netlink Message 分配 buffer (message header + message payload)
  // NLMSG_SPACE(len) 返回不小于 NLMSG_LENGTH(len) 且字节对齐的最小数值
  struct nlmsghdr* nlh = (struct nlmsghdr*)::malloc(NLMSG_SPACE(NLINK_MSG_LEN));

  // iovec 是一个在 C 和 C++ 中用于进行 IO 操作的数据结构, 通常用于分散-聚集(scatter-gather) IO
  // 它在系统编程中非常有用, 特别是用于处理大量数据时
  //
  // struct iovec {
  //     void  *iov_base;  // 指向数据的指针
  //     size_t iov_len;   // 数据的长度
  // };
  struct iovec iov;

  // msghdr 是一个用于 C 和 C++ 中进行套接字通信的数据结构, 它用于指定和描述数据的收发操作
  // 通常与系统调用 sendmsg 和 recvmsg 一起使用
  // 它允许你在一个单独的函数调用中操作多个数据块, 并提供了更多的控制选项, 适用于高级的网络编程需求
  struct msghdr msg;

  ::memset(nlh, 0, NLMSG_SPACE(NLINK_MSG_LEN));

  src_addr.nl_family = AF_NETLINK;  // AF_NETLINK socket protocol
  src_addr.nl_pid = 1;              // application unique id
  src_addr.nl_groups = 0;           // specify not a multicast communication

  // attach socket to unique id or address
  ::bind(fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

  nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);  // netlink message length
  nlh->nlmsg_pid = 1;                           // src application unique id
  nlh->nlmsg_flags = 0;

  iov.iov_base = (void*)nlh;     // netlink message header base address
  iov.iov_len = nlh->nlmsg_len;  // netlink message length

  dest_addr.nl_family = AF_NETLINK;  // protocol family
  dest_addr.nl_pid = 2;              // destination process id
  dest_addr.nl_groups = 0;

  msg.msg_name = (void*)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  for (int i = 0; i < 100; ++i) {
    // 拷贝数据
    std::string content = "Hello wordld! " + std::to_string(i);
    ::strcpy((char*)(NLMSG_DATA(nlh)), content.c_str());

    // 发送消息
    ssize_t bytes_sent = ::sendmsg(fd, &msg, 0);

    // 检查是否发送成功
    if (bytes_sent == -1) {
      perror2console("send message fail");
    } else {
      printf2console("send message [%s]", (char*)NLMSG_DATA(nlh));
    }
  }

  ::close(fd);  // close the socket
}
