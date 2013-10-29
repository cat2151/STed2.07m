/*
 * STed: trkset.c (track set subroutine)
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
#include	"sted.h"


void	fsel();
int	vinput();
void	sinput();
int	inkey2();
int	inkey3();
void	snsclr();
void	spcut();
void	str_change();
int	str_search();
int	str_last();
int	str_val();

void	trk_ed();
int	real_record();
int	trk_load();
int	trk_save();
void	option();
void	option2();
void	form3();
void	logo();
void	dplay();
void	all_note_off();
void	poplay();
void	fnc_dis();
char	*fstr();
char	*chstr();
char	strch();
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
void	g_print();
void	txerase();
char	*jstrupr();

int	trk_mix_sub();
int	trk_ext_sub();
void	repeat_comp();
int	repeat_ext();
int	same_meas_conv();
int	same_ext_sub();
int	trk_opt_sub();

int	size_max();
int	size_change();
int	size_add();
void	size_ref();

int	yn_sel();
void	rcp_dat();
void	rcp_dat2();
void	trk_dat();
void	beat_set();

int	trk_sel();
void	trk_no_cur();
void	trk_no_dis();
void	trk_set();
void	trmod_set();
void	trk_lin();
void	ts_swap();
void	ts_ins();
void	ts_sort();
void	ts_copy();
void	ts_clear();
void	ts_mix();
void	ts_measend();
void	ts_ext();
void	trksize();
void	trksize_c();

/***************************/
int	trk_sel(int cx)
{
	int	ke,sh;

	while( 1){
		trk_no_cur(track,2);if(cx<0){trk_no_cur(track,1);}else{trk_lin(track,cx,1);}
		ke=inkey2();sh=B_SFTSNS();
		trk_no_cur(track,0);if(cx>=0){trk_lin(track,cx,0);}

		if(sh&4){
			int	ex;
			char	t[32];
			t[0]=ke;t[1]=0;
			ex=str_search("1234567890,./*-+=\x0d",t);
			if( ex ){
				ex--;track=ex;
				if(ex<btrack || ex>btrack+17){
					btrack=ex/9*9;if(btrack>18){btrack=18;}
					form3();
				}
				track=ex;
			}
		}

		if( ke==9 ){
			int	j;
			msg("TRACK No.=");j=vinput(track+1,0x802);msg_clr();
			if(es==0 && j>0 && j<=36){
				j--;track=j;
				if(j<btrack || j>btrack+17){
					btrack=j/9*9;if(btrack>18){btrack=18;}
					form3();
				}
			}
		}
		if( ke==13 ){break;}
		if( ke==27 ){return(-1);}

		if( ke==0x0b ){track=btrack;}

		if( ke==0x05 ){track--;if(track<btrack){track=btrack+17;}}
		if( ke==0x18 ){track++;if(track>btrack+17){track=btrack;}}

		if( ke==0x17 && btrack>0){
			if(sh&1){btrack-=1;track-=1;}else{btrack-=9;track-=9;}
			if(btrack<0){btrack=0;track=0;}
			form3();snsclr();
		}
		if( ke==0x12 && btrack<18){
			if(sh&1){btrack+=1;track+=1;}else{btrack+=9;track+=9;}
			if(btrack>18){btrack=18;track=18;}
			form3();snsclr();
		}

	}
	trk_no_dis();return(0);
}

/***************************/
void	trk_no_cur(int i,int m)
{
	int a,b;

	if(m==0){a=15;b=8;}else{a=0;b=15;if(m==2){b=11;}}

	if(i>=btrack && i<btrack+18){
		vdis(13,192+(i-btrack)*16,i+1,2,a,b);
	}
}

/***************************/
void	trk_no_dis()
{
	vdis2(21+14,0,track+1,2,15,8);vdis2(21+14,512,track+1,2,15,8);
}

