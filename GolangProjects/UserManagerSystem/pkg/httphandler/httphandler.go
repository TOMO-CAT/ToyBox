package httphandler

import (
	"net/http"
)

func NewCommonMux() *http.ServeMux {
	m := http.NewServeMux()

	muxMetrics(m) // 监控打点
	muxPing(m)    // 打印服务信息
	muxPProf(m)   // 性能分析

	return m
}
