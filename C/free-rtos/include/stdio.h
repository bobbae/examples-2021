/* $Revision: 1.2 $ */

#ifndef _STDIO_H_
#define _STDIO_H_

#include "varargs.h"

void            kstdio_init(void);
int             kgetchar(void);
char           *kgets(void);
void            kputchar(char ch);
void            kprintf(char *fmt,...);
void            do_printf(void (*do_putc) (char), va_list pvar);
void            dump(int addr, int len);
void            panic(char *msg);

void            stdio_init(void);
int             task_stdio_init(void);
void            record_char(char ch);
void            putchar(char ch);
int             getchar(void);
char           *gets(void);
void            printf(char *fmt,...);

#endif
