/*
 * STed: assign.c (assign)
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

static char	*filter_name[14]={
		"PROGRAM CHANGE",
		"PITCH BEND",
		"AFTER TOUCH(CH. )",
		"AFTER TOUCH(POLY)",
		"PEDAL(HOLD1)",
		"MODULATION",
/*		"BREATH",2*/
		"VOLUME",
		"EXPRESSION",
/*		"PANPOT",10*/
		"ALL NOTE OFF",
		"CONTROL CHANGE",
		"EXCLUSIVE MESSAGE",
		"KEY# H:    L:",
		"KEY SHIFT   :",
		"VELOCITY    :",
};
/*
DISPLAY              
GATE TIME (GT)   :
VELOCITY  (VEL)  :
PITCH BEND       :
VOLUME           :
MODULATION       :
EXPRESSION       :
NOTE HEAD        :
NOTE HOOK        :
TIME SCALE x 2   :
QUANTIZE  (6)    :
TEMPO            :
*/

unsigned char	stgt_name[10][4]={
	"�ｿ","�ﾀ","�ﾁ","�ﾂ","�ﾃ","�ﾄ","�ﾅ","�ﾆ","�ﾇ","�ﾉ"};

void	sinput();
int	vinput();
int	inkey2();
char	*fstr();
int	str_val();
char	*hex_s();
int	yn_sel();
char	*keystr();
char	*prog_name();
char	*ctrl_type();
char	*spadd();
void	home2();
void	msg();
void	msg_clr();
void	sdis();
void	tdis();
void	tcur();
void	fnc_dis();
void	rhinit();
void	rhinit2();
void	stgt_init();
void	stgt_init2();
int	ctc();
int	spc_code();
int	meas_no();
int	meas_add();
int	meas_len();
int	real_cy();
void	snsclr();
void	twait();
void	txerase();

int	strch();
char	*chstr();

int	sttm_no();
int	sttm_ad();
int	step_no();
int	step_ad();

int	ras_save();
int	ras_load();

void	path_cut();
int	str_last();
int	str_search();
int	knck();
char	*jstrupr();
void	cons_md();
void	noed_cls();


int	timsel();
int	fsel();
int	varia();
void	s_test();
void	asin_init();

int	rhy_as_sel();
void	rhy_as_para();
int	part_sel();
void	filt_init();

