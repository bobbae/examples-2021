package main

import (
	"fmt"
	"net"
	"os"
	"time"
)

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func main() {
	if len(os.Args) != 3 {
		fmt.Fprintf(os.Stderr, "Usage:%s filename address:port\n", os.Args[0])
		os.Exit(1)
	}
	filename := os.Args[1]
	f, err := os.Open(filename)
	check(err)
	defer f.Close()
	conn, err := net.Dial("udp4", os.Args[2])
	defer conn.Close()
	ticker := time.NewTicker(time.Millisecond * 1000)
	total := 0
	go func() {
		for t := range ticker.C {
			fmt.Printf("Read %d bytes %v\n", total, t)
		}
	}()
	b1 := make([]byte, 1500)
	for {
		n1, err := f.Read(b1)
		check(err)
		total += n1
		_, err = conn.Write(b1[:n1])
		check(err)
		if n1 < 1500 {
			break
		}
	}
	ticker.Stop()
	fmt.Printf("total %d bytes\n", total)
}
