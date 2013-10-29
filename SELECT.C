/*
 * STed: select.c (selecter)
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
#include	"stat.h"

typedef struct {
	char		name[23];
	unsigned char	far;
	unsigned int	fln;
	unsigned int	ftm;
	unsigned int	fdt;
} DIR;

static char	*fsel_ext[]={
			"R36:RCP","RCP:R36","CM6","GSD","DEF:DFB","TRK","PRT","TXT","EXC",
			"REX","RES","RAS"};

static char	*fsel_title[]={
			"COMPOSE","COMPOSE","CM-CONTROL","GS-CONTROL","DEFINE","TRACK",
			"PART","TEXT","EXCLUSIVE(TEXT)",
			"User Exclusive(ALL)","User Exclusive(ONE)","RHYTHM ASSIGN"};

static char	speci_sym[24]={
		'G','M','T','P','C','K','L','D','V','N','E','H','A',0,'X','B','R','I',
		'E','E','N','N','R','R'};

static char	speci_sym2[24]={
		 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,0, 0 , 0 , 0 , 0 ,
		'M','L','M','L','M','L'};

static char	speci_name[24][9]={
		"PROGRAM ","MIDI CH.","TEMPO   ","PITCH   ","AFTER C.","AFTER K.","CONTROL ",
		"MODULAT ","VOLUME  ","PANPOT  ","EXPRESS ","HOLD1   ","RES.ALL ","        ",
		"Tr.Exclu","Rol.Base","Rol.Para","Rol.Dev#",
		"DATA MSB","DATA LSB","NRPN MSB","NRPN LSB","RPN MSB ","RPN LSB "};

/*		 1234567890123456789012345678901234567890*/
static char	*speci_come[24]={
		"PROGRAM CHANGE",
		"MIDI CHANNEL CHANGE",
		"TEMPO CHANGE",
		"PITCH BEND CHANGE",
		"AFTER TOUCH(CH.)",
		"AFTER TOUCH(POLY)",
		"CONTROL CHANGE",
		"MODULATION",
		"VOLUME",
		"PANPOT",
		"EXPRESSION",
		"HOLD1(DAMPER PEDAL)",
		"RESET ALL CONTROLLERS",
		"",
		"Track Exclusive",
		"Roland Base Address",
		"Roland Offset Add. & Para.",
		"Roland Device No. & Model ID",
		"Data Entry(MSB)",
		"Data Entry(LSB)",
		"Non Registerd Parameter MSB",
		"Non Registerd Parameter LSB",
		"Registerd Parameter MSB",
		"Registerd Parameter LSB"};

static char	*repl_title[3]={
		" Replace"," Delete"," Find"};

int	fsdirp[256],fsdiry[256];/*check*/

int	tkey,tvel;

char	*dtasc();
char	*tmasc();

void	sinput();
int	vinput();
int	inkey2();
void	spcut();
char	*sread();
char	*tim_names();
char	*fstr();
char	*hex_s();
int	yn_sel();
char	*keystr();
char	*prog_name();
char	*spadd();
void	home2();
void	msg();
void	msg_clr();
void	sdis();
void	tdis();
void	tcur();
void	fnc_dis();
void	all_note_off();
void	temporary_set();
void	timtra();
void	rhinit();
int	ctc();
int	meas_no();
int	meas_add();
int	meas_len();
int	real_cy();
void	snsclr();
void	twait();
void	txerase();

void	path_cut();
int	str_last();
int	str_search();
int	knck();
char	*jstrupr();
void	H_PRINT2();
void	t_scrw();
void	cons_md();
void	noed_cls();

void	rcp_dat2();
void	uex_read();

int	timsel();
int	timsel2();
int	varia();
void	s_test();
void	m_keyb();
void	stest_para_disp();
void	stest_para_edit();

void	fsel();
void	str_change();
void	path_cut();
void	path_down();
void	frees();
char	*dirs();
void	dir_disp();
int	fdir();
int	drv_code();
int	file_check();
void	file_back();
int	drvchk();
int	drvchk2();
void	memo_disp();
void	memo_load();

