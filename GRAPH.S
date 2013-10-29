	.cpu	68000

	.include iocscall.mac
	.include doscall.mac

	.xref	_scyp
	.xref	_edit_scr
	.xref	_font

	.xdef	_g_print
	.xdef	_tg_copy
	.xdef	_tg_copy2
	.xdef	_rev_area
	.xdef	_txcur
*	.xdef	_g_scr
*	.xdef	_t_scr
	.xdef	_t_scrw
*	.xdef	_txt_clr
*	.xdef	_fill

G_VRAM		equ	$00C00000	* ÉOÉâÉtÉBÉbÉNÇuÇqÇ`ÇlêÊì™ÉAÉhÉåÉX
T_VRAM		equ	$00E00000	* ÉeÉLÉXÉgÇuÇqÇ`ÇlêÊì™ÉAÉhÉåÉX

	.even

_g_print:
	movem.l	d3/d4/d5/a3/a4,-(sp)

	move.l	32(sp),a3	* string
	tst.b	(a3)
	beq	gpr99

	move.l	#G_VRAM,a4
	move.l	28(sp),d0	* y
	moveq.l	#10,d2
	asl.l	d2,d0
	add.l	24(sp),d0	* x
	add.l	d0,d0
	adda.l	d0,a4		* a4 = gram add

	move.w	38(sp),d3	* d3 = color

	pea	0.w
	DOS	_SUPER
	move.l	d0,d5
	addq.w	#4,sp

gpr41:
	moveq.l	#0,d1
	move.b	(a3)+,d1
	beq	gpr43

	cmp.w	#32,d1
	beq	gpr88
	cmp.w	#127,d1
	bls	gprhp
	cmp.w	#160,d1
	bls.s	gpr5
	cmp.w	#224,d1
	bls	gprhp
gpr5:
	asl.w	#8,d1
	move.b	(a3)+,d0
	add.b	d0,d1

gpr4:
	moveq.l	#8,d2	* Size
	moveq.l	#$16,d0
	trap	#15	* Get Kanji ROM address -->d0.l   xs -->d1.w
	move.l	d0,a1

	movea.l	a4,a0
	moveq.l	#16-1,d0

	tst.w	d1
	beq	gpr40

gpr27:
	move.w	(a1)+,d1
	beq	gpr18

	add.w	d1,d1
	bcc.s	gpr10
	move.w	d3,(a0)
gpr10:
	add.w	d1,d1
	bcc.s	gpr11
	move.w	d3,2(a0)
gpr11:
	add.w	d1,d1
	bcc.s	gpr12
	move.w	d3,4(a0)
gpr12:
	add.w	d1,d1
	bcc.s	gpr13
	move.w	d3,6(a0)
gpr13:
	add.w	d1,d1
	bcc.s	gpr14
	move.w	d3,8(a0)
gpr14:
	add.w	d1,d1
	bcc.s	gpr15
	move.w	d3,10(a0)
gpr15:
	add.w	d1,d1
	bcc.s	gpr16
	move.w	d3,12(a0)
gpr16:
	add.w	d1,d1
	bcc.s	gpr9
	move.w	d3,14(a0)
gpr9:
	add.w	d1,d1
	bcc.s	gpr19
	move.w	d3,16(a0)
gpr19:
	add.w	d1,d1
	bcc.s	gpr20
	move.w	d3,18(a0)
gpr20:
	add.w	d1,d1
	bcc.s	gpr21
	move.w	d3,20(a0)
gpr21:
	add.w	d1,d1
	bcc.s	gpr22
	move.w	d3,22(a0)
gpr22:
	add.w	d1,d1
	bcc.s	gpr23
	move.w	d3,24(a0)
gpr23:
	add.w	d1,d1
	bcc.s	gpr24
	move.w	d3,26(a0)
gpr24:
	add.w	d1,d1
	bcc.s	gpr25
	move.w	d3,28(a0)
gpr25:
	add.w	d1,d1
	bcc.s	gpr18
	move.w	d3,30(a0)
