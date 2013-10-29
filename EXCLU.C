/*
 * STed: exclu.c (selecter)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdio.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

static	char	uxcc[5][3]={"gt","ve","ch","cs","ss"};
static	char	dotp[4][3]={"�F","�G","�H","�I"};

static unsigned char	dotpat[32][32];

void	sinput();
int	vinput();
int	inkey2();
void	spcut();
char	*sread();
char	*fstr();
char	*hex_s();
int	str_val();
int	yn_sel();
char	*spadd();
int	knck();
char	*jstrupr();
void	home2();
void	msg();
void	msg_clr();
void	sdis();
void	tdis();
void	tcur();
void	fnc_dis();
void	snsclr();

int	trk_check();
int	trk_shift();
void	trk_delete();


void	all_note_off();
void	midi_clr();

void	hedset();
void	thedset();
int	rcp_buf_put();
int	rcp_buf_get();



void	H_PRINT2();
void	scinst_dsp();
void	scinst_dsp();
void	scpanel_dsp();

void	cons_md();
void	noed_cls();
int	rex_load();
int	rex_save();
void	user_exc_init();
void	user_exc_init2();

void	user_exc_as();
void	user_exc_dis();
void	exc_edit();
void	exc_dis();
void	excins();
void	excdel();
void	excsum();
int	exc_rec();
int	exc_load();
int	exc_save();
void	prn_gspanel();

int	dot_ed();
void	dot_ctrl();
void	dot_dis();
void	dattopat();
void	pattodat();

/***************************/
void	user_exc_as(int no)
{
	int	gy,ke,kc,sh,i,x=0,y=no*2,yy,nn,mm,a,cc;

	gy=scyp;
top:
	fill(0,512+32,767,1023,0);
	box(47,47+512,688+24,458+512,14,65535);
	fill(47,63+512,73,458+512,14);
	sdis(6,48+512," USER DEFINE EXCLUSIVE",80+3,15,14);
	home2(512);fnc_dis(4+3);
/*
[F1] gt : SEND GATE POSITION DATA   [F4] cs : CLEAR CHECK SUM
[F2] ve : SEND VELO POSITION DATA   [F5] ss : SEND CHECK SUM
[F3] ch : SEND CHANNLE No.          [F6] F7 : END OF EXCLUSIVE
*/

	for(i=0;i<7;i++){line(47,520+7*16+i*48,688+24,520+7*16+i*48,14,65535);}
redis:
	for(i=0;i<8;i++){
		B_LOCATE(6,5+i*3);B_PRINT(fstr(i,2));
		B_PRINT("  MEMO:");B_PRINT(user_exc_memo[i]);
		B_LOCATE(39,5+i*3);B_PRINT("|");
		user_exc_dis(i);
	}

	while( 1){
		nn=y>>1;mm=y&1;yy=nn*3+mm+5;cc=3;
		B_LOCATE(50,3);
		if(mm==0){
			B_PRINT("---");
			tcur(15,yy,24,3);
		}else{
			a=user_exc_data[nn][x];
			if(a>0x7f){cc=1;B_PRINT("---");}else{B_PRINT(fstr(a,3));}
			tcur(13+x*3,yy,4,cc);
		}

		while(B_KEYSNS()==0){}
		ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;sh=B_SFTSNS();
		if(kc>='a' && kc<='z'){kc-=32;}
		if(mm==0){tcur(15,yy,24,3);}else{tcur(13+x*3,yy,4,cc);}


		if( (ke>=0x40 && ke<=0x51) && (sh&4)!=0 ){
			int	j;

		static char tk[18]={13,14,15,7,8,9,16,4,5,6,17,1,2,3,18,10,11,12};

			j=tk[ke-0x40];
			if( j>0 && j<=7){y=j*2;kc=0;ke=0;}
		}


		if( ke==1 ){break;}
		if( ke==0x3f ){
			msg("エクスクルーシブを初期化しますか");
			if(yn_sel()>0){msg_clr();user_exc_init();C_CLS_AL();goto redis;}
		}
		if( kc==32 ){
			user_exc_init2(nn);C_CLS_AL();goto redis;
		}
		if( kc==13 ){
			if(sh&1){
				if(mm!=0){
					excins(user_exc_data[nn],23,x);
					user_exc_data[nn][x]=0;
					user_exc_dis(nn);
				}
			}else{
				if(mm==0){
					B_LOCATE(15,yy);sinput(user_exc_memo[nn],24);
					fnc_dis(4+3);
				}else{
					int	c;
					B_LOCATE(14+x*3,yy);
					c=vinput(0,0x2003);
					if(es==0 && c>=0 && c<128){
						excins(user_exc_data[nn],23,x);
						user_exc_data[nn][x]=c;
					}
					user_exc_dis(nn);ke=0x3d;kc=0;
				}
			}
		}

		if(mm!=0){
			if((kc>='0'&& kc<='9')||(kc>='A'&& kc<='F')){
				int	c,cc;
				B_LOCATE(14+x*3,yy);
				if(kc<'A'){cc=kc-'0';}else{cc=kc-'A'+10;}
				c=vinput(cc,0x2003);

				if(es==0 && c>=0 && c<128){
					if( (B_SFTSNS()&4096)!=0 || user_exc_data[nn][x]==0xf7){
						excins(user_exc_data[nn],23,x);
					}
					user_exc_data[nn][x]=c;
					ke=0x3d;kc=0;
				}
				user_exc_dis(nn);
			}
			if((kc=='+' || kc=='.' || kc=='>') && a<128){
				if( kc=='>' ){a+=16;}else{a++;}
				user_exc_data[nn][x]=a&127;
				user_exc_dis(nn);ke=0;kc=0;
			}
			if((kc=='-' || kc==',' || kc=='<') && a<128 ){
				if( kc=='<' ){a+=112;}else{a+=127;}
				user_exc_data[nn][x]=a&127;
				user_exc_dis(nn);ke=0;kc=0;
			}

			if( ke==0x37 || kc==7 ){
				excdel(user_exc_data[nn],23,x);
				user_exc_data[nn][23]=0xf7;
				user_exc_dis(nn);
			}
			if( x>0 &&(ke==0x0f ||kc==8)){
				x--;
				excdel(user_exc_data[nn],23,x);
				user_exc_data[nn][23]=0xf7;
				user_exc_dis(nn);
			}
			if( ke>=0x63 && ke<=0x67){
				excins(user_exc_data[nn],23,x);
				user_exc_data[nn][x]=ke-0x63+0x80;
				user_exc_dis(nn);
			}
			if( ke==0x68){
				user_exc_data[nn][x]=0xf7;
				user_exc_dis(nn);
			}

			if( ke==0x6a && x>5){
				excins(user_exc_data[nn],23,x);
				excsum(user_exc_data[nn],x);
				user_exc_dis(nn);
			}

			if( ke==0x10 /*|| ke==0x35*/ ){
				unsigned char c=user_exc_data[nn][x];
				excins(user_exc_data[nn],23,x);
				user_exc_data[nn][x]=c;
				user_exc_dis(nn);
			}

		}

		if( kc=='U' || kc=='u' ){
			int	c,i;
			msg("User Exc. No.=");
			c=vinput(0,0x803);msg_clr();
			if(es==0 && c>=0 && c<8){
				for(i=0;i<24;i++){
					user_exc_data[nn][i]=user_exc_data[c][i];
					user_exc_memo[nn][i]=user_exc_memo[c][i];
				}
				goto redis;
			}
		}

		if( ke==0x6b ){
			char	tmp0[256],tmp1[256],path2[256];

			C_CLS_AL();

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x009+(sh&1));
			if( es==0 && tmp1[0]!=0){
				int ec;
				strcpy(tmp0,path2);strcat(tmp0,tmp1);

				msg("Usr.Exc.ファイルのロード中です。");
				if((sh&1)==0){
					ec=rex_load(tmp0,-1);
				}else{
					ec=rex_load(tmp0,nn);
				}
				if(ec==0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
				}
			}
			goto top;
		}
		if( ke==0x6c ){
			char	tmp0[256],tmp1[256],path2[256];

			C_CLS_AL();

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x109+(sh&1));
			if( es==0 && tmp1[0]!=0){
				int	ec;
				strcpy(tmp0,path2);strcat(tmp0,tmp1);

				msg("Usr.Exc.ファイルのセーブ中です。");
				if((sh&1)==0){
					ec=rex_save(tmp0,-1);
				}else{
					ec=rex_save(tmp0,nn);
				}
				if(ec==0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
				}
			}
			goto top;
		}

		if( kc==0x13 || ke==0x3b){x--;if(x<0){x=23;}}
		if( kc==0x04 || ke==0x3d){x++;if(x>23){x=0;}}
		if( kc==0x05 || ke==0x3c){y--;if(y<0){y=15;}}
		if( kc==0x18 || ke==0x3e){y++;if(y>15){y=0;}}

	}
	fnc_dis(-1);C_CLS_AL();home2(gy);
}

