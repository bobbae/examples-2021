package main

import (
	"fmt"
	"io/ioutil"
	"net"
	"net/http"
	"os"
)

var udpAddress = ":9999"
var TotalUDP = uint64(0)

func main() {
	uc, err := net.ListenPacket("udp", udpAddress)
	if err != nil {
		fmt.Println("listen error", err)
		os.Exit(1)
	}
	defer uc.Close()
	myip := GetRealOutboundIP()

	fmt.Println("reading udp", udpAddress, "myip", myip)
	myipbytes := []byte(myip + ":")
	buffer := make([]byte, 1024)
	for {
		nr, addr, err := uc.ReadFrom(buffer)
		if err != nil {
			fmt.Println("udp read error", err)
			os.Exit(1)
		}
		TotalUDP += uint64(nr)
		fmt.Println("udp", TotalUDP, string(buffer))
		_, err = uc.WriteTo(myipbytes, addr)
		if err != nil {
			fmt.Println("udp write error", err)
			os.Exit(1)
		}
		_, err = uc.WriteTo(buffer, addr)
		if err != nil {
			fmt.Println("udp write error", err)
			os.Exit(1)
		}
	}
}

func GetRealOutboundIP() string {
	resp, err := http.Get("http://api.ipify.org")
	if err != nil {
		fmt.Println("error getting real outbound ip")
		return "unavailable"
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("error reading resp body", err)
		return "unavailable"
	}
	return fmt.Sprintf("%s", body)
}
