#!/bin/bash
VERBOSE=${VERBOSE:-0}

if [ ! -f testfile1 ]; then
	gsutil cp gs://gcp-public-data-landsat/index.csv.gz testfile1
fi

ls -lh testfile1
rm testfile

if [ $VERBOSE -eq 0 ]; then
	./ucat > ucat1.log 2>&1 &
	./ucat -f testfile1 -t 127.1 > ucat2.log 2>&1 &
else
	./ucat -v > ucat1.log 2>&1 &
	./ucat -v -f testfile1 -t 127.1 > ucat2.log 2>&1 &
fi

echo wait...
( tail -f -n0 ucat1.log & ) | grep -q "Exiting"
echo done.

ls -lh testfile testfile1

cmp testfile testfile1
#md5sum testfile testfile1

grep -C 10 Exiting ucat1.log ucat2.log
if [ $? -ne 0 ]; then
	exit 1
fi
exit 0
