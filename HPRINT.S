*-------------------------------------------------
*
*	<<<超高速print>>>
*
*	H_PRINT( char *w, char *y, char *s )
*		w:ホワイトでputする文字列
*		y:イエローでputする文字列
*		s:シアン  でputする文字列
*
*	Written by IRUKA 1989. All right reserved.
*	customize by TURBO 1991,1992
*
*-------------------------------------------------

	.cpu	68000

	.include	doscall.mac
	.include	iocscall.mac

FLASHRESS	=	1	* ちらつきの無いモード
IOCSFONT	=	1	* ユーザーフォント対応
MODULETEST	=	0	* 単体テスト
*
*	public	_H_ERA		* 行消去	(Current Line)
	public	_H_PRINT	* 超高速print	(*Cyan,*White,EOL)
	public	_H_PRINT2	* 超高速print	(*Cyan,*White,EOL)
	public	_H_INIT		* 準備 		(ANK fontの読み出し等)
*	public	_LDIRL		* ブロック転送	(long word)
	.xdef	_font
*
*	_LDIRLはロングワードでの転送
*	_H_ERA,_H_PRINTを行なう前に必ず _H_INITを呼び出すこと。
*					_H_INITは一度呼び出すだけでよい。
*	_H_PRINT使用後に、CLOSEに当たる処理は不要
*
*_EXIT		equ	$ff00
*_SUPER		equ	$ff20
*
X_MAX	equ	$970
Y_MAX	equ	$972
X_POS	equ	$974
Y_POS	equ	$976


push	macro	reg
	move.l	reg,-(sp)
	endm

pop	macro	reg
	move.l	(sp)+,reg
	endm
pushm	macro	regs
	movem.l	regs,-(sp)
	endm

popm	macro	regs
	movem.l	(sp)+,regs
	endm

put8	macro	n
	move.b	(a4)+,n*128-1(a5)
	endm

clr8	macro	n
	move.b	d0,n*128-1(a5)
	endm

put16	macro	n
	move.b	(a4)+,n*128-2(a5)
	move.b	(a4)+,n*128-1(a5)
	endm

put16w	macro	n
	move.w	(a4)+,n*128-2(a5)
	endm

ldz	macro	zpg,reg
	dc.w	$3038+reg*512	;0011 000 000 111 000
	dc.w	zpg
	endm

stz	macro	reg,zpg
	dc.w	$31c0+reg	;0011 000 111 000 000
	dc.w	zpg
	endm

ldzb	macro	zpg,reg
	dc.w	$1038+reg*512	;0011 000 000 111 000
	dc.w	zpg
	endm

stzb	macro	reg,zpg
	dc.w	$11c0+reg	;0011 000 111 000 000
	dc.w	zpg
	endm

ldzl	macro	zpg,reg
	dc.w	$2038+reg*512	;0011 000 000 111 000
	dc.w	zpg
	endm

stzl	macro	reg,zpg
	dc.w	$21c0+reg	;0011 000 111 000 000
	dc.w	zpg
	endm

tstzb	macro	zpg
	dc.w	$4a38
	dc.w	zpg
	endm

cmpz	macro	zpg,reg
	dc.w	$b078+reg*512	;1011 000 001 111 000
	dc.w	zpg
	endm

cmpzb	macro	zpg,reg
	dc.w	$b038+reg*512	;1011 000 000 111 000
	dc.w	zpg
	endm

clrz	macro	zpg
	dc.w	$4278		;0100 001 001 111 000
	dc.w	zpg
	endm

clrzb	macro	zpg
	dc.w	$4238		;0100 001 000 111 000
	dc.w	zpg
	endm

subz	macro	zpg,reg
	dc.w	$9078+reg*512	;1011 000 001 111 000
	dc.w	zpg
	endm

incz	macro	zpg
	dc.w	$5278
	dc.w	zpg
	endm

incz2	macro	zpg
	dc.w	$5478
	dc.w	zpg
	endm

decz	macro	zpg
	dc.w	$5378
	dc.w	zpg
	endm



	.text

	if	MODULETEST
	bsr	_H_INIT
	pea	MES1
	pea	MES2
	bsr	_H_PRINT
	DOS	_EXIT
MES2:	dc.b	' 123',$7f,0
MES1:	dc.b	'Hello                 1234567890漢字ABC',0
	even
	endif


*-------------------------------------------------
*
*	準備()
*
*-------------------------------------------------
_H_INIT:
	pea	0.w
	DOS	_SUPER
	addq.w	#4,SP

	movem.l	d0-d2/a0-a1,-(a7)

	bsr	m11
	bsr	sheffer
	bsr	eolchar
	bsr	eeol_line63
	movem.l	(a7)+,d0-d2/a0-a1

	move.l	d0,-(sp)
	DOS	_SUPER
	addq.w	#4,sp
	rts

