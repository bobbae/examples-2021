package main

import (
	"errors"
	"fmt"
	"gitlab.com/bobbae/q"
	"net"
	"strings"
)

func incIP(ip net.IP) {
	for i := len(ip) - 1; i >= 0; i-- {
		ip[i]++
		if ip[i] > 0 {
			break
		}
	}
}

func localIP() (net.IP, error) {
	tt, err := net.Interfaces()
	if err != nil {
		return nil, err
	}
	q.Q(tt)
	for _, t := range tt {
		q.Q(t)
		aa, err := t.Addrs()
		if err != nil {
			return nil, err
		}
		q.Q(aa)
		for _, a := range aa {
			ipnet, ok := a.(*net.IPNet)
			if !ok {
				continue
			}
			v4 := ipnet.IP.To4()
			if v4 == nil || v4[0] == 127 || (v4[0] == 169 && v4[1] == 254) {
				continue
			}
			startIP := ipnet.IP.Mask(ipnet.Mask)
			fmt.Printf("ipnet %+v\n", ipnet)
			q.Q(startIP, ipnet)
			for ip := startIP; ipnet.Contains(ip); incIP(ip) {
				fmt.Printf("%s\n", ip)
			}
			return v4, nil
		}
	}
	return nil, errors.New("cannot find local IP address")
}

func netRange() error {
	initialRange := "10.101.101.0/24"
	ipv4Addr, ipv4Net, err := net.ParseCIDR(initialRange)
	if err != nil {
		return err
	}

	i := strings.Index(initialRange, "/")
	suffix := initialRange[i:]
	q.Q(suffix)

	q.Q(ipv4Addr, ipv4Net)
	v4 := ipv4Net.IP.To4()
	q.Q(v4)
	startIP := ipv4Net.IP.Mask(ipv4Net.Mask)
	q.Q(startIP)

	ipnew := net.IPv4(v4[0], v4[1], v4[2]+1, v4[3])
	q.Q(ipnew, ipnew.String()+suffix)

	return nil
}

func main() {
	var ip net.IP
	var err error
	if err := netRange(); err != nil {
		panic(err)
	}
	if ip, err = localIP(); err != nil {
		panic(err)
	}
	fmt.Println(ip)
}
