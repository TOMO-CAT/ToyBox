package grpcserver

import (
	"context"
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/monitor"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/server/common"
	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util"
	proto "github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/proto/service"
)

func (s *server) Echo(ctx context.Context, req *proto.EchoRequest) (res *proto.EchoResponse, err error) {
	monitor.ExportCounterInc(monitor.ComponentEcho, "request_count")
	start := time.Now()
	ctx = context.WithValue(ctx, common.ContextKeyTraceID, common.NewTraceId())
	common.RecordRequestInfo(ctx, req)
	defer func() {
		util.HandlePanic(ctx, "")
		common.RecordResponseInfo(ctx, res, err, start)
		monitor.ExportLatency(monitor.ComponentEcho, "total", start)
	}()

	return &proto.EchoResponse{
		TraceId: req.GetTraceId(),
		Message: req.GetMessage(),
	}, nil
}