/***************************/
void	rhy_as()
{
	int	a,i,j,xx,x=0,y=0,f,gy,sh;
	char	tmp1[256];
	int	rno[32];

	for(i=0;i<32;i++){rno[i]=rhyno[i][0];}

	gy=scyp;
	fill(0,544,767,1023,0);
top:
	fill(2,666,564,708,14);box(2,708,564,986,14,65535);
	box(4,710,282,984,14,65535);box(284,710,562,984,14,65535);
	box(6,618,200+80,660,14,65535);box(8,620,198+80,658,14,65535);
	home2(512);
	rhy_as_para();

	es=0;B_COLOR(3);

	B_LOCATE(1,10);B_PRINT("RHYTHM ASSIGN");B_LOCATE(1,11);
	B_PRINT(" NO RHYTHM NAME     K#         GT  ");
	B_PRINT(" NO RHYTHM NAME     K#         GT  ");
redis:
	for(i=0;i<32;i++){
		int	a;
		B_LOCATE((i>>4)*35+2,(i&15)+13);
		B_PRINT(fstr(i+1,2));B_PRINT(":");B_PRINT(spadd(rhyna[i],14));
		a=rhyno[i][0];
		if(a==0){
			B_PRINT(" OFF      ");
		}else{
			B_PRINT(fstr(a,4));B_PRINT(" (");
			B_PRINT(keystr(a));B_PRINT(")");
		}
		B_PRINT(fstr(rhyno[i][1],5));
	}

ass_ret:
	while( 1){
		if(x<3){f=0;}else{f=1;}
		i=y+f*16;xx=f*35+5;f=x-f*3;

		j=3;
		if( f==0 ){
			j=14;
		}else{
			if( f==1 ){xx=xx+15;}else{xx=xx+26;}
		}

		tcur(xx,y+13,j,3);
		a=inkey2();sh=B_SFTSNS();
		if(a>='a' && a<='z'){a=a-32;}

		if(a==0x15){
			int a;
			a=rhy_stest[2];if(gs_mode[16]&4){a*=256;}
			s_test(rhy_stest[0],a,rhy_stest[1],rhyno[i][0],rhy_stest[4]);

		}
		if(a=='r'||a=='R'){s_test(11,-1,64,rhyno[i][0],rhy_stest[4]);}
		if(a=='e'||a=='E'){s_test(11,-1,65,rhyno[i][0],rhy_stest[4]);}

		tcur(xx,y+13,j,3);

		if( a==0x05 ){y--;if( y<0 ){y=15;}}
		if( a==0x18 ){y++;if( y>15 ){y=0;}}
		if( a==0x13 ){x--;if( x<0 ){x=5;}}
		if( a==0x04 ){x++;if( x>5 ){x=0;}}
		if( a==0x0b ){y=0;x=0;goto redis;}

		if( a==9 ){
			msg("No.=");j=vinput(f*16+y+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=32){j-=1;x=(j>>4)*3;y=j&15;}

		}
		if( a==12 ){
			msg("アサインを初期化しますか");
			if(yn_sel()>0){msg_clr();rhinit();goto redis;}
		}
		if( a==32 ){
			rhinit2(i);goto redis;
		}
		if(a=='C'){
			int n;
			char	tmp0[64];

			B_LOCATE(6,7);
			strcpy(tmp0,chstr(rhy_stest[0]));
			sinput(tmp0,0x803);n=strch(rhy_stest[0],tmp0);
			if(es==0 && n>=0 && n<=32){rhy_stest[0]=n;}
			rhy_as_para();
		}
		if(a=='B'){
			int n;

			n=rhy_stest[2];
			B_LOCATE(21,7);n=vinput(n,0x803);
			if(es==0 && n>=-1 && n<=127){rhy_stest[2]=n;}
			rhy_as_para();
		}
		if(a=='V'){
			int n;

			n=rhy_stest[4];
			B_LOCATE(29,7);n=vinput(n,0x803);
			if(es==0 && n>=0 && n<=127){rhy_stest[4]=n;}
			rhy_as_para();
		}
		if(a=='G'){
			int n;
			if((cmdflag&0x1000)==0){goto retimsel;}
timnore:
			n=rhy_stest[1];
			if((cmdflag&0x20)!=0 ){n++;}
			B_LOCATE(6,8);n=vinput(n,0xb03);
			if((cmdflag&0x20)!=0 ){n--;}

			if(es==0){
				if(ecode==0x18){
					int a;
retimsel:
/*					if(rhy_stest[2]>=0){a=rhy_stest[2]*256;}*/

					a=0;
					if(rhy_stest[2]>=0){
						a=rhy_stest[2];if(gs_mode[16]&4){a*=256;}
					}

					n=timsel(rhy_stest[0],a,36,rhy_stest[1]);

					if(n<0){goto timnore;}
					if(n>=0){n=n&0xff;}
				}
				if(n>=-1 && n<128){rhy_stest[1]=n;}
			}
			rhy_as_para();
		}

		if( a==13 ){
			int	n;
timnore2:
			B_LOCATE(xx,y+13);
			if( f==0 ){
				sinput(rhyna[i],14);
			}else{
				if( f==1 ){
					b_striS(tmp1,rhyno[i][0]);sinput(tmp1,0xb03);
					if(es==0){
						n=ctc(rhyno[i][0],tmp1,0);
						if(ecode==0x18){
							tdis(xx,(i&15)+13,tmp1,j,11);
							n=rhy_stest[1];if(n<0){n=0;}
							if(rhy_stest[2]>=0){n+=rhy_stest[2]*256;}
							n=timsel(rhy_stest[0]+0x100,n,36,0);
							if(n<0){goto timnore2;}
						}
						if( n>=0 && n<128 ){
							int as,bk;
							rhyno[i][0]=n;
							as=tim_asin[rhy_stest[0]];
							bk=rhy_stest[1];
							if(as==8 || as==10 || as==12 || as==14){
								varia(tmp1,bk,n,as);
								tmp1[15]=0;strcpy(rhyna[i],tmp1+1);
							}
							if(as==2 && n){
								if(n>=24){
									strcpy(rhyna[i],tim_name[cm6[0x130+(n-24)*4]+128]);
								}else{
									rhyna[i][0]=0;
								}
							}
						}
					}
				}else{
					n=vinput(rhyno[i][1],0x803);
					if( n>=0 && n<=255){rhyno[i][1]=n;}
				}
			}
			goto redis;
		}

		if( a>='0' && a<='9' ){
			int	n;
timnore3:
			B_LOCATE(xx,y+13);
			if( f==0 ){
				/*sinput(rhyna[i],14);*/
			}else{
				if( f==1 ){
					tmp1[0]=a;tmp1[1]=0;sinput(tmp1,0x603);
					if(es==0){
						n=ctc(rhyno[i][0],tmp1,0);
						if(ecode==0x18){
							tdis(xx,(i&15)+13,tmp1,j,11);
							n=rhy_stest[1];if(n<0){n=0;}
							if(rhy_stest[2]>=0){n+=rhy_stest[2]*256;}
							n=timsel(rhy_stest[0]+0x100,n,36,0);
							if(n<0){goto timnore3;}
						}
						if( n>=0 && n<128 ){rhyno[i][0]=n;}
					}
				}else{
					n=vinput(a-'0',0x403);
					if( n>=0 && n<=255){rhyno[i][1]=n;}
				}
			}
			goto redis;
		}


		if( (a=='+' || a=='>' || a=='.') && f>0 ){
			int n=rhyno[i][f-1];
			if(sh&1){n+=12;}else{n++;}
			if( f==1 ){n=n&127;}else{n=n&255;}
			rhyno[i][f-1]=n;
			goto redis;
		}
		if( (a=='-' || a=='<' || a==',') && f>0 ){
			int n=rhyno[i][f-1];
			if(sh&1){n+=244;}else{n+=255;}
			if( f==1 ){n=n&127;}else{n=n&255;}
			rhyno[i][f-1]=n;
			goto redis;
		}

		if( a=='L' ){
			char	tmp0[256],tmp1[256],path2[256];

			C_CLS_AL();

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x00b);
			if( es==0 && tmp1[0]!=0){
				strcpy(tmp0,path2);strcat(tmp0,tmp1);
				msg("Rhy.Ass.ファイルのロード中です。");
				if(ras_load(tmp0)==0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
				}
			}
			goto top;
		}
		if( a=='S' ){
			char	tmp0[256],tmp1[256],path2[256];

			C_CLS_AL();

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x10b);
			if( es==0 && tmp1[0]!=0){
				strcpy(tmp0,path2);strcat(tmp0,tmp1);
				msg("Rhy.Ass.ファイルのセーブ中です。");
				if(ras_save(tmp0)==0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
				}
			}
			goto top;
		}

		if(a=='A'){
			int i,n,a;
			msg("リズムネームを一括変更しますか");
			a=yn_sel();msg_clr();if(a==0){goto ass_ret;}

			for(i=0;i<32;i++){
				n=rhyno[i][0];
				if( n>=0 && n<128 /*&& rhyno[i][1]!=0*/){
					int as,bk;
					as=tim_asin[rhy_stest[0]];
					bk=rhy_stest[1];
					if(rhy_stest[2]>=0){bk+=rhy_stest[2]*256;}

					if(as==8 || as==10 || as==12 || as==14){
						varia(tmp1,bk,n,as);
						tmp1[15]=0;strcpy(rhyna[i],tmp1+1);
					}
					if(as==2 && n){
						if(n>=24){
							strcpy(rhyna[i],tim_name[cm6[0x130+(n-24)*4]+128]);
						}else{
							rhyna[i][0]=0;
						}
					}
				}else{
					strcpy(rhyna[i],"");
				}
			}
			goto redis;
		}

		if(a=='X'){
			int ex,n;
			char tmp0[128];
			B_LOCATE(35,6);B_PRINT("ExChange Rhythm Data");
			B_LOCATE(52,7);B_PRINT("Source No.=");B_PRINT(fstr(i+1,2));
			B_LOCATE(48,8);B_PRINT("Distnation No.=");
exc1:			ex=rhy_as_sel(i);if(ex==i){goto exc1;}
			txerase(35*8,6*16,40*8,3*16);

			if(ex>=0){
				n=rhyno[i][0];rhyno[i][0]=rhyno[ex][0];rhyno[ex][0]=n;
				n=rhyno[i][1];rhyno[i][1]=rhyno[ex][1];rhyno[ex][1]=n;

				strcpy(tmp0,rhyna[i]);strcpy(rhyna[i],rhyna[ex]);
				strcpy(rhyna[ex],tmp0);

				goto redis;
			}
		}

		if( a==27 ){
			int i,j,f;
			f=0;
			for(i=0;i<31;i++){
				for(j=i+1;j<32;j++){
					if(rhyno[i][0]==rhyno[j][0] && rhyno[i][0]!=0){
						f++;
					}
				}
			}
			if( f==0 ){break;}
			msg("キーナンバーが重複しています。よろしいですか");
			if(yn_sel()<1){msg_clr();goto redis;}
			break;

		}
	}

	f=0;
	for(i=0;i<32;i++){if(rno[i]!=rhyno[i][0]){f=1;}}

	if(f!=0 && (cmdflag&0x800)==0){
		int a=1;
		if((cmdflag&0x400)==0){
			msg("トラック・データを書き換えますか");
			a=yn_sel();if(a==0){msg_clr();goto ass_ret;}
		}
		if(a>0){
			int	i,j,n;
			msg("処理中...");
			for(i=0;i<36;i++){
				if((trrhy[i]&128)!=0){
					for(j=0;j<tr_len[i];j=j+4){
						unsigned char	a=trk[i][j];
						if(a<128 && a!=0 && trk[i][j+2]>0 && trk[i][j+3]>0){
							n=0;while(n<32){
								if(a==rno[n] && rhyno[n][0]!=0){
									trk[i][j]=rhyno[n][0];
									break;
								}
								n++;
							}
						}
					}
				}
			}
		}
	}
	msg_clr();
	B_CLR_AL();home2(gy);
}

