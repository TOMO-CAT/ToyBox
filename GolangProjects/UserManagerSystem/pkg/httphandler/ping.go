package httphandler

import (
	"encoding/json"
	"net/http"
	"os"
	"time"
)

var (
	tStart   = time.Now()
	hostName = func() string {
		hn, _ := os.Hostname()
		return hn
	}()
)

func ping(w http.ResponseWriter, r *http.Request) {
	serviceInfo := map[string]interface{}{
		"host_name":  hostName,
		"start_time": tStart.Format("2006-01-02 15:04:05"),
		"uptime":     time.Since(tStart).String(),
	}
	bytes, _ := json.MarshalIndent(serviceInfo, "", "  ")
	w.Write(bytes)
}

func muxPing(m *http.ServeMux) {
	m.HandleFunc("/ping", ping)
}
