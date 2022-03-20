package main

import (
	sh "github.com/codeskyblue/go-sh"
)

func main() {
	session := sh.NewSession()
	session.SetEnv("BUILD_ID", "123")
	session.SetEnv("LESS", "LSLSLS")
	session.SetDir("/")
	session.Command("echo", "hello").Run()
	session.Command("env").Run()
	session.ShowCMD = true
}
