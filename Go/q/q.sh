#!/usr/bin/env bash

logfile="q"

if [[ -z "$TMPDIR" ]]; then
	if [[ -e "/system/bin/adb" ]]; then
		# android
		logpath="/data/local/tmp/$logfile"
	else
		logpath="/tmp/$logfile"
	fi
else
    logpath="$TMPDIR/$logfile"
fi

set -euo pipefail

if [[ ! -f "$logpath" ]]; then
    echo 'Q LOG' > "$logpath"
fi

echo logpath is $logpath
tail -100f -- "$logpath"
