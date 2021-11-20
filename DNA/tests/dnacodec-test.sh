#!/bin/bash
DIR=/tmp/dnacodec-test
echo clean
rm -rf $DIR
mkdir $DIR
cp /etc/hosts $DIR/zzz
cd $DIR
echo we are in `pwd`
CURDIR=`pwd`
echo split
split -b 150 zzz
dnacodec -pad 150 -input xac -output xac.1
mv xac.1 xac
echo zfec
for i in x*; do zfec $i; done
echo base64
for i in *fec; do base64 $i > $i.b64; done
echo md5
for i in *.b64; do  echo $i | sed -e 's/\.fec.*/==/' > $i.b64md5; done
for i in *.b64; do  cat $i >> $i.b64md5; done
for i in  *.b64; do md5sum $i | awk '{print $1}' >> $i.b64md5; done
echo dnacodec
for i in *.b64md5; do dnacodec -encode -input $i -output $i.dna; done
echo fasta
for i in *.dna; do echo '> ' $i >> zzz.fasta; cat $i >> zzz.fasta; echo >> zzz.fasta; done
ls -l *.dna | awk ' { tl += $5} END { printf("%d", tl/NR); }' > avglen.txt
ls -l $DIR/zzz.fasta
echo wgsim
wgsim -N 10000 -e 0 -s 0 -r 0 -R 0 -X 0 -A 0 -d `cat avglen.txt` -1 250 -2 250  zzz.fasta out1.fq out2.fq
ls -l out*fq
echo pear
pear -f out1.fq -r out2.fq -o all.fq
echo awk
# TODO? we are not checking length == avglen
awk '(NR % 4 == 2 ) { print $0 }' *.assembled.fastq > out.good
echo dnacodec
dnacodec -receive -input out.good 
for i in *.received; do base64 --decode --output $i.decoded --input $i; done
