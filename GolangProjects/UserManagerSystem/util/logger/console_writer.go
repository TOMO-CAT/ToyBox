package logger

import (
	"fmt"
	"os"
)

type colorRecord Record

func (cr *colorRecord) String() string {
	switch cr.logLevel {
	case LogLevelDebug:
		return fmt.Sprintf("[\033[36m%s\033[0m] [\033[34m%s\033[0m] [\033[47;30m%s\033[0m] %s\n",
			cr.timestamp, logLevel2String[cr.logLevel], cr.codeLine, cr.message)
	case LogLevelInfo:
		return fmt.Sprintf("[\033[36m%s\033[0m] [\033[32m%s\033[0m] [\033[47;30m%s\033[0m] %s\n",
			cr.timestamp, logLevel2String[cr.logLevel], cr.codeLine, cr.message)
	case LogLevelWarn:
		return fmt.Sprintf("[\033[36m%s\033[0m] [\033[33m%s\033[0m] [\033[47;30m%s\033[0m] %s\n",
			cr.timestamp, logLevel2String[cr.logLevel], cr.codeLine, cr.message)
	case LogLevelError:
		return fmt.Sprintf("[\033[36m%s\033[0m] [\033[31m%s\033[0m] [\033[47;30m%s\033[0m] %s\n",
			cr.timestamp, logLevel2String[cr.logLevel], cr.codeLine, cr.message)
	case LogLevelFatal:
		return fmt.Sprintf("[\033[36m%s\033[0m] [\033[35m%s\033[0m] [\033[47;30m%s\033[0m] %s\n",
			cr.timestamp, logLevel2String[cr.logLevel], cr.codeLine, cr.message)
	}
	return fmt.Sprintf("invalid log level, cr:%v", *cr)
}

type ConsoleWriter struct {
	color bool
	level int
}

func NewConsoleWriter() *ConsoleWriter {
	return &ConsoleWriter{}
}

func (cw *ConsoleWriter) Write(r *Record) error {
	if r.logLevel < cw.level {
		return nil
	}

	if cw.color {
		fmt.Fprint(os.Stdout, ((*colorRecord)(r)).String())
	} else {
		fmt.Fprint(os.Stdout, r.String())
	}
	return nil
}

func (cw *ConsoleWriter) Init() error {
	return nil
}

func (cw *ConsoleWriter) SetColor(c bool) {
	cw.color = c
}

func (cw *ConsoleWriter) SetLevel(l int) {
	cw.level = l
}
