package main

import (
	"fmt"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/proto/config"
	"google.golang.org/protobuf/encoding/protojson"
)

func main() {
	enableFileWriter := true
	fileWriterLogLevel := config.LoggerConfig_LOG_LEVEL_DEBUG
	infoLogPath := "./logs/example.info"
	wfLogPath := "./logs/example.wf"
	var retainHours int32 = 48

	enableConsoleWriter := true
	consoleWriterLogLevel := config.LoggerConfig_LOG_LEVEL_INFO
	enableColor := true

	msg := config.LoggerConfig{
		FileWriterConfig: &config.LoggerConfig_FileWriterConfig{
			Enable:      &enableFileWriter,
			LogLevel:    &fileWriterLogLevel,
			InfoLogPath: &infoLogPath,
			WfLogPath:   &wfLogPath,
			RetainHours: &retainHours,
		},
		ConsoleWriterConfig: &config.LoggerConfig_ConsoleWriterConfig{
			Enable:      &enableConsoleWriter,
			LogLevel:    &consoleWriterLogLevel,
			EnableColor: &enableColor,
		},
	}

	// // fmt.Println(msg.String())

	str, _ := protojson.Marshal(&msg)
	fmt.Println(string(str))
	// fmt.Println(string(marshalText))

	// marshalText := protojson.Format(&msg)
	// fmt.Println(marshalText)
	// bs, _ := protojson.Marshal(&msg)
	// fmt.Println(string(bs))

	// var data config.LoggerConfig
	// protojson.Unmarshal(bs, &data)
	// fmt.Println(data.String())

	// msg := config.LoggerConfig{}
	// fmt.Println(protojson.Marshal(&msg))

	// var msg config.LoggerConfig = config.LoggerConfig{
	// 	FileWriterConfig:    &config.LoggerConfig_FileWriterConfig{},
	// 	ConsoleWriterConfig: &config.LoggerConfig_ConsoleWriterConfig{},
	// }
	// fmt.Println(msg.FileWriterConfig.GetInfoLogPath())
	// // msgData := "{}"
	// // protojson.Unmarshal([]byte(msgData), &msg)

	// // fmt.Println(msg.String())
	// fmt.Println(protojson.Format(&msg))
}