/***************************/
int	rhy_as_sel(int no)
{
	int	a,j,xx,x,y;

	x=no>>4;y=no&15;

	while( 1){
		xx=x*35+1;

		tcur(xx,y+13,3,3);
		a=inkey2();
		tcur(xx,y+13,3,3);

		if( a==0x05 ){y--;if( y<0 ){y=15;}}
		if( a==0x18 ){y++;if( y>15 ){y=0;}}
		if( a==0x13 ){x--;if( x<0 ){x=1;}}
		if( a==0x04 ){x++;if( x>1 ){x=0;}}
		if( a==0x0b ){y=0;x=0;}

		if( a==9 ){
			msg("No.=");j=vinput(x*16+y+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=32){j-=1;x=j>>4;y=j&15;}
		}
		if( a==13 ){return x*16+y;}
		if( a==27 ){return -1;}

	}
}

/***************************/
inline void	rhy_as_para()
{
	B_LOCATE(2,7);B_PRINT("CH.=");B_PRINT(chstr(rhy_stest[0]));
	B_LOCATE(11,7);
if(gs_mode[16]&4){
B_PRINT("BANK LSB=");
}else{
B_PRINT("BANK MSB=");
}
B_PRINT(fstr(rhy_stest[2],3));

	B_LOCATE(25,7);B_PRINT("VEL=");B_PRINT(fstr(rhy_stest[4],3));

	B_LOCATE(2,8);B_PRINT("PRG=");
	B_PRINT(prog_name(rhy_stest[0],rhy_stest[2]*256,rhy_stest[1],1));
}

