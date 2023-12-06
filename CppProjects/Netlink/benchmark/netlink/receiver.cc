#include <linux/netlink.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <string>

// 配置消息大小
// https://stackoverflow.com/questions/43560200/why-max-netlink-msg-size-is-limited-to-16k
constexpr uint32_t NLINK_MSG_LEN = 16384;  // 16 kb
// constexpr uint32_t NLINK_MSG_LEN = 1024;  // 1 kb
// 配置需要接收的消息数量
constexpr uint32_t kCount = 1000;

inline uint64_t TimestampNanoSec() {
  struct timespec time;
  ::clock_gettime(CLOCK_REALTIME, &time);
  return time.tv_sec * 1000 * 1000 * 1000 + time.tv_nsec;
}

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

#define NLINK_MSG_LEN 16384

int main() {
  // https://stackoverflow.com/questions/26238160/is-anyone-using-netlink-for-ipc
  // 创建 netlink socket
  // int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
  printf2console("receiver is ready!");
  if (fd < 0) {
    perror2console("create Netlink socket failed!");
    return -1;
  }

  // 声明发送地址
  struct sockaddr_nl src_addr;
  src_addr.nl_family = AF_NETLINK;  // AF_NETLINK socket protocol
  src_addr.nl_pid = 2;              // application unique id
  src_addr.nl_groups = 0;           // specify not a multicast communication

  // attach socket to unique id or address
  if (::bind(fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) != 0) {
    perror2console("bind Netlink socket failed!");
  }

  // 声明接收地址
  struct sockaddr_nl dest_addr;
  // allocate buffer for netlink message which
  // is message header + message payload
  struct nlmsghdr* nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(NLINK_MSG_LEN));
  nlh->nlmsg_len = NLMSG_SPACE(NLINK_MSG_LEN);
  nlh->nlmsg_pid = 1000;  // src application unique id
  nlh->nlmsg_flags = 0;

  struct iovec iov;
  iov.iov_base = reinterpret_cast<void*>(nlh);  // netlink message header base address
  iov.iov_len = nlh->nlmsg_len;                 // netlink message length

  struct msghdr msg;
  msg.msg_name = reinterpret_cast<void*>(&dest_addr);
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // 死循环接收数据
  uint32_t last_sequence = 0;
  uint64_t timestamp_ns = 0;
  uint64_t delay_ns = 0;
  while (1) {
    ssize_t bytes_received = ::recvmsg(fd, &msg, 0);
    if (bytes_received == -1) {
      perror2console("receive message fail");
    } else if (bytes_received == 0) {
      printf2console("connection closed by remote peer");
    } else {
      // 检测是否存在乱序
      uint32_t sequence = *reinterpret_cast<uint32_t*>(
          (reinterpret_cast<char*>(NLMSG_DATA(nlh)) + sizeof(timestamp_ns)));
      if (sequence != last_sequence + 1) {
        printf2console("out of order!");
      }
      last_sequence = sequence;
      timestamp_ns = *reinterpret_cast<uint64_t*>(NLMSG_DATA(nlh));
      delay_ns = TimestampNanoSec() - timestamp_ns;
      printf2console("receiver [%d] [delay_ns = %ld] [timestamp = %ld] [size = %ld]", sequence,
                     delay_ns, timestamp_ns, bytes_received);
      // printf2console("received message: %s", reinterpret_cast<char*>(NLMSG_DATA(nlh)));
    }
  }

  ::close(fd);
}
