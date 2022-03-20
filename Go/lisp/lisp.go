/*
 * A minimal Scheme interpreter, as seen in lis.py and SICP
 * http://norvig.com/lispy.html
 * http://mitpress.mit.edu/sicp/full-text/sicp/book/node77.html
 *
 * Pieter Kelchtermans 2013
 * LICENSE: WTFPL 2.0
 */
package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"reflect"
	"strconv"
	"strings"
)

func main() {
	Repl()
}

/*
 Eval / Apply
*/

func eval(expression scmer, en *env) (value scmer) {
	switch e := expression.(type) {
	case number:
		value = e
	case symbol:
		value = en.Find(e).vars[e]
	case []scmer:
		switch car, _ := e[0].(symbol); car {
		case "quote":
			value = e[1]
		case "if":
			if eval(e[1], en).(bool) {
				value = eval(e[2], en)
			} else {
				value = eval(e[3], en)
			}
		case "set!":
			v := e[1].(symbol)
			en.Find(v).vars[v] = eval(e[2], en)
			value = "ok"
		case "define":
			en.vars[e[1].(symbol)] = eval(e[2], en)
			value = "ok"
		case "lambda":
			value = proc{e[1], e[2], en}
		case "begin":
			for _, i := range e[1:] {
				value = eval(i, en)
			}
		default:
			operands := e[1:]
			values := make([]scmer, len(operands))
			for i, x := range operands {
				values[i] = eval(x, en)
			}
			value = apply(eval(e[0], en), values)
		}
	default:
		log.Println("Unknown expression type - EVAL", e)
	}
	return
}

func apply(procedure scmer, args []scmer) (value scmer) {
	switch p := procedure.(type) {
	case func(...scmer) scmer:
		value = p(args...)
	case proc:
		en := &env{make(vars), p.en}
		switch params := p.params.(type) {
		case []scmer:
			for i, param := range params {
				en.vars[param.(symbol)] = args[i]
			}
		default:
			en.vars[params.(symbol)] = args
		}
		value = eval(p.body, en)
	default:
		log.Println("Unknown procedure type - APPLY", p)
	}
	return
}

type proc struct {
	params, body scmer
	en           *env
}

/*
 Environments
*/

type vars map[symbol]scmer
type env struct {
	vars
	outer *env
}

func (e *env) Find(s symbol) *env {
	if _, ok := e.vars[s]; ok {
		return e
	} else {
		return e.outer.Find(s)
	}
}

/*
 Primitives
*/

var globalenv env

func init() {
	globalenv = env{
		vars{ //aka an incomplete set of compiled-in functions
			"+": func(a ...scmer) scmer {
				v := a[0].(number)
				for _, i := range a[1:] {
					v += i.(number)
				}
				return v
			},
			"-": func(a ...scmer) scmer {
				v := a[0].(number)
				for _, i := range a[1:] {
					v -= i.(number)
				}
				return v
			},
			"*": func(a ...scmer) scmer {
				v := a[0].(number)
				for _, i := range a[1:] {
					v *= i.(number)
				}
				return v
			},
			"/": func(a ...scmer) scmer {
				v := a[0].(number)
				for _, i := range a[1:] {
					v /= i.(number)
				}
				return v
			},
			"<=": func(a ...scmer) scmer {
				return a[0].(number) <= a[1].(number)
			},
			"equal?": func(a ...scmer) scmer {
				return reflect.DeepEqual(a[0], a[1])
			},
			"cons": func(a ...scmer) scmer {
				switch car := a[0]; cdr := a[1].(type) {
				case []scmer:
					return append([]scmer{car}, cdr...)
				default:
					return []scmer{car, cdr}
				}
			},
			"car": func(a ...scmer) scmer {
				return a[0].([]scmer)[0]
			},
			"cdr": func(a ...scmer) scmer {
				return a[0].([]scmer)[1:]
			},
			"list": eval(read(
				"(lambda z z)"),
				&globalenv),
		},
		nil}
}

/*
 Parsing
*/

//symbols, numbers, expressions, procedures, lists, ... all implement this interface, which enables passing them along in the interpreter
type scmer interface{}

type symbol string  //symbols are represented by strings
type number float64 //numbers by float64

func read(s string) (expression scmer) {
	tokens := tokenize(s)
	return readFrom(&tokens)
}

//Syntactic Analysis
func readFrom(tokens *[]string) (expression scmer) {
	//pop first element from tokens
	token := (*tokens)[0]
	*tokens = (*tokens)[1:]
	switch token {
	case "(": //a list begins
		L := make([]scmer, 0)
		for (*tokens)[0] != ")" {
			if i := readFrom(tokens); i != symbol("") {
				L = append(L, i)
			}
		}
		*tokens = (*tokens)[1:]
		return L
	default: //an atom occurs
		if f, err := strconv.ParseFloat(token, 64); err == nil {
			return number(f)
		} else {
			return symbol(token)
		}
	}
}

//Lexical Analysis
func tokenize(s string) []string {
	return strings.Split(
		strings.Replace(strings.Replace(s, "(", "( ",
			-1), ")", " )",
			-1), " ")
}

/*
 Interactivity
*/

func String(v scmer) string {
	switch v := v.(type) {
	case []scmer:
		l := make([]string, len(v))
		for i, x := range v {
			l[i] = String(x)
		}
		return "(" + strings.Join(l, " ") + ")"
	default:
		return fmt.Sprint(v)
	}
}

func Repl() {
	scanner := bufio.NewScanner(os.Stdin)
	for fmt.Print("> "); scanner.Scan(); fmt.Print("> ") {
		fmt.Println("==>", String(eval(read(scanner.Text()), &globalenv)))
	}
}