/***************************/
int	tagj(int *po,int fl)
{
	int	a,i,y=0;
	char	tmp0[128];

	fill(315,566,453,1001,0);
	box(316,588-16,444,999,14,65535);
	box(317,626-16,443,998,14,65535);
	fill(316,588-16,444,626-16,14);

	es=0;
	B_LOCATE(40,4);if(fl==0){B_PRINT("### TAG JUMP ##");}else{B_PRINT("### TAG SET ###");}
	B_LOCATE(40,5);B_PRINT("NO.TR MEAS  ST ");

	B_COLOR(3);

	for(i=0 ;i<23;i++){
		strcpy(tmp0," : ");tmp0[0]=i+'A';if(i==22){tmp0[0]=32;}

		if(tag[i][0]>=0){
			if(tag[i][0]<255){
				strcat(tmp0,fstr(tag[i][0]+1,2));
			}else{
				strcat(tmp0,"--");
			}
			strcat(tmp0,fstr(tag[i][1],5));strcat(tmp0,fstr(tag[i][2],5));
		}
		B_LOCATE(40,i+7);B_PRINT(tmp0);
	}
	while( 1){
		tmp0[0]=0;
		if(tag[y][0]>=0){
			if(tag[y][0]<255){
				strcpy(tmp0,fstr(tag[y][0]+1,3));
			}else{
				strcpy(tmp0," --");
			}
			strcat(tmp0,fstr(tag[y][1],5));strcat(tmp0,fstr(tag[y][2],5));
		}
		tdis(42,y+7,tmp0,13,11);
		a=inkey2();
		tdis(42,y+7,tmp0,13,3);

		if(a>='A' && a<='V'){y=a-'A';}
		if(a>='a' && a<='v'){y=a-'a';}
		if( a==0x05 ){y--;if( y<0 ){y=22;}}
		if( a==0x18 ){y++;if( y>22 ){y=0;}}
		if( a==0x0b ){y=0;break;}
		if( a==13 || a==32 ){es= 0;break;}
		if( a==27 ){es= 1;break;}
	}
	txerase(40*8,5*16-16,20*8,25*16+16);
	fill(315,587-16,445,1001,0);

	if( es==0 ){
		if( fl==0 ){
			if(tag[y][0]>=0){

				tag[22][0]=track;tag[22][1]=meas_no(*po);
				tag[22][2]=sttm_no(*po);

				if(tag[y][0]<255){track=tag[y][0];}
				*po=meas_add(tag[y][1]);
				if(*po<0){
					*po=tr_len[track]-4;
				}else{
					i=tag[y][2];*po=sttm_ad(*po,i);
				}
			}else{es=1;}
		}else{
			if(a==13){tag[y][0]=track;}else{tag[y][0]=255;}
			tag[y][1]=meas_no(*po);tag[y][2]=sttm_no(*po);
		}
	}
	return(es);
}

