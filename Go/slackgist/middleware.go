package slackgist

import (
	"log"
	"net/http"
	"time"
)

type MW struct {
	handler http.Handler
}

func (m MW) ServeHTTP(rw http.ResponseWriter, r *http.Request) {
	start := time.Now()
	m.handler.ServeHTTP(rw, r)
	log.Printf("%s served in %s\n", r.URL, time.Since(start))
}

func NewM(h http.Handler) http.Handler {
	return MW{h}
}
