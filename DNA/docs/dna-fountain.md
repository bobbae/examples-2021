# DNA Fountain Notes

## Code

Code is available at https://github.com/TeamErlich/dna-fountain. 

## Data

Sequencing data are available at 

* www.ebi.ac.uk/ena/data/view/PRJEB19305 
* www.ebi.ac.uk/ena/data/view/PRJEB19307

They are FTP'ed and placed under data1 and data2 directories here.

## Install dependencies

```
pip install Cython numpy reedsolo tqdm logging scipy biopython
```

Build Cython binaries

```
python setup.py build_ext --inplace
```

## PEAR

Clone and build PEAR.

Note stevschmid version works on Linux. tseemann works on MacOSX.

```
cd
git clone https://github.com/tseemann/PEAR
cd PEAR
make
cp src/pear ~/bin

```

Optionally run tests 


```
cd test
./test.py
```



Be sure to have ~/bin in PATH

```
export PATH=$PATH:~/bin
```


Run PEAR

```
pear -f data1/ERR1816980_1.fastq \
    -r data1/ERR1816980_2.fastq \
    -o data1/ERR1816980_all.fastq
```

## Filter

Retain only fragments with 152nt as the original oligo size.

```
awk '( NR % 4 == 2 && length($0) == 152 ) { print $0 }' \
    data1/ERR1816980_all.fastq.assembled.fastq > data1/all.fastq.good
```

Sort to prioritize highly abundant reads.

```
sort -S4G data1/all.fastq.good | uniq -c > data1/all.fastq.good.sorted
```


Install GNU sed by doing

```
brew install gnu-sed
```

Then,

```
gsed -r 's/^\s+//' data1/all.fastq.good.sorted | \
   sort  -r -n -k1 -S4G > data1/all.fastq.good.sorted.quantity
```

And

```
cut -f2 -d' ' data1/all.fastq.good.sorted.quantity | \
  grep -v 'N' > data1/all.fastq.good.sorted.seq
```

## Decode

Now ready to decode,

```
python receiver.py -f data1/all.fastq.good.sorted.seq  \
   --header_size 4 --rs 2 --delta 0.001 --c_dist 0.025 \
   -n 67088 -m 3 --gc 0.05 --max_hamming 0 \
   --out data1/decoder.out.bin
```


Original files:

```
md5 input_files/info_to_code/* input_files/info_to_code/*/*

MD5 (input_files/info_to_code/kolibri.img) = e47c869a513f7b79ed5e1da99dac3a06
MD5 (input_files/info_to_code/pioneer.svg) = eb6ec1f7c638ae2810f6497776ae21cb
MD5 (input_files/info_to_code/run-kolibri-distr.sh) = 38b036655aeef11cb6dc2bb9ff78a19c
MD5 (input_files/info_to_code/shannon.pdf) = c1f7c5fffb706d1f685b97fcdca04c05
MD5 (input_files/info_to_code/the_arrival_of_a_train.3pg) = be1f35c7b75a8802313fff5f0bf4f414
md5: input_files/info_to_code/zipbomb: Is a directory
MD5 (input_files/info_to_code/zipbomb/42.zip) = 1df9a18b18332f153918030b7b516615
MD5 (input_files/info_to_code/zipbomb/README) = 445ada2469387f326c2a392d1c21129f
```

Decoded:

```
md5 info_to_code/*  info_to_code/*/*

MD5 (info_to_code/amazon_gift_card.tab) = 6b4a64a68ca396d41b014cf534166901
MD5 (info_to_code/kolibri.img) = e47c869a513f7b79ed5e1da99dac3a06
MD5 (info_to_code/pioneer.svg) = eb6ec1f7c638ae2810f6497776ae21cb
MD5 (info_to_code/run-kolibri-distr.sh) = 38b036655aeef11cb6dc2bb9ff78a19c
MD5 (info_to_code/shannon.pdf) = c1f7c5fffb706d1f685b97fcdca04c05
MD5 (info_to_code/the_arrival_of_a_train.3pg) = be1f35c7b75a8802313fff5f0bf4f414
md5: info_to_code/zipbomb: Is a directory
MD5 (info_to_code/zipbomb/42.zip) = 1df9a18b18332f153918030b7b516615
MD5 (info_to_code/zipbomb/README) = 445ada2469387f326c2a392d1c21129f
```


## Encoding

```
tar -b1 -czvvf info_to_code.tar.gz ./info_to_code/

```

Pad the file to 512 byte boundary.

```
truncate -s NEWSIZE info_to_code.tar.gz
```

where NEWSIZE is ((ORIGINAL / 512.0) + 1) * 512

Encode this file:

```
python encode.py --file_in info_to_code.tar.gz \
   --size 32 -m 3 --gc 0.05 --rs 2 --delta 0.001 --c_dist 0.025  \
   --out info_to_code.tar.gz.dna --stop 72000
```

The output is FASTA file `info_to_code.tar.gz.dna` containing 72000 oligos.

Post-process it by adding annealing sites.  
152nt payload flanked by landing sites for sequencing primers:

```
cat info_to_code.tar.gz.dna | grep -v '>' | awk '{ print "GTTCAGAGTTCTACAGTCCGACGATC"$0"TGGAATTCTCGGGTGCCAAGG"} ' > info_to_code.tar.gz.dna_order
```

Each oligo is 152nt payload plus 47nt sites (26nt leading, 21 trailing) 
which is 199nt.  The paper says 200nt(?).  

The file `info_to_code.tar.gz.dna_order` is ready to be synthesized.

