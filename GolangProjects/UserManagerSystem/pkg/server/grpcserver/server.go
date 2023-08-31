package grpcserver

import (
	"context"
	"fmt"
	"net"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util/logger"
	proto "github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/proto/service"
	"google.golang.org/grpc"
)

type server struct {
	proto.UnimplementedUmsServiceServer
}

func NewServer() *server {
	return &server{}
}

func (s *server) Start(ctx context.Context, port int, grpcOpts ...grpc.ServerOption) error {
	svc := grpc.NewServer(grpcOpts...)
	proto.RegisterUmsServiceServer(svc, s)
	listen, err := net.Listen("tcp", fmt.Sprintf(":%d", port))
	if err != nil {
		logger.Fatal("listen port [%d] fail with err [%v]", port, err)
		return err
	}

	// 接受到上游 cancel 信号时尝试优雅退出
	go func() {
		<-ctx.Done()
		logger.Info("try to stop grpc server gracefully...")
		svc.GracefulStop()
		logger.Info("stop grpc server gracefully done!")
		_ = listen.Close()
	}()

	// 打印 grpc server 是否正常退出
	defer func() {
		if err != nil {
			logger.Error("grpc server quit with err [%v]", err)
		} else {
			logger.Info("grpc server quit successfully!")
		}
	}()

	err = svc.Serve(listen)
	return err
}
