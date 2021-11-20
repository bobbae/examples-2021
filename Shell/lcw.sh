#!/bin/bash

cat ../txt/words.txt | awk '{ print length, $0 }' | sort -n -r | cut -d" " -f2-| awk -f lcw.awk | head