/***************************/
int	timsel(int bmode,int bank,int gx,int prog)
{
	int	a,nm=0,i,i0,p=0,y=0,dx,scr,sut=0,sh;
	int	ch,ch2,bank2,bank_l,bank_l2,as,as2,md=0,wd=0;
	char	tmp0[256],tmp1[256];

	tkey=60;tvel=127;

	if(bmode&0x100){md=1;wd=2;}
	ch=bmode&0xff;bank_l=(bank>>8)&0x7f;bank=bank&0x7f;
	dx=gx*8;es=0;as=tim_asin[ch];
	if(prog>=0){p=prog&0xf0;y=prog&15;}
/*
	if(gs_mode[16]!=0 && as==7){as=gs_bank[16*128+bank_l];}
	if(gs_mode[17]!=0 && as==8){as=gs_bank[17*128+bank_l];}
*/
	if((gs_mode[16]&4)!=0){
		if((gs_mode[16]&128)!=0 && as==7){as=gs_bank[16*128+bank_l];}
		if((gs_mode[17]&128)!=0 && as==8){as=gs_bank[17*128+bank_l];}
	}else{
		if((gs_mode[16]&128)!=0 && as==7){as=gs_bank[16*128+bank];}
		if((gs_mode[17]&128)!=0 && as==8){as=gs_bank[17*128+bank];}
	}

	if((as==7||as==9||as==11||as==13)&&(gs_mode[as]&4)!=0){bank=bank_l;}

	if(as==0||as>399||(md==0 && as==2)){es=1;return(-1);}
	scr=edit_scr;edit_scr=0;as2=as;ch2=ch;bank2=bank;bank_l2=bank_l;

	fill(dx+316,566,dx+453+wd*8,997,0);
	fill(dx+316,650,dx+452,706,14);
/*	box(dx+316,707,dx+452+wd*8,988,14,65535);
	box(dx+317,708,dx+451+wd*8,987,14,65535);*/

/*	box(dx+316,707,dx+452+wd*8,988+4,14,65535);
	box(dx+318,709,dx+450+wd*8,986-8,14,65535);*/

	box(dx+316,707,dx+452+wd*8,988,14,65535);
	box(dx+318,709,dx+450+wd*8,986,14,65535);

	box(320+dx,602,448+dx,644,14,65535);
	box(322+dx,604,446+dx,642,14,65535);

	stest_para_disp(gx+44);
	B_LOCATE(gx+40,9);B_PRINT("## TONE LIST ##");

	if((cmdflag&0x2000)!=0 && (as==7||as==9||as==11||as==13)/* && bank<64*/){
		int n=timsel2(ch,bank_l*256+bank,gx,prog);
		prog=n&0xff;bank=n>>8;p=prog&0xf0;y=prog&15;
		if(es==0||es==-2){goto skip;}
	}

top:
	while(1){
		tdis(gx+40,10,tim_head[as-1],18,3);tmp0[0]=0;
		if(as>=7 && as<=14 && (md!=0 ||(as&1)!=0)){
			i0=gs_bank[as*128+bank];
			if(i0>0){strcpy(tmp0,tim_head[i0-1]);}
		}
		tdis(gx+40,11,tmp0,18,3);

		if(as==2){nm=85;}else{nm=128;}

		for(i=p ;i<= p+15;i++){
			if(as==2||(md!=0 && (as==8||as==10||as==12||as==14))){
				if(i<128){
					if(as==2){
						strcpy(tmp0,fstr(i+24,3));
						if(i<85){
							tmp0[3]=':';tmp0[4]=0;
							strcat(tmp0,tim_name[cm6[0x130+i*4]+128]);
						}else{
							tmp0[0]=0;
						}
					}else{
						strcpy(tmp0,fstr(i,3));
						varia(tmp1,bank_l*256+bank,i,as);strcat(tmp0,tmp1);
					}
				}else{
					tmp0[0]=0;
				}
			}else{
				i0=i;if((cmdflag&0x20)!=0){i0++;}
				strcpy(tmp0,fstr(i0,3));tmp0[3]=':';tmp0[4]=0;
				if( as==1 ){
					i0=cm6[0xa34+i*8]*64+cm6[0xa35+i*8];
					strcat(tmp0,tim_name[i0]);
				}
				if( as==3 ){
					i0=cm6[0x4eb2+i*19]*128+cm6[0x4eb3+i*19];
					strcat(tmp0,tim_name[i0+256]);
				}
				if( as>3){
					if(as<7 || as>=14 || as==8 || as==10 || as==12){
						strcat(tmp0,tim_names(as*128-128+i));
					}else{
						tmp0[3]=0;

						if(as==7 || as==9 || as==11||as==13){
							if(gs_mode[as]&4){bank_l=bank;}

							varia(tmp1,bank_l*256+bank,i,as);
						}else{
							varia(tmp1,bank_l*256+i,0,as+16);
						}
						strcat(tmp0,tmp1);
					}
				}
			}
			tdis(gx+40,(i-p)+13,tmp0,16+wd,3);
		}
 		while( 1){
			prog=y+p;
/*
			if(md==0 &&(as==8 || as==10 || as==12 || as==14)){
				tmp0[0]=0;
				i0=gs_bank[as*128+prog];
				if(i0>0){strcpy(tmp0,tim_head[i0-1]);}
				tdis(gx+40,11,tmp0,18,3);
			}
*/
/*
			strcpy(tmp0,"ch.=");strcat(tmp0,chstr(ch));
			strcat(tmp0," Bank L=");strcat(tmp0,fstr(bank_l,3));
			scprint(gx+40,28*2+2+1,tmp0);
*/

			tcur(gx+44,y+13,12+wd,3);
			a=inkey2();sh=B_SFTSNS();

			if(a==0x15){
				sut=1;
				if(as==2 || as==8 ||as==10 || as==12||as==14){/*rhythm*/
					if(md!=0){
						int oft=0;
						if(as==2){oft=24;}
						s_test(ch,bank_l*256,bank,prog+oft,tvel);
					}else{
						s_test(ch,bank_l*256,prog,tkey,tvel);
					}
				}else{
					if((as==7||as==9||as==11||as==13)&&(gs_mode[as]&4)!=0){
						s_test(ch,bank*256,prog,tkey,tvel);
					}else{
						s_test(ch,bank,prog,tkey,tvel);
					}
				}
			}

			tcur(gx+44,y+13,12+wd,3);

			if( as!=2 && (a==0x13 || a==0x04) ){

			if(sh&4){
/*BANK G.*/
				if(gs_mode[16]&&(as==7 || as==9 ||as==11 || as==13)){
					int i=bank_l;
					while(gs_bank[16*128+i]==0||bank_l==i||as==gs_bank[16*128+i]){
						if(a==0x13){
							i--;if(i<0){i=127;}
						}else{
							i++;if(i>127){i=0;}
						}
						if(i==bank_l){break;}
					}
					bank_l=i;
					as=gs_bank[16*128+bank_l];
					goto top;
				}
				if(gs_mode[17]&&(as==8 || as==10 ||as==12 || as==14)){
					int i=bank_l;
					while(gs_bank[17*128+i]==0||bank_l==i||as==gs_bank[17*128+i]){
						if(a==0x13){
							i--;if(i<0){i=127;}
						}else{
							i++;if(i>127){i=0;}
						}
						if(i==bank_l){break;}
					}
					bank_l=i;
					as=gs_bank[17*128+bank_l];
					goto top;
				}
			}else{
/*MIDI CH.*/

				if((sh & 2)!=0 && md==0){
					int i=ch;
					while(tim_asin[i]==2 || tim_asin[i]==0 ||tim_asin[i]==as){

						if(a==0x13){
							i--;if(i<1){i=32;}
						}else{
							i++;if(i>32){i=1;}
						}
						if(i==ch){break;}
					}
					ch=i;as=tim_asin[ch];
				}else{
/*BANK*/
					int i=bank;
					if((as>=7 && as<=14)&&(md!=0 || (as&1)!=0 )){
						while(gs_bank[as*128+i]==0 || bank==i){
							if(a==0x13){
								i--;if(i<0){i=127;}
							}else{
								i++;if(i>127){i=0;}
							}
							if(i==bank){break;}
						}
					}
					bank=i;
				}
				break;
		}
			}

			if( a==12 ){bank=bank2;bank_l=bank_l2;ch=ch2;as=as2;goto top;}

			if( a==0x05 ){
				y--;if( y<0 ){y=0;if( p>0 ){p--;break;}}
			}
			if( a==0x18 && prog+1<nm){
				y++;if( y>15 ){y=15;if( p+16<nm ){p++;break;}}
			}
			if( a==0x17 ){if( p>0 ){p-=16;if(p<0){p=0;}break;}else{y=0;}}

			if( a==0x12 ){if( p+16<nm ){p+=16;if(p+16>nm){p=nm-16;}
					if(p+y>=nm){y=(nm-1)-p;}break;
				}else{y=(nm-1)-p;if(y<0){y=0;}}}

			if( a==0x0b ){p=0;y=0;break;}

			if( a==9 ){
				int n=p+y;
				if(md==0 &&(cmdflag&0x20)!=0){n++;}
				msg("Tone No.=");n=vinput(n,0x803);msg_clr();
				if(md==0 &&(cmdflag&0x20)!=0){n--;}

				if( es==0 && n>=0 && n<nm){
					if(as==2 && n>24){n-=24;}
					p=n&0xf0;y=n&15;break;
				}
			}

			if(a=='Z'||a=='z'){timtra();}

			if(a=='K'||a=='k'){stest_para_edit(gx+49,0);}
			if(a=='V'||a=='v'){stest_para_edit(gx+49,1);}
/*
			if(a=='>'||a=='.'||a=='+'){
				if((sh&1)==0){
					if(tkey<=115){tkey+=12;}
				}else{
					if(tvel<=117){tvel+=10;}
				}
				stest_para_disp(gx+49);
			}
			if(a=='<'||a==','||a=='-'){
				if((sh&1)==0){
					if(tkey>=12){tkey-=12;}
				}else{
					if(tvel>=10){tvel-=10;}
				}
				stest_para_disp(gx+49);
			}
*/
			if((a=='?'||a=='/') && (as==7||as==9||as==11||as==13)/*&& bank<64*/){
				int n=timsel2(ch,bank_l*256+bank,gx,prog);
				prog=n&0xff;bank=(n>>8);p=prog&0xf0;y=prog&15;
				if(es==0 || es==-2){goto skip;}
				goto top;
			}

			if( a==13 ){es= 0;break;}
			if( a==27 ){es=-2;break;}
		}
		if( a==13 || a==27 ){break;}
	}
	if(sut!=0){temporary_set();}
skip:
	edit_scr=scr;
	txerase(dx+40*8,5*16,(18+wd)*8,25*16);
	fill(dx+316,601,dx+453+wd*8,997,0);
	if(md){bank=0;}
	if(as==2){prog+=24;}

	if( es==0 ){return(prog+bank*256);}else{return(-2);}
}

