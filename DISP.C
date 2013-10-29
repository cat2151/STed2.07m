/*
 * STed: disp.c (input and output subroutine)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

unsigned char	fnc_dat[9*10][9]={
	" æ“ªs "," ÅIs ","  ’uŠ·  ","  ŒŸõ  "," ŽŸŒŸõ\x1f",	/*edit*/
	"  ”ÍˆÍ  ","  íœ  ","  •¡ŽÊ  ","  “\•t  "," “ñd‰» ",

	"NOTE‘€ì"," Å“K‰» ","Žw’èíœ","Œã•ûŒŸõ"," ŽŸŒŸõ\x1e",	/*edit shift*/
	"UNDO“o˜^","UNDOŽÀs","MIX “\•t","”½“]“\•t","        ",

	"   ë¿   ","   ëÀ   ","   ëÁ   ","   ëÂ   ","   ëÃ   ",	/*one touch st/gt*/
	"   ëÄ   ","   ëÅ   ","   ëÆ   ","   ëÇ   ","   ëÉ   ",

	" SYSTEM ","LA PATCH","PCM PAT.","RHY.PAT."," TIMBRE ",	/*cm64 con*/
	"LOAD CM6","SAVE CM6","  PLAY  ","CM6 INIT","  EXIT  ",

	"SEND GT ","SEND VEL","SEND CH.","CLR SUM ","SEND SUM",	/*trk.exclu. edit*/
	"END EXCL","        ","CHEK.SUM","EXC LOAD","EXC SAVE",

	"TOP POSI","LAST POS","  LINE  ","        "," WAVE-A ",	/*visual edit*/
	" WAVE-B ","SIN +CRV","SIN WAVE","TRIANGLE"," SQUARE ",

	"ã‰º”½“]","¶‰E”½“]"," RANDOM ","        ","R.WAVE-A",	/*visual edit shift*/
	"R.WAVE-B","SIN -CRV","SIN WA/C","TRIANG/C","SQUARE/C",

	"SEND GT ","SEND VEL","SEND CH.","CLR SUM ","SEND SUM",	/*user exclu. edit*/
	"END EXCL","        ","CHEK.SUM","REX LOAD","REX SAVE",

	"        ","  ppp   ","   pp   ","   p    ","   mp   ",/*one touch velo*/
	"   mf   ","   f    ","   ff   ","  fff   ","        "};

/*
	"ã‰º”½“]","¶‰E”½“]"," ¶‰ñ“] "," ‰E‰ñ“] ","        ",	*dot edit*
	"        ","        ","        ","        ","        ",


	"  COPY  ","EXCHANGE"," INSERT ","MEAS END","OPTIMIZE",
	"COMP SAM","COMP REP","EXTRACT ","TRK.LOAD","TRK.SAVE",

	" ERASE  ","  MIX   ","  SORT  ","ALL M.E.","ALL OPT.",
	"ALL C.S.","ALL C.R.","ALL EXT.","RCP LOAD","RCP SAVE"};


	"EDIT&SET","  PLAY  "," RECORD ","  LOAD  ","  SAVE  ",
	" TITLE  ","RHY ASSI","CM64 CON"," OPTION ","  EXIT  ",

	"TIME&KEY"," PANEL  ","MIDI MON","BTM.LOAD","RCP SAVE",
	"  MEMO  ","USER EXC","GS/GM C.","UNIT.SEL","  INIT  "};

	" FILTER ","PART ASS","‹­§EXIT"


	"  Meas  "," Tempo  "," Beat   ","  Thru  ","  Sync  ",
	"  Mode  "," Metoro "," Filter "," AC.CH. "," Start  "};
*/

