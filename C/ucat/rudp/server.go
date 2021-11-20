package main

import (
	"bytes"
	"fmt"
	"math/rand"
	"net"
	"os"
	"strings"
	"sync/atomic"
	"time"

	"github.com/bobbae/q"
	"github.com/enobufs/go-rudp"
	"github.com/pion/logging"
)

type serverConfig struct {
	network       string
	listenPort    int
	bufferSize    int
	loggerFactory logging.LoggerFactory
}

type server interface {
	start(time.Duration) error
}

type sctpServer struct {
	network       string
	listenPort    int
	bufferSize    int
	loggerFactory logging.LoggerFactory
}

type tcpServer struct {
	network    string
	listenPort int
}

var totalReceived = 0

func newServer(cfg *serverConfig) (server, error) {
	if strings.HasPrefix(cfg.network, "udp") {
		return &sctpServer{
			network:       cfg.network,
			listenPort:    cfg.listenPort,
			bufferSize:    cfg.bufferSize,
			loggerFactory: cfg.loggerFactory,
		}, nil
	}

	if strings.HasPrefix(cfg.network, "tcp") {
		return &tcpServer{
			network:    cfg.network,
			listenPort: cfg.listenPort,
		}, nil
	}

	return nil, fmt.Errorf("invalid network %s", cfg.network)
}

func (s *sctpServer) start(duration time.Duration) error {
	locAddr, err := net.ResolveUDPAddr(s.network, fmt.Sprintf(":%d", s.listenPort))
	if err != nil {
		return err
	}
	q.Q(locAddr)
	// make sure the server is already listening when this function returns
	l, err := rudp.Listen(&rudp.ListenConfig{
		Network:       s.network,
		BufferSize:    s.bufferSize,
		LocalAddr:     locAddr,
		LoggerFactory: s.loggerFactory,
	})
	if err != nil {
		return err
	}
	defer l.Close()

	q.Q("listening on", l.LocalAddr().Network(), l.LocalAddr().String())
	var fp *os.File
	fp = nil
	if filename != "" {
		if filename == "-" {
			fp = os.Stdout
		} else {
			fp, err = os.Create(filename)
			if err != nil {
				q.Q("file open", err)
				return err
			}
			defer fp.Close()
		}
	}
	for {
		sconn, err := l.Accept()
		if err != nil {
			break
		}

		q.Q("new connection from ", sconn.RemoteAddr().String())

		since := time.Now()

		go func() {
			defer sconn.Close()

			// assume 1 channel per client
			serverCh, err := sconn.AcceptChannel()
			if err != nil {
				return
			}
			defer serverCh.Close()

			var totalBytesReceived uint64
			src := rand.NewSource(123)
			rnd := rand.New(src)
			exp := make([]byte, 64*1024)

			// Start printing out the observed throughput
			ticker := throughputTicker(&totalBytesReceived)

			buf := make([]byte, 64*1024)
			for duration == 0 || time.Since(since) < duration {
				n, err := serverCh.Read(buf)
				if err != nil {
					break
				}
				if fp == nil {
					if message == "" {
						_, err = rnd.Read(exp[:n])
						if err != nil {
							break
						}
					} else {
						copy(exp, message)
					}
					if !bytes.Equal(buf[:n], exp[:n]) {
						q.Q("data mismatch", buf[:n], exp[:n])
						//panic(fmt.Errorf("data mismatch"))
					}
				} else {
					_, err := fp.Write(buf[:n])
					if err != nil {
						q.Q("file write err", err)
						break
					}
				}

				atomic.AddUint64(&totalBytesReceived, uint64(n))
				totalReceived++
				if verbose > 0 {
					q.Q("received: ", n, string(buf[:n]))
				}
			}
			q.Q("closed connection for", sconn.RemoteAddr().String(), totalReceived, totalBytesReceived)

			ticker.Stop()
			fp.Sync()
		}()
	}
	return nil
}

func (s *tcpServer) start(duration time.Duration) error {
	locAddr, err := net.ResolveTCPAddr(s.network, fmt.Sprintf(":%d", s.listenPort))
	if err != nil {
		return err
	}

	// make sure the server is already listening when this function returns
	l, err := net.ListenTCP(s.network, locAddr)
	if err != nil {
		return err
	}
	defer l.Close()

	q.Q("listening on", l.Addr().Network(), l.Addr().String())

	for {
		sconn, err := l.Accept()
		if err != nil {
			break
		}

		q.Q("new connection from", sconn.RemoteAddr().String())

		since := time.Now()

		go func() {
			defer sconn.Close()

			var totalBytesReceived uint64
			src := rand.NewSource(123)
			rnd := rand.New(src)
			exp := make([]byte, 64*1024)

			// Start printing out the observed throughput
			ticker := throughputTicker(&totalBytesReceived)

			buf := make([]byte, 64*1024)
			for duration == 0 || time.Since(since) < duration {
				n, err := sconn.Read(buf)
				if err != nil {
					break
				}
				if message == "" {
					_, err = rnd.Read(exp[:n])
					if err != nil {
						break
					}
				} else {
					copy(exp, message)
				}
				if !bytes.Equal(buf[:n], exp[:n]) {
					q.Q("data mismatch", buf[:n], exp[:n])
					//panic(fmt.Errorf("data mismatch"))
				}

				atomic.AddUint64(&totalBytesReceived, uint64(n))
				totalReceived++
				if verbose > 0 {
					q.Q("received: ", n, string(buf[:n]))
				}
			}

			ticker.Stop()
			q.Q("closed connection for", sconn.RemoteAddr().String(), totalReceived)
		}()
	}

	return nil
}
