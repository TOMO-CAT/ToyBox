package app

import (
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"strconv"
	"syscall"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

const (
	PID_FILE_FORMAT = "%s.pid"
)

func isPidFileExist(dir, serviceName string) bool {
	filePath := path.Join(dir, fmt.Sprintf(PID_FILE_FORMAT, serviceName))
	if _, err := os.Stat(filePath); os.IsNotExist(err) {
		return false
	}
	return true
}

// findServicePid 返回当前服务对应的 pid
// @return ret
//   -1: 表示 pid 文件不存在
//   -2: 表示文件存在但是读取有错误
//   -3: pid 对应的进程不存在
//   >0: 服务对应的进程 id
func findServicePid(dir, serviceName string) (process *os.Process, ret int, err error) {
	filePath := path.Join(dir, fmt.Sprintf(PID_FILE_FORMAT, serviceName))
	if _, err = os.Stat(filePath); os.IsNotExist(err) {
		ret = -1
		err = fmt.Errorf("pid file [%s] donot exist", filePath)
		return
	}

	bytes, err := ioutil.ReadFile(filePath)
	if err != nil {
		ret = -2
		return
	}

	originalPid, err := strconv.Atoi(string(bytes))
	if err != nil {
		ret = -2
		return
	}

	process, _ = os.FindProcess(originalPid)
	if process.Signal(syscall.Signal(0)) == nil {
		ret = originalPid
		return
	}

	err = fmt.Errorf("pid file [%s] exist but the pid [%d] not exist", filePath, originalPid)
	process = nil
	ret = -3
	return
}

func writeServicePid(dir, serviceName string) error {
	filePath := path.Join(dir, fmt.Sprintf(PID_FILE_FORMAT, serviceName))

	// 只有 pid file 存在且 pid 不存在时才能覆盖
	_, pid, err := findServicePid(dir, serviceName)
	switch pid {
	case -1:
	case -2:
		return err
	case -3:
	default:
		return fmt.Errorf("pid file [%s] exist with pid [%d]", filePath, pid)
	}

	var f *os.File
	// O_RDWR: open the file read-write
	// O_CREATE: create a new file if none exists
	// O_TRUNC: truncate regular writable file when opened
	f, err = os.OpenFile(filePath, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return err
	}

	defer f.Close()
	if _, err := f.WriteString(fmt.Sprint(os.Getpid())); err != nil {
		return err
	}

	return nil
}

func deletePidFile(dir, serviceName string) {
	logger.Info("delete pid file")
	filePath := path.Join(dir, fmt.Sprintf(PID_FILE_FORMAT, serviceName))
	os.Remove(filePath)
}