/***************************/
inline void	user_exc_dis(int no)
{
	int	i,a;

	B_LOCATE(11,6+no*3);
	B_COLOR(9);B_PRINT("F0");B_COLOR(3);

	i=0;
	while(i<24 && user_exc_data[no][i]!=0xf7){
		B_PRINT(" ");
		a=user_exc_data[no][i++];

		if(a>=0x80 && a<=0x84){
			B_COLOR(1);B_PRINT(uxcc[a-0x80]);
		}else{
			B_COLOR(3);B_PRINT(hex_s(a,2));
		}

	}
	B_PRINT(" ");B_COLOR(9);B_PRINT("F7");B_COLOR(1);
	while(i<24 ){user_exc_data[no][i++]=0xf7;B_PRINT(" ..");}
	B_COLOR(3);snsclr();
}

/***************************/
void	exc_edit(int ad,int cy,int sw)
{
	int	ke,sh,kc,i,x=0,y=0,cx,gx,a,cc,co,po,nn;
	int	e=edit_scr;
	unsigned char	*buf;buf=dat+(16*1024);

	trk_dis(cy+6,ad,24-cy);

	if(edit_scr==0){cx=41;gx=328;}else{cx=1;gx=8;}
	cons_md(0);noed_cls();edit_scr=0;

top0:
	for(i=0;i<32*1024;i++){dat[i]=0xf7;}

	po=ad;co=0;
	while(trk[track][po+4]==0xf7){po+=4;
		dat[co]=trk[track][po+2];if(dat[co]==0xf7){break;}
		co++;
		dat[co]=trk[track][po+3];if(dat[co]==0xf7){break;}
		co++;
	}

top:
	box(gx-1,624-48,gx+417+16,997,14,65535);
	fill(gx,639-32,gx+15,996,8);
	fill(gx,639-40,gx+416+16,639-16-8,8);
	sdis(cx,624-48," TRACK EXCLUSIVE EDIT",54,15,14);
	for(i=0;i<22;i++){tdis(cx,7+i,hex_s(i,2),2,3);}
	for(i=0;i<16;i++){sdis(cx+i*3+7,512+6*16-8,hex_s(i,1),1,3,8);}
	fnc_dis(4);

	if(sw&6){co=dot_ed(cx,co);ke=0;goto ext;}

	exc_dis(i,cx,co,0);

	while( 1){
		cc=3;nn=y*16+x;
		a=dat[nn];

		B_LOCATE(cx+29,4);if(a>0x7f){cc=1;B_PRINT("---");}else{B_PRINT(fstr(a,3));}

		tcur(cx+5+x*3,y+7,4,cc);
		while(B_KEYSNS()==0){}
		ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;sh=B_SFTSNS();
		if(kc>='a' && kc<='z'){kc-=32;}
		tcur(cx+5+x*3,y+7,4,cc);

		if( ke==1 ){break;}

		if( kc==13 ){
			if(sh&1){
				excins(dat,co,nn);
				dat[nn]=0;co++;
				exc_dis(y,cx,co,1);ke=0x3d;kc=0;
			}else{
				if( co<256+96 && nn<=co ){
					int	c;
					B_LOCATE(cx+5+x*3,y+7);
					c=vinput(0,0x2003);
					if(es==0 && c>=0 && c<128){
						excins(dat,co,nn);
						dat[nn]=c;co++;
					}
					exc_dis(y,cx,co,1);ke=0x3d;kc=0;
				}
			}
		}

		if(((kc>='0'&& kc<='9')||(kc>='A'&& kc<='F'))&& co<256+96 && nn<=co){
			int	c,cc;
			B_LOCATE(cx+5+x*3,y+7);
			if(kc<'A'){cc=kc-'0';}else{cc=kc-'A'+10;}
			c=vinput(cc,0x2003);

			if(es==0 && c>=0 && c<128){
				if( (B_SFTSNS()&4096)!=0 || dat[nn]==0xf7){
					excins(dat,co,nn);co++;
				}
				dat[nn]=c;
				ke=0x3d;kc=0;
			}
			exc_dis(y,cx,co,1);
		}

		if((kc=='+' || kc=='.' || kc=='>') && a<128){
			if( kc=='>' ){a+=10;}else{a++;}
			dat[nn]=a&127;
			exc_dis(y,cx,co,2);ke=0;kc=0;
		}
		if((kc=='-' || kc==',' || kc=='<') && a<128){
			if( kc=='<' ){a+=118;}else{a+=127;}
			dat[nn]=a&127;
			exc_dis(y,cx,co,2);ke=0;kc=0;
		}

		if( ke==0x37 || kc==7 ){
			excdel(dat,co,nn);
			if(nn<co){co--;dat[co]=0xf7;}
			exc_dis(y,cx,co,1);
		}
		if( nn>0 && (ke==0x0f || kc==8)){
			nn--;
			excdel(dat,co,nn);
			if(nn<co){co--;dat[co]=0xf7;}
			exc_dis(y,cx,co,1);ke=0x3b;
		}
		if( ke>=0x63 && ke<=0x67 && co<256+96){
			excins(dat,co,nn);
			dat[nn]=ke-0x63+0x80;

			if(nn<co+1){co++;}
			exc_dis(y,cx,co,1);
		}
		if( ke==0x68){
			dat[y*16+x]=0xf7;exc_dis(y,cx,co,0);
		}
		if( ke==0x6a && co<256+96 && nn>5){
			excins(dat,co,nn);
			excsum(dat,nn);
			if(nn<co+1){co++;}
			exc_dis(y,cx,co,1);
		}

		if( ke==0x10 && co<256+96){
			unsigned char c=dat[nn];
			excins(dat,co,nn);
			dat[nn]=c;
			if(nn<co+1){co++;}
			exc_dis(y,cx,co,1);
		}

		if( kc=='$' && co<256+96){
			char	tmp0[64];
			int	i=0;

			tmp0[0]=0;
			msg("strings:");sinput(tmp0,0x8000+32);
			if( es==0 && tmp0[0]!=0){
				while(tmp0[i]!=0 && co<256+96){
					excins(dat,co,nn);
					if(tmp0[i]<32){dat[nn]=32;}else{dat[nn]=tmp0[i];}

					if(nn<co+1){co++;}
					i++;
					exc_dis(y,cx,co,1);

					x++;
					if(x>15){if(y<21){x=0;y++;}else{x=15;}}
					nn=y*16+x;
				}
			}
		}

		if( kc=='^' || ke==0x34 ){co=dot_ed(cx,co);x=0;y=0;goto top;}

		if( kc=='U' || kc=='u' ){
			int	c,i;
			msg("User Exc. No.=");
			c=vinput(0,0x803);msg_clr();
			if(es==0 && c>=0 && c<8){
				for(i=0;i<24;i++){
					dat[i]=user_exc_data[c][i];
				}
				dat[24]=0xf7;

				co=0;
				while( dat[co] != 0xf7 && co<24) {co++;}
				dat[co++]=0xf7;

				goto top;
			}
		}

		if( ke==0x6b ){
			char	tmp0[256],tmp1[256],path2[256];

			txerase(cx*8,4*16,53*8,26*16);

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x008+e*0x200);
			if( es==0 && tmp1[0]!=0){
				int ec;
				strcpy(tmp0,path2);strcat(tmp0,tmp1);

				msg("Exclusiveファイルのロード中です。");
				ec=exc_load(tmp0);
				if(ec>=0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
					co=ec;ke=0xff;break;}
			}
			goto top;
		}
		if( ke==0x6c ){
			char	tmp0[256],tmp1[256],path2[256];

			txerase(cx*8,4*16,53*8,26*16);

			strcpy(path2,prt_path);tmp1[0]=0;
			fsel(tmp1,path2,0x108+e*0x200);
			if( es==0 && tmp1[0]!=0){
				int ec;
				strcpy(tmp0,path2);strcat(tmp0,tmp1);

				msg("Exclusiveファイルのセーブ中です。");
				ec=exc_save(tmp0,co);
				if(ec>=0 ){msg_clr();
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
				}
			}
			goto top;
		}
/*
		if( kc=='R' || kc=='r' ){
			int ec;

			msg("MIDI IN Exclusive INPUT");

			ec=exc_rec();
			if(ec>=0 ){msg_clr();
				co=ec;ke=0xff;break;}

			goto top;
		}
*/

		if( ke==0x3a && co>0){break;}

		if( ke==0x36){x=0;y=0;}



		if(kc=='@'){
/*
			0x10 0x01～5 0x00 / 0x10 0x01～5 0x40
*/
			if( dat[0]==0x41 && dat[1]==0x10 && dat[2]==0x45 && dat[3]==0x12 &&
			    dat[4]==0x83 && dat[5]==0x10 ){
				if(dat[7]==0x00){
					if(dat[6]>0){
						prn_gspanel(cx);scpanel_dsp(cx+13,18,&dat[8]);
					}else{
						prn_gspanel(cx);scinst_dsp(cx+13,16,&dat[8]);
					}
					while(inkey2()==0){}
					goto top;
				}
			}
		}



		if( kc==0x13 || ke==0x3b){x--;
			if(x<0){if(y>0){x=15;ke=0x3c;}else{x=0;}}
		}
		if( kc==0x04 || ke==0x3d){x++;
			if(x>15){if(y<21){x=0;ke=0x3e;}else{x=15;}}
		}
		if( kc==0x05 || ke==0x3c){y--;if(y<0){y=21;}}
		if( kc==0x18 || ke==0x3e){y++;if(y>21){y=0;}}
	}
