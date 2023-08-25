# Logger

## 功能

异步日志库，包含如下特性：

1. 输出到日志文件或控制台
2. 按小时切割日志
3. Fatal 日志终止进程并打印堆栈
4. 支持删除过期日志, 避免打满服务器磁盘

## 用法

### 1. 例子

```bash
go run util/logger/example/main.go
```

### 2. 配置

```proto
message LoggerConfig {
  enum LogLevel {
    LOG_LEVEL_DEBUG = 1;
    LOG_LEVEL_INFO = 2;
    LOG_LEVEL_WARNING = 3;
    LOG_LEVEL_ERROR = 4;
    LOG_LEVEL_FATAL = 5;
  }

  message FileWriterConfig {
    // 是否将日志写入文件
    optional bool enable = 1 [default = false];
    // 日志级别, 只写入 >= LogLevel 的日志
    optional LogLevel log_level = 2 [default = LOG_LEVEL_DEBUG];
    // Info 日志存放路径
    optional string info_log_path = 3 [default = "logs/log.info"];
    // Wf 日志存放路径 (大于 Warning 级别的日志)
    optional string wf_log_path = 4 [default = "logs/log.wf"];
    // 保存小时数, -1 表示不进行日志切割
    optional int32 retain_hours = 5 [default = 48];
  };

  message ConsoleWriterConfig {
    // 是否将日志输出到控制台
    optional bool enable = 1 [default = true];
    // 日志级别, 只写入 >= LogLevel 的日志
    optional LogLevel log_level = 2 [default = LOG_LEVEL_INFO];
    // 是否开启彩色输出
    optional bool enable_color = 3 [default = true];
  }

  optional FileWriterConfig file_writer_config = 1;
  optional ConsoleWriterConfig console_writer_config = 2;
};
```

## Reference

[1] <https://github.com/shengkehua/xlog4go>
