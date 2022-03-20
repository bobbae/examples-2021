# slackgist

## Minimalist build tool:

* monitor github repo for a push
* checks out the repo and builds using a script
* creates a gist with build output
* sends a slack message to a channel with URL link to the gist

There are many other tools that basically do the same things.

## Why `slackgist`?  

* it works with insufficient permissions and setup credentials
* all you need is a oath2 token from github account you own and a slack webhook. Both of these can be obtained without special permissions and red tapes.
* you only need `build.sh` in your repo

## setup

You need slack webhook. Get it at  https://my.slack.com/services/new/incoming-webhook
then do `export SLACK_WEBHOOK=YOUR_WEBHOOK`

You also need github token. Get it at https://github.com/settings/tokens
Then do `export GITHUB_TOKEN=YOUR_TOKEN`
And `export GITHUB_PASS=YOUR_PASSWORD`

These can be put in ~/slackgist.env

```
SLACK_WEBHOOK=yourwebhook
GITHUB_TOKEN=yourtoken
```

Running example:

```
$ docker-compose up -d
```

The `docker-compose.yml` shows you an example. You can change `command` parameters in that file to suit your needs.

## docker

Slackgist runs as a docker container. It can also create output which is a docker container by using `Docker out of Docker` method.

## UI

There is a rudimentary UI that shows you the history of builds at port 18734.

## REST API

The REST API that allows you check the status is at port 18733.

## GRPC API

At port 18732.
