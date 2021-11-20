pear -f data1/ERR1816980_1.fastq -r data1/ERR1816980_2.fastq -o out_all.fastq
awk '( NR % 4 == 2 && length($0) == 152 ) { print $0 }' out_all.fastq.assembled.fastq > out_all.fastq.good
sort -S4G out_all.fastq.good | uniq -c > out_all.fastq.good.sorted
gsed -r 's/^\s+//' out_all.fastq.good.sorted | sort -r -n -k1 -S4G > out_all.fastq.good.sorted.quantity
cut -f2 -d' ' out_all.fastq.good.sorted.quantity | grep -v 'N' > out_all.fastq.seq
python receiver.py  -f out_all.fastq.seq --header_size 4 --rs 2 --delta 0.001 --c_dist 0.025 -n 67088 -m 3 --gc 0.05 --max_hamming 0 --out decoder.out.bin
