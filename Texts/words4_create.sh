cat words.txt | awk '{ print length, $0 }' | sort -n -r | cut -d" " -f2-|head > words4.txt
