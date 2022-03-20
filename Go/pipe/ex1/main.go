package main

import (
	"compress/gzip"
	"os"
)

func main() {
	done := make(chan bool)
	r, w, _ := os.Pipe()
	gw := gzip.NewWriter(os.Stdout)
	gr, _ := gzip.NewReader(os.Stdin)

	go func() {
		var buf [8192]byte
		for {
			n, err := r.Read(buf[:])
			if err != nil {
				break
			}
			gw.Write(buf[:n])
		}
		gw.Close()
		done <- true
	}()

	var buf [8192]byte
	for {
		n, err := gr.Read(buf[:])
		if err != nil {
			break
		}
		w.Write(buf[:n])
	}
	w.Close()
	<-done
}