/***************************/
void	trk_set(int md)
{
	int	ke,sh,cx,i,y;

	edfield(-1);edit_scr=0;logo(-1);i=track;

	cx=-1;
	while( 1){
top:
		trk_no_cur(i,2);if(cx<0){trk_no_cur(i,1);}else{trk_lin(i,cx,1);}
		ke=inkey3();sh=ke>>8;ke=ke&0xff;
		if(ke>='a' && ke<='z'){ke-=32;}
		trk_no_cur(i,0);if(cx>=0){trk_lin(i,cx,0);}
		track=i;

		if( ke>='A' && ke<='Z' && (sh&4)!=0 ){
			if(cnfflag&0x200){sh=1-(sh&1);}
			if(sh&1){option(ke-'A',1);}else{option2(ke-'A',1,0,0);}
			ke=0;
		}

		y=12+(i-btrack);

		if( ke==0xe0 ){if((sh&1)==0){poplay(0,4);}else{poplay(0,5);}}
		if( ke==0xe1 ){if((sh&1)==0){poplay(0,1);}else{poplay(0,2);}}


		if(sh&4){
			int	ex;
			char t[32];
			t[0]=ke;t[1]=0;
			ex=str_search("1234567890,./*-+=\x0d",t);
			if( ex ){
				i=ex-1;
				if(i<btrack || i>=btrack+18){
					btrack=i/9*9;if(btrack>18){btrack=18;}
					form3();
				}
				track=i;ke=0;
			}
		}

/*		if(sh&8){
			int	ex;
			char t[32];
			t[0]=ke;t[1]=0;
			ex=str_search("1234567890,./ *-+=\x0d",t);
			if( ex ){
				if(cx==1){mch[i]=ex;ke=0;}
			}
		}
*/

		if( ke==27 ){break;}
		if( ke==9 ){
			int	ex;
			msg("TRACK No.=");ex=vinput(i+1,0x802);msg_clr();
			if(es==0 && ex>0 && ex<=36){
				i=ex-1;
				if(i<btrack || i>=btrack+18){
					btrack=i/9*9;if(btrack>18){btrack=18;}
					form3();
				}
			}
		}

		if( ke==13 ){
			trk_no_dis();
			if( cx<0 || (sh&1)!=0 ){
				if(md==0){
					trk_ed();
					i=track;
					if(i<btrack || i>=btrack+18){
						btrack=i/9*9;if(btrack>18){btrack=18;}
						form3();
					}
					if(B_SFTSNS()&1){break;}
				}else{
					if(real_record()==0){form3();}
					break;
				}
				goto top;
			}

			if( cx==0 ){
				int	ex=trmod[i];trmod[i]=(ex&0xfc)+((ex+1)&3);
			}
			if( cx==1 ){
				int	ex;
				char	tmp0[64];

				strcpy(tmp0,chstr(mch[i]));
				B_LOCATE(21,y);sinput(tmp0,0x803);ex=strch(mch[i],tmp0);
				if(es==0 && ex<33 && ex>=0 ){mch[i]=ex;}
			}
			if( cx==2 ){
				int	ex=trst[i];if(ex>127){ex=ex-256;}
				B_LOCATE(25,y);ex=vinput(ex,0x803);
				if(es==0 && (ex>=-99 && ex<=99)){
					if( ex<0 ){ex=ex+256;}
					trst[i]=ex;}
			}
			if( cx==3 ){
				int	ex=trkey[i];if( ex>63 && ex<128){ex=ex-128;}
				B_LOCATE(29,y);ex=vinput(ex,0x803);
				if(es==0 && ((ex>=-64 && ex<64) || ex==128)){
					if( ex<0 ){ex=ex+128;}
					trkey[i]=ex;}
			}
			if( cx==4 ){
				int	ex=trrhy[i];
				if(ex<0x80){ex=0x80;}else{if(ex==0x80){ex=0x90;}else{ex=0;}}
				trrhy[i]=ex;
			}

			if( cx==5 ){B_LOCATE(37,y);sinput(trkmemo[i],36);}
			B_CLR_AL();
		}

		if( ke>='0' && ke<='9' ){
			trk_no_dis();

			if( cx==1 ){
				int	ex;
				char	tmp0[64];

				tmp0[0]=ke;tmp0[1]=0;
				B_LOCATE(21,y);sinput(tmp0,0x403);ex=strch(mch[i],tmp0);
				if(es==0 && ex<33 && ex>=0 ){mch[i]=ex;}
			}
			if( cx==2 ){
				int	ex=trst[i];if(ex>127){ex=ex-256;}
				B_LOCATE(25,y);ex=vinput(ke-'0',0x403);

				if(es==0 && (ex>=-99 && ex<=99)){
					if( ex<0 ){ex=ex+256;}
					trst[i]=ex;}
			}
			if( cx==3 ){
				int	ex=trkey[i];if( ex>63 && ex<128){ex=ex-128;}
				B_LOCATE(29,y);ex=vinput(ke-'0',0x403);
				if(es==0 && ((ex>=-64 && ex<64) || ex==128)){
					if( ex<0 ){ex=ex+128;}
					trkey[i]=ex;}
			}
			B_CLR_AL();
		}

		if( ke=='+' || ke=='>' || ke=='.' ){
			if( cx==0 ){
				int	ex=trmod[i];trmod[i]=(ex&0xfc)+((ex+1)&3);
			}
			if( cx==1 ){
				int	ex=mch[i];ex++;if(ex>32){ex=0;}
				mch[i]=ex;
			}
			if( cx==2 ){
				int	ex=trst[i];if(ex>127){ex=ex-256;}
				if(sh&1){ex+=12;}else{ex++;}
				if(ex<=99){
					if( ex<0 ){ex=ex+256;}
					trst[i]=ex;}
			}
			if( cx==3 ){
				int	ex=trkey[i];if( ex>63 && ex<128){ex=ex-128;}
				if(sh&1){ex+=12;}else{ex++;}
				if(ex<64){
					if( ex<0 ){ex=ex+128;}
					trkey[i]=ex;}
			}
			if( cx==4 ){
				int	ex=trrhy[i];
				if(ex<0x80){ex=0x80;}else{if(ex==0x80){ex=0x90;}else{ex=0;}}
				trrhy[i]=ex;
			}
		}
		if( ke=='-' || ke=='<' || ke==',' ){
			if( cx==0 ){
				int	ex=trmod[i];trmod[i]=(ex&0xfc)+((ex+3)&3);
			}
			if( cx==1 ){
				int	ex=mch[i];ex--;if(ex<0){ex=32;}
				mch[i]=ex;
			}
			if( cx==2 ){
				int	ex=trst[i];if(ex>127){ex=ex-256;}
				if(sh&1){ex-=12;}else{ex--;}
				if(ex>=-99){
					if( ex<0 ){ex=ex+256;}
					trst[i]=ex;}
			}
			if( cx==3 ){
				int	ex=trkey[i];if( ex>63 && ex<128){ex=ex-128;}
				if(sh&1){ex-=12;}else{ex--;}
				if(ex>=-64){
					if( ex<0 ){ex=ex+128;}
					trkey[i]=ex;}
			}
			if( cx==4 ){
				int	ex=trrhy[i];
				if(ex<0x80){ex=0x90;}else{if(ex==0x90){ex=0x80;}else{ex=0;}}
				trrhy[i]=ex;
			}

		}
		if( ke==32 ){
			if( (sh&1)==0 ){
				if( cx==0 ){trmod[i]=0;}
				if( cx==1 ){mch[i]=0;}
				if( cx==2 ){trst[i]=0;}
				if( cx==3 ){trkey[i]=/*0x80*/0;}
				if( cx==4 ){trrhy[i]=0;}
			}else{
				int j;
				for(j=0;j<36;j++){
					if(i!=j){
						if( cx==0 ){trmod[j]=0;}
						if( cx==1 ){mch[j]=0;}
						if( cx==2 ){trst[j]=0;}
						if( cx==3 ){trkey[j]=/*0x80*/0;}
						if( cx==4 ){trrhy[j]=0;}
						trk_lin(j,cx,0);
					}
				}
			}
		}
		if( ke=='=' ){
			if( cx==0 ){
				int	ex=trmod[i]^1;trmod[i]=ex;
			}
			if( cx==1 ){
				int	ex=mch[i];
				if( ex>0 ){
					if( ex>16 ){ex-=16;}else{ex+=16;}
					mch[i]=ex;
				}
			}
			if( cx==2 ){
				int	ex=trst[i];if(ex>127){ex=ex-256;}
				ex=-ex;
				if(ex>=-99 && ex<=99){
					if( ex<0 ){ex=ex+256;}
					trst[i]=ex;}
			}
			if( cx==3 ){
				int	ex=trkey[i];
				if( ex>0 && ex<128){
					if( ex>63 ){ex=ex-128;}
					ex=-ex;if( ex<0 ){ex=ex+128;}
				}else{ex=128-ex;}
				trkey[i]=ex;
			}
			if( cx==4 ){trrhy[i]=(trrhy[i]^0x80);}
		}


		if( ke==0x0b ){i=btrack;}

		if( ke==0x05 ){i--;if(i<btrack){i=btrack+17;}}
		if( ke==0x18 ){i++;if(i>btrack+17){i=btrack;}}
		if( ke==0x13 ){cx--;if(cx<-1){cx=5;}}
		if( ke==0x04 ){cx++;if(cx>5){cx=-1;}}

		if( ke==0x17 && btrack>0){
			if(sh&1){btrack-=1;i-=1;}else{btrack-=9;i-=9;}
			if(i<0){btrack=0;i=0;}
			form3();snsclr();
		}
		if( ke==0x12 && btrack<18){
			if(sh&1){btrack+=1;i+=1;}else{btrack+=9;i+=9;}
			if(btrack>18){btrack=18;i=18;}
			form3();snsclr();
		}

		if( cx==0 ){
			if( ke=='P' ){trmod_set(i,0,sh&1);ke=0;}
			if( ke=='M' ){trmod_set(i,1,sh&1);ke=0;}
			if( ke=='X' ){trmod_set(i,2,sh&1);ke=0;}
			if( ke=='R' ){trmod_set(i,3,sh&1);ke=0;}
		}

		if( ke=='X' ){
			int	ex;
			B_LOCATE(55,6);B_PRINT("ExChange Track Data");
			B_LOCATE(72,7);B_PRINT("Source Track=");B_PRINT(fstr(i+1,2));
			B_LOCATE(68,8);B_PRINT("Distnation Track=");
exc1:			ex=trk_sel(cx);if(ex==0 && i==track){goto exc1;}
			if(ex==0){
				B_LOCATE(68+17,8);B_PRINT(fstr(track+1,2));
				ts_swap(i,track);trk_lin(i,-1,0);trk_lin(track,-1,0);}
			i=track;
			B_CLR_AL();
		}
		if( ke=='C' ){
			int	ex;
			B_LOCATE(55,6);B_PRINT("Copy Track Data");
			B_LOCATE(72,7);B_PRINT("Source Track=");B_PRINT(fstr(i+1,2));
			B_LOCATE(68,8);B_PRINT("Distnation Track=");
cpy1:			ex=trk_sel(cx);if(ex==0 && i==track){goto cpy1;}
			if(ex==0){
				B_LOCATE(68+17,8);B_PRINT(fstr(track+1,2));
				ts_copy(i,track);trk_lin(track,-1,0);}
			B_CLR_AL();
			i=track;
		}

		if( ke=='I' ){
			int	ex,j;
			B_LOCATE(55,6);B_PRINT("Insert Track Data");
			B_LOCATE(72,7);B_PRINT("Source Track=");B_PRINT(fstr(i+1,2));
			B_LOCATE(68,8);B_PRINT("Distnation Track=");
ins1:			ex=trk_sel(cx);if(ex==0 && i==track){goto ins1;}
			if(ex==0){
				B_LOCATE(68+17,8);B_PRINT(fstr(track+1,2));
				ts_ins(i,track);for(j=0;j<36;j++){trk_lin(j,-1,0);}
			}
			i=track;
			B_CLR_AL();
		}

		if( ke=='T' ){
			int	ex,j;
			B_LOCATE(55,6);B_PRINT("Sort Track Data");
			B_LOCATE(72,7);B_PRINT("Start  Track=");B_PRINT(fstr(i+1,2));
			B_LOCATE(72,8);B_PRINT("End    Track=");
srt1:			ex=trk_sel(cx);if(ex==0 && i>=track){goto srt1;}
			if(ex==0){
				B_LOCATE(68+17,8);B_PRINT(fstr(track+1,2));
				ts_sort(i,track);for(j=0;j<36;j++){trk_lin(j,-1,0);}
			}
			i=track;
			B_CLR_AL();
		}

		if( ke=='M' ){
			int	ex;
			B_LOCATE(55,6);B_PRINT("Mix Track Data");
			B_LOCATE(72,7);B_PRINT("Source Track 1=");B_PRINT(fstr(track+1,2));
			B_LOCATE(72,8);B_PRINT("Source Track 2=");
mix1:			ex=trk_sel(cx);if(ex==0 && i==track){goto mix1;}
			if(ex==0){
				int	i2;
				B_LOCATE(72+15,8);B_PRINT(fstr(track+1,2));
				B_LOCATE(71,9);B_PRINT("Distnation Track=");
				i2=track;
				ex=trk_sel(cx);
				if(ex==0){
					B_LOCATE(71+17,9);B_PRINT(fstr(track+1,2));
					ts_mix(i,i2,track);
					trk_lin(i,-1,0);trk_lin(i2,-1,0);trk_lin(track,-1,0);
				}
			}
			i=track;
			txerase(432,107-16,744,164+16);
		}
		if( ke==12 || ke=='D' ){
			B_LOCATE(55,6);B_PRINT("Erase Track Data");
			B_LOCATE(79,7);B_PRINT("Track=");B_PRINT(fstr(i+1,2));
			msg("トラックデータを消去しますか");
			if(yn_sel()>0){ts_clear(i);trk_lin(i,-1,0);}
			B_CLR_AL();
		}
		if( ke=='O' ){
			B_LOCATE(55,6);B_PRINT("Optimize Track Data");
			B_LOCATE(79,7);B_PRINT("Track=");
			if(sh&1){B_PRINT("All");}else{B_PRINT(fstr(i+1,2));}
			msg("トラックデータを最適化しますか");
			if(yn_sel()>0){
				if(sh&1){
					int j;
					for(j=0;j<36;j++){
						exetrk_dsp(j);
						if(tr_len[j]>4){
							trk_opt_sub(j,0,tr_len[j]);trk_lin(j,-1,0);
						}
					}
				}else{
					trk_opt_sub(i,0,tr_len[i]);trk_lin(i,-1,0);
				}
			}
			B_CLR_AL();
		}
		if( ke=='A' || ke=='R' ){
			if( ke=='A' ){
				B_LOCATE(55,6);B_PRINT("SameMeas Compress Track Data");
			}else{
				B_LOCATE(55,6);B_PRINT("Repeat Compress Track Data");
			}
			B_LOCATE(79,7);B_PRINT("Track=");
			if(sh&1){B_PRINT("All");}else{B_PRINT(fstr(i+1,2));}
			msg("トラックデータを圧縮しますか");
			if(yn_sel()>0){
				int t=track;
				size_ref();
				if(sh&1){
					int j;
					for(j=0;j<36;j++){
						exetrk_dsp(j);
						if(tr_len[j]>4){
							track=j;
							if( ke=='A' ){
								same_meas_conv(j);
							}else{
								repeat_comp(j);
							}
							trk_lin(j,-1,0);
						}
					}
				}else{
					if( ke=='A' ){
						same_meas_conv(i);
					}else{
						repeat_comp(i);
					}
					trk_lin(i,-1,0);
				}
				track=t;
			}
			B_CLR_AL();
		}

		if( ke=='E' ){
			B_LOCATE(55,6);B_PRINT("SameMeas/Repeat Extract Track Data");
			B_LOCATE(79,7);B_PRINT("Track=");
			if(sh&1){B_PRINT("All");}else{B_PRINT(fstr(i+1,2));}
			msg("トラックデータを展開しますか");
			if(yn_sel()>0){
				if(sh&1){
					int j;
					for(j=0;j<36;j++){
						exetrk_dsp(j);
						if(tr_len[j]>4){ts_ext(j);trk_lin(j,-1,0);}
					}
				}else{
					ts_ext(i);trk_lin(i,-1,0);
				}
			}
			B_CLR_AL();
		}

		if( ke=='B' ){
			B_LOCATE(55,6);B_PRINT("Meas End Rewrite");
			B_LOCATE(79,7);B_PRINT("Track=");
			if(sh&1){B_PRINT("All");}else{B_PRINT(fstr(i+1,2));}
			ts_measend(i,sh);/*trk_lin(i,-1,0);*/
			txerase(432,107-16,744,164+16);
			B_CLR_AL();
		}

		if( ke=='L' ){
			if((sh&1)==0){
				char	tmp0[128],path2[128];

				track=i;
				trk_no_dis();
				strcpy(path2,trk_path);fsel(trk_file,path2,0x005);
				if( es==0 && trk_file[0]!=0 ){
					strcpy(tmp0,path2);strcat(tmp0,trk_file);
					msg("トラックデータのロード中です。");
					if(trk_load(tmp0)==0){
						if(!(cnfflag&0x0008)){strcpy(trk_path,path2);}
						trk_lin(track,-1,0);
						msg_clr();
					}
				}
			}else{load_sub(0);}
		}
		if( ke=='S' ){
			if((sh&1)==0){
				char	tmp0[128],path2[128];

				track=i;
				trk_no_dis();
				strcpy(path2,trk_path);fsel(trk_file,path2,0x105);
				if( es==0 && trk_file[0]!=0 ){
					strcpy(tmp0,path2);strcat(tmp0,trk_file);
					msg("トラックデータのセーブ中です。");
					if(trk_save(tmp0)==0){
						if(!(cnfflag&0x0008)){strcpy(trk_path,path2);}
						msg_clr();
					}
				}
			}else{save_sub(0);}
		}

	}
	logo(512);edit_scr=0;
}

