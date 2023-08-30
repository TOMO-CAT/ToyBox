package util

import (
	"os"
	"syscall"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

const (
	stdioLogPath = "./log/stdio.log"
)

var stdioLogFileINode uint64

// RedirectStdioAndStderr 将 stdout 和 stderr 重定向到文件中
func RedirectStdioAndStderr() (err error) {
	stdioLogFileINode, err = redirect2file(stdioLogPath)
	if err != nil {
		return
	}
	go monitorStdioLogFile()
	return nil
}

func redirect2file(filePath string) (iNode uint64, err error) {
	var f *os.File
	f, err = os.OpenFile(filePath, os.O_WRONLY|os.O_CREATE|os.O_APPEND, 0666)
	defer func() {
		if err := f.Close(); err != nil {
			logger.Error("close file err: %v", err)
		}
	}()
	if err != nil {
		iNode = 0
		return
	}

	var fInfo os.FileInfo
	fInfo, err = f.Stat()
	if err != nil {
		iNode = 0
		return
	}

	iNode = GetFileINode(fInfo)
	syscall.Dup2(int(f.Fd()), 1)
	syscall.Dup2(int(f.Fd()), 2)
	return
}

// monitorStdioLogFile 监控重定向文件, 如果这个文件被破坏则重新创建文件
func monitorStdioLogFile() {
	ticker := time.NewTicker(10 * time.Second)
	for range ticker.C {
		fi, err := os.Stat(stdioLogPath)
		if stdioLogFileINode == 0 || (err == nil && GetFileINode(fi) != stdioLogFileINode) || os.IsNotExist(err) {
			logger.Info("redirect stdio and stderr with inode [%d]", stdioLogFileINode)
			stdioLogFileINode, err = redirect2file(stdioLogPath)
		}
		if err != nil {
			logger.Error("redirect stdio and stderr fail with err [%v] and inode [%d]", err, stdioLogFileINode)
		}
	}
}