ext:

	dat[co]=0xf7;co++;if(co&1){dat[co]=0xf7;co++;}
	if(dat[0]==0xf7){co=0;}
	po=ad;cc=0;while(trk[track][po+4]==0xf7){po+=4;cc+=4;}

	{
		unsigned char	c1,c2;
		int bc=0,i=0;

		while(i<co){
			c1=dat[i];c2=dat[i+1];
			buf[bc]=0xf7;buf[bc+1]=0;buf[bc+2]=c1;buf[bc+3]=c2;

			if(c1==0xf7){buf[bc+3]=0xf7;}

			if(c1==0xf0 || c2==0xf0){
				if(c1==0xf0){
					buf[bc+2]=0xf7;buf[bc+3]=0xf7;
					i-=1;
				}else{
					buf[bc+3]=0xf7;
				}
				bc+=4;
				buf[bc]=0x98;buf[bc+1]=0;
				buf[bc+2]=0;buf[bc+3]=0;
			}
			bc+=4;i+=2;
		}

		if(bc<=cc || trk_check(bc-cc)==0){
			int	po=ad+4,i;

			trk_delete(po,cc);trk_shift(po,bc);
			for(i=0;i<bc;i++){trk[track][po+i]=buf[i];}

			if( ke==0x3a ){poplay(ad,8);
				if( (sh&1)!=0 ){
					twait(50);
					if( rcd->flg_gsinfo || rcd->flg_gsinst || rcd->flg_gspanel ) {
						prn_gspanel(cx);
						if( rcd->flg_gsinfo) {
							rcd->flg_gsinfo=0;
							scinst_dsp(cx+13,16,rcd->gs_info);
						}
						if( rcd->flg_gsinst) {
							rcd->flg_gsinst=0;
							scinst_dsp(cx+13,16,rcd->gs_inst);
						}
						if( rcd->flg_gspanel ) {
							rcd->flg_gspanel=0;
							scpanel_dsp(cx+13,18,rcd->gs_panel);
						}
						while(inkey2()==0){}
					}
				}
			}
		}
	}

	if( ke==0x3a || ke==0xff ){goto top0;}

	edit_scr=e;
	fnc_dis(0);/*txerase(cx*8,4*16,53*8,26*16);*/noed_cls_t();noed_cls();cons_md(1);


}

