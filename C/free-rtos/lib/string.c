/* $Revision: 1.2 $ */

/*
 * Copyright (c) 1987 Regents of the University of California. All rights
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
 * $Revision: 1.2 $ $Id: string.c,v 1.2 1995/08/17 00:59:05 hjb Exp $
 * $Log: string.c,v $
 * Revision 1.2  1995/08/17  00:59:05  hjb
 * indent changes -> to BSD kernel style
 * Revision 1.1.1.1  1995/08/13  20:01:47  hjb FREE_0_2
 * Revision 1.1  1994/11/15  08:07:59  hjb Initial revision
 * 
 */

#include "common.h"
#include "stdio.h"
#include "string.h"

int
islower(char ch)
{
	if (ch >= 'a' && ch <= 'z')
		return 1;
	return 0;
}

int
toupper(char ch)
{
	if (islower(ch))
		return (ch - 0x20);
	return ch;
}

int
isnewline(char ch)
{
	if (ch == '\r' || ch == '\f' || ch == '\n')
		return 1;
	return 0;
}

void
bcopy(char *b1, char *b2, int length)
{
	int             i;

	for (i = 0; i < length; i++)
		*b2++ = *b1++;
}

int
strncmp(char *s1, char *s2, int n)
{
	if (n == 0)
		return (0);
	do {
		if (*s1 != *s2++)
			return (*(unsigned char *) s1 - *(unsigned char *) --s2);
		if (*s1++ == 0)
			break;
	}
	while (--n != 0);
	return (0);
}

char           *
index(char *p, char ch)
{
	for (;; ++p) {
		if (*p == ch)
			return ((char *) p);
		if (!*p)
			return (0);
	}
}

char           *
rindex(char *p, char ch)
{
	char           *save;

	for (save = 0;; ++p) {
		if (*p == ch)
			save = (char *) p;
		if (!*p)
			return (save);
	}
}

int
isspace(char ch)
{
	if (ch == ' ' || ch == '\t')
		return 1;
	else
		return 0;
}

int
hexchar_to_int(char ch)
{
	if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	else if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 10);
	else if (ch >= '0' && ch <= '9')
		return (ch - '0');
	else
		return (-1);	/* error */
}

int
atoi(char *cp)
{
	int             val = 0;
	int             sign = 0;
	int             hex = 0;

	if (!cp)
		return 0;
	for (; *cp; cp++) {
		switch (*cp) {
		case ' ':
		case '\t':
			continue;
		case '-':
			sign++;
			continue;
		case '+':
			cp++;
			continue;
		case '0':
			if (*(cp + 1) && (*(cp + 1) == 'x' || *(cp + 1) == 'X')) {
				hex++;
				cp++;
			}
			continue;
		default:
			break;
		}
		break;
	}
	if (hex) {
		int             val2;

		while (*cp && (val2 = hexchar_to_int(*cp)) >= 0) {
			val = (val << 4) | val2;
			cp++;
		}
	} else {
		while (*cp >= '0' && *cp <= '9') {
			val = val * 10 + *cp++ - '0';
		}
	}
	return (sign ? -val : val);
}

/* get option letter from argument vector */
int             opterr = 1,	/* if error message should be printed */
                optind = 1,	/* index into parent argv vector */
                optopt;		/* character checked for validity */
char           *optarg;		/* argument associated with option */

#define	BADCH	(int)'?'
#define	EMSG	""
#define EOF -1

int
getopt(int nargc, char *const * nargv, char *ostr)
{
	static char    *place = EMSG;	/* option letter processing */
	char           *oli;	/* option letter list index */
	char           *p;

	if (!*place) {		/* update scanning pointer */
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = EMSG;
			return (EOF);
		}
		if (place[1] && *++place == '-') {	/* found "--" */
			++optind;
			place = EMSG;
			return (EOF);
		}
	}			/* option letter okay? */
	if ((optopt = (int) *place++) == (int) ':' ||
	    !(oli = index(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option, assume it
		 * means EOF.
		 */
		if (optopt == (int) '-')
			return (EOF);
		if (!*place)
			++optind;
		if (opterr) {
			if (!(p = rindex(*nargv, '/')))
				p = *nargv;
			else
				++p;
			(void) kprintf("%s: illegal option -- %c\n",
				       p, optopt);
		}
		return (BADCH);
	}
	if (*++oli != ':') {	/* don't need argument */
		optarg = 0;
		if (!*place)
			++optind;
	} else {		/* need an argument */
		if (*place)	/* no white space */
			optarg = place;
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			if (!(p = rindex(*nargv, '/')))
				p = *nargv;
			else
				++p;
			if (opterr)
				(void) kprintf("%s: option requires an argument -- %c\n",
					       p, optopt);
			return (BADCH);
		} else		/* white space */
			optarg = nargv[optind];
		place = EMSG;
		++optind;
	}
	return (optopt);	/* dump back option letter */
}

int
strlen(char *str)
{
	char           *s;

	for (s = str; *s; ++s);
	return (s - str);
}

#if 0				/* XXX gcc built-in */

char           *
strcpy(char *to, char *from)
{
	char           *save = to;

	for (; *to = *from; ++from, ++to);
	return (save);
}

int
strcmp(char *s1, char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == 0)
			return 0;
	return (*(u_char *) s1 - *(u_char *)-- s2);
}

#endif
