#
# $Revision: 1.1.1.1 $
#

default: tohex mvbug

tohex: tohex.o
	cc -g -o $@ tohex.o

tohex.o: tohex.c
	cc -g -c tohex.c

mvbug: mvbug.o
	cc -g -o $@ mvbug.o

mvbug.o: mvbug.c
	cc -g -c mvbug.c

depend:

clean:
	rm *.o tohex core mvbug