/***************************/
/*「現在 Track 1 を処理しています」*/
void	exetrk_dsp(int i)
{

	B_LOCATE(55,9);B_PRINT("TRACK=");B_PRINT(fstr(i+1,2));
}

/***************************/
void	trmod_set(int tr,int md,int sh)
{
	int j,i=tr;

	if(sh&1){
		for(j=0;j<36;j++){
			if(i!=j){
				trmod[j]=md;
				trk_lin(j,0,0);
			}
		}
	}else{
		trmod[i]=md;
		trk_lin(i,0,0);
	}
}

/***************************/
void	trk_lin(int i,int cx,int m)
{
	int	c1,c2,ex,y;
	char	tmp0[16];
static char	play_mode[5][5]={"Play","Mute"," Mix"," Rec"," Rec"};
static char	play_modec[5]={3,2,1,6,6};

	if(i<btrack || i>btrack+17){return;}
	y=192+(i-btrack)*16;

	if(m==1){c1=0;c2=15;}else{c1=15;c2=0;}
	if(m==2){c2=-1;}

	if(cx<0){vdis2(13,y,i+1,2,15,-1);}

	if(cx==0 || cx<0){
		int cl0=play_modec[trmod[i]&3],cl1=c1,cl2=c2;
		if(c1>0){cl1=cl0;}
		if(c2>0){cl2=cl0;}
		sdis2(16,y,play_mode[trmod[i]&3],4,cl1,cl2);}

	if(cx==1 || cx<0){
		ex=mch[i];
		strcpy(tmp0,chstr(ex));
		sdis2(21,y,tmp0,3,c1,c2);
	}
	if(cx==2 || cx<0){
		ex=trst[i];
		if( ex>127 ){ex=ex-256;}
		vdis2(25,y,ex,3,c1,c2);
	}
	if(cx==3 || cx<0){
		ex=trkey[i];
		if(ex>=0x80){
			sdis2(29,y,"OFF",3,c1,c2);
		}else{
			if( ex>63 ){ex=ex-128;}
			vdis2(29,y,ex,3,c1,c2);
		}
	}
	if(cx==4 || cx<0){
		if(trrhy[i]&0x80){
			strcpy(tmp0,"ON ");if(trrhy[i]>0x80){tmp0[2]='+';}
			sdis2(33,y,tmp0,3,c1,c2);
		}else{
			sdis2(33,y,"OFF",3,c1,c2);}
	}
	if(cx==5 || cx<0){sdis2(37,y,trkmemo[i],36,c1,c2);}
	if(cx<0 && m<2){trksize(i);}
}

