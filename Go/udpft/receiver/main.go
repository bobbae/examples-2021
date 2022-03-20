package main

import (
	"fmt"
	"net"
	"os"
)

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func main() {
	if len(os.Args) != 3 {
		fmt.Fprintf(os.Stderr, "Usage:%s address:port filename\n", os.Args[0])
		os.Exit(1)
	}
	filename := os.Args[2]
	f, err := os.Create(filename)
	check(err)
	defer f.Close()
	udpAddress, err := net.ResolveUDPAddr("udp4", os.Args[1])
	check(err)
	conn, err := net.ListenUDP("udp4", udpAddress)
	defer conn.Close()
	total := 0
	b1 := make([]byte, 1500)
	for {
		n1, _, err := conn.ReadFromUDP(b1)
		check(err)
		total += n1
		_, err = f.Write(b1[:n1])
		check(err)
		if n1 < 1500 {
			break
		}
	}
	fmt.Printf("total %d bytes\n", total)
}
