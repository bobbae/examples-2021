#!/bin/bash
./rudp -network udp4 -k 10 &
./rudp -network udp4 -s 127.0.0.1:40916 -m 1200 -n 1000000 &
