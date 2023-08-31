package main

import (
	"context"
	"fmt"
	"sync"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/server/httpserver"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/app"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
	"github.com/urfave/cli/v2"
)

const (
	kDefaultLoggerPath = "conf/logger.json"
)

func main() {
	umsApp := app.App{
		Name:    "ums",
		Usage:   "user managerment system",
		RunFunc: run,
	}
	umsApp.StartService(
		&cli.StringFlag{
			Name:  "conf",
			Usage: "config file path",
			Value: "conf/config.toml",
		}, &cli.StringFlag{
			Name:  "log-conf",
			Usage: "log config file path",
			Value: "conf/logger.json",
		},
	)
}

func run(flags map[string]interface{}, ctx context.Context, errChan chan error, appWg *sync.WaitGroup) error {
	// parse config
	// if err := config.ParseConfig(configPath); err != nil {
	// 	logger.Error("init config fail with err:%v", err)
	// 	return err
	// }

	// 初始化日志
	var loggerConfPath string = kDefaultLoggerPath
	if val, exists := flags["log-conf"]; exists {
		loggerConfPath = fmt.Sprintf("%v", val)
	}
	if err := logger.InitLogger(loggerConfPath); err != nil {
		fmt.Printf("init logger [%s] fail with err [%v]\n", loggerConfPath, err)
		return fmt.Errorf("init logger [%s] fail with err [%v]", loggerConfPath, err)
	}

	// 保证可以打印所有的异步日志
	defer func() {
		logger.Close()
	}()

	// metric && pprof http service
	httpPort := 3366
	appWg.Add(1)
	go func() {
		defer appWg.Done()
		logger.Info("start metric && pprof server with port: %d", httpPort)
		if err := httpserver.Start(ctx, httpPort); err != nil {
			errChan <- fmt.Errorf("http server closed with err:%v", err)
		} else {
			logger.Info("http server shutdown")
		}
	}()

	// start grpc server and block here
	// logger.Info("start grpc server with port: %d", config.GlobalCfg.Port.GrpcPort)
	// return grpcserver.NewServer().Serve(ctx, config.GlobalCfg.Port.GrpcPort)

	// 这里应该实现一个阻塞操作
	time.Sleep(60 * time.Minute)

	return nil
}
