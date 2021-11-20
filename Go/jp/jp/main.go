package main

import (
	"bufio"
	"encoding/json"
	"flag"
	"fmt"
	"html/template"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"reflect"
	"strings"

	"github.com/bobbae/glog"
	"github.com/bobbae/jp"
)

var progname string
var port = "8080"

func main() {
	defer func() { fmt.Println("ERROR") }()
	portopt := flag.String("port", "8080", "HTTP service port")
	serverMode := flag.Bool("server", false, "run server mode")
	if *serverMode {
		port = *portopt
	}
	progname = os.Args[0]
	flag.Parse()
	args := flag.Args()
	if len(args) > 0 && args[0] == "server" || *serverMode {
		log.Println("running as server")
		runServer()
		// should never reach here
		fmt.Fprintf(os.Stderr, "server done\n")
		os.Exit(0)
	}
	if len(args) > 0 && len(args) < 3 {
		fmt.Fprintf(os.Stderr, "%s FILENAME OPERATION PATH...\n", progname)
		fmt.Fprintf(os.Stderr, "\t OPERATION: len, print, if, foreach\n")
		os.Exit(1)
	}
	jsonStr := ""
	var err error
	if len(args) == 0 || args[0] == "-" {
		scanner := bufio.NewScanner(os.Stdin)
		for scanner.Scan() {
			jsonStr += scanner.Text()
		}
	} else {
		buf, err := ioutil.ReadFile(args[0])
		if err != nil {
			fmt.Fprintf(os.Stderr, "can't read file %s, %v\n", args[0], err)
		}
		jsonStr = string(buf)
	}
	if jsonStr == "" {
		fmt.Fprintf(os.Stderr, "empty input\n")
		os.Exit(0)
	}
	var str *string
	if str, err = processJSON(jsonStr, args); err != nil {
		os.Exit(1)
	}
	fmt.Println(*str)
	os.Exit(0)
}

func processJSON(jsonStr string, args []string) (*string, error) {
	var v interface{}
	err := json.Unmarshal([]byte(jsonStr), &v)
	if err != nil {
		return nil, fmt.Errorf("can't parse json string, %v", err)
	}

	if len(args) == 0 {
		b, err := json.MarshalIndent(v, "", "  ")
		if err != nil {
			return nil, fmt.Errorf("can't indent: %v", err)
		}
		res := fmt.Sprint(string(b))
		return &res, nil
	}
	if len(args) < 1 {
		return nil, fmt.Errorf("too few args")
	}

	if args[1] == "if" {
		if len(args) < 5 {
			return nil, fmt.Errorf("syntax error, insufficient arguments")
		}
		exp := strings.Fields(args[2])
		if len(exp) != 3 {
			return nil, fmt.Errorf("syntax error, invalid expression")
		}
		kind, res, err := jp.JSONPathos(v, exp[0])
		if err != nil {
			return nil, fmt.Errorf("json error")
		}
		if !validString(kind) {
			return nil, fmt.Errorf("invalid string")
		}
		comparator := exp[1]
		if comparator != "==" && comparator != "!=" {
			return nil, fmt.Errorf("invalid comparator")
		}
		//TODO: more comparators => , > , <=, <
		if (comparator == "==" && res == exp[2]) ||
			(comparator == "!=" && res != exp[2]) {
			op := args[3]
			if err = checkOp(op); err != nil {
				return nil, err
			}
			var res *string
			res, err = handleExp(v, op, args[4:])
			if err != nil {
				return nil, err
			}
			return res, nil
		}
		panic("never")
	}
	if args[1] == "foreach" {
		if len(args) < 4 {
			fmt.Fprintf(os.Stderr, "insufficient arguments, %v,%d\n", args, len(args))
			fmt.Fprintf(os.Stderr, "%s foreach PATH OPERATION\n", progname)
			return nil, fmt.Errorf("insufficient argument for foreach")
		}
		path := args[2]
		kind, res, err := jp.JSONPathos(v, path)
		if err != nil {
			fmt.Fprintf(os.Stderr, "json parse error, cannot find %s, %v\n", path, err)
			return nil, fmt.Errorf("json parse path error")
		}
		l := findLen(kind, res)
		if l < 0 {
			fmt.Fprintf(os.Stderr, "cannot find len for %s\n", path)
			return nil, fmt.Errorf("invalid len")
		}
		op := args[3]
		if op == "print" {
			accStr := ""
			for i := 0; i < l; i++ {
				p := fmt.Sprintf("%s[%d]", path, i)
				kind, res, err = jp.JSONPathos(v, p)
				if err != nil {
					return nil, fmt.Errorf("cannot find %s", p)
				}
				if kind == reflect.Invalid {
					return nil, fmt.Errorf("unexpected invalid type %s", p)
				}
				str, err := genOut(kind, op, res)
				if err != nil {
					return nil, err
				}
				accStr += *str + " "
			}
			return &accStr, nil
		}
		return nil, fmt.Errorf("unsupported operation %s", op)
	}
	op := args[1]
	if err = checkOp(op); err != nil {
		return nil, err
	}
	var res *string
	res, err = handleExp(v, op, args[2:])
	if err != nil {
		return nil, err
	}
	return res, nil
}

