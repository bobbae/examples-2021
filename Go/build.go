package slackgist

import (
	"bytes"
	"context"
	"fmt"
	"os"
	"time"

	"github.com/bobbae/q"
	"github.com/codeskyblue/go-sh"
	"github.com/google/go-github/github"
	"gopkg.in/src-d/go-git.v4"
)

func RunBuild(client *github.Client) error {
	var repoURL string
	GSC.NumBuilds++
	if Conf.Pass != "" {
		repoURL = fmt.Sprintf("https://%s:%s@github.com/%s/%s", *Conf.User, Conf.Pass, *Conf.Owner, *Conf.Repo)
	} else {
		repoURL = fmt.Sprintf("git@github.com:%s/%s", *Conf.Owner, *Conf.Repo)
	}
	q.Q(repoURL)
	if err := os.RemoveAll(*Conf.Dir); err != nil {
		q.Q(*Conf.Dir, err)
		GSC.NumBuildFails++
		return err
	}
	if err := os.MkdirAll(*Conf.Dir, 0777); err != nil {
		q.Q(*Conf.Dir, err)
		GSC.NumBuildFails++
		return err
	}
	r, err := git.PlainClone(*Conf.Dir, false, &git.CloneOptions{
		URL:               repoURL,
		RecurseSubmodules: git.DefaultSubmoduleRecursionDepth,
	})
	if err != nil {
		q.Q(err)
		GSC.NumBuildFails++
		return err
	}
	q.Q("cloned", *Conf.Dir)
	ref, err := r.Head()
	if err != nil {
		q.Q(err)
		GSC.NumBuildFails++
		return err
	}
	hash := ref.Hash()
	commit, err := r.CommitObject(hash)
	if err != nil {
		q.Q(err)
		GSC.NumBuildFails++
		return err
	}
	q.Q(commit.Hash)
	h := make([]byte, 20)
	for i := 0; i < 20; i++ {
		h[i] = commit.Hash[i]
	}
	if bytes.Equal(h, GSC.LastHash) {
		q.Q(commit.Hash)
		GSC.NumBuildFails++
		return nil
	}
	l := copy(GSC.LastHash, h)
	if l != 20 {
		q.Q("expected 20 got ", l)
	}
	q.Q(GSC.LastHash)
	var builderr error
	var output []byte
	output, err = sh.Command(*Conf.Buildcmd, sh.Dir(*Conf.Dir)).CombinedOutput()
	if err != nil {
		builderr = err
		q.Q(*Conf.Buildcmd, err, string(output))
		GSC.NumBuildFails++
	}
	q.Q("build", err)
	if *Conf.Slack == false {
		q.Q("skip sending slack message", string(output))
		return nil
	}
	vrai := true
	content := fmt.Sprintf("build hash: %v\ncommit: %v\nbuild output:\n%s", hash, commit, output)
	q.Q(content)
	var errContent, description string
	if builderr == nil {
		description = fmt.Sprintf("build-output-gist-%s", *Conf.Repo)
		errContent = fmt.Sprintf("no error")
	} else {
		description = fmt.Sprintf("ERROR-build-output-gist-%s", *Conf.Repo)
		errContent = fmt.Sprintf("error %v", builderr)
	}
	commitHash := fmt.Sprintf("%v %v", commit, hash)
	input := &github.Gist{
		Description: &description,
		Public:      &vrai,
		Files: map[github.GistFilename]github.GistFile{
			"build-result.txt": {Content: &content},
			"error.txt":        {Content: &errContent},
		},
	}
	q.Q("creating gist")
	gist, _, err := client.Gists.Create(context.Background(), input)
	GSC.LastGist = *input
	if err != nil {
		q.Q(err)
		GSC.NumBuildFails++
		return err
	}
	q.Q("creating slack msg")
	attachment1 := slackAttachment{}
	attachment1.AddField(field{Title: description, Value: *gist.HTMLURL}).AddField(field{Title: "commit", Value: commitHash})
	payload := slackPayload{
		Text:        "From slackgist buildbot @" + time.Now().UTC().String(),
		Username:    "github.com/bobbae/slackgist",
		Channel:     *Conf.Channel,
		IconEmoji:   ":monkey_face:",
		Attachments: []slackAttachment{attachment1},
	}
	errs := sendSlack(Conf.WebhookUrl, "", payload)
	if len(errs) > 0 {
		q.Q(err)
		GSC.NumBuildFails++
		return fmt.Errorf("errors, %v", errs)
	}
	q.Q(payload)
	GSC.LastSlack = payload
	builds = append(builds, Build{URL: *gist.HTMLURL, Commit: string(GSC.LastHash)})
	return nil
}
