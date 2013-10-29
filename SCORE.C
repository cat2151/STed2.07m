/*
 * STed: score.c  (fumen & data draw subroutine)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"graph.h"
#include	"math.h"
#include	"sted.h"

/*
ëÏ
ëÐ	ëÓ
ëÑ	ëÔ
*/

char	o_zero[12][3]  ={"ë³","ë´","ëµ","ë¶","ë·","ë¸","ë¹","ëº","ë»","ë¼","ë½","ë¾"};
char	o_flag_u[12][3]={"  ","ë¤","ë¤","ë¤","ë¤","ë¥","ë¥","ë¦","ë¦","ë§","ë§","ë¨"};
char	o_head_u[12][3]={"ëŸ","ë ","ë¡","ë¢","ë£","ë¢","ë£","ë¢","ë£","ë¢","ë£","ë£"};
char	o_head_d[12][3]={"ë©","ëª","ë«","ë¬","ë­","ë¬","ë­","ë¬","ë­","ë¬","ë­","ë­"};
char	o_flag_d[12][3]={"  ","ë®","ë®","ë®","ë®","ë¯","ë¯","ë°","ë°","ë±","ë±","ë²"};

char	onp_ps[12]={0,0,1,1,2,3,3,4,4,5,5,6};
char	sharp_yp[14]={0*4,3*4,-1*4,2*4,5*4,1*4,4*4 , 4*4,1*4,5*4,2*4,6*4,3*4,7*4};

int	ga_adr[512],ga_xpo[512],ga_co;
int	gr_adr[512],gr_xpo[512],gr_co;

int	step_cluc2();
int	add_set();
int	meas_add();
int	meas_no();
int	meas_no2();
void	all_note_off();
void	trk_cluc();
int	trk_shift();
void	trk_delete();
void	poplay();
void	undobuf();
void	msg();
void	msg_clr();
int	key_shift();
char	*ctrl_type();
char	*prog_name();

void	g_print();

int	vinput();
void	fnc_dis();
char	*fstr();
char	*chstr();
void	sdis();
void	sdis2();
void	vdis2();
void	snsclr();

void	gra();
int	gra_add();
void	gra_gakufu();
void	gra_special();
int	st_cv();
void	put_sharp();

/***************************/
void	gra(int po,int sh)
{
if(grpmode<4){
	if(grpmode&1){gra_gakufu(po,sh);}
	if(grpmode&2){gra_special(po,sh);}
}
	if(grpmode==4){exc_view(po);}
}

/***************************/
void	gra_c(int po)
{
	int	i,xx;

if(grpmode<4){
	if(grpmode&1){
		for(i=0;i<ga_co-1;i++){
			if(ga_adr[i+1]>po){
				xx=(ga_xpo[i]+ga_xpo[i+1])>>1;
				txyline(xx,592-512,850-592,1,65535);
				break;
			}
		}
	}
	if(grpmode&2){
		int yy=872-512;if((grpmode&1)==0){yy=yy-128;}
		for(i=0;i<gr_co-1;i++){
			if(gr_adr[i+1]>po){
				xx=gr_xpo[i];txyline(xx,yy,999-872,1,65535);
				break;
			}
		}
	}
}
}

