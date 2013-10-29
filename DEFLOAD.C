/*
 * STed: defload.c (def file)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"sted.h"

#if	STED
int	gpal_no[16]={
	0,14*64+28*2048+28*2,28*64+28*2048+14*2,28*64+28*2048+28*2,
	11*64+11*2048+11*2,6*64+6*2048+6*2,0,

	11*2048+14*2,14*2048+17*2, 17*2048,31*2048,31*2048+31*2,
	17*64+17*2048,31*64+31*2048,16*64+16*2048+16*2,31*64+31*2048+31*2};

unsigned char	stgt_def[30]={
	192,144, 96, 72, 48, 36, 24, 18, 12,  6,
	188,140, 92, 68, 45, 34, 22, 16, 10,  5,
	  1, 15, 29, 43, 57, 71, 85, 99,113,127};
#endif

char	tim_asin2[33];
char	defrhyna[32][15];
unsigned char	defrhyno[32][2];

char	def_user_exc_memo[8][25];
unsigned char	def_user_exc_data[8][24];

void	msg();
void	msg_clr();
void	snsclr();
void	fnc_dis();
void	spcut();
char	*spadd();
char	*jstrupr();
char	*hex_s();
char	*fstr();
char	*tim_names();
int	str_val();
int	ctc();
int	strch();
int	str_search();

void	tim_name_read();

void	rhinit();
void	rhinit2();
void	user_exc_init();
void	user_exc_init2();
void	stgt_init();
void	stgt_init2();
void	asin_change();
void	asin_init();
int	defload();
int	def_save();
int	def_bload();
int	def_bsave();
void	cnf_stgt();
void	cnf_rvel();
void	cnf_repl();
void	cnf_prog();
void	def_init();

/* ================================================== file */
#if	STED
void	rhinit()
{
	int	i;

	for(i=0;i<32;i++){rhinit2(i);}
}
void	rhinit2(int i)
{
	strcpy(rhyna[i],defrhyna[i]);
	rhyno[i][0]=defrhyno[i][0];rhyno[i][1]=defrhyno[i][1];
}
void	user_exc_init()
{
	int	i;

	for(i=0;i<8;i++){user_exc_init2(i);}
}
void	user_exc_init2(int i)
{
	int	j;

	strcpy(user_exc_memo[i],def_user_exc_memo[i]);
	for(j=0;j<24;j++){user_exc_data[i][j]=def_user_exc_data[i][j];}
}

/***************************/
void	stgt_init()
{
	int i;

	for(i=0;i<30;i++){stgt_init2(i);}
}
void	stgt_init2(int i)
{
	stgt_tbl[i]=stgt_def[i];
}
/***************************/
void	asin_change()
{
	int	a,i;

	if((mdlflag&3)==1){
		for(i=1;i<=16;i++){if(tim_asin[i]<4){tim_asin[i]=0;}}
		for(i=0x8d;i<=0x8d+8;i++){
			a=cm6[i]+1;
			if(a<17){
				if( tim_asin[a]==0){
					if(i<0x8d+8){tim_asin[a]=1;}else{tim_asin[a]=2;}
				}
			}
		}
		for(i=0x583c;i<=0x583c+5;i++){
			a=cm6[i]+1;if(a<17){if( tim_asin[a]==0){tim_asin[a]=3;}}
		}
	}
}

void	asin_init()
{
	int	i;
	for(i=1;i<33;i++){tim_asin[i]=tim_asin2[i];}
}
/***************************/
void	cnfinit()
{
	int i;

	inpmode=0;grpmode=0;thrumode=0;vis_reso=100;

	for(i=0;i<16;i++){mplay[i][0]=0;palet_dat[i]=gpal_no[i];}
	strcpy(mplay[1],"rcp -pbs");strcpy(mplay[9],"rcp -ps");

	for(i=0;i<128;i++){keyst_table[0][i]=0;keyst_table[1][i]=0;}
	for(i=0;i<26*3*2;i++){repl_t[i][0]=0;repl_d[i][0]=0;}

	for(i=0;i<16;i++){rhy_vel[i]=0;}
	for(i=1;i<10;i++){rhy_vel[i]=i*14;}
}
#endif
/***************************/
void	definit()
{
	int	i,j;

	strcpy(card_name[0],"NoUse <<PCM CARD>>");
	for(i=1;i<17;i++){strcpy(card_name[i]," --------");}

	for(i=0;i<80*128;i++){tim_name[i][0]=0;}
	for(i=0;i<400;i++){tim_top[i]=0;tim_name[i][0]=0;tim_head[i][0]=0;}

	for(i=0;i<8;i++){for(j=0;j<24;j++){
		def_user_exc_memo[i][j]=0;def_user_exc_data[i][j]=0xf7;}}

	init_exc_data[0]=255;

	for(i=0;i<18*128;i++){gs_bank[i]=0;}
	for(i=0;i<18;i++){gs_mode[i]=0;}
	gs_mode[16]=4;

	for(i=0;i<16;i++){rfilt[i][0]=0;rfilt[i][1]=0;rfilt[i+16][0]=128;rfilt[i+16][1]=128;}

	rec_met[0]=16;rec_met[1]=0;
	rec_met[2]=64;rec_met[3]=2;rec_met[4]=117;
	rec_met[5]=64;rec_met[6]=2;rec_met[7]=127;

	rhy_stest[0]=10;rhy_stest[1]=-1;
	rhy_stest[2]=-1;rhy_stest[4]=127;

	lsp_wait=1;bend_range=12;
	tim_name_read();
}

