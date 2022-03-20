package main

import (
	"github.com/docker/docker/client"
	"golang.org/x/net/context"
	"os"
)

func main() {
	cli, err := client.NewClientWithOpts(client.WithVersion("1.37"))
	if err != nil {
		panic(err)
	}

	err = cli.ContainerKill(context.Background(), os.Args[1], "SIGKILL")
	if err != nil {
		panic(err)
	}
}
