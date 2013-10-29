/*
 * STed2.x:key_sub.c midi in & chord & key name
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

/* ************** chord list */
static	char	chord_name[20][9]={
	" MAJOR  "," 7      "," maj7   "," 6      "," maj9   ",
	" 9      "," 7(b9)  "," 7(#9)  "," dim    "," dim7   ",
	" 7(b5)  "," m7(b5) "," aug    "," aug7   "," 7sus4  ",
	" 69     "," m9     "," m6     "," m7     "," m      "};

static char	chord_pat[20][5]={
	4, 7, 0, 0,0, /* 	*/	4, 7,10, 0,0, /* 7	*/
	4, 7,11, 0,0, /* maj7	*/	4, 7, 9, 0,0, /* 6	*/
	4, 7,11,14,0, /* maj9	*/	4, 7,10,14,0, /* 9	*/
	4, 7,10,13,0, /* 7(b9)	*/	4, 7,10,15,0, /* 7(#9)	*/
	3, 6, 9, 0,0, /* dim	*/	3, 6,10, 0,0, /* dim7	*/

	4, 6,10, 0,0, /* 7(b5)	*/	3, 6,10, 0,0, /* m7(b5)	*/
	4, 8, 0, 0,0, /* aug	*/	4, 8,10, 0,0, /* aug7	*/
	5, 7,10, 0,0, /* 7sus4	*/	4, 7, 9,14,0, /* 69	*/
	3, 7,10,14,0, /* m9	*/	3, 7, 9, 0,0, /* m6	*/
	3, 7,10, 0,0, /* m7	*/	3, 7, 0, 0,0, /* m	*/
};

/* *** key scale *** */
static char	key_name[32][4]={
	" C "," G "," D "," A "," E "," B ","FÅÚ","CÅÚ",
	" C "," F ","BÅÛ","EÅÛ","AÅÛ","DÅÛ","GÅÛ","CÅÛ",

	" A "," E "," B ","FÅÚ","CÅÚ","GÅÚ","DÅÚ","AÅÚ",
	" A "," D "," G "," C "," F ","BÅÛ","EÅÛ","AÅÛ"};

/*       C  C# D  D# E  F  F# G  G# A  A# B */
static char	key_shi[16][12]={
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*    */
	 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,	/* #1 */
	 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,	/* #2 */
	 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,	/* #3 */
	 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0,	/* #4 */
	 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0,	/* #5 */
	 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0,	/* #6 */
	 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1,	/* #7 */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*    */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,	/* b1 */
	 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0,-1,	/* b2 */
	 0, 0, 0, 0,-1, 0, 0, 0, 0,-1, 0,-1,	/* b3 */
	 0, 0,-1, 0,-1, 0, 0, 0, 0,-1, 0,-1,	/* b4 */
	 0, 0,-1, 0,-1, 0, 0,-1, 0,-1, 0,-1,	/* b5 */
	-1, 0,-1, 0,-1, 0, 0,-1, 0,-1, 0,-1,	/* b6 */
	-1, 0,-1, 0,-1,-1, 0,-1, 0,-1, 0,-1,	/* b7 */
};

/* *** note name *** */
static char	note[12][3]={
	"C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B "};
/*	"C ","Db","D ","Eb","E ","F ","Gb","G ","Ab","A ","Bb","B "};*/

static char	note2[24][3]={
	"C ","C\x12","D ","D\x12","E ","F ","F\x12","G ","G\x12","A ","A\x12","B ",
	"C ","D\x11","D ","E\x11","E ","F ","G\x11","G ","A\x11","A ","B\x11","B "};

static char	oct[11]={
	'.','0','1','2','3','4','5','6','7','8','9'};

int	outch,prog;

int	timsel();
void	msg();
void	msg_clr();
void	fnc_dis();
void	sdis();
int	varia();
void	sinput();
char	*fstr();
char	*hex_s();
char	*spadd();
char	*jstrupr();
int	str_val();
int	vinput();
void	home2();
void	poplay();
void	gra();
void	all_note_off();
int	trk_shift();
void	trk_delete();
void	snsclr();
void	undobuf();
void	noed_cls();
void	g_print();
void	twait();
int	strch();
char	*chstr();
char	*tim_names();

