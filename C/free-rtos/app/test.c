/* $Revision: 1.2 $ */

#include "common.h"
#include "cpu.h"
#include "kern.h"
#include "sem.h"
#include "shell.h"
#include "string.h"

#define SHELL_PRI	1000	/* pretty high */
char            shell_stack[1024];
tcb_t           shell_tcb;

extern char    *optarg;
extern int      optind;

extern int      dump();
extern int      trap();

int             help(void);
int             do_trap(int argc, char **argv);
int             md(int argc, char **argv);

int             tty1_test(void);

#define MAX_ARGS 10

static cmd_t    builtins[] =
{
	{"help", "", help, "help command"},
	{"?", "", help, "help command"},
	{"ps", "", ps, "list status of all tasks"},
	{"md", "addr len", md, "display memory"},
	{"trap", "num", do_trap, "trap instruction"},
	{"tty1_test", "", tty1_test, "test tty1"},
	{0}
};

int
help(void)
{
	cmd_t          *cp = builtins;

	kprintf("%20s %20s   %30s\n",
		"COMMAND", "ARGUMENTS", "DESCRIPTION");
	while (cp->name) {
		if (cp->help)
			kprintf("%20s %20s   %30s\n",
			      cp->name, cp->args ? cp->args : "", cp->help);
		cp++;
	}
}

int
md(int argc, char **argv)
{
	if (argc < 3) {
		kprintf("usage: md addr len\n");
		return;
	}
	dump(atoi(argv[1]), atoi(argv[2]));
}

int
do_trap(int argc, char **argv)
{
	if (argc < 2) {
		kprintf("usage: trap num\n");
		return;
	}
	trap(atoi(argv[1]));
}

cmd_t          *
valid_cmd(char *s)
{
	int             i;
	cmd_t          *cp = builtins;
	int             len;

	if (!s)
		return 0;
	while (cp->name) {
		len = strlen(cp->name);
		if (strncmp(cp->name, s, len) == 0)
			return cp;
		cp++;
	}
	return 0;
}


char           *
skip_spaces(char *str)
{
	if (!str)
		return 0;
	while (*str && isspace(*str))
		str++;
	if (*str)
		return str;
	return 0;
}

void
exec_cmd(cmd_t * cp, char *arg)
{
	int             len;
	int             i;
	char           *save;
	char           *argv[MAX_ARGS + 1];

	if (!cp->func)
		return;
	if (!arg)
		return;
	len = strlen(cp->name);
	arg += len;
	argv[0] = cp->name;
	for (i = 1; i < MAX_ARGS; i++) {
		arg = skip_spaces(arg);
		if (arg) {
			save = argv[i] = arg;
			if (arg = index(arg, ' ')) {
				argv[i][arg - save] = '\0';
				arg++;
			}
		} else
			break;
	}
	argv[i] = 0;
	(*cp->func) (i, argv);
}

void
shell_init(void)
{
	if (spawn(&shell_tcb, "shell", shell_stack,
		  shell_task, sizeof(shell_stack), SHELL_PRI, 0) < 0)
		panic("shell_init: spawn failed for shell\n");
	exec(&shell_tcb);
}

int
shell_task(void)
{
	char            buf[BUFSIZ];	/* XXX */
	char           *cp = buf;
	cmd_t          *kp;

	for (;;) {
		kprintf("> ");
		if (readline(cp) < 0)
			continue;
		if (kp = valid_cmd(cp))
			exec_cmd(kp, cp);
		else if (strlen(cp))
			kprintf("Invalid command <%s>\n", cp);
	}
}

void
backspace(void)
{
	kputchar('');
	kputchar(' ');
	kputchar('');
}

/* for testing tty1 only XXX */
int
tty1_test(void)
{
	tty1_puts("hi there\f\r");
}

int
readline(char *s)
{
	char           *cp = s;
	char            ch = '\0';

	while (!isnewline(ch)) {
		*cp++ = ch = getchar();
		switch (ch) {
		case '':	/* bs */
		case 0x7f:	/* del */
			if (--cp > s) {
				backspace();
				cp--;
			}
			break;
		case '':
			while (--cp > s)
				backspace();
			break;
		case '':
			return -1;
		default:
			kputchar(ch);
			break;
		}
	}
	*--cp = '\0';
	return (int) (cp - s);
}
