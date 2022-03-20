package main

import (
	"flag"
	"fmt"
	"log"
	"net"
)

var (
	addr = flag.String("addr", "", "IP address")
)

func main() {
	flag.Parse()
	if *addr == "" {
		log.Fatalf("missing addr")
	}
	netaddr, _ := net.ResolveIPAddr("ip4", *addr)
	conn, err := net.ListenIP("ip4:icmp", netaddr)
	if err != nil {
		log.Fatalf("ListenIP: %v", err)
	}
	buf := make([]byte, 1024)
	numRead, _, _ := conn.ReadFrom(buf)
	fmt.Printf("% X\n", buf[:numRead])
}
