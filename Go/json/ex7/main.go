package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
)

type Limit struct {
	Name  string
	Value int
}

func main() {
	data, e := ioutil.ReadFile("./limits.json")
	if e != nil {
		panic(e)
	}
	fmt.Printf("File content: %s\n", string(data))
	limits := []Limit{}
	if err := json.Unmarshal(data, &limits); err != nil {
		panic(err)
	}
	fmt.Println("limits:", limits)
	limitMas, err := json.Marshal(limits)
	if err != nil {
		panic(err)
	}
	fmt.Println("Marshal: ", string(limitMas))
}
