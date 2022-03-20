/* $Revision: 1.1.1.1 $ */

#ifndef _BSP_H_
#define _BSP_H_

void tty_init (void);
void tty_tx_handle (void);
void tty_rx_handle (void);
void tty_rxerr_handle (void);
void tty_write (volatile char *tty, char ch);
void delay (int val);
void tty_poll_write (volatile char *tty, char ch);
void timer_handle (void);
void timer_init (void);
void tty1_init (void);
void tty1_handle (void);
void tty1_putc (char ch);
void tty1_puts (char *str);
void mem_init (void);

#endif
