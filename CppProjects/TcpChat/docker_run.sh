#!/bin/bash

# 获取脚本所在的目录路径
# script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# echo ${script_dir}

# 基于 tcp_chat 镜像创建名为 TcpChat 的容器
docker run -v /root/github/ToyBox/CppProjects/TcpChat:/TcpChat -it tcp_chat