/***************************/
inline void	part_as()
{
	int	ke,sh,i,j,x=0,y=0,scr,gy;
	char	tmp[128];

	gy=scyp;
	scr=edit_scr;edit_scr=0;

	fill(0,512+32,767,1023,0);
	box(15,116+512,496,411+512,14,65535);
	box(17,133+512,494,409+512,14,65535);
	sdis(2, 84+512," PART ASSIGN",13,15,8);
	sdis(2 ,116+512," CH.  PART NAME",30,15,14);
	sdis(32,116+512," CH.  PART NAME",30,15,14);
	home2(512);
top:
	for(i=1;i<17;i++){
		for(j=0;j<2;j++){
			strcpy(tmp,chstr(i+j*16));
			tdis(3+j*30,8+i,tmp,3,3);

			ke=tim_asin[i+j*16];
			if(ke!=0){strcpy(tmp,tim_head[ke-1]);}
			else{strcpy(tmp,"No Assign");}
			tdis(8+j*30,8+i,tmp,20,3);
		}
	}

	while( 1){
		int nn=y+x*16+1;
		ke=tim_asin[nn];
		if(ke!=0){strcpy(tmp,tim_head[ke-1]);}
		else{strcpy(tmp,"No Assign");}

		tdis(x*30+8,9+y,tmp,20,11);
		ke=inkey2();sh=B_SFTSNS();
		tdis(x*30+8,9+y,tmp,20,3);

		if(sh&4){
			int	ex;
			char	t[32];
			t[0]=ke;t[1]=0;
			ex=str_search("1234567890,./*-+=\x0d",t);
			if(ex>0 && ex<19){ex--;x=ex>>4;y=ex&15;ke=0;}
		}

		if( ke==27 ){break;}
		if( ke==13 ){
			tdis(x*30+8,9+y,tmp,20,10);
			i=part_sel(tim_asin[nn]);
			if(i>=0){tim_asin[nn]=i;}
		}

		if( ke>='0' && ke<='9' ){
			tim_asin[nn]=ke-'0';
		}
		if((ke>='a' && ke<='f')||(ke>='A' && ke<='F')){
			tim_asin[nn]=(ke&15)+9;
		}

		if( ke==0x05 ){y--;if( y<0 ){y=15;}}
		if( ke==0x18 ){y++;if( y>15 ){y=0;}}
		if( ke==0x13 || ke==0x04 ){x++;if( x>1 ){x=0;}}

		if( ke==9 ){
			msg("No.=");j=vinput(x*16+y+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=32){j-=1;x=j>>4;y=j&15;}
		}

		if( ke==12 ){
			msg("アサインを初期化しますか");
			if(yn_sel()>0){msg_clr();asin_init();goto top;}
		}
	}
	edit_scr=scr;
	C_CLS_AL();home2(gy);
}