void	excsum(unsigned char *da,int nn)
{
	int	top=4,i,sum=0;

	if(da[0]==0x43){top--;}
	for(i=top;i<nn;i++){sum+=da[i];}
	da[nn]=(128-(sum & 0x7f))&0x7f;
}

void	excins(unsigned char *da,int co,int nn)
{
	int	i;

	for(i=co;i>nn;i--){da[i]=da[i-1];}
}

void	excdel(unsigned char *da,int co,int nn)
{
	int	i;

	for(i=nn;i<co;i++){da[i]=da[i+1];}
}

/************************** RCM2  MAX=263 +9+1  272*/
void	exc_dis(int no,int cx,int co,int m)
{
	int	i,a=0;

	B_LOCATE(cx+3,7);B_COLOR(9);B_PRINT("F0");B_COLOR(3);

	if(m==0){no=0;}

	while(no<=16+6){
		int nn=no*16;

		if(m!=0 && nn!=co+1 && a==0xf7){break;}

		B_LOCATE(cx+5,7+no);

		i=0;
		while(i<16 && nn<=co){
			B_PRINT(" ");
			a=dat[nn++];i++;

			if(a>=0x80 && a<=0x84){
				B_COLOR(1);B_PRINT(uxcc[a-0x80]);
			}else{
				if(a<0x80){B_COLOR(3);}else{B_COLOR(9);}
				B_PRINT(hex_s(a,2));if(a>=0x80){B_COLOR(3);}
			}

			if(a==0xf7){break;}
		}
		B_COLOR(1);
		while(i<16 && nn<=256+96){dat[nn++]=0xf7;B_PRINT(" ..");i++;}
		if(m==2){break;}
		no++;
	}
	B_COLOR(3);snsclr();
}

