# original 

```
usage: word_counter [text_file]
using cinderella.txt
a => 25
able => 2
after => 1
afterwards => 1
again => 10
against => 2
all => 12
allow => 1
allowed => 2
along => 1
also => 2
always => 2
an => 4
and => 140
anew => 1
anger => 1
another => 2
answered => 1
any => 1
anyone => 2
...


```



# modified to sort frequency of words
```
$ v run word_counter.v -sortval -file cinderella.txt
using cinderella.txt
===========================
1 => young
2 => younger
3 => whenever
4 => what
5 => wife
6 => your
7 => will
8 => too
9 => would
10 => were
11 => pigeons
12 => them
13 => took
14 => him
15 => went
16 => this
17 => one
18 => tree
20 => then
21 => when
23 => that
25 => a
26 => was
27 => on
28 => cinderella
30 => in
31 => with
34 => you
36 => had
57 => she
82 => to
87 => her
140 => and
212 => the
```
