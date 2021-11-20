package main

import (
	"bufio"
	"encoding/base64"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path"
	"strings"

	"github.com/codeskyblue/go-sh"
	"gitlab.com/bobbae/dna/dnacodec"
	"gitlab.com/bobbae/q"
)

var dir = flag.String("dir", "/tmp/dnazfec", "directory to work in")
var clean = flag.Bool("clean", true, "clean first")
var input = flag.String("input", "", "full path to input file")
var split = flag.Int("split", 150, "split size")
var decode = flag.Bool("decode", false, "simulate sequencer and decode")
var numpairs = flag.Int("numpairs", 100000, "number of read pairs") // 1000000
var stddev = flag.Int("stddev", 7, "standard deviation")            // 50
var baseerr = flag.Float64("baseerr", 0.005, "base error rate")     // 0.02
var ratemut = flag.Float64("ratemut", 0.001, "rate of mutation")
var fracindel = flag.Float64("fracindel", 0.1, "fraction of indels") // 0.15
var probext = flag.Float64("probext", 0.3, "probability an indel is extended")
var maxnrat = flag.Float64("maxnrat", 0.05, "disregard if the fraction of ambiguous bases higher than float")
var hlen = flag.Int("hlen", 250, "pair read len")

func main() {
	flag.Parse()
	var err error
	if *input == "" {
		log.Fatalln("input required")
	}
	if *clean {
		if err = os.RemoveAll(*dir); err != nil {
			log.Fatal(err)
		}
		q.Q("removeall", *dir)
	}
	if err = os.MkdirAll(*dir, 0775); err != nil {
		log.Fatal(err)
	}
	q.Q("mkdirall", *dir)
	if err = os.Chdir(*dir); err != nil {
		log.Fatal(err)
	}
	//q.Q(*split)
	out, err := sh.Command("split", "-b", intstrval(*split), *input).Output()
	if err != nil {
		log.Fatalln("split", out, err)
	}
	files, err := ioutil.ReadDir("./")
	if err != nil {
		log.Fatalln("readdir", err)
	}
	lastfile := ""
	for _, f := range files {
		lastfile = f.Name()
	}
	q.Q(lastfile)
	//fi, err := os.Stat(lastfile)
	// if err != nil {
	// 	log.Fatalln("stat", lastfile, err)
	// }
	//lastSz := fi.Size()
	rdat, err := ioutil.ReadFile(lastfile)
	if err != nil {
		log.Fatalln("read", lastfile, err)
	}
	dat := make([]byte, *split)
	copy(dat, rdat)
	//n1 := copy(dat, rdat)
	//dnacodec.PadBytes(dat[n1:])
	//dat[n1] = '='
	//dat[n1+1] = '='
	if err = ioutil.WriteFile(lastfile, dat, 0666); err != nil {
		log.Fatalln("writefile", lastfile, err)
	}
	for _, f := range files {
		_, err = sh.Command("zfec", f.Name()).Output()
		if err != nil {
			log.Fatalln("zfec", f.Name(), err)
		}
	}
	files, err = ioutil.ReadDir("./")
	if err != nil {
		log.Fatalln("readdir", err)
	}
	outfn := *dir + "/" + path.Base(*input) + ".fasta"
	outfp, err := os.Create(outfn)
	if err != nil {
		log.Fatalln("create", err)
	}
	var encLen = 0
	for _, f := range files {
		fn := f.Name()
		if strings.HasSuffix(fn, ".fec") {
			dat, err := ioutil.ReadFile(fn)
			if err != nil {
				log.Fatalln("read", fn, err)
			}
			estr := base64.StdEncoding.EncodeToString(dat)
			fnstr := fn[:strings.Index(fn, ".fec")]
			bufstr := fmt.Sprintf("%s==%s", fnstr, estr)
			md5str := dnacodec.GetMD5Hash(bufstr)
			bufstr = fmt.Sprintf("%s%s", bufstr, md5str)
			encbufstr := dnacodec.Encode(len(bufstr), bufstr)
			if _, err := fmt.Fprintf(outfp, "> %s\n%s\n", fnstr+":"+bufstr, encbufstr); err != nil {
				log.Fatalln("fprintf", err)
			}
			//q.Q(bufstr, len(bufstr))
			encLen = len(encbufstr)
		}
	}
	outfp.Close()
	//q.Q(encLen)
	if *decode {
		decodeBack(encLen, outfn)
	}
	os.Exit(0)
}

