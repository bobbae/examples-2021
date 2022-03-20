package main

import (
	"net"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		os.Stderr.WriteString("Please provide interface name\n")
		os.Exit(1)
	}
	name := os.Args[1]
	intf, err := net.InterfaceByName(name)
	if err != nil {
		os.Stderr.WriteString("invalid interface: " + name + "\n")
		os.Exit(1)
	}
	addrs, err := intf.Addrs()
	if err != nil {
		os.Stderr.WriteString("Oops: " + err.Error() + "\n")
		os.Exit(1)
	}

	for _, a := range addrs {
		if ipnet, ok := a.(*net.IPNet); ok && !ipnet.IP.IsLoopback() {
			if ipnet.IP.To4() != nil {
				os.Stdout.WriteString(ipnet.IP.String() + "\n")
			}
		}
	}
}
