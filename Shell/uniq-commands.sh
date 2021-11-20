#!/bin/bash

awk '{ $1=""; print $0 }'  | sort| uniq