/**************************19971028/29/30*/
/*
int	exc_rec()
{
	unsigned char	buf[8*1024];
	int	ad,ln,pp,cc,a,bpo,i,fl;

	all_note_off(1);hedset(36);bpo=rcp_buf_put(hed,0,1414);
	for(i=0;i<36;i++){thedset(i,48);hed[44]=0xfe;bpo=rcp_buf_put(hed,bpo,48);}}
	rcd->fmt=2;rcd->totalcount=bpo;
	rcd->data_valid=1;rcd->word_valid=0;rcd->tone_valid=0;rcd->gsd_valid=0;

	(*rcd->init)();midi_clr();

	ln=0;

	while(1){
		a=(*rcd->md_get)();
		if(a>=0 && a<0xf8){
			if( INPOUT(0xfe)!=0 ){if(INPOUT(0xff)==27){break;}}

			if(a==0xf0){fl=1;}
			if(fl){dat[ln++]=a;}
			if(a==0xf7){fl=0;}

			if(ln>=32*1024+512){msg("バッファが一杯になりました");break;}
		}
	}

	strcpy(buf,dat,ln);
	pp=0;cc=0;

	while( buf[pp]==0xf0 && pp<ln) {
		if(cc==0){pp++;}
		while( buf[pp] != 0xf7 && pp<ln) {dat[cc++]=buf[pp++];}
		pp++;
	}
	dat[cc]=0xf7;

	return(cc);
}
*/
/***************************/
int	exc_load(char *fna)
{
	char	tmp0[2048],li[1024];
	unsigned char	buf[8*1024];

	int	ad,ln,n,j,pp,cc;
	unsigned char	a;
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	ln=fread(dat,1,32*1024,fp);fclose(fp);
	j=0;ad=0;

	while(ln>0){
		n=0;
 		while(1){
 			a=dat[ad++];
			if(ad>=ln){ln=fread(dat,1,32*1024,fp);ad=0;if(ln==0){break;}}
			if(a<32){
 				if(a==13){break;}
				a=32;
			}else{
				if(a=='*'){a=0;}
			}
			li[n++]=a;
		}
		li[n]=0;li[1023]=0;
		if(dat[ad]==0x0a){ad++;
			if(ad>=ln){ln=fread(dat,1,32*1024,fp);ad=0;}
		}

		spcut(li);
		if( li[0]!=0 ){
			char	tmp9[128];
			int	i=0;

			strcpy(tmp0,li);jstrupr(tmp0);
			spcut(tmp0);

			while(i<strlen(tmp0)){
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

					buf[j++]=a;if(j>=8*1024){goto skip;}
					i=i+2;
				}
			}
		}
	}
skip:
	fclose(fp);

	pp=0;cc=0;

	while( buf[pp]==0xf0 && pp<j) {
		if(cc==0){pp++;}
		while( buf[pp] != 0xf7 && pp<j) {dat[cc++]=buf[pp++];}
		pp++;
	}
	dat[cc]=0xf7;

	return(cc);
}

/***************************/
int	exc_save(char *fna,int co)
{
	int	sz,i;
	unsigned char	buf[4096];
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	strcpy(buf,"F0 ");dat[co++]=0xf7;

	for(i=0;i<co;i++){
		unsigned char a=dat[i];

		if(a>=0x80 && a!=0xf7){
			if(a>=0x80 && a<=0x84){strcat(buf,uxcc[a-0x80]);strcat(buf," ");}

			if(a==0xf0){
				strcat(buf,"F7\r\n\r\n");
				fwrite(buf,1,strlen(buf),fp);buf[0]=0;
				strcat(buf,"F0 ");
			}
		}else{
			strcat(buf,hex_s(dat[i],2));strcat(buf," ");
		}

		sz=strlen(buf);
		if(sz>=16*3){
			buf[sz-1]=0;strcat(buf,"\r\n");fwrite(buf,1,sz+1,fp);buf[0]=0;
		}
	}
	strcat(buf,"\r\n");fwrite(buf,1,strlen(buf),fp);
	fclose(fp);

	return(0);
}

/***************************/
void prn_gspanel( int cx )
{
	struct	TXFILLPTR	*t,buf;

	t=&buf;

	t->x = (cx+11)*8  ;t->y = 15*16+8;t->x1 = 36*8;t->y1 = 9*16;
	t->vram_page = 0;t->fill_patn = 0xffff;TXFILL( t );
	t->vram_page = 1;t->fill_patn = 0x0000;TXFILL( t );

	t->x = (cx+11)*8+2;t->y = 15*16+8+2;t->x1 = 36*8-4;t->y1 = 9*16-4;
	t->vram_page = 0;t->fill_patn = 0x0000;TXBOX( t );

	t->x = (cx+13)*8  ;t->y = 18*16;t->x1 = 32*8;t->y1 = 6*16;
	t->vram_page = 0;t->fill_patn = 0x0;TXFILL( t );
	t->vram_page = 1;t->fill_patn = 0xffff;TXFILL( t );

	/*t->x = (cx+13)*8*/  ;t->y = 16*16;t->x1 = 32*8;t->y1 = 16;
	t->vram_page = 0;t->fill_patn = 0x0;TXFILL( t );
	t->vram_page = 1;t->fill_patn = 0xffff;TXFILL( t );

}

