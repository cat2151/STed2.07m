* OnGraphic.s  writeen by Ext. rewrite by TURBO

WK_screen_mode_093C:	equ	$00093C
WK_GRAPH_2_width_x_0960:equ	$000960
WK_GRAPH_palet_max_0964:equ	$000964
IO_CRTC_R20_mode_set_28:equ	$E80028
GRAPHIC_PALET_top:	equ	$E82000
IO_VC_mem_mode_2400:	equ	$E82400
IO_VC_special_2600:	equ	$E82600

	.cpu	68000

	.include	doscall.mac
	.include	iocscall.mac

	.text
	globl	_OnGraphic

*********************************************

_OnGraphic:
	pea	0.w
	DOS	_SUPER
	addq.l	#4,SP
	move.l	D0,stackSave

	bclr.b	#$0003,IO_CRTC_R20_mode_set_28
	move.b	IO_CRTC_R20_mode_set_28,D1
	or.b	#$07,D1
*	move.b	WK_screen_mode_093C,D0
*	move.b	D0,D2
*	and.b	#$0F,D2
*	cmp.b	#$04,D2
*	bcs.s	LLAB26
*	and.b	#$FB,D1
LLAB26:
*	moveq	#$FF,D2
*	and.w	#$000C,D0
*	cmp.b	#$0C,D0
*	beq.s	LLAB46
	bclr.l	#$0001,D1
*	move.w	#$00FF,D2
*	cmp.b	#$08,D0
*	beq.s	LLAB46
	bclr.l	#$0000,D1
	moveq	#$0F,D2
LLAB46:
	move.b	D1,D0
	move.w	D2,WK_GRAPH_palet_max_0964
	and.w	#$0007,D0
	move.b	D1,IO_CRTC_R20_mode_set_28
	move.w	D0,IO_VC_mem_mode_2400
	move.b	D1,D0
	move.l	#$00000400,D1
*	btst.l	#$0002,D0
*	beq.s	LLAB6E
	add.l	D1,D1
LLAB6E:
	move.l	D1,WK_GRAPH_2_width_x_0960

*	lea	GRAPHIC_PALET_top,A0
*	move.w	#$007F,D0
*	move.w	#$0001,D1
*LLABB2:
*	move.w	D1,(A0)+
*	move.w	D1,(A0)+
*	addi.w	#$0202,D1
*	dbf	D0,LLABB2

	bclr.b	#$0003,IO_CRTC_R20_mode_set_28
	move.w	#$003F,IO_VC_special_2600

	move.l	stackSave,D0
*	cmpi.l	#-1,D0
*	beq	End
	move.l	d0,-(sp)
	DOS	_SUPER
	addq.w	#4,sp
End:

	rts

	bss
stackSave:
	ds.l	1

	end
