package httpserver

import (
	"context"
	"fmt"
	"net/http"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/httphandler"
)

func Start(ctx context.Context, port int) error {
	server := http.Server{
		Addr:    fmt.Sprintf("0.0.0.0:%d", port),
		Handler: httphandler.NewCommonMux(),
	}

	// 接收到 cancel 信号时停止 http server
	errChan := make(chan error)
	go func() {
		<-ctx.Done()
		errChan <- server.Close()
	}()

	// http server 报错时退出
	go func() {
		if err := server.ListenAndServe(); err != nil {
			if err != http.ErrServerClosed {
				errChan <- err
			}
		}
	}()

	// 阻塞在接收 errChan 信号上, 直到上游 cannel 或者 http server 报错
	return <-errChan
}
