package app

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"path/filepath"
	"runtime/debug"
	"sort"
	"strings"
	"sync"
	"syscall"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
	cli "github.com/urfave/cli/v2"
)

type App struct {
	Name        string
	Usage       string
	PrepareFunc cli.BeforeFunc
	RunFunc     func(map[string]interface{}, context.Context, chan error, *sync.WaitGroup) error

	// exit gracefully
	wg      sync.WaitGroup
	errChan chan error
}

var commonFlags = []cli.Flag{
	// &cli.StringFlag{
	// 	Name:    "config",
	// 	Aliases: []string{"c"},
	// 	Usage:   "config file path",
	// 	Value:   "conf/config.toml",
	// },
	// &cli.StringFlag{
	// 	Name:  "log-conf",
	// 	Usage: "log config file path",
	// 	Value: "conf/logger.json",
	// },
	&cli.BoolFlag{
		Name:    "help",
		Aliases: []string{"h"},
		Usage:   "print help message",
	},
	&cli.BoolFlag{
		Name:    "info",
		Aliases: []string{"i"},
		Usage:   "print build info",
	},
	&cli.StringFlag{
		Name:  "control",
		Usage: "send command to control the running service [start | stop | restart]",
	},
	&cli.StringFlag{
		Name:  "pid-dir",
		Usage: "where to put the pid file",
		Value: ".",
	},
}

func (a *App) StartService(customFlags ...cli.Flag) error {
	a.errChan = make(chan error, 1)

	flags := append(commonFlags, customFlags...)
	sort.Sort(cli.FlagsByName(flags))

	app := cli.App{
		Name:        a.Name,
		Usage:       a.Usage,
		Version:     fmt.Sprintf("%s - %s [%s]", Version, Commit, Branch),
		HideHelp:    true, // 覆盖 cli 包原生的 help 命令行参数
		HideVersion: true, // 覆盖 cli 包原生的 version 命令行参数
		Flags:       flags,
		Before: func(ctx *cli.Context) error {
			if a.PrepareFunc != nil {
				return a.PrepareFunc(ctx)
			}
			return nil
		},
		Action: a.runFuncWrapper(),
	}

	if err := app.Run(os.Args); err != nil {
		logger.Error("run service fail with err [%v]", err)
		return err
	}

	return nil
}

func (a *App) runFuncWrapper() cli.ActionFunc {
	return func(c *cli.Context) error {
		if c.Bool("help") {
			return cli.ShowAppHelp(c)
		}

		if c.Bool("info") {
			printBuildInfo()
			return nil
		}

		// 初始化日志
		if loggerConfAbsPath, err := filepath.Abs(c.String("log-conf")); err != nil {
			fmt.Printf("invalid logger conf [%s]\n", c.String("log-conf"))
			return fmt.Errorf("invalid logger conf [%s]", c.String("log-conf"))
		} else if !util.IsFileExist(loggerConfAbsPath) {
			fmt.Printf("logger conf [%s] don't exist\n", loggerConfAbsPath)
			return fmt.Errorf("logger conf [%s] don't exist", loggerConfAbsPath)
		} else {
			if err = logger.InitLogger(loggerConfAbsPath); err != nil {
				fmt.Printf("init logger fail with error [%s]\n", err)
				return fmt.Errorf("init logger fail with error [%s]", err)
			}
		}
		// 保证可以打印所有的异步日志
		defer func() {
			logger.Close()
		}()

		// // 检查配置文件是否存在
		// if confAbsPath, err := filepath.Abs(c.String("config")); err != nil {

		// }

		// if !util.IsFileExist(c.String("config")) {
		// 	logger.Error("invalid config path||path=%s", c.String("config"))
		// 	return cli.ShowAppHelp(c)
		// }

		pidFileDir := c.String("pid-dir")
		if strings.TrimSpace(pidFileDir) == "" {
			pidFileDir, _ = os.Getwd()
		}

		// 处理启停控制指令 (-c start | stop | restart )
		controlCmd := c.String("control")
		if controlCmd != "" {
			switch controlCmd {
			case "stop":
				controlStopHandler(pidFileDir, a.Name)
				return nil
			case "start":
				controlStartHandler(pidFileDir, a.Name)
			case "restart":
				controlRestartHandler(pidFileDir, a.Name)
			default:
				fmt.Printf("[Error] unsupported control command [%s]\n", controlCmd)
				return cli.ShowAppHelp(c)
			}
		}

		// 处理 pid 文件
		if _, isDaemon := os.LookupEnv("DAEMON"); isDaemon {
			if controlCmd == "start" || controlCmd == "restart" {
				logger.Info("daemon process [%d] start running in the background", os.Getpid())
				if err := writeServicePid(pidFileDir, a.Name); err != nil {
					logger.Error("write pid to file fail with err [%v]", err)
					fmt.Printf("[Error] write pid to file fail with err [%v]\n", err)
					syscall.Exit(1)
				}
				defer deletePidFile(pidFileDir, a.Name)

				// 以后台方式运行的时候重定向 stdout 和 stderr 到日志中, 避免丢失控制台日志
				if err := util.RedirectStdioAndStderr(); err != nil {
					logger.Error("redirect stdout && stderr fail with err:%v", err)
				}
			}
		}

		ctx, cancel := context.WithCancel(context.Background())
		defer cancel()

		// 运行主逻辑
		a.wg.Add(1)
		// 获取所有的 flag 透传出去给用户 run 方法
		var flagMap = make(map[string]interface{})
		for _, name := range c.FlagNames() {
			flagMap[name] = c.Generic(name)
		}
		go func() {
			defer func() {
				defer a.wg.Done()
				if err := recover(); err != nil {
					logger.Error("panic with err [%v], stack:\n%s", err, string(debug.Stack()))
					a.errChan <- fmt.Errorf("panic with err [%v]", err)
				}
			}()

			logger.Info("app [%s] start", a.Name)
			err := a.RunFunc(flagMap, ctx, a.errChan, &a.wg)
			a.errChan <- err
			logger.Info("app [%s] quit with err: %v", a.Name, err)
		}()

		// 优雅退出
		a.wait(cancel)

		return nil
	}
}

func (a *App) wait(cancel context.CancelFunc) {
	// 一直阻塞直到接收到信号或者抛出错误
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGQUIT, syscall.SIGTERM)
	select {
	case sig := <-sigChan:
		logger.Info("receive signal [%d], process [%d] quit", sig, os.Getpid())
	case err := <-a.errChan:
		if err != nil {
			logger.Error("service exit with err [%s]", err)
		}
	}

	// 等待两秒以实现优雅退出, 如果还是不能退出则强制退出
	cancel()
	waitCtx, waitCancel := context.WithTimeout(context.Background(), time.Second*2)
	defer waitCancel()

	isWgDone := make(chan struct{})
	go func() {
		a.wg.Wait()
		close(isWgDone)
	}()

	select {
	case <-waitCtx.Done():
		fmt.Println("force quit!")
		logger.Error("force quit!")
	case <-isWgDone:
		return
	}
}
