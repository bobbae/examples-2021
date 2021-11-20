#!/bin/bash
set -e
eval `ssh-agent`
ssh-add
/usr/local/bin/slackgist $*
bash
