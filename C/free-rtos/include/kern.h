/* $Revision: 1.2 $ */

#ifndef _KERN_H_
#define _KERN_H_

#include "cpu.h"

#define MAX_PRIORITY_GROUPS 		32
#define MAX_PRIORITIES_PER_GROUP 	MAX_PRIORITY_GROUPS
#define PRI_TO_GROUP_INDEX(pri)		(((pri) >> 5) & 0x1f)
#define PRI_TO_PRI_INDEX(pri)		((pri) & 0x1f)

#define MIN_PRIORITY 			0
#define MAX_PRIORITY ((MAX_PRIORITY_GROUPS * MAX_PRIORITIES_PER_GROUP) - 1)

#ifndef ASM_LANGUAGE
struct tcb {
	context_t       tcb_context;
	char           *tcb_name;
	int             (*tcb_main) ();
	int             (*restart) ();
	int             tcb_stkbeg;
	int             tcb_stkend;
	int             tcb_stksiz;
	int             tcb_group_index;
	int             tcb_pri_index;
	int             tcb_stat;
	int             tcb_arg;
	char           *tcb_stdio_buf;
	struct tcb     *tcb_next;	/* linked list */
};
typedef struct tcb tcb_t;

struct sem {
	u_int           sem_gindex;
	u_int           sem_pindex[MAX_PRIORITIES_PER_GROUP];
	int             sem_count;
};

typedef struct sem sem_t;

int             idle_task(void);
void            kern_init(void);
void            task_main(void);
void            restart(tcb_t * tcb);
int 		spawn(tcb_t * new_tcb, char *name, char *stack,
			int (*code) (), int stksiz, int priority, int arg);
int             ready(tcb_t * task);
int             suspend(tcb_t * task);
tcb_t          *pick(sem_t * sem);
void            reschedule(context_t * trap_context);
void            yield(void);
void            exec(tcb_t * task);
void            finish(tcb_t * task);
void            exit(int code);
void            ps_print(tcb_t * tcb);
int             ps(void);

#endif

/* flag bits used for tcb stat */
#define TASK_INACTIVE		0x0
#define TASK_READY 		0x1
#define TASK_SUSPENDED 		0x2
#define TASK_RUNNING		0x4
#define TASK_STATE_MASK		0x7

#endif
