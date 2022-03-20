|
| $Revision: 1.1.1.1 $
|

#include "cpu.h"
#include "kern.h"

	.text
	.even

|
| halt() never returns -- busy loop
|

	.globl	_halt

_halt:
	movew	#IPL_LOCK,sr
	stop	#IPL_LOCK
	jmp	_halt


|
| default trap handler
|

	.globl	_trap_default, _trap_catch

_trap_default:

	movew	#IPL_LOCK,sr
        subql   #8,sp                   | make space for context_t cruft
        moveml  d0-d7/a0-a7,sp@-        | save all regs: 64 bytes
        addl    #20,sp@(SP_OFF)
        movel   sp,sp@-                 | push pointer to reg save area
        movel   sp,a0
        addl    #76,a0                  | 64 + 8 + 4 == 76
        pea     a0@                     | push pointer to exception frame
        jsr     _trap_catch             | will return pointer to reg save area
        movel   d0,sp                   | SP points to reg save area again
        moveml  sp@+,d0-d7/a0-a7        | restore all regs
        addql   #8,sp                   | reclaim the space for context_t cruft
        movew   #0,sp@(6)               | bogus fmt code
        rte


	.globl	 _context_intr_swap, _context_swap

|
| interrupt level context switch -- old tasks context is
| already saved in reschedule() from the interrupt context passed from
| default trap handler trap_default() which saves all regs in its stack.
|
_context_intr_swap:

	movel	sp@(4),a0		| a0 <- new context ptr
	moveml	a0@,d0-d7/a0-a6		| restore all regs (except SP)
	movel	sp@(4),a0		| a0 <- new context ptr again -- XXX
	movel	a0@(SP_OFF),sp		| retrieve SP from new context
	movew	#0,sp@-			| bogus fmt code (4 words loaded)
	movel	a0@(PC_OFF),sp@-	| new PC
	movew	a0@(SR_OFF),sp@-	| new SR
	movel	a0@(A0_OFF),a0		| restore a0
	rte
|
| task level context switch
|
_context_swap:

	movel	a0,sp@-			| save a0
	movel	sp@(8),a0		| a0 <- old context ptr
	moveml	d0-d7/a0-a7,a0@		| save all regs
	movel	sp@,a0@(A0_OFF)		| restore a0 from saved val on stack
	addql	#8,a0@(SP_OFF)		| update saved SP, as if we returned
	movew	sr,a0@(SR_OFF)		| save SR
	movel	sp@(4),a0@(PC_OFF)	| save PC passed in the call stack
	movel	sp@(12),a0		| a0 <- new context ptr
	moveml	a0@,d0-d7/a0-a6		| restore all regs (except SP)
	movel	sp@(12),a0		| a0 <- new context ptr again -- XXX
	movel	a0@(SP_OFF),sp		| retrieve SP from new context
	movel	a0@(PC_OFF),sp@-	| new PC
	movew	a0@(SR_OFF),sr		| new SR
	movel	a0@(A0_OFF),a0		| restore a0
	rts

|
| UNIX like setjmp/longjmp
|
	.globl	_setjmp

_setjmp:

	movl	sp@(4),a0		| a0 <- ptr to jmp_buf
	movl	sp,a0@+			| save SP
	movl	a6,a0@+			| save Frame Ptr
	movl	sp@,a0@+		| save PC
	movml	d0-d7/a0-a7,a0@		| save all regs
	clrl	d0			| always 0 return XXX
	rts

	.data
	.even
bad_sp_msg:

	.asciz "longjmp: bad SP (NULL).\n"

	.text
	.even
	.globl	_longjmp

_longjmp:

	movl	sp@(4),a0		| a0 <- ptr to jmp_buf
	tstl	a0@(8)			| zero SP?
	jeq	bad_sp
	movl	sp@(8),d0		| d0 <- retval arg
	jne	non_zero		| already non-zero return val
	moveq	#1,d0			| otherwise make it so

non_zero:

	movl	a0@+,sp			| restore SP
	movl	a0@+,a6			| restore Frame Ptr
	movl	a0@+,sp@		| restore PC
	movml	a0@,d0-d7/a0-a7		| etc.
	rts

bad_sp:

	pea	bad_sp_msg
	jsr	_panic

|
| first_bit() used for scanning 32bit integer for a bit set to 1.
| the postion of the first bit set from the right-most position is returned.
| used in pick() for scheduling.  useful for other selection type work.
|

	.globl	_first_bit		| find first bit set in an integer

_first_bit:

	bfffo	sp@(4){#0:#0},d1
	movel	#MAX_PRIORITY_GROUPS-1,d0
	subl	d1,d0
	rts				| return index to the first set bit

|
| vec_set() initializes an interrupt vector
|

	.globl	_vec_set

_vec_set:

	movec	vbr,a0
	addl	sp@(4),a0
	movel	sp@(8),a0@
	rts


	.text
	.even

	.globl	_splhi, _splx

	.globl	_spl7, _spl6, _spl5, _spl4, 
	.globl _spl3, _spl2, _spl1, _spl0

	.globl	_enable_intrs

_enable_intrs:

	movew	#IPL_ENABLE,sr
	rts

_splx:

	movel	sp@(4),d0
	movew	d0,sr
	rts

_splhi:
_spl7:

	movew	sr,d0
	movew	#IPL7_LOCK,sr
	rts

_spl6:
	movew	sr,d0
	movew	#IPL6_LOCK,sr
	rts

_spl5:
	movew	sr,d0
	movew	#IPL5_LOCK,sr
	rts

_spl4:
	movew	sr,d0
	movew	#IPL4_LOCK,sr
	rts

_spl3:
	movew	sr,d0
	movew	#IPL3_LOCK,sr
	rts

_spl2:
	movew	sr,d0
	movew	#IPL2_LOCK,sr
	rts

_spl1:
	movew	sr,d0
	movew	#IPL1_LOCK,sr
	rts

_spl0:
	movew	sr,d0
	movew	#IPL0_LOCK,sr
	rts

|
| VAX insque/remque instruction emulation. 
| (as per: VAX Architecture Handbook, pp. 228-235)
| Used mainly by network code.
|

	.globl	_insque

_insque:
	movew	sr,d0
	movew	#IPL_LOCK,sr		| disable interrupts
	movel	sp@(8),a0		| a0 <- targ
	movel	sp@(4),a1		| a1 <- elem
	movel	a0@,a1@			| elem->next <- targ->next
	movel	a0,a1@(4)		| elem->prev <- targ
	movel	a1,a0@			| targ->next <- elem
	movel	a1@,a0			| a0 <- elem->next
	movel	a1,a0@(4)		| elem->next->prev = elem
	movew	d0,sr			| restore intr mask
	rts

	.globl _remque

_remque:
	movew	sr,d0
	movew	#IPL_LOCK,sr		| disable interrupts
	movel	sp@(4),a0		| a0 <- element to remove
	movel	a0@,a1			| a1 <- elem->next
	movel	a0@(4),a0		| a0 <- elem->prev
	movel	a0,a1@(4)		| elem->next->prev <- elem->prev
	movel	a1,a0@			| elem->prev->next <- elem->next
	movew	d0,sr			| restore intr mask
	rts