/***************************/
int	timsel2(int ch,int bank,int gx,int prog)
{
	int	a,nm,i,j,i0,p=0,y=0,dx,as,as2,sut=0,sh;
	int	bank_l,bank_l2;
	char	tmp0[256];
	int	tbk[1024],tpg[1024],tnb[1024];

	bank_l=(bank>>8)&0x7f;bank=bank&0x7f;
	dx=gx*8;es=0;as=tim_asin[ch];

	bank_l2=bank_l;as2=as;
/*
	if(gs_mode[16]!=0 && as==7){as=gs_bank[16*128+bank_l];}
*/
	if((gs_mode[16]&4)!=0){
		if((gs_mode[16]&128)!=0 && as==7){as=gs_bank[16*128+bank_l];}
	}else{
		if((gs_mode[16]&128)!=0 && as==7){as=gs_bank[16*128+bank];}
	}


/*	if(gs_mode[17]!=0 && as==8){as=gs_bank[17*128+bank_l];}*/
top:
/*	if(as!=7 && as!=9 && as!=11 && as!=13){es=1;return(-1);}*/

	nm=0;/*p=0;y=0;*/
	for(i=0;i<128;i++){
		for(j=0;j<64+64;j++){
			int	n;

			if(i==prog && j<=bank){p=nm&0xff0;y=nm&15;}

			n=gs_bank[as*128+j]*128-128+i;
			if(n>=0 && tim_names(n)[0]>32){
				if(nm<1024){
					tbk[nm]=j;tpg[nm]=i;tnb[nm]=n;nm++;
				}
			}
		}
	}

while(p+16>nm && p>0 && y<15){p--;y++;}

	while(1){
		tdis(gx+40,10,tim_head[as-1],18,3);

		for(i=p;i<p+16;i++){
			int cc=3;tmp0[0]=0;
			if(i<nm){
				i0=tbk[i];
				if(i0==0 ||i==0||(tbk[i]<=tbk[i-1])){
					if(i0){cc=1;}
					i0=tpg[i];
					if((cmdflag&0x20)!=0){i0++;}
					strcpy(tmp0,fstr(i0,3));
				}else{
					strcpy(tmp0,"   ");cc=1;
				}
				strcat(tmp0,":");
				i0=tnb[i];if(i0>=0){strcat(tmp0,tim_names(i0));}
			}
			tdis(gx+ 40,(i-p)+13,tmp0,16,cc);
		}
 		while(1){
			int cc=3;
			i=y+p;if(i<nm){bank=tbk[i];prog=tpg[i];if(bank){cc=1;}}

			tmp0[0]=0;
			i0=gs_bank[as*128+bank];
			if(i0>0 && i0<=400){strcpy(tmp0,tim_head[i0-1]);}
			tdis(gx+40,11,tmp0,18,3);

			tcur(gx+44,y+13,12,cc);
			a=inkey2();sh=B_SFTSNS();

			if(a==0x15){
				sut=1;
				if((gs_mode[as]&4)!=0){
					s_test(ch,bank*256,prog,tkey,tvel);
				}else{
					s_test(ch,bank_l*256+bank,prog,tkey,tvel);
				}
			}

			tcur(gx+44,y+13,12,cc);

			if( as!=2 && (a==0x13 || a==0x04) ){
			if(sh&4){
				if(gs_mode[16]&&(as==7 || as==9 ||as==11 || as==13)){
					int i=bank_l;
					while(gs_bank[16*128+i]==0||bank_l==i||as==gs_bank[16*128+i]){
						if(a==0x13){
							i--;if(i<0){i=127;}
						}else{
							i++;if(i>127){i=0;}
						}
						if(i==bank_l){break;}
					}
					bank_l=i;
					as=gs_bank[16*128+bank_l];
					goto top;
				}
			}
			}
			if( a==12 ){bank_l=bank_l2;as=as2;goto top;}

			if( a==0x05 ){
				y--;if( y<0 ){y=0;if( p>0 ){p--;break;}}
			}
			if( a==0x18 && i+1<nm){
				y++;if( y>15 ){y=15;if( p+16<nm ){p++;break;}}
			}
			if( a==0x17 ){if( p>0 ){p-=16;if(p<0){p=0;}break;}else{y=0;}}

			if( a==0x12 ){if( p+16<nm ){p+=16;if(p+16>nm){p=nm-16;}
					if(p+y>=nm){y=(nm-1)-p;}break;
				}else{y=(nm-1)-p;if(y<0){y=0;}}}

			if( a==0x0b ){p=0;y=0;break;}

			if( a==9 ){
				int i,n=tpg[p+y];
				if((cmdflag&0x20)!=0){n++;}
				msg("Tone No.=");n=vinput(n,0x803);msg_clr();
				if((cmdflag&0x20)!=0){n--;}
				if( es==0 && n>=0 && n<128){
					for(i=0;i<nm;i++){
						if(tpg[i]>=n){p=i&0xfff0;y=i&15;break;}
					}
					break;
				}
			}

			if(a=='Z'||a=='z'){timtra();}

			if(a=='K'||a=='k'){stest_para_edit(gx+49,0);}
			if(a=='V'||a=='v'){stest_para_edit(gx+49,1);}

			if( a=='/'||a=='?' ){es=1;goto skip;}
			if( a==13 ){es= 0;break;}
			if( a==27 ){es=-2;break;}
		}
		if( a==13 || a==27 ){break;}
	}
skip:
	if(sut!=0){temporary_set();}
	return(prog+bank*256);
}

