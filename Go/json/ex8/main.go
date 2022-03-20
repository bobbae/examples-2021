package main

import (
	"encoding/json"
	"fmt"
)

type Logline struct {
	Facility,Severity int
	Id,Timestamp,Origin,Tag,Content string
}


// Main function
// I realize this function is much too simple I am simply at a loss to

func main() {
	ll := &Logline{}
	ll.Id = "sdfdaf"
	logMas,err := json.Marshal(ll)
	if err != nil {
		panic(err)
	}
	fmt.Println("Marshal: ",string(logMas))
}