/***************************/
/*
	cmdflag=		gs_mode=	mdlfalg=
	   00	    00		   00		   00
	84218421 84218421	84218421	84218421
	00000000 00000000	00000000	00000000
	|||||||| ||||||||	||   |||	||||||||
	|||||||| ||||||||	||   |||	||||++++MODULE ID(1=CM-64 2=SC-55
	|||||||| ||||||||	||   |||	++++----	  3=SC-88)
	|||||||| ||||||||	||   ||+bit0	1=capital down on
	|||||||| ||||||||	||   |+-bit1	1=rhy.cap down on
	|||||||| ||||||||	||   +--bit2	0=bank msb 1=bank lsb
	|||||||| ||||||||	||
	|||||||| ||||||||	|+------bit6	1=rhythm mode on
	|||||||| ||||||||	+-------bit7	1=bank mode on
	|||||||| |||||||+TIMBRE NAME
	|||||||| ||||||+-RET&SPC
	|||||||| |||||+--SPC.CTRL SEL
	|||||||| ||||+---BUF LOAD
	|||||||| |||+----BUF SET
	|||||||| ||+-----PROG_MIN
	|||||||| |+------ALL NOTE OFF
	|||||||| +-------RCD POINT PLAY
	|||||||+Rol.Para HEX
	||||||+-MOUSE CONTROL (0=on 1=off)
	|||||+--RHY AS. check (0=yes/no check 1=all yes)
	||||+---RHY AS. change(0=on 1=off)
	|||+----timbre select (0=val 1=sel)
	||+-----timbre select mode(0=normal 1=bank)
	|+------meas play mluti track lsp
	+-------rcp panel mode

	cnfflag=
	   00       00
	84218421 84218421
	00000000 00000000
	|||||||| ||||||||
	|||||||| |||||||+file selecter sort
	|||||||| ||||||+-file write check
	|||||||| |||||+--file back up
	|||||||| ||||+---file selecter path move(0=on 1=off)
	|||||||| |||+----
	|||||||| ||+-----
	|||||||| |+------file save mode(0=r36 1=rcp)
	|||||||| +-------file load part assign read
	||||||||
	|||||||+
	||||||+-t.set. option mode(0=option2 1=option1)
	|||||+--t.edit option mode(0=option2 1=option1)
	||||+---
	|||+----replace selecter mode(0=normal 1=select)
	||+-----one touch stgt	(0=normal 1=chord)
	|+------visual edit	(0=normal 1=wide)
	+-------gs bar display	(0=normal 1=reverse)
*/

