/* $Revision: 1.2 $ */

#include "common.h"
#include "cpu.h"

extern int      timer_ticks, timer_secs;

init(void)
{
	int             secs;

	mem_init();
	trap_init();
	kstdio_init();
	stdio_init();
	tty_init();
	timer_init();
	kern_init();
	enable_intrs();
#ifdef MV134_TIMER_TEST
	/* for testing only, XXX */
	timer_test_init();
#endif
	/* gdb_init(); XXX if enabled never returns */
	shell_init();
}
