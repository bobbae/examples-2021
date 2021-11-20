#!/bin/bash
if [ $# -lt 1 ]; then
	echo need filename
	exit 1
fi
mkdir -p /tmp/dna-test
rm -rf /tmp/dna-test/*
cd /tmp/dna-test
split -b 150 $1
fl=`echo x*`
prefix=`basename $1`
for i in x*;do  truncate -s 150 $i ; done
for i in x*;do  zfec $i; done
for i in *.fec; do  base64 -i $i -o $i.b64; done

#for i in *.b64; do  echo $i==>>$i.cksum; cat $i>>$i.cksum; dnacrc $i>>$i.cksum;done
for i in *.b64; do  echo $i==>>$i.cksum; cat $i>>$i.cksum; md5sum $i | awk '{print $1}'>>$i.cksum;done

for i in *.cksum; do  dnacodec -encode -input $i -output $i.dna; done
for i in *.dna; do  echo '>' $i >> $prefix.fasta; cat $i >> $prefix.fasta; echo >> $prefix.fasta; done

#wgsim -N 5000 -e 0 -d 388 -s 0 -1 200 -2 200 -r 0 -R 0 -X 0  $prefix.fasta $prefix.out1.fq $prefix.out2.fq
wgsim -N 5000 -e 0 -d 484 -s 0 -1 250 -2 250 -r 0 -R 0 -X 0  $prefix.fasta $prefix.out1.fq $prefix.out2.fq

pear -f $prefix.out1.fq -r $prefix.out2.fq -o $prefix.all

#awk '( NR % 4 == 2 && length($0) == 388 ) { print $0 }' $prefix.all.assembled.fastq > $prefix.all.good
awk '( NR % 4 == 2 && length($0) == 484 ) { print $0 }' $prefix.all.assembled.fastq > $prefix.all.good

sort -S4G $prefix.all.good | uniq -c > $prefix.all.good.sorted
gsed -r 's/^\s+//' $prefix.all.good.sorted | sort -r -n -k1 -S4G > $prefix.all.good.sorted.quantity
cut -f2 -d' ' $prefix.all.good.sorted.quantity | grep -v 'N' > $prefix.all.fastq.seq

#uniq $prefix.all.good > $prefix.all.fastq.seq

#dnacodec -receive -checksum crc32 -input $prefix.all.fastq.seq 
dnacodec -receive -checksum md5 -input $prefix.all.fastq.seq  -debug 2>> dnacodec.receive.log

for i in *.received; do base64 -D -i $i -o $i.ufec; done
for i in $fl; do  zunfec $i.*.ufec -o $i.zunfec; done