func intstrval(val int) string {
	return fmt.Sprintf("%d", val)
}

func floatstrval(val float64) string {
	return fmt.Sprintf("%f", val)
}

func decodeBack(encLen int, outfn string) {
	q.Q(encLen, outfn)
	out, err := sh.Command("wgsim",
		"-N", intstrval(*numpairs),
		"-e", floatstrval(*baseerr),
		"-s", intstrval(*stddev),
		"-r", floatstrval(*ratemut),
		"-R", floatstrval(*fracindel),
		"-X", floatstrval(*probext),
		"-A", floatstrval(*maxnrat),
		"-d", intstrval(encLen),
		"-1", intstrval(*hlen),
		"-2", intstrval(*hlen),
		outfn,
		"out1.fq",
		"out2.fq").CombinedOutput()
	if err != nil {
		log.Fatalln("wgsim", err, string(out))
	}
	q.Q(string(out))
	q.Q("running pear...")
	out, err = sh.Command("pear", "-f", "out1.fq", "-r", "out2.fq", "-o", "all.fq").CombinedOutput()
	if err != nil {
		log.Fatalln("pear", err, string(out))
	}
	infp, err := os.Open("all.fq.assembled.fastq")
	if err != nil {
		log.Fatalln("open all.fq", err)
	}
	outfp, err := os.Create("out.good")
	if err != nil {
		log.Fatalln("create out.good", err)
	}
	scanner := bufio.NewScanner(infp)
	lineno := 0
	numreceived := 0
	for scanner.Scan() {
		line := scanner.Text()
		if lineno%4 == 2 {
			outfp.Write([]byte(line))
			nline := strings.Replace(line, "\n", "", 1)
			n := len(nline)
			l := dnacodec.Decode(n, nline)
			lines := strings.Split(l, "==")
			//q.Q("decoded", len(lines), lines)
			if len(lines) < 3 {
				//q.Q("missing checksum")
				continue
			}
			lines[1] = lines[1] + "=="
			bufstr := lines[0] + "==" + lines[1]
			chash := dnacodec.GetMD5Hash(bufstr)
			if chash != lines[2] {
				//q.Q("checksum mismatch", chash, lines[1])
				continue
			}
			fn := lines[0] + ".received"
			//q.Q("checksums match", fn)
			err = ioutil.WriteFile(fn, []byte(lines[1]+"\n"), 0666)
			if err != nil {
				log.Fatalln("write", fn, err)
			}
			//q.Q(fn)
			numreceived++
		}
		lineno++
	}
	infp.Close()
	outfp.Close()
	q.Q(numreceived)
	files, err := ioutil.ReadDir("./")
	if err != nil {
		q.Q(err)
		log.Fatalln("readdir", err)
	}
	fnames := make(map[string][]string)
	numdecoded := 0
	for _, fi := range files {
		fn := fi.Name()
		if !strings.HasSuffix(fn, ".received") {
			continue
		}
		dat, err := ioutil.ReadFile(fn)
		if err != nil {
			log.Fatalln("read", fn, err)
		}
		decodedBase64, err := base64.StdEncoding.DecodeString(string(dat))
		if err != nil {
			log.Fatalln("base64decode", fn, string(dat), err)
		}
		fn = fn + ".decoded"
		if err = ioutil.WriteFile(fn, decodedBase64, 0666); err != nil {
			log.Fatalln("write", fn, err)
		}
		key := fn[:strings.Index(fn, ".")]
		fnames[key] = append(fnames[key], fn)
		numdecoded++
	}
	q.Q(numdecoded)
	numzunfec := 0
	for k := range fnames {
		//q.Q(fnames[k])
		args := []string{"-o", k + ".zunfec"}
		args = append(args, fnames[k]...)
		intf := make([]interface{}, len(args))
		for i, v := range args {
			intf[i] = v
		}
		out, err := sh.Command("zunfec", intf...).CombinedOutput()
		if err != nil {
			log.Fatalln("zunfec", err, string(out))
		}
		numzunfec++
	}
	q.Q(numzunfec)
}
