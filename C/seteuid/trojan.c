#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	uid_t id;
	uid_t id2;

	id = geteuid();
	printf("eu id %d\n", id);

	id2 = getuid();
	printf("uid %d\n", id2);

	if (seteuid(id) < 0) {
		perror("seteuid");
		exit(1);
	}

	if (argc < 2) {
		printf("Usage: %s command arg1 arg2 ...\n", argv[0]);
		exit(1);
	}

	char *command = argv[1];
	char **argslist;
       
	argslist = &argv[1];

	int status = execvp(command, argslist);

	if (status < 0) {
		perror("execvp");
		exit(1);
	}	
	exit(0);
}
