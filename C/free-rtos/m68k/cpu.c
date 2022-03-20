/* $Revision: 1.2 $ */

#include "cpu.h"

void
ctx_fill(context_t * tp, int pc, int sr, int flag, int fp, int sp)
{
	tp->ctx_pc = pc;
	tp->ctx_sr = sr;
	tp->ctx_flag = flag;
	tp->ctx_reg.fp = fp;	/* XXX for gdb stack trace */
	tp->ctx_reg.sp = sp;
}

int
ctx_stack(context_t * tp)
{
	return (tp->ctx_reg.sp);
}
