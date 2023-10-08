# README

## 功能

基于 Netlink 的内核通信 Hello-World 简单例子。

## 文件目录

```bash
$ tree
.
├── Makefile
├── netlink_test.c
└── usr_netlink.c

0 directories, 3 files
```


## 用法

```bash
# 编译
make
gcc -g user_netlink.c -o user_netlink

# 加载内核模块
sudo insmod netlink_test.ko

# 运行测试代码
./user_netlink

# 移除内核模块
sudo rmmod netlink_test

# 查看内核输出缓冲区
dmesg

# 清理中间文件
make clean
```
