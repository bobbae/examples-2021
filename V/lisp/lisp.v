module main

type Number = f64 | int
type Symbol = string
type Bool = bool
type Atom = Number | Symbol | Bool
type List = []Atom
type Expression =  Atom | List | Proc

[heap]
struct Environment  {
    mut: vars map[string]Expression
    outer &Environment
}

struct Proc {
    params Expression
    body Expression
    env &Environment
}

fn eval(exp Expression, mut env Environment) ( Expression){
    match typeof(exp).name {
	'Number' { return exp }
	'Symbol' {
	    atom := exp as Atom
	    sym := atom as Symbol
	    return env.find(sym).vars[sym]
	}
	'[]Expression' {
	    elist := exp as List
	    sym := elist[0] as Symbol
	    match sym {
		'quote' {return elist[1] }
		'if' {
		    res := eval(elist[1], mut env)
		    atomb := res as Atom
		    valid := atomb as Bool
		    if valid == true {
			return eval(elist[2], mut env)
		    } else {
			return eval(elist[3], mut env)
		    }
		}
		'set!' {
		    sym1 := elist[1] as Symbol
		    env.find(sym1).vars[sym1] = eval(elist[2], mut env)
		    return Atom(Symbol("ok"))
		}
		'define' {
		    sym1 := elist[1] as Symbol
		    env.vars[sym1] = eval(elist[2], mut env)
		    return Atom(Symbol("ok"))
		}
		'lambda' {
		    return Proc{elist[1], elist[2], env}
		}
		'begin' {
		    mut ev := Expression(Atom(Number(0)))
		    el_len := elist.len
		    for i in 1 .. el_len {
			ex := elist[i]
			ev = eval(Expression(ex), mut env)
		    }
		    return ev
		}
		else {}
	    }
	}
	else {println("Unknown expression type $exp")}
    }
    return Atom(Symbol("eval unknown"))
}

fn (env Environment) find(sym Symbol) Environment {
    if sym in env.vars {
	return env
    }
    return env.outer.find(sym) 
}

fn repl(){
}

fn main() {
    repl()
}
