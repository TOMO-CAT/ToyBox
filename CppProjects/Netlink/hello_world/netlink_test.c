#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <net/sock.h>

#define NETLINK_TEST 30
#define MSG_LEN 125
#define USER_PORT 100

/* 声明模块协议、作者和描述 */
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("K");
MODULE_DESCRIPTION("A simple example for netlink");

/* Netlink Socket */
struct sock* nlsk = NULL;
/* 声明一个外部的全局变量 init_net */
extern struct net init_net;

/* 发送消息到用户空间 */
int send_usrmsg(char* pbuf, uint16_t len) {
  /*
   * sk_buff: Linux 内核忠用于表示网络数据包的数据结构, 用于在网络忠传递和处理数据包
   *
   * @field: struct sk_buff *next 和 struct sk_buff *prev 构造了一个双向链表, 通常用于实现队列
   * @field: struct sk_buff *prev 指向与该数据包相关的 socket 指针, 以便内核根据 socket 信息来处理数据包
   * @field: unsigned char *head 和 unsigned char *data 指向数据包的起始位置和实际数据内容起始地址
   * @field: unsigned int len 是 sk_buffer 中有效数据的长度
   * @field: nsigned int truesize 表示分配给 sk_buff 结构体的内存大小, 通常比 len 大以容纳额外的元数据和内核信息
   * @field: struct skb_shared_info *shinfo 指向共享信息结构的指针, 该结构用于存储与数据包相关的元信息, 如协议头和接口索引等信息
   * @field: struct sk_buff_head queue_mapping 用于将 sk_buff 结构体组织成队列
   * @field: unsigned char cb[48] 存储内核或网络协议栈的私有数据, 各个网络协议栈的实现可以使用这个字段来存储自定义信息
   */
  struct sk_buff* nl_skb;

  /*
   * nlmsghdr: NetlinkMessageHeader, Linux 内核中用于表示 Netlink 消息头的数据结构
   *
   * @field: __u32 nlmsg_len 表示消息头和消息数据的总长度
   * @field: __u16 nlmsg_type 消息类型
   * @field: __u16 nlmsg_flags 消息属性
   * @field: __u32 nlmsg_seq 消息序列号
   * @field: __u32 nlmsg_pid 消息发出的进程 ID
   */
  struct nlmsghdr* nlh;

  int ret;

  /* 创建sk_buff 空间 */
  nl_skb = nlmsg_new(len, GFP_ATOMIC);
  if (!nl_skb) {
    printk("netlink alloc failure\n");
    return -1;
  }

  /*
   * nlmsg_put: 向 Netlink 消息中添加消息头信息
   *
   * struct nlmsghdr *nlmsg_put(struct sk_buff *skb, u32 pid, u32 seq, int type, int payload, int flags);
   *
   * @field: skb 指向包含 Netlink 消息的 sk_buff 结构体指针, 其中 sk_buff 用于表示网络数据包
   * @field: pid 发送消息的进程 ID, 通常是发送消息的内核模块或者用户空间地址的 PID
   * @field: seq 消息序列号
   * @field: type 消息类型
   * @field: payload 消息的数据部分长度, 即消息之后的有效数据长度
   * @field: flags 消息的标志属性, 用于指示消息的属性或状态
   */
  nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
  if (nlh == NULL) {
    printk("nlmsg_put failaure \n");
    nlmsg_free(nl_skb);
    return -1;
  }

  /* 拷贝数据发送 */
  memcpy(nlmsg_data(nlh), pbuf, len);

  /*
   * netlink_unicast: 向单个 Netlink socket 发送消息的函数, 用于将 Netlink 消息发送给指定的接收方 (通常是用户空间进程或者内核模块),
   *   并且提供了一种简单的单播 (unicast) 机制, 实现了点对点的通信
   *
   * int netlink_unicast(struct sock *sk, struct sk_buff *skb, __u32 pid, int nonblock);
   *
   * @field: sk 指向要发送消息的 Netlink socket 指针, 这个 socket 是消息的发送方, 用于指示消息应该通过哪个 Netlink socket 发送
   * @field: skb 指向包含要发送的 Netlink 消息的 sk_buff 结构体指针, 它包含了消息的数据和消息头
   * @field: pid 消息的接受者的用户空间进程 ID, 或者在内核模块之间通信时是接收模块的 PID, 接收方通过这个 PID 确定消息的发送者
   * @field: nonblock 整数标志, 用于指示发送操作是否应该是非阻塞的, 如果将此参数设置为非零值 (一般是 MSG_DONTWAIT ) 那么发送操作是非阻塞的, 如果发送缓冲区已满则立即返回错误
   */
  ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);

  return ret;
}

static void netlink_rcv_msg(struct sk_buff* skb) {
  struct nlmsghdr* nlh = NULL;
  char* umsg = NULL;
  char* kmsg = "hello users!!!";

  if (skb->len >= nlmsg_total_size(0)) {
    nlh = nlmsg_hdr(skb);
    umsg = NLMSG_DATA(nlh);
    if (umsg) {
      printk("kernel recv from user: %s\n", umsg);
      send_usrmsg(kmsg, strlen(kmsg));
    }
  }
}

/* netlink_kernel_cfg 是在 Linux 内核编程中用于配置 Netlink 通信的数据结构 */
struct netlink_kernel_cfg cfg = {
    /* input 指定 Netlink 通信中接受消息时的回调函数, 它会在内核收到消息时被调用 */
    .input = netlink_rcv_msg,
};

int test_netlink_init(void) {
  /* create netlink socket */
  nlsk = (struct sock*)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
  if (nlsk == NULL) {
    printk("netlink_kernel_create error !\n");
    return -1;
  }
  printk("test_netlink_init\n");

  return 0;
}

void test_netlink_exit(void) {
  if (nlsk) {
    netlink_kernel_release(nlsk); /* release ..*/
    nlsk = NULL;
  }
  printk("test_netlink_exit!\n");
}

/* 注册模块初始化函数 */
module_init(test_netlink_init);

/* 注册模块退出函数 */
module_exit(test_netlink_exit);
