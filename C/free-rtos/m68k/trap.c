/* $Revision: 1.2 $ */

#include "common.h"
#include "cpu.h"
#include "stdio.h"

extern          trap_default();

void            reg_dump(reg_t * reg);

int             interrupt_nest = 0;
int             need_resched = 0;
int_func_t      trap_funcs[256];
int             trap_func_args[256];

char           *trap_names[] =
{
	"reset SP", "reset PC", "Bus Error", "Addr Error",	/* 0 */
	"Illegal Instruction", "Zero Devide", "CHK", "TrapV",	/* 7 */
	"Privilege Violation", "Trace",	/* 9 */
	"Line 1010 Emul", "Line 1111 Emul",	/* 11 */
	"", "", "", "", "", "", "", "",	/* 19 */
	"", "", "", "", "Spurious", "Auto Vec Level 1",	/* 25 */
	"Auto Vec Level 2", "Auto Vec Level 3", "Auto Vec Level 4",	/* 28 */
	"Auto Vec Level 5", "Auto Vec Level 6", "Auto Vec Level 7",	/* 31 */
	"User Trap 0", "User Trap 1", "User Trap 2", "User Trap 3",	/* 35 */
	"User Trap 4", "User Trap 5", "User Trap 6",	/* 38 */
	"User Trap 7", "User Trap 8", "User Trap 9",
	"User Trap 10", "User Trap 11", "User Trap 12",	/* 44 */
	"User Trap 13", "User Trap 14", "User Trap 15",	/* 47 */
	"", "", ""		/* XXX only upto 50 */
};

void
trap_init(void)
{
	int             i;

	interrupt_nest = 0;
	need_resched = 0;
	for (i = 0; i < 256; i++) {
		/*
		 * XXX skip vector 4  and 9 -- used by vmebug breakpoint.
		 * skip over 0 and 1 since they're reset SP/PC.
		 */
		trap_funcs[i] = 0;
		if (i < 2 || i == 4 || i == 9)
			continue;
		vec_set(i << 2, trap_default);
	}
}

int
trap_catch(excfrm_t * efp, context_t * context)
{
	int             vec;
	int             format;
	reg_t          *reg = &context->ctx_reg;
	excfrm_10_t    *efp10;
	excfrm_11_t    *efp11;

	interrupt_nest++;
	vec = (efp->exc_vec & 0xfff) >> 2;
	if (trap_funcs[vec] != 0) {
		(*trap_funcs[vec]) (trap_func_args[vec]);
		if (need_resched > 0) {
			debug("trap_catch: need_resched %x, sr %x pc %x sp %x\n",
			      need_resched, efp->exc_sr, efp->exc_pc,
			      reg->sp);
			context->ctx_sr = efp->exc_sr;
			context->ctx_pc = efp->exc_pc;
			/* XXX flags? */
			reschedule(context);
			need_resched--;
		}
		interrupt_nest--;
		return (int) reg;	/* return ptr to reg save area */
	}
	format = (efp->exc_vec & 0xf000) >> 12;
	if (vec >= 0 && vec <= 50)
		kprintf("\n\ntrap_catch EXCEPTION: %s (vec# %h) format %d",
			trap_names[vec], vec, format);
	else
		kprintf("\n\ntrap_catch: EXCEPTION (vec# %h) format %d",
			vec, format);
	kprintf(" PC %8.0h SR %4.0h\n", efp->exc_pc, efp->exc_sr);
	kprintf("\ntrap_catch: register dump:\n");
	reg_dump(reg);
	kprintf("\n");
	switch (format) {
	case 11:
		efp11 = (excfrm_11_t *) efp;
		kprintf("Stage B address: %h\n",
			efp11->stage_b_addr);
		kprintf("Data input buffer: %h\n",
			efp11->data_input_buf);
		/* FALLTHROUGH */
	case 10:
		efp10 = (excfrm_10_t *) efp;
		kprintf("Special status word: %h\n",
			efp10->special_status_reg);
		kprintf("Pipe stage C instruction: %h\n",
			efp10->pipe_stage_c_inst);
		kprintf("Pipe stage B instruction: %h\n",
			efp10->pipe_stage_b_inst);
		kprintf("Data cycle fault address: %h\n",
			efp10->data_cycle_fault_addr);
		kprintf("Data output buffer: %h\n",
			efp10->data_output_buf);
		break;
	}
	kprintf("\ntrap_catch: dump of exception frame:\n");
	dump((int) efp, 0x60);
	if (interrupt_nest > 1)
		kprintf("\ntrap_catch: entered %d times\n", interrupt_nest);
	interrupt_nest--;
	halt();
}

void
reg_dump(reg_t * reg)
{
	kprintf("REGISTER DUMP:\n");
	kprintf("    d0: %8.0h    d1: %8.0h    d2: %8.0h    d3: %8.0h\n",
		reg->d0, reg->d1, reg->d2, reg->d3);
	kprintf("    d4: %8.0h    d5: %8.0h    d6: %8.0h    d7: %8.0h\n",
		reg->d4, reg->d5, reg->d6, reg->d7);
	kprintf("    a0: %8.0h    a1: %8.0h    a2: %8.0h    a3: %8.0h\n",
		reg->a0, reg->a1, reg->a2, reg->a3);
	kprintf("    a4: %8.0h    a5: %8.0h    fp: %8.0h    sp: %8.0h\n",
		reg->a4, reg->a5, reg->fp, reg->sp);
}

void
                trap_connect(int vec, int (*func) ( /* ??? */ ), int arg) {
	trap_funcs[vec] = func;
	trap_func_args[vec] = arg;
	vec_set(vec << 2, trap_default);	/* just in case it got
						 * altered */
	debug("trap_connect: vec %h func %h arg %h\n", vec, func, arg);
}

void
trap(int val)
{
	switch (val) {
		case 0:
		asm("trap #0");
		break;
	case 1:
		asm("trap #1");
		break;
	case 2:
		asm("trap #2");
		break;
	case 3:
		asm("trap #3");
		break;
	case 4:
		asm("trap #4");
		break;
	case 5:
		asm("trap #5");
		break;
	case 6:
		asm("trap #6");
		break;
	case 7:
		asm("trap #7");
		break;
	case 8:
		asm("trap #8");
		break;
	case 9:
		asm("trap #9");
		break;
	case 10:
		asm("trap #10");
		break;
	case 11:
		asm("trap #11");
		break;
	case 12:
		asm("trap #12");
		break;
	case 13:
		asm("trap #13");
		break;
	case 14:
		asm("trap #14");
		break;
	case 15:
		asm("trap #15");
		break;
	}
}
