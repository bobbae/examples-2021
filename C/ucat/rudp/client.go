package main

import (
	"bufio"
	"fmt"
	"io"
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

type clientConfig struct {
	network       string
	server        string
	bufferSize    int
	loggerFactory logging.LoggerFactory
}

type client interface {
	start(time.Duration) error
}

type sctpClient struct {
	network       string
	remAddr       *net.UDPAddr
	bufferSize    int
	loggerFactory logging.LoggerFactory
}

type tcpClient struct {
	network       string
	remAddr       *net.TCPAddr
	loggerFactory logging.LoggerFactory
}

func newClient(cfg *clientConfig) (client, error) {
	if strings.HasPrefix(cfg.network, "udp") {
		remAddr, err := net.ResolveUDPAddr(cfg.network, cfg.server)
		if err != nil {
			return nil, err
		}
		return &sctpClient{
			network:       cfg.network,
			remAddr:       remAddr,
			bufferSize:    cfg.bufferSize,
			loggerFactory: cfg.loggerFactory,
		}, nil
	}

	if strings.HasPrefix(cfg.network, "tcp") {
		remAddr, err := net.ResolveTCPAddr(cfg.network, cfg.server)
		if err != nil {
			return nil, err
		}
		return &tcpClient{
			network:       cfg.network,
			remAddr:       remAddr,
			loggerFactory: cfg.loggerFactory,
		}, nil
	}

	return nil, fmt.Errorf("invalid network %s", cfg.network)
}

func (c *sctpClient) start(duration time.Duration) error {
	q.Q("connecting to server", c.remAddr.String())
	rudpc, err := rudp.Dial(&rudp.DialConfig{
		Network:       c.network,
		RemoteAddr:    c.remAddr,
		BufferSize:    c.bufferSize,
		LoggerFactory: c.loggerFactory,
	})
	if err != nil {
		return err
	}
	defer rudpc.Close()

	clientCh, err := rudpc.OpenChannel(777)
	if err != nil {
		return err
	}

	maxBufferAmount := uint64(1024 * 1024)
	bufferedAmountTh := uint64(512 * 1024)
	var totalBytesSent uint64
	writable := make(chan struct{}, 1)

	clientCh.SetBufferedAmountLowThreshold(bufferedAmountTh)
	clientCh.OnBufferedAmountLow(func() {
		select {
		case writable <- struct{}{}:
		default:
		}
	})

	// Start printing out the observed throughput
	ticker := throughputTicker(&totalBytesSent)

	src := rand.NewSource(123)
	rnd := rand.New(src)

	since := time.Now()

	buf := make([]byte, msgSize)
	var reader *bufio.Reader
	reader = nil
	if filename != "" {
		if filename == "-" {
			reader = bufio.NewReader(os.Stdin)
			q.Q("stdin open")
		} else {
			fp, err := os.Open(filename)
			if err != nil {
				q.Q("file open error", err)
				return err
			}
			defer fp.Close()
			q.Q("file open", filename)
			reader = bufio.NewReader(fp)
		}
	} else {
		if message != "" {
			copy(buf, message)
		}
	}
	totalSent := 0
	isEOF := false
	count := 0
	for i := 0; i < numMsgs && (duration == 0 || time.Since(since) < duration); i++ {
		if reader != nil {
			count, err = reader.Read(buf)
			if err != nil {
				if err != io.EOF {
					q.Q("file read error", err)
					return err
				}
				isEOF = true
				if count == 0 {
					q.Q("count is zero", count)
				}
			}
		} else {
			if message == "" {
				_, err := rnd.Read(buf)
				if err != nil {
					panic(err)
				}
			}
		}
		if clientCh.BufferedAmount() >= maxBufferAmount {
			<-writable
		}

		n, err := clientCh.Write(buf[:count])
		if err != nil {
			panic(err)
		}
		atomic.AddUint64(&totalBytesSent, uint64(n))
		totalSent++
		if isEOF {
			q.Q(isEOF)
			break
		}
	}

	ticker.Stop()

	// Wait until the buffer is completely drained
	for clientCh.BufferedAmount() > 0 {
		time.Sleep(time.Second)
	}

	q.Q("client done", totalSent, totalBytesSent)
	close(writable)
	clientCh.Close()
	time.Sleep(200 * time.Millisecond)
	return nil
}

func (c *tcpClient) start(duration time.Duration) error {
	q.Q("connecting to server", c.remAddr.String())
	locAddr := &net.TCPAddr{
		Port: 0,
	}
	tcpc, err := net.DialTCP(c.network, locAddr, c.remAddr)
	if err != nil {
		return err
	}
	defer tcpc.Close()

	var totalBytesSent uint64

	// Start printing out the observed throughput
	ticker := throughputTicker(&totalBytesSent)

	src := rand.NewSource(123)
	rnd := rand.New(src)

	since := time.Now()

	buf := make([]byte, msgSize)
	for i := 0; i < numMsgs && (duration == 0 || time.Since(since) < duration); i++ {
		_, err := rnd.Read(buf)
		if err != nil {
			panic(err)
		}
		var nWritten int
		for nWritten < msgSize {
			n, err := tcpc.Write(buf[nWritten:])
			if err != nil {
				panic(err)
			}
			atomic.AddUint64(&totalBytesSent, uint64(n))
			nWritten += n
		}
	}

	ticker.Stop()

	// Shutdown gracefully
	tcpc.CloseWrite()
	for {
		_, err := tcpc.Read(buf)
		if err != nil {
			break
		}
	}

	q.Q("client done")
	tcpc.CloseRead()
	return nil
}

func throughputTicker(totalBytes *uint64) *time.Ticker {
	ticker := time.NewTicker(2 * time.Second)
	lastBytes := atomic.LoadUint64(totalBytes)
	go func() {
		for {
			since := time.Now()
			select {
			case _, ok := <-ticker.C:
				if !ok {
					return
				}
			}
			totalBytes := atomic.LoadUint64(totalBytes)
			bps := float64((totalBytes-lastBytes)*8) / time.Since(since).Seconds()
			lastBytes = totalBytes
			q.Q("Throughput: Mbps", bps/1024/1024)
		}
	}()
	return ticker
}
