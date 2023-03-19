# LogParser

## 功能

并发解析多个日志文件，并提供如下 API：

* 支持获取列的最大值、最小值、中位数、均值和求和
* 支持返回列的排序结果，包括正序和倒序

## 使用

### 1. 生成测试日志文件

注意需要使用 python3。

```bash
# 进入文件夹
$ cd CppProjects/LogParser

# 编译
$ blade build scripts

# 生成测试日志
$ ../build64_release/LogParser/scripts/gen_test_log
```

### 2. LogParser

```bash
# 进入文件夹
$ cd CppProjects

# 编译
$ blade build LogParser

# 进入执行文件夹
$ cd LogParser

# 执行
$ ../build64_release/LogParser/LogParser
```
