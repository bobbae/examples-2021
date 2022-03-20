package main
import (
	"time"
	"fmt"
)

func main() {
	fmt.Println(time.Local)
	t,err := time.Parse("2006 Jan 2 15:04:05 MST", "2014 May 29 10:13:05 PST")
	if err != nil {
		panic(err)
	}
	fmt.Println(t)
	//t = t.Add(-400*time.Hour)
	time.Sleep(1 * time.Second)
	fmt.Println(time.Since(t))


	var month,date, hms string
	n, err := fmt.Sscanf("Jun  2 11:33:22 adf ","%s %s %s", &month,&date,&hms)
	fmt.Println(n,month,date,hms)
	var a,b,c,port string
	n, err=fmt.Sscanf("ksdf sadf --port 9999 --aac-dsdf=.*","%s %s %s %s", &a,&b,&c,&port)
	fmt.Println(port)
}