/***************************/
inline int	part_sel(int yy)
{
	int	ke,i,j,y=yy;
	char	tmp[128];

	box(527,116+512,752,411+512,14,65535);
	box(529,133+512,750,409+512,14,65535);
	sdis(66,84+512," PART SELECT",13,15,8);
	sdis(66,116+512," No.  PART NAME",28,15,14);
	home2(512);

	for(i=0;i<16;i++){
		strcpy(tmp,fstr(i,2));
		tdis(67,9+i,tmp,3,3);

		if(i!=0){strcpy(tmp,tim_head[i-1]);}
		else{strcpy(tmp,"No Assign");}
		tdis(70,9+i,tmp,20,3);
	}

	while( 1){
		if(y!=0){strcpy(tmp,tim_head[y-1]);}
		else{strcpy(tmp,"No Assign");}

		tdis(70,9+y,tmp,20,11);
		ke=inkey2();
		tdis(70,9+y,tmp,20,3);

		if( ke==27 ){y=-1;break;}
		if( ke==13 ){break;}
		if( ke==0x05 ){y--;if( y<0 ){y=15;}}
		if( ke==0x18 ){y++;if( y>15 ){y=0;}}

		if( ke==9 ){
			msg("No.=");j=vinput(y+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=16){j-=1;y=j&15;}
		}

	}
	txerase(527,32,767,511);
	fill(527,512+32,767,1023,0);
	return y;
}

