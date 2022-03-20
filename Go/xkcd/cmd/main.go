package main

import (
	"context"
	"fmt"
	"log"
	"math/rand"
	"time"

	"github.com/nishanths/go-xkcd"
)

func main() {
	client := xkcd.NewClient()

	s1 := rand.NewSource(time.Now().UnixNano())
	r1 := rand.New(s1)
	comic, err := client.Get(context.Background(), r1.Intn(2421)+1)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("%s: %s\n", comic.Title, comic.ImageURL)
}
