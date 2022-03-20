package main

import (
	"flag"
	"log"
	"net/http"

	"gitlab.com/bobbae/kuberp/server"
)

func main() {
	grpcAddress := flag.String("grpc", ":18080", "grpc addr")
	restAddress := flag.String("rest", ":18081", "rest addr")
	proxyAddress := flag.String("proxy", ":443", "proxy addr")
	certDir := flag.String("cert", "/kuberp", "certificate dir")
	flag.Parse()
	if err := server.CheckCerts(*certDir); err != nil {
		log.Fatalln("bad certs", *certDir)
	}
	go server.ListenAndServeREST(*restAddress, *grpcAddress)
	go server.ListenAndServeGRPC(*grpcAddress)
	log.Fatal(http.ListenAndServeTLS(*proxyAddress, *certDir+"/cert.pem", *certDir+"/key.pem", server.NewRouter()))
}
