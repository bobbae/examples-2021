package main

import (
	"fmt"
	"hash/crc32"
	"io/ioutil"
	"log"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		log.Fatalf("filename required")
	}
	crc32q := crc32.MakeTable(0xD5828281)

	fn := os.Args[1]
	if fn == "" {
		log.Fatalf("missing filename")
	}

	dat, err := ioutil.ReadFile(fn)
	if err != nil {
		log.Fatalf("can't read file %s, %v", fn, err)
	}
	fmt.Printf("%08x\n", crc32.Checksum(dat, crc32q))
}
