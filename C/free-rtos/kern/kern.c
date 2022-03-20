/* $Revision: 1.2 $ */

#include "common.h"
#include "cpu.h"
#include "sem.h"
#include "kern.h"
#include "errno.h"
#include "stdio.h"

extern int      context_swap();
extern char    *kgets();
extern int      first_bit();

extern int      timer_secs;
extern tcb_t    preempt_tcb;
extern int      need_resched, interrupt_nest;

int             errno = 0;
tcb_t           fake_tcb;
tcb_t          *fake_task = &fake_tcb;
tcb_t          *cur_task;
tcb_t          *priority_list[MAX_PRIORITY_GROUPS][MAX_PRIORITIES_PER_GROUP];
u_int           group_ready_mask;
u_int           pri_ready_masks[MAX_PRIORITY_GROUPS];
tcb_t           idle_tcb;
char            idle_stack[1024];
int             cpu_usage;

int
idle_task(void)
{
	u_int           secs;
	u_int           iters;
	u_int           max_iters;
	volatile char   ch;

	secs = timer_secs;
	max_iters = 0;
	iters = 0;
	for (;;) {
		if (timer_secs != secs) {
			if (iters > max_iters)
				max_iters = iters;
			cpu_usage = ((max_iters - iters) * 100) / max_iters;
			debug("%d secs since boot, cpu usage %d%%\n",
			      timer_secs, cpu_usage);
			secs = timer_secs;
			iters = 0;
		} else
			iters++;
	}
}

void
kern_init(void)
{
	int             i, j;

	for (i = 0; i < MAX_PRIORITY_GROUPS; i++) {
		for (j = 0; j < MAX_PRIORITIES_PER_GROUP; j++)
			priority_list[i][j] = 0;
		pri_ready_masks[i] = 0;
	}
	group_ready_mask = 0;
	cur_task = &fake_tcb;
	fake_tcb.tcb_name = "fake";
	debug("kern_init: spawning idle\n");
	if (spawn(&idle_tcb, "idle", idle_stack, idle_task,
		  sizeof(idle_stack), MIN_PRIORITY, 0) < 0)
		panic("kern_init: can't spawn idle task\n");
	ready(&idle_tcb);
}

void
task_main(void)
{
	if (!cur_task)
		panic("task_main: cur_task is nil!");
	debug("task_main: cur_task %h<%s> main(pc) %h\n",
	      cur_task, cur_task->tcb_name, cur_task->tcb_main);
	(*cur_task->tcb_main) ();
	exit(0);
}

void
restart(tcb_t * tcb)
{
	context_t      *tp;

	tp = &tcb->tcb_context;
	ctx_fill(tp, (int) tcb->restart, IPL_ENABLE, 0, 0, tcb->tcb_stkbeg);
	tcb->tcb_stat = TASK_INACTIVE;
	(*tcb->restart) ();
}

int
spawn(tcb_t * new_tcb, char *name, char *stack, int (*code) (),
      int stksiz, int priority, int arg) 
{
	context_t      *tp;
	int             s;

	if (!new_tcb) {
		debug("spawn: new_tcb %h invalid\n", new_tcb);
		return -1;
	}
	if (!stack) {
		debug("spawn: stack %h invalid\n", stack);
		return -1;
	}
	if (priority > MAX_PRIORITY || priority < MIN_PRIORITY) {
		debug("spawn: priority %h invalid\n", priority);
		return -1;
	}
	s = splhi();
	new_tcb->tcb_group_index = PRI_TO_GROUP_INDEX(priority);
	new_tcb->tcb_pri_index = PRI_TO_PRI_INDEX(priority);
	if (priority_list[new_tcb->tcb_group_index][new_tcb->tcb_pri_index]) {
		debug("spawn: priority slot already occupied by %h\n",
		      priority_list[new_tcb->tcb_group_index][new_tcb->tcb_pri_index]);

		splx(s);
		return -1;
	}
	debug("spawn: new_tcb %h<%s> stk %h code %h stksz %h pri %h\n",
	      new_tcb, name, stack, code, stksiz, priority);
	new_tcb->tcb_main = code;
	new_tcb->restart = code;
	tp = &new_tcb->tcb_context;
	new_tcb->tcb_stkbeg = (int) (stack + stksiz);
	ctx_fill(tp, (int) task_main, IPL_ENABLE, 0, 0, new_tcb->tcb_stkbeg);
	new_tcb->tcb_stkend = new_tcb->tcb_stkbeg;
	new_tcb->tcb_stksiz = stksiz;
	new_tcb->tcb_stat = TASK_INACTIVE;
	new_tcb->tcb_name = name;
	new_tcb->tcb_next = 0;
	new_tcb->tcb_stdio_buf = 0;
	new_tcb->tcb_arg = arg;
	priority_list[new_tcb->tcb_group_index][new_tcb->tcb_pri_index] =
		new_tcb;
	splx(s);
	return 0;
}

int
ready(tcb_t * task)
{
	int             s;

	if (!task) {
		if (cur_task == 0)
			panic("ready: cur_task == 0, task == 0");
		task = cur_task;
	}
	if (task->tcb_stat & TASK_READY) {
		debug("ready: %h<%s> already ready\n", task, task->tcb_name);

		return -1;
	}
	s = splhi();
	task->tcb_stat &= ~TASK_STATE_MASK;
	task->tcb_stat |= TASK_READY;
	group_ready_mask |= (1 << task->tcb_group_index);
	pri_ready_masks[task->tcb_group_index] |= (1 << task->tcb_pri_index);
	debug("ready: %h marked ready, group_ready_mask %h\n",
	      task, group_ready_mask);
	splx(s);
	return 0;
}