/***************************/
void	ts_swap(int a,int b)
{
	char	tmp0[128];
	int	i,j,d;

	size_ref();
	if(size_change(a,tr_len[b])){return;}
	if(size_change(b,tr_len[a])){return;}

	memcpy_l(dat,trk[a],tr_len[a]);
	memcpy_l(trk[a],trk[b],tr_len[b]);
	memcpy_l(trk[b],dat,tr_len[a]);

	for(i=0;i<2;i++){for(j=0;j<4;j++){
		d=tr_pos[a][i][j];tr_pos[a][i][j]=tr_pos[b][i][j];tr_pos[b][i][j]=d;}}

	d=tr_len[a];tr_len[a]=tr_len[b];tr_len[b]=d;

	d=trmod[a];trmod[a]=trmod[b];trmod[b]=d;
	d=trrhy[a];trrhy[a]=trrhy[b];trrhy[b]=d;
	d=mch[a];mch[a]=mch[b];mch[b]=d;
	d=trkey[a];trkey[a]=trkey[b];trkey[b]=d;
	d=trst[a];trst[a]=trst[b];trst[b]=d;
	strcpy(tmp0,trkmemo[a]);strcpy(trkmemo[a],trkmemo[b]);strcpy(trkmemo[b],tmp0);
}

/***************************/
void	ts_ins(int a,int b)
{
	int	i;

	if(a<b){
		for(i=a;i<b;i++){ts_swap(i,i+1);}
	}else{
		for(i=a-1;i>b-1;i--){ts_swap(i,i+1);}
	}
}

