||| -*- mode: gas; gas-comment-char: 124 -*-
||| z80 emulator for 68k calculators

||| Astrid Smith
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
||| A4/esp	emulated SP
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
.include	"./tios.inc"

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
	move.w	#0x4000,d1
	bsr	deref
	move.l	a0,epc
	move.l	a0,esp

	rts

emu_teardown:
	rts


.include	"memory.s"

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

