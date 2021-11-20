package main

import (
	"bufio"
	"fmt"
	"os"
	"path"

	"github.com/ghodss/yaml"
)

func main() {
	progname := path.Base(os.Args[0])
	if progname != "jy" && progname != "yj" {
		fmt.Fprintf(os.Stderr, "program is not name jy or yj\n")
		os.Exit(1)
	}
	instr := getInput()
	if len(instr) == 0 {
		fmt.Fprintf(os.Stderr, "empty input\n")
		os.Exit(0)
	}
	var err error
	var outstr []byte
	if progname == "jy" {
		outstr, err = yaml.JSONToYAML(instr)
		if err != nil {
			fmt.Fprintf(os.Stderr, "error %v\n", err)
			os.Exit(1)
		}
	} else if progname == "yj" {
		outstr, err = yaml.YAMLToJSON(instr)
		if err != nil {
			fmt.Fprintf(os.Stderr, "error %v\n", err)
			os.Exit(1)
		}
	}
	fmt.Println(string(outstr))
	os.Exit(0)
}

func getInput() []byte {
	instr := ""
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		instr += scanner.Text() + "\n"
	}
	return []byte(instr)
}