gpr18:
	lea	2048(a0),a0
	dbra	d0,gpr27

	lea.l	32(a4),a4
	bra	gpr41


gprhp:			* hprint font add
	asl.w	#4,d1
	lea	_font(pc),a1
	adda.w	d1,a1

	movea.l	a4,a0
	moveq.l	#16-1,d0

gpr40:
	move.b	(a1)+,d1
	beq.s	gpr31

	add.b	d1,d1
	bcc.s	gpr32
	move.w	d3,(a0)
gpr32:
	add.b	d1,d1
	bcc.s	gpr33
	move.w	d3,2(a0)
gpr33:
	add.b	d1,d1
	bcc.s	gpr34
	move.w	d3,4(a0)
gpr34:
	add.b	d1,d1
	bcc.s	gpr35
	move.w	d3,6(a0)
gpr35:
	add.b	d1,d1
	bcc.s	gpr36
	move.w	d3,8(a0)
gpr36:
	add.b	d1,d1
	bcc.s	gpr37
	move.w	d3,10(a0)
gpr37:
	add.b	d1,d1
	bcc.s	gpr38
	move.w	d3,12(a0)
gpr38:
	add.b	d1,d1
	bcc.s	gpr31
	move.w	d3,14(a0)
gpr31:
	lea	2048(a0),a0
	dbra	d0,gpr40

gpr88:
	lea	16(a4),a4
	bra	gpr41
gpr43:
	move.l	d5,-(sp)
	DOS	_SUPER
	addq.w	#4,sp

gpr99:
	movem.l	(sp)+,d3/d4/d5/a3/a4
	rts

**********
_tg_copy:
	movem.l	d3-d7/a3-a5,-(sp)

	move.l	#T_VRAM+2+4*16*128,a4
	move.l	#G_VRAM+16*2+(4*16+512)*1024*2,a3

	tst.w	36+2(sp)
	beq.s	tgc2

	lea	56(a4),a4
	lea	56*8*2(a3),a3
tgc2:
	move.l	a4,a5
	add.l	#$20000,a5

	pea	0.w
	DOS	_SUPER
	move.l	d0,a0
	addq.w	#4,sp

	move.w	#26*16-1,d7	* yl
tgc3:
	moveq.l	#18-1,d6
tgc6:
	move.w	(a5)+,d2
	move.w	(a4)+,d1

	move.w	d2,d0
	or.w	d1,d0
	bne.s	tgc9

	lea	32(a3),a3
	dbra	d6,tgc6
	bra	tgc22
tgc9:
	moveq.l	#16-1,d3
tgc11:
	clr.w	d0
	add.w	d2,d2
	addx.w	d0,d0
	add.w	d1,d1
	addx.w	d0,d0
	move.w	d0,(a3)+

	dbra	d3,tgc11
	dbra	d6,tgc6
tgc22:
	lea	92(a5),a5
	lea	92(a4),a4
	lea	1472(a3),a3
	dbra	d7,tgc3

	move.l	a0,-(sp)
	DOS	_SUPER
	addq.w	#4,sp

	movem.l	(sp)+,d3-d7/a3-a5
	rts

_tg_copy2:
	movem.l	d3-d7/a3-a5,-(sp)

	move.l	#T_VRAM+6*16*128,a4
	move.l	#G_VRAM+(6*16+512)*1024*2,a3

	tst.w	36+2(sp)
	beq.s	tg2c2

	lea	38(a4),a4
	lea	38*8*2(a3),a3
tg2c2:
	move.l	a4,a5
	add.l	#$20000,a5

	pea	0.w
	DOS	_SUPER
	move.l	d0,a0
	addq.w	#4,sp

	move.w	#24*16-1,d7	* yl
tg2c3:
	moveq.l	#28-1,d6
tg2c6:
	move.w	(a5)+,d2
	move.w	(a4)+,d1

	move.w	d2,d0
	or.w	d1,d0
	bne.s	tg2c9

	lea	32(a3),a3
	dbra	d6,tg2c6
	bra	tg2c22
