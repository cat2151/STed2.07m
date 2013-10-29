/* X68000 music keyboard v0.2 1989/04/17 by TURBO */

/*
 * STed: music keyboard (1994/07/20)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
/*#include	"basic0.h"
#include	"BASIC.h"*/
#include	"graph.h"
#include	"sted.h"


unsigned char	bitkey[7][10]={
	"  123456","7890-^\\ "," QWERTYU","IOP@[ AS","DFGHJKL;",":]ZXCVBN","M,./_   "};

unsigned char	keytable[44]="AZSXCFVGBNJMK,L./:_]" "1Q2WE4R5TY7U8I9OP-@^[\\";

/*C4=60*/
unsigned char	keycode[44]={
	0 ,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
	56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77
};
/*
ÏJÏKÏLÏMÏNÏOÏP
ÏQÏQÏQÏQÏQÏQÏQ

ÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏP
ÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQ
ÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏPÏJÏKÏLÏMÏNÏOÏP
ÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQÏQ
  c1     c2     c3     c4     c5     c6     c7     c8
*/
void	msg();
void	msg_clr();
void	snsclr();

/***************************/
void	m_keyb(int ch,int bank,int prg,int velo)
{
	int	bnk_m,bnk_l,pm,ch2;
	int	a,i;

	unsigned char	onkey[32],onnote[32];
	unsigned char	s[16],ke[128];
	char	obuf[64];

	msg("Music KeyBoard Emulater");

	if(rcd->act==0){all_note_off(0);}
	bnk_l=bank>>8;bnk_m=bank&0xff;ch=(ch-1)&31;ch2=ch&15;
	pm=rcd->put_mode;rcd->put_mode=rcd->ch_port[ch];
	twait(5);

	if(bank>=0 && tim_asin[ch+1]>=7 && tim_asin[ch+1]<=14){
		obuf[0]=0xb0+ch2;obuf[1]=0;obuf[2]=bnk_m;
		obuf[3]=32;obuf[4]=bnk_l;obuf[5]=255;
		(*rcd->mix_out)(obuf);
	}

	if(prg>=0){
		obuf[0]=0xc0+ch2;obuf[1]=prg;obuf[2]=255;
		(*rcd->mix_out)(obuf);
	}
	twait(8);

	strcpy(onkey,"                ");

	while (1){
		int	ln,sh;
		if(INPOUT(0xff)==27){break;}
		sh=B_SFTSNS();

		ln=0;
		for(i=0;i<=6;i++){
			a=BITSNS(i);
			if( a & 1   ){ke[ln++]=bitkey[i][0];}
			if( a & 2   ){ke[ln++]=bitkey[i][1];}
			if( a & 4   ){ke[ln++]=bitkey[i][2];}
			if( a & 8   ){ke[ln++]=bitkey[i][3];}
			if( a & 16  ){ke[ln++]=bitkey[i][4];}
			if( a & 32  ){ke[ln++]=bitkey[i][5];}
			if( a & 64  ){ke[ln++]=bitkey[i][6];}
			if( a & 128 ){ke[ln++]=bitkey[i][7];}
		}
		ke[ln]=0;

		for (i=0;i<16;i++){
			if ( onkey[i]>32 ) {
				int a;
				s[0]=onkey[i];s[1]=0;
				a=str_search(ke,s);
				if( a==0 ){
					int note;
					onkey[i]=32;note=onnote[i];

					/* note off */

					obuf[0]=0x90+ch2;obuf[1]=note;obuf[2]=0;
					obuf[3]=255;(*rcd->mix_out)(obuf);
				}
			}
		}
		ln=0;
		for (i=0;i<16;i++){
			if(ke[ln]==0){break;}
			if(onkey[i]==32){
				s[0]=ke[ln++];s[1]=0;
				if(str_search(onkey,s)==0){
					int a;
					a=str_search(keytable,s);
					if(a!=0){
						int note;
						onkey[i]=s[0];note=keycode[a];
						if(sh&1){note+=12;}/*shift*/
						if(sh&2){note-=12;}/*ctrl*/
						if(sh&128){note+=12;}/*caps*/
						if(sh&256){note-=12;}/*Ç©Ç»*/
						onnote[i]=note;

						/* note on */

						obuf[0]=0x90+ch2;obuf[1]=note;obuf[2]=velo;
						obuf[3]=255;(*rcd->mix_out)(obuf);

					}
				}
			}
		}
	}

/*
97-10-24	key.emu.Ç≈î≠âπÇµÇΩÇ‹Ç‹Ç…Ç»ÇÈÇ©Ç‡ÇµÇÍÇ»Ç¢ÇÃÇèCê≥ÇµÇΩ

	for (i=0;i<16;i++){
		if ( onkey[i]>32 ) {
			int note;

			note=onnote[i];

			*//* note off *//*

			obuf[0]=0x90+ch2;obuf[1]=note;obuf[2]=0;
			obuf[3]=255;(*rcd->mix_out)(obuf);
		}
	}
*/

	snsclr();rcd->put_mode=pm;msg_clr();
}
