package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
)

type LogDetail struct {
	Words  string
	Metric []int
}

type LogContent struct {
	Timestamp string
	Tag       string
	Origin    string
	Details   []LogDetail
}

type LogItem struct {
	Id      string
	Content LogContent
}

type Logs struct {
	Items []LogItem
}

// Main function
// I realize this function is much too simple I am simply at a loss to

func main() {
	data, e := ioutil.ReadFile("./logs.json")
	if e != nil {
		panic(e)
	}
	fmt.Printf("File content: %s\n", string(data))
	logs := &Logs{}
	if err := json.Unmarshal(data, &logs); err != nil {
		panic(err)
	}
	fmt.Println("Logs:", logs)
	fmt.Printf("Results: %v\n", logs)
}