tg2c9:
	moveq.l	#16-1,d3
tg2c11:
	clr.w	d0
	add.w	d2,d2
	addx.w	d0,d0
	add.w	d1,d1
	addx.w	d0,d0
*	move.w	d0,(a3)+
*
tst.w	d0
	bne	@f
	addq.l	#2,a3

	dbra	d3,tg2c11
	dbra	d6,tg2c6
	bra	tg2c22

@@:
	move.w	d0,(a3)+

	dbra	d3,tg2c11
	dbra	d6,tg2c6
tg2c22:
	lea	72(a5),a5
	lea	72(a4),a4
	lea	1216-16*4(a3),a3
	dbra	d7,tg2c3

	move.l	a0,-(sp)
	DOS	_SUPER
	addq.w	#4,sp

	movem.l	(sp)+,d3-d7/a3-a5
	rts

**********
_rev_area:
	movem.l	d2/d3/a3,-(sp)
	move.l	20(sp),d1
	move.l	16(sp),d0

	asl.l	#4,d0
	moveq.l	#6*16,d2
	add.l	d2,d0
	asl.l	#7,d0

	move.l	#T_VRAM+12,a3
	add.l	d0,a3

	tst.w	24+2(sp)
	beq.s	reva17

	lea	56(a3),a3
reva17:
	move.w	d1,d3
	asl.w	#2,d3
	subq.w	#1,d3

	pea	0.w
	DOS	_SUPER
	move.l	d0,d1
	addq.w	#4,sp

	move.l	#$88888888,d2
reva18:
	eor.l	d2,(a3)+
	eor.l	d2,(a3)+
	eor.l	d2,(a3)+
	eor.l	d2,(a3)+
	eor.l	d2,(a3)+
	eor.l	d2,(a3)+
	eor.w	d2,(a3)+

	lea	486(a3),a3
	dbra	d3,reva18

	move.l	d1,-(sp)
	DOS	_SUPER
	addq.w	#4,sp

	movem.l	(sp)+,d2/d3/a3
	rts

_txcur:
	movem.l	d2-d6/a3/a4,-(sp)
	move.l	32(sp),d1
	move.l	36(sp),d0
	move.w	40+2(sp),d4
	move.w	44+2(sp),d3
	move.w	48+2(sp),d5

	moveq.l	#1,d2
	cmp.w	_edit_scr+2,d2
	bne.s	txcu2
	moveq.l	#56,d2
	add.l	d2,d1
txcu2:
	move.l	#T_VRAM,a4
	moveq.l	#11,d2
	asl.l	d2,d0
	add.l	d1,d0
	add.l	d0,a4
	move.l	#$20000,a3
	add.l	a4,a3

	asl.w	#4,d3	*yl=ys*16-1;
	subq.w	#1,d3

	move.l	#128,d6
	sub.w	d4,d6

	moveq.l	#0,d2
	btst	#0,d1
	beq.s	@f
	moveq.l	#1,d2
	subq.l	#1,d4
@@:

	pea	0.w
	DOS	_SUPER
	move.l	d0,d1
	addq.w	#4,sp

txcu3:
	cmp.w	#1,d5
	beq.s	txcu6

	tst.b	d2
	beq	@f
	not.b	(a3)+

@@:
	move.w	d4,d0
	asr.w	#2,d0
	beq.s	@f
	subq.w	#1,d0
txcu7:
	not.l	(a3)+
	dbra	d0,txcu7
@@:
	btst	#1,d4
	beq.s	@f
	not.w	(a3)+
@@:
	btst	#0,d4
	beq.s	@f
	not.b	(a3)+
@@:

txcu6:
	cmp.w	#2,d5
	beq.s	txcu10

	tst.b	d2
	beq	@f
	not.b	(a4)+

@@:
	move.w	d4,d0
	asr.w	#2,d0
	beq.s	@f
	subq.w	#1,d0
