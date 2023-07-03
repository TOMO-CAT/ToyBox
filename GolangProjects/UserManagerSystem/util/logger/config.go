package logger

import (
	"encoding/json"
	"errors"
	"io/ioutil"
)

// 输出到日志文件的配置
type FileWriterConf struct {
	Enable       bool   `json:"Enable"`       // 是否将日志写入文件
	LogLevel     string `json:"LogLevel"`     // 日志级别, 只写入 >= LogLevel 的日志
	LogPath      string `json:"LogPath"`      // 日志存放文件名
	ErrorLogPath string `json:"ErrorLogPath"` // Warning 以上日志存放文件名
}

// 输出到控制台的配置
type ConsoleWriterConf struct {
	Enable      bool   `json:"Enable"`      // 是否将日志输出到控制台
	LogLevel    string `json:"LogLevel"`    // 日志级别, 只写入 >= LogLevel 的日志
	EnableColor bool   `json:"EnableColor"` // 终端输出是否彩色
}

type LoggerConf struct {
	FileWriter    FileWriterConf    `json:"FileWriter"`
	ConsoleWriter ConsoleWriterConf `json:"ConsoleWriter"`
}

// InitLoggerWithConf 根据配置文件初始化日志
func InitLoggerWithConf(confFile string) (err error) {
	var conf LoggerConf

	// 1. 从文件中读取配置
	var confContent []byte
	if confContent, err = ioutil.ReadFile(confFile); err != nil {
		return err
	}

	if err = json.Unmarshal(confContent, &conf); err != nil {
		return err
	}

	// 2. 解析配置
	if conf.FileWriter.Enable {
		// 2.1 INFO 日志
		if len(conf.FileWriter.LogPath) > 0 {
			w := NewFileWriter()
			w.SetFileName(conf.FileWriter.LogPath)
			// w.SetPathPattern(conf.FileWriter.RotateLogPath)
			w.SetLogLevelFloor(LogLevelDebug)
			if len(conf.FileWriter.ErrorLogPath) > 0 {
				w.SetLogLevelCeiling(LogLevelInfo)
			} else {
				w.SetLogLevelCeiling(LogLevelFatal)
			}
			w.SetRetainHours(5)
			Register(w)
		}

		// 2.2 WF 日志
		if len(conf.FileWriter.ErrorLogPath) > 0 {
			w := NewFileWriter()
			w.SetFileName(conf.FileWriter.ErrorLogPath)
			// w.SetPathPattern(conf.FileWriter.RotateErrorLogPath)
			w.SetLogLevelFloor(LogLevelWarn)
			w.SetLogLevelCeiling(LogLevelFatal)
			w.SetRetainHours(10)
			Register(w)
		}
	}

	if conf.ConsoleWriter.Enable {
		w := NewConsoleWriter()
		w.SetColor(conf.ConsoleWriter.EnableColor)
		if consoleLogLevel, ok := string2logLevel[conf.ConsoleWriter.LogLevel]; !ok {
			err = errors.New("invalid log level: " + conf.ConsoleWriter.LogLevel)
			return
		} else {
			w.SetLevel(consoleLogLevel)
		}
		Register(w)
	}

	if fileLogLevel, ok := string2logLevel[conf.FileWriter.LogLevel]; !ok {
		err = errors.New("invalid log level: " + conf.FileWriter.LogLevel)
		return
	} else {
		SetLevel(fileLogLevel)
	}

	return
}