int	defload(char *fi,int md)
{
	char	tmp0[2048],tmp1[1024],li[1024],li2[1024];
	int	ad,nm,ln,n,i0=0,i1=0,f,mutef=0,initf=0;
	int	last_no=4*128,last_co=0;
	FILE	*fp;

	if(str_search(fi,".dfb")||str_search(fi,".DFB")>0){return def_bload(fi,md);}

	if(!(fp= fopen(fi,"rb"))){return(-1);}
	ln=fread(dat,1,work_size,fp);ad=0;nm=99999;

#if	STED
	if(md==0){
		B_PRINT("定義ファイルのロード中です。\r\n");
	}
	if(md==1){
		msg("定義ファイルのロード中です。\r\n");
	}
#endif
#if	CONV
	B_PRINT("定義ファイルのコンパイル中です。\r\n");
#endif

	while(ln>0){
top:
		n=0;
		while(1){
			unsigned char	a;
loop:
			a=dat[ad++];
			ln--;if(ln==0){ln=fread(dat,1,work_size,fp);ad=0;if(ln==0){break;}}
			if(a<32){
				if(a==13){break;}
				if(a==9){goto loop;}
				a=32;
			}else{
				if(a=='*'){a=0;
					if(n!=0 && li[n-1]=='\\'){n--;a='*';}
				}
				if(a=='=' && li[0]=='#'){while(li[n-1]==' '){n--;}}
			}
			li[n++]=a;
		}
		li[n]=0;li[1023]=0;
		if(dat[ad]==0x0a){ad++;
			ln--;if(ln==0){ln=fread(dat,1,work_size,fp);ad=0;}
		}

		spcut(li);
		if( li[0]==0 ){goto top;}

		if( li[0]=='#' ){
			strcpy(li2,li);jstrupr(li2);

			if( str_search(li2,"#END")>0 ){break;}

#if	STED
			if( str_search(li2,"#COMMENT=")>0 ){strcpy(comment,&li[9]);}

			if( str_search(li2,"#DEF_FILE=")>0 ){
				if(def_file[0]==0){strcpy(def_file,&li[10]);}}
			if( str_search(li2,"#DEF_PATH=")>0 ){
				if(def_path[0]==0){strcpy(def_path,&li[10]);}}

			if( str_search(li2,"#FON_PATH=")>0 ){
				if(fon_path[0]==0){strcpy(fon_path,&li[10]);}}
			if( str_search(li2,"#HLP_PATH=")>0 ){
				if(hlp_path[0]==0){strcpy(hlp_path,&li[10]);}}

			if( str_search(li2,"#PRT_PATH=")>0 ){
				if(prt_path[0]==0){strcpy(prt_path,&li[10]);}}
			if( str_search(li2,"#TRK_PATH=")>0 ){
				if(trk_path[0]==0){strcpy(trk_path,&li[10]);}}

			if(str_search(li2,"#FSORT=ON"    )>0){cnfflag|=0x0001;}
			if(str_search(li2,"#FCHECK=ON"   )>0){cnfflag|=0x0002;}
			if(str_search(li2,"#FBACKUP=ON"  )>0){cnfflag|=0x0004;}
			if(str_search(li2,"#FSEL_MOV=OFF")>0){cnfflag|=0x0008;}
			if(str_search(li2,"#SAVE_DEF=RCP")>0){cnfflag|=0x0040;}
			if(str_search(li2,"#PAS_READ=OFF")>0){cnfflag|=0x0080;}

/*			if(str_search(li2,"#MAIN_OPT=ON" )>0){cnfflag|=0x0100;}*/
			if(str_search(li2,"#TSET_OPT=ON" )>0){cnfflag|=0x0200;}
			if(str_search(li2,"#TEDT_OPT=ON" )>0){cnfflag|=0x0400;}
			if(str_search(li2,"#REPL_SEL=ON" )>0){cnfflag|=0x1000;}

			if(str_search(li2,"#ONE_CHORD=ON")>0){cnfflag|=0x2000;}
			if(str_search(li2,"#VIS_WIDE=ON" )>0){cnfflag|=0x4000;}
			if(str_search(li2,"#BAR_REV=ON"  )>0){cnfflag|=0x8000;}

			if(str_search(li2,"#TONENAME=OFF")>0){cmdflag|=0x0001;}
			if(str_search(li2,"#ENTER=ON"    )>0){cmdflag|=0x0002;}
			if(str_search(li2,"#SPC_SEL=ON"  )>0){cmdflag|=0x0004;}
			if(str_search(li2,"#PROG_MIN=1"  )>0){cmdflag|=0x0020;}
			if(str_search(li2,"#NOTEOFF=ON"  )>0){cmdflag|=0x0040;}
			if(str_search(li2,"#ROL_HEX=ON"  )>0){cmdflag|=0x0100;}
			if(str_search(li2,"#RHY_TRK=ON"  )>0){cmdflag|=0x0400;}
			if(str_search(li2,"#RHY_TRK=OFF" )>0){cmdflag|=0x0800;}
			if(str_search(li2,"#TONESEL=OFF" )>0){cmdflag|=0x1000;}
			if(str_search(li2,"#TSELBANK=ON" )>0){cmdflag|=0x2000;}
			if(str_search(li2,"#MP_MLSP=OFF" )>0){cmdflag|=0x4000;}

/*			if(str_search(li2,"#RHY_SYM=ON"  )>0){cmdflag|=0x20000;}*/
			if(str_search(li2,"#THRUMODE=ON" )>0){thrumode=1;}

			if( str_search(li2,"#TIME_LAG=")>0){
				int n=str_val(&li[10]);if( n>99 && n<65536 ){tm_lag=n;}
			}
			if( str_search(li2,"#EDIT_MODE=")>0){
				int n=str_val(&li[11]);if( n>=0 && n<6 ){inpmode=n;}
			}
			if( str_search(li2,"#GRAP_MODE=")>0){
				int n=str_val(&li[11]);if( n>=0 && n<8 ){grpmode=n;}
			}

			if( str_search(li2,"#VIS_RESO=")>0){
				int n=str_val(&li[10]);if( n>=1 && n<=100 ){vis_reso=n;}
			}

			if( str_search(li2,"#PROGRAM=")>0 ){cnf_prog(li,li2,0);}
			if( str_search(li2,"#SUBPROG=")>0 ){cnf_prog(li,li2,1);}

			if( str_search(li2,"#REPLACE=")>0 ){cnf_repl(li,li2,0);}
			if( str_search(li2,"#DELETE=")>0 ){cnf_repl(li,li2,1);}
			if( str_search(li2,"#FIND=")>0 ){cnf_repl(li,li2,2);}

			if( str_search(li2,"#MPLAY=")>0 ){
				int	c,n;
				n=li2[7]-'0';
				if(n>=0 && n<16){
					strcpy(tmp1,&li[9]);strcat(tmp1,",");
					while(n<16){
						c=str_search(tmp1,",");if( c==0 ){break;}
						strcpy(tmp0,tmp1);tmp0[c-1]=0;spcut(tmp0);
						strcpy(tmp1,&tmp1[c]);
						tmp0[15]=0;
						strcpy(mplay[n],tmp0);
						n++;
					}
				}
			}

			if( str_search(li2,"#RHY_VEL=")>0 ){cnf_rvel(li);}

			if( str_search(li2,"#FKEY_ST=")>0 ){cnf_stgt(li,0);}
			if( str_search(li2,"#FKEY_GT=")>0 ){cnf_stgt(li,10);}
			if( str_search(li2,"#FKEY_VE=")>0 ){cnf_stgt(li,20);}

			if( str_search(li2,"#KEYST=")>0 ){
				int	c,n,k=0;
				strcpy(tmp1,&li[7]);strcat(tmp1,",");n=0;
				while(n<3){
					c=str_search(tmp1,",");if( c==0 ){break;}
					strcpy(tmp0,tmp1);tmp0[c-1]=0;spcut(tmp0);
					strcpy(tmp1,&tmp1[c]);
					if(n==0){
						k=ctc(64,tmp0,0);
						if(k<0||k>127){break;}
					}else{
						keyst_table[n-1][k]=str_val(tmp0);
					}
					n++;
				}
			}

			if( str_search(li2,"#PALET")>0 ){
				int	c,n;
				c=str_search(li,"=");
				if(c){
					li2[c]=0;n=str_val(&li2[6]);
					if(n>=0 && n<16){
						strcpy(tmp1,&li[c]);strcat(tmp1,",");
						while(n<16){
							c=str_search(tmp1,",");if( c==0 ){break;}
							strcpy(tmp0,tmp1);tmp0[c-1]=0;
							spcut(tmp0);palet_dat[n]=str_val(tmp0);
							strcpy(tmp1,&tmp1[c]);n++;
						}
					}
				}
			}
#endif



			if( str_search(li2,"#MODULE=")>0 ){strcpy(module,&li[8]);}

			if( str_search(li2,"#ID=")>0 ){
				strcpy(tmp1,&li2[4]);
				if( str_search(tmp1,"CM64")>0 ){mdlflag|=1;}
				if( str_search(tmp1,"SC55")>0 ){mdlflag|=2;}
				if( str_search(tmp1,"SC88")>0 ){mdlflag|=3;}
				if( str_search(tmp1,"05RW")>0 ){mdlflag|=4;}
			}
			if( str_search(li2,"#MUTE=")>0 ){
				strcpy(tmp1,&li2[6]);mutef=0;
				if( str_search(tmp1,"OFF")>0  ){mutef=1;}
				if( str_search(tmp1,"CM64")>0 ){mutef=2;}
				if( str_search(tmp1,"SC55")>0 ){mutef=3;}
			}
			if( str_search(li2,"#INIT=")>0 ){
				strcpy(tmp1,&li2[6]);initf=0;
				if( str_search(tmp1,"OFF")>0  ){initf=1;}
				if( str_search(tmp1,"CM64")>0 ){initf=2;}
				if( str_search(tmp1,"SC55")>0 ){initf=3;}
				if( str_search(tmp1,"SC88")>0 ){initf=5;}
			}

			if( str_search(li2,"#LSP_WAIT=")>0){
				int n=str_val(&li[10]);if( n>=-1 && n<16 ){lsp_wait=n;}
			}
			if( str_search(li2,"#PB_RANGE=")>0){
				int n=str_val(&li[10]);if( n>0 && n<96 ){bend_range=n;}
			}

			if( str_search(li2,"#GROUP")>0 ){
				int	c,n;
				c=str_search(li,"=");
				if(c!=0){
					n=0;if(li2[7]=='B'){n=16;}
					strcpy(tmp1,&li[c]);strcat(tmp1,",");
					while(n<32){
						c=str_search(tmp1,",");if(c==0){break;}
						strcpy(tmp0,tmp1);tmp0[c-1]=0;
						n++;tim_asin2[n]=str_val(tmp0);
						strcpy(tmp1,&tmp1[c]);
					}
				}
			}
			if( str_search(li2,"#CARD_NO=")>0 ){
				int n=str_val(&li[9]);if(n>=0 && n<=16){card_no[0]=n;}
			}

			if( str_search(li2,"#PART=")>0 ){
				int	ch,c,n;
				ch=str_val(&li2[6])-1;
				c=str_search(li,",");
				if(ch>=0 && ch<400){
					strcpy(tmp1,&li[c]);strcat(tmp1,",");n=0;
					while(n<2){
						c=str_search(tmp1,",");if( c==0 ){break;}
						strcpy(tmp0,tmp1);tmp0[c-1]=0;
						if(n==0){strcpy(tim_head[ch],tmp0);}
						else{strcpy(tim_sym[ch],tmp0);}
						strcpy(tmp1,&tmp1[c]);n++;
					}
					nm=ch*128;if( ch==1 ){nm=192;}
					if(tmp1[0]!=0){nm+=str_val(tmp1);}

					if(ch>3){
						if(last_no<80*128-1){
							tim_name[last_no++][0]=0;
							tim_top[ch]=last_no;last_co=0;
							if(tmp1[0]!=0){last_co=str_val(tmp1);}
						}
					}
				}
			}
			if( str_search(li2,"#USER_EXC=")>0 ){
				int	ch,c;
				char	tmp9[128];
				ch=str_val(&li2[10]);
				c=str_search(li,",");
				if(ch>=0 && ch<=7){
					int	i=0,j=0;
					strcpy(tmp1,&li[c]);
					c=str_search(tmp1,",");
					strcpy(tmp0,tmp1);if(c){tmp0[c-1]=0;}
					spcut(tmp0);

					while(i<c && j<24 && tmp0[i]!=0){
						int	a;
						if(tmp0[i]==32){i++;}else{
							strcpy(tmp9,&tmp0[i]);
							if(tmp9[1]==0){
								tmp9[1]='H';tmp9[2]=0;
							}else{
								tmp9[2]='H';tmp9[3]=0;
							}
							a=str_val(tmp9);tmp9[2]=0;
							if(strcmp(tmp9,"GT")==0){a=0x80;}
							if(strcmp(tmp9,"VE")==0){a=0x81;}
							if(strcmp(tmp9,"CH")==0){a=0x82;}
							if(strcmp(tmp9,"CS")==0){a=0x83;}
							if(strcmp(tmp9,"SS")==0){a=0x84;}

							def_user_exc_data[ch][j++]=a;
							i=i+2;
						}
					}
					if(c){
						strcpy(tmp0,&tmp1[c]);
						tmp0[24]=0;strcpy(def_user_exc_memo[ch],tmp0);
					}
				}
			}
			if( str_search(li2,"#INIT_EXC=")>0 ){def_init(li);}

			if( str_search(li2,"#CARD=")>0 ){
				int	c,ch;
				strcpy(tmp1,&li[6]);
				c=str_search(tmp1,",");
				if(c){
					strcpy(tmp0,tmp1);tmp0[c-1]=0;ch=str_val(tmp0);
					strcpy(tmp0,&tmp1[c]);
					if(ch>0 && ch<17){
						strcpy(card_name[ch],tmp0);
						nm=(ch+15)*128;

						if(last_no<80*128-1){
							tim_name[last_no++][0]=0;
							tim_top[ch+15]=last_no;last_co=0;
						}
					}
				}
			}

			if( str_search(li2,"#GS_PBANK")>0 ){gs_mode[ 7]|=128   ;nm=0x10380;}
			if( str_search(li2,"#GS_RBANK")>0 ){gs_mode[ 8]|=128+64;nm=0x10400;}
			if( str_search(li2,"#GM_PBANK")>0 ){gs_mode[ 9]|=128   ;nm=0x10480;}
			if( str_search(li2,"#GM_RBANK")>0 ){gs_mode[10]|=128+64;nm=0x10500;}
			if( str_search(li2,"#GX_PBANK")>0 ){gs_mode[11]|=128   ;nm=0x10580;}
			if( str_search(li2,"#GX_RBANK")>0 ){gs_mode[12]|=128+64;nm=0x10600;}
			if( str_search(li2,"#GY_PBANK")>0 ){gs_mode[13]|=128   ;nm=0x10680;}
			if( str_search(li2,"#GY_RBANK")>0 ){gs_mode[14]|=128+64;nm=0x10700;}

			if( str_search(li2,"#GS_PGROUP")>0 ){gs_mode[16]|=128   ;nm=0x10800;}
			if( str_search(li2,"#GS_RGROUP")>0 ){gs_mode[17]|=128+64;nm=0x10880;}

			if( str_search(li2,"#GS_GMODE=")>0){
				int	n=str_val(&li[10]);
				if( n>=0 && n<16){gs_mode[16]&=0xf0;gs_mode[16]|=n;}
			}

			if( str_search(li2,"#GS_MODE=")>0){
				int	n=str_val(&li[9]);
				if( n>=0 && n<16){gs_mode[7]&=0xf0;gs_mode[7]|=n;}
			}
			if( str_search(li2,"#GM_MODE=")>0){
				int	n=str_val(&li[9]);
				if( n>=0 && n<16){gs_mode[9]&=0xf0;gs_mode[9]|=n;}
			}
			if( str_search(li2,"#GX_MODE=")>0){
				int	n=str_val(&li[9]);
				if( n>=0 && n<16){gs_mode[11]&=0xf0;gs_mode[11]|=n;}
			}
			if( str_search(li2,"#GY_MODE=")>0){
				int	n=str_val(&li[9]);
				if( n>=0 && n<16){gs_mode[13]&=0xf0;gs_mode[13]|=n;}
			}

			if( str_search(li2,"#RHY_AS=")>0 ){
				int	c;
				nm=0x10000+128*18;i0=0;i1=0;

				c=str_search(li,"=");
				if(c!=0){
					int n=0;
					strcpy(tmp1,&li[c]);strcat(tmp1,",");
					while(n<6){
						int cc;
						c=str_search(tmp1,",");if(c==0){break;}
						strcpy(tmp0,tmp1);tmp0[c-1]=0;
						if(n==0){cc=strch(0,tmp0);
						}else{cc=str_val(tmp0);}
						if((cc>=0||(n>0 && n<5))&& cc<=127){rhy_stest[n]=cc;}
						strcpy(tmp1,&tmp1[c]);
						n++;
					}
				}
			}


			if( str_search(li2,"#REC_MET")>0 ){
				int	c,n;
				c=str_search(li,"=");
				if(c!=0){
					n=0;
					strcpy(tmp1,&li[c]);strcat(tmp1,",");
					while(n<10){
						c=str_search(tmp1,",");if(c==0){break;}
						strcpy(tmp0,tmp1);tmp0[c-1]=0;
						rec_met[n++]=str_val(tmp0);
						strcpy(tmp1,&tmp1[c]);
					}
				}
			}

			if( str_search(li2,"#DEFOFF")>0 ){nm=99999;}

		}else{
			int	c;
			char	*pp=li,*tmp0;
			strcat(li,",");
			while( nm<99999 ){
				c=str_search(pp,",");if( c==0 ){break;}
				tmp0=pp;tmp0[c-1]=0;pp+=c;

				if(nm<0x10000 ){
					if(c>15){tmp0[14]=0;}
					spcut(tmp0);

					if( nm<0x200 ){
						strcpy(tim_name[nm],tmp0);
					}else{
						if(last_no<80*128-1){
							if(tmp0[0]!=0){
								strcpy(tim_name[last_no],tmp0);
								tim_name[last_no++][14]=last_co;
							}
							if(last_co++==127){nm=99999;}
						}
					}
				}else{
					if(nm<0x10900){
						int ch=str_val(tmp0);
						if(ch<400){
							gs_bank[nm-0x10000]=ch;
						}
					}else{
						if(i0<32){
							if(i1){
								defrhyno[i0][i1-1]=str_val(tmp0);
								/*if(i1==1){
									defrhyno[i0][0]=ctc(60,tmp0,0);
								}else{
									defrhyno[i0][1]=str_val(tmp0);
								}*/
							}else{
								if(c>15){tmp0[14]=0;}
								strcpy(defrhyna[i0],tmp0);
							}
							i1++;if(i1>=3){i1=0;i0++;}
						}
					}
				}
				nm++;
			}
/*			if(md==1){msg(fstr(last_no,5));}*/
		}
	}
	fclose(fp);
	tim_name[last_no++][0]=0;

#if	STED
	f=(mdlflag&0x7);if(f>2){f=2;}
	if(f>0 && f<4){rcd->moduletype=f-1;}

	if(mutef){rcd->mute_mode=mutef-1;}
	if(initf){rcd->init_mode=initf-1;}

#endif
#if	CONV
		def_bsave(fi,last_no,mutef,initf);
#endif

	return(0);
}