/***************************/
void	ts_sort(int a,int b)
{
	int	i,j;

	for(i=a;i<=b-1;i++){
			B_LOCATE(55,9);B_PRINT("TRACK=");B_PRINT(fstr(i+1,2));
		for(j=i+1;j<=b;j++){
			if( (mch[i]>mch[j] || ( mch[j]==10||mch[j]==26) ||mch[i]==0) && mch[j]!=0 ){
				ts_swap(i,j);
			}
		}
	}
}

/***************************/
void	ts_copy(int a,int b)
{
	int	i,j;

	size_ref();
	if(size_change(b,tr_len[a])){return;}

	memcpy_l(trk[b],trk[a],tr_len[a]);

	for(i=0;i<2;i++){for(j=0;j<4;j++){tr_pos[b][i][j]=tr_pos[a][i][j];}}
	tr_len[b]=tr_len[a];trmod[b]=trmod[a];
	trrhy[b]=trrhy[a];
	mch[b]=mch[a];trkey[b]=trkey[a];trst[b]=trst[a];
	strcpy(trkmemo[b],trkmemo[a]);
}

/***************************/
void	ts_clear(int a)
{
	int	j;

	trk[a][0]=0xfe;tr_len[a]=4;
	for(j=0;j<4;j++){tr_pos[a][0][j]=0;tr_pos[a][1][j]=0;}
	trmod[a]=0;trno[a]=a+1;trrhy[a]=0;mch[a]=a+1;trkey[a]=0;trst[a]=0;
	if(a==9){trrhy[9]=128;trkey[9]=128;}
	if(a>15){mch[a]=0;}
	trkmemo[a][0]=0;

	size_ref();
}

