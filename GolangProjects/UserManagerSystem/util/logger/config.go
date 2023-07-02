package logger

import (
	"encoding/json"
	"errors"
	"io/ioutil"
)

// 输出到日志文件的配置
type FileWriterConf struct {
	On                 bool   `json:"On"`                 // 是否将日志写入文件
	LogLevel           string `json:"LogLevel"`           // 日志级别, 只写入 >= LogLevel 的日志
	LogPath            string `json:"LogPath"`            // 日志存放文件名
	RotateLogPath      string `json:"RotateLogPath"`      // 日志切割文件名
	ErrorLogPath       string `json:"ErrorLogPath"`       // Warning 以上日志存放文件名
	RotateErrorLogPath string `json:"RotateErrorLogPath"` // Warning 以上日志切割文件名
}

type ConsoleWriterConf struct {
	On       bool   `json:"On"`
	LogLevel string `json:"LogLevel"`
	Color    bool   `json:"Color"`
}

type LoggerConf struct {
	FileWriter    FileWriterConf    `json:"FileWriter"`
	ConsoleWriter ConsoleWriterConf `json:"ConsoleWriter"`
}

// InitLoggerWithConf 根据配置文件初始化日志
func InitLoggerWithConf(confFile string) (err error) {
	var conf LoggerConf

	var confContent []byte
	if confContent, err = ioutil.ReadFile(confFile); err != nil {
		return err
	}

	if err = json.Unmarshal(confContent, &conf); err != nil {
		return err
	}

	if conf.FileWriter.On {
		if len(conf.FileWriter.LogPath) > 0 {
			w := NewFileWriter()
			w.SetFileName(conf.FileWriter.LogPath)
			w.SetPathPattern(conf.FileWriter.RotateLogPath)
			w.SetLogLevelFloor(LogLevelDebug)
			if len(conf.FileWriter.ErrorLogPath) > 0 {
				w.SetLogLevelCeiling(LogLevelInfo)
			} else {
				w.SetLogLevelCeiling(LogLevelFatal)
			}
			Register(w)
		}

		if len(conf.FileWriter.ErrorLogPath) > 0 {
			w := NewFileWriter()
			w.SetFileName(conf.FileWriter.ErrorLogPath)
			w.SetPathPattern(conf.FileWriter.RotateErrorLogPath)
			w.SetLogLevelFloor(LogLevelWarn)
			w.SetLogLevelCeiling(LogLevelFatal)
			Register(w)
		}
	}

	if conf.ConsoleWriter.On {
		w := NewConsoleWriter()
		w.SetColor(conf.ConsoleWriter.Color)
		if consoleLogLevel, ok := string2logLevel[conf.ConsoleWriter.LogLevel]; !ok {
			err = errors.New("invalid log level: " + conf.FileWriter.LogLevel)
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
