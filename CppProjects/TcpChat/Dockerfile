# 指定基础镜像
FROM ubuntu:22.04

# 安装依赖
RUN apt-get update && apt-get install -y --fix-missing \
    build-essential \
    git \
    curl \
    && curl https://raw.githubusercontent.com/baidu/blade-build/master/install.sh | sh \
    && rm -rf /var/lib/apt/lists/*

# 安装 blade
# RUN git clone https://github.com/chen3feng/blade-build.git /blade-build
# RUN cd /blade-build && \
#     ./install.sh && \
#     source ~/.profile && \
#     cd .. && \
#     rm -rf blade-build

# 创建一个挂载点, 用于将 TcpChat 目录映射到 docker 的 /TcpChat 目录
VOLUME ../TcpChat/TcpChat

# 指定工作目录
WORKDIR /TcpChat