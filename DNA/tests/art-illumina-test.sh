#!/bin/bash

# Download art from
#   https://www.niehs.nih.gov/research/resources/software/biostatistics/art/index.cfm
#GA1 - GenomeAnalyzer I (36bp,44bp)
#GA2 - GenomeAnalyzer II (50bp, 75bp) 
#HS10 - HiSeq 1000 (100bp)
#HS20 - HiSeq 2000 (100bp)
#HS25 - HiSeq 2500 (125bp, 150bp)
#HSXn - HiSeqX PCR free (150bp)
#HSXt - HiSeqX TruSeq (150bp)
#MinS - MiniSeq TruSeq (50bp)
#MSv1 - MiSeq v1 (250bp)
#MSv3 - MiSeq v3 (250bp)
#NS50 - NextSeq500 v2 (75bp)
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
for i in x*; do  truncate -s 150 $i ; done
for i in x*; do  zfec $i; done
for i in *.fec; do  base64 -i $i -o $i.b64; done

#for i in *.b64; do  echo $i==>>$i.cksum; cat $i>>$i.cksum; dnacrc $i>>$i.cksum;done
for i in *.b64; do  echo $i==>>$i.cksum; cat $i>>$i.cksum; md5sum $i | awk '{print $1}'>>$i.cksum;done

for i in *.cksum; do  dnacodec -encode -input $i -output $i.dna; done
for i in *.dna; do  echo '>' $i >> $prefix.fasta; cat $i >> $prefix.fasta; echo >> $prefix.fasta; done

#art_illumina -ss MSv3 -sam -i $prefix.fasta -p  -l 200 -f 10000 -m 388 -s 10 -o $prefix.out
art_illumina -ss MSv1 -sam -i $prefix.fasta -p  -l 200 -f 100000 -m 484 -s 1 -o $prefix.out

pear -f $prefix.out1.fq -r $prefix.out2.fq -o $prefix.all

#awk '( NR % 4 == 2 && length($0) == 388 ) { print $0 }' $prefix.all.assembled.fastq > $prefix.all.good
awk '( NR % 4 == 2 && length($0) == 484 ) { print $0 }' $prefix.all.assembled.fastq > $prefix.all.good

sort -S4G $prefix.all.good | uniq -c > $prefix.all.good.sorted
gsed -r 's/^\s+//' $prefix.all.good.sorted | sort -r -n -k1 -S4G > $prefix.all.good.sorted.quantity
cut -f2 -d' ' $prefix.all.good.sorted.quantity | grep -v 'N' > $prefix.all.fastq.seq

#uniq $prefix.all.good > $prefix.all.fastq.seq

#dnacodec -receive -input $prefix.all.fastq.seq 
dnacodec -receive -checksum md5 -input $prefix.all.fastq.seq  -debug 2>> dnacodec.receive.log

for i in *.received; do base64 -D -i $i -o $i.ufec; done
for i in $fl; do  zunfec $i.*.ufec -o $i.zunfec; done