txcu11:
	not.l	(a4)+
	dbra	d0,txcu11
@@:
	btst	#1,d4
	beq.s	@f
	not.w	(a4)+
@@:
	btst	#0,d4
	beq.s	@f
	not.b	(a4)+
@@:

txcu10:
	add.l	d6,a4
	add.l	d6,a3
	dbra	d3,txcu3

	tst.l	d1
	bmi.s	@f
	move.l	d1,-(sp)
	DOS	_SUPER
	addq.w	#4,sp
@@:
	movem.l	(sp)+,d2-d6/a3/a4
	rts


*_g_scr:
*	movem.l	d3-d7/a3/a4/a5,-(sp)
*	move.w	38(sp),d6	*x1
*	move.w	42(sp),d4	*y1
*	move.w	46(sp),a5	*xs
*	move.w	50(sp),d7	*ys
*	move.w	54(sp),d5	*x2
*	move.w	58(sp),d3	*y2
*
*	move.l	#G_VRAM,a3
*	move.l	a3,a4
*
*	pea	0.w
*	DOS	_SUPER
*	move.l	d0,a1
*	addq.w	#4,sp
*
*	move.w	#1024,d1	*1024-xs
*	sub.w	a5,d1
*
*	ext.l	d4		*y1*1024+x1
*	moveq.l	#10,d2
*	asl.l	d2,d4
*	move.l	d4,a0
*	add.w	d6,a0
*	move.l	a0,d4
*	add.l	d4,d4
*	add.l	d4,a4
*
*	ext.l	d3		*y2*1024+x2
*	asl.l	d2,d3
*	move.l	d3,a0
*	add.w	d5,a0
*	move.l	a0,d3
*	add.l	d3,d3
*	add.l	d3,a3
*
*	cmp.l	a4,a3
*	bcc	gscr3
*
*	add.w	d1,d1
*	subq.w	#1,d7
*gscr9:
*	move.w	a5,d0
*	lsr.w	#3,d0
*	subq.w	#1,d0
*gscr8:
*	move.l	(a4)+,(a3)+
*	move.l	(a4)+,(a3)+
*	move.l	(a4)+,(a3)+
*	move.l	(a4)+,(a3)+
*	dbra	d0,gscr8
*
*	add.w	d1,a4
*	add.w	d1,a3
*	dbra	d7,gscr9
*	bra	gscr10
*
*gscr3:
*	move.w	d7,d0		*ys*1024-ofs
*	ext.l	d0
*	moveq.l	#10,d2
*	asl.l	d2,d0
*	move.l	d0,a0
*	sub.w	d1,a0
*	move.l	a0,d0
*	add.l	d0,d0
*
*	add.l	d0,a4
*	add.l	d0,a3
*
*	add.w	d1,d1
*	subq.w	#1,d7
*gscr16:
*	move.w	a5,d0
*	lsr.w	#3,d0
*	subq.w	#1,d0
*gscr15:
*	move.l	-(a4),-(a3)
*	move.l	-(a4),-(a3)
*	move.l	-(a4),-(a3)
*	move.l	-(a4),-(a3)
*	dbra	d0,gscr15
*
*	sub.w	d1,a4
*	sub.w	d1,a3
*	dbra	d7,gscr16
*gscr10:
*	move.l	a1,-(sp)
*	DOS	_SUPER
*	addq.w	#4,sp
*	movem.l	(sp)+,d3-d7/a3/a4/a5
*	rts