/***************************/
void	gra_gakufu(int po,int md)
{
	int	a,b,c,d,i,ad,ad2,gx,cc=0,skey,co=0;
	int	tb=(tbase/48);
	char	tmp0[128];
	unsigned char	kno[512];
	unsigned char *ptr=trk[track];

	if((md&16)==0){
		fill(328-8,590,766,851,0);box(327,589,767,852,14,65535);
	}else{
		fill(392-44,590,766,851,0);
	}
	for(i= 0;i<= 4;i++){
		line(327+48,680- 56+i*8,767,680- 56+i*8,14,0xcccc);
		line(327   ,680    +i*8,767,680    +i*8,14,0xffff);
		line(327   ,680+ 48+i*8,767,680+ 48+i*8,14,0xffff);
		line(327+48,680+104+i*8,767,680+104+i*8,14,0xcccc);
	}
	g_print(330,675,"ëÏ",15);g_print(330,691,"ëÐ",15);g_print(330,707,"ëÑ",15);
	g_print(330,728,"ëÓ",15);g_print(330,744,"ëÔ",15);

	gx=392;ad=gra_add(po,md);

	skey=bkey;
	ad2=ad;
	while(ad2>=0){
		if(ptr[ad2]>0xfc ){break;}
		if(ptr[ad2]==0xf5 ){skey=ptr[ad2+1];}
		ad2-=4;
	}

	if(trrhy[track]&0x80){skey=0;}
	put_sharp(skey,328+20,670);put_sharp(skey,328+20,670+14*4);

	while(gx<760 && ad<tr_len[track]-4){
ggloop:		a=ptr[ad];if(a==0xf7){ad+=4;goto ggloop;}
		b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];
		if(tb>1){b=b/tb;}

		ad2=ad;
		if(b){ga_adr[co]=ad;ga_xpo[co++]=gx;}

		if(a>0xf4 ){
			gx=gx+3;
			if(a==0xf9 ){
				line(gx,680,gx  ,760,11,65535);
				line(gx,680,gx+3,680,11,65535);
				line(gx,760,gx+3,760,11,65535);
			}
			if(a==0xf8 ){
				line(gx  ,680,gx,760,11,65535);
				line(gx-3,680,gx,680,11,65535);
				line(gx-3,760,gx,760,11,65535);
			}
			if(a>=0xfc ){
				line(gx,680,gx,760,14,65535);skey=bkey;
				if(trrhy[track]&0x80){skey=0;}
			}
			gx=gx-3;
			if(a==0xf5 && gx!=392){
				skey=b;
				if(trrhy[track]&0x80){skey=0;}
				put_sharp(skey,gx,670);put_sharp(skey,gx,670+14*4);
				b=44;
			}else{b=2;}
			goto next;
		}

		if(a<128){
			if(c==0 || d==0){
				kno[cc]=255;cc++;
			}else{
				kno[cc]=a;cc++;
			}
		}

		if(b>0){
			int	xx,bb=0,e,f,i,ss;
			i=ad;while(i<tr_len[track]-4){
loop:				i+=4;e=ptr[i];f=ptr[i+1];

				if(e==0xf6 || e==0xf7){goto loop;}
				if(e<0x80 || e>=0xf0){
					if(e<0x80 && (ptr[i+2]==0||ptr[i+3]==0)){
						if(cc==0 || (cc==1 && kno[0]==255)){
							bb+=f;goto loop;
						}
					}
					ad=i-4;break;
				}
				bb+=f;
			}
			if(tb>1){bb=bb/tb;}
			b+=bb;

			xx=gx-6+(b>>1);if(gx==392 && b>40){xx=xx-(b>>1)+6;}
			ss=st_cv(b);

			if(cc==0 || (cc==1 && kno[0]==255)){

				strcpy(tmp0,o_zero[ss]);
				g_print(xx,688,tmp0,15);g_print(xx,688+6*8,tmp0,15);
			}else{
				int	ff,y,f,i,a,sff;

				ff=9999;strcpy(tmp0,o_head_u[ss]);
				for(i=0;i<cc;i++){
					a=kno[i];
					if(a>9 && a<108){
						f=((a*43))>>9;a=a-(f*12);
						sff=key_shift(skey,&f,&a);
						f=onp_ps[a]+f*7;y=(819+7*4)-(f*4);

						if(y==651||y==659||y==707||y==755||y==763){
							g_print(xx-2,y,"ëÎ",14);}

						g_print(xx,y,tmp0,15);
						if(sff>0){g_print(xx-10,y+4,"ëÍ",15);}
						if(sff<0){g_print(xx-10,y+4,"ëË",15);}
						if(y<ff){ff=y;}
					}
				}
				if(ff<9999){g_print(xx,ff-6,o_flag_u[ss],15);}
			}
			cc=0;
		}
next:
		if(po<=ad && (md&32)==0 && (b>0 || a>0xf0)){
			int	xx,bb=b;

			xx=gx;
			if(a<0xf0){xx+=(bb>>1)-1;if(gx==392 && bb>40){xx=xx-(bb>>1)+5;}}
			else{xx+=2;}
			line(xx,592,xx,850,8,65535);
			po=tr_len[track]+4;
		}

		if(gx==392 && b>40){b=b-(b>>1)+6;}
		gx=gx+b;ad=ad+4;
	}
	ga_co=co;
}