/***************************/
void	ts_mix(int tr1,int tr2,int tr3)
{
	int	po;

	rcpf=0;cpleng=0;

	msg("TRACK MIX...");

	po=trk_ext_sub(tr1,cpbuf);if(po==0){return;}
	po=trk_ext_sub(tr2,rcpbuf);if(po==0){return;}

	po=trk_mix_sub(cpbuf,rcpbuf);
	if(size_change(tr3,po+4)){return;}

	memcpy_l(trk[tr3],dat,po);trk[tr3][po]=0xfe;tr_len[tr3]=po+4;

	msg_clr();
}

/***************************/
void	ts_measend(int tr1,int sh)
{
	static	unsigned char	bars[4*7]={
		0xf9,0,0,0, 0xf9,0,0,0, 0,48,0,0, 0xf8,4,0,0, 0,0,0,0,
		0xfd,0,0,0, 0xf8,3,0,0};

	int	po,mc,barlen,ofs,maxm;

	rcpf=0;cpleng=0;

	msg("MEAS END CHANGE");
loop:
	barlen = tbase * 4 * beat1 / beat2;
	B_LOCATE(17,1);B_PRINT("MEAS ST=");
	barlen=vinput(barlen,0x804);if(es!=0){msg_clr();return;}
	if(barlen<24 || barlen>2000){goto loop;}
loop2:
	B_LOCATE(30,1);B_PRINT("OFFSET=");
	ofs=vinput(0,0x804);if(es!=0){msg_clr();return;}
	if(ofs<0 || ofs>2000){goto loop2;}
loop3:
	B_LOCATE(42,1);B_PRINT("LAST MEAS=");
	maxm=vinput(0,0x804);if(es!=0){msg_clr();return;}
	if(maxm<0 || maxm>2000){goto loop3;}

	if(sh&1){tr1=0;}
loopx:
	exetrk_dsp(tr1);

	if(tr_len[tr1]<=4){goto skip;}

	po=trk_ext_sub(tr1,cpbuf);if(po==0){return;}

	po=0;mc=((tr_step[tr1]-ofs)/barlen);

	if(maxm>0 && mc<maxm){mc=maxm;if(ofs){mc--;}}

	if(ofs){
		int i=ofs;
		while(i>0){
			dat[po]=0;dat[po+1]=0;dat[po+2]=0;dat[po+3]=0;
			if(i<240){dat[po+1]=i;i=0;}else{dat[po+1]=240;i-=240;}
			po+=4;
		}
		dat[po]=0xfd;po+=4;
	}

	while(mc>0){
		memcpy(&dat[po],bars,4*7);
		if(barlen<=240+15 ){
			dat[po+9]=barlen;dat[po+13]=1;
		}else{
			int bl;
			dat[po+9]=240;dat[po+13]=barlen/240;
			bl=barlen-240*dat[po+13];
			dat[po+17]=bl;
		}

		if(mc<240){dat[po+25]=mc;mc=0;}else{dat[po+25]=240;mc-=240;}
		po+=4*7;
	}
	dat[po]=0xfe;po+=4;
	po=repeat_ext(rcpbuf,po);

	po=trk_mix_sub(rcpbuf,cpbuf);
	if(size_change(tr1,po+4)){return;}

	if(po>4 && dat[po-4]==0xfd && dat[po-8]==0xfd){po-=4;}

	memcpy_l(trk[tr1],dat,po);
	trk[tr1][po]=0xfe;tr_len[tr1]=po+4;

	trk_lin(tr1,-1,0);
skip:
	if(sh&1){
		tr1++;if(tr1<36){goto loopx;}
	}
	msg_clr();
}