/***************************/
void	stest_para_disp(int x)
{
	B_LOCATE(x,6);B_PRINT("KEY= ");B_PRINT(keystr(tkey));
	B_LOCATE(x,7);B_PRINT("VEL= ");B_PRINT(fstr(tvel,3));
}

void	stest_para_edit(int x,int a)
{
	int	v;
	char	tmp0[128];

	if(a==0){
		B_LOCATE(x,6);strcpy(tmp0,keystr(tkey));sinput(tmp0,0x803);
		if(es==0){v=ctc(60,tmp0,0);if(v>=0){tkey=v;}}
	}else{
		B_LOCATE(x,7);v=vinput(tvel,0x803);
		if(es==0 && v>=0 && v<128){tvel=v;}
	}
	stest_para_disp(x-5);
}

/***************************/
int	varia(char *pname,int bank,int prg,int md)
{
	int	bank2,bank_l,n=0;
	bank_l=(bank>>8)&0xff;bank=bank&0xff;

/*	if((md==7||md==9||md==11||md==13)&& (gs_mode[md]&4)){bank=bank_l;}

	if(gs_mode[16]!=0 &&  md==7         ){md=gs_bank[16*128+bank_l];}
	if(gs_mode[17]!=0 && (md==8||md==24)){md=gs_bank[17*128+bank_l]|(md&16);}
*/
/*
	if((gs_mode[16]&4)!=0 ){
		if(gs_mode[16]!=0 &&  md==7         ){md=gs_bank[16*128+bank_l];}
		if(gs_mode[17]!=0 && (md==8||md==24)){md=gs_bank[17*128+bank_l]|(md&16);}
	}else{
		if(gs_mode[16]!=0 &&  md==7         ){md=gs_bank[16*128+bank];}
		if(gs_mode[17]!=0 && (md==8||md==24)){md=gs_bank[17*128+bank]|(md&16);}
	}
*/
	if((md==7||md==9||md==11||md==13)&& (gs_mode[md]&4)){bank=bank_l;}


	bank2=bank;

	while(bank>=0){
		if(md<16){
			n=gs_bank[md*128+bank]*128-128+prg;
		}else{
			n=(md-1)*128-128+bank;
		}

		if((gs_mode[7]&1)==0 && (md==7||md==24)){break;}
		if((gs_mode[9]&1)==0 && (md==9||md==26)){break;}
		if((gs_mode[11]&1)==0 && (md==11||md==28)){break;}
		if((gs_mode[13]&1)==0 && (md==13||md==30)){break;}

		if((md==8||md==10||md==12||md==14)&&(gs_mode[md]&2)==0){break;}

		if(bank>63 ||(n>=0 && tim_names(n)[0]!=0)){break;}

		if(md==7 || md==9 || md==11||md==13){
			if(prg>=120){break;}
			if((bank&15)==0){bank-=16;}else{bank--;}
		}
		if(md==8 || md==10 || md==12||md==14){
			if(bank>=49){break;}
			if((bank&31)==0){bank-=32;}else{bank=bank&0xe0;}
		}
		if(md==24 || md==26 || md==28||md==30){
			if((bank&7)==0){bank-=8;}else{bank=bank&0xf8;}
		}
	}
	pname[1]=0;if(bank!=bank2 && bank>=0){pname[0]='#';}else{pname[0]=':';}
	if((md==7||md==9||md==11||md==13)&& gs_mode[md]==0){bank=0;n=md*128-128+prg;}

	if(bank>=0 && n>=0){strcat(pname,tim_names(n));}
	return(bank);
}

/***************************/
void	s_test(int ch,int bank,int prg,int note,int velo)
{
	int	bnk_m,bnk_l,pm,ch2;
	char	obuf[64];

	if(B_SFTSNS()&1){m_keyb(ch,bank,prg,velo);return;}

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

	obuf[0]=0x90+ch2;obuf[1]=note;obuf[2]=velo;obuf[3]=255;
	(*rcd->mix_out)(obuf);

	twait(5);while((BITSNS(7)&4)==4){}

	obuf[0]=0x90+ch2;obuf[1]=note;obuf[2]=0;obuf[3]=255;
	(*rcd->mix_out)(obuf);

	snsclr();rcd->put_mode=pm;
}

/***************************/
int	spc_select()
{
	int	ke,i,y=0,cx,gx;
	char	tmp0[128];

	if(edit_scr==0){cx=41;gx=327;}else{cx=2+1;gx=15+8;}
	cons_md(0);noed_cls();

	box(gx,585,gx+417,997,14,65535);
	sdis(cx,570," SPECIAL CONTROLER",52,15,14);

	for(i=0;i<24;i++){
		if(speci_sym[i]!=0){
			strcpy(tmp0,"[ ]  ");tmp0[1]=speci_sym[i];
			if(speci_sym2[i]!=0){tmp0[0]=32;tmp0[1]=32;tmp0[2]=32;}
			strcat(tmp0,speci_name[i]);strcat(tmp0," : ");
			strcat(tmp0,speci_come[i]);
			B_LOCATE(cx+1,i+5);B_PRINT(tmp0);
		}
	}

	while( 1){
		tdis(cx+17,y+5,speci_come[y],34,11);
		ke=inkey2();if(ke>='a' && ke<='z'){ke=ke-32;}
		tdis(cx+17,y+5,speci_come[y],34,3);

		if( ke==27 ){break;}
		if((ke==0x04 || ke==' ' || ke==13) && speci_sym[y]!=0 ){break;}
		if( ke==0x05 ){y--;if( y<0 ){y=23;}}
		if( ke==0x18 ){y++;if( y>23 ){y=0;}}

		if(ke>='A' && ke<='Z'){
			for(i=0;i<18;i++){
				if(ke==speci_sym[i]){y=i;}
			}
		}
	}
	/*txerase(cx*8,5*16,51*8,24*16);*/noed_cls_t();noed_cls();cons_md(1);
	if( ke==13 ){return(speci_sym[y]+speci_sym2[y]*256);}
	return(0);
}