void	inpmon();
void	midi_change();
int	midi_inout();
void	midi_chout();
int	midi_in();
void	midi_clr();
char	*mdinport();
int	polykey();
int	onekey();
int	chord_sel();
int	chord_ten();
int	chord_ins();
void	chord_dis();
int	mml_inp();
int	scale_no();
char	*keystr();
char	*keystr2();
char	*prog_name();
int	key_shift();
void	key_shift_sub();

/***************************/
void	inpmon()
{
	int	a,b,c,f=0;

	all_note_off(1);outch=0;prog=0;

	fill(0,544,767,1001,0);box(0,564,767,1001,14,65535);
	sdis(0,548," MIDI INPUT MONITOR",20,15,8);
	home2(512);

	C_WINDOW(0,32);

	B_LOCATE(2,31);B_PRINT("MIDI IN:");B_PRINT(mdinport());

	B_LOCATE(25,31);B_PRINT("OUT MIDI CH.:THRU");
	B_LOCATE(76,31);B_PRINT("[ESC]Ç≈èIóπÇµÇ‹Ç∑ÅB");
	C_WINDOW(4,26);B_LOCATE(0,0);
	midi_clr();

	while(1){
		if(f==0){a=midi_inout();}else{a=f;f=0;}
		if( a<0 ){break;}

		if( a>127 && a<0xf0){B_PRINT("\r\nch=");B_PRINT(fstr((a&15)+1,2));B_PRINT(" : ");}

		if( a>=0x80 && a<=0x9f){
			if( a<0x90 ){B_PRINT("note off :");}else{B_PRINT("note on  :");}
			while(1){
				b=midi_inout();if(b>127 ||b<0){f=b;break;}
				c=midi_inout();
				B_PRINT(keystr(b));B_PRINT(fstr(b,3));B_PRINT(",");
				B_PRINT(fstr(c,3));B_PRINT(":");
			}
			a=-1;
		}
		if( a>=0xa0 && a<=0xaf){
			B_PRINT("after key:");
			while(1){
				b=midi_inout();if(b>127 ||b<0){f=b;break;}
				c=midi_inout();
				B_PRINT(fstr(b,3));B_PRINT(",");
				B_PRINT(fstr(c,3));B_PRINT(":");
			}
			a=-1;
		}
		if( a>=0xb0 && a<=0xbf){
			B_PRINT("control  :");
			while(1){
				b=midi_inout();if(b>127 ||b<0){f=b;break;}
				c=midi_inout();
				B_PRINT(fstr(b,3));B_PRINT(",");
				B_PRINT(fstr(c,3));B_PRINT(":");
			}
			a=-1;
		}
		if( a>=0xc0 && a<=0xcf){
			b=midi_inout();
			B_PRINT("prog ch. : no.=");B_PRINT(fstr(b+1,3));
			a=-1;
		}
		if( a>=0xd0 && a<=0xdf){
			B_PRINT("after ch.:");
			while(1){
				b=midi_inout();if(b>127 ||b<0){f=b;break;}
				B_PRINT(fstr(b,3));B_PRINT(":");
			}
			a=-1;
		}
		if( a>=0xe0 && a<=0xef){
			B_PRINT("pitch    :");
			while(1){
				b=midi_inout();if(b>127 || b<0){f=b;break;}
				c=midi_inout();
				B_PRINT(fstr((c*128+b)-8192,5));
				B_PRINT(":");
			}
			a=-1;
		}
		if( a==0xf0 ){
			B_PRINT("\r\n exclusive : ");
			while(1){
				b=midi_inout();if(b>127 || b<0){f=b;break;}
				B_PRINT(hex_s(b,2));
				B_PRINT("H ");
			}
			a=-1;
		}
		if( a>=0 ){B_PRINT(fstr(a,4));}
	}
	C_WINDOW(0,32);B_CLR_AL();home2(0);
}

/***************************/
static inline void	midi_change(int md)
{
	int	c;
	char	tmp0[64];

	if(md!=0 && outch==0){return;}

	B_CLR_AL();C_WINDOW(0,32);
	if(md==0){
		msg("OUT MIDI CH.=");
		strcpy(tmp0,chstr(outch));
		sinput(tmp0,0x803);c=strch(outch,tmp0);

		if(c>=0 && c<=32){outch=c;}
		msg_clr();B_LOCATE(38,31);
		if(outch!=0){B_PRINT(chstr(outch));B_PRINT(" ");}else{B_PRINT("THRU");}
	}else{
		c=timsel(outch,0,36,prog);if(c>=0){c=c&0xff;}
		if(c>=0){
			prog=c;strcpy(tmp0,"PROGRAM:");strcat(tmp0,prog_name(outch,0,c,1));
			B_LOCATE(44,31);B_PRINT(tmp0);
		}
		midi_chout(0xc0+((outch-1)&15));midi_chout(prog);
	}
	C_WINDOW(4,26);B_LOCATE(0,0);
}

