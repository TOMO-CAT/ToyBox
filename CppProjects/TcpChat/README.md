# TcpChat

## TcpServer

TcpServer 使用 epoll 实现并发处理多个连接请求，每个连接套接字都是在**非阻塞模式**下处理，这使得服务器能够同时处理多个连接请求并且不会因为某个连接被阻塞而影响其他连接的处理。当有新的连接请求时，服务器将新连接套接字加入 epoll 实例，并等待数据的到来。当某个连接上有数据到来时，服务器将数据读入 buffer 中，并处理客户端发送的数据，如果客户端关闭连接，则服务器关闭连接套接字，并从 epoll 实例中移除。

## Build

```bash
# 进入编译文件夹
$ cd CppProjects

# 编译 TcpServer 和 TcpClient
$ blade build ...

# 运行 server 端
$ ./build64_release/TcpChat/server
```