/***************************//*4.66(2.18) 4.00(1.85)*/
/***************************/
#if	STED
void	cnf_stgt(char *li,int nn)
{
	int	c,n;
	char	tmp0[1024];
	char	tmp1[1024];

	c=str_search(li,"=");
	if(c!=0){
		strcpy(tmp1,&li[c]);strcat(tmp1,",");n=nn;
		while(n<30){
			c=str_search(tmp1,",");if(c==0){break;}
			strcpy(tmp0,tmp1);tmp0[c-1]=0;
			stgt_def[n++]=str_val(tmp0);
			strcpy(tmp1,&tmp1[c]);
		}
	}
}

/***************************/
void	cnf_rvel(char *li)
{
	int	c,n;
	char	tmp0[1024];
	char	tmp1[1024];

	c=str_search(li,"=");
	if(c!=0){
		n=0;
		strcpy(tmp1,&li[c]);strcat(tmp1,",");
		while(n<15){
			int cc;
			c=str_search(tmp1,",");if(c==0){break;}
			strcpy(tmp0,tmp1);tmp0[c-1]=0;
			n++;cc=str_val(tmp0);
			if(cc>=0 && cc<=127){rhy_vel[n]=cc;}
			strcpy(tmp1,&tmp1[c]);
		}
	}
}

