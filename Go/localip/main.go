package main

import (
	"errors"
	"net"
	"fmt"
)

func localIP() (net.IP, error) { 
        tt, err := net.Interfaces() 
        if err != nil { 
                return nil, err 
        } 
        for _, t := range tt { 
                aa, err := t.Addrs() 
                if err != nil { 
                        return nil, err 
                } 
                for _, a := range aa { 
                        ipnet, ok := a.(*net.IPNet) 
                        if !ok { 
                                continue 
                        } 
                        v4 := ipnet.IP.To4() 
                        if v4 == nil || v4[0] == 127 { // loopback address 
                                continue 
                        } 
                        return v4, nil 
                } 
        } 
        return nil, errors.New("cannot find local IP address") 
} 

func main(){
	var ip net.IP
	var err error
	if ip, err = localIP(); err != nil {
		panic(err)
	}
	fmt.Println(ip)
}