eeol_line63:
	move.l	#$e00000,a0
	add.l	#128*16*63,a0
	  bsr	eeol_1plane
	add.l	#$020000,a0

eeol_1plane:
	move.l	a0,-(sp)
	move	#128,d1
	moveq.l	#0,d0
eeol_l63:
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	subq	#1,d1
	bne	eeol_l63

	lea.l	_font+16*16,a0
	lea.l	_exfont,a1
	move	#3*16-1,d1
@@:
	move.l	(a1)+,(a0)+
	dbra	d1,@b

	move.l	(sp)+,a0
	rts

_exfont:
	dc.b	$00,$00,$40,$44,$5C,$7C,$64,$44,$44,$5C,$7C,$64,$44,$04,$00,$00
	dc.b	$00,$00,$40,$40,$40,$40,$40,$58,$64,$44,$48,$48,$50,$60,$00,$00
	dc.b	$00,$00,$04,$26,$3E,$7C,$64,$24,$24,$26,$3E,$7C,$64,$20,$00,$00

*-------------------------------------------------
*
*	１行消去()
*
*-------------------------------------------------
*_H_ERA:
*	movem.l	d1-d3,-(a7)
*	ldz	$00000976,1	* y
*	and	#$003f,d1
*	asl	#2,d1
*	add	#$fc00,d1
*	moveq	#$0004,d2
*	moveq	#$0003,d3
*	moveq	#$df,d0
*	trap	#15		* Raster Copy
*	movem.l	(a7)+,d1-d3
*	rts

*-------------------------------------------------
*
*	<<<超高速print>>>
*
*	H_PRINT( char *s, char *w)
*		s:シアン  でputする文字列
*		w:ホワイトでputする文字列
*
*	レジスタ割り当てリスト
*		a0:表示文字列を指すポインタ        (読み取り)
*		a2:e8002a(同時書き込みポート)
*		a3:半角フォントのバッファを指すポインタ(固定)
*
*		a4:putするフォントのバッファポインタ
*		a5:putするVRAMアドレスのポインタ
*
*		d0:putする文字 := (a0)++
*		d1:ShiftJISフォント用
*		d2-d3:scratch!
*
*
*	str: 7f=縦棒
*	     1f=改行マーク
*-------------------------------------------------
_H_PRINT:
_HP_40:
	movem.l	d1-d3/a0/a2-a5,-(a7)

******	bsr	erase_current_line	* CURRENT LINE CLEAR
*-------------------------------------------------
*	まず現在行を消去する
*	63行目はクリアされているものとする。
*
erase_current_line:
    if	FLASHRESS
	move	#$fcf8,d1	* Line 63-->62
    else
	ldz	$00000976,1	* y
	and	#$003f,d1
	asl	#2,d1
	add	#$fc00,d1
    endif
	moveq	#$0004,d2
	moveq	#$0003,d3
* d1=Src_raster/4 *256 +Dst_raster/4
* d2=count/4
* d3=direction(MSB)+plane enable
	moveq	#$df,d0
	trap	#15		* Raster Copy

*****	bsr	_HSETUP			* (x,y) ---> a5
*---------------- High Speed Putc ----------------
* Entry is Here !!!! 
_HSETUP:	* set vram address --> a5
		* set font address --> a3

	movea.l	#$e8002a,a2
	move.w	(a2),e8002a_sav

	move.w	#$0133,(a2)		* WHITE
	moveq	#0,D0			* d0= 00000000
	move	#$e0,d1
	swap	d1			* d1= 00e0xxxx
	ldz	$00000974,1		* x:d1

    if	FLASHRESS
	move.l	#62*128*16,d0		* y: 62行
    else
	ldz	$00000976,0		* y:d0
	ror	#6,D0			* 	y *= (128 * 16)
	asl.l	#1,D0			*	
    endif

	add.l	D1,D0			*	y += x + e00000
	movea.l	D0,A5			*  a5 = y;
*	lea	_font(PC),a3		* font  ==   a3
	lea	_font,a3		* font  ==   a3

	  move.l 9*4(a7),a0		* 最初の文字列
	bsr	_HP_60

	  move	 #$0123,(a2)		* YELLOW
	  move.l 10*4(a7),a0		* ２番目の文字列
	bsr	_HP_60

	  move	 #$0113,(a2)		* CYAN!
	  move.l 11*4(a7),a0		* ３番目の文字列
	bsr	_HP_60

*	bsr	_H_CRMARK		* 改行マーク


    if	FLASHRESS
	ldz	$00000976,1	* y
	and	#$003f,d1
	asl	#2,d1
	add	#$f800,d1
	moveq	#$0004,d2
	moveq	#$0003,d3