/***************************/
void	cnf_prog(char *li,char *li2,int md)
{
	int	i,c,n;
	char	tmp0[1024];
	char	tmp1[1024];

	i=li2[9]-'A';

	if(i>=0 && i<26){
		if(md){i+=26;}
		strcpy(tmp1,&li[11]);strcat(tmp1,",");n=0;
		while(n<4){
			c=str_search(tmp1,",");if( c==0 ){break;}

			if(c>1 && tmp1[c-2]=='\\'){
				int d;
				strcpy(&tmp1[c-2],&tmp1[c-1]);

				d=str_search(&tmp1[c-1],",");if( d==0 ){break;}
				c+=d-1;
			}

			strcpy(tmp0,tmp1);tmp0[c-1]=0;spcut(tmp0);
			strcpy(tmp1,&tmp1[c]);
			if((n&1)==0){
				tmp0[38+10]=0;
				strcpy(chcom_s[i][n>>1],tmp0);
			}else{
				tmp0[15]=0;
				strcpy(chcom_c[i][n>>1],tmp0);
			}
			n++;
		}
	}
}

/***************************/
void	cnf_repl(char *li,char *li2,int md)
{
	int	i,c;
	char	tmp0[1024];
	char	tmp1[1024];

	c=9;if(md){c=8;if(md>1){c=6;}}

	i=li2[c]-'A';
	if(li2[c]=='S' && li2[c+1]>='A' && li2[c+1]<='Z'){i=(li2[c+1]-'A')+26;c++;}else{
	if(li2[c]==' '){i=(li2[c+1]-'A');c++;}}

	c=str_search(li,",");if( c==0 ){return;}

	if(i>=0 && i<52){
		strcpy(tmp1,&li[c/*+2*/]);
		c=str_search(tmp1,",");if( c==0 ){return;}
		strcpy(tmp0,tmp1);tmp0[c-1]=0;spcut(tmp0);
		strcpy(tmp1,&tmp1[c]);spcut(tmp1);

		while(tmp1[0]==32){strcpy(tmp1,&tmp1[1]);}

		tmp0[35]=0;strcpy(repl_t[i+md*52],tmp0);
		tmp1[35]=0;strcpy(repl_d[i+md*52],tmp1);
	}
}
#endif