/***************************/
void	gra_special(int po,int md)
{
	int	a,b,c,d,bb,i,ad,gx,co=0,retad=0;
	int	pitch,opitch,velo,ovel,vol,ovol,modu,omodu,expr,oexpr,entry,oentry;
	int	pan,opan;
	int	tb=(tbase/48);
	int	me_ad[2048];
	unsigned char *ptr=trk[track];

	int ys=1;
	if((grpmode&1)==0){ys=2;}

	add_set(track,me_ad);
	gx=392;ad=gra_add(po,md);

	if((md&16)==0){
		fill(328-8,857+15+128-128*ys,766,1000,0);
		box(327,856+128-128*ys,767,1001,14,65535);
		sdis(41,856+128-128*ys," CONTOROLER GRAPHIC DISPLAY",55,15,14);
	}else{
		fill(392,857+15+128-128*ys,766,1000,0);
	}

	line(327,936+64-ys*64,767,936+64-64*ys,14,0xaaaa);

	opitch=-1;ovol=-1;oexpr=-1;omodu=-1;oentry=-1;opan=-1;

	i=ad;while(i>0){
		unsigned char	d;
top:
		i=i-4;d=ptr[i];
		if(d==0xee && opitch<0){
			opitch=(ptr[i+3]*128+ptr[i+2])*ys;
		}
		if(d==0xeb){
			unsigned char	b=ptr[i+2];
			if(b== 7 && ovol <0){ovol=ptr[i+3]*ys;}
			if(b== 1 && omodu<0){omodu=ptr[i+3]*ys;}
			if(b==11 && oexpr<0){oexpr=ptr[i+3]*ys;}
			if(b== 6 && oentry<0){oentry=ptr[i+3]*ys;}
			if(b==10 && opan<0){opan=ptr[i+3]*ys;}
		}

		if( d>=0xfc && retad!=0){i=retad;retad=0;goto top;}

		if( d==0xfc ){
			int	ii,jj,ct;
			jj=i;
resame:
			ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct+1]-4;

			if(jj!=ii && ii>=0){
				jj=ii;if(ptr[ii]==0xfc){goto resame;}
				retad=i;i=ii;goto top;
			}
		}

	}
	if(opitch<0){opitch =8192*ys;}
	if(ovol <0){ovol =100*ys;}
	if(oexpr<0){oexpr =127*ys;}
	if(omodu<0){omodu=  0;}
	if(oentry<0){oentry=  0;}
	if(opan<0){opan= 64*ys;}

	opitch=(opitch)>>7;

	ovel=0;velo=ovel;pitch=opitch;vol=ovol;expr=oexpr;modu=omodu;entry=oentry;
	pan=opan;

	while(gx<767 && ad<tr_len[track]-4){
gsloop:		a=ptr[ad];if(a==0xf7){ad+=4;goto gsloop;}
		b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];
		if(tb>1){b=b/tb;}
		bb=b;

		if(gx==392 && b>40){b=(b>>1)+6;}

		if(b){gr_adr[co]=ad;gr_xpo[co++]=gx;}

		if(po==ad && (md&32)==0){line(gx,872+128-128*ys,gx,999,8,65535);}

		if(a==0xee){pitch=((d*128+c)*ys)>>7;}
		if(a==0xeb){
			if( c==7){vol=d*ys;}
			if( c==1){modu=d*ys;}
			if( c==11){expr=d*ys;}
			if( c==6){entry=d*ys;}
			if( c==10){pan=d*ys;}
		}
		if(a>=0xf0 ){
			b=0;
			if(a>=0xfc ){line(gx+3,872+128-128*ys,gx+3,999,14,65535);b=2;}
			if(a==0xf5 ){b=64;}
		}

		if(b>0){
			line(gx,488+512-opitch,gx  ,488+512-pitch,13,65535);
			line(gx,488+512-ovol  ,gx  ,488+512-vol  ,12,65535);
			line(gx,488+512-oexpr ,gx  ,488+512-expr , 9,65535);
			line(gx,488+512-omodu ,gx  ,488+512-modu ,10,65535);
			line(gx,488+512-oentry,gx  ,488+512-entry,11,0xaaaa);
			line(gx,488+512-opan  ,gx  ,488+512-pan  ,10,0xaaaa);
/*‰¡ü*/
			if(b>1){
				line(gx,488+512-pitch ,gx+b-1,488+512-pitch,13,65535);
				line(gx,488+512-vol   ,gx+b-1,488+512-vol  ,12,65535);
				line(gx,488+512-expr  ,gx+b-1,488+512-expr , 9,65535);
				line(gx,488+512-modu  ,gx+b-1,488+512-modu ,10,65535);
				line(gx,488+512-entry ,gx+b-1,488+512-entry,11,0xaaaa);
				line(gx,488+512-pan   ,gx+b-1,488+512-pan  ,10,0xaaaa);
			}
			opitch=pitch;ovol=vol;oexpr=expr;omodu=modu;oentry=entry;
			opan=pan;
		}

		if(a<128){
			int	i;
			if(c==0 || d==0){
				ovel=0;
			}else{
				int f=0,b2=b;

				velo=d;
				if(b>0 && c>bb){f=1;}
				if(b==0 && c>0){
					i=ad;while(i<tr_len[track]-4){
						if(ptr[i+1]>0 && ptr[i]<0xf7){
							b2=ptr[i+1];bb=b2;
							if(gx==392 && b2>40){b2=(b2>>1)+6;}
							if(c>ptr[i+1]){f=1;}
							break;
						}
						i=i+4;
					}
				}
				if(b2==bb){
					line(gx,488+512-ovel*ys,gx,488+512-velo*ys,15,65535);
				}
				if(c>b2){c-=(bb-b2);}
				line(gx,488+512-velo*ys,gx+c,488+512-velo*ys,15,65535);
				if(f==0){
					line(gx+c,488+512,gx+c,488+512-velo*ys,15,65535);
					if(b2>0){ovel=0;}
				}else{
					ovel=velo;
				}
			}
		}
		gx=gx+b;ad=ad+4;
	}
	gr_co=co;
}