/***************************/
int	repl_select(int md)
{
	int	ke,sh,i,y=0,cx,gx,pg=0;
	char	tmp0[128];

/*
replinp:

	if(md==0){
		msg("REPLACE=");sinput(repl,0x200+30);
	}else{
		if(md==1){
			msg("DELETE=");sinput(delt,0x200+30);
		}else{
			msg("FIND=");sinput(srch,0x200+30);
		}
	}
	msg_clr();

	if(ecode==0x18){
		int ct=repl_select(md);
		if(ct>=0){
			if(md==0){
				strcpy(repl,repl_d[ct]);
			}else{
				if(md==1){
					strcpy(delt,repl_d[ct]);
				}else{
					strcpy(srch,repl_d[ct]);
				}
			}
		}
		goto replinp;
	}
*/


	if(edit_scr==0){cx=41+16-16;gx=327+128-128;}else{cx=1+16;gx=7+128;}

	cons_md(0);noed_cls();

	box(gx,585-14,gx+417-112,997,14,65535);
	strcpy(tmp0,repl_title[md]);
	strcat(tmp0," Macro Selecter");
	sdis(cx,570-14,tmp0,38,15,14);
redraw:

	for(i=0;i<26;i++){
		int nn=i+md*52+pg;
		if(repl_t[nn][0]!=0 && repl_d[nn][0]!=0){
			strcpy(tmp0,"  :");tmp0[1]=i+'A';if(pg){tmp0[0]='s';}
		}else{strcpy(tmp0,"   ");}

		strcat(tmp0,repl_t[nn]);
		tdis(cx,i+4,tmp0,34+3,3);
	}

	while( 1){
		int nn=y+md*52+pg;

		tdis(cx+3,y+4,repl_t[nn],34,11);
		ke=inkey2();sh=B_SFTSNS();
		if(ke>='a' && ke<='z'){ke=ke-32;}
		tdis(cx+3,y+4,repl_t[nn],34,3);

		if( ke==27 ){break;}
		if( ke==0x15 ){sh=0;break;}

		if(ke>='A' && ke<='Z'){
			y=ke-'A';
			nn=y+md*52+pg;
/*
if((sh&4)!=0){if(nn>25){nn-=26;}
if((sh&8)!=0){if(nn<26){nn+=26;}
*/
			if( /*repl_t[nn][0]!=0 &&*/ repl_d[nn][0]!=0 ){
				if((sh&1)==0){y+=128;}
				ke=13;break;
			}
		}
		if( ke==13 && /*repl_t[nn][0]!=0 &&*/ repl_d[nn][0]!=0 ){
			if((sh&1)==0){y+=128;}
			break;}

		if( ke==0x05 ){y--;if( y<0 ){y=25;}}
		if( ke==0x18 ){y++;if( y>25 ){y=0;}}

		if( ke==0x17 || ke==0x13 ){pg-=26;if(pg<0){pg=26;}goto redraw;}
		if( ke==0x12 || ke==0x04 ){pg+=26;if(pg>26){pg=0;}goto redraw;}

	}
	/*txerase(cx*8,4*16,38*8,26*16);*/noed_cls_t();noed_cls();cons_md(1);
	if( ke==13 ){return(y+md*52+pg);}
	if((sh&1)!=0){return -2;}
	return(-1);
}

