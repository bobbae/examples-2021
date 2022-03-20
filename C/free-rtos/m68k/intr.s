|
| $Revision: 1.1.1.1 $
|

|
| ASM interrupt handlers
|

	.globl 	_tty_rx_intr, _tty_rx_handle

_tty_rx_intr:	
	moveml	d0-d7/a0-a7,sp@-
	jsr	_tty_rx_handle
	moveml	sp@+,d0-d7/a0-a7
	rte

	.globl 	_tty_rxerr_intr, _tty_rxerr_handle

_tty_rxerr_intr:	
	moveml	d0-d1/a0-a1,sp@-
	jsr	_tty_rxerr_handle
	moveml	sp@+,d0-d1/a0-a1
	rte


	.globl 	_tty_tx_intr, _tty_tx_handle

_tty_tx_intr:	
	moveml	d0-d1/a0-a1,sp@-
	jsr	_tty_tx_handle
	moveml	sp@+,d0-d1/a0-a1
	rte

	.globl	_timer_intr, _timer_handle

_timer_intr:
	moveml	d0-d1/a0-a1,sp@-
	jsr	_timer_handle
	moveml	sp@+,d0-d1/a0-a1
	rte

|	.globl _tty1_intr, _tty1_handle
|	
|_tty1_intr:
|	moveml	d0-d1/a0-a1,sp@-
|	jsr	_tty1_handle
|	moveml	sp@+,d0-d1/a0-a1
|	rte
