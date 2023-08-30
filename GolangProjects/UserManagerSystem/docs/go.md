# go

## 安装

```bash
sudo apt install golang-go
```

## 设置代理

> <https://goproxy.io/zh/>

```bash
# 配置 GOPROXY 环境变量, 在 ~/.bashrc 中配置
export GOPROXY=https://proxy.golang.com.cn,direct

# 还可以设置不走 proxy 的私有仓库或组，多个用逗号相隔（可选）
# export GOPRIVATE=git.mycompany.com,github.com/my/private
```

或者直接在 `.vscode` 中配置代理：

```json
"go.toolsEnvVars": {
    "GO111MODULE": "on",
    "GOPROXY": "goproxy.cn,direct",
    "GOSUMDB": "off",
},
```

## 初始化仓库

```bash
go mod init github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem
```

## 下载依赖

```bash
go mod tidy
go mod vendor
```