/***************************/
/*
	1997-07-25	Trk.Editで[OPT1]+[CLR]でトラックメモを表示する
			トラックセレクタを呼び出せるようにした
	1997-07-26	↑のトラックセレクタ上でも[OPT1]+テンキーで移
			動出来るようにした
	1997-07-29	Trk.Editでの[CTRL]+[T]で[↓]でトラックセレクタ
			を呼び出せるようにした
*/
/*
int	trk_sel_menu(int md)
{
	int	ke,sh,i,y=0,cx,gx,pg=0;
	char	tmp0[128];

	if(edit_scr==0){cx=41+16-16;gx=327+128-128;}else{cx=1+16;gx=7+128;}

	cons_md(0);noed_cls();


if(md<26){pg=0;y=md;}else{pg=10;y=md-10;}

	box(gx,585-14,gx+417-112,997,14,65535);
	strcpy(tmp0," Track Selecter");
	sdis(cx,570-14,tmp0,38,15,14);
redraw:

	for(i=0;i<26;i++){
		int nn=i+pg;
		if(tr_len[nn]>4){
			strcpy(tmp0,fstr(nn+1,2));
			strcat(tmp0,":");
		}else{strcpy(tmp0,"   ");}

		strcat(tmp0,trkmemo[nn]);
		tdis(cx,i+4,tmp0,34+3,3);
	}

	while( 1){
		int nn=y+pg;

		tdis(cx+3,y+4,trkmemo[nn],34,11);
		ke=inkey2();sh=B_SFTSNS();
		if(ke>='a' && ke<='z'){ke=ke-32;}
		tdis(cx+3,y+4,trkmemo[nn],34,3);

		if( ke==27 ){break;}

		if(sh&4){
			int	ex;
			char	t[32];
			t[0]=ke;t[1]=0;
			ex=str_search("1234567890,./""*-+=\x0d",t);
			if( ex ){
				y=ex-1;pg=0;ke=13;
				break;
			}
		}

		if( ke==13 && tr_len[nn]>4 ){break;}

		if( ke==0x05 ){y--;if( y<0 ){y=25;}}
		if( ke==0x18 ){y++;if( y>25 ){y=0;}}

		if( ke==0x17 || ke==0x13 ){pg-=10;if(pg<0){pg=10;}goto redraw;}
		if( ke==0x12 || ke==0x04 ){pg+=10;if(pg>10){pg=0;}goto redraw;}

	}
	noed_cls_t();noed_cls();cons_md(1);
	if( ke==13 ){return(y+pg);}
	return(-1);
}
*/
/***************************/
void	fsel(char *fna,char *pth,int w)
{
	int	a=0,nm=0,p,y,cc,gy,xx,cx,gx;
	char	tmp0[256],exts[16];
	DIR	dir[512];
	int	cdirp[32],cdiry[32],cdirc;

	cc=w>>8;w=w&0xff;es=0;
	if(w<64){xx=319;if(w<6||w>8){xx=0;}}else{xx=335+8;if(w>=64+26){xx=319;}}
	if((cc&2)==0){cx=44;gx=352;}else{cx=2;gx=16;xx=0;}

	fill(xx,544,gx+432,1023,0);
	box(gx-1,512+92,gx+401,512+484,14,65535);
	box(gx+1,512+152,gx+399,512+183,14,65535);
	box(gx+1,512+185,gx+399,512+456,14,65535);
	fill(gx-1,512+92,gx+401,512+150,14);fill(gx-1,512+458,gx+401,512+484,14);

	if(w<6){
		box(7+8,168+64+512,248+8,375+64+512,14,65535);
		box(9+8,170+64+512,246+8,373+64+512,14,65535);
		sdis(2,153+64+512," MEMO",30,15,14);
	}
	gy=scyp;home2(512);

	strcpy(tmp0," FILE SELECTER ");
	if(w<64){
		if((cc&1)==0){strcpy(tmp0," LOAD ");}else{strcpy(tmp0," SAVE ");}
		strcat(tmp0,fsel_title[w]);strcat(tmp0," FILE ");
		strcpy(exts,fsel_ext[w]);
	}else{
		strcpy(exts,chcom_c[w&0x3f][1]);
	}

	sdis(cx,586,tmp0,strlen(tmp0),15,8);
	if(w<6){sdis(4,560," MUSIC TITLE :",79,15,8);}

	B_LOCATE(cx+1,6);B_PRINT("Path = ");B_LOCATE(cx+10,8);B_PRINT("K Byte Free");
	B_LOCATE(cx+22,8);B_PRINT("(");B_PRINT(exts);B_PRINT(")");
	B_LOCATE(cx+35,8);B_PRINT("files[  0/  0]");

	while(1){
reinp:
		if(drvchk(pth)>=0){
			int	i;
			i=strlen(pth);if(i>2 && pth[2]!='\\'){pth[2]='\\';pth[3]=0;}

			while(fdir(pth,exts,dir,-1)<=-3){
				if(strlen(pth)<=3){break;}
				path_down(pth);
			}
		}

		if(w<6){memo_disp();}
		B_COLOR(3);
/*		B_LOCATE(cx+1,29);B_PRINT("          [↓]セレクタ  [ESC]取り消し  [RET]決定");*/
		B_LOCATE(cx+1,29);B_PRINT("                  �Zレクタ  �謔闖ﾁし  ��定");

		frees(pth,cx);

		B_LOCATE(cx+1,10);sinput(fna,0x200+48);spcut(fna);str_change(fna);

		if( es==1 ){break;}
		if( ecode==13 && fna[0]!=0){
fsdirp[w]=0;fsdiry[w]=0;/*check*/
drvmove:
			if(str_search(fna,":")>2||fna[0]==':'){goto reinp;}

			if( fna[1]==':'){
				if(drvchk(fna)<0){nm=0;goto reinp;}

				strcpy(pth,fna);pth[2]=0;strcpy(fna,&fna[2]);
				strcat(pth,"\\");CURDIR(drv_code(pth),tmp0);
				if(tmp0[0]!=0){strcat(pth,tmp0);strcat(pth,"\\");}
			}

			if(fna[0]=='.'){
				if(fna[1]=='.'){
					if(fna[2]=='\\'||fna[2]==0){
						path_down(pth);
						if(fna[2]!=0 && fna[3]!=0){strcpy(fna,&fna[3]);}else{fna[0]=0;}
					}else{goto reinp;}
				}else{goto reinp;}
			}

			if( fna[0]=='\\' ){pth[3]=0;}

			strcpy(tmp0,pth);
			if( fna[0]=='\\'){strcat(tmp0,&fna[1]);}else{strcat(tmp0,fna);}

			if((CHMOD(tmp0,-1)&0x18)==0x10){
				strcpy(pth,tmp0);fna[0]=0;
				if( str_last(pth)!='\\' ){strcat(pth,"\\");}
			}

			if( str_last(fna)!='\\' ){
				if( strlen(fna)>2 && str_search(fna,"\\")>0 ){
					if(fna[0]=='\\'){
						unsigned char tmp[128];
						strcpy(tmp,pth);tmp[2]=0;
						strcat(tmp,fna);strcpy(fna,tmp);
					}
					path_cut(pth,fna);
					break;
				}
				if(fna[0]!=0){break;}
			}else{
				if(fna[0]!='\\'){strcat(pth,"\\");}
				strcat(pth,fna);
				fna[0]=0;
			}
		}

		cdirc=0;
resel:
		p=0;y=0;
		p=fsdirp[w];y=fsdiry[w];/*check*/
resel2:
		a=0;msg_clr();
		if(drvchk(pth)<0){nm=0;goto reinp;}
		nm=fdir(pth,exts,dir,w);

		if( nm==0 || nm==-2 ){nm=0;/*msg("ファイルがありません。");goto reinp;*/}
		if( nm==-3         ){msg("ディレクトリがありません。");goto reinp;}
		if( nm==-13        ){msg("ディレクトリ名が異常です。");goto reinp;}
		if( nm<0 && nm!=-2 ){msg("err.");B_PRINT(fstr(nm,4));goto reinp;}

/*		B_LOCATE(cx+1,29);B_PRINT("[→]/[←]ドライブ移動   ");*/
		B_LOCATE(cx+1,29);B_PRINT("��/�hライブ移動  ��/�I択");
		frees(pth,cx);B_LOCATE(cx+45,8);B_PRINT(fstr(nm,3));

		if(p+y>=nm){p=0;y=0;}/*check*/
		while(1){
			int	n,i;
			dir_disp(dir,nm,p,cx,0,16);

 			while( 1){
				n=y+p;
				B_LOCATE(cx+41,8);B_PRINT(fstr(n+1,3));

				if( dir[n].far&0x10 ){i=2;}else{i=3;}
				tcur(cx+1,y+12,22,i);
				a=inkey2();if(a>='a' && a<='z'){a=a-32;}
				tcur(cx+1,y+12,22,i);

				if(w<2 && (dir[n].far&0x20)){
					if(a==32){
						memo_load(pth,dir[n].name);
					}
					if(a=='A'){
						int i;
						memo_load(pth,dir[n].name);
						for(i=0 ;i<32;i++){
							strcpy(rhyna[i],sread(0x206+i*16,14));
							rhyno[i][0]=hed[0x206+i*16+14];rhyno[i][1]=hed[0x206+i*16+15];
						}
					}
					if(a=='U'){
						int i;
						memo_load(pth,dir[n].name);
						for(i=0;i<8;i++){uex_read(0x406,i);}
					}
					if(a=='T'){
						memo_load(pth,dir[n].name);
						strcpy(mtitle,sread(0x20,64));
						rcp_dat2(0,0);
					}
					if(a=='M'){
						int i;
						memo_load(pth,dir[n].name);
						for(i=0 ;i<12;i++){strcpy(memo[i],sread(0x60+i*28,28));}
					}
				}
				if( a==13 ){es= 0;break;}
				if( a==27 ){es= 1;break;}
				/*if( a==0x0b ){p=0;y=0;break;}*/
				if( a==0x07 ){a=27;break;}

				if( a==0x05 ){
					if( p==0 && y==0 ){a=27;break;}
					y--;if( y<0 ){y=0;if( p>0 ){p--;
						t_scrw(cx,12*16,50,15*16,cx,13*16);
						dir_disp(dir,nm,p,cx,0,1);}}
				}
				if( a==0x18 && n+1<nm){
					y++;if( y>15 ){y=15;if( p+16<nm ){p++;
						t_scrw(cx,13*16,50,15*16,cx,12*16);
						dir_disp(dir,nm,p,cx,15,1);}}
				}
				if( a==0x17 ){if( p>0 ){p-=16;if(p<0){p=0;}break;}else{y=0;}}

				if( a==0x12 ){if( p+16<nm ){p+=16;if(p+16>nm){p=nm-16;}
						if(p+y>=nm){y=(nm-1)-p;}break;
					}else{y=(nm-1)-p;if(y<0){y=0;}}}


				if(a==0x13 || a==4){
					int drn,dro;
					drn=drv_code(pth);dro=drn;
redrv:
					if(a==4){
						drn++;if(drn>26){drn=1;}
					}else{
						drn--;if(drn<1){drn=26;}
					}
					fna[0]=drn+'A'-1;fna[1]=':';fna[2]=0;

					if(dro!=drn && drvchk2(fna)<0){nm=0;goto redrv;}
					nm=0;goto drvmove;
				}
				if( a==0x15 || a==0x8 ){
					path_down(pth);
					if(cdirc>0){cdirc--;p=cdirp[cdirc];y=cdiry[cdirc];goto resel2;}
					goto resel;
				}
				if( a=='\\' || a==0x0b ){
					pth[3]=0;cdirc=0;goto resel;
				}

			}
			if( a==27 ){break;}
			if( a==13 && n<nm ){
				if(dir[n].far&0x20){strcpy(fna,dir[n].name);
fsdirp[w]=p;fsdiry[w]=y;/*check*/
					break;
				}

				if(dir[n].name[0]=='.'){
					if(dir[n].name[1]=='.'){
						path_down(pth);
						if(cdirc>0){cdirc--;p=cdirp[cdirc];y=cdiry[cdirc];goto resel2;}
					}
				}else{
					strcat(pth,dir[n].name);strcat(pth,"\\");
					if(cdirc<32){cdirp[cdirc]=p;cdiry[cdirc]=y;cdirc++;}
				}
fsdirp[w]=0;fsdiry[w]=0;/*check*/
				goto resel;
			}
		}
		if(a==13){
			if(drvchk(pth)<0){goto reinp;}
			break;
		}
	}
	msg_clr();

	spcut(fna);if( fna[0]!=0 && str_search(fna,".")==0){
		char etmp[256];
		strcpy(etmp,exts);etmp[3]=0;
		strmfe(fna,fna,etmp);
	}
	if(es==0){strcpy(tmp0,pth);strcat(tmp0,fna);
		if((cc&1)!=0){
			if((DRVCTRL(0,drv_code(pth))&8)!=0){
				msg("書き込みが禁止されています。");goto reinp;}
			if(file_check(tmp0,1)!=0){es=1;}else{file_back(tmp0);}
		}else{
			if(file_check(tmp0,0)==0){goto reinp;}
		}
	}
	txerase((cx+1)*8,6*16,50*8,24*16);if(w<6){C_CLS_AL();}
	home2(gy);fill(xx,544,gx+431,1023,0);

}