/***************************/
int	gra_add(int ad,int md)
{
	int	a=0,i,tr,of,tb;
	tr=track;
	i=ad;

	tb=(tbase/48);

	if((md&32)==0){of=96+64;}else{of=24;}
	of=of*tb;

	while(i>0){
		i=i-4;
		if( trk[tr][i]<0xf5 ){
			a=a+trk[tr][i+1];
			if(a>=of){
				while(i>0 && trk[tr][i-4]<0xf5 && trk[tr][i-3]==0){i=i-4;}
				break;
			}
		}
	}
	return(i);
}

/***************************/
int	st_cv(int st)
{
	st=st/(tbase/48);

	if(st>=192-24 ){return  0;}	/*192*/
	if(st>=144-24 ){return  1;}	/*144*/
	if(st>= 96-12 ){return  2;}	/* 96*/
	if(st>= 72-12 ){return  3;}	/* 72*/
	if(st>= 48- 6 ){return  4;}	/* 48*/
	if(st>= 36- 6 ){return  5;}	/* 36*/
	if(st>= 24- 3 ){return  6;}	/* 24*/
	if(st>= 18- 3 ){return  7;}	/* 18*/
	if(st>= 12- 1 ){return  8;}	/* 12*/
	if(st>=  9- 1 ){return  9;}	/*  9*/
	if(st>=  6- 1 ){return 10;}	/*  6*/
			return 11;	/*  3*/
}

/***************************/
void	put_sharp(int skey,int x,int y)
{
	int	a,i,bs;
	char	*tmp0;

	a=skey&7;
	if(a!=0){
		if((skey&15)<8){bs=0;tmp0="ëÍ";}else{bs=7;tmp0="ëÌ";}
		for(i=0;i<a;i++){g_print(x,y+sharp_yp[i+bs],tmp0,15);x+=4;}
	}
}

