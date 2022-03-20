/* $Revision: 1.2 $ */

#ifndef _COMMON_H_
#define _COMMON_H_

#include "conf.h"

typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned char u_char;
typedef int     (*int_func_t) ();
typedef void    (*void_func_t) ();

typedef struct {
	struct queue   *q_next;
	struct queue   *q_prev;
}               queue_t;

#define BUFSIZ 512
#define NULL 0

#endif
