package app

import (
	"fmt"
	"os"
	"path"
	"syscall"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

func controlStartHandler(pidFileDir string, serviceName string) {
	if _, isDaemon := os.LookupEnv("DAEMON"); !isDaemon {
		if isPidFileExist(pidFileDir, serviceName) {
			fmt.Printf("[Error] please stop the DAEMON process before start it again, pid file [%s]\n",
				path.Join(pidFileDir, fmt.Sprintf(PID_FILE_FORMAT, serviceName)))
			os.Exit(1)
		}
	}

	childPid, _ := syscall.ForkExec(os.Args[0], os.Args, &syscall.ProcAttr{
		Env: append(os.Environ(), []string{"DAEMON=true"}...),
		Sys: &syscall.SysProcAttr{
			Setsid: true,
		},
		Files: []uintptr{0, 1, 2},
	})
	fmt.Printf("[Info] start service [%s] successfully! service with pid [%d] will run as daemon\n", serviceName, childPid)
	os.Exit(0)
}

func controlStopHandler(pidFileDir string, serviceName string) {
	if _, isDaemon := os.LookupEnv("DAEMON"); !isDaemon {
		process, runningPid, err := findServicePid(pidFileDir, serviceName)
		if err != nil {
			logger.Error("find service pid fail||err=%v", err)
			fmt.Printf("[Error] find service pid fail, err: %s\n", err.Error())
			return
		}
		logger.Info("receive stop signal, pid (%d) is going to exit", runningPid)
		fmt.Printf("[Info] stop pid [%d]\n", runningPid)
		process.Signal(os.Interrupt)
	}
}

func controlRestartHandler(pidFileDir string, serviceName string) {
	controlStopHandler(pidFileDir, serviceName)
	time.Sleep(1 * time.Second)
	controlStartHandler(pidFileDir, serviceName)
}