*_t_scr:
*	movem.l	d3-d7/a2-a6,-(sp)
*	move.w	38+8(sp),d6	*x1
*	move.w	42+8(sp),d4	*y1
*	move.w	46+8(sp),a2	*xs
*	move.w	50+8(sp),d7	*ys
*	move.w	54+8(sp),d5	*x2
*	move.w	58+8(sp),d3	*y2
*
**	moveq.l	#4,d1
**	asl.w	d1,d4
**	asl.w	d1,d7
**	asl.w	d1,d3
*
*	move.l	#T_VRAM,a3
*	move.l	a3,a4
*
*	move.l	a3,a5
*	add.l	#$20000,a5
*	move.l	a5,a6
*
*	pea	0.w
*	DOS	_SUPER
*	move.l	d0,a1
*	addq.w	#4,sp
*
*	move.w	#128,d1		*128-xs
*	sub.w	a2,d1
*
*	ext.l	d4		*y1*128+x1
*	moveq.l	#7,d2
*	asl.l	d2,d4
*	move.l	d4,a0
*	add.w	d6,a0
*	move.l	a0,d4
*
*	add.l	d4,a4
*	add.l	d4,a6
*
*	ext.l	d3		*y2*128+x2
*	asl.l	d2,d3
*	move.l	d3,a0
*	add.w	d5,a0
*	move.l	a0,d3
*
*	add.l	d3,a3
*	add.l	d3,a5
*
*	cmp.l	a4,a3
*	bcc	tscr3
*
*	subq.w	#1,d7
*tscr9:
*	move.w	a2,d0
**	lsr.w	#2,d0
*	subq.w	#1,d0
*tscr8:
*	move.b	(a4)+,(a3)+
*	move.b	(a6)+,(a5)+
*	dbra	d0,tscr8
*
*	add.w	d1,a4
*	add.w	d1,a3
*	add.w	d1,a6
*	add.w	d1,a5
*	dbra	d7,tscr9
*	bra	tscr10
*
*tscr3:
*	move.w	d7,d0		*ys*128-ofs
*	ext.l	d0
*	moveq.l	#7,d2
*	asl.l	d2,d0
*	move.l	d0,a0
*	sub.w	d1,a0
*	move.l	a0,d0
*
*	add.l	d0,a4
*	add.l	d0,a3
*	add.l	d0,a6
*	add.l	d0,a5
*
*	subq.w	#1,d7
*tscr16:
*	move.w	a2,d0
**	lsr.w	#2,d0
*	subq.w	#1,d0
*tscr15:
*	move.b	-(a4),-(a3)
*	move.b	-(a6),-(a5)
*	dbra	d0,tscr15
*
*	sub.w	d1,a4
*	sub.w	d1,a3
*	sub.w	d1,a6
*	sub.w	d1,a5
*	dbra	d7,tscr16
*tscr10:
*	move.l	a1,-(sp)
*	DOS	_SUPER
*	addq.w	#4,sp
*	movem.l	(sp)+,d3-d7/a2-a6
*	rts

_t_scrw:
	movem.l	d3-d7/a2-a6,-(sp)
	move.w	38+8(sp),d6	*x1
	move.w	42+8(sp),d4	*y1
	move.w	46+8(sp),a2	*xs
	move.w	50+8(sp),d7	*ys
	move.w	54+8(sp),d5	*x2
	move.w	58+8(sp),d3	*y2

	move.l	#T_VRAM,a3
	move.l	a3,a4

	move.l	a3,a5
	add.l	#$20000,a5
	move.l	a5,a6

	pea	0.w
	DOS	_SUPER
	move.l	d0,a1
	addq.w	#4,sp

	move.w	#128,d1		*128-xs
	sub.w	a2,d1

	ext.l	d4		*y1*128+x1
	moveq.l	#7,d2
	asl.l	d2,d4
	move.l	d4,a0
	add.w	d6,a0
	move.l	a0,d4

	add.l	d4,a4
	add.l	d4,a6

	ext.l	d3		*y2*128+x2
	asl.l	d2,d3
	move.l	d3,a0
	add.w	d5,a0
	move.l	a0,d3

	add.l	d3,a3
	add.l	d3,a5

	cmp.l	a4,a3
	bcc	tscrw3

	subq.w	#1,d7
tscrw9:
	move.w	a2,d0
	lsr.w	#2,d0
	beq	tscrw81
	subq.w	#1,d0
