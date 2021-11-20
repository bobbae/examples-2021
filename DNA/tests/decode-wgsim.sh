echo wgsim
wgsim -N 10000 -e 0 -s 0 -r 0 -R 0 -X 0 -A 0 -d 452 -1 250 -2 250  $1 out1.fq out2.fq
ls -l out*fq
echo pear
pear -f out1.fq -r out2.fq -o all.fq
echo awk
# TODO? we are not checking length == avglen
awk '(NR % 4 == 2 ) { print $0 }' *.assembled.fastq > out.good
echo dnacodec
dnacodec -receive -input out.good 
for i in *.received; do base64 --decode < $i > $i.decoded ; done
for i in xaa xab xac xad xae xaf xag xah ; do  zunfec -o $i.zunfec $i*.decoded; done
