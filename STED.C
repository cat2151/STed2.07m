/*
 * MIDI Music Composer STed v2.07 (c)1996/11/17 by TURBO
 *      MIDI Sequencer
 *
 * v2.06f 95/04/18 YUI-NET‚ÌSYSOP(‚ä‚¢H–[)‚³‚ñ‚Éƒ\[ƒXŒöŠJ
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
#include	"graph.h"
#include	"interupt.h"
#include	"sted.h"


static char	menu1[18][9]={
	"LOAD    ","SAVE    ","EDIT&SET","PLAY    ","RECORD  ","FILTER  ",
	"TIME&KEY","TITLE   ","MEMO    ","PART ASS","RHY ASSI","USER EXC",
	"CM64 CON","SC55 CON","OPTION  ","UNIT SEL","INIT    ","EXIT    "};
/*
static char	menu2[18][9]={
	"BTM.LOAD","R36.SAVE","EDIT&SET","PANEL   ","MIDI MON","FILTER  ",
	"TIME&KEY","TITLE   ","MEMO    ","PART ASS","RHY ASSI","USER EXC",
	"CM64 CON","SC55 CON","OPTION  ","UNIT SEL","INIT    ","‹­§EXIT"};
*/

/*
static char	beats[21][5]={
	"1/2","2/2","2/3",
	"1/4","2/4","3/4","4/4","5/4","6/4",
	"1/8","2/8","3/8","4/8","5/8","6/8","7/8","8/8","9/8","10/8","11/8","12/8"};
*/

static int	tbase_tbl[]={48,60,80,96,120,160,192,240,384,480};
/*RCM V2.5G  : 48,60,80,96,120,160,240,480*/

static unsigned char	fnckey[12]={
	0x12,0x17,0x01,0x07,	0x05,0x13,0x04,0x18,	0x0C,0x16,0x0B,0x15};
/*	R.UP,R.DOWN,INS,DEL,	UP,LEFT,RIGHT,DOWN,	CLR,HELP,HOME,UNDO*/

unsigned char	fnckey0[712],fnckey1[712];
char	fonbuf[188*32];

char	env_opt[32][128];
char	rc_path[256];

int	cmdflagold,used_u=0;

void	rhy_as();
void	user_exc_as();
void	part_as();
void	fsel();
int	vinput();
void	sinput();
void	spcut();
int	klen();
int	knck();
void	str_change();
int	str_search();
int	str_last();
int	str_val();

void	trk_ed();
void	rtrk_ed();
void	inpmon();
char	*mdinport();
int	real_record();
int	scale_sel();
char	*scale_str();
void	cm64_con();
void	gsd_con();
void	cnfinit();
void	definit();
int	defload();
int	timload();
int	timsave();
int	gsdload();
void	timtra();
int	dload();
int	dsave();
int	bufset();
int	bufload();
void	dplay();
void	all_note_off();
void	cminit();
void	gsinit();
void	asin_init();
void	user_exc_init();
void	stgt_init();
void	filt_init();
void	rec_filter();
void	dinit();
void	poplay();
void	fnc_dis();
char	*fstr();
char	*nstr();
char	*chstr();
char	strch();
int	inkey2();
int	inkey3();
void	msg();
void	msg_clr();
void	vdis();
void	vdis2();
void	sdis();
void	sdis2();
void	tdis();
void	home2();
int	step_cluc2();
void	edfield();
void	H_INIT();
void	OnGraphic();
void	g_print();
char	*jstrupr();
int	drv_code();
void	key_vect_set();
void	key_vect_reset();
void	set_errvect();
void	rst_errvect();
void	keyclr();

void	path_cut();
void	path_set();

int	size_max();
int	size_change();
int	size_add();
void	size_ref();

void	com_sel();
void	load_sub();
void	save_sub();
void	trk_set();
void	trk_lin();
void	beat_set();
void	memo_ed();
int	option();
int	option2();
int	progmode();
void	key_pause();
int	yn_sel();
void	form();
void	form2();
void	form3();
void	sinit();
void	logo();
void	rcp_dat();
void	rcp_dat2();
void	trk_dat();
void	trksize();
void	tpl();
void	dpl();

void	fonload();
void	fonread();
void	fonset();
int	option_read();

int	exe();

void	end_proc();
void	ErrorTrap(void);

#define	Trap14		(0x002e)	/*TRAP	#14‚ÅŽQÆ‚·‚éƒxƒNƒ^*/

