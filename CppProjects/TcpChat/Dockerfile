# 指定基础镜像
FROM ubuntu:20.04

# 安装依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# 创建一个挂载点, 用于将 TcpChat 目录映射到 docker 的 /TcpChat 目录
VOLUME ../TcpChat/TcpChat

# 指定工作目录
WORKDIR /TcpChat

