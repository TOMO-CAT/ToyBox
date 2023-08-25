

# 根据 *.proto 文件生成对应的 *.pb.go
.PHONY: proto
proto:
	protoc --go_out=. --go_opt=paths=source_relative --go-grpc_out=. --go-grpc_opt=paths=source_relative ./proto/config/logger_config.proto

.PHONY: build
build:
	go mod tidy