/***************************/
#if NORMAL
void	dat_info(int po,int sh)
{
	int	i,ln,top,st,pitch=0,ch=mch[track],prg=0,tempo=64,retad=0;
	char	tmp[256];
	unsigned char	ctr[256];
static unsigned char	ctno1[16]={7,11,1,10,64,91,93,94, 5,65,66,67,99,101, 6, 0};
static unsigned char	ctno2[16]={0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,98,100,38,32};

static unsigned char	ctno3[16]={7,11,1,10,64,91,93,94,71,72,73,74,99,101, 6, 0};

	int	me_ad[2048];
	unsigned char *ptr=trk[track];

if(sh&10){dat_info2(po,sh);return;}

	add_set(track,me_ad);

	top=0;ln=po;
	if( cplen<0 && cpadd!=po){
		ln=abs(cpadd-po);if(cpadd>po ){top=po;}else{top=cpadd;}
	}
	st=step_cluc2(track,top,ln);

	fill(316,564,453,1001,0);
	strcpy(tmp,"P.INFO.  /TR.:");
	strcat(tmp,fstr(track+1,2));

	tmp[7]=(sh&1)+'1';

	sdis(40,571,tmp,16,15,14);
	box(319,604,448,997,14,65535);box(320,605,447,996,14,65535);

	strcpy(tmp," MEAS:");strcat(tmp,fstr(meas_no(po),4));
/*	strcat(tmp,"(");strcat(tmp,fstr(meas_no2(po),5));strcat(tmp,")");*/
	strcat(tmp,"/");strcat(tmp,fstr(meas_no(tr_len[track]-4),4));strcat(tmp," ");

	sdis2(40,588,tmp,16,15,0);

	for(i=0;i<127;i++){ctr[i]=0;}
	ctr[7]=100;ctr[11]=127;ctr[10]=64;ctr[91]=40;
	ctr[71]=64;ctr[72]=64;ctr[73]=64;ctr[74]=64;
	ctr[128]=64;ctr[129]=64;ctr[130]=64;ctr[131]=64;

	i=0;while(i<=po){
		unsigned char	d;
top:
		d=ptr[i];
		if(d==0xeb){ctr[ptr[i+2]]=ptr[i+3];
			if(ptr[i+2]==6 && ctr[99]==1){
				if(ctr[98]== 33){ctr[71]=ptr[i+3];}	/*Resonanc*/
				if(ctr[98]==102){ctr[72]=ptr[i+3];}	/*Rel.Time*/
				if(ctr[98]== 99){ctr[73]=ptr[i+3];}	/*Att.Time*/
				if(ctr[98]== 32){ctr[74]=ptr[i+3];}	/*Cutoff F*/

				if(ctr[98]==100){ctr[128]=ptr[i+3];}	/*Decay Time*/
				if(ctr[98]==  8){ctr[129]=ptr[i+3];}	/*Vibrato Rate*/
				if(ctr[98]==  9){ctr[130]=ptr[i+3];}	/*Vibrato Depth*/
				if(ctr[98]== 10){ctr[131]=ptr[i+3];}	/*Vibrato Delay*/
			}
		}
		if(d==0xee){pitch=((ptr[i+3]*128+ptr[i+2])-8192);}
		if(d==0xec){prg=ptr[i+2];}
		if(d==0xe2){prg=ptr[i+2];ctr[0]=ptr[i+3];}
		if(d==0xe7){tempo=ptr[i+2];}
		if(d==0xe6){ch=ptr[i+2];}

		if( d>=0xfc && retad!=0){i=retad;retad=0;goto top;}

		if( d==0xfc ){
			int	ii,jj,ct;
			jj=i;
resame:
			ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct];

			if(jj!=ii){
				jj=ii;if(ptr[ii]==0xfc){goto resame;}
				retad=i+4;i=ii;goto top;
			}
		}
		i=i+4;
	}

	for(i=0;i<15;i++){
		if((sh&1)==0){
			strcpy(tmp,ctrl_type(ctno1[i]));strcat(tmp," : ");
			if(i>11){tmp[5]=':';tmp[6]=0;}
			strcat(tmp,fstr(ctr[ctno1[i]],3));
			if(i>11){strcat(tmp,": ");strcat(tmp,fstr(ctr[ctno2[i]],3));}
		}else{
			strcpy(tmp,ctrl_type(ctno3[i]));strcat(tmp," : ");
			strcat(tmp,fstr(ctr[ctno3[i]],3));
			if(i>11){tmp[0]=0;}
		}
		sdis2(41,608+i*16,tmp,13,15,-1);
	}

	if((sh&1)!=0){
		sdis2(41,608+12*16,"DECAY    :",9,15,-1);vdis2(51,608+12*16,ctr[128],4,15,-1);
		sdis2(41,608+13*16,"VIB RATE :",9,15,-1);vdis2(51,608+13*16,ctr[129],4,15,-1);
		sdis2(41,608+14*16,"VIB DEPTH:",9,15,-1);vdis2(51,608+14*16,ctr[130],4,15,-1);
		sdis2(41,608+15*16,"VIB DELAY:",9,15,-1);vdis2(51,608+15*16,ctr[131],4,15,-1);
	}else{
		sdis2(41,880-32,"PITCH   :",9,15,-1);vdis2(50,880-32,pitch,5,15,-1);
	}
	sdis2(41,896-32,"TEMPO:   (    )",16,15,-1);vdis2(47,896-32,tempo,3,15,-10);
	vdis2(51,896-32,(tempo*mtempo+33)>>6,4,15,-1);

	sdis2(41,912-32,"MIDI CH.:",9,15,-1);
	strcpy(tmp,chstr(ch));sdis2(52,912-32,tmp,3,15,-1);

	sdis2(41,896,"BANK MSB:",16,15,-1);vdis2(52,896,ctr[0],3,15,-10);
	sdis2(41,912,"BANK LSB:",16,15,-1);vdis2(52,912,ctr[32],3,15,-10);

	if(ch!=0){
		sdis2(41,928,"PROGRAM :",9,15,-1);
		sdis2(43,944,prog_name(ch,ctr[0]+ctr[32]*256,prg,0),10,15,-1);
		if((cmdflag&0x20)!=0){prg++;}
		vdis2(51,928,prg,4,15,-1);
	}
	sdis(41,962,"= TOTAL ST. =",5,15,-1);vdis2(45,978,st,10,15,-1);
}
#endif
/***************************/

