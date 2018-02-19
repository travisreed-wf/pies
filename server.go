package main

import (
	"fmt"
	"net/http"

	log "github.com/Sirupsen/logrus"
	"github.com/rcrowley/go-metrics"
)

type RestServer struct {
	reqs metrics.Counter 
}

func GetRestServer() *RestServer {
	c := metrics.NewCounter()
	metrics.Register("piesTelemetry.reqs", c)
	svr := &RestServer{
		reqs: c,
	}
	return svr
}

func (r *RestServer) Run() error {
	svr := &http.Server{
		Addr: ":8080",
		Handler: r,
	}
	log.Info("rest_server.run: rest server starting up")
	return svr.ListenAndServe()
}

func (r *RestServer) ServeHTTP(res http.ResponseWriter, req *http.Request) {
	log.Info("Received request")
	r.reqs.Inc(1)
	if req.Method == "GET" {
		res.WriteHeader(http.StatusOK)
		s := fmt.Sprintf("Requested %s\n", req.URL.Path)
		res.Write([]byte(s))
		return
	} else {
		log.Warnf("Invalid request method [%s]", req.Method)
		res.Header().Set("Allow", "GET")
		res.WriteHeader(http.StatusMethodNotAllowed)
		res.Write([]byte("Method not allowed"))
		return
	}
}

// meter := metrics.NewMeter()
// metrics.Register("quux", meter)
// meter.Mark(47)
