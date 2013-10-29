	.cpu	68000

	.include iocscall.mac
	.include doscall.mac

	.text
	.even
	.xdef	_memcpy_l
	.xdef	_keyclr

_memcpy_l:
	move.l	4(sp),a0	* dst
	move.l	8(sp),a1	* src
	move.l	12(sp),d0	* len

	tst.l	d0
	beq	mca20

	lsr.l	#2,d0
	move.l	d0,d2
	subq.l	#1,d0

	move.l	d0,d1
	swap	d1

	cmp.l	a0,a1
	beq	mca20
	bcc	mcs10
mca00:				* src<dst
	lsl.l	#2,d2
	add.l	d2,a1
	add.l	d2,a0
mca10:
	move.l	-(a1),-(a0)
	dbra	d0,mca10
	dbra	d1,mca10
mca20:
	rts
mcs10:				* src>dst
	move.l	(a1)+,(a0)+
	dbra	d0,mcs10
	dbra	d1,mcs10
mcs20:
	rts


_keyclr:		* ã≠êßìIÇ…keyoffÇ≥ÇπÇÈ
	pea	0.w
	DOS	_SUPER
	addq.w	#4,sp

	moveq.l	#4-1,d1
	move.l	#$80a.w,a0
kloop:
	clr.b	(a0)+
	dbra	d1,kloop

	move.l	d0,-(sp)
	DOS	_SUPER
	addq.w	#4,sp

	rts

	.end