/***************************/
void	dat_info(int po,int sh)
{
	int	i,ln,top,st,pitch=0,ch=mch[track],prg=0,tempo=64,retad=0;
	char	tmp[256];
	unsigned char	ctr[256];
static unsigned char	ctno1[24]={7,11,1,10,64,91,93,94, 5,65,66,67,71,72,73,74,128,129,130,131,99,101, 6, 0};
static unsigned char	ctno2[24]={0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,98,100,38,32};

static char	vib_name[4][10]={"DECAY    ","VIB RATE ","VIB DEPTH","VIB DELAY"};


	int	me_ad[2048];
	unsigned char *ptr=trk[track];

	add_set(track,me_ad);

	top=0;ln=po;
	if( cplen<0 && cpadd!=po){
		ln=abs(cpadd-po);if(cpadd>po ){top=po;}else{top=cpadd;}
	}
	st=step_cluc2(track,top,ln);

	fill(316,564,453,1001,0);
	strcpy(tmp,"P.INFO.  /TR.:");
	strcat(tmp,fstr(track+1,2));

	sdis(40,571,tmp,16,15,14);
	box(319,604,448,997,14,65535);box(320,605,447,996,14,65535);

	strcpy(tmp,"MEAS:");strcat(tmp,fstr(meas_no(po),4));
	strcat(tmp,"/");strcat(tmp,fstr(meas_no(tr_len[track]-4),4));

	sdis2(41,588,tmp,16,15,0);

	for(i=0;i<127;i++){ctr[i]=0;}
	ctr[7]=100;ctr[11]=127;ctr[10]=64;ctr[91]=40;
	ctr[71]=64;ctr[72]=64;ctr[73]=64;ctr[74]=64;
	ctr[128]=64;ctr[129]=64;ctr[130]=64;ctr[131]=64;

	i=0;while(i<=po){
		unsigned char	d;
top:
		d=ptr[i];
		if(d==0xeb){ctr[ptr[i+2]]=ptr[i+3];
			if(ptr[i+2]==6 && ctr[99]==1){
				if(ctr[98]== 33){ctr[71]=ptr[i+3];}	/*Resonanc*/
				if(ctr[98]==102){ctr[72]=ptr[i+3];}	/*Rel.Time*/
				if(ctr[98]== 99){ctr[73]=ptr[i+3];}	/*Att.Time*/
				if(ctr[98]== 32){ctr[74]=ptr[i+3];}	/*Cutoff F*/

				if(ctr[98]==100){ctr[128]=ptr[i+3];}	/*Decay Time*/
				if(ctr[98]==  8){ctr[129]=ptr[i+3];}	/*Vibrato Rate*/
				if(ctr[98]==  9){ctr[130]=ptr[i+3];}	/*Vibrato Depth*/
				if(ctr[98]== 10){ctr[131]=ptr[i+3];}	/*Vibrato Delay*/
			}
		}
		if(d==0xee){pitch=((ptr[i+3]*128+ptr[i+2])-8192);}
		if(d==0xec){prg=ptr[i+2];}
		if(d==0xe2){prg=ptr[i+2];ctr[0]=ptr[i+3];}
		if(d==0xe7){tempo=ptr[i+2];}
		if(d==0xe6){ch=ptr[i+2];}

		if( d>=0xfc && retad!=0){i=retad;retad=0;goto top;}

		if( d==0xfc ){
			int	ii,jj,ct;
			jj=i;
resame:
			ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct];

			if(jj!=ii){
				jj=ii;if(ptr[ii]==0xfc){goto resame;}
				retad=i+4;i=ii;goto top;
			}
		}
		i=i+4;
	}

	for(i=0;i<23;i++){
		int nn=ctno1[i];
		if(nn<128){strcpy(tmp,ctrl_type(nn));strcat(tmp," : ");
		}else{
			strcpy(tmp,vib_name[nn-128]);strcat(tmp,": ");
		}
		if(i>19){tmp[5]=':';tmp[6]=0;}
		strcat(tmp,fstr(ctr[ctno1[i]],3));
		if(i>19){strcat(tmp,": ");strcat(tmp,fstr(ctr[ctno2[i]],3));}

		sdis3(41,608+i*10,tmp,13,15,-1);
	}

	sdis3(41,608+10*23,"PITCH   :",9,15,-1);vdis3(50,608+10*23,pitch,5,15,-1);

	sdis2(41,896-32,"TEMPO:   (    )",16,15,-1);vdis2(47,896-32,tempo,3,15,-10);
	vdis2(51,896-32,(tempo*mtempo+33)>>6,4,15,-1);

	sdis2(41,912-32,"MIDI CH.:",9,15,-1);
	strcpy(tmp,chstr(ch));sdis2(52,912-32,tmp,3,15,-1);