/***************************/
static inline int	midi_inout()
{
	int	a,b,k;
	while(1){
		a=(*rcd->md_get)();if(a>=0 && a<0xf8){break;}
		if( INPOUT(0xfe)!=0 ){
			k=INPOUT(0xff);
			if( k=='c' || k=='C' ){midi_change(0);}
			if( k=='g' || k=='G' ){midi_change(1);}
			if(k==27){return(-27);}
		}
	}
	b=a;
	if(outch>0 && b>127 && b<0xf0){b=(b&0xf0)+((outch-1)&15);}
	midi_chout(b);

	return(a);
}

/***************************/
void	midi_chout(int a)
{
	int	pm;

	pm=rcd->put_mode;
	if(outch>0){rcd->put_mode=rcd->ch_port[outch-1];}
	(*rcd->md_put)(a);
	rcd->put_mode=pm;
}

/***************************/
int	midi_in()
{
	int	a;
	while(1){
		if( INPOUT(0xfe)!=0 ){if(INPOUT(0xff)==27){return(-27);}}
		a=(*rcd->md_get)();
		if(a>=0 && a<0xf8){break;}
	}
	if(thrumode){(*rcd->md_put)(a);}
	return(a);
}

/***************************/
int	midi_in_fk()
{
	int	a;
	while(1){
		if( B_KEYSNS()!=0 ){
			int a=B_KEYINP()>>8;
			if(a>=0x63 && a<=0x6c){return((a-0x63)+(B_SFTSNS()&1)*16+0x200);}
			if(a==1){return(-27);}
		}
		a=(*rcd->md_get)();
		if(a>=0 && a<0xf8){break;}
	}
	if(thrumode){(*rcd->md_put)(a);}
	return(a);
}

/***************************/
void	midi_clr()
{
	while((*rcd->md_get)()!=-1){}
}

/***************************/
int	polykey(int cde)
{
	int	a,b,c,f,co=0,bl=0;

	f=cde;
	if(cde<0x80){
		c=midi_in();if(c>127 ||c<0){f=c;goto skip;}
		if(c!=0){
			bl++;dat[co]=cde;dat[co+1]=c;co=co+4;
		}
		f=0x90;
	}
skip:
	while(1){
		if(f==0){a=midi_in();}else{a=f;f=0;}
		if(a<0){break;}

		if( a>=0x80 && a<=0x9f){
			while(1){
				b=midi_in();if(b>127 ||b<0){f=b;break;}
				c=midi_in();if(c>127 ||c<0){f=c;break;}
				if(a<0x90 || c==0){
					bl--;if(bl<=0){return(co);}
				}else{
					bl++;dat[co]=b;dat[co+1]=c;co=co+4;
				}
			}
		}
		if(f<0){break;}
	}
	return(co);
}

/***************************/
static inline int	onekey(int imd,int cde)
{
	int	a,co=0;

	if(cde<0x80){dat[co]=cde;goto one_b;}

one_a:	a=midi_in();if(a>127 || a<0){goto next;}
	if(a>0xf7){goto one_a;}
	dat[co]=a;

one_b:	a=midi_in();if(a>127 || a<0){goto next;}
	if(a>0xf7){goto one_b;}
	dat[co+1]=a;

	if(dat[co+1]>0){co+=4;}
	if(imd==4 && co==4){return 4;}
	twait(4);
loop:
	if( INPOUT(0xfe)!=0 ){if(INPOUT(0xff)==27){return(-27);}}
	a=(*rcd->md_get)();if(a>0xf7){goto loop;}
	if(a>=0 && thrumode){(*rcd->md_put)(a);}
	if(a>=0 && a<128){dat[co]=a;goto one_b;}
next:
	if(a>=0x90 && a<=0x9f){goto one_a;}

	return(co);
}

