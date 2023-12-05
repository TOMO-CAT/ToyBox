#include <linux/netlink.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <string>
#include <thread>

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

// https://stackoverflow.com/questions/43560200/why-max-netlink-msg-size-is-limited-to-16k
// #define NLINK_MSG_LEN 16384
uint32_t NLINK_MSG_LEN = 16384;

int main() {
  // 创建 netlink socket
  // int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
  printf2console("sender is ready!");
  if (fd < 0) {
    perror2console("create Netlink socket failed!");
    return -1;
  }

  // 声明发送地址
  struct sockaddr_nl src_addr;
  src_addr.nl_family = AF_NETLINK;  // AF_NETLINK socket protocol
  src_addr.nl_pid = 1;              // application unique id
  src_addr.nl_groups = 0;           // specify not a multicast communication

  // attach socket to unique id or address
  if (::bind(fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) != 0) {
    perror2console("bind Netlink socket failed!");
    return -1;
  }

  // 声明接收地址
  struct sockaddr_nl dest_addr;
  dest_addr.nl_family = AF_NETLINK;  // protocol family
  dest_addr.nl_pid = 2;              // destination process id
  dest_addr.nl_groups = 0;

  // nlmsghdr 是 Netlink 消息头部结构体
  // struct nlmsghdr {
  //     __u32 nlmsg_len;   /* 长度字段，表示整个消息的长度 */
  //     __u16 nlmsg_type;  /* 消息类型 */
  //     __u16 nlmsg_flags; /* 消息标志 */
  //     __u32 nlmsg_seq;   /* 序列号，用于匹配请求和响应消息 */
  //     __u32 nlmsg_pid;   /* 发送进程的 PID */
  // };
  //
  // NLMSG_SPACE 用于计算给定长度 len 的 Netlink 消息的总空间大小 (包括 nlmsghdr 的大小)
  struct nlmsghdr* nlh = (struct nlmsghdr*)::malloc(NLMSG_SPACE(NLINK_MSG_LEN));
  ::memset(nlh, 0, NLMSG_SPACE(NLINK_MSG_LEN));
  nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);  // netlink message length
  nlh->nlmsg_pid = 1;                           // src application unique id
  nlh->nlmsg_flags = 0;
  printf2console("total netlink msg length [%u]", nlh->nlmsg_len);

  // iovec 是用于分散-聚集 IO (scatter-gather IO), 这里用于构建 netlink 消息的数据部分,
  // 用于将 Netlink 消息的数据分散在多个非连续的内存中, 以适应消息的多部份结构
  struct iovec iov;
  iov.iov_base = reinterpret_cast<void*>(nlh);  // netlink message header base address
  iov.iov_len = nlh->nlmsg_len;                 // netlink message length

  // struct msghdr {
  //     void         *msg_name;       /* 用于指定消息的目标地址信息 */
  //     socklen_t     msg_namelen;    /* 目标地址信息的长度 */
  //     struct iovec *msg_iov;        /* 数据缓冲区数组的指针，用于分散-聚集 I/O */
  //     size_t        msg_iovlen;     /* 数据缓冲区数组的长度 */
  //     void         *msg_control;    /* 用于传递辅助数据（控制信息）的缓冲区 */
  //     socklen_t     msg_controllen; /* 辅助数据缓冲区的长度 */
  //     int           msg_flags;      /* 操作标志 */
  // };
  struct msghdr msg;
  msg.msg_name = reinterpret_cast<void*>(&dest_addr);
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // 构造即将发送的数据
  std::string content = std::string(NLINK_MSG_LEN, 'x');

  // 将数据拷贝到 nlh 中的 data
  ::snprintf(reinterpret_cast<char*>(NLMSG_DATA(nlh)), NLINK_MSG_LEN, "%s", content.c_str());

  for (int i = 0; i < 10; ++i) {
    ssize_t bytes_sent = ::sendmsg(fd, &msg, 0);

    // 检查是否发送成功
    if (bytes_sent == -1) {
      perror2console("send message fail");
    } else {
      // printf2console("send message [%s]", reinterpret_cast<char*>(NLMSG_DATA(nlh)));
      printf2console("send %ld message", bytes_sent);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  ::close(fd);  // close the socket
}
