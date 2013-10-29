* key_rep.s	key repeat on/off routine
* main routine by GAO & call routine by TURBO

		.cpu	68000

		.include	doscall.mac

		.text
		.globl	_key_vect_set
		.globl	_key_vect_reset
		.globl	_key_rep_off
		.globl	_key_rep_on

*----------------------------------------
*	KEY VECTOR($4c)ÇèëÇ´ä∑Ç¶ÇÈ
*----------------------------------------
_key_vect_set:
		pea	mfp_bufful(pc)
		move.w	#$4c,-(sp)
		DOS	_INTVCS
		addq.w	#6,sp
		move.l	d0,org_vect
		rts

*----------------------------------------
*	KEY VECTORÇå≥Ç…ñﬂÇ∑
*----------------------------------------
_key_vect_reset:
		move.l	org_vect(pc),-(sp)
		move.w	#$4c,-(sp)
		DOS	_INTVCS
		addq.w	#6,sp
		rts

*----------------------------------------
*	KEY REPEATÇã÷é~Ç∑ÇÈ
*----------------------------------------
_key_rep_off:
		clr.w	rep_flag
		rts

*----------------------------------------
*	KEY REPEATÇãñâ¬Ç∑ÇÈ
*----------------------------------------
_key_rep_on:
		move.w	#1,rep_flag
		rts

*----------------------------------------
*	MFP_BUFFUL ROUTINE
*----------------------------------------
mfp_bufful	movem.l	d0-d1/a0,-(sp)

		tst.w	rep_flag	* repeat flag chack
		bne	mfp_bufful1

		moveq	#0,d0
		move.b	$00e8802f,d0
		cmpi.b	#$80,d0
		bcc	mfp_bufful1
		movea.l	#$800,a0
		move.w	d0,d1
		lsr.w	#3,d1
		andi.w	#$0007,d0
		btst.b	d0,(a0,d1.w)
		bne	mfp_bufful2
mfp_bufful1	movem.l	(sp)+,d0-d1/a0
		move.l	org_vect(pc),-(sp)
		rts

mfp_bufful2	movem.l	(sp)+,d0-d1/a0
		rte

		.bss

org_vect	dc.l	0

rep_flag	dc.w	1