/***************************/
static inline int	onekey2()
{
	int	a,b,c;

one_a:	a=midi_in_fk();if(a<0 || a>=0x200){return(a);}
	if(a>127 && (a>0x9f || a<0x90)){
		while((a>0x9f || a<0x90)){
			a=midi_in_fk();if(a<0 || a>=0x200){return(a);}
		}
		goto one_a;
	}
	if(a>127 ){
		b=midi_in_fk();if(b<0 || b>=0x200){return(b);}
	}else{b=a;}
	c=midi_in();if((b>=0x80 && b<=0x8f) || c==0){goto one_a;}

	return(b);
}

/***************************/
int	ret_midi(int cde,int imd,int ad,int cy)
{
	int	j,co=0,po=0,cc,k;
	unsigned char	b=48,c=46;

	msg("MIDI INPUT(NOTE)");
	if(imd==3 || imd==5){co=polykey(cde);}else{co=onekey(imd,cde);}
	msg_clr();if(co==0){return 0;}

	if(imd>=4){
		if(keyst_table[0][dat[0]]==255){
			unsigned char	cd=keyst_table[1][dat[0]];
			if(cd==0){dat[0]=0;co=4;}
			if(cd==1){return -1;}
			if(cd==2){return -2;}
		}
	}

	line_ins_aft(ad);
	if(trk_shift(ad,co)!=0){return(0);}

	if( ad>0 ){
		j=ad;
		while(j>0){
			j=j-4;
			if( trk[track][j]<128 ){b=trk[track][j+1];c=trk[track][j+2];
				if(b>0){if(c==0){c=b-1;}break;}
			}
		}
	}

	if(imd>=4){
		fnc_dis(2);
key2in:		msg("MIDI INPUT(ST/GT)");k=onekey2();msg_clr();
		if(k>=0x200){
			int kk=(k&15);
			b=stgt_tbl[kk];c=stgt_tbl[kk+10];if(k&16){c=0;}
		}else{
			if(k>=0){
				b=keyst_table[0][k];c=keyst_table[1][k];
				if(b==0 || b==255){goto key2in;}
				if(dat[0]==0){c=0;}
			}
		}
		fnc_dis(0);
	}
	cc=co;
	while(co>0){
		unsigned char	a,d;
		if(co>4 && (imd==3||imd==5)){a=0;}else{a=b;}
		if(c==0){d=0;}else{d=dat[po+1];}
		trk[track][ad]=dat[po];trk[track][ad+1]=a;
		trk[track][ad+2]=c;trk[track][ad+3]=d;
		co=co-4;po=po+4;ad+=4;
	}
	return(cc);
}

/***************************/
int	ret_chord(int ad)
{
	int	co;

	undobuf(0);
	co=chord_sel(trk[track][ad],ad);if(co<2){return(0);}
	line_ins_aft(ad);
	if(chord_ins(ad,co)!=0){return(0);}else{return(co*4-4);}
}

/***************************/
static inline int	chord_sel(int key,int ad)
{
	int	po,i,ke,kc,sh,exc=0,no=0,d,okey;
	char	temp[16];

	chord_dis(0);okey=key;po=chord_ten(key,0);

	while( 1){
		msg("chord :");B_PRINT(keystr(key));
		B_PRINT(":");B_PRINT(chord_name[no]);
		B_PRINT(":");for(i=0;i<po;i++){B_PRINT(" ");B_PRINT(keystr(dat[i]));}

		if(grpmode!=0){
			if(chord_ins(ad,po)==0){
				gra(ad,0);trk_delete(ad,po*4-4);snsclr();
			}
		}

		while(B_KEYSNS()==0){}
		ke=B_KEYINP();kc=ke&0xff;ke=ke>>8;sh=B_SFTSNS();

		if( ke==0x70 ){chord_dis(10);}
		if( ke==0xf0 ){chord_dis(0);}

		if(ke==1){exc=-1;trk[track][ad]=okey;break;}
		if( kc==13 ){exc=po;break;}

		if( ke>0x62 && ke<0x6d ){
			if( (sh & 1)!=0 ){no=ke-0x63+10;}else{no=ke-0x63;}
			po=chord_ten(key,no);
		}

/*cursor*/
		if( kc==0x13 || ke==0x3b){
			no--;if( no<0 ){no=19;}
			po=chord_ten(key,no);
		}
		if( kc==0x04 || ke==0x3d){
			no++;if( no>19 ){no=0;}
			po=chord_ten(key,no);
		}
		if( kc==0x05 || ke==0x3c){
			if((dat[0]+12)<=127){
				int	i;
				d=dat[0];for(i=1;i<po;i++){dat[i-1]=dat[i];}
				dat[po-1]=d+12;
			}
		}
		if( kc==0x18 || ke==0x3e){
			if((dat[po-1]-12)>=0){
				int	i;
				d=dat[po-1];for(i=po-1;i>=1;i--){dat[i]=dat[i-1];}
				dat[0]=d-12;
			}
		}

/*roll */
 		if( ke==0x39 ){if(key>0){key--;po=chord_ten(key,no);}}
		if( ke==0x38 ){if(key<127){key++;po=chord_ten(key,no);}}

		if(kc==7 && edit_scr==0){
			noed_cls();
			grpmode++;if(grpmode>4){grpmode=0;}
		}
		if(kc=='M'||kc=='m'||ke==0x55 ||ke==0x3a){
			if(chord_ins(ad,po)==0){
				int	i;
				for(i=0;i<po;i++){temp[i]=dat[i];}
				poplay(ad+po*4-4,7);trk_delete(ad,po*4-4);
				for(i=0;i<po;i++){dat[i]=temp[i];}
			}
		}
	}
	if((sh&1)!=0){fnc_dis(1);}else{fnc_dis(0);}
	msg_clr();
	return(exc);
}

