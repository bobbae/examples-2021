cat words.txt | awk 'NR%1000==0' > words2.txt
