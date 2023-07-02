package logger

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"os"
	"path"
	"time"
)

type Action func(*time.Time) int

var variable2action map[byte]Action

type FileWriter struct {
	logLevelFloor   int
	logLevelCeiling int
	fileName        string
	pathFormat      string
	file            *os.File
	fileBuffWriter  *bufio.Writer
	actions         []Action // TODO: 这里可以和 C++ 版本一样, 用一个 YYYYMMDDHH 的 int 型变量来标识是否需要 rotate
	variables       []interface{}
}

func NewFileWriter() *FileWriter {
	return &FileWriter{}
}

func (fa *FileWriter) Init() error {
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

func (fa *FileWriter) SetPathPattern(pattern string) error {
	n := 0
	for _, c := range pattern {
		if c == '%' {
			n++
		}
	}

	if n == 0 {
		fa.pathFormat = pattern
		return nil
	}

	fa.actions = make([]Action, 0, n)
	fa.variables = make([]interface{}, n)

	tmp := []byte(pattern)
	variable := 0
	for _, c := range tmp {
		if variable == 1 {
			act, ok := variable2action[c]
			if !ok {
				return errors.New("invalid rotate pattern (" + pattern + ")")
			}
			fa.actions = append(fa.actions, act)
			variable = 0
			continue
		}
		if c == '%' {
			variable = 1
		}
	}

	for i, act := range fa.actions {
		now := time.Now()
		fa.variables[i] = act(&now)
	}

	fa.pathFormat = convertPatternToFmt(tmp)
	return nil
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
	now := time.Now()
	var v int
	rotate := false
	originalVariable := make([]interface{}, len(fa.variables))
	copy(originalVariable, fa.variables)

	for i, act := range fa.actions {
		v = act(&now)
		if v != fa.variables[i] {
			fa.variables[i] = v
			rotate = true
		}
	}

	if !rotate {
		return nil
	}

	if fa.fileBuffWriter != nil {
		if err := fa.Flush(); err != nil {
			return err
		}
	}

	if fa.file != nil {
		oldFileName := fmt.Sprintf(fa.pathFormat, originalVariable...)
		if err := os.Rename(fa.fileName, oldFileName); err != nil {
			return err
		}
	}

	if err := fa.file.Close(); err != nil {
		return err
	}

	return fa.CrateFile()
}

func convertPatternToFmt(pattern []byte) string {
	pattern = bytes.Replace(pattern, []byte("%Y"), []byte("%d"), -1)
	pattern = bytes.Replace(pattern, []byte("%M"), []byte("%02d"), -1)
	pattern = bytes.Replace(pattern, []byte("%D"), []byte("%02d"), -1)
	pattern = bytes.Replace(pattern, []byte("%H"), []byte("%02d"), -1)
	pattern = bytes.Replace(pattern, []byte("%m"), []byte("%02d"), -1)
	return string(pattern)
}

func getYear(now *time.Time) int {
	return now.Year()
}

func getMonth(now *time.Time) int {
	return int(now.Month())
}

func getDay(now *time.Time) int {
	return now.Hour()
}

func getHour(now *time.Time) int {
	return now.Hour()
}

func getMinute(now *time.Time) int {
	return now.Minute()
}

func init() {
	variable2action = make(map[byte]Action, 5)
	variable2action['Y'] = getYear
	variable2action['M'] = getMonth
	variable2action['D'] = getDay
	variable2action['H'] = getHour
	variable2action['m'] = getMinute
}