/*
	sdis3(41,608+10*25,"TEMPO:   (    )",16,15,-1);vdis3(47,608+10*25,tempo,3,15,-10);
	vdis3(51,608+10*25,(tempo*mtempo+33)>>6,4,15,-1);

	sdis3(41,608+10*26,"MIDI CH.:",9,15,-1);
	strcpy(tmp,chstr(ch));sdis3(52,608+10*26,tmp,3,15,-1);
*/

	sdis2(41,896,"BANK MSB:",16,15,-1);vdis2(52,896,ctr[0],3,15,-10);
	sdis2(41,912,"BANK LSB:",16,15,-1);vdis2(52,912,ctr[32],3,15,-10);

	if(ch!=0){
		sdis2(41,928,"PROGRAM :",9,15,-1);
		sdis2(43,944,prog_name(ch,ctr[0]+ctr[32]*256,prg,0),10,15,-1);
		if((cmdflag&0x20)!=0){prg++;}
		vdis2(51,928,prg,4,15,-1);
	}
	sdis(41,962,"= TOTAL ST. =",5,15,-1);vdis2(45,978,st,10,15,-1);
/*	sdis3(41,962+6,"= TOTAL ST. =",5,15,-1);vdis2(45,978,st,10,15,-1);*/
}

/***************************/

