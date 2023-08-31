package monitor

import (
	"time"

	"github.com/TOMO-CAT/ToyBox/GolangProjects/UserManagerSystem/pkg/util"
	"github.com/prometheus/client_golang/prometheus"
)

const (
	kNamespace  = "tomocat"
	kSubsystem  = "ums"
	kTimeWindow = 5 * time.Minute
)

var (
	latencyExporter = prometheus.NewSummaryVec(
		prometheus.SummaryOpts{
			Namespace:  kNamespace,
			Subsystem:  kSubsystem,
			Name:       "latency",
			Help:       "latency summary milliseconds",
			MaxAge:     kTimeWindow,
			Objectives: map[float64]float64{0.5: 0.05, 0.9: 0.01, 0.99: 0.001},
		},
		[]string{"component", "type"},
	)

	errorExporter = prometheus.NewCounterVec(
		prometheus.CounterOpts{
			Namespace: kNamespace,
			Subsystem: kSubsystem,
			Name:      "error",
			Help:      "error counter",
		},
		[]string{"component", "type"},
	)

	counterExporter = prometheus.NewCounterVec(
		prometheus.CounterOpts{
			Namespace: kNamespace,
			Subsystem: kSubsystem,
			Name:      "counter",
			Help:      "counter",
		},
		[]string{"component", "type"},
	)
)

func init() {
	prometheus.MustRegister(
		latencyExporter,
		errorExporter,
		counterExporter,
	)
}

func ExportLatency(component, latencyType string, tStart time.Time) {
	latencyExporter.WithLabelValues(component, latencyType).Observe(util.MsSince(tStart))
}

func ExportError(component, errorType string) {
	errorExporter.WithLabelValues(component, errorType).Inc()
}

func ExportCounterInc(component, counterType string) {
	counterExporter.WithLabelValues(component, counterType).Inc()
}

func ExportCounterWithValue(component, counterType string, count int64) {
	counterExporter.WithLabelValues(component, counterType).Add(float64(count))
}
