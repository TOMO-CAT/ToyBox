package main

import (
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

// 运行: go run pkg/util/logger/example/main.go
func main() {
	if err := logger.InitLogger(util.DirAbsPath() + "/" + "logger.json"); err != nil {
		panic(err)
	}

	defer logger.Close()

	logger.Debug("%s log", "debug")
	logger.Info("%s log", "info")
	logger.Warn("%s log", "warn")
	logger.Error("%s log", "error")
	logger.Fatal("%s log", "fatal")
}
