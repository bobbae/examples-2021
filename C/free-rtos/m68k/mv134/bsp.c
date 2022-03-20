/* $Revision: 1.2 $ */

#include "cpu.h"
#include "common.h"
#include "kern.h"
#include "sem.h"
#include "bsp.h"

extern          tty_tx_intr();
extern          tty_rx_intr();
extern          tty_rxerr_intr();
extern          timer_intr();

volatile char  *tty;
volatile char  *tty0;
int             timer_ticks, timer_secs;
int             tty_rx_err;

/*
 * various offsets for mc68901 (Multi-Function Peripheral chip) according to
 * mv134 manual, page 3-7
 */

#define	tty_iera (tty + 0x7)
#define	tty_ierb (tty + 0x9)
#define	tty_imrb (tty + 0x15)
#define tty_vr (tty + 0x17)
#define	tty_tsr (tty + 0x2d)
#define	tty_udr (tty + 0x2f)
#define	tty_imra (tty + 0x13)
#define	tty_tacr (tty + 0x19)
#define	tty_tbcr (tty + 0x1b)
#define	tty_tcdcr (tty + 0x1d)
#define	tty_tcdr (tty + 0x23)
#define tty_tadr (tty + 0x1f)
#define tty_tbdr (tty + 0x21)
#define tty_tddr (tty + 0x25)
#define	tty_ucr (tty + 0x29)
#define	tty_rsr (tty + 0x2b)
#define tty_isra (tty + 0xf)
#define tty_isrb (tty + 0x11)
#define tty_ipra (tty + 0xb)
#define tty_iprb (tty + 0xd)

void
tty_init(void)
{
	int             i;

	/* offset to MFP (mv134 manual, page 3-7) */
	tty0 = tty = (volatile char *) 0xfff80000;

	/* ucr set to: clock devisor 16x 8 bit, no parity one stop bit */
	*tty_ucr = 0x88;

	*tty_isra = *tty_isrb = *tty_ipra = *tty_iprb =
		*tty_ierb = *tty_imrb = 0;

	/* tell mc68901 which vectors to use */
	*tty_vr = 64;		/* vectors start at user trap #0 */

	/* set our interrupt vectors for mc68901 */
	vec_set((64 + 0xa) << 2, tty_tx_intr);
	vec_set((64 + 0xb) << 2, tty_rxerr_intr);
	vec_set((64 + 0xc) << 2, tty_rx_intr);
	vec_set((64 + 0xd) << 2, timer_intr);

	/* enable timer_a, rx, rx_err, and tx_empty intr */
	*tty_iera = 0x10 | 0x8 | 0x4 | 0x20;

	/*
	 * tx_empty  will be our signal - it is set in tty_write and cleared
	 * in tty_tx_handle.
	 */
	*tty_imra = 0x10 | 0x8 | 0x20;

	/* timer_a set to 60hz */
	*tty_tacr = 0x7;
	*tty_tadr = 205;	/* 60hz */
	*tty_tbcr = *tty_tbdr = 0x0;

	/* baud rate set at 9600 */
	*tty_tcdcr = 0x10 | (*tty_tcdcr & 0x7);
	*tty_tcdr = 1;

	*tty_tddr = 0x0;
	*tty_rsr = 1;		/* enable rx */
	*tty_tsr = 1;		/* enable tx */
}

void
tty_tx_handle(void)
{
	*tty_imra &= ~0x4;	/* clear intr request */
}

void
tty_rx_handle(void)
{
	record_char(*tty_udr);
}

void
tty_rxerr_handle(void)
{
	tty_rx_err++;
}

void
tty_write(volatile char *tty, char ch)
{
	char            status;

	status = *tty_imra;

	/* XXX shouldn't really poll */
	while ((status & 0x4) != 0)
		status = *tty_imra;
	*tty_imra |= 0x4;	/* intr me when empty */
	*tty_udr = ch;
}

void
delay(int val)
{
	int             i;

	for (i = 0; i < val; i++);
}

void
tty_poll_write(volatile char *tty, char ch)
{
	char            status;

	status = *tty_tsr;

	while ((status & 0x80) == 0)
		status = *tty_tsr;
	*tty_udr = ch;
}

#ifdef MV134_TIMER_TEST
/* XXX for testing only */
extern sem_t    timer_sem;
#endif

void
timer_handle(void)
{
	timer_ticks++;
	if ((timer_ticks % 60) == 0) {
		timer_secs++;

#ifdef MV134_TIMER_TEST
		/* XXX for testing only */
		sem_v(&timer_sem);
#endif
	}
}

void
timer_init(void)
{
	timer_ticks = 0;
	timer_secs = 0;
}

extern int      _start(), _end();

void
mem_init(void)
{
}