tscrw8:
	move.l	(a4)+,(a3)+
	move.l	(a6)+,(a5)+
	dbra	d0,tscrw8

tscrw81:
	move.w	a2,d0
	and.w	#2,d0
	beq	tscrw83

	move.w	(a4)+,(a3)+
	move.w	(a6)+,(a5)+
tscrw83:

	add.w	d1,a4
	add.w	d1,a3
	add.w	d1,a6
	add.w	d1,a5
	dbra	d7,tscrw9
	bra	tscrw10

tscrw3:
	move.w	d7,d0		*ys*128-ofs
	ext.l	d0
	moveq.l	#7,d2
	asl.l	d2,d0
	move.l	d0,a0
	sub.w	d1,a0
	move.l	a0,d0

	add.l	d0,a4
	add.l	d0,a3
	add.l	d0,a6
	add.l	d0,a5

	subq.w	#1,d7
tscrw16:
	move.w	a2,d0
	lsr.w	#2,d0
	beq	tscrw152
	subq.w	#1,d0
tscrw15:
	move.l	-(a4),-(a3)
	move.l	-(a6),-(a5)
	dbra	d0,tscrw15

tscrw152:
	move.w	a2,d0
	and.w	#2,d0
	beq	tscrw154

	move.w	-(a4),-(a3)
	move.w	-(a6),-(a5)
tscrw154:

	sub.w	d1,a4
	sub.w	d1,a3
	sub.w	d1,a6
	sub.w	d1,a5
	dbra	d7,tscrw16
tscrw10:
	move.l	a1,-(sp)
	DOS	_SUPER
	addq.w	#4,sp
	movem.l	(sp)+,d3-d7/a2-a6
	rts

*_txt_clr:
*					* âÊñ è¡ãé
*	movem.l	d0-d3/a1-a2,-(sp)
*
*	pea	0.w
*	DOS	_SUPER
*	move.l	d0,d2
*	addq.w	#4,sp
*
*	movea.l	#$e8002a,a2
*	move.w	(a2),d3
*	move.w	#$01f3,(a2)
*
*	lea	clr_line,a1
**	move.w	#0,(a1)
*	IOCS	_TXFILL
*
*	move.w	d3,(a2)	* e8002a ïúãA
*
*
*	move.l	d2,-(sp)
*	DOS	_SUPER
*	addq.w	#4,sp
*
*	movem.l	(sp)+,d0-d3/a1-a2
*	rts
*
*clr_line:
*	dc.w	0,0,0,1024,1024,0