/***************************/

/***************************/
/*970112 bar display edit*/
int	dot_ed(int dx,int co)
{
	int	ke,kc,sh,cx,cy,i,j,gx,pmode=1;
	int	msf=-1;

	for(i=0;i<16;i++){for(j=0;j<16;j++){dotpat[i][j]=0;}}

/*		0x10 0x01～5 0x00 / 0x10 0x01～5 0x40	*/

	if( dat[0]==0x41 && dat[1]==0x10 && dat[2]==0x45 && dat[3]==0x12 &&
	    dat[4]==0x83 && dat[5]==0x10 && dat[6]>0 && dat[7]==0x00){
		dattopat();
	}else{
		if(co==0){
			dat[0]=0x41;dat[1]=0x10;dat[2]=0x45;dat[3]=0x12;
			dat[4]=0x83;dat[5]=0x10;dat[6]=1;dat[7]=0x00;
		}else{
			msg("GS BAR DISPLAY DATAではありません");return co;
		}
	}

	gx=dx*8;
	cx=0;cy=0;

	txerase(dx*8,15*16,53*8,15*16);
	fill(gx+31-8,512+240,gx+360+32,995,0);

	box(gx+31-8,512+240+8,gx+360+64,995-6,14,65535);
	sdis(dx+4-1,512+240+8," GS BAR DISPLAY EDIT",48+2,15,14);

	if(cnfflag&0x8000){
		fill(gx+64-16,512+288-16,gx+63+16*16+16,512+287+8*16+16,14);
		fill(gx+64   ,512+288   ,gx+63+16*16   ,512+287+8*16   ,0);

		for(i=0;i<17;i++){
			line(gx+64,512+287+i*8, gx+63+16*16,512+287+i*8, 4,65535);
			line(gx+63+i*16,512+288, gx+63+i*16,512+287+8*16, 4,65535);
		}
		B_COLOR(3);
	}else{
/*		GPALET(2,0x97c0);
		TPALET(2,0x7700);*/

		GPALET(2,0x7700);
		TPALET(2,0x4400);

		fill(gx+64-16,512+288-16,gx+63+16*16+16,512+287+8*16+16,14-12);
		fill(gx+64   ,512+288   ,gx+63+16*16   ,512+287+8*16   ,0);

		for(i=0;i<17;i++){
			line(gx+64,512+287+i*8, gx+63+16*16,512+287+i*8, 4-2,65535);
			line(gx+63+i*16,512+288, gx+63+i*16,512+287+8*16, 4-2,65535);
		}
		B_COLOR(2);
	}

	fnc_dis(-1);

	dot_dis(dx);
if((cmdflag&0x200)==0){
	MS_INIT();MS_LIMIT(gx,80,gx+432,511-16-8);MS_CURON();
}

	while( 1){
		int ma=0,mx,my;
/*top:*/
		box(gx+cx*16+63,cy*8+287+512,gx+63+cx*16+16,cy*8+287+512+8,6,65535);
/*
		B_LOCATE(dx+8,26);B_PRINT("X=");B_PRINT(fstr(cx,2));
		B_LOCATE(dx+8,27);B_PRINT("Y=");B_PRINT(fstr(cy,2));
*/
		while(B_KEYSNS()==0){
			/*ma=MS_GETDT()&0xff00;mx=MS_CURGT();my=mx&0xffff;mx=mx>>16;*/
			ma=MS_GETDT()&0xff00;mspos(&mx,&my);
			if(ma==0){msf=-1;}
			if(ma /*&& msf!=0*/){break;}
			if(BITSNS(14)&12){break;}
		}

		ke=0;kc=0;sh=B_SFTSNS();
		if(B_KEYSNS()!=0){
			ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;
		}

		if(cnfflag&0x8000){
			box(gx+cx*16+63,cy*8+287+512,gx+63+cx*16+16,cy*8+287+512+8,4,65535);
		}else{
			box(gx+cx*16+63,cy*8+287+512,gx+63+cx*16+16,cy*8+287+512+8,4-2,65535);
		}

		if(ma!=0 && msf!=0){
			ke=0;kc=0;msf=0;mx-=gx;
			if(mx>=8*8 && mx<8*8+16*16 && my>=18*16 && my<18*16+8*16 ){
				cx=(mx-8*8)>>4;cy=(my-18*16)>>3;
				pmode=1-dotpat[cx][cy];goto skip;
			}
		}
		if(ma!=0 && msf==0){
			int a;
			ke=0;kc=0;msf=0;mx-=gx;
			if(mx>=8*8 && mx<8*8+16*16 && my>=18*16 && my<18*16+8*16 ){
				cx=(mx-8*8)>>4;cy=(my-18*16)>>3;
skip:
				dotpat[cx][cy]=pmode;
				B_LOCATE(dx+8+cx*2,(cy>>1)+18);
/*				B_PRINT(dotp[dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2]);*/
				a=dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2;
				if(cnfflag&0x8000){a=3-a;}
				B_PRINT(dotp[a]);

			}
		}

		if( ke==1 || ke==0x34 ){pattodat();break;}

		if( ke==0x3a ){
			char buf[128];
			int sum=0;
			pattodat();
			buf[0]=0xf0;

			for(i=0;i<4;i++){buf[i+1]=dat[i];}
			for(i=5;i<72;i++){buf[i]=dat[i];sum=sum+dat[i];}
			buf[72]=(128-(sum&0x7f))&0x7f;
			buf[73]=0xf7;buf[74]=0xff;

			(*rcd->mix_out)(buf);
		}

		if( kc==0x13||ke==0x3b ||ke==0x43||ke==0x47||ke==0x4B ){cx--;if(cx<0){cx=15;}}
		if( kc==0x04||ke==0x3d ||ke==0x45||ke==0x49||ke==0x4D ){cx++;if(cx>15){cx=0;}}
		if( kc==0x05||ke==0x3c ||ke==0x43||ke==0x44||ke==0x45 ){cy--;if(cy<0){cy=15;}}
		if( kc==0x18||ke==0x3e ||ke==0x4B||ke==0x4C||ke==0x4D ){cy++;if(cy>15){cy=0;}}
/*
		if( ke==0x36 ){cx=0;cy=0;}
*/
		if( (sh&4)!=0 ){
			int a;
			dotpat[cx][cy]=1;
			B_LOCATE(dx+8+cx*2,(cy>>1)+18);
			/*B_PRINT(dotp[dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2]);*/
			a=dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2;
			if(cnfflag&0x8000){a=3-a;}
			B_PRINT(dotp[a]);
		}
		if( (sh&8)!=0 ){
			int a;
			dotpat[cx][cy]=0;
			B_LOCATE(dx+8+cx*2,(cy>>1)+18);
			/*B_PRINT(dotp[dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2]);*/
			a=dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2;
			if(cnfflag&0x8000){a=3-a;}
			B_PRINT(dotp[a]);
		}

		if( kc==' ' ){
			int a;
			dotpat[cx][cy]=1-dotpat[cx][cy];
			B_LOCATE(dx+8+cx*2,(cy>>1)+18);
			/*B_PRINT(dotp[dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2]);*/
			a=dotpat[cx][cy&14]+dotpat[cx][(cy&14)+1]*2;
			if(cnfflag&0x8000){a=3-a;}
			B_PRINT(dotp[a]);
		}

		if( kc=='G' || kc=='g' ){
			dattopat();dot_dis(dx);
		}

		if( ke==0x3f ){
			for(i=0;i<16;i++){for(j=0;j<16;j++){dotpat[i][j]=0;}}
			dot_dis(dx);
		}

		if( kc=='R' || kc=='r'){
			for(i=0;i<16;i++){for(j=0;j<16;j++){dotpat[i][j]=1-dotpat[i][j];}}
			dot_dis(dx);
		}


		if( sh&2 ){
			if( ke==0x3b ){
				for(j=0;j<16;j++){
					int a=dotpat[0][j];
					for(i=0;i<15;i++){dotpat[i][j]=dotpat[i+1][j];}
					dotpat[15][j]=a;
				}
				dot_dis(dx);ke=0;
			}
			if( ke==0x3d ){
				for(j=0;j<16;j++){
					int a=dotpat[15][j];
					for(i=15;i>0;i--){dotpat[i][j]=dotpat[i-1][j];}
					dotpat[0][j]=a;
				}
				dot_dis(dx);ke=0;
			}
			if( ke==0x3c ){
				for(j=0;j<16;j++){
					int a=dotpat[j][0];
					for(i=0;i<15;i++){dotpat[j][i]=dotpat[j][i+1];}
					dotpat[j][15]=a;
				}
				dot_dis(dx);ke=0;
			}
			if( ke==0x3e ){
				for(j=0;j<16;j++){
					int a=dotpat[j][15];
					for(i=15;i>0;i--){dotpat[j][i]=dotpat[j][i-1];}
					dotpat[j][0]=a;
				}
				dot_dis(dx);ke=0;
			}
		}


		if( ke>=0x63 && ke<=0x67 ){
			dot_ctrl(ke-0x63);
			dot_dis(dx);
		}

	}

	B_COLOR(3);
	txerase(dx*8+8,15*16,42*8,15*16);
	fill(gx+31-8,512+240,gx+360+64,995,0);
	dpl(1);tpl(1);
	fnc_dis(4);

if((cmdflag&0x200)==0){
	MS_LIMIT(0,0,767,511);MS_CUROF();
}
	return 66+8;
}

