# 指定基础镜像
FROM ubuntu:22.04

# 安装依赖
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
    && apt-get clean && apt-get update && apt-get install -y --fix-missing \
    vim \
    python3 \
    python3-pip \
    sudo

# 另一种安装 pip3 的方法
# wget https://bootstrap.pypa.io/get-pip.py
# sudo python3 get-pip.py 
