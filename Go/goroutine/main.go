package main

import (
	"fmt"
	"time"
)

func createGoRoutines(ch1, ch2 chan string) {
	go func() {
		for {
			str := <-ch1
			if str == "exit" {
				break
			}
			fmt.Println(str)
		}
		fmt.Println("1 exit")
	}()
	go func() {
		for {
			str := <-ch2
			if str == "exit" {
				break
			}
			fmt.Println(str)
		}
		fmt.Println("2 exit")
	}()
}

func main() {
	ch1 := make(chan string, 5)
	ch2 := make(chan string, 5)
	createGoRoutines(ch1, ch2)
	i := 0
	for {
		i++
		if i > 3 {
			break
		}
		time.Sleep(time.Second)
		ch1 <- "hello"
		ch2 <- "world"
	}
	ch1 <- "exit"
	close(ch1)
	ch2 <- "exit"
	close(ch2)
	time.Sleep(time.Second)
	fmt.Println("main exit")
}
