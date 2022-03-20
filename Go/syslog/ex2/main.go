package main

import (
	"io"
	"log"
	"log/syslog"
	"os/exec"
)

func main() {

	// using log/syslog

	l2, err := syslog.New(syslog.LOG_LOCAL0, "bob")
	defer l2.Close()
	if err != nil {
		log.Fatal("error writing syslog!")
	}
	io.WriteString(l2, "writestring hello go")
	l2.Debug("debug hello go")
	l2.Notice("notice hello go")
	l2.Info("info hello go")
	l2.Warning("warn hello go")

	/// using logger

	cmd := exec.Command("logger", "-p", "local0.err", "-t", "bash", "hello bash")

	err = cmd.Run()
	if err != nil {
		log.Fatal("error running cmd!")
	}
}
