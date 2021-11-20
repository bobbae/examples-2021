#define _GNU_SOURCE
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
        if (argc < 2) {
                exit(1);
        }
        if (seteuid(0) < 0) {
                perror("seteuid");
                exit(errno);
        }
        if (execvpe(argv[1], &argv[1], envp) < 0) {
                perror("exec");
                exit(errno);
        }
        exit(0); /* never reached */
}
