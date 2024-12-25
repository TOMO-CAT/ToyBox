# Aiortc WSL Camera Server

## 环境准备

```bash
# windows 安装 ffmpeg
# https://ffmpeg.org/download.html

# windows 转发摄像头数据流
# https://blog.iyatt.com/?p=10249

# WSL
#
# 查询主机 ip
ifconfig

# Windows
#
# 首先在主机中用 ffmpeg 查询 USB 摄像头名称
ffmpeg -list_devices true -f dshow -i dummy
# 转发摄像头数据
ffmpeg -f dshow -i video="Integrated Camera" -preset ultrafast -tune zerolatency -vcodec libx264 -f mpegts udp://172.26.39.229:5000

# WSL
#
# 运行脚本展示数据

```