char	hex_code[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

int	meas_add();
int	meas_no();
void	all_note_off();
void	trk_cluc();
int	trk_shift();
void	trk_delete();
void	trksize();
void	trk_no_dis();
void	poplay();
void	undobuf();

void	fnc_dis();
int	vinput();
void	sinput();
void	spcut();
int	inkey2();
int	knck();
void	home2();
char	*fstr();
char	*hex_s();
unsigned char	*jstrupr();
char	*chstr();
int	str_search();
int	str_last();
int	str_val();
void	g_print();
void	g_print2();
void	vdis();
void	vdis2();
void	vdis3();
void	sdis();
void	sdis2();
void	sdis3();
void	tdis();
void	msg();
void	msg_clr();
void	trk_free();
void	snsclr();
void	trk_no();

/***************************/
int	vinput(int l,int ln)
{
	char	sb[256];
	char	*pp=(char *)sb;

	if(ln & 0x2000){strcpy(sb,hex_s(l,ln&0xff));}else{b_striS(sb,l);}

	while(*pp=='0'){pp++;}
	strcpy(sb,pp);

	if( l==0 ){sb[0]=0;}

re_vinp:
	sinput(sb,0x1400|ln);if(ecode==14){goto re_vinp;}
	if( sb[0]==0 ){return(0);}
	if(ln & 0x2000){strcat(sb,"H");}
	return(str_val(sb));
}

/***************************/
/*
  im  exc  len
 0000 0000 0000 0000
 |||| ||||
 |||| |||+-return/space
 |||| ||+--not return 
 |||| |+---cursor last p.
 |||| +----over write
 | |+------chr.type(0=all/1=hex)
 | +-------hex mode(vinput only)
 +---------space nocut
-----------point move
*/
void	sinput(char *sb,int ln)
{
	int	a,x,y,xx,p=0,o=0,exc,im,sln;
	char	ke[4],st[256],tmp0[256];

	x=B_LOCATE(-1,0);y=x&0xffff;x=x>>16;
	exc=ln>>8;ln=ln&0xff;im=(exc>>4)&3;
	if((exc&0x80)==0){spcut(sb);}

	if( (exc & 8)!=0 ){st[0]=0;o=1;}else{strcpy(st,sb);}
	if( (exc & 4)!=0 ){p=strlen(st);}

	if((exc&0x100)!=0){
		int c=str_search(sb,"$<");
		if(c){p=c-1;strcpy(&sb[c-1],&sb[c+1]);}
	}


	xx=x*8;if(edit_scr==1){xx=xx+56*8;}
	/*c=point(xx,y*16+scyp);*/
	fill(xx-1,y*16+scyp,xx+(ln)*8+1,y*16+15+scyp,7);
	tdis(x,y,sb,ln,3);

	while(1){
		B_LOCATE(x+p,y);C_CURON();while(INPOUT(0xfe)==0){}
		a=INPOUT(0xff);ecode=a;ke[0]=a;ke[1]=0;
		sln=strlen(st);st[sln+1]=0;

		if( (127<a && 161>a) || 224<a ){ke[1]=INPOUT(0xff);ke[2]=0;}
		C_CUROFF();
		if((a>=13 && a<=15)||((a==0x13||a==0x04||a==0x18||a==0x05||a==32)&&(exc&1)!=0)){
			if((cmdflag&0x2)!=0 && (exc&1)!=0){
				if(a==32){a=13;}else{if(a==13){a=32;}}
			}
			if(a==32||a==15){a=0x04;}
			es=0;ecode=a;
			if( st[0]==0 && o==1 ){break;}
			if((exc&0x80)==0){spcut(st);}
			strcpy(sb,st);break;
		}

		if( a==27 ){es= 1;break;}
		if( a==0x18 && (exc&2)!=0 ){es= 0;break;}
		if( a==0x13 && p>0 ){if(p==1){p=0;}else{if(knck(st,p-2)){p=p-2;}else{p=p-1;}}}
		if( a==0x04 ){if( knck(st,p) ){p=p+(p+2<=ln)*2;}else{p=p+(p+1<=ln);}}

		if( a==0x10 ){p=sln;}
		if( a==0x11 ){p=0;}
		if( a==0x15 ){strcpy(st,&st[p]);p=0;}

		if( p>sln ){p=sln;}

		if( a==0x0c ){p=0;st[0]=0;}

		if( st[0]!=0 ){
			if( a==8 && p>0 ){
				int i=1+knck(st,p-2);
				strcpy(tmp0,&st[p]);st[p-i]=0;strcat(st,tmp0);
				p=p-i;
			}
			if( a==0x07 ){
				strcpy(tmp0,&st[p+1+knck(st,p)]);st[p]=0;strcat(st,tmp0);
			}
			if( a==0x0b ){
				st[p]=0;
			}
		}

		if( im!=1 || (im==1 && str_search("0123456789ABCDEFabcdef.-Hh$",ke)>0) ){

			if( a>31 && a!=127 ){
				if( (B_SFTSNS()&4096)==0 && st[0]!=0 ){
					strcpy(tmp0,&st[p+1+knck(st,p)]);st[p]=0;strcat(st,tmp0);
				}
				sln=strlen(st);
				if( (sln+strlen(ke))<=ln ){
					if( sln==p ){
						strcat(st,ke);
					}else{
						strcpy(tmp0,&st[p]);st[p]=0;strcat(st,ke);strcat(st,tmp0);
					}
					p=p+strlen(ke);
				}
			}
		}
		tdis(x,y,st,ln,3);
	}
	tdis(x,y,sb,ln,3);B_LOCATE(x,y);
	fill(xx-1,y*16+scyp,xx+ln*8+1,y*16+15+scyp,0);
}

/***************************/
int	inkey2(void)
{
	int	ke=0;
	while(INPOUT(0xfe)!=0){INPOUT(0xff);}
	while(ke==0){ke=INPOUT(0xff);}
/*	if(ke>='a' && ke<='z'){ke-=32;}*/
	return(ke);
}

/***************************/
int	inkey3(void)
{
	int	ke=0,ex,sh;

	while(INPOUT(0xfe)!=0){INPOUT(0xff);}
	while(ke==0){
		ke=INPOUT(0xff);sh=B_SFTSNS()&15;

		ex=BITSNS(0xa);
		if( (ex&32)!=0 ){ke=0xe0;}
		if( (ex&64)!=0 ){ke=0xe1;}
		if( (ex&128)!=0 && sh==0){rcd->sts=0;}
		ex=BITSNS(0xb);
		if( (ex&1)!=0 && sh==0){rcd->sts=1;}
		if( (ex&2)!=0 && sh==0){all_note_off(1);}
		ex=BITSNS(0xc);
		if( (ex&8)!=0 ){ke=0xf0;}
		if( (ex&16)!=0 ){ke=0xf1;}
		if( (ex&32)!=0 ){ke=0xf2;}
		if( (ex&64)!=0 ){ke=0xf3;}
		if( (ex&128)!=0 ){ke=0xf4;}
		ex=BITSNS(0xd);
		if( (ex&1)!=0 ){ke=0xf5;}
		if( (ex&2)!=0 ){ke=0xf6;}
		if( (ex&4)!=0 ){ke=0xf7;}
		if( (ex&8)!=0 ){ke=0xf8;}
		if( (ex&16)!=0 ){ke=0xf9;}
	}
	while(INPOUT(0xfe)!=0){INPOUT(0xff);}
	return(ke+(sh*256));
}

/***************************/
void	snsclr()
{
	while(B_KEYSNS()){B_KEYINP();}
}

/***************************/
void	spcut(char *st)
{
	int	p;

	p= strlen(st);
	while(1){
		if( p<1 ){st[0]=0;return;}
		if( st[p-1]!=32 && st[p-1]!=9 ){break;}
		p--;
	}
	if( knck(st,p) ){p++;}
	st[p]=0;
}

/***************************/
int	knck(unsigned char *st,int p)
{
	int	i,f=0,a;

	if( p<0 ){return(0);}
	for(i=0 ;i<=p;i++){a=st[i];if( 127<a && (161>a || 224<a) ){f=(i==p);i++;}}
	return(f);
}

/***************************/
int	str_search(unsigned char *tmp0,unsigned char *tmp1)
{
	int	i=0;
	unsigned char	a;

	while(tmp0[i]!=0){
		a=tmp0[i];
		if(a==tmp1[0]){
			int j=0;
			while(1){
				if(tmp0[i+j]!=tmp1[j]){break;}
				j++;
				if(tmp1[j]==0){return(i+1);}
			}
		}
		i++;
		if( a>127 && (a<161 || a>224) ){i++;}
	}
	return(0);
}

/***************************/
int	str_last(unsigned char *tmp0)
{
	int	i,a;

	i=strlen(tmp0);
	if(i<2){return(tmp0[0]);}

	a=tmp0[i-1];
	if(knck(tmp0,i-2)){a=tmp0[i-2]*256+a;}
	return(a);
}

/***************************/
unsigned char	*jstrupr(unsigned char *st)
{
	unsigned char	a;
	unsigned char	*ss;
	ss=st;

	while(*st!=0){
		a=*st;
		if( 127<a && (161>a || 224<a) ){
			st++;
		}else{
			if('a'<=a && 'z'>=a){*st=a-32;}
		}
		st++;
	}
	return(ss);
}

/***************************/
char	*spadd(char *s,int c)
{
	static char	tmp0[128];
	int	i;

	strcpy(tmp0,s);for(i=strlen(s);i<=127;i++){tmp0[i]=32;}
	tmp0[c]=0;return(tmp0);
}

/***************************/
char	*fstr(int v,int c)
{
static	char	tmp0[64];
	char	tmp1[64];

	b_striS(tmp1,v);strcpy(tmp0,"                ");strcat(tmp0,tmp1);
	return(&tmp0[strlen(tmp0)-c]);
}

char	*nstr(int v)
{
static	char	tmp0[64];

	b_striS(tmp0,v);
	return(tmp0);
}

/***************************/
int	str_val(char *s)
{
	int	v=0;
	unsigned char	a;

	while(*s==' '){s++;}

	jstrupr(s);
	if(str_last(s)!='H' && s[0]!='$' ){return atoi(s);}

	while(*s!=0){
		a=*s++;
		if(a==' '||a==',' || a=='='){break;}
		if(a>='0' && a<='9'){v=v<<4;v=v+(a-'0');}
		if(a>='A' && a<='F'){v=v<<4;v=v+(a-'A')+10;}
	}

	return v;
}

/***************************/
char	*hex_s(int a,int c)
{
static	char	s[16];

	s[c]=0;
	while(c){c--;s[c]=hex_code[a&15];a=a>>4;}
	return(s);
}

/***************************/
void	fnc_dis(int n)
{
	int	i,j;
	char	tmp[64];

	KNJCTRL(1,0);
	if( n<0 ){
		TXRASCPY((30*4)*256+(31*4),4,3);
		for(i=1;i<=20;i++){FNCKEYST(i,"");}
	}else{
		if( n==2 ){
			for(i=0;i<10;i++){
				strcpy(tmp,fstr(stgt_tbl[i],4));
				strcat(tmp,fstr(stgt_tbl[i+10],5));
				for(j=0;j<9;j++){if(tmp[j]==32){tmp[j]='\\';}}

				tmp[0]=0x0c;tmp[4]=0x0f;
				tmp[5]=0x0c;tmp[9]=0x0e;tmp[10]=0;
				FNCKEYST(i+1,tmp);
				tmp[6]='0';tmp[7]=0xe;tmp[8]=0;

				FNCKEYST(i+1+10,tmp);
			}
		}

		if( n==8 ){
			for(i=0;i<10;i++){
				strcpy(tmp,fstr(stgt_tbl[i+20],3));
				for(j=0;j<2;j++){if(tmp[j]==32){tmp[j]='\\';}}

				tmp[3]=0x0d;tmp[4]=0;
				FNCKEYST(i+1,tmp);
				FNCKEYST(i+11,tmp);
			}
		}

		tmp[0]=32;tmp[1]=0;if(n==1){tmp[0]='*';}
		B_PUTMES(3,0,31,1,tmp);

		for(i=0;i<=9;i++){
			j=i*9+2;if(i>4){j+=3;}
			B_PUTMES(11,j,31,7,fnc_dat[n*10+i]);
		}
	}
}

/***************************/
void	home2(int y)
{
	HOME(0,0,y);scyp=y;
}

/***************************/
void	edfield(int m)
{
	if(m<1){fill(0,544,767,1023,0);if(m!=0){return;}}
	if(edit_scr<2 ){
		int	xx;

		if(m!=0){
			if(edit_scr==0){fill(0,544,312,1001,0);}else{fill(455,544,767,1001,0);}
		}
		if(edit_scr==0){xx=1;box(7,603,312,999,4,65535);
		}else{xx=57;box(455,603,760,999,4,65535);}

		sdis(xx,556,"M:",3,15,8);
		sdis2(xx,572," TR.:   MEAS:      CH.:    USED:",0,15,-1);
		sdis(xx,589," MEAS STEP:NOTE K#    ST    GT   VEL",38,15,14);
	}else{
		sdis(21,556,"MEMO:",41,15,8);sdis(64,556,"TR.:",6,15,8);
		sdis(72,556,"CH.:",7,15,8);sdis(80,556,"USED:",10,15,8);

		box(0,630,688,1001,14,65535);
		box(2,632,26,903,14,65535);box(28,632,148,903,14,65535);
		box(2,905,148,999,14,65535);box(150,632,686,999,14,65535);

		sdis(0,599+16,"    RHYTHM NAME   :STEP    4       8      12      16      20      24      28      32",86,15,14);
		sdis(2,512+26*16,"VEL :",4,15,-1);sdis(2,512+27*16," GT :",4,15,-1);

		sdis(46,592,"VEL",3,15,0);
		box(397,590,642,608,14,65535);box(363,588,644,610,14,65535);
	}
	trk_free(tr_pos[track][edit_scr][0]+tr_pos[track][edit_scr][2]*4);trk_no();snsclr();
}

/***************************/
void	noed_cls()
{
	if(edit_scr!=0){fill(0,544,454,1001,0);}else{fill(313,544,767,1001,0);}
}

void	noed_cls_t()
{
	if(edit_scr!=0){txerase(0,32,454,489-32,0);}else{txerase(313,32,767-313,489-32,0);}
}

/***************************/
void	cons_md(int x)
{
	if(edit_scr==1 && x!=0){B_CONSOL(56*8,0,95-56,30);}else{B_CONSOL(0,0,95,30);}
}

/***************************/
void	trk_free(int ad)
{
	int	xx,yy,cc;
	char	tmp[10];

	if(edit_scr>=2){
		xx=66;yy=556;cc=8;
	}else{
		if(edit_scr==0){xx=13+1;}else{xx=69+1;}
		yy=572;cc=0;

		vdis2(xx,yy,meas_no(ad),4,15,cc);
	}

	strcpy(tmp,chstr(channele_no(ad)));
	sdis2(xx+10,yy,tmp,3,15,cc);

	vdis2(xx+19,yy,(tr_len[track]>>2)-1,5,15,cc);
	trksize(-2);
}

/***************************/
void	trk_no()
{
	int	xx=31-8;

	trk_no_dis();
	if(edit_scr<2){
		if(edit_scr==0){xx=0;}else{xx=56;}
		vdis2(xx+6,572,track+1,2,15,0);
	}else{
		vdis2(68,556,track+1,2,15,8);
	}
	sdis(xx+3,556,trkmemo[track],36,15,8);
}

/***************************/
void	sdis(int x,int y,char *s,int c,int c1,int c2)
{
	x=x*8;
	if( c2>=0 ){fill(x-1,y-1,x+c*8,y+15,c2);}
	g_print(x,y,s,c1);
}

/***************************/
void	sdis2(int x,int y,char *s,int c,int c1,int c2)
{
	x=x*8;
	if( c2>=0 ){fill(x-1,y,x+c*8,y+15,c2);}
	g_print(x,y,s,c1);
}

/***************************/
void	sdis3(int x,int y,char *s,int c,int c1,int c2)
{
	x=x*8;
	if( c2>=0 ){fill(x-1,y,x+c*8,y+7,c2);}
	g_print2(x,y,s,c1);
}

/***************************/
void	vdis(int x,int y,int v,int c,int c1,int c2)
{
	sdis(x,y,fstr(v,c),c,c1,c2);
}

/***************************/
void	vdis2(int x,int y,int v,int c,int c1,int c2)
{
	sdis2(x,y,fstr(v,c),c,c1,c2);
}

/***************************/
void	vdis3(int x,int y,int v,int c,int c1,int c2)
{
	sdis3(x,y,fstr(v,c),c,c1,c2);
}

/***************************/
void	g_print2(int x,int y,char *s,int c1)
{
	char	tmp2[256];
	int i=0;

	while(*s!=0){tmp2[i++]=0xf0;tmp2[i++]=*s++;}
	tmp2[i]=0;

	g_print(x,y,tmp2,c1);
}

/***************************/
void	tdis(int x,int y,char *s,int c,int c1)
{
	if(c1>=0){B_COLOR(c1);}
	B_LOCATE(x,y);B_PRINT(spadd(s,c));B_COLOR(3);
}

/***************************/
void	msg(char *s)
{
	msg_clr();B_LOCATE(0,1);B_PRINT(s);
}

void	msg_clr()
{
	TXRASCPY((30*4)*256+(1*4),4,3);
}

/***************************/
int	help(int md)
{
	int	k,y,n,pg=0,ad,page=0,ln;
	int	poi[128];
	char	li[1024];
	FILE	*fp;

	if(!(fp= fopen(hlp_path,"rb"))){
		msg("HELPƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB");return(1);}
	ln=fread(dat,1,63*1024,fp);fclose(fp);

	ad=0;poi[0]=0;dat[ln]=0x1a;
	while(ad<ln && page<127){
		if(dat[ad]==12 || dat[ad]==0x1a ){
			if( dat[ad]==12 ){ad=ad+2;}
			page++;poi[page]=ad+1;
			if( dat[ad]==0x1a){break;}
		}
		ad++;
	}
	if(page>0){page--;}

	fill(0,544,767,1001,0);box(0,564,767,1001,14,65535);
	sdis(0,548," COMMAND HELP",14,15,8);home2(512);
	C_WINDOW(0,32);
/*	B_LOCATE(0,31);B_PRINT(" [SPC]/[R.UP] ŽŸ‚Ìƒy[ƒW  [BS]/[R.DOWN] ‘O‚Ìƒy[ƒW  [TAB] ƒy[ƒWŽw’è  [ESC] EXIT");*/
	B_LOCATE(0,31);B_PRINT(" ì@ìA/ëó ŽŸ‚Ìƒy[ƒW  ëø/ëô ‘O‚Ìƒy[ƒW  ìB ƒy[ƒWŽw’è  ëö EXIT");
	C_WINDOW(0,31);

	while(1){
		B_CLR_AL();
		B_LOCATE(80,2);B_PRINT("PAGE:");B_PRINT(fstr(pg+1,3));
		B_PRINT("/");B_PRINT(fstr(page+1,3));

		y=4;ad=poi[pg];
		while( dat[ad]!=0x1a && dat[ad]!=0 && dat[ad]!=12){
			n=0;
			while(dat[ad]>31||dat[ad]==9||dat[ad]==27){li[n]=dat[ad];n++;ad++;}
			li[n]=0;if( li[0]!='*' ){B_LOCATE(0,y);B_PRINT(li);y++;}

			ad++;
			if(dat[ad]==0x0d){ad++;}
			if(dat[ad]==0x0a){ad++;}
		}
		while(1){
			k=inkey2();
			if(k==27){break;}
			if(k==0x12||k==32){
				pg++;if(pg>page){pg=0;}
				break;
			}
			if(k==0x17||k==8){
				pg--;if(pg<0){pg=page;}
				break;
			}

			if( k==9 ){
				int n;
				msg("Page No.=");n=vinput(pg+1,0x803);msg_clr();

				if( es==0 && n>0 && n<=page+1){
					pg=n-1;break;
				}
			}

		}
		if(k==27){break;}
	}
	C_WINDOW(0,32);B_CLR_AL();C_WINDOW(0,31);return(0);
}

/***************************/

