CFLAGS = -g

ucat: main.c argparse.o
	gcc -rdynamic -g main.c argparse.o -lpthread -o $@ -ldl

argparse.o: argparse.c argparse.h

fmt: main.c
	clang-format -i main.c
