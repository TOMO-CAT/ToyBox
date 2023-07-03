package main

import (
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/util"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/util/logger"
)

// 运行: go run util/logger/example/main.go
func main() {
	if err := logger.InitLoggerWithConf(util.DirAbsPath() + "/" + "logger.json"); err != nil {
		panic(err)
	}

	for {
		logger.Debug("%s log", "debug")
		logger.Info("%s log", "info")
		logger.Warn("%s log", "warn")
		logger.Error("%s log", "error")
		logger.Fatal("%s log", "fatal")

		time.Sleep(time.Second * 1)
	}

	// defer logger.Close()
}