/***************************/
void	dot_ctrl(int m)
{
	int i,j;
	int dot2[16][16];

	if( m==0 ){
		for(j=0;j<16;j++){
			for(i=0;i<8;i++){
				int a;
				a=dotpat[j][i];
				dotpat[j][i]=dotpat[j][15-i];
				dotpat[j][15-i]=a;
			}
		}
	}
	if( m==1 ){
		for(j=0;j<16;j++){
			for(i=0;i<8;i++){
				int a;
				a=dotpat[i][j];
				dotpat[i][j]=dotpat[15-i][j];
				dotpat[15-i][j]=a;
			}
		}
	}
/*
	if(m==2){
		for(j=0;j<16;j++){
			for(i=0;i<16;i++){
				dot2[j][i]=dotpat[15-i][j];
			}
		}
		for(j=0;j<16;j++){
			for(i=0;i<16;i++){
				dotpat[j][i]=dot2[j][i];
			}
		}
	}*/
/*	if(m==3){*/
	if(m==2){
		for(j=0;j<16;j++){
			for(i=0;i<16;i++){
				dot2[j][i]=dotpat[i][15-j];
			}
		}
		for(j=0;j<16;j++){
			for(i=0;i<16;i++){
				dotpat[j][i]=dot2[j][i];
			}
		}
	}
}