/***************************/
void	str_change(char *s)
{
	char	*ss=s;
	int j;

	while(*s!=0){
		if(*s=='/'){*s='\\';}
		s++;
	}

	while(1){
		j=str_search(ss,"\\\\");if(j==0){break;}
		strcpy(&ss[j-1],&ss[j]);
	}
}

/***************************/
void	path_cut(char *fpath,char *fname)
{
	int	j;
	char	tmp0[128];

	fpath[0]=0;
	while(1){
		j=str_search(fname,"\\");if(j==0){j=str_search(fname,":");if(j==0){break;}}
		strcpy(tmp0,fname);tmp0[j]=0;strcat(fpath,tmp0);strcpy(fname,&fname[j]);
	}
}

/***************************/
void	path_down(char *pth)
{
	int	i;

	i=strlen(pth)-1;
	while(i>2){
		if(pth[i-1]=='\\'){break;}
		i--;
	}
	if(i>2){pth[i]=0;}
}

/***************************/
void	path_set(char *pth)
{
	char	tmp0[128],tmp2[128];

	if( pth[0]==0 || knck(pth,0) || pth[1]!=':' ){
		strcpy(tmp0,pth);strcpy(pth," :");
		pth[0]=CURDRV()+'A';strcat(pth,tmp0);
	}
	if( pth[2]==0 ){
		strcat(pth,"\\");CURDIR(drv_code(pth),tmp2);
		if(tmp2[0]!=0){strcat(pth,tmp2);}
	}
	if(str_last(pth)!='\\'){strcat(pth,"\\");}
}

/***************************/
inline int	file_check(char *fna,int cc)
{
	FILE	*fp;

	if((cc&1)!=0 && (cnfflag&0x0002)==0){return(0);}
	if(!(fp= fopen(fna,"rb"))){
		if((cc&1)==0){msg("ファイルがありません。");}
		return(0);}
	fclose(fp);

	if((cc&1)!=0){
		msg("既に存在するファイルに上書きしますか");
		if(yn_sel()>0){msg_clr();return(0);}

		msg("ファイルの書き込みを中止します。");
	}
	return(-1);
}

/***************************/
inline void	file_back(char *fna)
{
	FILE	*fp;
	char	fnb[256];

	if((cnfflag&0x0004)==0){return;}
	if(!(fp= fopen(fna,"rb"))){return;}
	fclose(fp);

	strcpy(fnb,fna);fnb[strlen(fnb)-3]='$';
	DELETE(fnb);RENAME(fna,fnb);
}

