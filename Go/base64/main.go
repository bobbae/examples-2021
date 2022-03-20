package main

import (
	"encoding/base64"
	"fmt"
	"io/ioutil"
)

func main() {
	msg, err := ioutil.ReadFile("/tmp/x")
	if err != nil {
		panic(err)
	}
	encoded := base64.StdEncoding.EncodeToString(msg)
	fmt.Println(encoded)
	decoded, err := base64.StdEncoding.DecodeString(encoded)
	if err != nil {
		fmt.Println("decode error:", err)
		return
	}
	fmt.Println(string(decoded))
}
