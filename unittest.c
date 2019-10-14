/* Unit tests and test harness for 680 project.
 *
 * Copyright 2011, Astrid Smith
 * GPL
 */

#define TEST 1

#include <default.h>

/* These values are hard-coded into the assembly code in do_instr
 * below.  TREAD LIGHTLY.
 */
enum z80regs {
	/* normal registers */
	REG_A = 0,
	REG_F = 1,
	REG_B = 2,
	REG_C = 3,
	REG_D = 4,
	REG_E = 5,
	REG_H = 6, REG_L = 7,
	REG_IX_H = 8, REG_IX_L = 9,
	REG_IY_H = 10, REG_IY_L = 11,

	/* shadow registers */
	REG_A_ = 12,
	REG_F_ = 13,
	REG_B_ = 14,
	REG_C_ = 15,
	REG_D_ = 16,
	REG_E_ = 17,
	REG_H_ = 18, REG_L_ = 19,
	REG_IX_H_ = 20, REG_IX_L_ = 21,
	REG_IY_H_ = 22, REG_IY_L_ = 23,

	REG_SP_H = 24, REG_SP_L = 25,
	REG_PC_H = 26, REG_PC_L = 27,

	REG_number_of_registers = 28
};

typedef struct {
	// registers
	char regs[28];

	// storage space for instruction, immediate values, etc
	char program[8];

	// storage space for emulated stack
	char stack[8];

	// emulated addresses $fff8 - $0007
	char heap[8];
} z80_state_t;

void _main(void) {
	z80_state_t foo;
}

/* function to execute an instruction, using the state indicated by
 * z80_state_t 'proc'.
 */

void do_instr(z80_state_t *proc_begin, z80_state_t *proc_end)
{
	/* For offsets, use the values in enum z80regs above.
	 */
	asm( "
	movem.l	d3-d7/a3-a6,-(a7)
	movem.l	d0-d2/a0-a3,-(a7)
	move.l	%[regs_out],-(a7)
||| register setup
	lea	emu_plain_op,a5
	move.l	%[regs_in],a0
	movea.l	%[epc],a6
	movea.l	%[esp],a4

	move.b	13(a0),d3	| f_
	rol.w	#8,d3
	move.b	12(a0),d3	| a_
	rol.w	#8,d3
	move.b	1(a0),d3	| f
	rol.w	#8,d3
	move.b	0(a0),d3	| a

	move.b	14(a0),d4	| b_
	rol.w	#8,d4
	move.b	15(a0),d4	| c_
	rol.w	#8,d4
	move.b	2(a0),d4	| b
	rol.w	#8,d4
	move.b	3(a0),d4	| c

	move.b	16(a0),d5	| d_
	rol.w	#8,d5
	move.b	17(a0),d5	| e_
	rol.w	#8,d5
	move.b	4(a0),d5	| d
	rol.w	#8,d5
	move.b	5(a0),d5	| e

	move.b	18(a0),d6	| h_
	rol.w	#8,d6
	move.b	19(a0),d6	| l_
	rol.w	#8,d6
	move.b	6(a0),d6	| h
	rol.w	#8,d6
	move.b	7(a0),d6	| l

||| call the routine

||| register teardown

	move.l	(a7)+,a0	| pop regs_out
	movem.l	d0-d2/a0-a3,-(a7)	| pop other stuff, to give gcc room to play in


	move.b	d3,13(a0)	| f_
	rol.w	#8,d3
	move.b	d3,12(a0)	| a_
	rol.w	#8,d3
	move.b	d3,1(a0)	| f
	rol.w	#8,d3
	move.b	d3,0(a0)	| a

	move.b	d4,14(a0)	| b_
	rol.w	#8,d4
	move.b	d4,15(a0)	| c_
	rol.w	#8,d4
	move.b	d4,2(a0)	| b
	rol.w	#8,d4
	move.b	d4,3(a0)	| c

	move.b	d5,16(a0)	| d_
	rol.w	#8,d5
	move.b	d5,17(a0)	| e_
	rol.w	#8,d5
	move.b	d5,4(a0)	| d
	rol.w	#8,d5
	move.b	d5,5(a0)	| e

	move.b	d6,18(a0)	| h_
	rol.w	#8,d6
	move.b	d6,19(a0)	| l_
	rol.w	#8,d6
	move.b	d6,6(a0)	| h
	rol.w	#8,d6
	move.b	d6,7(a0)	| l


	movem	(a7)+,d3-d7/a1-a6

	"
	     :
	     : [regs_in] "p" (proc_begin->regs),
	       [regs_out] "p" (proc_end->regs),
	       [epc] "p" (*proc_begin->program),
	       [esp] "p" (*proc_begin->stack)
	     : "memory", "a0"
		);

}