/***************************/
inline void	rec_filter(int md)
{
	int	i,y=0,x=0,gy;
	char	tmp[128];

	gy=scyp;

	if(md==0){
		fill(0,512+32,767,1023,0);
	}else{
		txerase(22*8,9*16,59*8,18*16);
		fill(174,115+512,641,379+512,0);
	}

	box(174,115+512,361,379+512,14,65535);
	box(176,133+512,359,377+512,14,65535);
	sdis(22,116+512," RECORD FILTER",23,15,14);

	box(366,115+512,641,379+512,14,65535);
	box(368,133+512,639,377+512,14,65535);

	box(495,133+512+4,511,377+512-4,14,65535);

	sdis(46,116+512," Control Change Convert",34,15,14);
	sdis(46+16,128+512+16*7+8,"⇒",2,15,-1);

	home2(512);
top:
	for(i=0;i<14;i++){
		int	ke,ka,kb;

		tdis(23,9+i,filter_name[i],18,3);
		ke=rfilt[i][0];
		if(ke){strcpy(tmp,": ON");}else{strcpy(tmp,":OFF");}
		tdis(40,9+i,tmp,4,3);

		ka=rfilt[i+16][0];kb=rfilt[i+16][1];

		if(ka==128){strcpy(tmp,"---  --------");}else{
			strcpy(tmp,fstr(ka,3));strcat(tmp,"  ");
			strcat(tmp,ctrl_type(ka));}
		tdis(41+7,9+i,tmp,13,3);

		if(kb==128){strcpy(tmp,"---  --------");}else{
			strcpy(tmp,fstr(kb,3));strcat(tmp,"  ");
			strcat(tmp,ctrl_type(kb));}
		tdis(41+24,9+i,tmp,13,3);
	}
	tdis(40-10,9+11,fstr(rfilt[11][1],3),3,3);
	tdis(40-4,9+11,fstr(rfilt[11][2],3),3,3);
	tdis(40-4,9+12,fstr(rfilt[12][1]-128,3),3,3);
	tdis(40-4,9+13,fstr(rfilt[13][1],3),3,3);

	while( 1){
		int	ke;

		tcur(41+x*7,9+y,3,3);
		if(x){tcur(41+x*7+17,9+y,3,3);}

		ke=inkey2();
		tcur(41+x*7,9+y,3,3);
		if(x){tcur(41+x*7+17,9+y,3,3);}

		if( ke==27 ){break;}
		if( ke==13 ){
			if(x==0){rfilt[y][0]=1-rfilt[y][0];
				if(rfilt[y][0]){tdis(41,y+9,"ON ",3,3);}

				if(y==11 && rfilt[y][0]){
					int ct;
					char tmp0[64];

					B_LOCATE(41-11,y+9);tmp0[0]=0;
					b_striS(tmp0,rfilt[y][1]);
					sinput(tmp0,0xb04);

					if(es==0){
						ct=ctc(60,tmp0,0);
						if(ct>=0 && ct<=127){rfilt[y][1]=ct;}

						B_LOCATE(41-5,y+9);tmp0[0]=0;
						b_striS(tmp0,rfilt[y][2]);
						sinput(tmp0,0xb04);

						if(es==0){
							ct=ctc(60,tmp0,0);
							if(ct>=0 && ct<=127){rfilt[y][2]=ct;}
						}
					}
				}
				if(y==12 && rfilt[y][0]){
					int ct;
					ct=rfilt[y][1]-128;
					B_LOCATE(41-5,y+9);ct=vinput(ct,0x803);
					if(es==0 && (ct>=-99 && ct<99)){
						rfilt[y][1]=ct+128;}
				}
				if(y==13 && rfilt[y][0]){
					int ct;
					char tmp0[64];

					B_LOCATE(41-5,y+9);tmp0[0]=0;
					b_striS(tmp0,rfilt[y][1]);
					sinput(tmp0,0xb03);

					if(es==0){
						ct=str_val(tmp0);
						if(ct>=0 && ct<=127){rfilt[y][1]=ct;}
					}
				}


			}
			if(x!=0){
				int ct,ctl;
				char tmp0[64];

				B_LOCATE(41+7,y+9);tmp0[0]=0;
				if(rfilt[y+16][0]<128){b_striS(tmp0,rfilt[y+16][0]);}
				sinput(tmp0,0xb05);

				if(es==0){
					ct=spc_code(tmp0,&ctl);
					if(ct==0xeb && ctl>=0){rfilt[y+16][0]=ctl;}

					B_LOCATE(41+24,y+9);tmp0[0]=0;
					if(rfilt[y+16][1]<128){b_striS(tmp0,rfilt[y+16][1]);}
					sinput(tmp0,0xb05);

					if(es==0){
						ct=spc_code(tmp0,&ctl);
						if(ct==0xeb && ctl>=0){rfilt[y+16][1]=ctl;}
					}
				}

			}
			goto top;}

		if( ke==0x05 ){y--;if( y<0 ){y=13;}}
		if( ke==0x18 ){y++;if( y>13 ){y=0;}}

		if( ke==0x13 ){x--;if( x<0 ){x=1;}}
		if( ke==0x04 ){x++;if( x>1 ){x=0;}}

		if( ke==12 ){
			msg("初期化しますか");
			if(yn_sel()>0){
				msg_clr();filt_init();goto top;
			}
		}
	}
	txerase(22*8,9*16,59*8,18*16);home2(gy);
	fill(174,115+512,641,379+512,0);

}