/***************************/
static inline int	chord_ten(int key,int no)
{
	int	po,i,j;

	dat[0]=key;po=1;
	for(i=0;i<=4;i++){
		if(chord_pat[no][i]!=0 ){
			j=key+chord_pat[no][i];
			if(j<128 && j>=0){dat[po]=j;po++;}
		}
	}
	return(po);
}

/***************************/
static inline int	chord_ins(int ad,int co)
{
	int	po,gt,vl;

	if(trk_shift(ad,co*4-4)!=0){return(1);}
	gt=trk[track][ad+2];vl=trk[track][ad+3];po=0;
	while(co>0){
		trk[track][ad]=dat[po];trk[track][ad+2]=gt;trk[track][ad+3]=vl;
		if(co>1){trk[track][ad+1]=0;}
		co--;po++;ad=ad+4;
	}
	return(0);
}

/***************************/
static inline void	chord_dis(int n)
{
	int	i,j;
	char	tmp[2];

	KNJCTRL(1,0);

	tmp[0]=32;tmp[1]=0;if(n!=0){tmp[0]='*';}
	B_PUTMES(3,0,31,1,tmp);

	for(i=0;i<=9;i++){
		j=i*9+2;if(i>4){j+=3;}
		B_PUTMES(11,j,31,7,chord_name[i+n]);
	}
}

/***************************/
int	ctc(int da,char *s,int skey)
{
	int	ct,co,dd=0,i,a;

	if( da>127 ){da=60;if( da>255 ){dd=1;}}
	if( s[0]==0 ){return(-1);}
	jstrupr(s);ct=0;
	i=0;while(s[i]!=0){
		a=s[i++];
		if( a=='H' ){ct=0;break;}
		if( a=='$' ){strcpy(s,&s[1]);ct=0;break;}

		if( a=='C' ){ct=1;}
		if( a=='D' ){ct=3;}
		if( a=='E' ){ct=5;}
		if( a=='F' ){ct=6;}
		if( a=='G' ){ct=8;}
		if( a=='A' ){ct=10;}
		if( a=='B' ){ct=12;}

		if( ct!=0 && s[i]=='B'){i++;ct--;skey=0;}
		if( a=='N' || a=='=' || a=='#' || a=='+' || a=='-' ){skey=0;}
	}

	if( da<0 ){if( ct>0 ){return(1);}else{return(0);}}
	if( ct==0 ){
		da=str_val(s);
	}else{
		ct=ct+key_shi[skey&15][ct-1];

		co=(da/12)-1;
		i=0;while(s[i]!=0){
			a=s[i++];
			if( a=='#' || a=='+' ){ct++;}
			if( a=='-' ){ct--;}
			if( a>='0' && a<='9' ){co=a-'0';}
			if( a=='.' ){co=-1;}
			if( a=='<' && dd==0 ){co++;}
			if( a=='>' && dd==0 ){co--;}
		}
		da=((co+1)*12+ct-1);
	}
	if(da<0||da>127){da=-1;}
	return(da);
}

