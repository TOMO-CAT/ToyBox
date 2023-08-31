package common

import (
	"context"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
)

func RecordRequestInfo(ctx context.Context, req interface{}) {
	funcName := util.GetSrcMethod(2)
	logger.Info("request||func_name=%s||trace_id=%s||caller=%s||req=%s",
		funcName, ctx.Value(ContextKeyTraceID), ctx.Value(ContextKeyCaller), util.ToString(req))
}

func RecordResponseInfo(ctx context.Context, resp interface{}, err error, start time.Time) {
	funcName := util.GetSrcMethod(3)
	tUse := time.Since(start)

	if err != nil {
		logger.Error("response||func_name=%s||trace_id=%s||caller=%s||resp=%s||time_cost=%s||err=%v",
			funcName, ctx.Value(ContextKeyTraceID), ctx.Value(ContextKeyCaller), util.ToString(resp), tUse.String(), err)
	} else {
		logger.Info("response||func_name=%s||trace_id=%s||caller=%s||resp=%s||time_cost=%s",
			funcName, ctx.Value(ContextKeyTraceID), ctx.Value(ContextKeyCaller), util.ToString(resp), tUse.String())
	}
}
