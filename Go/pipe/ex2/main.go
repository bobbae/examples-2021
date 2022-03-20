package main
import (
	"fmt"
	"os"
	"os/exec"
)

func pipe_commands(commands ...*exec.Cmd) ([]byte, error) {
	for i, command := range commands[:len(commands) - 1] {
		out, err := command.StdoutPipe()
		if err != nil {
			return nil, err
		}
		command.Start()
		commands[i + 1].Stdin = out
	}
	final, err := commands[len(commands) - 1].Output()
	if err != nil {
		return nil, err
	}
	return final, nil
}

func main() {
	var dirs []string
	if len(os.Args) > 1 {
		dirs = os.Args[1:]
	} else {
		dirs = []string{"."}
	}
	for _, dir := range dirs {
		ls := exec.Command("ls", "-Ft", dir)
		grep_dir := exec.Command("grep", "-v", "/$")
		grep_exe := exec.Command("grep", "-v", "\\*$")
		head := exec.Command("head", "-n", "1")
		output, err := pipe_commands(ls, grep_dir, grep_exe, head)
		if err != nil {
			fmt.Println(err.Error())
		} else {
			fmt.Print(string(output))
		}
	}
}