/***************************/
void	ts_ext(int i)
{
	int	po;

	rcpf=0;cpleng=0;

	po=trk_ext_sub(i,cpbuf);if(po==0){return;}

	size_ref();
	if(size_change(i,po)){return;}

	memcpy_l(trk[i],cpbuf,po);
	tr_len[i]=po;
/*	trk_lin(i,-1,0);*/
}

/***************************/
void	trksize(int tr)
{
	int	i,us=-(4*36),s,c=0,fr=buff_free,yy;

	if(tr<0){c=-1;}else{tr_step[tr]=step_cluc2(tr,0,tr_len[tr]);}
	for(i=0 ;i<36;i++){
		us+=tr_len[i];
		fr+=(tr_alc[i]-tr_len[i]);
		if((tr==-1 || tr==i)&& i>=btrack && i<btrack+18){
			yy=192+(i-btrack)*16;

			vdis2(74,yy,(tr_len[i]-4)>>2,5,15,c);
			s=tr_step[i];vdis2(80,yy,s,12,15,c);
		}
	}
	vdis2(6,0,fr>>2,7,15,8);vdis2(6,512,fr>>2,7,15,8);
	vdis2(21,0,us>>2,7,15,8);vdis2(21,512,us>>2,7,15,8);
}

/***************************/
void	trksize_c()
{
	int	i;
	for(i=0 ;i<36;i++){tr_step[i]=step_cluc2(i,0,tr_len[i]);}
}

/***************************/
