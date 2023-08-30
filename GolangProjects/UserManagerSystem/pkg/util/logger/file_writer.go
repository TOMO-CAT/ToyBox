package logger

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"path"
	"strconv"
	"time"
)

type FileWriter struct {
	logLevelFloor   int
	logLevelCeiling int

	fileName       string
	file           *os.File
	fileBuffWriter *bufio.Writer

	currentFileSuffix int   // 当前日志文件后缀 `YYYYmmddHH`, 用于日志按小时切割
	retainHours       int   // 日志保留小时数
	historyFileSuffix []int // 当前所有历史日志文件后缀 `YYYYmmddHH` 的集合, 用于过期日志删除
}

func NewFileWriter() *FileWriter {
	return &FileWriter{}
}

func (fa *FileWriter) Init() error {
	now := time.Now()
	fa.currentFileSuffix = genCurrentFileSuffix(&now)
	return fa.CrateFile()
}

func (fa *FileWriter) SetFileName(fileName string) {
	fa.fileName = fileName
}

func (fa *FileWriter) SetLogLevelFloor(logLevel int) {
	fa.logLevelFloor = logLevel
}

func (fa *FileWriter) SetLogLevelCeiling(logLevel int) {
	fa.logLevelCeiling = logLevel
}

func (fa *FileWriter) SetRetainHours(retainHours int) {
	fa.retainHours = retainHours
}

func (fa *FileWriter) Write(r *Record) error {
	if r.logLevel < fa.logLevelFloor || r.logLevel > fa.logLevelCeiling {
		return nil
	}

	if fa.fileBuffWriter == nil {
		return errors.New("file doesn't open")
	}

	if _, err := fa.fileBuffWriter.WriteString(r.String()); err != nil {
		return err
	}

	return nil
}

func (fa *FileWriter) CrateFile() error {
	// log.Println("create log file: ", fa.fileName)
	if err := os.MkdirAll(path.Dir(fa.fileName), 0755); err != nil {
		if !os.IsExist(err) {
			return err
		}
	}

	if file, err := os.OpenFile(fa.fileName, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0644); err != nil {
		return err
	} else {
		fa.file = file
	}

	if fa.fileBuffWriter = bufio.NewWriterSize(fa.file, 8192); fa.fileBuffWriter == nil {
		return errors.New("NewWriterSize fail")
	}

	return nil
}

func (fa *FileWriter) Flush() error {
	return fa.fileBuffWriter.Flush()
}

func (fa *FileWriter) Rotate() error {
	var (
		now           = time.Now()
		isRotate      = false
		curFileSuffix = genCurrentFileSuffix(&now)
	)

	// 进入新的小时, 准备日期切割
	// log.Println("curFileSuffix: ", curFileSuffix)
	// log.Println("currentFileSuffix: ", fa.currentFileSuffix)
	if curFileSuffix != fa.currentFileSuffix {
		isRotate = true
	}

	if !isRotate {
		return nil
	}

	if fa.fileBuffWriter != nil {
		if err := fa.Flush(); err != nil {
			return err
		}
	}

	if fa.file != nil {
		oldFileName := fa.genLogFileName(fa.currentFileSuffix)
		if err := os.Rename(fa.fileName, oldFileName); err != nil {
			return err
		}
		fa.historyFileSuffix = append(fa.historyFileSuffix, fa.currentFileSuffix)
		fa.currentFileSuffix = curFileSuffix
	}

	if err := fa.file.Close(); err != nil {
		return err
	}

	if err := fa.CrateFile(); err != nil {
		return err
	}

	// 如果历史日志文件个数超过 RetainHours, 那么清理过期日志
	if len(fa.historyFileSuffix) > fa.retainHours {
		overdueFileName := fa.genLogFileName(fa.historyFileSuffix[0])
		// log.Println("delete file: ", overdueFileName)
		if err := os.Remove(overdueFileName); err != nil {
			return err
		}
		fa.historyFileSuffix = fa.historyFileSuffix[1:]
	}

	return nil
}

func (fa *FileWriter) genLogFileName(hourSuffix int) string {
	return fmt.Sprintf("%s.%d", fa.fileName, hourSuffix)
}

func genCurrentFileSuffix(t *time.Time) int {
	suffixStr := t.Format("2006010215")
	suffixInt, err := strconv.ParseInt(suffixStr, 10, 64)
	if err != nil {
		panic(err)
	}
	return int(suffixInt)
}

func init() {
}