/***************************/
void	def_init(char *li)
{
	char	tmp0[1024];
	char	tmp1[1024];
	int	i=0,j=0,sum=0;

	strcpy(tmp0,&li[10]);
	spcut(tmp0);

	while(j<256 && tmp0[i]!=0){
		int	a;
		if(tmp0[i]==32){i++;
		}else{
			strcpy(tmp1,&tmp0[i]);
			if(tmp1[1]==0){
				tmp1[1]='H';tmp1[2]=0;
			}else{
				tmp1[2]='H';tmp1[3]=0;
			}
			a=str_val(tmp1);tmp1[2]=0;
			if(strcmp(tmp1,"CS")==0){sum=0;goto ne;}
			if(strcmp(tmp1,"SS")==0){a=(127-(sum&0x7f)&0x7f);}
			init_exc_data[j++]=a;sum+=a;
		}
ne:
		i=i+2;
	}
	init_exc_data[j]=0xff;
}

/***************************/
#if	CONV
int	defsave(char *fi,int md,int mutef,int initf)
{
	int	ln,n=0,i,j,k;
	FILE	*fp;

	char gggg[12][12]={"#gs_pbank","#gs_rbank","#gm_pbank","#gm_rbank",
			"#gx_pbank","#gx_rbank","#gy_pbank","#gy_rbank","",
			"#gs_pgroup","#gs_rgroup"};

	char	li0[]= "*--------------------------------------------------------------------------------------\r\n";
	char	li1[]= "*======================================================================================\r\n";

	strmfe(fi,fi,"def");
	if(!(fp=fopen(fi,"wb"))){B_PRINT("ファイルがオープン出来ません。\r\n");exit(1);}

	n=buf_put(li0,n);

	n=buf_put("\r\n#module=",n);n=buf_put(module,n);n=buf_put("\r\n\r\n",n);


	if(mdlflag&15){
		n=buf_put("#id=",n);
		if((mdlflag&15)==1){n=buf_put("cm64",n);}
		if((mdlflag&15)==2){n=buf_put("sc55",n);}
		if((mdlflag&15)==3){n=buf_put("sc88",n);}
		if((mdlflag&15)==4){n=buf_put("05rw",n);}
		n=buf_put("\r\n",n);
	}

	if( mutef>0 ){
		n=buf_put("#mute=",n);
		if(mutef==1){n=buf_put("off",n);}
		if(mutef==2){n=buf_put("cm64",n);}
		if(mutef==3){n=buf_put("sc55",n);}
		n=buf_put("\r\n",n);
	}
	if( initf>0 ){
		n=buf_put("#init=",n);
		if(initf==1){n=buf_put("off",n);}
		if(initf==2){n=buf_put("cm64",n);}
		if(initf==3){n=buf_put("sc55",n);}
		if(initf==5){n=buf_put("sc88",n);}
		n=buf_put("\r\n",n);
	}

	n=buf_put("\r\n",n);
	if(card_no[0]){
		n=buf_put("#card_no=",n);n=buf_put(fstr(card_no[0],2),n);n=buf_put("\r\n",n);
	}
	n=buf_put("#lsp_wait=",n);n=buf_put(fstr(lsp_wait,2),n);n=buf_put("\r\n",n);
	if(bend_range!=12){
		n=buf_put("#pb_range=",n);n=buf_put(fstr(bend_range,2),n);n=buf_put("\r\n",n);
	}
	n=buf_put("\r\n",n);

	n=buf_put(li1,n);

	n=buf_put("\r\n#rec_met=",n);
	for(i=0;i<8;i++){n=buf_put(fstr(rec_met[i],3),n);if(i<7){n=buf_put(",",n);}}
	n=buf_put("\r\n\r\n",n);

	n=buf_put(li1,n);


	for(i=0;i<8;i++){
		n=buf_put("#user_exc=",n);
		n=buf_put(fstr(i,1),n);n=buf_put(",",n);
		n=buf_put(exc_str(def_user_exc_data[i],24,0),n);
		n=buf_put(",",n);
		n=buf_put(def_user_exc_memo[i],n);n=buf_put("\r\n",n);
	}
	n=buf_put("\r\n",n);

	if(init_exc_data[0]!=0xff){
		n=buf_put("#init_exc=",n);
		n=buf_put(exc_str(init_exc_data,256,1),n);
		n=buf_put("\r\n\r\n",n);
	}

	n=buf_put(li1,n);

	n=buf_put("#rhy_as=",n);
	for(i=0;i<5;i++){n=buf_put(fstr(rhy_stest[i],3),n);if(i<4){n=buf_put(",",n);}}
	n=buf_put("\r\n",n);
	for(i=0;i<32;i++){
		n=buf_put(defrhyna[i],n);n=buf_put(",",n);
		n=buf_put(fstr(defrhyno[i][0],3),n);n=buf_put(",",n);
		if(defrhyno[i][1]<100){
			n=buf_put(fstr(defrhyno[i][1],2),n);
		}else{
			n=buf_put(fstr(defrhyno[i][1],3),n);
		}
		if((i&3)==3){n=buf_put("\r\n",n);}else{n=buf_put(",",n);}
	}
	n=buf_put("#defoff\r\n\r\n",n);

	n=buf_put(li1,n);


	n=buf_put("#group_a=",n);
	for(i=1;i<17;i++){n=buf_put(fstr(tim_asin2[i],2),n);if(i<16){n=buf_put(",",n);}}
	n=buf_put("\r\n",n);
	n=buf_put("#group_b=",n);
	for(i=17;i<33;i++){n=buf_put(fstr(tim_asin2[i],2),n);if(i<32){n=buf_put(",",n);}}
	n=buf_put("\r\n\r\n",n);

	if(gs_mode[7]&15){n=buf_put("#gs_mode=",n);
		n=buf_put(fstr(gs_mode[7]&15,3),n);n=buf_put("\r\n\r\n",n);}
	if(gs_mode[9]&15){n=buf_put("#gm_mode=",n);
		n=buf_put(fstr(gs_mode[9]&15,3),n);n=buf_put("\r\n\r\n",n);}
	if(gs_mode[11]&15){n=buf_put("#gx_mode=",n);
		n=buf_put(fstr(gs_mode[11]&15,3),n);n=buf_put("\r\n\r\n",n);}
	if(gs_mode[13]&15){n=buf_put("#gy_mode=",n);
		n=buf_put(fstr(gs_mode[13]&15,3),n);n=buf_put("\r\n\r\n",n);}

	if(gs_mode[16]&128){n=buf_put("#gs_gmode=",n);
		n=buf_put(fstr(gs_mode[16]&15,3),n);n=buf_put("\r\n\r\n",n);}
	n=buf_put("\r\n",n);

	for(i=7;i<18;i++){
		if(gs_mode[i]&0xf0){
			n=buf_put(gggg[i-7],n);n=buf_put("\r\n",n);

			for(j=0;j<128;j++){
				n=buf_put(fstr(gs_bank[i*128+j],3),n);
				if((j&15)!=15){n=buf_put(",",n);}else{n=buf_put("\r\n",n);}
			}
			n=buf_put("\r\n",n);
		}
	}

	n=buf_put(li1,n);

	ln=fwrite(dat,1,n,fp);n=0;

	for(i=1;i<4;i++){
		if( tim_top[i-1]!=0 || tim_head[i-1][0]!=0 ){
			int stp;
			n=buf_put("#part=",n);
			n=buf_put(fstr(i,3),n);n=buf_put(",",n);
			n=buf_put(tim_head[i-1],n);n=buf_put(",",n);
			n=buf_put(tim_sym[i-1],n);n=buf_put("\r\n",n);
			n=buf_put(li0,n);

			stp=0;if( i==2 ){stp=64;}

			for(j=stp;j<128;j++){
				n=buf_put(spadd(tim_names(j+i*128-128),10),n);
				if((j&7)!=7){n=buf_put(",",n);}else{n=buf_put("\r\n",n);}
			}
			n=buf_put(li0,n);
			n=buf_put("#defoff\r\n\r\n",n);
		}
	}

	for(i=4;i<16;i++){
		if( tim_top[i-1]!=0 || tim_head[i-1][0]!=0 ){

			n=buf_put("#part=",n);
			n=buf_put(fstr(i,3),n);n=buf_put(",",n);
			n=buf_put(tim_head[i-1],n);n=buf_put(",",n);
			n=buf_put(tim_sym[i-1],n);n=buf_put("\r\n",n);
			n=buf_put(li0,n);

			for(j=0;j<128;j++){
				n=buf_put(tim_names(j+i*128-128),n);
				if((j&3)!=3){n=buf_put(",",n);}else{n=buf_put("\r\n",n);}
			}
			n=buf_put(li0,n);
			n=buf_put("#defoff\r\n\r\n",n);
		}
	}

	for(i=1;i<17;i++){
		if( card_name[i][0]!=32 ){
			int edp;
			n=buf_put("#card=",n);
			n=buf_put(fstr(i,2),n);n=buf_put(",",n);
			n=buf_put(card_name[i],n);n=buf_put("\r\n",n);
			n=buf_put(li0,n);

			edp=127;while(edp>=0){
				if(tim_names(edp+(15+i)*128)[0]!=0){break;}
				edp--;
			}
			if(edp>=0){
				edp|=7;
				for(j=0;j<=edp;j++){
					n=buf_put(spadd(tim_names(j+(15+i)*128),10),n);
					if((j&7)!=7){n=buf_put(",",n);}else{n=buf_put("\r\n",n);}
				}
			}
			n=buf_put(li0,n);
			n=buf_put("#defoff\r\n\r\n",n);
			tim_top[i+15]=0;
		}
	}

	for(i=16;i<400;i++){
		if( tim_top[i-1]!=0 || tim_head[i-1][0]!=0 ){

			n=buf_put("#part=",n);
			n=buf_put(fstr(i,3),n);n=buf_put(",",n);
			n=buf_put(tim_head[i-1],n);n=buf_put(",",n);
			n=buf_put(tim_sym[i-1],n);n=buf_put("\r\n",n);
			n=buf_put(li0,n);

			for(j=0;j<128;j++){
				n=buf_put(tim_names(j+i*128-128),n);
				if((j&3)!=3){n=buf_put(",",n);}else{n=buf_put("\r\n",n);}
			}
			n=buf_put(li0,n);
			n=buf_put("#defoff\r\n\r\n",n);
		}
		if(i==100||i==200||i==300){ln=fwrite(dat,1,n,fp);n=0;}
	}

	n=buf_put("#end\r\n\r\n",n);
	n=buf_put(li0,n);

	ln=fwrite(dat,1,n,fp);

	fclose(fp);

	B_PRINT("定義ファイルをコンパイルしました。\r\n");
	exit(1);

	return(0);
}

