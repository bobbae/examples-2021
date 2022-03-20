package main

import (
	"fmt"

	"github.com/bobbae/examples/gomisc/trie"
)

func main() {
	trie := trie.NewTrie()
	const fv = "first"
	cases := []struct {
		key   string
		value interface{}
	}{
		{"dog", 0},
		{"cat", 1},
		{"cats", 2},
		{"dogs", 3},
		{"catdog", 4},
		{"dogcatdog", 5},
		{"/cats", 6},
		{"/cats/garfield", 7},
		{"/dogs/goofy", 8},
	}
	for _, c := range cases {
		if v := trie.Get(c.key); v != nil {
			fmt.Println(c.key, v)
		}
	}

	for _, c := range cases {
		if new := trie.Put(c.key, fv); !new {
			fmt.Println("key exists", c.key)
		}
	}

	for _, c := range cases {
		if new := trie.Put(c.key, c.value); new {
			fmt.Println("key expected", c.key)
		}
	}

	for _, c := range cases {
		if v := trie.Get(c.key); v == c.value {
			fmt.Println(v, c)
		}
	}

	dcases := []struct {
		key   string
		value interface{}
	}{
		{"dog", 0},
		{"dogs", 3},
		{"dogcatdog", 5},
		{"/dogs/goofy", 8},
	}
	for _, d := range dcases {
		if deleted := trie.Delete(d.key); deleted {
			fmt.Println("deleted", d)
		}
	}

	for _, d := range dcases {
		if v := trie.Get(d.key); v == nil {
			fmt.Println("not found", d)
		}
	}

	for _, c := range cases {
		if v := trie.Get(c.key); v == nil {
			fmt.Println("not found", c)
		}
	}

	table := map[string]interface{}{
		"cat":            1,
		"cats":           2,
		"catdog":         4,
		"/cats":          6,
		"/cats/garfield": 7,
	}

	walked := make(map[string]int)
	for k := range table {
		walked[k] = 0
	}
	walker := func(key string, value interface{}) error {
		if value != table[key] {
			fmt.Println("mismatch", value, table[key])
		}
		walked[key]++
		return nil
	}

	if err := trie.Walk(walker); err != nil {
		fmt.Println("walk error", err)
	}

	for k, c := range walked {
		fmt.Println(k, c)
	}
}
