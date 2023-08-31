package util

import (
	"context"
	"runtime"
	"runtime/debug"
	"strings"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

func GetSrcMethod(skip int) string {
	if pc, _, _, ok := runtime.Caller(skip); ok {
		method := runtime.FuncForPC(pc).Name()
		if idx := strings.LastIndex(method, "."); idx != -1 {
			method = method[idx+1:]
		}
		return method
	}
	return ""
}

func HandlePanic(ctx context.Context, tag string) {
	if strings.TrimSpace(tag) == "" {
		tag = GetSrcMethod(2)
	}

	if err := recover(); err != nil {
		logger.Error("panic||trace=%v||err=%v||stack=%v||tag=%v", ctx.Value("trace_id"), err, string(debug.Stack()), tag)
	}
}