/***************************//*4.66(2.18) 4.00(1.85)*/
char	*exc_str(unsigned char *li,int ln,int md)
{
	char	tmp0[1024];
	int	i=0;

	strcpy(tmp0,"");

	while(i<ln && tmp0[i]!=0){
		int	a;
		a=tmp0[i];

		if(a==0xff){break;}
		if(a==0xf7){if(md!=0){strcat(tmp0,"F7");}break;}

		if(a<0x80){
			strcat(tmp0,hex_s(a,2));
		}else{
			if(a==0xf0){strcat(tmp0,"F0 ");}

			if(a==0x80){strcat(tmp0,"gt ");}
			if(a==0x81){strcat(tmp0,"ve ");}
			if(a==0x82){strcat(tmp0,"ch ");}
			if(a==0x83){strcat(tmp0,"cs ");}
			if(a==0x84){strcat(tmp0,"cs ");}
		}
		i++;
	}
	return tmp0;
}

/***************************/
int	buf_put(char *ptr,int ln)
{
/*	if((rcd->bufcap)<(po+ln)){return(po+ln);}
*/
	while(*ptr){
		dat[ln++]=*ptr++;
	}
	return(ln);
}

/***************************/
int	def_bsave(char *fi,int last_no,int mutef,int initf)
{
	int	i,ln,n,top_no,cc,ad;
	FILE	*fp;

	strmfe(fi,fi,"dfb");
	if(!(fp=fopen(fi,"wb"))){B_PRINT("ファイルがオープン出来ません。\r\n");exit(1);}

	for(i=0;i<work_size;i++){dat[i]=0;}

	memcpy(&dat[0x0000],"STED2DEFFILE0.01",16);
	memcpy(&dat[0x0010],module,32);					/*module*/
	dat[0x30]=mdlflag;						/*id*/
	dat[0x32]=mutef;						/*mute*/
	dat[0x33]=initf;						/*init*/
	n=lsp_wait;if(n<0){n=128;}dat[0x34]=n;				/*lsp_wait*/
	dat[0x36]=bend_range;						/*bend range*/
	dat[0x38]=card_no[0];						/*card_no*/

	for(i=0;i<8;i++){
		memcpy(&dat[0x40+i*48],def_user_exc_memo[i],24);
		memcpy(&dat[0x40+i*48+24],def_user_exc_data[i],24);	/*USER_EXC*/
	}
	memcpy(&dat[0x01c0],init_exc_data,256);				/*INIT_EXC*/

	for(i=0;i<10;i++){n=rec_met[i];if(n<0){n=128;}dat[0x2c0+i]=n;}	/*REC_MET*/
	for(i=0;i<6;i++){n=rhy_stest[i];if(n<0){n=128;}dat[0x2ca+i]=n;}	/*RHY_AS*/

	for(i=0;i<32;i++){
		memcpy(&dat[0x300+i*16],defrhyna[i],14);
		dat[0x300+i*16+14]=defrhyno[i][0];dat[0x300+i*16+15]=defrhyno[i][1];
	}
	for(i=0;i<16;i++){memcpy(&dat[0x500+i*48],card_name[i+1],48);}	/*card*/

	memcpy(&dat[0x800],&tim_asin2[1],32);				/*group*/
	memcpy(&dat[0x820],&gs_mode[4],14*2);				/*gsmode*/
	memcpy(&dat[0x840],&gs_bank[128*4],14*128*2);			/*gsbank*/


	cc=0;for(i=0;i<400;i++){if(tim_top[i]>0 || tim_head[i][0]>0){cc++;}}
	dat[0x1640]=cc>>8;dat[0x1641]=cc&0xff;
	top_no=0;while(top_no<last_no){if(tim_name[top_no][0]){break;}top_no++;}
	dat[0x1642]=last_no>>8;dat[0x1643]=last_no&0xff;
	dat[0x1644]=top_no>>8;dat[0x1645]=top_no&0xff;

	ad=0x1650;							/*part*/
	for(i=0;i<400;i++){
		if(tim_top[i]>0 || tim_head[i][0]>0){
			memcpy(&dat[ad],tim_head[i],20);
			memcpy(&dat[ad+20],tim_sym[i],8);
			dat[ad+28]=i>>8;dat[ad+29]=i&0xff;
			dat[ad+30]=tim_top[i]>>8;dat[ad+31]=tim_top[i]&0xff;
			ad+=32;
		}
	}


	ln=fwrite(dat,1,ad,fp);

	ln=last_no-top_no;
	if(ln){ln=fwrite(&tim_name[top_no][0],1,ln*15,fp);}		/*tim_name*/

	fclose(fp);

	B_PRINT("定義ファイルをコンパイルしました。\r\n");
	exit(1);

	return(0);
}
#endif

