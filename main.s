||| -*- mode: gas; gas-comment-char: 124 -*-
||| z80 emulator for 68k calculators

||| Duncan Smith
||| Project started: 2010-06-06
||| GPL

||| Yes, I use lots of big ascii art.  With this much code, you need
||| something to catch your eye when scrolling through it.  I suppose
||| I'll split it into different files later.

||| Registers used:
|||
||| A7		68000 stack pointer
||| A6/epc	emulated PC
||| A5		instruction table base pointer
||| A4		emulated SP
||| A3		pointer to flag_storage
||| A2		
||| A1		
||| A0		
|||
||| D0		current instruction, scratch for macros
||| D1		scratch for instructions
||| D2		further scratch
|||
|||
||| The following have their shadows in the top half of the register
||| D3/eaf = AF		A is in the low byte, F in the high byte (yeah ... speed)
||| D4/ebc = BC		B high, C low
||| D5/ede = DE		D high, E low
||| D6/ehl = HL		H high, L low
|||
||| IY is used more often so it's easier to get at.  It can be slow
||| but I don't really care to go to the effort to make it so.
||| D7/eixy = IX (hi word), IY (low word)


||| emulated I and R are both in RAM

.xdef		_ti89
|.xdef		_ti92plus
.xdef		__main
|.xdef		_tigcc_native
.include	"../tios.h"

.include	"global.inc"

__main:
	movem.l d0-d7/a0-a6,-(sp)
	bsr	init_load
	bsr	display_setup

	bsr	emu_setup
	lea	emu_plain_op,a5

	|| ... aaaaand we're off!
	jsr	emu_run
	bsr	emu_teardown

	bsr	display_teardown
	bsr	unload
	movem.l (sp)+,d0-d7/a0-a6
	rts

.include	"ports.s"
.include	"interrupts.s"
.include	"flags.s"
.include	"alu.s"

emu_setup:
	movea.l	emu_op_00,a5
	lea	emu_run,a2
	lea	flag_storage,a3
	move.w	#$4000,d1
	bsr	deref
	move.l	a0,epc
	move.l	a0,esp

	rts

emu_teardown:
	rts


|| ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||  _ __ ___   ___ _ __ ___   ___  _ __ _   _   |||||||||||||||||||||||||||
|| | '_ ` _ \ / _ \ '_ ` _ \ / _ \| '__| | | |  \\\\\\\\\\\\\\\\\\\\\\\\\\\
|| | | | | | |  __/ | | | | | (_) | |  | |_| |  |||||||||||||||||||||||||||
|| |_| |_| |_|\___|_| |_| |_|\___/|_|   \__, |  ///////////////////////////
|| of the virtual type                  |___/   |||||||||||||||||||||||||||
|| =============================================JJJJJJJJJJJJJJJJJJJJJJJJJJJ

	|| Take a virtual address in d1 and dereference it.  Returns the
	|| host address in a0.  Destroys a0, d0.
deref:	| 76 cycles + 18 cycles for bsr
	| 20 bytes to inline, saves 34 cycles per call
	move.w	d1,d0
	andi.w	#$3FFF,d0
	movea.w	d0,a0
	move.w	d1,d0
	andi.w	#$C000,d0	| Can cut this out by pre-masking the table.
	rol.w	#4,d0
	adda.l	deref_table(pc,d0.w),a0
	rts

.even
.data
deref_table:
mem_page_0:	.long	0		| bank 0 / 0x0000
mem_page_1:	.long	0		| bank 1 / 0x4000
mem_page_2:	.long	0		| bank 2 / 0x8000
mem_page_3:	.long	0		| bank 3 / 0xc000

.xdef	mem_page_0
.xdef	mem_page_1
.xdef	mem_page_2
.xdef	mem_page_3

mem_page_loc_0:	.byte	0
mem_page_loc_1:	.byte	0
mem_page_loc_2:	.byte	0
mem_page_loc_3:	.byte	0

.xdef	mem_page_loc_0
.xdef	mem_page_loc_1
.xdef	mem_page_loc_2
.xdef	mem_page_loc_3

pages:	.long	0

.xdef pages

.text

	|| Take a physical address in a0 and turn it into a virtual
	|| address in d0
	|| Destroys d0
| XXX AFAICS, a1 is currently a scratch address register, so you can load deref_table in it, and then save some space:
| But you may wish to use it for other purposes in the future, so you needn't integrate that immediately.

	|| Guessing this is 300 cycles.
underef:
	move.l	d2,-(a7)
	lea	deref_table(pc),a1
	move.l	a0,d0
	clr.w	d2
	sub.l	(a1)+,d0
	bmi.s	underef_not0
	cmpi.l	#$4000,d0
	bmi.s	underef_thatsit
underef_not0:
	move.l	a0,d0
	move.w	#$4000,d2
	sub.l	(a1)+,d0
	bmi.s	underef_not1
	cmpi.l	#$4000,d0
	bmi.s	underef_thatsit
underef_not1:
	move.l	a0,d0
	move.w	#$8000,d2
	sub.l	(a1)+,d0
	bmi.s	underef_not2
	cmpi.l	#$4000,d0
	bmi.s	underef_thatsit
underef_not2:
	move.w	#$c000,d2
	suba.l	(a1)+,a0
	|| if that fails too, well shit man!
	moveq	#0,d0
underef_thatsit:
	add.w	d2,d0
	move.l	(a7)+,d2
	rts


|| =========================================================================
|| instruction   instruction   instruction  ================================
||      _ _                 _       _       ================================
||   __| (_)___ _ __   __ _| |_ ___| |__    ================================
||  / _` | / __| '_ \ / _` | __/ __| '_ \   ================================
|| | (_| | \__ \ |_) | (_| | || (__| | | |  ================================
||  \__,_|_|___/ .__/ \__,_|\__\___|_| |_|  ================================
||             |_|                         =================================
|| ==========       ========================================================
|| =========================================================================

.include	"opcodes.s"

emu_run:
	|| XXX: make this actually return
	DONE
	rts

emu_op_undo_cb:
emu_op_undo_dd:
emu_op_undo_ed:
emu_op_undo_fd:
	rts
