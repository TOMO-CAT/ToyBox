package httphandler

import (
	"net/http"
	"net/http/pprof"
)

func muxPProf(m *http.ServeMux) {
	if m == http.DefaultServeMux {
		return
	}
	m.HandleFunc("/debug/pprof", pprof.Index)
	m.HandleFunc("/debug/pprof/cmdline", pprof.Cmdline)
	m.HandleFunc("/debug/pprof/profile", pprof.Profile)
	m.HandleFunc("/debug/pprof/symbol", pprof.Symbol)
	m.HandleFunc("/debug/pprof/trace", pprof.Trace)
}