/********** program start **********/
void	main(int argc,char *argv[])
{
	int	i,f=0,lzf=0,work_size2=0,eoptc=0;
	char	tmp0[128],tmp1[128],cmdline[256],cmd[128];
	char	*p;
	FILE	*fp;

	rcd_check();H_INIT();keyclr();

	ErrorTrap_Old=NULL;ErrFlag=0;

	TRACK_SIZE=64;buff_max=256*36*1024;
	rcp_path[0]=0;prt_path[0]=0;trk_path[0]=0;
	rcp_file[0]=0;prt_file[0]=0;trk_file[0]=0;
	def_path[0]=0;def_file[0]=0;
	fon_path[0]=0;hlp_path[0]=0;rc_path[0]=0;
	card_no[0]=0;

	track=0;poft=0;rcpf=0;rcplen=0;cpadd=0;cplen=0;cpleng=0;
	cmdflag=0;cnfflag=0;mdlflag=0;

	lcbuf[0]=255;rlcbuf[0]=255;
	btrack=0;rec_getch=0;rec_putmd=1;

	cnfinit();

	if ((p=getenv("RCP_DIR"))==(char*)0) p=getenv("rcp_dir");
	if (p!=(char*)0){strcpy(rcp_path,p);}

	if ((p=getenv("PRT_DIR"))==(char*)0) p=getenv("prt_dir");
	if (p!=(char*)0){strcpy(prt_path,p);}

	if ((p=getenv("RC_PATH"))==(char*)0) p=getenv("rc_path");
	if (p!=(char*)0){strcpy(rc_path,p);}

	if ((p=getenv("STED2"))==(char*)0) p=getenv("sted2");
	if (p!=(char*)0){strcpy(cmdline,p);eoptc=option_read(cmdline);}

	B_PRINT("X68k MIDI Music Composer STed v2.07m Copyright 1990-97 TURBO\r\n");
/*	B_PRINT("X68k MIDI Sequencer STed v2.07m Copyright 1990-97 TURBO\r\n");*/

	if( argc>1 ){
		for(i=1;i<argc;i++){if(eoptc<32){strcpy(env_opt[eoptc++],argv[i]);}}

		for(i=0;i<eoptc;i++){
			strcpy(cmd,env_opt[i]);

			if( cmd[0]=='-' ){
				switch(cmd[1]){

					case 'M':
					case 'm':TRACK_SIZE=str_val(&cmd[2]);break;

					case 'U':
					case 'u':buff_max=str_val(&cmd[2])*1024;
						used_u=1;break;

					case 'W':
					case 'w':work_size2=str_val(&cmd[2])*1024;break;

					case 'B':
					case 'b':card_no[0]=str_val(&cmd[2]);break;

					case 'D':
					case 'd':
						strcpy(rcp_path,&cmd[2]);
						if(rcp_path[0]==0){
							if(i+1<eoptc && env_opt[i+1][0]!='-'){
								i++;strcpy(rcp_path,env_opt[i]);}
						}
						break;

					case 'F':
					case 'f':
						strcpy(def_file,&cmd[2]);
						if(def_file[0]==0){
							if(i+1<eoptc && env_opt[i+1][0]!='-'){
								i++;strcpy(def_file,env_opt[i]);}
						}
						break;

					case 'P':
					case 'p':cmdflag|=0x8000;break;

					case 'R':
					case 'r':cmdflag|=0x0080;break;

					case 'L':
					case 'l':cmdflag|=0x0008;break;

					case 'S':
					case 's':cmdflag|=0x0010;break;

					case 'O':
					case 'o':cmdflag|=0x0200;break;

					default:f=1;
				}
			}else{
				strcpy(rcp_file,cmd);
			}
		}
	}

	if( f!=0 ){
		B_PRINT(
		"usage: STed2 [option...] [file name] (.RCP/.R36/.MDF)\r\n"
		"options\r\n"
		"\t-M<size>\t1ƒgƒ‰ƒbƒN‚ÌÅ‘å—e—Ê‚ðŽw’è‚·‚é\r\n"
		"\t-U<size>\tƒgƒ‰ƒbƒNƒoƒbƒtƒ@—e—Ê‚ðŽw’è‚·‚é\r\n"
		"\t-W<size>\tƒŒƒR[ƒfƒBƒ“ƒOƒoƒbƒtƒ@—e—Ê‚ðŽw’è‚·‚é\r\n"
		"\t-F<file>\t‰¹Œ¹’è‹`ƒtƒ@ƒCƒ‹‚ðŽw’è‚·‚é\r\n"
		"\t-B<num>\t\t‰¹F–¼•\Ž¦‚ÉŽg—p‚·‚éPCM CARD‚ðŽw’è‚·‚é\r\n"
		"\t-D<path>\tƒf[ƒ^ƒfƒBƒŒƒNƒgƒŠ‚ðŽw’è‚·‚é\r\n"
		"\t-R\t\tƒ|ƒCƒ“ƒgƒvƒŒƒC‚ðRCD‚Ì‹@”\‚ÅŽÀs‚·‚é\r\n"
		"\t-P\t\t‰‰‘tƒRƒ}ƒ“ƒh‚ÌƒfƒtƒHƒ‹ƒg‚ðƒpƒlƒ‹•\Ž¦‚É‚·‚é\r\n"
		"\t-L\t\t‹N“®Žž‚ÉRCDƒoƒbƒtƒ@‚Ìƒf[ƒ^‚ð“Ç‚Ýž‚Þ\r\n"
		"\t-S\t\tI—¹Žž‚ÉRCDƒoƒbƒtƒ@‚Öƒf[ƒ^‚ð“]‘—‚µ‚È‚¢\r\n\n"

/*		"\t-O\t\tƒ}ƒEƒX‚Ì§Œä‚ð‹ÖŽ~‚·‚é\r\n\n"*//*97-02-01*/

/*		"\tŠÂ‹«•Ï” STED2 ‚Ì“à—e‚ªƒRƒ}ƒ“ƒhƒ‰ƒCƒ“‚ÌÅ‰‚É’Ç‰Á‚³‚ê‚Ü‚·B\r\n\n"*/
		);
		exit(1);
	}

	if( (int)rcd==0 ) {
		B_PRINT("RCD ‚ªí’“‚µ‚Ä‚¢‚Ü‚¹‚ñB\r\n" );exit(1);
	}else{
		if( strcmp(rcd_version,"3.01")<0 || strcmp(rcd_version,"3.09")>0 ){
			B_PRINT("–¢‘Î‰ž‚Ì RCD ‚ªí’“‚µ‚Ä‚¢‚Ü‚·B\r\n");
			exit(1);
		}
	}

	if( card_no[0]<0 || card_no[0]>16 || TRACK_SIZE<32 || TRACK_SIZE>256 ||
	    buff_max<64*1024 || buff_max>256*36*1024 ){
		B_PRINT("ƒIƒvƒVƒ‡ƒ“‚ÌŽw’è‚ª–³Œø‚Å‚·B\r\n");exit(1);
	}

	TRACK_SIZE=TRACK_SIZE*1024-48;buff_size=TRACK_SIZE*36;
	if(buff_size>buff_max){buff_size=buff_max;}

	work_size=TRACK_SIZE+32*1024;if(work_size<work_size2){work_size=work_size2;}

	cpbuf=(unsigned char *)MALLOC(TRACK_SIZE);
	rcpbuf=(unsigned char *)MALLOC(TRACK_SIZE);

	dat=(unsigned char *)MALLOC(work_size);
	dat2=(unsigned char *)MALLOC(work_size);
	if((int)dat>=0x81000000){
		B_PRINT("ƒgƒ‰ƒbƒNƒoƒbƒtƒ@‚ªŠm•Ûo—ˆ‚Ü‚¹‚ñB\r\n");exit(1);}

realc:
	trk[0]=(unsigned char *)MALLOC(buff_size);
	if((int)trk[0]>=0x81000000){
		if(!used_u){
			buff_size=((int)trk[0])&0xfffffc;if(buff_size>=64*1024){goto realc;}
		}
		B_PRINT("ƒgƒ‰ƒbƒNƒoƒbƒtƒ@‚ªŠm•Ûo—ˆ‚Ü‚¹‚ñB\r\n");exit(1);}

	buff_max=buff_size;

	if(TGUSEMD(0,-1)==1){
		B_PRINT("G-RAM‚ªSYSTEM‚ÅŽg—p’†‚Å‚·B‰Šú‰»‚µ‚Ä‚à‚¢‚¢‚Å‚·‚©H(Y/N)");
		while(1){
			i=inkey2();
			if(i=='Y'||i=='y'){break;}
			if(i=='N'||i=='n'){B_PRINT("’†Ž~‚µ‚Ü‚·B\r\n");exit(1);}
		}
	}

	str_change(rcp_path);str_change(prt_path);str_change(trk_path);
	str_change(rcp_file);str_change(def_file);str_change(rc_path);

	if( rcp_file[0]!=0 ){
		if(str_search(rcp_file,".MDF")>0 || str_search(rcp_file,".mdf")>0){
			lzf=1;
		}else{
			if(str_search(rcp_file,".")==0){
/*
				strmfe(rcp_file,rcp_file,'DEF");
				if(!(fp= fopen(rcp_file,"rb"))){
					strmfe(rcp_file,rcp_file,'DFB");
					if(!(fp= fopen(rcp_file,"rb"))){
*/
						strmfe(rcp_file,rcp_file,"MDF");
						if(!(fp= fopen(rcp_file,"rb"))){
							strmfe(rcp_file,rcp_file,"R36");
							if(!(fp= fopen(rcp_file,"rb"))){
								strmfe(rcp_file,rcp_file,"RCP");
							}else{
								fclose(fp);
							}
						}else{
							lzf=1;
							fclose(fp);
						}
/*
					}else{
						strcpy(def_file,rcp_file);
						rcp_file[0]=0;
						fclose(fp);
					}
				}else{
					strcpy(def_file,rcp_file);
					rcp_file[0]=0;
					fclose(fp);
				}
*/
			}
		}

		if(!(fp= fopen(rcp_file,"rb"))){
			B_PRINT(rcp_file);
			B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");exit(1);
		}
		fclose(fp);
		path_cut(rcp_path,rcp_file);
	}

	definit();
	strmfe(tmp0,argv[0],"cnf");
	if(defload(tmp0,2)!=0){
		B_PRINT(tmp0);B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");exit(1);}

	strmfe(tmp0,argv[0],"cf2");defload(tmp0,2);
/*		1997-07-20	STed2.cf2‚ª‚ ‚ê‚Î‚»‚ê‚à“Ç‚Ýž‚Þ‚æ‚¤‚É‚µ‚½*/

	if(tm_lag==0){tm_lag=8000;}
	cmdflagold=cmdflag;

	stgt_init();filt_init();

	if(def_file[0]==0){strcpy(def_file,argv[0]);strmfe(def_file,def_file,"def");}
	str_change(def_file);

{
	int lvl=0;
	if(str_search(def_file,".")==0){lvl=2;}
loop:
	if(lvl==2){strmfe(def_file,def_file,"dfb");}
	if(lvl==1){strmfe(def_file,def_file,"def");}

	i=defload(def_file,0);
	if(i!=0){
		strcpy(tmp0,def_path);strcat(tmp0,def_file);

		i=defload(tmp0,0);
		if(i!=0){
			strcpy(tmp1,argv[0]);path_cut(tmp0,tmp1);
			strcat(tmp0,def_file);

			i=defload(tmp0,0);
			if(i!=0){
				if(lvl==2){lvl--;goto loop;}
				if(i==-1){B_PRINT(def_file);B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");}
				exit(1);}
		}
		strcpy(def_file,tmp0);
	}else{
		strcpy(tmp1,def_file);path_cut(tmp0,tmp1);
		if(tmp0[0]==0){def_file[0]=0;path_set(def_file);strcat(def_file,tmp1);}
	}
}
	path_cut(def_path,def_file);

	card_name_get();

	fonread(fonbuf);
	if(fon_path[0]==0){
		strmfe(fon_path,argv[0],"fon");
	}else{
		str_change(fon_path);
		if(str_last(fon_path)!='\\'){strcat(fon_path,"\\");}
		strcpy(tmp0,argv[0]);strmfe(tmp0,tmp0,"fon");
		path_cut(tmp1,tmp0);strcat(fon_path,tmp0);
	}
	fonload(fon_path);

	if(hlp_path[0]==0){
		strmfe(hlp_path,argv[0],"hlp");
	}else{
		str_change(hlp_path);
		if(str_last(hlp_path)!='\\'){strcat(hlp_path,"\\");}
		strcpy(tmp0,argv[0]);strmfe(tmp0,tmp0,"hlp");
		path_cut(tmp1,tmp0);strcat(hlp_path,tmp0);
	}


	for(i=0;i<12;i++){fnckey1[0x280+i*6]=fnckey[i];}
	FNCKEYGT(0,fnckey0);FNCKEYST(0,fnckey1);

	path_set(rcp_path);path_set(def_path);

	if(trk_path[0]==0){
		strcpy(trk_path,prt_path);
		if(prt_path[0]==0){strcpy(trk_path,rcp_path);}}
	if(prt_path[0]==0){strcpy(prt_path,rcp_path);}

	if(str_last(rcp_path)!='\\'){strcat(rcp_path,"\\");}
	if(str_last(prt_path)!='\\'){strcat(prt_path,"\\");}
	if(str_last(trk_path)!='\\'){strcat(trk_path,"\\");}
	if(str_last(def_path)!='\\'){strcat(def_path,"\\");}
	if(rc_path[0]!=0 && str_last(rc_path)!='\\'){strcat(rc_path,"\\");}

	dinit();
	if(lzf){
		char	tmp[128];int	ec;

		strcpy(tmp0,rcp_path);strcat(tmp0,rcp_file);
		strcpy(tmp,"lzm -x ");strcat(tmp,tmp0);ec=exe(tmp);
		if(ec<0){tmp[2]='z';ec=exe(tmp);}
		if(ec!=0 && ec<32){FNCKEYST(0,fnckey0);fonset(fonbuf);exit(1);}
		cmdflag|=0x8;
	}

	set_errvect();

	C_CUROFF();LEDMOD(4,1);C_FNKMOD(3);sinit();/*MS_CUROF();*/

	if((cmdflag&0x8)!=0){
		if(bufload()!=0){bufset();}
	}else{
		bufset();
		if( rcp_file[0]!=0 ){
			strcpy(tmp0,rcp_path);strcat(tmp0,rcp_file);dload(tmp0,0);

			if(cm6_file[0]!=0){
				strcpy(tmp0,rcp_path);strcat(tmp0,cm6_file);
				if(timload(tmp0)!=0){
					msg(cm6_file);
					B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");cm6_file[0]=0;
				}
			}
			if(gsd_file[0]!=0){
				strcpy(tmp0,rcp_path);strcat(tmp0,gsd_file);
				if(gsdload(tmp0)!=0){
					msg(gsd_file);
					B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");gsd_file[0]=0;
				}
			}
		}
	}
	form();timtra();/*key_vect_set();*/com_sel();/*key_vect_reset();*/

	end_proc();exit(0);
}

/***************************/
void	end_proc()
{
	msg_clr();tpl(0);G_CLR_ON();CRTMOD(16);C_FNKMOD(0);C_CURON();LEDMOD(4,0);
	if(!(cmdflag&0x10)){bufset();}
	FNCKEYST(0,fnckey0);fonset(fonbuf);
	rst_errvect();exit(0);
}

/***************************/
void	set_errvect()
{
	if (NULL == (ErrorTrap_Old=(void *)B_INTVCS(Trap14,(int)*ErrorTrap)) ) {
		B_PRINT("TRAP14‚ªÄÝ’è‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½B’†Ž~‚µ‚Ü‚·B\r\n");
		exit(1);
	}
}

void	rst_errvect()
{

	if (ErrorTrap_Old != NULL) {
		B_INTVCS(Trap14,(int)ErrorTrap_Old);
	}
}

/***************************/
inline void	com_sel()
{
	int	ke,sh,cy=0,i;
	char	tmp0[128],tmp1[128],path2[128];

	for(i=0;i<18;i++){sdis(1,112+i*21,menu1[i],8,15,8);}

	C_WINDOW(0,31);

	while( 1){
		sdis(1,112+cy*21,menu1[cy],8,0,15);
		ke=inkey3();sh=ke>>8;ke=ke&0xff;
		if(ke>='a' && ke<='z'){ke-=32;}
		sdis(1,112+cy*21,menu1[cy],8,15,8);

		if( ke==0xe0 ){if((sh&1)==0){poplay(0,4);}else{poplay(0,5);}}
		if( ke==0xe1 ){if((sh&1)==0){poplay(0,1);}else{poplay(0,2);}}

		if( ke>='A' && ke<='Z' && (sh&4)!=0 ){option(ke-'A',0);ke=0;}
		if(ke=='L'){cy=0;ke=13;}
		if(ke=='S'){cy=1;ke=13;}
		if(ke=='E'){cy=2;ke=13;}
		if(ke=='P' || ke==0x10){cy=3;ke=13;}
		if(ke=='R'){cy=4;ke=13;}
		if(ke=='F'){cy=5;ke=13;}

		if(ke=='K'){cy=6;ke=13;}
		if(ke=='T'){cy=7;ke=13;}
		if(ke=='M'){cy=8;ke=13;}
		if(ke=='G'){cy=9;ke=13;}
		if(ke=='A'){cy=10;ke=13;}
		if(ke=='U'){cy=11;ke=13;}
		if(ke=='C'){cy=12;ke=13;if(sh&1){cy=13;}}

		if(ke=='O'){cy=14;ke=13;}
		if(ke=='D'){cy=15;ke=13;}
		if(ke=='I'){cy=16;ke=13;}
		if(ke=='X'){cy=17;ke=13;}

		if(ke=='Z'){timtra();}

		if( ke==13 || ke==32 ){
			/*if(sh&1){
				sdis(1,112+cy*21,menu2[cy],8,0,11);
			}else*/{
				sdis(1,112+cy*21,menu1[cy],8,0,11);
			}

			if( cy==0 ){load_sub(sh);}
			if( cy==1 ){save_sub(sh);}
			if( cy==2 ){trk_set(0);}
			if( cy==3 ){dplay(sh&3,0,0);}
			if( cy==4 ){if((sh & 1)!=0){inpmon();}else{trk_set(1);}}
			if( cy==5 ){rec_filter(0);}
			if( cy==6 ){beat_set();}
			if( cy==7 ){
				if(sh&1){
					msg("ƒ^ƒCƒgƒ‹‚ðV‹K“ü—Í‚µ‚Ü‚·‚©");
					if(yn_sel()>0){mtitle[0]=0;}
				}
				B_LOCATE(28,3);sinput(mtitle,64);rcp_dat2(0,0);}

			if( cy==8 ){memo_ed(sh);}
			if( cy==9 ){part_as();}
			if( cy==10 ){rhy_as();}
			if( cy==11 ){user_exc_as(0);}
			if( cy==12 ){cm64_con();}
			if( cy==13 ){gsd_con();}
			if( cy==14 ){option(-1,0);}

			if( cy==15 ){
				strcpy(tmp1,def_file);strcpy(path2,def_path);
				fsel(tmp1,path2,0x004);
				if( es==0 && tmp1[0]!=0){
					int n;
					strcpy(tmp0,path2);strcat(tmp0,tmp1);
					card_no[0]=0;card_no[1]=0;definit();
					cmdflag=cmdflagold;mdlflag=0;
					n=defload(tmp0,1);
					if(n==0){
						card_name_get();

						if(!(cnfflag&0x0008)){strcpy(def_path,path2);}
						strcpy(def_file,tmp1);
						if(cm6_file[0]==0){cminit();}
						if(gsd_file[0]==0){gsinit();}
						asin_init();if(rcp_file[0]==0){user_exc_init();}
						msg_clr();sdis2(30+9,0,module,22,15,8);
					}else{if(n==-1){msg("ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");}}
				}
			}

			if( cy==16 ){
				msg("ƒf[ƒ^‚ð‰Šú‰»‚µ‚Ü‚·‚©");
				if(yn_sel()>0){dinit();rcp_file[0]=0;timtra();form2();}
			}
			if( cy==17 ){
				if((sh & 1)!=0){end_proc();}
				msg("I—¹‚µ‚Ü‚·‚©");
				if(yn_sel()>0){end_proc();}
			}
			C_WINDOW(0,31);B_LOCATE(0,2);C_CLS_ED();
		}

		if( ke==5 || ke==0x3c ){cy--;if(cy<0){cy=17;}}
		if( ke==24 || ke==0x3e ){cy++;if(cy>17){cy=0;}}
	}
}

/***************************/
void	load_sub(int sh)
{
	char	tmp0[128],tmp1[128],path2[128];

	strcpy(tmp1,rcp_file);
	strcpy(path2,rcp_path);fsel(tmp1,path2,0x001);
	if( es==0 && tmp1[0]!=0){
		strcpy(tmp0,path2);strcat(tmp0,tmp1);
		msg("ƒf[ƒ^‚Ìƒ[ƒh’†‚Å‚·B");
		if(dload(tmp0,sh&1)==0 ){
			cminit();gsinit();
			strcpy(tmp0,path2);strcat(tmp0,cm6_file);
			if(cm6_file[0]!=0){
				if(timload(tmp0)!=0){
					msg(cm6_file);B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");
					cm6_file[0]=0;
				}
			}
			strcpy(tmp0,path2);strcat(tmp0,gsd_file);
			if(gsd_file[0]!=0){
				if(gsdload(tmp0)!=0){
					msg(gsd_file);B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");
					gsd_file[0]=0;
				}
			}
			if(!(cnfflag&0x0008)){strcpy(rcp_path,path2);}
			strcpy(rcp_file,tmp1);
			form2();timtra();msg_clr();
		}
	}
}

void	save_sub(int sh)
{
	char	tmp0[128],tmp1[128],path2[128];

	strcpy(tmp1,rcp_file);

	if(tmp1[0]==0 || (sh&1)==0){
		if(cnfflag&0x0040){sh=1-(sh&1);}
	}else{
		if(str_search(rcp_file,".R36")>0){
			strmfe(tmp1,tmp1,"RCP");sh=0;
		}else{
			strmfe(tmp1,tmp1,"R36");sh=1;
		}
	}

	strcpy(path2,rcp_path);fsel(tmp1,path2,0x100+(sh&1));
	if( es==0 && tmp1[0]!=0 ){
		strcpy(tmp0,path2);strcat(tmp0,tmp1);
		msg("ƒf[ƒ^‚ÌƒZ[ƒu’†‚Å‚·B");
		if(dsave(tmp0)==0){
			if(!(cnfflag&0x0008)){strcpy(rcp_path,path2);}
			strcpy(rcp_file,tmp1);
			msg_clr();
		}
	}
	sdis2(28,64,rcp_file,22,15,0);
}

/***************************/
inline void	memo_ed(int sh)
{
	int	ke,i,y=0;
	char	tmp[128];

	tmp[0]=0;

	if(sh&1){
redraw:
		msg("ƒƒ‚‚ð‰Šú‰»‚µ‚Ü‚·‚©");
		if(yn_sel()>0){for(i=0;i<12;i++){memo[i][0]=0;}}
	}
	msg_clr();

	fill(0,512+32,767,1023,0);
	box(263,152+512,504,375+512,14,65535);
	box(265,169+512,502,373+512,14,65535);
	sdis(33,152+512," MEMO",30,15,14);
	home2(512);
top:
	for(i=0;i<12;i++){tdis(34,11+i,memo[i],28,3);}

	while( 1){
		tdis(34,11+y,memo[y],28,11);
		ke=inkey2();
		tdis(34,11+y,memo[y],28,3);

		if( ke==0xc ){goto redraw;}
		if( ke==27 ){C_CLS_AL();home2(0);break;}
		if( ke==13 ){B_LOCATE(34,11+y);sinput(memo[y],28);}
		if( ke==0x05 ){y--;if( y<0 ){y=11;}}
		if( ke==0x18 ){y++;if( y>11 ){y=0;}}

		if( ke==7 ){
			strcpy(tmp,memo[y]);
			for(i=y;i<11;i++){
				strcpy(memo[i],memo[i+1]);}
			strcpy(memo[11],"");goto top;
		}
		if( ke==0x15 ){
			for(i=11;i>y;i--){
				strcpy(memo[i],memo[i-1]);}
			strcpy(memo[y],tmp);goto top;
		}
	}
}

/***************************/
inline int	option(int md,int sm)
{
	int	ke,i,y=0,ex=0,gy=scyp;
	int	cmd=0;
	char	path2[128],tmp0[128],tmp1[128],tmp2[128];

	B_CLR_AL();
	fill(0,544,767,1001,0);box(0,556,320,1001,14,65535);fill(1,556+17,8,1000,8);
	sdis(0,556," OPTION COMMAND",40,15,14);home2(512);

	for(i=0;i<26;i++){
		tmp0[0]=i+'A';tmp0[1]=32;tmp0[2]=0;strcat(tmp0,chcom_s[i][0]);
		tdis(0,4+i,tmp0,39,3);}

	if(md>=0){y=md;goto cexec;}

	while( 1){
top:
		md=-1;
		tdis(2,4+y,chcom_s[y][0],37,11);
		ke=inkey2();
		tdis(2,4+y,chcom_s[y][0],37,3);

		if( ke==27 ){break;}
		if( ke==0x05 ){y--;if( y<0 ){y=25;}}
		if( ke==0x18 ){y++;if( y>25 ){y=0;}}
		if(ke>='a' && ke<='z'){y=ke-'a';ke=13;}
		if(ke>='A' && ke<='Z'){y=ke-'A';ke=13;}

		if( (ke==13 || ke==32)&& chcom_s[y][1][0]!=0 ){
cexec:
			tdis(2,4+y,chcom_s[y][0],37,9);
			strcpy(tmp0,chcom_s[y][1]);
			strcpy(tmp2,chcom_c[y][0]);

			cmd=progmode(tmp2);

			if((cmd&512)!=0 && sm){
				msg("Main MenuˆÈŠO‚Å‚Í–³Œø‚Å‚·B");
				if(md<0){goto top;}
				break;
			}

			if((cmd&32)!=0 && chcom_c[y][1][0]!=0){
				tmp1[0]=0;
				strcpy(path2,rcp_path);fsel(tmp1,path2,y+64);
				if( es==0 && tmp1[0]!=0 ){
					strcat(tmp0," ");
					strcat(tmp0,path2);strcat(tmp0,tmp1);
				}else{tmp0[0]=0;}
			}
			if((cmd&64)!=0){
				if(rcp_file[0]!=0){
					strcpy(tmp1,rcp_file);
					if(chcom_c[y][1][0]!=0){
						strmfe(tmp1,tmp1,chcom_c[y][1]);
					}
					strcat(tmp0," ");
					strcat(tmp0,rcp_path);strcat(tmp0,tmp1);
				}else{tmp0[0]=0;}
			}

			if(tmp0[0]!=0){ex=paraexe(tmp0,0,0,sm,cmd);break;}
		}
	}
	B_CLR_AL();home2(gy);
	return 1;
}

/***************************/
inline int	option22(int md,int sm,int ad,int ew)
{
	int	ke,i,y=0,ex=0,gy=scyp,cx,gx;
	int	cmd=0;
	char	path2[128],tmp0[128],tmp1[128],tmp2[128];

	if(ew==0){cx=41+16-16;gx=327+128-128;}else{cx=1+16-2;gx=7+128-16;}

	cons_md(0);
redraw:
edit_scr=ew;noed_cls();edit_scr=0;

	box(gx,556,gx+320,1001,14,65535);fill(gx+1,556+17,gx+8,1000,8);
	sdis(cx,556," OPTION COMMAND",40,15,14);home2(512);

	for(i=0;i<26;i++){
		tmp0[0]=i+'A';tmp0[1]=32;tmp0[2]=0;strcat(tmp0,chcom_s[i+26][0]);
		tdis(cx,4+i,tmp0,39,3);}

	if(md>=0){y=md;goto cexec;}

	while( 1){
top:
		md=-1;
		tdis(cx+2,4+y,chcom_s[y+26][0],37,11);
		ke=inkey2();
		tdis(cx+2,4+y,chcom_s[y+26][0],37,3);

		if( ke==27 ){break;}
		if( ke==0x05 ){y--;if( y<0 ){y=25;}}
		if( ke==0x18 ){y++;if( y>25 ){y=0;}}
		if(ke>='a' && ke<='z'){y=ke-'a';ke=13;}
		if(ke>='A' && ke<='Z'){y=ke-'A';ke=13;}

/*		if( ke==0x17 || ke==0x13 ){pg-=26;if(pg<0){pg=26;}goto redraw;}
		if( ke==0x12 || ke==0x04 ){pg+=26;if(pg>26){pg=0;}goto redraw;}
*/
		if( (ke==13 || ke==32)&& chcom_s[y+26][1][0]!=0 ){
cexec:
			tdis(cx+2,4+y,chcom_s[y+26][0],37,9);
			strcpy(tmp0,chcom_s[y+26][1]);
			strcpy(tmp2,chcom_c[y+26][0]);

			cmd=progmode(tmp2);

			if((cmd&512)!=0 && sm){
				msg("Main MenuˆÈŠO‚Å‚Í–³Œø‚Å‚·B");
				if(md<0){goto top;}
				break;
			}

			if((cmd&32)!=0 && chcom_c[y+26][1][0]!=0){
				tmp1[0]=0;
				strcpy(path2,rcp_path);

edit_scr=ew;noed_cls_t();noed_cls();edit_scr=0;

			if(sm==2){fsel(tmp1,path2,y+64+(ew&1)*0x200+26);
			}else{fsel(tmp1,path2,y+64);}
				if( es==0 && tmp1[0]!=0 ){
					strcat(tmp0," ");
					strcat(tmp0,path2);strcat(tmp0,tmp1);
				}else{tmp0[0]=0;}
if(es!=0){goto redraw;}

			}
			if((cmd&64)!=0){
				if(rcp_file[0]!=0){
					strcpy(tmp1,rcp_file);
					if(chcom_c[y+26][1][0]!=0){
						strmfe(tmp1,tmp1,chcom_c[y+26][1]);
					}
					strcat(tmp0," ");
					strcat(tmp0,rcp_path);strcat(tmp0,tmp1);
				}else{tmp0[0]=0;}
			}

			if(tmp0[0]!=0){
edit_scr=ew;noed_cls_t();noed_cls();edit_scr=0;
ex=paraexe(tmp0,ad,ew,sm,cmd);break;}
else{
goto redraw;}
		}
	}

if(ke==27){edit_scr=ew;noed_cls_t();noed_cls();edit_scr=0;}

	cons_md(1);
	home2(gy);
	return ex;
}

/***************************/
inline int	option2(int md,int sm,int ad,int ew)
{
	int	y,ex=0,gy;
	int	cmd=0;
	char	path2[128],tmp0[128],tmp1[128],tmp2[128];
	gy=scyp;y=md+26;

	if( chcom_s[y][1][0]!=0 ){
		strcpy(tmp0,chcom_s[y][1]);
		strcpy(tmp2,chcom_c[y][0]);

		cmd=progmode(tmp2);

		if((cmd&512)!=0 && sm){
			msg("Main MenuˆÈŠO‚Å‚Í–³Œø‚Å‚·B");
			goto ext;
		}

		if((cmd&32)!=0 && chcom_c[y][1][0]!=0){
			tmp1[0]=0;
			strcpy(path2,rcp_path);

			if(sm==2){fsel(tmp1,path2,y+64+(ew&1)*0x200);
			}else{fsel(tmp1,path2,y+64);}

			if( es==0 && tmp1[0]!=0 ){
				strcat(tmp0," ");
				strcat(tmp0,path2);strcat(tmp0,tmp1);
			}else{tmp0[0]=0;}
		}

		if((cmd&64)!=0){
			if(rcp_file[0]!=0){
				strcpy(tmp1,rcp_file);
				if(chcom_c[y][1][0]!=0){
					strmfe(tmp1,tmp1,chcom_c[y][1]);
				}
				strcat(tmp0," ");
				strcat(tmp0,rcp_path);strcat(tmp0,tmp1);
			}else{tmp0[0]=0;}
		}

		if(tmp0[0]!=0){ex=paraexe(tmp0,ad,ew,sm,cmd);}
	}
ext:
	home2(gy);
	return(ex);
}

int	paraexe(char *tmp0,int ad,int ew,int sm,int cmd)
{
	int	a,i,ex,im;
	char	tmp1[256],tmp2[256];
	int	dlen=0,dadd=0,dlen2=0;

	if( (cplen<0 && cpadd!=ad) && sm==2){
		dadd=cpadd;
		dlen=abs(dadd-ad);if(dadd>ad ){dadd=ad;}
		dlen2=dlen;
	}else{dlen=tr_len[track];}

	tmp1[0]=0;

	i=0;
	while((a=tmp0[i++])!=0){
		if(a=='%'){
			a=tmp0[i++];if(a==0){break;}
			switch( a){
				case  't':strcat(tmp1,nstr(track+1));break;
				case  'm':strcat(tmp1,nstr(meas_no(ad)));break;

				case  'o':strcat(tmp1,nstr(ad+44));break;

				case  'u':strcat(tmp1,nstr(dadd+44));break;
				case  'd':strcat(tmp1,nstr(dadd+dlen+40));break;
				case  'l':strcat(tmp1,nstr(dlen2));break;

				case  'w':strcat(tmp1,nstr(ew));break;
				case  's':strcat(tmp1,nstr(sm));break;

				default:tmp2[0]=a;tmp2[1]=0;strcat(tmp1,tmp2);
			}
		}else{
			tmp2[0]=a;tmp2[1]=0;strcat(tmp1,tmp2);
		}
	}


	if((cmd&1)!=0){
		if((cmd&0x2000)==0){bufset();}else{bufset_trk(track);}
	}
	if((cmd&8)!=0){all_note_off(2);}

	C_CURON();C_FNKMOD(0);FNCKEYST(0,fnckey0);
	C_WINDOW(0,31);B_COLOR(3);B_LOCATE(0,0);

	if((cmd&0x400)==0){B_CLR_AL();dpl(0);tpl(0);}
	else{B_LOCATE(0,1);}

	if((cmd&0x800)!=0 && sm==2){if(grpmode){grpmode=0;/*noed_cls();*/}}

	im=(B_SFTSNS()&4096);
	ex=exe(tmp1);
	if(im){LEDMOD(4,1);}else{LEDMOD(4,0);}

	if(ex==0 && sm==2){cplen=0;}

	C_FNKMOD(3);fnc_dis(-1);FNCKEYST(0,fnckey1);
	if((cmd&16)!=0){(*rcd->end)();}
	if((cmd&128)!=0 || (ex!=0 && (cmd&0x400)==0)){key_pause();}
	B_COLOR(3);C_CUROFF();

	if((cmd&0x400)==0){B_CLR_AL();CRTMOD(16+256);OnGraphic();}
	dpl(1);tpl(1);
	if(sm<2){home2(0);}else{home2(512);}
	WINDOW(0,0,1023,1023);

	ex=0;if(point(0,0)!=8||(cmd&0x100)!=0){sinit();ex=1;}
	if((cmd&(2+4))!=0){
		poft=0;
		if((cmd&0x2000)==0){bufload();form2();
		}else{bufload_trk(track);trk_lin(track,-1,0);}
		ex=1;
		if((cmd&2)!=0){timtra();}
	}else{
		if(ex!=0){form();}
	}

	rcd->data_valid=0;
	return ex;
}

/***************************/
int	progmode(char *tmp2)
{
	int	a,i,cmd=0;

	jstrupr(tmp2);

	i=0;
	while((a=tmp2[i++])!=0){
		if(a=='B'){cmd|=0x0001;}	/*bufset*/
		if(a=='L'){cmd|=0x0002;}	/*bufread*/
		if(a=='N'){cmd|=0x0004;}	/*bufread non cm6/gsd*/
		if(a=='O'){cmd|=0x0008;}	/* ŽÀs‘O‚É‰‰‘t‚ð’âŽ~‚·‚é*/
		if(a=='E'){cmd|=0x0010;}	/* ŽÀsŒã‚É‰‰‘t‚ð’âŽ~‚·‚é*/
		if(a=='S'){cmd|=0x0020;}	/*select*/
		if(a=='F'){cmd|=0x0040;}	/*ext change*/
		if(a=='P'){cmd|=0x0080;}	/*pause*/
		if(a=='G'){cmd|=0x0100;}	/*rewrite*/
		if(a=='M'){cmd|=0x0200;}	/*main only*/
		if(a=='U'){cmd|=0x0400;}	/*not screen clr*/
		if(a=='R'){cmd|=0x0800;}	/*not edit window clr*/

		/*if(a=='C'){cmd|=0x1000;}*/	/*command.x*/
		if(a=='T'){cmd|=0x2000;}	/*trk data‚Ì‚Ýƒoƒbƒtƒ@ƒAƒNƒZƒX‚·‚é*/
	}
	return cmd;
}

/***************************/
void	key_pause()
{
	B_COLOR(1);B_LOCATE(0,31);B_PRINT("STed2‚É–ß‚è‚Ü‚·I[RET]ƒL[‚ð‰Ÿ‚µ‚Ä‰º‚³‚¢");
	while(inkey2()!=13){}
	B_COLOR(1);B_LOCATE(0,31);B_PRINT("                                        ");
}

/***************************/
int	yn_sel()
{
	int	ke,ec=0;

	B_PRINT("H(Y/N)");
	while(1){
		ke=inkey2();
		if(ke=='Y' || ke=='y'||ke==13){ec= 1;break;}
		if(ke=='N' || ke=='n'){ec=-1;break;}
		if(ke==27){break;}
	}
	msg_clr();
	return(ec);
}

/***************************/
void	form()
{
	rcp_dat();trk_dat();
}

/***************************/
void	form2()
{
	fill(102,43,767,485,0);form();
}

/***************************/
void	form3()
{
	fill(103,188,743,484,0);trk_dat();
}

/***************************/
void	sinit()
{
	int	i;

	CRTMOD(16);G_CLR_ON();WINDOW(0,0,1023,1023);home2(0);C_WINDOW(0,32);
	B_CLR_AL();dpl(1);tpl(1);logo(0);logo(512);sdis2(30+9,0,module,22,15,8);
	box(0,43,96,99,14,65535);
	for(i=0;i<18;i++){sdis(1,112+i*21,menu1[i],8,15,8);}
}

/***************************/
void	logo(int y)
{
	if(y<0){
		fill(496,512,767,512+31,0);
		fill(496,512,767,512+15,8);box(496,512,767,512+15,14,65535);
		y=510;
	}else{
		sdis2(0,y," FREE:",14,15,8);sdis2(15,y," USED:",14,15,8);
		sdis2(30,y," TR.: 1",8,15,8);
		fill(496,y,767,y+31,8);box(496,y,767,y+31,14,65535);
		symbol(496+5,y+18,comment,1,1,0,15,0);
		trksize(-2);
	}
/*	g_print(496+24,y+2,"STed v2.07  (c)1990-97 TURBO",15);*/

	g_print(496+13,y+3,"ëßëàëá ëäëåëîmëç1990-97ëèëéëêëë",0);
	g_print(496+12,y+2,"ëßëàëá ëäëåëîmëç1990-97ëèëéëêëë",15);

/*	g_print(496   ,y+2,"ëßëàëáëâëãëäëåëî ëç1990-97ëèëéëêëë",15);*/
/*	g_print(496+8 ,y+2,"ëßëàëáëâëãëäëåëî ëç1990-97 TURBO",15);*/
}
/* JIS-[7621] SJIS-[EB9F]
----------------------------------------------------------------
ëŸë ë¡ë¢ë£ë¤ë¥ë¦ë§ë¨ë©ëªë«ë¬ë­ë®ë¯ë°ë±ë²ë³ë´ëµë¶ë·ë¸ë¹ëºë»ë¼ë½ë¾

ë¿ëÀëÁëÂëÃëÄëÅëÆëÇëÈëÉëÊëËëÌëÍëÎëÏëÐëÑëÒëÓëÔëÕëÖë×ëØëÙëÚëÛëÜëÝëÞ
----------------------------------------------------------------
ëßëàëáëâëãëäëåëæëçëèëéëêëëëìëíëîëïëðëñëòëóëôëõëöë÷ëøëùëúëûëüì@ìA

ìBìCìDìEìFìGìHìIìJìKìLìMìNìOìPìQìRìSìTìUìVìWìXìYìZì[ì\ì]ì^ì_ì`ìa
----------------------------------------------------------------
ìbìcìdìeìfìgìhìiìjìkìlìmìnìoìpìqìrìsìtìuìvìwìxìyìzì{ì|ì}ì~ì€ìì‚

ìƒì„ì…ì†ì‡ìˆì‰ìŠì‹ìŒììŽììì‘ì’ì“ì”ì•ì–ì—ì˜ì™ìšì›ìœììž
----------------------------------------------------------------
*/

/***************************/
void	trk_dat()
{
	int	i;
static	short	xpos[7]={163,195,227,259,291,589,637};

	fill(103,189,120,484,8);
	for(i=0;i<7;i++){line(xpos[i],171,xpos[i],484,14,65535);}

	for(i=btrack;i<btrack+18;i++){trk_lin(i,-1,2);}
	trksize(-1);
}

/***************************/
void	rcp_dat()
{
	box(102,43,744,99-16,14,65535);
	sdis(14,48,"MUSIC TITLE  :",0,15,-1);sdis(14,64,"FILE NAME    :",0,15,-1);
	sdis(14+37,80-16,"/",0,15,-1);sdis(14+37+19,80-16,"/",0,15,-1);

	box(102,106-16,424,164,14,65535);box(432,106-16,744,164,14,65535);

	sdis(14, 96,"TEMPO    :",0,15,-1);	sdis(34, 96,"TIMEBASE :",0,15,-1);
	sdis(14,112,"PLAY BIAS:",0,15,-1);	/*midi thru*/
	sdis(14,128,"KEY  :",0,15,-1);		/*sync mode*/
	sdis(14,144,"BEAT  :    /",0,15,-1);	sdis(34,144,"MIDI IN :",9,15,-1);

	rcp_dat2(-1,0);

	box(102,171,744,485,14,65535);
	sdis(13,172,"TR MODE CH. ST+ K#+ RHY    TRACK MEMO                         STEP     TOTAL ST",80,15,14);
}

/***************************/
void	rcp_dat2(int n,int m)
{
	int	i,c1,c2;

	if(m==1){c1=0;c2=15;}else{c1=15;c2=0;}
	if(m==2){c2=-1;}

	if(n==0 || n<0){sdis2(28,48,mtitle,64,c1,c2);}
	if(n==1 || n<0){sdis2(28,64,rcp_file,22,c1,c2);}
	if(n==2 || n<0){sdis2(28+25,64,cm6_file,12,c1,c2);sdis2(28+44,64,gsd_file,16,c1,c2);}
	if(n==3 || n<0){vdis(25,96,mtempo,3,c1,c2);}
	if(n==4 || n<0){i=pbias;if(i>127){i=i-256;}vdis(25,112,i,3,c1,c2);}
	if(n==5 || n<0){sdis2(20,128,scale_str(bkey),13,c1,c2);}
	if(n==6 || n<0){vdis(22,144,beat1,2,c1,c2);vdis(26,144,beat2,2,c1,c2);}
	if(n==7 || n<0){vdis(45,96,tbase,3,c1,c2);}
	if(n==8 || n<0){sdis2(44,144,mdinport(),8,c1,c2);}
}

/***************************/
void	beat_set()
{
	int	ke,sh,ex,cx=0,i=0,mm=0;

	while(1){

		if(cx==0 && i==0){mm=3;}
		if(         i==1){mm=4;}
		if(         i==2){mm=5;}
		if(         i==3){mm=6;}
		if(cx==1 && i==0){mm=7;}

		rcp_dat2(mm,1);
		ke=inkey3();sh=ke>>8;ke=ke&0xff;
		rcp_dat2(mm,0);

		if( ke==27 ){break;}

		if( ke==13 ){
			if( mm==3 ){
				B_LOCATE(25,6);ex=vinput(mtempo,0x803);
				if(es==0 && ex>7 && ex<256 ){mtempo=ex;}
			}
			if( mm==4 ){
				ex=pbias;if(ex>127){ex=ex-256;}
				B_LOCATE(25,7);ex=vinput(ex,0x803);
				if(ex>=-36 && ex<=36){
					if(ex<0){ex=ex+256;}
					if(es==0 && ex>=0 && ex<=255){pbias=ex;}
				}
			}
			if( mm==5 ){
				sdis2(20,128,"",13,0,7);
				ex=scale_sel(20,8,bkey);
				if(ex>=0){bkey=ex;}
			}
			if( mm==6 ){
				B_LOCATE(22,9);ex=vinput(beat1,0x802);
				if(es==0 && ex>0 && ex<=32){beat1=ex;}
				B_LOCATE(26,9);ex=vinput(beat2,0x802);
				if(es==0 && ex>1 && ex<=8){beat2=ex;}
			}
			if( mm==7 ){
				B_LOCATE(45,6);ex=vinput(tbase,0x803);
				if(es==0 && ex>=48 && ex<=480){tbase=ex;}
			}
			B_LOCATE(0,2);C_CLS_ED();
		}

		if( ke>='0' && ke<='9' ){
			if( mm==3 ){
				B_LOCATE(25,6);ex=vinput(ke-'0',0x403);
				if(es==0 && ex>7 && ex<256 ){mtempo=ex;}
			}
			if( mm==4 ){
				ex=pbias;if(ex>127){ex=ex-256;}
				B_LOCATE(25,7);ex=vinput(ke-'0',0x403);
				if(ex>=-36 && ex<=36){
					if(ex<0){ex=ex+256;}
					if(es==0 && ex>=0 && ex<=255){pbias=ex;}
				}
			}
			if( mm==6 ){
				B_LOCATE(22,9);ex=vinput(ke-'0',0x402);
				if(es==0 && ex>0 && ex<=32){beat1=ex;}
				B_LOCATE(26,9);ex=vinput(beat2,0x802);
				if(es==0 && ex>1 && ex<=8){beat2=ex;}
			}
			if( mm==7 ){
				B_LOCATE(45,6);ex=vinput(ke-'0',0x403);
				if(es==0 && ex>=48 && ex<=480){tbase=ex;}
			}
			B_LOCATE(0,2);C_CLS_ED();
		}

		if( ke=='+' || ke=='>' || ke=='.' ){
			if( mm==3 ){
				if(sh&1){mtempo+=10;}else{mtempo++;}
				if(mtempo>255){mtempo=255;}}
			if( mm==4 ){
				ex=pbias;if(ex>127){ex=ex-256;}
				if(sh&1){ex+=12;}else{ex++;}
				if(ex<=36){
					if(ex<0){ex=ex+256;}
					pbias=ex;
				}
			}
			if( mm==5 ){bkey++;if(bkey>31){bkey=0;}}
			if( mm==7 ){
				int j;
				for(j=0;j<10;j++){
					if(tbase<tbase_tbl[j]){tbase=tbase_tbl[j];break;}
				}
			}
		}
		if( ke=='-' || ke=='<' || ke==',' ){
			if( mm==3 ){
				if(sh&1){mtempo-=10;}else{mtempo--;}
				if(mtempo<8){mtempo=8;}}
			if( mm==4 ){
				ex=pbias;if(ex>127){ex=ex-256;}
				if(sh&1){ex-=12;}else{ex--;}
				if(ex>=-36){
					if(ex<0){ex=ex+256;}
					pbias=ex;
				}
			}
			if( mm==5 ){bkey--;if(bkey<0){bkey=31;}}
			if( mm==7 ){
				int j;
				for(j=10-1;j>=0;j--){
					if(tbase>tbase_tbl[j]){tbase=tbase_tbl[j];break;}
				}
			}
		}

		if( ke==32 ){
			if( mm==3 ){mtempo=120;}
			if( mm==4 ){pbias=0;}
			if( mm==5 ){bkey=0;}
			if( mm==6 ){beat1=4;beat2=4;}
			if( mm==7 ){tbase=48;}
		}
		if( ke==0x0b ){i=0;}

		if( ke==0x05 ){i--;if(i<0){i=3;}}
		if( ke==0x18 ){i++;if(i>3){i=0;}}
		if( ke==0x13 ){cx--;if(cx<0){cx=1;}}
		if( ke==0x04 ){cx++;if(cx>1){cx=0;}}
	}
}

/***************************/
char	*mdinport()
{
static	char tmp0[64];

	if(rcd->rcd_type==4){strcpy(tmp0,"RS-232C");
	}else{
		if(rcd->rcd_type==3){strcpy(tmp0,"POLYPHON");
		}else{
			if(rcd->put_mode==2){
				if(rcd->rcd_type==0){strcpy(tmp0,"RS-232C");
				}else{strcpy(tmp0,"POLYPHON");}
			}else{strcpy(tmp0,"CZ-6BM1");}
		}
	}
	return tmp0;
}

/***************************/
void	dpl(int a)
{
	int	i;

	for(i=0;i<=15;i++){if(a==0){GPALET(i,0);}else{GPALET(i,palet_dat[i]);}}
}

/***************************/
void	tpl(int a)
{
	int	i;

	for(i=1;i<=3;i++){if(a==0){TPALET(i,-2);}else{TPALET(i,palet_dat[i]);}}
}

/***************************/
void	fonload(char *fi)
{
	FILE	*fp;

	if(!(fp= fopen(fi,"rb"))){
		B_PRINT(fi);B_PRINT(":ƒtƒ@ƒCƒ‹‚ªŒ©•t‚©‚è‚Ü‚¹‚ñB\r\n");exit(1);}
	fread(dat,1,12*1024,fp);fclose(fp);
	fonset(dat);
}

/***************************/
void	fonread(unsigned char *buf)
{
	int	i;

	struct	FNTBUF	*fntb,buff;
	fntb=&buff;

	for(i=0;i<94;i++){
		FNTGET(8,0x7621+i,fntb);memcpy(&buf[i*32      ],fntb->buffer,32);
		FNTGET(8,0x7721+i,fntb);memcpy(&buf[i*32+94*32],fntb->buffer,32);
	}
}

/***************************/
void	fonset(unsigned char *buf)
{
	int	i;

	for(i=0;i<94;i++){
		DEFCHR(8,0x7621+i,&buf[i*32      ]);
		DEFCHR(8,0x7721+i,&buf[i*32+94*32]);
	}
}

/***************************/
int	option_read(char *env_option)
{
	int	i,s,nn;

	strcat(env_option," ");i=0;s=0;nn=0;

	while(env_option[i]!=0){
		if(env_option[i]==32){
			if(s>0){
				env_opt[nn][s]=0;nn++;s=0;if(nn>31){break;}
			}
		}else{
			env_opt[nn][s]=env_option[i];s++;
		}
		i++;
	}
	return(nn);
}

/***************************/
int	exe(char *cl)
{
	int	ec,er,use;
	char	tmp0[128];

	rst_errvect();
	size_ref();

	if(used_u==0){
		use=buff_size-buff_free;
		SETBLOCK((int)trk[0],use);

		ec=-1;
		if(rc_path[0]!=0){strcpy(tmp0,rc_path);strcat(tmp0,cl);ec=child(tmp0);}
		if(ec<0){ec=child(cl);}

		buff_size=buff_max;
/*realc:*/
		er=SETBLOCK((int)trk[0],buff_size);
/*
		if(er>=0x81000000){
			msg("ƒgƒ‰ƒbƒNƒoƒbƒtƒ@‚ªÄŠm•Ûo—ˆ‚Ü‚¹‚ñB");
			buff_size=er&0xfffffc;
			if(er<0x82000000){goto realc;}
		}

		buff_free=buff_size-use;
*/
	}else{
		ec=child(cl);
	}
/*	trksize(-2);*/
	set_errvect();
	return(ec);
}

/***************************/
void	ErrorTrap(void)
{
	register int	ErrCode	asm("extern d7");

	if (((UWORD)ErrCode >=0x1000) && ((UWORD)ErrCode <= 0x7fff)) {
		switch ( (UBYTE)ErrCode ) {
			/*case 0x01:msg("–³Œø‚Èƒ†ƒjƒbƒg‚ðŽw’è‚µ‚Ü‚µ‚½B");break;
			case 0x02:msg("ƒhƒ‰ƒCƒu‚Ì€”õ‚ª‚Å‚«‚Ä‚¢‚Ü‚¹‚ñB");break;
			case 0x03:msg("–³Œø‚ÈƒRƒ}ƒ“ƒh‚ðŽw’è‚µ‚Ü‚µ‚½B");break;*/
			case 0x04:msg("CRCƒGƒ‰[");break;
			case 0x05:msg("ŠÇ——Ìˆæ‚ªˆÙí‚Å‚·B");break;
			case 0x06:msg("ƒV[ƒNƒGƒ‰[");break;
			case 0x07:msg("–³Œø‚ÈƒƒfƒBƒA‚Å‚·B");break;
			case 0x08:msg("ƒZƒNƒ^‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñB");break;
			case 0x0a:msg("‘‚«ž‚ÝƒGƒ‰[");break;
			case 0x0b:msg("“Ç‚Ýž‚ÝƒGƒ‰[");break;
			case 0x0d:msg("ƒ‰ƒCƒgƒvƒƒeƒNƒg‚³‚ê‚Ä‚Ü‚·B");break;
			case 0x0e:msg("‘‚«ž‚Ý‚ª‹ÖŽ~‚³‚ê‚Ä‚¢‚Ü‚·B");break;
			/*case 0x0f:msg("ƒtƒ@ƒCƒ‹‹¤—Lˆá”½‚Å‚·B");break;*/

			case 0x09:/*msg("ƒvƒŠƒ“ƒ^ƒIƒtƒ‰ƒCƒ“");*/;
			/*case 0x1f:msg("ƒCƒ“ƒ^ƒ‰ƒvƒg‚ª‰Ÿ‚³‚ê‚Ü‚µ‚½B");*/
				IJUMP_RTE();return;break;
			default:
				IJUMP(ErrorTrap_Old);return;break;
		}
		/*bufset();IJUMP(ErrorTrap_Old);*/
		home2(0);B_LOCATE(0,2);C_CLS_ED();fcloseall();

		asm("andi.w #$d8ff,sr");
		IJUMP((int)*com_sel);
	} else {
		IJUMP(ErrorTrap_Old);
	}
}

/***************************/
