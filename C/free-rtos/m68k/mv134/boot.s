|
| $Revision: 1.1.1.1 $
|

	.text
	.even
	.globl	_init, __start

__start:
boot:
	movew	#0x2700,sr
	movel	#0x300000,sp
	jsr	_init
	jsr	_halt		| should never get here
