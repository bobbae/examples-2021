/* $Revision: 1.2 $ */

#include "sem.h"

extern tcb_t   *cur_task;
extern int      interrupt_nest;
extern int      need_resched;

void
sem_init(sem_t * sem)
{
	int             s = splhi();
	int             i;

	sem->sem_count = 0;
	sem->sem_gindex = 0;
	for (i = 0; i < MAX_PRIORITIES_PER_GROUP; i++) {
		sem->sem_pindex[0] = 0;
	}
	splx(s);
}

void
sem_p(sem_t * sem)
{
	int             s = splhi();
	tcb_t          *first;

	if (--sem->sem_count < 0) {
		sem->sem_gindex |= (1 << cur_task->tcb_group_index);
		sem->sem_pindex[cur_task->tcb_group_index] |=
			(1 << cur_task->tcb_pri_index);
		debug("sem_p: task %h<%s> yields.\n",
		      cur_task, cur_task->tcb_name);
		splx(s);
		yield();
	}
}

extern tcb_t   *pick(sem_t * sem);

int
sem_v(sem_t * sem)
{
	tcb_t          *task;
	int             s;

	if (sem->sem_gindex == 0)	/* nobody waiting */
		return 0;
	s = splhi();
	if (sem->sem_count++ < 0) {
		/* pick highest priority task waiting on this semaphore */
		task = pick(sem);
		debug("sem_v: pick returns %h\n", task);
		if (!(task->tcb_stat & TASK_SUSPENDED)) {
			debug("sem_v: task %h<%s> stat %h not suspended\n",
			      task, task->tcb_name, task->tcb_stat);
			splx(s);
			return -1;
		}
		sem->sem_gindex &= ~(1 << cur_task->tcb_group_index);
		sem->sem_pindex[cur_task->tcb_group_index] &=
			~(1 << cur_task->tcb_pri_index);
		if (interrupt_nest == 0 &&
		    task->tcb_group_index > cur_task->tcb_group_index &&
		    task->tcb_pri_index > cur_task->tcb_pri_index) {
			debug("sem_v: task %h<%s> made to run.\n",
			      task, task->tcb_name);
			splx(s);
			exec(task);
		} else {
			debug("sem_v: task %h<%s> made ready.\n",
			      task, task->tcb_name);
			splx(s);
			ready(task);
			need_resched++;
		}
	}
	return 1;
}