* d1=Src_raster/4 *256 +Dst_raster/4
* d2=count/4
* d3=direction(MSB)+plane enable
	moveq	#$df,d0
	trap	#15		* Raster Copy
    endif

*	  move	 e8002a_sav(PC),(a2)	* e8002a 復帰
	  move	 e8002a_sav,(a2)	* e8002a 復帰
	movem.l	(a7)+,d1-d3/a0/a2-a5
	rts

*-------------------------------------------------
_H_PRINT2:
	movem.l	d1-d3/a0/a2-a5,-(a7)

	movea.l	#$e8002a,a2
	move.w	(a2),e8002a_sav

	move.w	#$0133,(a2)		* WHITE
	moveq	#0,D0			* d0= 00000000
	move	#$e0,d1
	swap	d1			* d1= 00e0xxxx
	ldz	$00000974,1		* x:d1

	ldz	$00000976,0		* y:d0
	ror	#6,D0			* 	y *= (128 * 16)
	asl.l	#1,D0			*	

	add.l	D1,D0			*	y += x + e00000
	movea.l	D0,A5			*  a5 = y;
*	lea	_font(PC),a3		* font  ==   a3
	lea	_font,a3		* font  ==   a3

	  move.l 9*4(a7),a0		* 最初の文字列
	bsr	_HP_60

	  move	 #$0123,(a2)		* YELLOW
	  move.l 10*4(a7),a0		* ２番目の文字列
	bsr	_HP_60

	  move	 #$0113,(a2)		* CYAN!
	  move.l 11*4(a7),a0		* ３番目の文字列
	bsr	_HP_60

*	  move	 e8002a_sav(PC),(a2)	* e8002a 復帰
	  move	 e8002a_sav,(a2)	* e8002a 復帰
	movem.l	(a7)+,d1-d3/a0/a2-a5
	rts

*-------------------------------------------------
_HSCLR8:
	addq.l	#1,a5
	bra	_HP_59
*				ASCII | kana
_HP_ea:
	asl.w	#4,d0
	lea	(a3,d0.w),a4	* font

	move.b	(a4)+,(a5)+
	put8	1
	put8	2
	put8	3
	put8	4
	put8	5
	put8	6
	put8	7
	put8	8
	put8	9
	put8	10
	put8	11
	put8	12
	put8	13
	put8	14
	put8	15
_HP_59:
	incz	$974
_HP_60:
	moveq	#0,d0
	move.b	(a0)+,d0
	  beq	_HP__14		* EOL
	cmp.b	#$20,d0
	  beq	_HSCLR8		* SPACE
	cmp.b	#$81,d0
	  bcs	_HP_ea		* ASCii
	cmp.b	#$a0,d0
	  bcs	_HP_b0		* kanji
	cmp.b	#$e0,d0
	  bcs	_HP_ea		* kana
	cmp.b	#$f0,d0
	  bcs	_HP_b0		* kanji
	cmp.b	#$f6,d0
	  bcs	_HP_b0		* f0-f5
	  bra	_HP_ea		* ASCii
*
*	<EOL>
_HP__14:
	rts

*-------------------------------------------------
*
*	KANJI PUT
*
	if	IOCSFONT
*
*	KANJI			IOCS CALL
*

_HP_b0:
	lsl.w	#8,d0
	move.b	(a0)+,d0
	bsr	_HIJIS
	bra	_HP_60
	rts

* 全角漢字

_HIJIS:
* Second,Get Font Pattern. ---> a4
	move	d0,d1	* Shift JIS CODE
	moveq	#8,d2	* Size
	moveq	#$16,d0
	trap	#15	* Get Kanji ROM address -->d0.l
	move.l	d0,a4
	move	d1,d0
	tst	d0
	beq	hj1

* Third,Put Font (a4) ---> (a5)
	move.b	(a4)+,(a5)+
	move.b	(a4)+,(a5)+

	put16	1
	put16	2
	put16	3
	put16	4
	put16	5
	put16	6
	put16	7
	put16	8
	put16	9
	put16	10
	put16	11
	put16	12
	put16	13
	put16	14
	put16	15

	incz2	$974
	rts

	else

*
*	KANJI		NON IOCS CALL
*
_HP_b0:
	lsl.w	#8,d0
	move.b	(a0)+,d0
* 全角漢字
_HIJIS:	move	d0,d1
	bsr	JISROM
	move.l	d0,a4

	move.l	a5,d1
	btst	#0,d1
	  beq	_HIJISW
* Put Font (a4) ---> (a5)
	move.b	(a4)+,(a5)+
	move.b	(a4)+,(a5)+

	put16	1
	put16	2
	put16	3
	put16	4
	put16	5
	put16	6
	put16	7
	put16	8
	put16	9
	put16	10
	put16	11
	put16	12
	put16	13
	put16	14
	put16	15

	incz2	$974
	bra	_HP_60

