# proto

## VSCode 插件

### 1. vscode-proto3

可以实现格式化，格式化参数可以在项目根目录配置一个 `.protolint.yaml`：

```yaml
# Proto Lint 插件配置文件
lint:
  rules:
    add:
      - MESSAGE_NAMES_UPPER_CAMEL_CASE
      - SERVICE_NAMES_UPPER_CAMEL_CASE
      - MAX_LINE_LENGTH

  rules_option:
      max_line_length:
        # Enforces a maximum line length
        max_chars: 140
        # Specifies the character count for tab characters
        tab_chars: 2
```

### 2. protolint

> 已经放在 ./dependency 文件夹中。

需要自己下载 protolint：

> https://github.com/yoheimuta/protolint/releases

```bash
$ cd /tmp
$ wget https://github.com/yoheimuta/protolint/releases/download/v0.45.0/protolint_0.45.0_Linux_x86_64.tar.gz
$ tar -zxvf protolint_0.45.0_Linux_x86_64.tar.gz
LICENSE
README.md
protolint
protoc-gen-protolint

$ sudo mv ./protolint /usr/bin/
$ sudo mv ./protoc-gen-protolint /usr/bin/
```