/***************************/
char	*keystr(int da)
{
	static char	tmp0[8];
	int	a,b;

	a=((da*43))>>9;b=da-(a*12);
	strcpy(tmp0,note[b]);tmp0[2]=oct[a];tmp0[3]=0;return(tmp0);
}

char	*keystr1(int da,int skey)
{
	static char	tmp0[8];
	int	a,b,bb;

	a=((da*43))>>9;b=da-(a*12);
	strcpy(tmp0,note2[b]);

	bb=b+1;if(bb>11){bb=0;}
	if(key_shi[skey&15][bb]<0){strcpy(tmp0,note2[b+12]);}

	tmp0[2]=oct[a];tmp0[3]=0;
	return(tmp0);
}

/***************************/
char	*keystr2(int da,int skey)
{
	static char	tmp0[8];
	int	a,b;

	a=((da*43))>>9;b=da-(a*12);key_shift_sub(skey&15,&a,&b);
	strcpy(tmp0,note[b]);tmp0[2]=oct[a];tmp0[3]=0;return(tmp0);
}

/***************************/
char	*chstr(int ex)
{
	static char	tmp0[8];

	if(ex<1 || ex>32){
		strcpy(tmp0,"OFF");
	}else{
		tmp0[0]=((ex-1)>>4)+'A';tmp0[1]=0;
		strcat(tmp0,fstr(((ex-1)&15)+1,2));

	}
	return(tmp0);
}
/***************************/
int	strch(int och,char *tmp0)
{
	int	ex;

	jstrupr(tmp0);

	if(tmp0[0]=='A'||tmp0[0]=='B'){
		ex=str_val(&tmp0[1]);
		if(ex<1 || ex>16){ex=1;}

		if(tmp0[1]==0 && och>0){ex=((och-1)&15)+1;}

		ex+=(tmp0[0]-'A')*16;
	}else{
		ex=str_val(tmp0);
		if(ex<0 || ex>32){ex=0;}
		if(och>16 && ex<17){ex+=16;}
	}

	return(ex);
}

/***************************/
int	key_shift(int skey,int *f,int *a)
{
static	char	shfl[12]={0,1,0,1,0,0,1,0,1,0,1,0};
	int	no;

	no=skey&15;
	if(key_shi[no][*a]!=0){return(-1);}
	key_shift_sub(no,&*f,&*a);
	return(shfl[*a]);
}

/***************************/
void	key_shift_sub(int no,int *f,int *a)
{
	int	n;

	n=*a-1;if(n<0){n=11;}
	if(key_shi[no][n]>0){*a=n;if(n==11 && *f>0){*f=*f-1;}}
	n=*a+1;if(n>11){n=0;}
	if(key_shi[no][n]<0){*a=n;if(n==0 && *f<10){*f=*f+1;}}
}

/***************************/
char	*scale_str(int da)
{
	int	a;
static	char	tmp0[16];

	da=da&31;strcpy(tmp0,key_name[da]);
	if(da<16){strcat(tmp0,"MAJOR");}else{strcat(tmp0,"minor");}

	a=da&15;
	if(a==0 || a==8){
		strcat(tmp0,"     ");
	}else{
		if(a>7 ){strcat(tmp0,"(ÅÛ )");}else{strcat(tmp0,"(ÅÚ )");}
		tmp0[11]='0'+(a&7);
	}
	return(tmp0);
}

/***************************/
int	scale_no(int j)
{
	int	d,skey;

	skey=bkey;
	while(j>=0){
		d=trk[track][j];
		if( d==0xf5 ){skey=trk[track][j+1];break;}
		if( d>0xfb ){break;}
		j=j-4;
	}
	return(skey);
}

/***************************/
int	scale_sel(int x,int y,int skey)
{
	int	mf,sf,kc,ke;

	skey=skey&31;mf=skey&16;sf=skey&15;

	while(1){
		B_LOCATE(x,y);B_PRINT(scale_str(sf+mf));

		while(B_KEYSNS()==0){}
		ke=B_KEYINP();kc=ke&0xff;ke=ke>>8;

		if(kc==' '){mf=16-mf;}
		if(kc=='+'||kc=='#'){		sf=sf&7;}
		if(kc=='-'||kc=='b'||kc=='B'){	sf=sf|8;}
		if(kc>='0' && kc<='7'){sf=(sf&8)+(kc-'0');}
		if(kc==13 || kc==27){break;}
		if( ke==0x38){if(sf<15){sf++;}}
		if( ke==0x39){if(sf>0){sf--;;}}
	}

	if(kc==13){return(sf+mf);}
	return(-1);
}

