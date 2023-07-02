package logger

import (
	"fmt"
	"log"
	"path"
	"runtime"
	"strconv"
	"strings"
	"sync"
	"time"
)

var (
	logLevel2String = [...]string{"DEBUG", "INFO", "WARN", "ERROR", "FATAL"}
	string2logLevel = map[string]int{"DEBUG": LogLevelDebug, "INFO": LogLevelInfo, "WARN": LogLevelWarn, "ERROR": LogLevelError, "FATAL": LogLevelFatal}
	recordPool      *sync.Pool
)

const (
	LogLevelDebug = iota
	LogLevelInfo
	LogLevelWarn
	LogLevelError
	LogLevelFatal
)

// Record 单行日志
type Record struct {
	timestamp string
	codeLine  string
	message   string
	logLevel  int
}

func (r *Record) String() string {
	return fmt.Sprintf("[%s] [%s] [%s] %s\n", logLevel2String[r.logLevel], r.timestamp, r.codeLine, r.message)
}

type Writer interface {
	Init() error
	Write(*Record) error
}

type Rotater interface {
	Rotate() error
	SetPathPattern(string) error
}

type Flusher interface {
	Flush() error
}

type Logger struct {
	writers    []Writer
	recordChan chan *Record
	level      int
	c          chan bool
	layout     string
	// levelFunc  func(int) int
}

var (
	defaultLogger *Logger
	takeup        = false
)

func NewLogger() *Logger {
	if defaultLogger != nil && !takeup {
		takeup = true
		return defaultLogger
	}

	l := new(Logger)
	l.writers = make([]Writer, 0, 2)
	l.recordChan = make(chan *Record, 1024)
	l.c = make(chan bool, 1)
	l.level = LogLevelDebug
	l.layout = "2006-01-02T15:04:05.000+0800"

	go boostrapLogWriter(l)

	return l
}

func boostrapLogWriter(logger *Logger) {
	if logger == nil {
		panic("nil logger")
	}

	var (
		r  *Record
		ok bool
	)

	// check first record
	if r, ok = <-logger.recordChan; !ok {
		logger.c <- true
		return
	}

	for _, w := range logger.writers {
		if err := w.Write(r); err != nil {
			log.Println(err)
		}
	}

	flushTimer := time.NewTimer(time.Millisecond * 500)
	rotateTimer := time.NewTimer(time.Second * 10)

	for {
		select {
		case r, ok = <-logger.recordChan:
			if !ok {
				logger.c <- true
				return
			}

			for _, w := range logger.writers {
				if err := w.Write(r); err != nil {
					log.Println(err)
				}
			}

			recordPool.Put(r)
		case <-flushTimer.C:
			for _, w := range logger.writers {
				var f Flusher
				if f, ok = w.(Flusher); ok {
					if err := f.Flush(); err != nil {
						log.Println(err)
					}
				}
			}
			flushTimer.Reset(time.Millisecond * 1000)
		case <-rotateTimer.C:
			for _, w := range logger.writers {
				var r Rotater
				if r, ok = w.(Rotater); ok {
					if err := r.Rotate(); err != nil {
						log.Println(err)
					}
				}
			}
			rotateTimer.Reset(time.Second * 10)
		}
	}
}

func SetLevel(level int) {
	defaultLogger.level = level
}

func SetLayout(layout string) {
	defaultLogger.layout = layout
}

func (l *Logger) deliverRecord2Writer(level int, format string, args ...interface{}) {
	if level < l.level {
		return
	}

	var (
		logMessage  string
		codeMessage string
	)

	if strings.TrimSpace(format) != "" {
		logMessage = fmt.Sprintf(format, args...)
	} else {
		logMessage = fmt.Sprint(args...)
	}

	// source code, file and line number
	_, file, line, ok := runtime.Caller(2)
	if ok {
		codeMessage = path.Base(file) + ":" + strconv.Itoa(line)
	}

	r := recordPool.Get().(*Record)
	r.message = logMessage
	r.codeLine = codeMessage
	r.timestamp = time.Now().Format(l.layout)
	r.logLevel = level

	l.recordChan <- r
}

func (l *Logger) RegisterWriter(w Writer) {
	if err := w.Init(); err != nil {
		panic(err)
	}
	l.writers = append(l.writers, w)
}

func (l *Logger) Close() {
	close(l.recordChan)
	<-l.c

	for _, w := range l.writers {
		if f, ok := w.(Flusher); ok {
			if err := f.Flush(); err != nil {
				log.Println(err)
			}
		}
	}
}

func Debug(fmt string, args ...interface{}) {
	defaultLogger.deliverRecord2Writer(LogLevelDebug, fmt, args...)
}

func Info(fmt string, args ...interface{}) {
	defaultLogger.deliverRecord2Writer(LogLevelInfo, fmt, args...)
}

func Warn(fmt string, args ...interface{}) {
	defaultLogger.deliverRecord2Writer(LogLevelWarn, fmt, args...)
}

func Error(fmt string, args ...interface{}) {
	defaultLogger.deliverRecord2Writer(LogLevelError, fmt, args...)
}

func Fatal(fmt string, args ...interface{}) {
	defaultLogger.deliverRecord2Writer(LogLevelFatal, fmt, args...)
}

func Register(w Writer) {
	defaultLogger.RegisterWriter(w)
}

func Close() {
	defaultLogger.Close()
}

func init() {
	defaultLogger = NewLogger()
	recordPool = &sync.Pool{New: func() interface{} {
		return &Record{}
	}}
}