/***************************/
int	def_bload(char *fi,int md)
{
	int	i,ln,n,f,top_no,last_no,mutef=0,initf=0,cc,ad;
	FILE	*fp;

	strmfe(fi,fi,"dfb");
	if(!(fp=fopen(fi,"rb"))){return(-1);}
	ln=fread(dat,1,0x1650,fp);


#if	STED
	if(md==0){
		B_PRINT("定義ファイルのロード中です。\r\n");
	}
	if(md==1){
		msg("定義ファイルのロード中です。\r\n");
	}
#endif
#if	CONV
	B_PRINT("定義ファイルの逆コンパイル中です。\r\n");
#endif

	if( strncmp(&dat[0x0000],"STED2DEFFILE0.01",16)!=0 ){
		if(md==0 || md==2){
			B_PRINT("違うバージョンのファイルです。\r\n");
		}else{
			msg("違うバージョンのファイルです。\r\n");

		}
		/*fclose(fp);return(-2);*/
		fclose(fp);exit(1);
	}

	memcpy(module,&dat[0x0010],32);					/*module*/
	mdlflag=dat[0x30];						/*id*/
	mutef=dat[0x32];						/*mute*/
	initf=dat[0x33];						/*init*/
	n=dat[0x34];if(n>127){n=-1;}lsp_wait=n;				/*lsp_wait*/
	bend_range=dat[0x36];						/*bend range*/
	card_no[0]=dat[0x38];						/*card_no*/

	for(i=0;i<8;i++){
		memcpy(def_user_exc_memo[i],&dat[0x40+i*48],24);
		memcpy(def_user_exc_data[i],&dat[0x40+i*48+24],24);	/*USER_EXC*/
	}
	memcpy(init_exc_data,&dat[0x01c0],256);				/*INIT_EXC*/

	for(i=0;i<10;i++){n=dat[0x2c0+i];if(n>127){n=-1;}rec_met[i]=n;}	/*REC_MET*/
	for(i=0;i<6;i++){n=dat[0x2ca+i];if(n>127){n=-1;}rhy_stest[i]=n;}/*RHY_AS*/

	for(i=0;i<32;i++){
		memcpy(defrhyna[i],&dat[0x300+i*16],14);
		defrhyno[i][0]=dat[0x300+i*16+14];
		defrhyno[i][1]=dat[0x300+i*16+15];
	}

	for(i=0;i<16;i++){memcpy(card_name[i+1],&dat[0x500+i*48],48);}	/*card*/
	memcpy(&tim_asin2[1],&dat[0x800],32);				/*group*/
	memcpy(&gs_mode[4],&dat[0x820],14*2);				/*gsmode*/
	memcpy(&gs_bank[128*4],&dat[0x840],14*128*2);			/*gsbank*/

	cc=dat[0x1640]*256+dat[0x1641];
	last_no=dat[0x1642]*256+dat[0x1643];
	top_no=dat[0x1644]*256+dat[0x1645];

	ln=fread(dat,1,cc*32,fp);

	ad=0;								/*part*/
	for(i=0;i<cc;i++){
		int n=dat[ad+28]*256+dat[ad+29];
		memcpy(tim_head[n],&dat[ad],20);
		memcpy(tim_sym[n],&dat[ad+20],8);
		tim_top[n]=dat[ad+30]*256+dat[ad+31];
		ad+=32;
	}

	ln=last_no-top_no;
	if(ln){ln=fread(&tim_name[top_no][0],1,ln*15,fp);}		/*tim_name*/

	fclose(fp);

#if	STED
	f=(mdlflag&0x7);if(f>2){f=2;}
	if(f>0 && f<4){rcd->moduletype=f-1;}

	if(mutef){rcd->mute_mode=mutef-1;}
	if(initf){rcd->init_mode=initf-1;}
#endif

#if	CONV
	defsave(fi,md,mutef,initf);
#endif

	return(0);
}

/***************************/
