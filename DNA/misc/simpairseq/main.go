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
	output := flag.String("output", "out-", "prefix for output files")
	skip := flag.Bool("skip", false, "skip flanking sites")
	rlen := flag.Int("rlen", 150, "read length")

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

	path1 := *output + "1.fq"
	out1, err := os.Create(path1)
	if err != nil {
		log.Fatalf("cannot create %s, %v", path1, err)
	}
	defer out1.Close()

	path2 := *output + "2.fq"
	out2, err := os.Create(path2)
	if err != nil {
		log.Fatalf("cannot create %s, %v", path2, err)
	}
	defer out2.Close()

	w1 := bufio.NewWriter(out1)
	w2 := bufio.NewWriter(out2)

	buf := make([]byte, *rlen)
	for i := range buf {
		buf[i] = '+'
	}
	bufstr := string(buf)

	headname := ""
	recnum2 := 1
	recnum1 := 1
	for scanner.Scan() {
		line := scanner.Text()
		line = strings.Trim(line, "\r\n")

		if strings.HasPrefix(line, ">") {
			headname = string(line[1:])
			continue
		}

		if *skip {
			line = line[len(land1):]
			l := len(line) - len(land2)
			line = line[:l]
		}

		s1 := string(line[:*rlen])

		for i := 0; i < 140; i++ {
			fmt.Fprintf(w1, "@%s_%d_0_0_0_0_0_0_0_0_%d/1\n", headname, recnum1, i)
			recnum1++
			fmt.Fprintln(w1, s1)
			fmt.Fprintln(w1, "+")
			fmt.Fprintln(w1, bufstr)
		}

		s2 := string(line[len(line)-*rlen:])
		s2 = reverse(s2)

		for i := 0; i < 140; i++ {
			fmt.Fprintf(w2, "@%s_%d_0_0_0_0_0_0_0_0_%d/2\n", headname, recnum2, i)
			recnum2++
			fmt.Fprintln(w2, s2)
			fmt.Fprintln(w2, "+")
			fmt.Fprintln(w2, bufstr)
		}

	}
	w1.Flush()
	w2.Flush()

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

func reverse(s string) string {
	runes := []rune(s)
	for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
		runes[i], runes[j] = runes[j], runes[i]
	}
	return string(runes)
}
