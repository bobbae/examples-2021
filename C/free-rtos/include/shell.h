/* $Revision: 1.2 $ */

#ifndef _SHELL_H_
#define _SHELL_H_

typedef struct {
	char           *name;
	char           *args;
	int_func_t      func;
	char           *help;
}

                cmd_t;

int             help(void);
int             md(int argc, char **argv);
int             do_trap(int argc, char **argv);
cmd_t          *valid_cmd(char *s);
char           *skip_spaces(char *str);
void            exec_cmd(cmd_t * cp, char *arg);
void            shell_init(void);
int             shell_task(void);
void            backspace(void);
int             tty1_test(void);
int             readline(char *s);

#endif