/***************************/
char	*prog_name(int ch,int bank,int prog,int h)
{
	int	i,i0,symn,bank_l,bank_m,hh,sf=0;
static char	tmp0[32];
	char	tmp1[32];

	hh=h>>1;h=h&1;
	bank_l=(bank>>8)&0x7f;
	bank_m=bank&0x7f;

	if(bank<0){bank_l=0;bank_m=0;}

	if((cmdflag&0x20)!=0){sf=1;}

	if(ch>32){ch=0;}
	i=tim_asin[ch];
	tmp0[0]=0;
/*
	if(gs_mode[16]!=0 && i==7){i=gs_bank[16*128+bank_l];}
	if(gs_mode[17]!=0 && i==8){i=gs_bank[17*128+bank_l];}
*/
	if((gs_mode[16]&4)!=0 ){
		if((gs_mode[16]&128)!=0 && i==7){i=gs_bank[16*128+bank_l];}
		if((gs_mode[17]&128)!=0 && i==8){i=gs_bank[17*128+bank_l];}
	}else{
		if((gs_mode[16]&128)!=0 && i==7){i=gs_bank[16*128+bank_m];}
		if((gs_mode[17]&128)!=0 && i==8){i=gs_bank[17*128+bank_m];}
	}


	if(i>399||i==0||i==2||prog>127||prog<0||(cmdflag&0x1)!=0 ){
		if(h!=0){strcpy(tmp0,"     ");}
		if(prog<0){strcpy(tmp0,"  OFF");}else{
			strcat(tmp0,fstr(prog+sf,5));
			if(hh){strcat(tmp0,fstr(bank&127,6));}
		}
	}else{
		symn=i;
		if(i==7||i==9||i==11||i==13){symn=gs_bank[i*128+varia(tmp1,bank,prog,i)];}
		if(i==8||i==10||i==12||i==14){symn=gs_bank[i*128+varia(tmp1,prog,0,i+16)];}
		if(h!=0){
			if(symn>0){strcpy(tmp0,tim_sym[symn-1]);}
			if((i>=7 && i<=14) && tmp0[0]==0){strcpy(tmp0,tim_sym[i-1]);}
			strcpy(tmp0,spadd(tmp0,6));
		}else{
			strcpy(tmp0," ");
		}
		if( i>=7 && i<=14){
			if(h!=0){tmp0[5]=0;}else{tmp0[0]=0;if(tmp1[0]==':'){tmp1[0]=32;}}
			if(tmp1[1]==0){strcat(tmp1,tim_names(prog+i*128-128));}
			if(tmp1[1]!=0){strcat(tmp0,tmp1);}
		}else{
			if( i==1 ){
				i0=cm6[0xa34+prog*8]*64+cm6[0xa35+prog*8];
			}else{
				if( i==3 ){
					i0=cm6[0x4eb2+prog*19]*128+cm6[0x4eb3+prog*19]+256;
				}else{i0=prog+i*128-128;}
			}
			strcat(tmp0,tim_names(i0));
		}
		if(tmp0[h*6]==0){strcat(tmp0,fstr(prog+sf,6));
			if(hh){strcat(tmp0,fstr(bank&127,6));}
		}
	}
	strcpy(tmp0,spadd(tmp0,18));return(tmp0);
}

/***************************/
char	*tim_names(int no)
{
	static	char	tmp0[128];
	int	nm,bank=no>>7;no=no&0x7f;

	if(bank<4){strcpy(tmp0,tim_name[bank*128+no]);return tmp0;}

	tmp0[0]=0;nm=tim_top[bank];

	while(tim_name[nm][0]!=0){
		if(tim_name[nm][14]==no){strncpy(tmp0,tim_name[nm],14);break;}
		nm++;
	}
	return tmp0;
}

