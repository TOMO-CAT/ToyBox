package util

import (
	"os"
	"path"
	"runtime"
	"syscall"
)

// FileAbsPath 获取当前文件绝对路径
func FileAbsPath() string {
	_, file, _, _ := runtime.Caller(1)
	return file
}

// DirAbsPath 获取当前文件夹绝对路径
func DirAbsPath() string {
	_, file, _, _ := runtime.Caller(1)
	return path.Dir(file)
}

// GetFileINode 获取文件 inode
func GetFileINode(fi os.FileInfo) uint64 {
	fStat := fi.Sys().(*syscall.Stat_t)
	return fStat.Ino
}