* Put Font (a4) ---> (a5)
_HIJISW:
	move.w	(a4)+,(a5)+
	put16w	1
	put16w	2
	put16w	3
	put16w	4
	put16w	5
	put16w	6
	put16w	7
	put16w	8
	put16w	9
	put16w	10
	put16w	11
	put16w	12
	put16w	13
	put16w	14
	put16w	15

	incz2	$974
	bra	_HP_60
*----------------------------------------------------------------
	endif

*_H_CRR:	rts
*_H_CRMARK:
*	ldz	X_POS,0
*	cmpz	X_MAX,0
*	bhi	_H_CRR
*_eolput:
*	moveq	#$1f,d0
*	asl	#4,d0
*	lea	0(a3,d0),a4	* font

	if	IOCSFONT
hj1:
	move.b	(a4)+,(a5)+
	put8	1
	put8	2
	put8	3
	put8	4
	put8	5
	put8	6
	put8	7
	put8	8
	put8	9
	put8	10
	put8	11
	put8	12
	put8	13
	put8	14
	put8	15
	incz	$974
	rts

	else
*
*	ShiftJIS(d1) --> ROM address(d0.l)
*
JISROM:
	cmp	#$eb00,d1	* eb00 ~
	  bcc.s	_ORGROM

* 下位バイトのチェック(40~7e,80~fc)
	move.l	#$00f00000/32,d0	* 漢字ROMベースアドレス
	move.b	d1,d0
	sub.b	#$40,d0
	  bcs.s	_UFO
	cmp.b	#$7f,d1
	  bcs.s	_JR1
	  beq.s	_UFO
	subq.b	#1,d0
	cmp.b	#$fd,d1
	  bcc.s	_UFO
_JR1:
*--------------------------------------
*	move	d1,d2
*	lsr	#8,d2
*	sub	#$0081,d2
*	mulu	#$00bc,d2
*	add.l	d2,d0
*--------------------------------------
	*代替乗算 		d0 += (d1-$81)*$bc
	moveq	#0,d2
	move	d1,d2
	and	#$ff00,d2
	sub	#$8100,d2
	lsr	#1,d2		* x %1100_0000 - %100
	  add.l	d2,d0
	lsr	#1,d2
	  add.l	d2,d0
	lsr	#4,d2
	  sub.l	d2,d0
*--------------------------------------
	lsl.l	#5,d0			* x 32bytes
	cmp	#$8500,d1
	  bcs.s	_JRET
	cmp	#$889f,d1
	  bcs.s	_ORGROM			* ＮＥＣ外字

	sub.l	#$00005240,d0
	cmp	#$a000,d1		* 非漢字ギャップ
	  bcs.s	_JRET

	sub.l	#$0005e000,d0
	cmp	#$e000,d1		* 第二水準ギャップ
	  bcc.s	_JRET

_ORGROM:
	moveq	#8,d2	* Size
	moveq	#$16,d0
	trap	#15	* Get Kanji ROM address -->d0.l
_JRET:	rts

*
*	豆腐文字
*
_UFO:	lea	_UFONT(pc),a4
	move.l	a4,d0
	rts
_UFONT:	dc.w	$5555,$aaaa,$5555,$aaaa,$5555,$aaaa,$5555,$aaaa
	dc.w	$5555,$aaaa,$5555,$aaaa,$5555,$aaaa,$5555,$aaaa

	endif




m11:
	moveq.l	#$20,d1
	moveq.l	#$7f,d2
	bsr	m111
	move.l	#$a0,d1
	move.l	#$e0,d2
m111:
	push	d1
	push	d2
	bsr	getrom_
	pop	d2
	pop	d1

	addq	#1,d1
	cmp	d2,d1
	bne	m111
	rts


getrom_:moveq	#8,d2
	push	d1
	moveq	#$16,d0
	trap	#15
	pop	d1
	movea.l	d0,a0
	and	#$ff,d1
	asl	#4,d1
	add.l	#_font,d1
	movea.l	d1,a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	rts

sheffer:move.l	#$10101010,d0
	lea	$7f*16+_font,a0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	rts

eolchar:move.l	#$11f,d1
	bra	getrom_







*_LDIRL:
*	move.l	4(a7),a0
*	move.l	8(a7),a1
*	move.l	12(a7),d0
*	subq	#1,d0
*	lsr	#2,d0
*_ldir1:
*	move.l	(a0)+,(a1)+
*	dbra	d0,_ldir1
*	rts

	.bss
	.even
e8002a_sav:	dc.w	0
_font:		ds.b	4096
*stackSave:
*	ds.l	1
	.end
