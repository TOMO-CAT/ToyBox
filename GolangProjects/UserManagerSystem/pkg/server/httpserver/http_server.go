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

	errChan := make(chan error)
	go func() {
		<-ctx.Done()
		errChan <- server.Close()
	}()

	go func() {
		if err := server.ListenAndServe(); err != nil {
			if err != http.ErrServerClosed {
				errChan <- err
			}
		}
	}()

	return <-errChan
}