/***************************/
int	mml_inp(int ad)
{
	int	i,j,a,ct,da,co=0,oct=4,st=48,gt=46,vel=64,/*nn=0,*/ntc=0,nts=0;
	int	skey,skey2=scale_no(ad);
	char	s[40],nt[16];
/*
	int	lpad[18];
	unsigned char lpco[18];
*/
	if( ad>0 ){
		j=ad-4;
		while( trk[track][j]>127 && j>=0){j=j-4;}
		if( j>=0 ){
			st=trk[track][j+1];gt=trk[track][j+2];vel=trk[track][j+3];

			if(st==0){st=48;}
			if(gt==0 && st>2){gt=st-2;}
			if(vel==0){vel=64;}
		}
	}

	/*oct=(da/12)-1;*/

	if( trrhy[track]&0x80 ){skey=0;skey2=0;}

	msg("MML:");sinput(s,0x800+32);
	jstrupr(s);

	if(es==0){
		i=0;
		while(s[i]!=0){
loop1:
			a=s[i++];

/*
			if(a=='[' && nn<16){
				nn++;lpad[nn]=i;lpco[nn]=0;
			}
			if(a==']' && nn>0){
				int a=0;
				while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
				if(a>=1 && a<=256){
					lpco[nn]+=1;
					if(lpco[nn]!=a){i=lpad[nn];goto loop1;}
				}
				nn--;
			}
*/
			if(a=='['){
				dat[co++]=0xf9;dat[co++]=0;
				dat[co++]=0;dat[co++]=0;
			}
			if(a==']'){
				int a=0;
				while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
				if(a>=1 && a<=256){
					dat[co++]=0xf8;dat[co++]=a;
					dat[co++]=0;dat[co++]=0;
				}
			}

			if(a==0x27 || a==0x60){
				if(nts==0){
					nts=1;ntc=0;/*goto loop1;*/
				}else{
					int a=0,ss=st,gg=gt;
					while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
					if(a>=1 && a<256){ss=a;}
					if(s[i]==','){i++;
						a=0;
						while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
						if(a>=1 && a<256){gg=a;}
					}

					nts=0;
					if(ntc){
						int i;
						for(i=0;i<ntc;i++){
							dat[co++]=nt[i];dat[co++]=0;
							dat[co++]=gg;dat[co++]=vel;
						}
						dat[co-3]=ss;/*goto loop1;*/
					}
				}
			}


			if( a=='O' ){
				if(s[i]>='0' && s[i]<='9'){oct=s[i++]-'0';}
				if(s[i]=='.' ){oct=-1;i++;}
			}
			if( a=='L' ){
				if(s[i]>='0' && s[i]<='9'){
					int a=0;
					while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
					if(a>=1 && a<=64){st=192/a;gt=st-2;}
				}
			}
			if( a=='V' ){
				if(s[i]>='0' && s[i]<='9'){
					int a=0;
					while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
					if(a>=0 && a<=127){vel=a;}
				}
			}

			if( a=='<' && oct<9){oct++;}
			if( a=='>' && oct>0){oct--;}

			if((a>='A' && a<='G') ||a=='R'){
				int ss=st,gg=gt;

				skey=skey2;
				ct=-1;
				if( a=='C' ){ct=1;}
				if( a=='D' ){ct=3;}
				if( a=='E' ){ct=5;}
				if( a=='F' ){ct=6;}
				if( a=='G' ){ct=8;}
				if( a=='A' ){ct=10;}
				if( a=='B' ){ct=12;}

				if( s[i]=='N' || s[i]=='=' || s[i]=='#' || s[i]=='+' || s[i]=='-' ){
					if( *s=='#' || *s=='+' ){ct++;}
					if( *s=='-' ){ct--;}

					i++;
					skey=0;
				}

				if(s[i]>='0' && s[i]<='9'){
					int a=0;
					while(s[i]>='0' && s[i]<='9'){a=a*10+s[i++]-'0';}
					if(a>=1 && a<=64){ss=192/a;gg=ss-2;}
				}

				if(ct>=0){
					ct=ct+key_shi[skey&15][ct-1];
					da=((oct+1)*12+ct-1);

					if( da>=0 && da<=127 ){
						if(nts!=0 && ntc<16){
							nt[ntc++]=da;
						}else{
							dat[co++]=da;dat[co++]=ss;
							dat[co++]=gg;dat[co++]=vel;
						}
					}
				}else{
					if(nts==0){
						dat[co++]=0;dat[co++]=ss;
						dat[co++]=0;dat[co++]=0;
					}
				}
			}
		}

		if(co){
			if(trk_shift(ad,co)!=0){snsclr();return(0);}
			memcpy_l(&trk[track][ad],dat,co);
		}
	}

	msg_clr();
	return(co);
}

/***************************/
