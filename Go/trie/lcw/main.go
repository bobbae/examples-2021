package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"sort"

	"github.com/bobbae/examples/gomisc/trie"
)

type byLength []string

func (s byLength) Len() int {
	return len(s)
}
func (s byLength) Swap(i, j int) {
	s[i], s[j] = s[j], s[i]
}
func (s byLength) Less(i, j int) bool {
	return len(s[i]) > len(s[j]) //reverse
}

func main() {
	filename := flag.String("filename", "", "input file name")
	flag.Parse()

	if *filename == "" {
		log.Fatal(fmt.Errorf("file name is required"))
	}

	file, err := os.Open(*filename)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	words := []string{}
	scanner := bufio.NewScanner(file)
	trie := trie.NewTrie()
	val := 0
	for scanner.Scan() {
		word := scanner.Text()
		if err := scanner.Err(); err != nil {
			log.Fatal(err)
		}
		val++
		new := trie.Put(word, val)
		if !new {
			log.Fatal(fmt.Errorf("word %s exists", word))
		}
		words = append(words, word)
	}
	sort.Sort(byLength(words))

	for _, word := range words {
		if isLCW(trie, word, 0) {
			fmt.Println("LCW", word, "len", len(word))
			break
		}
	}
}

func isLCW(trie *trie.Trie, word string, iter int) bool {
	if iter > 0 && trie.Get(word) != nil {
		return true
	}
	prefixes := findPrefixes(trie, word)
	for _, idx := range prefixes {
		_, suffix := split(word, idx)
		if isLCW(trie, suffix, iter+1) {
			return true
		}
	}

	return false
}

func split(word string, idx int) (string, string) {
	return word[:idx], word[idx:]
}

func findPrefixes(trie *trie.Trie, word string) []int {
	prefixes := []int{}
	for i := 1; i < len(word); i++ {
		p := word[:i]
		if v := trie.Get(p); v != nil {
			prefixes = append(prefixes, i)
		}
	}
	return prefixes
}
