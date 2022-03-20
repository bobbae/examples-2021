/* $Revision: 1.2 $ */

/*
 * Copyright (c) 1983 Regents of the University of California. All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 3. All advertising
 * materials mentioning features or use of this software must display the
 * following acknowledgement: This product includes software developed by the
 * University of California, Berkeley and its contributors. 4. Neither the
 * name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * UNIX stdio-like I/O package (polling version). These are useful kernel
 * debug I/O messages. Tasks that need to be preempted should use non-polling
 * versions -- notably, getchar() instead of kgetchar(), and gets() instead
 * of kgets().
 */

#include "varargs.h"
#include "common.h"
#include "kern.h"
#include "sem.h"

void            kstdio_init(void);
int             kgetchar(void);
char           *kgets(void);
void            kputchar(char ch);
void            do_printf(void (*do_putc) (char), va_list pvar);
void            dump(int addr, int len);
void            panic(char *msg);

extern volatile char *tty;

int             paniced = 0;
char            panic_msg[BUFSIZ];
char           *panic_str = panic_msg;

void
kstdio_init(void)
{
}

extern char     cinput[];
extern int      cinput_i;
extern int      coutput_i;

int
kgetchar(void)
{
	int             ch;

	if (cinput_i == coutput_i)
		return -1;
	ch = cinput[coutput_i];
	coutput_i = (coutput_i + 1) % BUFSIZ;
	return (ch);
}

char           *
kgets(void)
{
	static char     buf[BUFSIZ];
	static char    *str = buf;
	char            ch;
	char           *org = str;

	do {
		if ((ch = kgetchar()) < 0)
			continue;
		*str = ch;
		str++;
	}
	while (!isnewline(ch));
	*str = '\0';
	return org;
}

void
kputchar(char ch)
{
	if (ch == '\n')
		tty_poll_write(tty, '\r');
	if (ch == '\r')
		tty_poll_write(tty, '\n');
	tty_poll_write(tty, ch);
	if (paniced)		/* put a copy in panic memory location */
		*panic_str++ = ch;
}

char            hexdigits[] = "0123456789abcdef";

static char    *
ksprintn(long unsigned int ul, int base, int *lenp)
{
	static char     buf[20];
	register char  *p;

	p = buf;
	*p = '\0';
	do {
		*++p = hexdigits[ul % base];
	}
	while (ul /= base);
	if (lenp)
		*lenp = p - buf;
	return p;
}

void
kprintf(va_alist)
va_dcl
{
	va_list         pvar;

	va_start(pvar);
	do_printf(kputchar, pvar);
	va_end(pvar);
}

void
do_printf(void (*do_putc) (char), va_list pvar) 
{
	char           *p;
	int             ch;
	unsigned long   ul;
	int             base, lflag, tmp, width, upper;
	char            padc;
	static char     dummy[2] =
	{'a', 'b'};
	static char     buffer[BUFSIZ];
	char           *buf = buffer;
	int             i, len;
	char           *fmt;

	fmt = va_arg(pvar, char *);
	*buf = '\0';
	for (;;) {
		padc = ' ';
		width = 0;
		while ((ch = *(unsigned char *) fmt++) != '%') {
			if (ch == '\0') {
				*buf++ = '\0';
				goto kprintf_exit;
			}
			*buf++ = ch;
		}
		lflag = 0;
		upper = 0;

reswitch:

		switch (ch = *(unsigned char *) fmt++) {
		case '0':
			padc = '0';
			goto reswitch;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			for (width = 0;; ++fmt) {
				width = width * 10 + ch - '0';
				ch = *fmt;
				if (ch < '0' || ch > '9')
					break;
			}
			goto reswitch;
		case '.':
			goto reswitch;
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'c':
			*buf++ = va_arg(pvar, int);
			break;
		case '*':	/* only valid for 's' */
			if (*(unsigned char *) fmt != 's')
				panic("kprintf: we're too stupid");
			fmt++;
			width = va_arg(pvar, int);
			/* fall through to the 's' case */
		case 's':
			p = va_arg(pvar, char *);
			while (ch = *p++) {
				*buf++ = ch;
				if (width != 0 && --width == 0)
					break;
			}
			while (width > 0) {
				width--;
				*buf++ = padc;
			}
			break;
		case 'd':
			ul = lflag ? va_arg(pvar, long) : va_arg(pvar, int);
			if ((long) ul < 0) {
				*buf++ = '-';
				ul = -(long) ul;
			}
			base = 10;
			goto number;
		case 'o':
			ul = lflag ? va_arg(pvar, long) : va_arg(pvar, u_int);
			base = 8;
			goto number;
		case 'u':
			ul = lflag ? va_arg(pvar, u_long) : va_arg(pvar, u_int);
			base = 10;
			goto number;
		case 'h':
			/* FALLTHROUGH */
		case 'H':
			if (*(buf - 1) != 'x' || *(buf - 1) != 'X' || *(buf - 2) != '0') {
				*buf++ = '0';
				*buf++ = ch == 'h' ? 'x' : 'X';
			}
			goto hex_print;
		case 'X':
			upper = 1;
			/* FALLTHROUGH */
hex_print:
		case 'x':
			ul = lflag ? va_arg(pvar, u_long) : va_arg(pvar, u_int);
			base = 16;
number:	
			p = ksprintn(ul, base, &tmp);
			if (width && (width -= tmp) > 0)
				while (width--)
					*buf++ = padc;
			while (ch = *p--)
				*buf++ = (upper ? toupper(ch) : ch);
			*buf = '\0';
			break;
		case '%':
			*buf++ = ch;
			break;
		default:
			panic("kprintf: we're too stupid");
			/* NOTREACHED */
		}
	}
kprintf_exit:
	len = strlen(buffer);
	for (i = 0; i < len; i++)
		(*do_putc) (buffer[i]);
}

/* table of  printable ASCII characters */
u_char         *ascii_table = (u_char *) " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.";

void
dump(int addr, int len)
{
	int             i, j;
	u_char          buf[33];/* 32 chars plus NUL */
	u_char         *bp = buf;
	u_char         *ap;
	u_long          val;

	/* assume 4 bytes alignment */
	for (i = 0; i < len; i += 4, addr += 4) {
		if ((i % 0x10) == 0) {
			if (i != 0) {
				kprintf("    %s\n", buf);
				bp = buf;
			}
			kprintf("  %8.0h:  ", addr);
		}
		ap = (u_char *) addr;
		val = (u_long) (((int) *ap) << 24 |
				((int) *(ap + 1)) << 16 |
				((int) *(ap + 2)) << 8 |
				((int) *(ap + 3)));
		kprintf("%8.0x ", val);
		for (ap = (u_char *) addr, j = 0; j < 4; j++, ap++, bp++) {
			if ((int) (*ap) < 128 && (int) (*ap) > 31)
				*bp = ascii_table[(int) (*ap) - 32];
			else
				*bp = '.';
		}
		*bp = '\0';
	}
	kprintf("    %s\n", buf);
}

void
panic(char *msg)
{
	paniced = 1;		/* signal panic msg to be copied to memory */
	kprintf("\nPANIC: ");
	kprintf(msg);
	kputchar('\n');
	halt();
}

#ifdef DEBUG
void
debug(va_alist)
va_dcl
{
	va_list         pvar;

	va_start(pvar);
	do_printf(kputchar, pvar);
	va_end(pvar);
}

#else
void
debug(char *fmt,...)
{
}

#endif