func handleExp(v interface{}, op string, tokens []string) (*string, error) {
	accStr := ""
	for _, tok := range tokens {
		kind, res, err := jp.JSONPathos(v, tok)
		if err != nil {
			return nil, fmt.Errorf("json parse error for %s, %v", tok, err)
		}
		str, err := genOut(kind, op, res)
		if err != nil {
			return nil, err
		}
		accStr += *str + " "
		if op == "list" {
			accStr += " "
		}
	}
	if op == "list" {
		accStr += "\n"
	}
	return &accStr, nil
}

func validString(kind reflect.Kind) bool {
	if kind == reflect.Invalid {
		fmt.Fprintf(os.Stderr, "unexpected invalid type")
		return false
	}
	if kind != reflect.String {
		fmt.Fprintf(os.Stderr, "expected string but got %v\n", kind)
		return false
	}
	return true
}

func checkOp(op string) error {
	if op == "if" || op == "len" || op == "print" || op == "foreach" || op == "list" {
		return nil
	}
	return fmt.Errorf("invalid operation %s", op)
}

func findLen(kind reflect.Kind, res string) int {
	if kind == reflect.Slice {
		var vv []interface{}
		if err := json.Unmarshal([]byte(res), &vv); err != nil {
			fmt.Fprintf(os.Stderr, "cannot unmarshall slice %s, %v", res, err)
			return -1
		}
		return len(vv)
	} else if kind == reflect.Map {
		var vv map[string]interface{}
		if err := json.Unmarshal([]byte(res), &vv); err != nil {
			fmt.Fprintf(os.Stderr, "cannot unmarshall map %s, %v", res, err)
			return -1
		}
		return len(vv)
	}
	fmt.Fprintf(os.Stderr, "neither slice, nor map, cannot find length\n")
	return -1
}

func genOut(kind reflect.Kind, op string, res string) (*string, error) {
	if kind == reflect.Invalid {
		return nil, fmt.Errorf("not found")
	}
	if op == "len" {
		l := findLen(kind, res)
		if l < 0 {
			return nil, fmt.Errorf("invalid len operation for type %v", kind)
		}
		str := fmt.Sprintf("len %d", l)
		return &str, nil
	} else if op == "print" {
		str := fmt.Sprintf("%s", res)
		return &str, nil
	} else if op == "list" {
		str := fmt.Sprintf("%s", res)
		return &str, nil
	}
	panic("never here")
}

func runServer() {
	server := http.NewServeMux()
	server.HandleFunc("/", handler)
	log.Printf("Server listening on port %s\n", port)
	err := http.ListenAndServe(":"+port, server)
	log.Fatal(err)
}

type Out struct {
	JSONData string
}

func handler(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		return
	}

	switch r.Method {
	case "GET":
		//fmt.Fprint(w, page)
		http.ServeFile(w, r, "index.html")
	case "POST":
		var err error
		// Call ParseForm() to parse the raw query and update r.PostForm and r.Form.
		if err = r.ParseForm(); err != nil {
			//fmt.Fprintf(w, "ParseForm() err: %v", err)
			return
		}
		//fmt.Fprintf(w, "Post from website! r.PostFrom = %v\n", r.PostForm)
		jsonStr := r.FormValue("json")
		jpStr := r.FormValue("jp")
		//fmt.Fprintf(w, "json = %s\n", jsonStr)
		//fmt.Fprintf(w, "jp = %s\n", jpStr)
		var args []string
		if jpStr != "" {
			args = strings.Split(jpStr, " ")
		}
		var str *string
		if str, err = processJSON(jsonStr, args); err != nil {
			//fmt.Fprintf(w, "err %v\n", err)
			return
		}
		if str == nil {
			//fmt.Fprintf(w, "Null result")
			return
		}
		out := Out{JSONData: *str}
		tmpl := template.Must(template.ParseFiles("index.html"))
		tmpl.Execute(w, out)

		//fmt.Fprintf(w, "RES: %s\n", *str)
		glog.Infof("RES: %s", *str)
	default:
		fmt.Fprintf(w, "Sorry, only GET and POST methods are supported.")
	}
}
