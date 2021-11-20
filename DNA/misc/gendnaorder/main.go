package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
)

var land1 = "GTTCAGAGTTCTACAGTCCGACGATC"
var land2 = "TGGAATTCTCGGGTGCCAAGG"

//for use with dna-fountain
func main() {
	filename := flag.String("filename", "", "name of the input file (required)")
	prefix := flag.Bool("prefix", true, "whether to insert the line with beginning character '>'")
	flank := flag.Bool("flank", true, "whether to flank with landing sites")

	flag.Parse()

	if *filename == "" {
		log.Fatalf("filename required")
	}

	file, err := os.Open(*filename)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	maxlen := 0
	numlines := 0
	for scanner.Scan() {
		line := scanner.Text()
		line = strings.Trim(line, "\r\n")
		if strings.HasPrefix(line, ">") {
			if *prefix {
				line = strings.Replace(line, " ", "_", 1)
				fmt.Println(line)
			}
			continue
		}
		if *flank {
			line = land1 + line + land2
		}
		if len(line) > maxlen {
			maxlen = len(line)
		}
		fmt.Println(line)
		numlines++
	}
	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}
