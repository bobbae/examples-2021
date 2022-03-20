// Go offers built-in support for JSON encoding and
// decoding, including to and from built-in and custom
// data types.

package main

import "encoding/json"
import "fmt"

// We'll use these two structs to demonstrate encoding and
// decoding of custom types below.
type LogItem struct {
	Id string
	Content string
}
type Logs struct {
	Items[] LogItem
}

func main() {
	str := `{ "items" : [ { "id": "id1", "content": "hello" },  {"id":"id2", "content":"there"}]}`
	logs := &Logs{}
	err := json.Unmarshal([]byte(str), &logs)
	if err != nil {
		panic(err)
	}
	fmt.Println(logs)
	fmt.Println(logs.Items[0])
}
