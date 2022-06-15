package main

// #cgo CFLAGS: -g -Wall
// #include <stdlib.h>
// #include "greeter.h"
import "C"
import (
	"fmt"
	"unsafe"
)

func main() {
	name := C.CString("Gopher")
	defer C.free(unsafe.Pointer(name))

	year := C.int(2018)


	g := C.struct_Greetee{
		name: name,
		year: year,
	}

	ptr := C.malloc(C.sizeof_char * 1024)
	defer C.free(unsafe.Pointer(ptr))

	size := C.greet(name, year, (*C.char)(ptr))
	b := C.GoBytes(ptr, size)
	fmt.Println("greet",string(b))

	size = C.greet2(&g, (*C.char)(ptr))
	b = C.GoBytes(ptr, size)
	fmt.Println("greet2",string(b))

}