int
suspend(tcb_t * task)
{
	int             s;

	if (!task) {
		if (cur_task == 0)
			panic("suspend: cur_task == 0, task == 0");
		task = cur_task;
	}
	if (task->tcb_stat & TASK_SUSPENDED) {
		debug("suspend: %h<%s> already suspended\n", task, task->tcb_name);
		splx(s);
		return -1;
	}
	s = splhi();
	task->tcb_stat &= ~TASK_STATE_MASK;
	task->tcb_stat |= TASK_SUSPENDED;
	pri_ready_masks[task->tcb_group_index] &= ~(1 << task->tcb_pri_index);
	if (pri_ready_masks[task->tcb_group_index] == 0)
		group_ready_mask &= ~(1 << task->tcb_group_index);
	debug("suspend: %h<%s> suspended\n", task, task->tcb_name);
	splx(s);
	return 0;
}

tcb_t          *
pick(sem_t * sem)
{
	int             group_index;
	int             pri_index;
	tcb_t          *retval;

	if (sem) {
		group_index = first_bit(sem->sem_gindex);
		pri_index = first_bit(sem->sem_pindex[group_index]);
		debug("pick: sem %h gmask %h gidx %h pmask=%h pidx %h\n",
		      sem, group_ready_mask, group_index,
		      pri_ready_masks[group_index], pri_index);

		retval = priority_list[group_index][pri_index];
	} else {
		group_index = first_bit(group_ready_mask);
		pri_index = first_bit(pri_ready_masks[group_index]);
		debug("pick: gmask %h gidx %h pmask=%h pidx %h\n",
		      group_ready_mask, group_index,
		      pri_ready_masks[group_index], pri_index);

		retval = priority_list[group_index][pri_index];
	}
	return (retval);
}

void
reschedule(context_t * trap_context)
{
	int             s;
	tcb_t          *new_task, *old_task;

	new_task = pick(0);
	if (new_task == cur_task) {
		debug("reschedule: cur %h<%s> == new\n",
		      cur_task, cur_task->tcb_name);
	}
	s = splhi();
	old_task = cur_task;
	cur_task = new_task;
	if (trap_context && interrupt_nest > 0) {
		debug("reschedule: trpctx %x sp %x:%x old_task %h<%s>\n",
		      trap_context, ctx_stack(&old_task->tcb_context),
		      trap_context->ctx_reg.sp,
		      old_task, old_task->tcb_name);
		old_task->tcb_context = *trap_context;
		old_task->tcb_stat &= ~TASK_RUNNING;
		new_task->tcb_stat |= TASK_RUNNING;
		context_intr_swap(&new_task->tcb_context);
	} else {
		old_task->tcb_stat &= ~TASK_RUNNING;
		new_task->tcb_stat |= TASK_RUNNING;
		context_swap(&old_task->tcb_context, &new_task->tcb_context);
	}
	splx(s);
}

void
yield(void)
{
	debug("yield: cur_task %h<%s>\n", cur_task, cur_task->tcb_name);

	if (cur_task == 0)
		panic("yield: cur_task==0");
	suspend(cur_task);
	reschedule(0);
}

void
exec(tcb_t * task)
{
	debug("exec: task %h<%s>\n", task, task->tcb_name);
	ready(task);
	reschedule(0);
}

void
finish(tcb_t * task)
{
	int             s;

	if (!task) {
		if (cur_task == 0)
			panic("finish: cur_task == 0, task == 0");
		task = cur_task;
	}
	debug("finish: task %h<%s>, being suspended and deleted\n",
	      task, task->tcb_name);
	s = splhi();
	cur_task = &fake_tcb;
	suspend(task);
	priority_list[task->tcb_group_index][task->tcb_pri_index] = 0;
	splx(s);
	reschedule(0);
}

void
exit(int code)
{
	/* XXX do something with 'code' */
	finish(0);
}

void
ps_print(tcb_t * tcb)
{
	context_t      *ctx;

	ctx = &tcb->tcb_context;

	kprintf("%10s %8.0h %8.0h %8.0h %4d %4d %4d ",
		tcb->tcb_name, tcb, tcb->tcb_stkbeg, ctx->ctx_reg.sp,
		tcb->tcb_stksiz, tcb->tcb_group_index, tcb->tcb_pri_index);
	if (tcb->tcb_stat == 0) {
		kprintf("INACTIVE\n");
		return;
	}
	if (tcb->tcb_stat & TASK_RUNNING)
		kprintf("RUNNING ");
	if (tcb->tcb_stat & TASK_SUSPENDED)
		kprintf("SUSPEND ");
	if (tcb->tcb_stat & TASK_READY)
		kprintf("READY ");
	kprintf("\n");
}

int
ps(void)
{
	int             i, j;
	tcb_t          *tcb;

	kprintf("%10s %10s %10s %10s %4s %4s %4s %8s\n",
		"NAME", "TCB", "STKTOP", "SP",
		"SSIZ", "GPRI", "LPRI", "STATUS");
	for (i = 0; i < MAX_PRIORITY_GROUPS; i++) {
		for (j = 0; j < MAX_PRIORITIES_PER_GROUP; j++) {
			if (priority_list[i][j] == 0)
				continue;
			ps_print(priority_list[i][j]);
		}
	}
}