/***************************/
void	filt_init()
{
	int	i;

	for(i=0;i<16;i++){
		rfilt[i][0]=0;rfilt[i][1]=0;
		rfilt[i+16][0]=128;rfilt[i+16][1]=128;
	}
	rfilt[11][1]=127;rfilt[11][2]=0;
	rfilt[12][1]=128;
	rfilt[13][1]=127;

}

/***************************/
void	stgt_as()
{
	int	a,n,i,xx,yy,x=0,y=0,scr,sh;

	scr=edit_scr;edit_scr=0;

	fill(316,566,453,997,0);
	box(316,643,452,996-8,14,65535);
	box(317,644,451,995-8,14,65535);
	fill(316,602,452,644,14);

	es=0;B_COLOR(3);

	B_LOCATE(40,6);B_PRINT("* ST/GT ASSIGN *");

redis:
	B_LOCATE(40,7);
	if(x<2){
		B_PRINT("NO.       ST  GT");
	}else{
		B_PRINT("NO.   ST  GT VEL");
	}

	for(i=0;i<10;i++){
		B_LOCATE(40,i*2+9);
		B_PRINT("F");B_PRINT(fstr(i+1,2));B_PRINT(": ");
		if(x<2){
			B_PRINT(stgt_name[i]);
			B_PRINT(fstr(stgt_tbl[i],5));
			B_PRINT(fstr(stgt_tbl[i+10],4));
		}else{
			B_PRINT(fstr(stgt_tbl[i],3));
			B_PRINT(fstr(stgt_tbl[i+10],4));
			B_PRINT(fstr(stgt_tbl[i+20],4));
		}
	}
	while( 1){
		i=y+x*10;xx=49/*+(x&1)*4*/;if(x){xx+=4;}
		yy=y*2+9;

		tcur(xx,yy,3,3);
		a=inkey2();sh=B_SFTSNS();
		tcur(xx,yy,3,3);

		if( a==0x05 ){y--;if( y<0 ){y=9;/*SKEYSET(0x85);SKEYSET(0x5);*/}}
		if( a==0x18 ){y++;if( y>9 ){y=0;}}
		if( a==0x13 ){x--;if( x<0 ){x=2;}if(x==2||x==1){goto redis;}}
		if( a==0x04 ){x++;if( x>2 ){x=0;}if(x==2||x==0){goto redis;}}
		if( a==0x0b ){y=0;x=0;break;}
/*
		if( a==9 ){
			msg("No.=");j=vinput(y+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=10){y=j-1;}
		}
*/
		if( a==12 ){
			msg("アサインを初期化しますか");
			if(yn_sel()>0){msg_clr();stgt_init();goto redis;}
		}
		if( a==32 ){
			stgt_init2(i);goto redis;
		}

		if( a==13 ){
			B_LOCATE(xx,yy);
			n=vinput(stgt_tbl[i],0x803);
			if(es==0 && n>=0 && ((x<2 && n<=255) || n<=127)){stgt_tbl[i]=n;}
			goto redis;
		}
		if( a>='0' && a<='9' ){
			B_LOCATE(xx,yy);
			n=vinput(a-'0',0x403);
			if(es==0 && n>=0 && ((x<2 && n<=255) || n<=127)){stgt_tbl[i]=n;}
			goto redis;
		}

		if( a=='+' || a=='>' || a=='.' ){
			int n=stgt_tbl[i];
			if(sh&1){n+=12;}else{n++;}

			n=n&255;
			if(x==2){n=n&127;}
			stgt_tbl[i]=n;
			goto redis;
		}
		if( a=='-' || a=='<' || a==',' ){
			int n=stgt_tbl[i];
			if(sh&1){n+=244;}else{n+=255;}
			n=n&255;
			if(x==2){n=n&127;}
			stgt_tbl[i]=n;
			goto redis;
		}

		if( a==27 ){break;}
	}

	edit_scr=scr;
	txerase(40*8,5*16,16*8,25*16);
	fill(316,537,453,997,0);

}

/***************************/
