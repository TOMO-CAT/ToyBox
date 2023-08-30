package httphandler

import (
	"net/http"

	"github.com/prometheus/client_golang/prometheus/promhttp"
)

func muxMetrics(m *http.ServeMux) {
	m.Handle("/metrics", promhttp.Handler())
}
