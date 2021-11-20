package main

import (
	"bufio"
	"flag"
	"io/ioutil"
	"log"
	"os"
	"strings"

	"gitlab.com/bobbae/dna/dnacodec"
	"gitlab.com/bobbae/q"
)

var infp, outfp *os.File

func main() {
	input := flag.String("input", "", "name of the input (required)")
	output := flag.String("output", "", "name of the output (required)")
	doEncode := flag.Bool("encode", false, "encode")
	doDecode := flag.Bool("decode", false, "decode")
	pad := flag.Int("pad", 0, "pad")
	receive := flag.Bool("receive", false, "receive")
	flag.Parse()
	if *input == "" {
		log.Fatalln("input required")
	}
	if *output == "" && !*receive {
		log.Fatalln("output required")
	}
	if *pad == 0 && !*doEncode && !*doDecode && !*receive {
		log.Fatalln("pad, encode, decode or receive flag must be set")
	}
	var err error
	infp, err = os.Open(*input)
	if err != nil {
		log.Fatal(err)
	}
	defer infp.Close()
	q.Q("input open", *input)
	if !*receive {
		outfp, err = os.Create(*output)
		if err != nil {
			log.Fatal(err)
		}
		defer outfp.Close()
	}
	q.Q("output open", *output)
	numRead := 0
	fi, err := infp.Stat()
	if err != nil {
		log.Fatal(err)
	}
	fileSz := fi.Size()
	q.Q(*input, fileSz)
	if *pad > 0 {
		dat := make([]byte, *pad)
		n1, err := infp.Read(dat)
		q.Q("read", dat, n1, err)
		if err != nil {
			log.Fatal(err)
		}
		dnacodec.PadBytes(dat[n1:])
		q.Q("dat", dat)
		n1, err = outfp.Write(dat)
		if err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}
	scanner := bufio.NewScanner(infp)
	lineno := 0
	for scanner.Scan() {
		line := scanner.Text()
		n := len(line)
		lineno++
		numRead = numRead + n
		if *doEncode {
			s := dnacodec.Encode(n, line)
			outfp.Write([]byte(s))
		} else if *doDecode {
			if n%4 != 0 {
				q.Q("not module 4", n)
			}
			l := dnacodec.Decode(n, line)
			outfp.Write([]byte(l))
		} else if *receive {
			nline := strings.Replace(line, "\n", "", 1)
			n = len(nline)
			l := dnacodec.Decode(n, nline)
			lines := strings.Split(l, "==")
			q.Q("decoded", len(lines), lines)
			if len(lines) < 3 {
				q.Q("missing checksum")
				continue
			}
			lines[1] = lines[1] + "=="
			chash := ""
			chash = dnacodec.GetMD5Hash(lines[1])
			if chash != lines[2] {
				q.Q("checksum mismatch", chash, lines[1])
				continue
			}
			fn := lines[0] + ".received"
			q.Q("checksums match", fn)
			err = ioutil.WriteFile(fn, []byte(lines[1]+"\n"), 0666)
			if err != nil {
				q.Q("can't write", fn, err)
			}
		}
	}
	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}