*********************************************
* gfill
*********************************************
_fill:
*----------------------------------------------------
* graphic fill routine Copyright 1993/05/15 by turbo
*----------------------------------------------------
*_g_fill:
*	movem.l	d3-d7/a1-a6,-(sp)
*
*	move.l	28+20(sp),d1	* x1
*	move.l	32+20(sp),d0	* y1
*	move.w	38+20(sp),d5	* x2
*	move.w	42+20(sp),d7	* y2
*	move.l	44+20(sp),d2	* col
*
*	cmp.w	d5,d1
*	ble	@f
*	move.w	d1,d4
*	move.w	d5,d1
*	move.w	d4,d5
*@@:
*	cmp.w	d7,d0
*	ble	@f
*	move.w	d0,d4
*	move.w	d7,d0
*	move.w	d4,d7
*@@:
*
*	cmp.w	#-1,d0
*	bne	@f
*	moveq.l	#0,d0
*@@:
*	cmp.w	#-1,d1
*	bne	@f
*	moveq.l	#0,d1
*@@:
*
*	sub.w	d1,d5		* xs=x2-x1+1
*	addq.w	#1,d5
*	sub.w	d0,d7		* ys=y2-y1+1
*
*	move.w	d5,d6		* ss=(xs+1024)*2
*	add.w	#1024,d6
*	add.w	d6,d6
*
*	moveq.l	#10,d4		* gptr=y1*1024+x1+gram
*	asl.l	d4,d0
*	add.l	d1,d0
*	add.l	d0,d0
*	move.l	d0,a6
*	add.l	#12582912,a6
*
*	move.w	d5,d0		* gptr+=xs
*	add.w	d0,d0
*	adda.w	d0,a6
*
*	move.w	d2,d0		* col=col+col*65536
*	swap.w	d2
*	move.w	d0,d2
*
*	pea	0.w
*	DOS	_SUPER
*	addq.w	#4,sp
*
*	move.l	d2,d3
*	move.l	d2,a0
*	move.l	d2,a1
*	move.l	d2,a2
*	move.l	d2,a3
*	move.l	d2,a4
*	move.l	d2,a5
*
*	cmp.w	#255,d5
*	ble	gf2loop
*
*gfloop:
*	move.w	d5,d1
*
*gfdot512:
*	btst	#9,d1
*	beq	gfdot256
*	rept	32
*	movem.l	d2-d3/a0-a5,-(a6)
*	endm
*gfdot256:
*	btst	#8,d1
*	beq.s	gfdot128
*	rept	16
*	movem.l	d2-d3/a0-a5,-(a6)
*	endm
*gfdot128:
*	add.b	d1,d1
*	bcc.s	gfdot64
*	rept	8
*	movem.l	d2-d3/a0-a5,-(a6)
*	endm
*gfdot64:
*	add.b	d1,d1
*	bcc.s	gfdot32
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*gfdot32:
*	add.b	d1,d1
*	bcc.s	gfdot16
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*gfdot16:
*	add.b	d1,d1
*	bcc.s	gfdot8
*	movem.l	d2-d3/a0-a5,-(a6)
*gfdot8:
*	add.b	d1,d1
*	bcc.s	gfdot4
*	movem.l	d2-d3/a0-a1,-(a6)
*gfdot4:
*	add.b	d1,d1
*	bcc.s	gfdot2
*	movem.l	d2/d3,-(a6)
*gfdot2:
*	add.b	d1,d1
*	bcc.s	gfdot1
*	move.l	d2,-(a6)
*gfdot1:
*	add.b	d1,d1
*	bcc.s	gfnext
*	move.w	d2,-(a6)
*gfnext:
*	add.w	d6,a6
*	dbra	d7,gfloop
*
*	move.l	d0,-(sp)
*	DOS	_SUPER
*	addq.w	#4,sp
*
*	movem.l	(sp)+,d3-d7/a1-a6
*	rts
*
*
*gf2loop:
*	move.w	d5,d1
*gf2dot128:
*	add.b	d1,d1
*	bcc.s	gf2dot64
*	rept	8
*	movem.l	d2-d3/a0-a5,-(a6)
*	endm
*gf2dot64:
*	add.b	d1,d1
*	bcc.s	gf2dot32
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*gf2dot32:
*	add.b	d1,d1
*	bcc.s	gf2dot16
*	movem.l	d2-d3/a0-a5,-(a6)
*	movem.l	d2-d3/a0-a5,-(a6)
*gf2dot16:
*	add.b	d1,d1
*	bcc.s	gf2dot8
*	movem.l	d2-d3/a0-a5,-(a6)
*gf2dot8:
*	add.b	d1,d1
*	bcc.s	gf2dot4
*	movem.l	d2-d3/a0-a1,-(a6)
*gf2dot4:
*	add.b	d1,d1
*	bcc.s	gf2dot2
*	movem.l	d2/d3,-(a6)
*gf2dot2:
*	add.b	d1,d1
*	bcc.s	gf2dot1
*	move.l	d2,-(a6)
*gf2dot1:
*	add.b	d1,d1
*	bcc.s	gf2next
*	move.w	d2,-(a6)
*gf2next:
*	add.w	d6,a6
*	dbra	d7,gf2loop
*
*	move.l	d0,-(sp)
*	DOS	_SUPER
*	addq.w	#4,sp
*
*	movem.l	(sp)+,d3-d7/a1-a6
*	rts

	.end

