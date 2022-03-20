package slackgist

import (
	"flag"
	"fmt"
	"os"

	"github.com/bobbae/q"
	"github.com/google/go-github/github"
)

type Configuration struct {
	Name, GrpcAddress, RestAddress, Owner *string
	Repo, User, Buildcmd, Dir, Channel    *string
	UIAddress                             *string
	Delay                                 *int
	Last, Once, Slack                     *bool
	Pass, GithubToken, WebhookUrl         string
}

type GSContext struct {
	Config                   Configuration
	LastHash                 []byte
	NumBuildFails, NumBuilds int
	LastGist                 github.Gist
	LastSlack                slackPayload
}

var GSC = &GSContext{LastHash: make([]byte, 20)}
var Conf = &GSC.Config

func InitConfig() {
	Conf.GrpcAddress = flag.String("grpc", ":18732", "grpc address")
	Conf.RestAddress = flag.String("rest", ":18733", "rest address")
	Conf.UIAddress = flag.String("ui", ":18734", "ui address")
	Conf.Owner = flag.String("owner", "", "repo owner")
	Conf.Repo = flag.String("repo", "", "repo name")
	Conf.User = flag.String("user", "", "user name")
	Conf.Buildcmd = flag.String("buildcmd", "./build.sh", "build command")
	Conf.Dir = flag.String("dir", "", "directory to clone into")
	Conf.Channel = flag.String("channel", "#swbuild", "slack channel")
	Conf.Delay = flag.Int("delay", 10, "seconds to delay before polling")
	Conf.Last = flag.Bool("last", false, "build last push and exit")
	Conf.Slack = flag.Bool("slack", true, "send slack message")
	Conf.Pass = os.Getenv("GITHUB_PASS")
	Conf.GithubToken = os.Getenv("GITHUB_TOKEN")
	Conf.WebhookUrl = os.Getenv("SLACK_WEBHOOK")
	var falsey = false
	Conf.Once = &falsey
}

func ValidateConf() error {
	if Conf.GithubToken == "" {
		return fmt.Errorf("missing GITHUB_TOKEN")
	}
	if Conf.WebhookUrl == "" {
		return fmt.Errorf("missing SLACK_WEBHOOK env var")
	}
	if *Conf.Owner == "" {
		return fmt.Errorf("owner required")
	}
	if *Conf.Repo == "" {
		return fmt.Errorf("missing repo parameter")
	}
	if *Conf.Dir == "" {
		return fmt.Errorf("missing dir to clone into")
	}
	if Conf.Pass == "" {
		q.Q("ok, missing GITHUB_PASS, will use ssh")
	}
	if *Conf.Last {
		*Conf.Once = true
		q.Q("will build only once.")
	}
	q.Q(Conf)
	return nil
}
