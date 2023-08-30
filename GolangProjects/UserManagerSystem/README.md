# UserManagerSystem

## 功能

基于 Golang 的用户管理系统，面试必备项目。在这个项目中你可以学到：

* 简单但完整的 golang 后端项目框架
* 常用的 rpc 框架，包括 grpc、http 和 thrift
* 监控报警
* mysql 数据库的简单使用
* redis 缓存的简单使用
* 基于 http 的基础前端搭建方法
* 基于 vue 的简单前端搭建方法（<https://www.gin-vue-admin.com>）
* 单元测试写法
* 性能测试写法
* 压测工具
* docker 镜像使用方法

## 搭建 docker 环境

```bash
# 搭建 docker 容器
bash docker.sh build

# 运行 docker 容器
bash docker.sh run
```

## 依赖

### 1. protoc

已经在 Dockerfile 中增加了，如果使用 docker 的话可以跳过这一步：

```bash
sudo apt install protobuf-compiler
```

还需要安装 `protoc-gen-go`，用于生成 `xx.pb.go` 文件：

```bash
# 网络不通时使用代理:
# export GOPROXY=https://proxy.golang.com.cn,direct

go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
```

再安装 `protoc-gen-go-grpc` 插件，用于生成 `xx_grpc.pb.go` 文件：

```bash
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
```

此时这两个插件会安装在 `~/go/bin` 下，我们可以在 `~/.bashrc` 中加入一行：

```bash
export PATH=~/go/bin:$PATH
```

检查插件是否安装成功：

```bash
$ protoc-gen-go --version
protoc-gen-go v1.31.0
```

### 2. redis

### 3. MySql

## 编译

```bash
go mod tidy
```
