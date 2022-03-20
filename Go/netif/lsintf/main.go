package main

import (
	"flag"
	"fmt"
	"log"
	"net"

	"github.com/bobbae/q"
)

func main() {
	flag.Parse()
	interfaces, err := net.Interfaces()
	if err != nil {
		log.Fatal(err)
	}
	q.Q(interfaces)
	for _, intf := range interfaces {
		addrs, err := intf.Addrs()
		if err != nil {
			log.Fatal(err)
		}
		outstr := fmt.Sprintf("%v %v", intf.Name, addrs)
		fmt.Println(outstr)
		q.Q(intf.Name, addrs)
	}
}
