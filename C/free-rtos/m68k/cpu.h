/* $Revision: 1.1.1.1 $ */

#ifndef _MC68K_CPU_H_
#define _MC68K_CPU_H_

#ifndef ASM_LANGUAGE
#include "common.h"

typedef struct
  {
    u_int d0, d1, d2, d3, d4, d5, d6, d7;
    u_int a0, a1, a2, a3, a4, a5, fp, sp;
  } reg_t;

typedef struct
  {
    reg_t ctx_reg;
    u_short ctx_sr;		/* cruft */
    u_long ctx_pc;		/* cruft */
    u_short ctx_flag;		/* cruft */
  } context_t;

typedef struct
  {
    u_short exc_sr;
    u_long exc_pc;
    u_short exc_vec;		/* top nibble == format */
  } excfrm_t;

/* short bus cycle error */
typedef struct
  {
    excfrm_t common;
    u_short pad0;
    u_short special_status_reg;
    u_short pipe_stage_c_inst;
    u_short pipe_stage_b_inst;
    u_long data_cycle_fault_addr;
    u_long pad2;
    u_long data_output_buf;
    /* rest ignored XXX */
  } excfrm_10_t;

/* long bus cycle error */
typedef struct
  {
    excfrm_t common;
    u_short pad0;
    u_short special_status_reg;
    u_short pipe_stage_c_inst;
    u_short pipe_stage_b_inst;
    u_long data_cycle_fault_addr;
    u_long pad2;
    u_long data_output_buf;
    u_long pad3[2];
    u_long stage_b_addr;
    u_long pad4;
    u_long data_input_buf;
    /* rest ignored XXX */
  } excfrm_11_t;

void ctx_fill (context_t * tp, int pc, int sr, int flag, int fp, int sp);
int ctx_stack (context_t * tp);

#endif

/* offsets to various members of 'context_t' for ASM programs. offsets must
   match the layout of 'context_t' & 'reg_t'. */
#define A0_OFF		32
#define SP_OFF		60
#define SR_OFF		64
#define PC_OFF		66

#define IPL_LOCK	0x2700
#define IPL_UNLOCK	0x2000
#define IPL_ENABLE	0x2000	/* when task is created SR == IPL_ENABLE */

#define IPL7_LOCK	IPL_LOCK
#define IPL6_LOCK	0x2600
#define IPL5_LOCK	0x2500
#define IPL4_LOCK	0x2400
#define IPL3_LOCK	0x2300
#define IPL2_LOCK	0x2200
#define IPL1_LOCK	0x2100
#define IPL0_LOCK	IPL_UNLOCK

#endif
