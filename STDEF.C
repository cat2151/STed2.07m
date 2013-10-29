/*
 * def file converter v0.01 (c)1996/12/11 by TURBO
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"stddef.h"
#include	"class.h"
#include	"ctype.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"sted.h"

void	spcut();
int	klen();
int	knck();
void	str_change();
int	str_search();
int	str_last();
int	str_val();

void	cnfinit();
void	definit();
int	defload();
void	asin_init();
void	user_exc_init();
void	stgt_init();
char	*fstr();
char	*chstr();
int	strch();
unsigned char	*jstrupr();

char	hex_code[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

/********** program start **********/
void	main(int argc,char *argv[])
{
	int	i,f=0,convmode;
	char	cmd[128];

	def_file[0]=0;

	work_size=64*1024;
	/*cmdflag=0;cnfflag=0;*/mdlflag=0;

	B_PRINT("X68k STed def<->dfb file converter STdef.x v0.12 Copyright 1996,97 TURBO\r\n");

	if( argc>1 ){
		for(i=1;i<argc;i++){
			strcpy(cmd,argv[i]);

			if( cmd[0]=='-' ){
				switch(cmd[1]){

					case 'C':
					case 'c':convmode=0;break;

					case 'R':
					case 'r':convmode=1;break;

					default:f=1;
				}
			}else{
				strcpy(def_file,cmd);
			}
		}
	}

	if( f!=0 || argc<2){
/*		B_PRINT(
		"usage: STdef [option...] [file name] (.DEF/.DFB)\r\n"
		"options\r\n"
		"\t-C<file>\t音源定義ファイルをバイナリに変換する\r\n\n"
*/
		B_PRINT("usage: STdef <file name> (.DEF/.DFB)\r\n\n");
		exit(1);
	}

	dat=(unsigned char *)MALLOC(work_size);
	if((int)dat>=0x81000000){
		B_PRINT("バッファが確保出来ません。\r\n");exit(1);}

	if( def_file[0]==0 ){
		B_PRINT("ファイルを指定して下さい。\r\n");exit(1);
	}

	definit();

	if(str_search(def_file,".")==0){

		strmfe(def_file,def_file,"def");
		i=defload(def_file,2);

		if(i!=0){
			if(i==-2){exit(1);}

			strmfe(def_file,def_file,"dfb");
			i=def_bload(def_file,2);

			if(i!=0){
				if(i==-1){B_PRINT("ファイルが見付かりません。\r\n");}
				exit(1);
			}

		}
	}else{
		i=defload(def_file,2);

		if(i!=0){
			if(i==-1){B_PRINT("ファイルが見付かりません。\r\n");}
			exit(1);
		}
	}
	exit(1);
}

void	tim_name_read(){}
void	msg(){}

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
		ct=ct/*+key_shi[skey&15][ct-1]*/;

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
static	char	tmp0[128];
	char	tmp1[128];

	b_striS(tmp1,v);strcpy(tmp0,"                ");strcat(tmp0,tmp1);
	return(&tmp0[strlen(tmp0)-c]);
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
	tmp0[14]=0;
	return tmp0;
}

/***************************/
