package main

import (
	"context"
	"flag"
	"log"
	"os"
	"time"

	"github.com/bobbae/q"
	"github.com/bobbae/slackgist"
	"github.com/bobbae/slackgist/server"
	"github.com/google/go-github/github"
	"golang.org/x/oauth2"
)

func main() {
	slackgist.InitConfig()
	flag.Parse()
	if err := slackgist.ValidateConf(); err != nil {
		log.Fatalln("error", err)
	}
	ts := oauth2.StaticTokenSource(
		&oauth2.Token{AccessToken: slackgist.Conf.GithubToken},
	)
	ctx := context.Background()
	tc := oauth2.NewClient(ctx, ts)
	client := github.NewClient(tc)
	// list all repositories for the authenticated user
	repos, _, err := client.Repositories.List(ctx, "", nil)
	if err != nil {
		log.Fatalln("can't get repo list", err)
	}
	q.Q(len(repos))
	rep, _, err := client.Repositories.Get(ctx, *slackgist.Conf.Owner, *slackgist.Conf.Repo)
	if err != nil {
		log.Fatalln("can't get repo info", err)
	}
	go func() {
		log.Println("rest server", *slackgist.Conf.RestAddress)
		err := server.ListenAndServeREST(*slackgist.Conf.RestAddress, *slackgist.Conf.GrpcAddress)
		if err != nil {
			log.Fatalln(err)
		}
	}()
	q.Q(*slackgist.Conf.RestAddress)
	go func() {
		log.Println("grpc server", *slackgist.Conf.GrpcAddress)
		err := server.ListenAndServeGRPC(*slackgist.Conf.GrpcAddress)
		if err != nil {
			log.Fatalln(err)
		}
	}()
	q.Q(*slackgist.Conf.UIAddress)
	go func() {
		log.Println("ui server", *slackgist.Conf.UIAddress)
		err := server.ListenAndServeUI(*slackgist.Conf.UIAddress)
		if err != nil {
			log.Fatalln(err)
		}
	}()
	q.Q(*slackgist.Conf.GrpcAddress)
	if *slackgist.Conf.Once {
		q.Q("once")
		if err = slackgist.RunBuild(client); err != nil {
			q.Q(err)
		}
		os.Exit(0)
	}
	lastPush := rep.PushedAt.Time
	q.Q("lastPush", rep.PushedAt.String())
	for {
		time.Sleep(time.Duration(*slackgist.Conf.Delay) * time.Second)
		rep, _, err = client.Repositories.Get(ctx, *slackgist.Conf.Owner, *slackgist.Conf.Repo)
		if err != nil {
			q.Q(err)
			continue
		}
		if !lastPush.Before(rep.PushedAt.Time) {
			//q.Q("no change since ", rep.PushedAt.Time.String())
			continue
		}
		q.Q(lastPush, rep.PushedAt.String())
		lastPush = rep.PushedAt.Time
		q.Q(lastPush.String(), slackgist.GSC.NumBuilds)
		if err = slackgist.RunBuild(client); err != nil {
			q.Q(err)
		}
	}
}
