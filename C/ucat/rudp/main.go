package main

import (
	"flag"
	"time"

	"github.com/bobbae/q"
	"github.com/pion/logging"
)

var (
	network    string // used by both client and server
	serverAddr string // used by server
	port       int    // used by server
	msgSize    int    // used by client
	numMsgs    int    // used by client
	bufferSize int    // used for UDPConn
	shutDownIn int    // in seconds (0: default to no shutdown)
	message    string
	filename   string
	verbose    int
)

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}

func init() {
	flag.StringVar(&network, "t", "udp4", "Network type. specify \"udp\" for SCTP, or \"tcp\" for TCP")
	flag.StringVar(&serverAddr, "s", "", "Remote server address (e.g. 127.0.0.1:2666)")
	flag.IntVar(&port, "p", 2666, "Port number to listen as a server")
	flag.IntVar(&msgSize, "l", 32768, "Message length")
	flag.IntVar(&numMsgs, "n", 32768, "Number of messages to transfer")
	flag.IntVar(&bufferSize, "b", 0, "SCTP read/write buffer size (0: use default)")
	flag.IntVar(&shutDownIn, "k", 0, "Shutdown timer in seconds (0: defaults to no-shutdown)")
	flag.StringVar(&message, "m", "", "Message content")
	flag.IntVar(&verbose, "v", 0, "verbosity")
	flag.StringVar(&filename, "f", "", "File name")

	flag.Parse()
}

func main() {
	loggerFactory := logging.NewDefaultLoggerFactory()
	q.Q(verbose)
	if len(serverAddr) > 0 {
		c, err := newClient(&clientConfig{
			network:    network,
			server:     serverAddr,
			bufferSize: bufferSize,
			loggerFactory: loggerFactory,
		})
		checkErr(err)
		err = c.start(time.Duration(shutDownIn) * time.Second)
		checkErr(err)
	} else {
		s, err := newServer(&serverConfig{
			network:    network,
			listenPort: port,
			bufferSize: bufferSize,
			loggerFactory: loggerFactory,
		})
		checkErr(err)
		err = s.start(time.Duration(shutDownIn) * time.Second)
		checkErr(err)
	}
}
