package main

import (
	"flag"
	"os"
	"fmt"
	"log/syslog"
)

var service = flag.String("service", "localhost:514", "addr:port")
var msg = flag.String("message", "a test log message", "message")
var priority = flag.Int("priority", int(syslog.LOG_ERR), "priority")

func main() {
	flag.Parse()
	fmt.Printf("connecting to server at %v\n",*service)
	logger,err := syslog.Dial("udp4", *service, syslog.Priority(*priority), "[go_logger]")
	defer logger.Close()
	if err != nil {
		fmt.Printf("cannot connect to syslog server at ", service)
		os.Exit(1)
	}
	logger.Write([]byte(*msg))
}