/***************************/
int	drvchk(char *pth)
{
	int	a;

	a=DRVCTRL(0,drv_code(pth));

	if(a<0){     msg("ドライブ指定が無効です。");return -2;}
	if((a&2)==0){msg("ディスクが入っていません。");return -1;}
	if((a&4)!=0){msg("ディスクの準備が出来ていません。");return -1;}

	return 0;
}

int	drvchk2(char *pth)
{
	int	a;

	a=DRVCTRL(0,drv_code(pth));

	if(a<0 || (a&2)==0 || (a&4)!=0){return -1;}
	return 0;
}

/***************************/
int	drv_code(char *st)
{
	int	a;

	a=st[0];
	if(a>='a'&& a<='z'){a=a-32;}
	if(a<'A'|| a>'Z'){return 0;}
	a=a-'A'+1;return a;
}

/***************************/
void	frees(char *pth,int cx)
{
	int	a;
	char	tmp0[256],tmp1[256];

	strcpy(tmp0,pth);tmp1[0]=0;
	if(strlen(pth)>42){strcpy(tmp1,&tmp0[42]);tmp0[42]=0;tmp1[42]=0;}
	tdis(cx+8,6,tmp0,43,3);tdis(cx+8,7,tmp1,43,3);

	if(drvchk(pth)<0){a=0;}else{a=dskf(drv_code(pth))>>10;}
	B_LOCATE(cx+2,8);B_PRINT(fstr(a,8));
	CHDIR(pth);
}

/***************************/
inline char	*dirs(char *s)
{
	int	a;
	static char	name[24];
	char	ext[8];

	ext[0]=0;strcpy(name,s);
	a =str_search(name,".");
	a+=str_search(&name[a],".");
	a+=str_search(&name[a],".");

	if(a>0 && name[0]!='.'){strcpy(ext,&name[a-1]);name[a-1]=0;}

	strcpy(name,spadd(name,18));strcat(name,spadd(ext,4));

	return(name);
}

/***************************/
void	dir_disp(DIR *dir,int nm,int p,int cx,int sp,int ln)
{
	int	sta,i,j;
	char	tmp0[128];
	char	tmp1[128];

	cx++;
	txerase((cx)*8,(12+sp)*16,48*8,ln*16);

	sta=SUPER(0);
	for(i=sp ;i< sp+ln;i++){
		int nn=p+i;if( nn>=nm ){break;}
		strcpy(tmp0,dirs(dir[nn].name));
		B_LOCATE(cx,i+12);
		if(dir[nn].far&0x10){
			strcat(tmp0,"  <dir> ");
			H_PRINT2("",tmp0,"");tmp0[0]=0;
		}else{
			strcat(tmp0,fstr(dir[nn].fln,8));
		}
/*
  date >> 9) & 0x7f) + 80      |* year  *|
  date >> 5) & 0x0f            |* month *|
  date & 0x1f)                 |* day   *|
  time >> 11) & 0x1f           |* hour  *|
  time >>  5) & 0x3f           |* min   *|
  time & 0x1f) * 2             |* hour  *|
*/
		strcat(tmp0," ");j=dir[nn].fdt;strcpy(tmp1,dtasc(j));
		if(tmp1[0]==0){strcpy(tmp1,fstr(((j>>9)&0x7f)+80,2));strcat(tmp1,"-00-00");}
		strcat(tmp0,tmp1);strcat(tmp0," ");strcat(tmp0,tmasc(dir[nn].ftm));
		H_PRINT2(tmp0,"","");
	}
	SUPER(sta);
}

/***************************/
int	fdir(char *pth,char *exts,DIR *dir,int w)
{
	struct FILBUF	*buffer,buf;
	DIR	p;

	int	a,i,j,nm=0,atr=0x30;
	char	file[256],tmp0[256],tmp1[256];

	buffer = &buf;
	strcpy(file,pth);strcat(file,"*.*");strcpy(tmp0,".");
	if(w>=0){strcat(tmp0,exts);}

	a=FILES(buffer,file,atr);if( a!=0 ){return(a);}
	if(w<0){return(0);}
	do{
		int as;
		strcpy(tmp1,buffer->name);jstrupr(tmp1);

		if(tmp1[0]!='.' || tmp1[1]!=0){

			as =str_search(tmp1,".");
			as+=str_search(&tmp1[as],".");
			as+=str_search(&tmp1[as],".");

			if(as>0){strcpy(tmp1,spadd(&tmp1[as],3));}else{tmp1[0]=0;}

			if( buffer->atr&0x10 || (buffer->atr&0x20 && str_search(tmp0,tmp1)>0) ){
				if(nm>=512){msg("ファイルが多すぎます。");break;}

				strcpy(dir[nm].name,buffer->name);dir[nm].far=buffer->atr;
				dir[nm].fln=buffer->filelen;dir[nm].fdt=buffer->date;
				dir[nm].ftm=buffer->time;
				nm++;
			}
		}
	}while(NFILES(buffer) >= 0);

	if((cnfflag&0x0001)!=0 && nm>1){
		for(i=0;i<nm-1;i++){
			for(j=i+1;j<nm;j++){
				if( dir[i].far==dir[j].far ){
					if( strcmp(dir[i].name,dir[j].name)>0 ){
						p=dir[i];dir[i]=dir[j];dir[j]=p;
					}
				}else{
					if( dir[i].far>dir[j].far ){
						p=dir[i];dir[i]=dir[j];dir[j]=p;
					}
				}
			}
		}
	}
	return(nm);
}

/***************************/
inline void	memo_disp()
{
	int	i;

	txerase(2*8,12*16,34*8+7,12*16+16);

	tdis(18,3,mtitle,65,3);
	for(i=0;i<12;i++){tdis(3,15+i,memo[i],28,3);}
}

/***************************/
inline void	memo_load(char *pth,char *fna)
{
	int	i;
	char	tmp0[128];
	FILE	*fp;

	tdis(2,12,"FILE NAME:",10,1);tdis(12,12,fna,22,3);

	strcpy(tmp0,pth);strcat(tmp0,fna);
	if(fp= fopen(tmp0,"rb")){
		fread(hed,1,1414,fp);fclose(fp);
	}else{
		for(i=0;i<1414;i++){hed[i]=0;}
	}
	tdis(18,3,sread(0x20,64),65,3);
	for(i=0;i<12;i++){tdis(3,15+i,sread(0x60+i*28,28),28,3);}
}

/***************************/

