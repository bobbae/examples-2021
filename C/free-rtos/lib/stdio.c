/* $Revision: 1.2 $ */

#include "varargs.h"
#include "common.h"
#include "kern.h"
#include "sem.h"

extern volatile char *tty;

void            stdio_init(void);
int             task_stdio_init(void);
void            record_char(char ch);
void            putchar(char ch);
int             getchar(void);
char           *gets(void);

char            cinput[BUFSIZ];

int             cinput_i = 0;
int             coutput_i = 0;

sem_t           stdio_sem;

void
stdio_init(void)
{
	sem_init(&stdio_sem);
}

extern tcb_t   *cur_task;
extern tcb_t   *fake_task;

int
task_stdio_init(void)
{
	static char     buffer[BUFSIZ];

	if (!cur_task || cur_task == fake_task)
		return -1;
	cur_task->tcb_stdio_buf = buffer;	/* XXX malloc it! */
	return 1;
}

/* called by BSP-specific tty_rx_handle() */
void
record_char(char ch)
{
	cinput[cinput_i] = ch;
	cinput_i = (cinput_i + 1) % BUFSIZ;
	sem_v(&stdio_sem);
}

extern int      paniced;
extern char    *panic_str;

void
putchar(char ch)
{
	if (paniced) {
		kputchar(ch);
		*panic_str++ = ch;

		return;
	}
	if (ch == '\n')
		tty_write(tty, '\r');
	if (ch == '\r')
		tty_write(tty, '\n');
	tty_write(tty, ch);
}

int
getchar(void)
{
	int             ch;

	for (;;) {
		while (cinput_i == coutput_i)
			sem_p(&stdio_sem);
		if ((ch = kgetchar()) < 0)
			continue;
		return ch;
	}
}

char           *
gets(void)
{
	char           *str;
	char            ch;
	char           *org = str;

	if (task_stdio_init() < 0)
		return 0;
	str = cur_task->tcb_stdio_buf;
	do {
		if ((ch = getchar()) < 0)
			continue;
		*str = ch;
		str++;
	}
	while (!isnewline(ch));
	*str = '\0';
	return org;
}

void
printf(va_alist)
va_dcl
{
	va_list         pvar;

	va_start(pvar);
	do_printf(putchar, pvar);
	va_end(pvar);
}