/***************************/
void	dot_dis(int cx)
{
	int	i,j,a;

	for(i=0;i<16;i+=2){
		B_LOCATE(cx+8,(i>>1)+18);
		for(j=0;j<16;j++){
		/*	B_PRINT(dotp[dotpat[j][i]+dotpat[j][i+1]*2]);*/
			a=dotpat[j][i]+dotpat[j][i+1]*2;
			if(cnfflag&0x8000){a=3-a;}
			B_PRINT(dotp[a]);
		}
	}
}

/***************************/
void	dattopat(int me)
{
	int	ad=8,i,j,a;

	for(j=0;j<16;j++){
/*		a=dat[ad]&31;a=a*32+(dat[ad+16]&31);
		a=a*32+(dat[ad+32]&31);a=a*32+(dat[ad+48]&31);*/

		a=((((((dat[ad]*32)|dat[ad+16])*32)|dat[ad+32])*32)|dat[ad+48]);

		a=a>>4;
		ad++;

		for(i=0;i<16;i++){dotpat[15-i][j]=a&1;a=a>>1;}
	}
}

/***************************/
void	pattodat(int me)
{
	int	ad=8,i,j,a;

	for(j=0;j<16;j++){
		a=0;
		for(i=0;i<16;i++){a=a<<1;if(dotpat[i][j]==1){a|=1;}}
		a=a<<4;

		dat[ad   ]=(a>>15)&31;dat[ad+16]=(a>>10)&31;
		dat[ad+32]=(a>>5 )&31;dat[ad+48]=(a    )&31;
		ad++;
	}
	dat[72]=0x84;dat[73]=0xf7;
}

/***************************/

/***************************/
/*			0x10 0x01～5 0x00 / 0x10 0x01～5 0x40*/
void	exc_view(int ad)
{
	int	i,cx,gx,co,po,excmd=0;
	int	e=edit_scr;
	char	tmp0[128];
	unsigned char	*buf;buf=dat+(16*1024);

	if(edit_scr==0){cx=41;gx=328;}else{cx=1;gx=8;}
	cons_md(0);noed_cls();edit_scr=0;

	if(trk[track][ad]<0x90 || trk[track][ad]>0x98){goto ext;}

	if(trk[track][ad]==0x98){
		po=ad;co=0;
		while(trk[track][po+4]==0xf7){po+=4;
			dat[co]=trk[track][po+2];if(dat[co]==0xf7){break;}
			co++;
			dat[co]=trk[track][po+3];if(dat[co]==0xf7){break;}
			co++;
		}
	}else{
		excmd=trk[track][ad]-0x90+1;po=0;co=0;
		while(user_exc_data[excmd-1][po]!=0xf7 && po<24){
			dat[co++]=user_exc_data[excmd-1][po++];
		}
		dat[co++]=0xf7;
	}

	box(gx-1,624-48,gx+417+16,997,14,65535);
	fill(gx,639-32,gx+15,996,8);
	fill(gx,639-40,gx+416+16,639-16-8,8);

	if(excmd==0){strcpy(tmp0," Track");}else{strcpy(tmp0," User");}
	strcat(tmp0," Exclusive View");

	if(excmd){strcat(tmp0," [ ] ");tmp0[22]=excmd+'0'-1;
		strcat(tmp0,user_exc_memo[excmd-1]);}

	sdis(cx,624-48,tmp0,54,15,14);

/*
	strcpy(tmp0," Tr:");strcat(tmp0,fstr(track+1,2));
	strcat(tmp0," Meas:");strcat(tmp0,fstr(meas_no(ad),4));

	sdis(cx,624-48+16,tmp0,54,15,14);
*/

	for(i=0;i<22;i++){tdis(cx,7+i,hex_s(i,2),2,3);}
/*	for(i=0;i<22;i++){sdis(cx,512+(7+i)*16,hex_s(i,2),2,3,8);}*/
	for(i=0;i<16;i++){sdis(cx+i*3+7,512+6*16-8,hex_s(i,1),1,3,8);}

	exc_dis2(cx,co);

	if( dat[0]==0x41 && dat[1]==0x10 && dat[2]==0x45 && dat[3]==0x12 &&
	    dat[4]==0x83 && dat[5]==0x10 ){
		if(dat[7]==0x00){
			if(dat[6]>0){
				prn_gspanel(cx);scpanel_dsp(cx+13,18,&dat[8]);
			}else{
				prn_gspanel(cx);scinst_dsp(cx+13,16,&dat[8]);
			}
		}
	}
	tg_copy2(1-e);
	edit_scr=e;
/*	txerase(cx*8,4*16,53*8,26*16);*/
	noed_cls_t();
ext:
	edit_scr=e;
	cons_md(1);

}

inline void	exc_dis2(int cx,int co)
{
	int	i,a=0,no=0;

	B_LOCATE(cx+3,7);B_COLOR(9);B_PRINT("F0");B_COLOR(3);

	while(no<=16+6){
		int nn=no*16;

		B_LOCATE(cx+5,7+no);

		i=0;
		while(i<16 && nn<=co){
			B_PRINT(" ");
			a=dat[nn++];i++;

			if(a>=0x80 && a<=0x84){
				B_COLOR(1);B_PRINT(uxcc[a-0x80]);
			}else{
				if(a<0x80){B_COLOR(3);}else{B_COLOR(9);}
				B_PRINT(hex_s(a,2));if(a>=0x80){B_COLOR(3);}
			}

			if(a==0xf7){break;}
		}
		B_COLOR(1);

		no++;
	}
	B_COLOR(3);snsclr();
}

