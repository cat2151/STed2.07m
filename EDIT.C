/*
 * STed: edit.c (edit mainroutine)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

static char	*inmd[6]={
	"EDIT","EDIT SOUND","EDIT MIDI IN(Mono)","EDIT MIDI IN(Poly)",
	"EDIT MIDI IN(Step)","EDIT MIDI IN(P/St)"};

int	vinput();
void	sinput();
void	keyinp();
void	snsclr();
void	spcut();
int	klen();
int	knck();
void	fnc_dis();
void	tcur();
void	msg();
void	msg_clr();
void	timtra();
int	tagj();
int	timsel();
void	help();
int	vis_edit();
void	gra();
void	dat_info();
void	trksize();
void	edfield();
void	home2();
void	vdis();
void	vdis2();
void	sdis();
void	sdis2();
void	fsel();
char	*jstrupr();
char	*fstr();
char	*hex_s();
char	*spadd();
int	str_val();
void	midi_clr();
int	onekey();
int	polykey();
int	chord_ten();
void	all_note_off();
char	*keystr();
char	*keystr1();
char	*keystr2();
char	*prog_name();
int	ctc();

int	strch();
char	*chstr();

void	trk_ed();
void	trk_ed_end();
void	trk_pos_set();
void	rtrk_ed();
void	stgt_as();
void	part_as();
void	user_exc_as();
void	exc_edit();
void 	sttrc();
void	replace();
int	find();
int	find_rep();
void	poplay();
void	partf();
void	trk_cluc();
int	step_cluc();
int	meas_add();
int	meas_no();
int	meas_len();

int	sttm_no();
int	sttm_ad();
int	step_no();
int	step_ad();

void	trk_no();
void	undobuf();
void	trk_free();

int	trk_ext_sub();
int	trk_mix_sub();
int	same_meas_conv();
int	same_meas_extr();
void	trk_opt_sub();

int	cpybuf_set();
int	replicate();
int	ret_midi();
int	ret_chord();
int	scale_no();
int	scale_sel();
int	prog_no();
char	*scale_str();
int	spc_select();
void	rev_area();
void	same_shift();
int	buf_meas();
void	noed_cls();
void	tg_copy();
void	H_PRINT();

int	size_max();
int	size_change();
int	size_add();
void	size_ref();

int	real_po();
int	real_ad();
int	real_cy();
int	trk_check();
int	trk_shift();
void	trk_delete();

char	*trk_dis();
char	*ctrl_type();
void	cons_md();
void	val_add();
void	val_sub();
void	val_rev();
void	cur_up();
void	cur_down();
void	roll_up();
void	roll_down();
void	line_del();
void	line_ins();
void	line_ins_aft();
void	dat_rev();
void	ret();
void	spcon();
int	bank_no();
int	channele_no();
void	inmd_disp();
int	padjust();
int	spc_code();
int	bendc2();
void	bendc();
void	note_ctrl();
int	comment_inp();
void	trk_next();

/***************************/
void	trk_ed()
{
	int	po,ad,cx,cy,i,j,ke,kc,sh,rc,cs,xx,mdc,mdiv=0,tcp=0,tcs=0;
	int	r_ad,r_ln;
	char	tmp0[256];

	track1=track;track2=track;
	midi_clr();LEDMOD(0,0);LEDMOD(2,0);inmd_disp();home2(512);

track_edit_top:
	size_max(track);

	trk_cluc();cplen=0;cons_md(1);B_CLR_AL();edfield(1);fnc_dis(0);rc=tm_lag;
	po=tr_pos[track][edit_scr][0];cx=tr_pos[track][edit_scr][1];cy=tr_pos[track][edit_scr][2];
	if(real_ad(po,cy)>=tr_len[track]){po=tr_len[track]-4;cy=padjust(&po,18);}else{trk_dis(6,po,24);}
	gra(po,0);

	while( 1){
		if((B_SFTSNS()&16)!=0){sttrc(&po,&cy,0);}
		ad=real_ad(po,cy);

		r_ad=0;r_ln=-1;
		if(cplen<0){
			int	p_ad,p_ln;
			p_ad=cpadd;p_ln=abs(p_ad-ad);if(p_ad>ad){p_ad=ad;}

			if(p_ad<=po){r_ad=0;}else{r_ad=real_cy(po,p_ad);}
			if(r_ad<24){
				if(p_ad+p_ln>real_ad(po,23)){
					r_ln=24-r_ad;
				}else{
					r_ln=real_cy(po,p_ad+p_ln)-r_ad;
				}
			}
			if(r_ln>0){rev_area(r_ad,r_ln,edit_scr);}
		}
		cs=0;i=trk[track][ad];
		if(i<253 && i!=0xf6){
			if(trk[track][ad+1]==0){cs=2;}
			if(i>127){cs=1;}
		}
		if( cx==0 ){xx=0;}else{xx=2;}
		tcur(cx*6+12+xx,cy+6,8-xx,cs);
		mdc=0;
		do{
			rc++;
			if( rc==tm_lag ){
				sh=B_SFTSNS();
				if((sh&1)!=0){fnc_dis(1);}else{fnc_dis(0);}
				trk_free(ad);
				if(grpmode!=0){gra(ad,0);}
			}
			mdc=(*rcd->md_get)();
			if(mdc>=0 && mdc<0xf8){
				if(thrumode){
					if(rcd->act){poplay(0,0);}
					(*rcd->md_put)(mdc);
				}
				if(inpmode>1 && (sh&1)==0){
					if((mdc>=0x90 && mdc<=0x9f)){break;}
				/*	if(mdc>=0x80 && mdc<0xf8){mdiv=mdc&0xf0;}
					if((mdc>=0x90 && mdc<=0x9f)||(mdiv==0x90 && mdc<0x80)){break;}*/
				}
			}
			mdc=0;
		}while(B_KEYSNS()==0);
		tcur(cx*6+12+xx,cy+6,8-xx,cs);

		ke=0;kc=0;sh=0;
		if(B_KEYSNS()!=0){
			ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;sh=B_SFTSNS();
			if(kc>='a' && kc<='z'){kc-=32;}
		}

		if(ke>=0x70 && ke<=0x73){rc=tm_lag;}else{rc=0;}

		if( (sh&6)==0 ){tcp=0;tcs=0;}

		if( ke==0x70 ){fnc_dis(1);}
		if( ke==0xf0 ){fnc_dis(0);}
		if( ke==0x5a ){
			if((sh&16)!=0){sttrc(&po,&cy,0);}
		}
		if( ke==1 ){
			if( cplen<0 ){
				cplen=0;r_ln=0;trk_dis(6,po,24);
			}else{
/*
				trk_pos_set(po,cx,cy);
				trksize(track);if(edit_scr==0){track1=track;}else{track2=track;}
				B_CLR_AL();fnc_dis(-1);msg_clr();edit_scr=0;cons_md(0);
				edfield(-1);sdis2(30+9,512,"",18,0,0);home2(0);
*/
				trk_ed_end(po,cx,cy);
				break;
			}
		}


/*play*/
if((sh&4)==0){
		if(kc=='Z'){timtra();}

		if(kc=='P'||ke==0x56){if( (sh&1)==0 ){poplay(ad,1);}else{poplay(ad,2);}}
		if(kc=='M'||ke==0x55){
			if( (sh&2)==0 ){
				if( (sh&1)==0 ){poplay(ad,3);}else{poplay(ad,4);}
			}else{poplay(ad,5);}
		}
		if(kc=='N' && trk[track][ad]<128){poplay(ad,6);}
		if( ke==0x57 && (sh&15)==0){rcd->sts=0;}
		if( ke==0x58 && (sh&15)==0){rcd->sts=1;}
		if( ke==0x59 && (sh&15)==0){poplay(0,0);}
}

		if(r_ln>0){rev_area(r_ad,r_ln,edit_scr);}

		if(((mdc>=0x90 && mdc<=0x9f)/*||(mdiv==0x90 && mdc<0x80)*/)){
			int	j,i;
			i=ret_midi(mdc,inpmode,ad,cy);
			if(i>0){
				trk_dis(6+cy,ad,24-cy);
				for(j=0;j<i;j=j+4){cur_down(&po,&cy);}
			}
			if(i==-1){ke=0;kc='*';}
			if(i==-2){ke=0;kc=8;}
			/*if(trk[track][po]!=0xfd){if(step_cluc(po)>=192){ke='*';}}*/
		}

		if( ((kc>='A' && kc<='Z')|| kc=='@') && (sh&4)!=0 ){
			int i=edit_scr,ex;
			cons_md(0);edit_scr=0;

			if(cnfflag&0x0400){sh=1-(sh&1);}

			if( kc=='@' ){
				ex=option22(-1,2,ad,i);
			}else{
				if(sh&1){ex=option(kc-'A',2);}else{ex=option2(kc-'A',2,ad,i);}
			}

			edit_scr=i;if(ex){edfield(0);}else{tg_copy2(1-i);}
			trk_no();trk_dis(6,po,24);rc=tm_lag-1;kc=0;
		}

/*		if( (ke>=0x3f && ke<=0x51) && (sh&4)!=0 ){*/
		if( (ke>=0x40 && ke<=0x51) && (sh&4)!=0 ){
			int	i,j;

			static char tk[18]={13,14,15,7,8,9,16,4,5,6,17,1,2,3,18,10,11,12};

/*			if( ke==0x3f ){j=trk_sel_menu(track)+1;}else*/{j=tk[ke-0x40];}

			if( j>0 && j<=36){
				trk_pos_set(po,cx,cy);
				trksize(track);i=meas_no(ad);track=j-1;
				size_max(track);trk_cluc();trk_no();cplen=0;
				po=meas_add(i);cy=padjust(&po,8);
				rc=tm_lag-1;
			}
			kc=0;ke=0;
		}

		if( ke==0x3f ){noed_cls();grpmode=0;}

		if( kc=='X' ){
			unsigned char	da=trk[track][ad];
			if(da<128 || (ad>0 && da>=0xfc && trk[track][ad-4]<128)){
				int	j,i;
				if(da>=0xfc){cur_up(&po,&cy);ad-=4;}

				tcur(1,cy+6,12,0);i=ret_chord(ad);tcur(1,cy+6,12,0);
				if(i>0){
					trk_dis(6+cy,ad,24-cy);
					for(j=0;j<i;j=j+4){cur_down(&po,&cy);}
				}
			}
		}
		if( kc=='U' ){
			int	j,i;

			i=mml_inp(ad);
			if(i>0){
				trk_dis(6+cy,ad,24-cy);
				for(j=0;j<i;j=j+4){cur_down(&po,&cy);}
			}
		}
		if( kc==13 ){
			ret(ad,cx,cy,0);
			if( es==0 ){
				trk_dis(cy+6,ad,24-cy);cx=0;
				if(inpmode==1){poplay(ad,6);}
				ke=retkey2(ecode);
			}else{ke=0x137;}
		}

/*0～9*/
		if( kc>0x2f && kc<0x3a ){
			int	ct;
			unsigned char da=trk[track][ad];
			if( da==0xeb && cx==0){cx=2;}

			if( cx>0 && (da<0xf0 || da==0xf8 || da==0xfc)){
				if( da==0xee && cx>1 ){
					cx=3;B_LOCATE(32,cy+6);i=vinput(kc-'0',0x105)+8192;
					if( es==0 ){
						trk[track][ad+2]=i&0x7f;trk[track][ad+3]=(i>>7)&0x7f;
					}
				}else{
					if( (da==0xec || da==0xe2) && cx>1 ){
						int bank;
						if(da==0xec){cx=2;}
timnore:					B_LOCATE(16+2*6,cy+6);ct=vinput(kc-'0',0x304+0x400);
						if((cmdflag&0x20)!=0 && cx==2){ct--;}
						if(ecode==0x18){
							cx=2;
							trk_dis(cy+6,ad,1);
							cons_md(0);
							ct=timsel(channele_no(ad),bank_no(ad),0,prog_no(ad));
							cons_md(1);
							if(ct<0){kc='0';goto timnore;}
							bank=(ct&0xff00)>>8;ct=ct&0xff;
							trk[track][ad+3]=bank;
						}
						if(ct<0){ct=0;}
						if(ct>127){ct=127;}
					}else{
						int	fl=0x104+0x400;
						unsigned char	tmp0[16];
						if( da==0xfc ){cx=1;}
						if(da>=0xdd && da<=0xdf && cx>1 &&(cmdflag & 0x100)!=0){fl+=0x2000;}
resame:
						B_LOCATE(15+cx*6,cy+6);
						if(da==0xe6 && cx==2){
							tmp0[0]=kc;tmp0[1]=0;
							sinput(tmp0,fl);
							ct=strch(trk[track][ad+2],tmp0);
						}else{ct=vinput(kc-'0',fl);}

						msg_clr();
						if( ((cx==0||cx==3)&& da<128)||(da>=0x80 && da<=0xef && da!=0xe7 && cx>1) ){
							if(ct>127){ct=127;}
							if(ct<0){ct=0;}
						}
						if( es==0 && da==0xfc ){
							if(ct>1024||ct<1|| ct>=meas_no(ad) ||\
							trk[track][meas_add(ct)]==0xfc ){
								msg("指定出来ない小節です。");
								goto resame;
							}else{
								ct--;
								trk[track][ad+2]=ct>>8;
								ct=ct&0xff;
							}
						}
					}
					if( es==0 ){trk[track][ad+cx]=ct;}
				}
				if( es==0 ){
					ke=retkey(ecode);
				}
				trk_cluc();trk_dis(cy+6,ad,24-cy);kc=0;
			}
			if( cx==0 && da<128 ){
				int	ct;
				unsigned char	tmp0[16];

				tmp0[0]=kc;tmp0[1]=0;
				B_LOCATE(12,cy+6);sinput(tmp0,0x504);
				if(trrhy[track]&0x80){ct=0;}else{ct=scale_no(ad);}
				ct=ctc(da,tmp0,ct);
				if( es==0 && ct>=0){
					trk[track][ad]=ct;
					ke=retkey(ecode);
				}
				trk_dis(cy+6,ad,1);
			}
			if( da>=0xfd ){
				ret(ad,cx,cy,kc);kc=0;cx=0;
				if( es==0 ){
					trk_dis(cy+6,ad,24-cy);
					if(inpmode==1){poplay(ad,6);}
					ke=retkey2(ecode);
				}else{ke=0x137;}
			}
		}

		if( ke>0x62 && ke<0x6d ){
			if( (sh & 4)!=0 ){
				if( trk[track][ad]<128 ){
					ed_ch_stgt(ad,cx,cy,ke,sh);
					/*trk_dis(y+6,ad,24-cy);*/
					trk_dis(6,po,24);

					while(ad<tr_len[track]-4){
						cur_down(&po,&cy);ad=real_ad(po,cy);
						if(trk[track][ad]<128 &&
							trk[track][ad+cx]>0){break;}
					}
				}
				ke=0;kc=0;snsclr();
			}
		}

		if(kc>='A' && kc<='G'){
			unsigned char da=trk[track][ad];
			tmp0[0]=kc;tmp0[1]=0;
			if( da<128 ){
				int	ct;
				B_LOCATE(12,cy+6);sinput(tmp0,0x504);
				if(trrhy[track]&0x80){ct=0;}else{ct=scale_no(ad);}
				ct=ctc(da,tmp0,ct);
				cx=0;
				if( es==0 && ct>=0 ){
					trk[track][ad]=ct;
					ke=retkey(ecode);
				}
			}
			if( da>0xfb ){
				ret(ad,cx,cy,kc);kc=0;cx=0;
				if( es==0 ){
					trk_dis(cy+6,ad,24-cy);
					if(inpmode==1){poplay(ad,7);}
					ke=retkey2(ecode);
				}else{ke=0x137;}
			}
			if( da==0xee ){
				int	ct;
				B_LOCATE(32,cy+6);sinput(tmp0,0x504);
				ct=bendc2(ad,tmp0)+8192;
				if( es==0 ){
					trk[track][ad+2]=ct&0x7f;
					trk[track][ad+3]=(ct>>7)&0x7f;
					ke=retkey(ecode);
				}
			}
			if( da==0xe6 && kc<='B'){
				int	ct;
				B_LOCATE(cx*6+14,cy+6);sinput(tmp0,0x504);
				ct=strch(trk[track][ad+2],tmp0);

				if( es==0 ){
					trk[track][ad+2]=ct;
					ke=retkey(ecode);
				}
			}
			if(da>=0xdd && da<=0xdf && cx>1 &&(cmdflag & 0x100)!=0){
				int	ct;
				B_LOCATE(cx*6+14,cy+6);ct=vinput(kc-'A'+10,0x2304);
				if( es==0 ){
					trk[track][cx]=ct&0x7f;
					ke=retkey(ecode);
				}
			}
			trk_dis(cy+6,ad,1);
		}

		if( kc=='@'|| kc=='`' ){
			int	i;
			i=comment_inp(ad,cy,sh);
			if(i!=0){ke=0x3e;kc=0;}
			trk_dis(6+cy,ad,24-cy);msg_clr();
		}

		ad=real_ad(po,cy);

		if( kc=='+' || kc=='.' || kc=='>' ){
			val_add(ad,cx,kc);trk_dis(6+cy,ad,24-cy);snsclr();
		}
		if( kc=='-' || kc==',' || kc=='<' ){
			val_sub(ad,cx,kc);trk_dis(cy+6,ad,24-cy);snsclr();
		}
		if( kc=='=' ){
			val_rev(ad,cx);trk_dis(cy+6,ad,1);
		}
		if( ke==0x0d ){
			int	j,ct;
			if( (sh&3)==0 ){
				if(trk_check(4)==0){
					if((ad>0 && trk[track][ad-4]<0xfc)&& trk[track][ad]!=0xfc){
						same_shift(ad,0,2);}else{same_shift(ad,0,1);}

					line_ins(po,cy);trk[track][ad]=0xfc;trk_cluc();
					trk_dis(cy+6,ad,24-cy);
					msg("SAME MEAS");
resame2:
					B_LOCATE(22,cy+6);j=vinput(0,3)-1;
					trk[track][ad+1]=j&0xff;trk[track][ad+2]=j>>8;
					msg_clr();
					if( es==0 ){
						if( j<0 || j>1023 || (j+1)>=meas_no(ad) ||
							meas_add(j+1)>=65536-44 ){
							msg("指定出来ない小節です。");
							goto resame2;
						}
					}
					if( es==0 ){
						trk_dis(cy+6,ad,1);ke=0x3e;
					}else{ke=0x137;}
				}
			}else{
				int j=meas_no(ad),j2=sttm_no(ad);ct=0;
				if( (sh&1)!=0 ){
					ct=same_meas_conv(track);
				}else{
					if(cplen<0){
						cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
						ct=same_meas_extr(track,cpadd,cplen);
					}else{
						ct=same_meas_extr(track,0,tr_len[track]-4);
					}
				}
				if(ct==0){
					trk_cluc();po=sttm_ad(meas_add(j),j2);cy=padjust(&po,cy);
					rc=tm_lag-1;
				}
			}
		}

		if( trk_check(4)==0){
			if( ke==0x6c ){
				unsigned char	da=trk[track][ad];
				if( da<0xfe && da!=0xf7 ){
					if( da==0xf6 || da==0x98 ){
						int	cc=0,ad2=ad;
						while(cc<4096){
							dat[cc++]=trk[track][ad2++];
							dat[cc++]=trk[track][ad2++];
							dat[cc++]=trk[track][ad2++];
							dat[cc++]=trk[track][ad2++];
							if(trk[track][ad2]!=0xf7){break;}
						}
						if(trk_check(cc)==0){
							int	i;
							trk_shift(ad,cc);
							for(i=0;i<cc;i++){trk[track][i+ad]=dat[i];}
							trk_cluc();trk_dis(6,po,24);
							ke=0x3e;snsclr();
						}
					}else{
						if(da>=0xfc){same_shift(ad,0,1);}
						line_ins(po,cy);
						trk_cluc();trk_dis(cy+6,ad,24-cy);
						ke=0x3e;snsclr();
					}
				}
			}
			if( kc=='/' ){
				spcon(ad,cy);msg_clr();
				if( es==0 ){
					trk_cluc();trk_dis(cy+6,ad,24-cy);ke=0x3e;
				}else{ke=0x137;}
			}
			if( kc=='?' ){
				int i=edit_scr;
				B_CLR_AL();cons_md(0);edit_scr=0;
				if(trk[track][ad]>=0x90 && trk[track][ad]<=0x97){
				user_exc_as(trk[track][ad]-0x90);}else{user_exc_as(0);}
				edit_scr=i;edfield(0);
				trk_no();trk_dis(6,po,24);rc=tm_lag-1;
			}
			if( ke==0x34 ){
				if(trk[track][ad]!=0x98 || (sh&1)!=0){
					line_ins(po,cy);
					trk[track][ad]=0x98;trk[track][ad+1]=0;
					trk[track][ad+2]=0;trk[track][ad+3]=0;
					exc_edit(ad,cy,sh);
					if(trk[track][ad+4]!=0xf7){ke=0x137;}
				}else{
					exc_edit(ad,cy,sh);
				}
			}

			if( kc=='*' ){
				if(!((ad>0 && trk[track][ad-4]<0xfc)&&\
					trk[track][ad]==0xfc)){same_shift(ad,0,1);}
				line_ins(po,cy);trk[track][ad]=0xfd;
				trk_cluc();trk_dis(cy+6,ad,24-cy);ke=0x3e;
			}
			if( kc=='[' ){
				line_ins_aft(ad);
				line_ins(po,cy);trk[track][ad]=0xf9;
				trk_dis(cy+6,ad,24-cy);ke=0x3e;
			}
			if( kc==']' ){
				line_ins_aft(ad);
				line_ins(po,cy);trk[track][ad]=0xf8;
				trk_dis(cy+6,ad,1);
				B_LOCATE(22,cy+6);trk[track][ad+1]=vinput(0,3);
				if( es==0 ){trk_dis(cy+6,ad,24-cy);ke=0x3e;}else{ke=0x137;}
			}
			if( kc=='K' ){
				int	ct,ca=trk[track][ad];
				msg("Music Key");
				if(ca!=0xf5){
					line_ins_aft(ad);
					line_ins(po,cy);
					trk[track][ad]=0xf5;trk[track][ad+1]=bkey;
					trk_dis(cy+6,ad,1);
				}
				ct=scale_sel(22,cy+6,trk[track][ad+1]);
				if(ct>=0){
					trk[track][ad+1]=ct;
					ke=0x3e;
				}else{
					ke=0;if(ca!=0xf5){ke=0x137;}
				}
				trk_dis(cy+6,ad,24-cy);
				msg_clr();
			}
		}

		if((ke==0x0f || kc==8)&& ad>0){cur_up(&po,&cy);ad=real_ad(po,cy);ke=0x37;}
		if(ke==0x37 || ke==0x137 || kc==25){
			if( tr_len[track]>ad+4 && trk[track][ad]!=0xf7){
				unsigned char	da=trk[track][ad-4],db,dc;
				int	i,ll=4;
				while(trk[track][ad+ll]==0xf7){ll=ll+4;}
				db=trk[track][ad+ll];dc=trk[track][ad];

				if(ke!=0x137){
					if(ll>1020){ll=1020;}
					for(i=0 ; i<ll ;i++){lcbuf[i]=trk[track][ad+i];}
					lcbuf[1024]=(ll>>2)-1;
				}

				if(dc==0xfc||\
				(dc==0xfd && (ad==0 || !(da<0xfc && db==0xfc)))||\
				(dc<0xfc &&(ad==0||da>=0xfc)&& db==0xfc)){

					if(lcbuf[0]==0xfc &&(ad>0 && da<0xfc)&& db<0xfc){
						same_shift(ad,1,2);}else{same_shift(ad,1,1);}
				}
				line_del(po,cy);snsclr();
			}
		}

		if((ke==0x3a || kc==12) && ((lcbuf[0]!=0xf6 && lcbuf[0]!=0x98 && trk_check(4)==0)||trk_check(40)==0)){
			if( lcbuf[0]<254 ){
				unsigned char	da=trk[track][ad-4],db=trk[track][ad];
				int	i,ll=(lcbuf[1024]+1)*4;

				if(lcbuf[0]==0xfc||\
				(lcbuf[0]==0xfd && (ad==0 || !(da<0xfc && db==0xfc)))||\
				(lcbuf[0]<0xfc && (ad==0||da>=0xfc)&& db==0xfc)){
					if(lcbuf[0]==0xfc &&(ad>0 && da<0xfc)&& db<0xfc){
						same_shift(ad,0,2);}else{same_shift(ad,0,1);}
				}
				trk_shift(ad,ll);
				for(i=0;i<ll;i++){trk[track][ad+i]=lcbuf[i];}
				trk_cluc();trk_dis(cy+6,ad,24-cy);
				snsclr();
			}
		}

		if( ke==0x10 ){
			if( (sh&3)==0 ){
				msg("JUMP MEAS=");j=vinput(0,4);
				if( es==0 && j>0 ){
					po=meas_add(j);cy=padjust(&po,8);
				}
				msg_clr();rc=tm_lag-1;
			}else{
				if( (sh&1)!=0 ){
					int es;
					trksize(track);cons_md(0);es=tagj(&po,0);cons_md(1);
					if(es==0){
						trk_no();trk_cluc();size_max(track);
						cplen=0;cy=padjust(&po,8);
					}
				}else{
					cons_md(0);tagj(&ad,1);cons_md(1);
				}
			}
		}
		if( kc==20 ){
			int	i,j;
jmptrk:
			msg("JUMP TRACK=");j=vinput(0,2/*+0x200*/);

			msg_clr();

			/*if(ecode==0x18){
				j=trk_sel_menu(track)+1;if(j==0){goto jmptrk;}
			}*/

			if( es==0 && j>0 && j<=36){
				trk_pos_set(po,cx,cy);
				trksize(track);i=meas_no(ad);track=j-1;
				size_max(track);trk_cluc();trk_no();cplen=0;
				po=meas_add(i);cy=padjust(&po,8);
			}
			msg_clr();rc=tm_lag-1;
		}


/*cursor*/
		if( (ke==0x3b || ke==0x3d) && (sh&6)!=0 ){
			trk_pos_set(po,cx,cy);
			trksize(track);

			if( (sh&6)==2){
				int	i,j;
				if(tcp==0){i=meas_no(ad);tcp=i;j=sttm_no(ad);tcs=j;}else{i=tcp;j=tcs;}
				if( ke==0x3b){trk_next(0);}else{trk_next(1);}
				po=meas_add(i);if(po<0){po=tr_len[track]-4;}
				po=sttm_ad(po,j);
			}else{
				int	i,j;
				if(tcp==0){i=meas_no(ad);tcp=i;j=step_no(ad);tcs=j;}else{i=tcp;j=tcs;}
				if( ke==0x3b){trk_next(0);}else{trk_next(1);}
				po=meas_add(i);if(po<0){po=tr_len[track]-4;}
				po=step_ad(po,j);
			}
			size_max(track);
			trk_no();trk_cluc();cplen=0;cy=padjust(&po,cy);ke=0;
			snsclr();rc=tm_lag-1;
		}


		if( ke==0x3c && (sh&4)!=0 ){roll_up(&po,&cy);ke=0;}
		if( ke==0x3e && (sh&4)!=0 ){roll_down(&po,&cy);ke=0;}

		if( kc==0x13 || ke==0x3b){cx--;if( cx<0 ){cx=3;ke=0x3c;}}
		if( kc==0x04 || ke==0x3d || kc==' '){cx++;if( cx>3 ){cx=0;ke=0x3e;}}
		if( kc==0x05 || ke==0x3c){cur_up(&po,&cy);}
		if( kc==0x18 || ke==0x3e){cur_down(&po,&cy);}

/*roll*/
		if( ke==0x39 || kc==0x12){
			if( sh&4 ){
				int	i;
				i=meas_no(ad)-1;
				if( i>0 ){po=meas_add(i);cy=padjust(&po,8);}
				snsclr();
			}else{
				if((sh&1)==0){
					if( po>0 ){
						po=real_po(po,24);if( po<0 ){po=0;cy=0;}
						trk_dis(6,po,24);snsclr();
					}else{cy=0;}
				}else{roll_up(&po,&cy);}
			}
		}
		if( ke==0x38 || kc==0x03){
			if( sh&4 ){
				int	i;
				i=meas_no(ad);i=meas_add(i+1);
				if( i>=0 ){po=i;cy=padjust(&po,8);}
				snsclr();
			}else{
				if((sh&1)==0){
					if( po<tr_len[track] ){
						if( real_ad(po,24)>=tr_len[track] ){
							cy=real_cy(po,tr_len[track]-4);
						}else{
							po=real_ad(po,24);
							if(real_ad(po,24)>=tr_len[track] ){
								po=tr_len[track]-4;cy=padjust(&po,23);
							}
						}
						trk_dis(6,po,24);snsclr();
					}
				}else{roll_down(&po,&cy);}
			}
		}


		if( kc==0x17 ){roll_up(&po,&cy);kc=0;ke=0;}
		if( kc==0x1a ){roll_down(&po,&cy);kc=0;ke=0;}

		if( ke==0x36 ){
			if((sh&1)==0){cx=0;cy=0;}else{sttrc(&po,&cy,1);}
		}

		ad=real_ad(po,cy);
/*f1*/
		if( ke==0x63 ){
			if((sh&1)==0){
				po=0;cy=0;cx=0;
			}else{
				note_ctrl(ad);trk_cluc();
				rc=tm_lag-1;
			}
			trk_dis(6,po,24);

		}
		if( ke==0x64 ){
			if( (sh&1)==0 ){
				if( po+4<tr_len[track] ){
					po=tr_len[track]-4;cy=padjust(&po,20);
				}
			}else{
				int j=meas_no(ad),j2=sttm_no(ad);
				undobuf(0);
				if(cplen<0){
					cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
					trk_opt_sub(track,cpadd,cplen);
				}else{
					trk_opt_sub(track,ad,tr_len[track]-ad-4);
				}
				trk_cluc();po=sttm_ad(meas_add(j),j2);cy=padjust(&po,cy);
				rc=tm_lag-1;
			}
		}
		if( ke==0x65 ){
			if(cnfflag&0x01000){es=0;goto replinp2;}
replinp:
			if((sh&1)==0){msg("REPLACE=");sinput(repl,0x200+30+0x10000);
			}else{msg("DELETE=");sinput(delt,0x200+30+0x10000);}
			msg_clr();

			if(ecode==0x18){
				int ct;
replinp2:
				if((sh&1)==0){
					ct=repl_select(0);if(ct>=0){strcpy(repl,repl_d[ct&127]);}
				}else{
					ct=repl_select(1);if(ct>=0){strcpy(delt,repl_d[ct&127]);}
				}
				if((ct&128)==0||ct==-1){goto replinp;}
				if(ct==-2){es=1;}
			}
			if( es==0 ){
				if(cplen<0){
					cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
					replace(cpadd,cplen,sh&1,0);
				}else{
					replace(ad,tr_len[track]-ad-4,sh&1,0);
				}
				if(ad>tr_len[track]-4){po=tr_len[track]-4;cy=0;}
				trk_cluc();trk_dis(6,po,24);
			}
		}
		if( ke==0x66 ){
			if(cnfflag&0x01000){es=0;goto findinp2;}
findinp:
			msg("FIND=");sinput(srch,0x200+30+0x10000);msg_clr();

			if(ecode==0x18){
				int ct;
findinp2:
				ct=repl_select(2);if(ct>=0){strcpy(srch,repl_d[ct&127]);}
				if((ct&128)==0||ct==-1){goto findinp;}
				if(ct==-2){es=1;}
			}
			if(es==0){ke=0x67;kc=0;}
		}
		if( ke==0x67 ){
			int	j;
			j=find(ad,sh);
			if(j>=0){
				if(j<po || j>real_ad(po,23)){
					po=real_po(j,13);cy=13;if( po<0 ){po=0;cy=0;}
					trk_dis(6,po,24);
				}else{cy=real_cy(po,j);}
			}
		}
/*f6*/
		if( ke==0x68 || ke==0x69 ){
			if( (sh&1)!=0){
				if( ke==0x68 ){
					undobuf(0);
					msg("UNDOバッファに登録しました。");
				}else{
					undobuf(1);
					if(real_ad(po,cy)>=tr_len[track]){
						po=tr_len[track]-4;cy=padjust(&po,18);
					}
					trk_dis(6,po,24);
				}
				ke=0;
			}
		}

		if( ke==0x68 ){
			if( cplen<0 ){
				cplen=0;trk_dis(6,po,24);
			}else{
				cpadd=ad;cplen=-1;cpcy=cy;
				trk_dis(cy+6,ad,1);
			}
		}

		if( ke==0x6a ){
			if( (sh&1)!=0){
				if( cpleng>0 ){
					undobuf(0);
					if(trk_check(cpleng)==0){
						int	ec;

						cpbuf[cpleng]=0xfe;
						ec=trk_mix_sub(trk[track]+ad,cpbuf);
						if(!size_change(track,ad+ec)){
							dat[ec]=0xfe;ec+=4;
							memcpy_l(&trk[track][ad],dat,ec);
							tr_len[track]=ad+ec;
							trk_cluc();trk_dis(6,po,24);
						}
					}
				}
				ke=0;
			}
		}

		if( ke==0x69 || ke==0x6a ){
			if( cplen<0 && cpadd!=ad){
				cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;cpcy=cy;}
				cpleng=cpybuf_set(track,cpadd,cplen);
				if(cpbuf[cpleng-4]==254){cpleng-=4;cplen-=4;}

				if(ke==0x69&& cplen>0){
					undobuf(0);
					same_shift(cpadd,1,buf_meas(cpbuf,cpleng));

		if(cpadd>0 && trk[track][cpadd-4]<0xfc && trk[track][cpadd]==0xfc){
			trk[track][cpadd]=0xfd;cpadd+=4;cplen-=4;}

					trk_delete(cpadd,cplen);
					trk_cluc();po=cpadd;cy=padjust(&po,cpcy);
				}else{
					trk_dis(6,po,24);
				}
			}
		}
		if( ke==0x6b ){
			if( cpleng>0 ){
				undobuf(0);
				if(trk_check(cpleng)==0){
					int	i;
					same_shift(ad,0,buf_meas(cpbuf,cpleng));
					trk_shift(ad,cpleng);
					for(i=0;i<cpleng;i++){trk[track][i+ad]=cpbuf[i];}
					if( (sh&1)!=0){dat_rev(track,ad,cpleng);}
					trk_cluc();trk_dis(6,po,24);
				}
			}
		}
		if( kc=='R' ){
			if( replicate(ad,sh)==0 ){trk_cluc();trk_dis(6,po,24);}
		}
		if( kc=='T' ){
			if( replicate2(ad,sh)==0 ){trk_cluc();trk_dis(6,po,24);}
		}

		if( kc==14 ){
			int	i;
			i=meas_no(ad);i=meas_add(i+1);
			if( i>=0 ){po=i;cy=padjust(&po,8);}
			snsclr();
		}
		if( kc==2 ){
			int	i;
			i=meas_no(ad)-1;
			if( i>0 ){po=meas_add(i);cy=padjust(&po,8);}
			snsclr();
		}
		if( kc=='\\' || kc=='|' ){
			if( ad<(tr_len[track]-4) ){
				bendc(ad,sh);trk_dis(6,po,24);
			}
		}
		if( kc=='Q' ){
			if(cplen<0){
				cplen=abs(cpadd-ad);if(cpadd>ad){cpadd=ad;}
				trk_quan(track,cpadd,cpadd+cplen);
			}else{
				trk_quan(track,ad,tr_len[track]-4);
			}
			trk_cluc();trk_dis(6,po,24);
		}
		if( ke==0x52 || ke==0x53){
			if( trrhy[track]&0x80 ){
				trk_pos_set(po,cx,cy);
				B_CLR_AL();cons_md(0);rtrk_ed(ke-0x52,sh);
				edfield(0);po=tr_pos[track][edit_scr][0];cx=0;cy=0;trk_no();
				trk_cluc();trk_dis(6,po,24);rc=tm_lag-1;
			}
		}

		if( kc==':' ){
			inpmode++;if(inpmode>5){inpmode=0;}
			inmd_disp();midi_clr();mdiv=0;
		}

		if( ke==0x54 ){
			fnc_dis(-1);cons_md(0);B_CLR_AL();help(1);
			edfield(0);B_CLR_AL();fnc_dis(0);trk_dis(6,po,24);rc=tm_lag-1;
		}
		if( kc=='I' ){
			if(grpmode!=0){noed_cls();grpmode=0;}
			dat_info(ad,sh);
		}
		if( edit_scr==0){
			if(kc==7){
				grpmode++;if(grpmode>4){grpmode=0;}
				noed_cls();if(grpmode!=0){rc=tm_lag;gra(ad,0);}
			}
		}

		if( kc=='L' ){
			if( (sh&1)==0 ){
				partf(0,ad);trk_dis(6,po,24);
			}else{
				trk_pos_set(po,cx,cy);
				B_CLR_AL();cons_md(0);load_sub(0);goto track_edit_top;
			}
		}
		if( kc=='S' ){
			if( (sh&1)==0 ){
				partf(1,ad);trk_dis(6,po,24);
			}else{
				trk_pos_set(po,cx,cy);
				B_CLR_AL();cons_md(0);save_sub(0);goto track_edit_top;
			}
		}
		if( kc=='O' ){partf(2,ad);trk_dis(6,po,24);}

		if( kc=='V' ){
			if(cplen<0){
				cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
				po=vis_edit(cpadd,cplen,sh);
			}else{
				po=vis_edit(ad,0,sh);
			}
			trk_cluc();cy=padjust(&po,8);
			rc=tm_lag-1;
		}

		if( kc=='W' ){
			trk_pos_set(po,cx,cy);
			trksize(track);tg_copy(edit_scr);B_CLR_AL();
			if(grpmode!=0){noed_cls();grpmode=0;}
			if(sh&1){track1=track;track2=track;}
			if(edit_scr==0){
				track1=track;track=track2;edit_scr=1;
			}else{
				track2=track;track=track1;edit_scr=0;
			}
			if(sh&1){trk_pos_set(po,cx,cy);}
			goto track_edit_top;
		}
		if( (kc==0x17 || kc==0x16) && trk[track][ad]==0xfc ){
			int	nn,pp,ln;

			nn=(trk[track][ad+2]&3)*256+trk[track][ad+1]+1;
			pp=meas_add(nn);
			if(pp>=0){
				if(grpmode!=0){noed_cls();grpmode=0;}

				edit_scr=1-edit_scr;trk_cluc();cons_md(1);
				edfield(1);

				sdis(edit_scr*56+1,556," --- SAME MEAS VIEW ---",38,15,14);
/*
char tmp[64];
				strcpy(tmp," --- SAME MEAS VIEW --- [");
				strcat(tmp,fstr(nn,4));strcat(tmp," ]");
				sdis(edit_scr*56+1,556,tmp,38,15,14);
*/
				ln=meas_len(pp)>>2;
				if(ln<24){trk_dis(6+((23-ln)>>1),pp,ln);}else{trk_dis(6,pp,24);}

				tg_copy(edit_scr);
				edit_scr=1-edit_scr;trk_cluc();cons_md(1);

				B_CLR_AL();
				trk_dis(6,po,24);
			}
		}
		if( kc=='}' ){
			cons_md(0);stgt_as();cons_md(1);
		}
		if( kc=='{' ){
			B_CLR_AL();cons_md(0);part_as();
			edfield(0);trk_no();trk_dis(6,po,24);rc=tm_lag-1;
		}
		if( ke==0x29 && (sh&2)==2){
			int	j;
			j=find_rep(ad);
			if(j>=0){
				if(j<po || j>real_ad(po,23)){
					po=real_po(j,13);cy=13;if( po<0 ){po=0;cy=0;}
					trk_dis(6,po,24);
				}else{cy=real_cy(po,j);}
			}
		}
	}
}

/***************************/
void trk_ed_end(int po,int cx,int cy)
{
	trk_pos_set(po,cx,cy);
	trksize(track);if(edit_scr==0){track1=track;}else{track2=track;}
	B_CLR_AL();fnc_dis(-1);msg_clr();edit_scr=0;cons_md(0);
	edfield(-1);sdis2(30+9,512,"",18,0,0);home2(0);
}

void trk_pos_set(int po,int cx,int cy)
{
	tr_pos[track][edit_scr][0]=po;
	tr_pos[track][edit_scr][1]=cx;tr_pos[track][edit_scr][2]=cy;
}

/***************************/
void	val_add(int ad,int cx,int kc)
{
	int	i;
	unsigned char da=trk[track][ad];

	if(da<128 ){
		i=trk[track][ad+cx];
		if((cx==0 || cx==3)){
			if( kc=='>' ){if(cx==0){i+=12;}else{i+=10;}}else{i++;}
			i=i&127;
		}else{
			if( kc=='>' ){i+=12;}else{i++;}
			i=i&255;
		}
		trk[track][ad+cx]=i;
	}else{
		if( (da<0xf0 || da==0xf8 || da==0xf5) && cx>0){
			if( cx>1 && da==0xee ){
				i=trk[track][ad+2]+trk[track][ad+3]*128;
				if( kc=='>' ){i+=10;}else{i+=1;}
				trk[track][ad+2]=i&0x7f;trk[track][ad+3]=(i>>7)&0x7f;
			}else{
				if( cx>1 && (da==0xec)){cx=2;}
				if( cx>0 && (da==0xf5)){cx=1;}
				if( cx>1 && (da==0xe6)){cx=2;}
				i=trk[track][ad+cx];
				if( kc=='>' ){if(cx==1){i+=12;}else{i+=10;}}else{i+=1;}
				if(cx>1 && da>=0x80 && da<0xf0 && da!=0xe7){i=i&127;}
				if(cx>0 && da==0xf5){i=i&31;}
				if(cx>1 && da==0xe6){if(i>32){i=0;}}

				trk[track][ad+cx]=i;
			}
		}
		if(cx==0){
			if( da>=0x90 && da<=0x97 ){
				trk[track][ad]=((da+1)&7)+0x90;
			}
			if( da==0xeb ){
				trk[track][ad+2]=(trk[track][ad+2]+1)&127;
			}
		}

		if( cx>0 ){
			if(da==0xfc ){
				int ct=(trk[track][ad+2]&3)*256+trk[track][ad+1];
				ct++;
				if(ct<1024 && ct>=0 && ct+1<meas_no(ad)){
					trk[track][ad+2]=ct>>8;trk[track][ad+1]=ct&0xff;
				}
			}
		}
	}
	trk_cluc();
}

void	val_sub(int ad,int cx,int kc)
{
	int	i;
	unsigned char da=trk[track][ad];

	if( da<128 ){
		i=trk[track][ad+cx];
		if((cx==0 || cx==3)){
			if( kc=='<' ){if(cx==0){i+=116;}else{i+=118;}}else{i+=127;}
			i=i&127;
		}else{
			if( kc=='<' ){i+=244;}else{i+=255;}
			i=i&255;
		}
		trk[track][ad+cx]=i;
	}else{
		if( (da<0xf0 || da==0xf8 || da==0xf5) && cx>0){
			if( cx>1 && da==0xee ){
				i=trk[track][ad+2]+trk[track][ad+3]*128;
				if( kc=='<' ){i-=10;}else{i-=1;}
				trk[track][ad+2]=i&0x7f;trk[track][ad+3]=(i>>7)&0x7f;
			}else{
				if( cx>1 && (da==0xec)){cx=2;}
				if( cx>0 && (da==0xf5)){cx=1;}
				if( cx>1 && (da==0xe6)){cx=2;}
				i=trk[track][ad+cx];
				if( kc=='<' ){if(cx==1){i+=244;}else{i+=246;}}else{i+=255;}
				if(cx>1 && da>=0x80 && da<0xf0 && da!=0xe7){i=i&127;}
				if(cx>0 && da==0xf5){i=i&31;}
				if(cx>1 && da==0xe6){if(i>32){i=32;}}

				trk[track][ad+cx]=i;
			}
		}
		if(cx==0){
			if( da>=0x90 && da<=0x97 ){
				trk[track][ad]=((da+7)&7)+0x90;
			}
			if( da==0xeb ){
				trk[track][ad+2]=(trk[track][ad+2]+127)&127;
			}
		}
		if( cx>0 ){
			if(da==0xfc ){
				int ct=(trk[track][ad+2]&3)*256+trk[track][ad+1];
				ct--;
				if(ct<1024 && ct>=0 &&  ct+1<meas_no(ad)){
					trk[track][ad+2]=ct>>8;trk[track][ad+1]=ct&0xff;
				}
			}
		}
	}
	trk_cluc();
}

void	val_rev(int ad,int cx)
{
	if( cx>1 && trk[track][ad]==0xee ){
		int	i;
		i=16384-(trk[track][ad+2]+trk[track][ad+3]*128);
		trk[track][ad+2]=i&0x7f;trk[track][ad+3]=(i>>7)&0x7f;
	}
	if( cx>0 && trk[track][ad]==0xf5 ){
		int	i;
		i=trk[track][ad+1];
		trk[track][ad+1]=i^16;
	}
	if( cx==2 && trk[track][ad]==0xe6 ){
		int	i;
		i=trk[track][ad+2];
		if(i>0){
			if( i>16){i=i-16;}else{i=i+16;}
		}
		trk[track][ad+2]=i;
	}
	if( cx==3 && trk[track][ad]==0xeb && trk[track][ad+2]==10 ){
		int	i;
		i=128-trk[track][ad+3];
		if(i<0){i=0;}
		if(i>127){i=127;}
		trk[track][ad+3]=i;
	}
}

/***************************/
void	ed_ch_stgt(int ad,int cx,int cy,int ke,int sh)
{
		int nn=(ke-0x63);

if(cnfflag&0x2000){sh=1-(sh&1);}

		if( cx<2 ){
			trk[track][ad+1]=stgt_tbl[nn];
			trk[track][ad+2]=stgt_tbl[nn+10];
		}else{
			if( cx==2 ){
				trk[track][ad+2]=stgt_tbl[nn+10];
			}else{
				trk[track][ad+3]=stgt_tbl[nn+20];
			}
		}

		if(sh&1){
			int ad2=ad;
			while(ad2>0 && trk[track][ad2-4]<128 && trk[track][ad2-3]==0 &&
				trk[track][ad2-2]!=0 && trk[track][ad2-1]!=0 ){
				ad2-=4;

				if( cx<2 ){
					trk[track][ad2+2]=stgt_tbl[nn+10];
				}else{
					if( cx==2 ){
						trk[track][ad2+2]=stgt_tbl[nn+10];
					}else{
						trk[track][ad2+3]=stgt_tbl[nn+20];
					}
				}

			}
		}

		trk_dis(cy+6,ad,24-cy);
/*
		while(ad<tr_len[track]-4){
			cur_down(&po,&cy);ad=real_ad(po,cy);
			if(trk[track][ad]<128 &&
				trk[track][ad+cx]>0){break;}
		}
*/
}

/***************************/
int	retkey(int ecode)
{
	int	ke;

	ke=0x3d;			/*右*/
	if( ecode==13 ){ke=0x3e;}	/*下*/
	if( ecode==0x13 ){ke=0x3b;}	/*左*/
	if( ecode==0x18 ){ke=0x3e;}	/*下*/
	if( ecode==0x05 ){ke=0x3c;}	/*上*/

	return ke;
}

int	retkey2(int ecode)
{
	int	ke;
	ke=0x3e;			/*下*/
	if( ecode==0x13 ){ke=0x3b;}	/*左*/
	if( ecode==0x05 ){ke=0x3c;}	/*上*/

	return ke;
}

/***************************/
void	inmd_disp()
{
	sdis2(30+9,512,inmd[inpmode],18,15,8);
}

/***************************/
void	cur_up(int *po,int *cy)
{
	if( *cy>0 ){
		*cy=*cy-1;
	}else{
		if( *po>3 ){
			TXRASCPY((28*4+3)*256+(29*4+3),24*4,3+0x8000);
loop:			*po=*po-4;
			if(trk[track][*po]==0xf7){goto loop;}
			trk_dis(6,*po,1);
		}
	}
}

void	cur_down(int *po,int *cy)
{
	if( *cy<23 && tr_len[track]>real_ad(*po,*cy+1) ){
		*cy=*cy+1;
	}else{
		if( real_ad(*po,24)<tr_len[track] ){
			TXRASCPY((7*4)*256+(6*4),24*4,3);
			*po=real_ad(*po,1);trk_dis(29,real_ad(*po,23),1);
		}
	}
}

/***************************/
void	roll_up(int *po,int *cy)
{
cur_up_loop:
	if( *po>3 ){
		TXRASCPY((28*4+3)*256+(29*4+3),24*4,3+0x8000);
loop:		*po=*po-4;if( trk[track][*po]==0xf7){goto loop;}

		trk_dis(6,*po,1);
		if( *cy<23 && tr_len[track]>real_ad(*po,*cy+1) ){*cy=*cy+1;}
	}

	if( BITSNS(7)&2 || BITSNS(7)&16 ){goto cur_up_loop;}
	snsclr();
}

void	roll_down(int *po,int *cy)
{
cur_down_loop:
	if( real_ad(*po,24)<tr_len[track] ){
		TXRASCPY((7*4)*256+(6*4),24*4,3);
loop:		*po=*po+4;if( trk[track][*po]==0xf7){goto loop;}

		trk_dis(29,real_ad(*po,23),1);
		if( *cy>0 ){*cy=*cy-1;}
	}

	if( BITSNS(7)&1 || BITSNS(7)&64 ){goto cur_down_loop;}
	snsclr();
}

/***************************/
void	line_del(int po,int cy)
{
	int	a,ad=real_ad(po,cy),co=4;
	a=trk[track][ad];

	if((a==0xf6 || a==0x98)){
		while(trk[track][ad+co]==0xf7){co+=4;}
	}
	trk_delete(ad,co);
	trk_cluc();trk_dis(cy+6,ad,24-cy);
}

void	line_ins(int po,int cy)
{
	trk_shift(real_ad(po,cy),4);
	TXRASCPY(28*1024+3*256+29*4+3,(24-cy)*4,3+0x8000);
	TXRASCPY(63*1024+(6+cy)*4,4,3);
}

void	line_ins_aft(int ad)
{
	if( (ad>0 && trk[track][ad-4]>=0xfc) && trk[track][ad]==0xfc){same_shift(ad,0,1);}
}

/***************************/
void	ret(int ad,int cx,int cy,int kc)
{
	int	i,j,ct,ct2;
	char	tmp0[256],tmp1[256];

	if(trk_check(4)!=0){return;}

	line_ins_aft(ad);
	line_ins(real_po(ad,cy),cy);
	trk[track][ad]=60;trk[track][ad+1]=48;trk[track][ad+2]=46;trk[track][ad+3]=64;

	if( ad>0 ){
		j=ad-4;
		while( trk[track][j]>127 && j>=0){j=j-4;}
		if( j>=0 ){for(i=0 ;i<=3;i++){trk[track][ad+i]=trk[track][j+i];}}
	}

ret1:
	while(1){
		ct=trk[track][ad];
		strcpy(tmp0,keystr2(ct,scale_no(ad)));

		strcpy(tmp1,keystr(ct));
		if( scale_no(ad)!=0 && tmp1[1]==32 ){tmp0[1]='n';}/**/

		if( ct>127 ){tmp0[0]=0;}

		B_LOCATE(12,cy+6);
		if(kc>0){
			tmp0[0]=kc;tmp0[1]=0;sinput(tmp0,0x504);
		}else{
			sinput(tmp0,0x904);
		}

		if(trrhy[track]&0x80){ct=0;}else{ct=scale_no(ad);}
		ct=ctc(trk[track][ad],tmp0,ct);
		if( ct>=0 ){trk[track][ad]=ct;trk_dis(cy+6,ad,1);break;}
		if( es!=0 ){break;}
		if(ecode==0x18||ecode==0x05){return;}
	}
	if( ecode==0x13){return;}
	if( trk[track][ad]>127 ){es=1;}
ret2:
	if( es==0 && ecode!=13 ){
		fnc_dis(2);ct=trk[track][ad+1];
		B_LOCATE(22,cy+6);trk[track][ad+1]=vinput(ct,0x904);
		fnc_dis(0);
		if(ecode==0x13){ecode=0;goto ret1;}
		if(ecode==0x18||ecode==0x05){return;}
	}
	if( es==0 && ecode!=13 ){
		if( ct==trk[track][ad+1]){
			ct2=trk[track][ad+2];
		}else{
			ct2=trk[track][ad+1]-1;if(ct2<1){ct=0;}
		}
ret3:
		B_LOCATE(28,cy+6);trk[track][ad+2]=vinput(ct2,0x904);
		if(ecode==0x13){ecode=0;goto ret2;}
		if(ecode==0x18||ecode==0x05){return;}
	}
	if( es==0 && ecode!=13 ){
		fnc_dis(8);
		B_LOCATE(34,cy+6);ct=vinput(trk[track][ad+3],0x903);
		fnc_dis(0);
		if(ct<0){ct=0;}
		if(ct>127){ct=127;}
		trk[track][ad+3]=ct;
		if(ecode==0x13){ecode=0;ct2=trk[track][ad+2];goto ret3;}
	}
}

/***************************/
int	comment_inp(int ad,int cy,int sh)
{
	int	i,len,po;
	char	tmp0[64];

	msg("Comment");
	if(trk[track][ad]!=0xf6 ||(sh&1)!=0){
		if(trk_check(40)!=0){return(0);}
		line_ins(real_po(ad,cy),cy);
		tmp0[0]=0;B_LOCATE(15,cy+6);sinput(tmp0,20);
		if(es!=0){line_del(real_po(ad,cy),cy);return(0);}

		line_ins_aft(ad);
		trk_shift(ad,40-4);len=20;
	}else{
		len=0;po=ad;
		tmp0[len++]=trk[track][po+2];tmp0[len++]=trk[track][po+3];

		while(trk[track][po+4]==0xf7){
			po+=4;tmp0[len++]=trk[track][po+2];tmp0[len++]=trk[track][po+3];
		}
		tmp0[len]=0;spcut(tmp0);

		if(len<=20){
			B_LOCATE(15,cy+6);sinput(tmp0,20);
		}else{
			B_LOCATE(7,1);sinput(tmp0,len);msg_clr();
		}
		if(es!=0){return(0);}
	}
	strcpy(tmp0,spadd(tmp0,len));
	for(i=0;i<len;i+=2){
		po=ad+i*2;
		trk[track][po]=0xf7;trk[track][po+1]=0;
		trk[track][po+2]=tmp0[i];trk[track][po+3]=tmp0[i+1];
	}
	trk[track][ad]=0xf6;return(4);
}

/***************************/
void	spcon(int ad,int cy)
{
	int	i,ct,ctl;
	char	tmp0[256];

	line_ins_aft(ad);
	line_ins(real_po(ad,cy),cy);
	for(i=0;i<=3;i++){trk[track][ad+i]=0;}
	msg("Special Controler");

	if( (cmdflag&0x4)==0){
spc_spcsym:	tmp0[0]=0;B_LOCATE(15,cy+6);sinput(tmp0,0x305);

		if( ecode==0x18){
spc_spcsel:		B_LOCATE(12,cy+6);B_PRINT("Special Controler");
			ct=spc_select();tmp0[0]=ct&0xff;tmp0[1]=ct>>8;tmp0[2]=0;
			B_LOCATE(12,cy+6);B_PRINT("                 ");
			if(ct==0){goto spc_spcsym;}
		}
	}else{
		goto spc_spcsel;
	}

	ct=spc_code(tmp0,&ctl);if(ct==0){es=1;return;}else{es=0;}
	if( ct==0xe2||ct==0xec){trk[track][ad+2]=prog_no(ad-4);
		if(ctl>0 && (cmdflag&0x20)!=0){ctl--;}
	}
	if( ctl>127 ){ctl=127;}

	if( ct==0xeb ){trk[track][ad+2]=128;}
	if( ctl>=0   ){trk[track][ad+2]=ctl;}
	if( ct==0xee ){trk[track][ad+2]=0;trk[track][ad+3]=64;}
	if( ct==0xe6 ){trk[track][ad+2]=channele_no(ad);}

	trk[track][ad]=ct;trk_cluc();trk_dis(cy+6,ad,1);
	if( es==0 ){
		int	i;
		B_LOCATE(22,cy+6);i=vinput(trk[track][ad+1],0x904);
		if( i<0 ){i=0;}
		if( i>255){i=255;}
		trk[track][ad+1]=i;
	}
	if( es==0 ){
		if( ct==0xee ){
			int	i;
			tmp0[0]=0;B_LOCATE(32,cy+6);sinput(tmp0,0x905);
			i=bendc2(ad,tmp0)+8192;
			trk[track][ad+2]=i&0x7f;trk[track][ad+3]=(i>>7)&0x7f;
		}

		if( (ct==0xec || ct==0xe2)&& ctl<0 ){
			int	i;
			if( (cmdflag&0x1000)!=0 ){
spc_timno:
				i=trk[track][ad+2];
				if((cmdflag&0x20)!=0){i++;}
				B_LOCATE(28,cy+6);i=vinput(i,0xb03);
				if((cmdflag&0x20)!=0){i--;}
				if(i<0){i=0;}
				if(i>127){i=127;}

				if(ecode==0x18){
					trk_dis(cy+6,ad,1);
spc_timsl:				cons_md(0);
					i=timsel(channele_no(ad),bank_no(ad),0,prog_no(ad));
					cons_md(1);
					if(i>=0){
						trk[track][ad+2]=i&0xff;trk[track][ad+3]=i>>8;
						return;}
					if(i<0){goto spc_timno;}
				}
			}else{goto spc_timsl;}
			trk[track][ad+2]=i;
		}

		if( ct!=0xee && ct!=0xec && ct!=0xe2 && ctl<0 ){
			int	i;
			char	tmp0[64];

			if(ct>=0xdd && ct<=0xdf &&(cmdflag & 0x100)!=0){
				B_LOCATE(28,cy+6);i=vinput(trk[track][ad+2],0x2904);
			}else{
				b_striS(tmp0,trk[track][ad+2]&127);
				B_LOCATE(28,cy+6);sinput(tmp0,0x904);
				if(ct==0xe6){i=strch(trk[track][ad+2],tmp0);}else{i=str_val(tmp0);}
			}

			if( ct!=0xe7 ){
				if( i<0 ){i=0;}
				if( i>127 ){i=127;}
			}else{
				if( i<=0 ){i=1;}
				if( i>255 ){i=255;}
			}

			trk[track][ad+2]=i;
		}
	}
	if( es==0 ){
		if( ct!=0xee && ct!=0xed && ct!=0xec && ct!=0xe6  ){
			int	i,fl=0x904;

			if(ct>=0xdd && ct<=0xdf && (cmdflag & 0x100)!=0){fl+=0x2000;}

			B_LOCATE(33,cy+6);i=vinput(trk[track][ad+3],fl);
			if( i<0 ){i=0;}
			if( ct!=0xe7 ){
				if( i>127 ){i=127;}
			}else{
				if( i>255 ){i=255;}
			}
			trk[track][ad+3]=i;
		}
	}
}

/***************************/
char	*trk_dis(int y,int ad,int line)
{
	static char	tmp0[64],tmp1[64];
	int	i,po,cc=0,x,a,b,c,d,capi,skey;
	int	meas=0,meas2=0,step=0,sta;
	unsigned char	*bp=trk[track];

	sta=SUPER(0);
	x=edit_scr*56+1;po=ad;
	skey=scale_no(ad);

	if(line==1){
		if(po==0 || bp[po-4]>=0xfc || bp[po]==0xfc){goto clpo;}
		if(!(bp[po]<0xf0 &&(po==0||bp[po-4+1]!=0||bp[po-4]>=0xf0))){goto trpo;}
	}
clpo:
	if(y>=0){
		while(po>0){
			unsigned char	e;
			po=po-4;e=bp[po];
			if(meas2==0){
				if(e<0xf0 &&(po==0||bp[po-4+1]!=0||bp[po-4]>=0xf0)){
					if(bp[po-4]==0xf7){
						while(bp[po-4]==0xf7){po-=4;}
						if(bp[po-4]==0x98 && bp[po-4+1]==0){goto next1;}
					}
					step++;
				}
			}
next1:
			if( e==0xfd ){
				meas=bp[po+2]*256+bp[po+3];break;
			}else{
				if( e>=0xfc || bp[po+4]==0xfc){meas2++;}
			}
		}
		meas+=meas2;step++;meas++;po=ad;
	}
trpo:
	cons_md(0);
	while(line!=0){
		cc=0;
redis:
		if( po>=tr_len[track] ){tmp0[0]=0;tmp1[0]=0;goto brank;}
		a=bp[po];b=bp[po+1];c=bp[po+2];d=bp[po+3];
		if(a==0xf7){po+=4;goto redis;}
		if( a!=0xfe ){
			if(po==0 || bp[po-4]>=0xfc || bp[po]==0xfc){
				strcpy(tmp1,fstr(meas,5));
			}else{
				strcpy(tmp1,"     ");
			}
			if(a<0xf0 &&(po==0||bp[po-4+1]!=0||bp[po-4]>=0xf0)){
				if(bp[po-4]==0xf7){
					int	pp=po-4;
					while(bp[pp]==0xf7){pp-=4;}
					if(bp[pp]==0x98 && bp[pp+1]==0){goto next2;}
				}
				strcat(tmp1,fstr(step,5));strcat(tmp1,":");step++;
			}else{
next2:
				strcat(tmp1,"     :");
			}
		}else{
			strcpy(tmp1,"            ");
		}

		if( a<128 ){
			if(c!=0 && d!=0){
				if(y<=0){
					strcpy(tmp0,keystr(a));
				}else{
					strcpy(tmp0,keystr1(a,skey));
				}
			}else{
				strcpy(tmp0,"   ");
			}
			strcat(tmp0,fstr(a,4));strcat(tmp0,fstr(b,6));
			strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
			if(b>0 && c>b){tmp0[19]='*';}
			if(b==0 && c>0){
				int	i0=po;
				while(i0<tr_len[track]-8){
					i0=i0+4;
					if(bp[i0]<0xf7){
						int	e=bp[i0+1];
						if(e>0){
							if(c>e){tmp0[19]='*';}
							break;
						}
					}
				}
			}
			if(b==0){cc=2;}
		}else{
			switch( a){
				case  0x90:
				case  0x91:
				case  0x92:
				case  0x93:
				case  0x94:
				case  0x95:
				case  0x96:
				case  0x97:
					strcpy(tmp0,"UserExc ");tmp0[7]=a-0x90+'0';
					strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
					break;

				case  0x98:
					strcpy(tmp0,"Tr.Exclu");
					strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));

					while(bp[po+4]==0xf7){po+=4;}
					break;

				case  0xdd:
					strcpy(tmp0,"Rol.Base");
					/*strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));*/
					break;

				case  0xde:
					strcpy(tmp0,"Rol.Para");
					/*strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));*/
					break;

				case  0xdf:
					strcpy(tmp0,"Rol.Dev#");
					/*strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));*/
					break;

				case  0xe6:
					strcpy(tmp0,"MIDI CH.");strcat(tmp0,fstr(b,5));
					strcat(tmp0,"   ");strcat(tmp0,chstr(c));
					break;

				case  0xe7:
					strcpy(tmp0,"TEMPO");strcat(tmp0,fstr(b,8));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
					break;

				case  0xea:
					strcpy(tmp0,"AFTER C.");strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));
					break;

				case  0xeb:
					strcpy(tmp0,ctrl_type(c));
					strcat(tmp0,fstr(b,5));strcat(tmp0,fstr(c&127,6));
					strcat(tmp0,fstr(d,6));
					break;

				case  0xe2:
					capi=channele_no(po);
					strcpy(tmp0,"BankProg");
/*					d|=(bank_no(po)&0xff00);
	1997-10-10    	trk.edでのbankprgの表示がbank lsbに対応して
			なかったのを修正した
*/
					if(b==0){
						strcat(tmp0,prog_name(capi,d,c,3));b=1;
					}else{
						strcat(tmp0,fstr(b,5));
						strcat(tmp0,prog_name(capi,d,c,2));
					}
					break;
				case  0xec:
					capi=bank_no(po);
					strcpy(tmp0,"PROGRAM ");

					if(b==0){
						strcat(tmp0,prog_name(d,capi,c,1));b=1;
					}else{
						strcat(tmp0,fstr(b,5));
						strcat(tmp0,prog_name(d,capi,c,0));
					}
					break;

				case  0xed:
					strcpy(tmp0,"AFTER K.");strcat(tmp0,fstr(b,5));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
					break;

				case  0xee:
					strcpy(tmp0,"PITCH");strcat(tmp0,fstr(b,8));
					strcat(tmp0,fstr((c+d*128)-8192,12));
					break;

				case  0xc0:strcpy(tmp0,"DX7FUNC");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc1:strcpy(tmp0,"DX.PARA");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc2:strcpy(tmp0,"DX.PREF");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc3:strcpy(tmp0,"TX.FUNC");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;

				case  0xc5:strcpy(tmp0,"FB-01 P");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc6:strcpy(tmp0,"FB-01 S");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc7:strcpy(tmp0,"TX81Z V");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc8:strcpy(tmp0,"TX81Z A");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xc9:strcpy(tmp0,"TX81Z P");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xca:strcpy(tmp0,"TX81Z S");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xcb:strcpy(tmp0,"TX81Z E");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xcc:strcpy(tmp0,"DX7-2 R");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xcd:strcpy(tmp0,"DX7-2 A");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xce:strcpy(tmp0,"DX7-2 P");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xcf:strcpy(tmp0,"TX802 P");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xdc:strcpy(tmp0,"MKS-7  ");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));break;
				case  0xe5:strcpy(tmp0,"KeyScan");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));break;
/*				case  0xef:strcpy(tmp0,"CMU-800");strcat(tmp0,fstr(b,6));strcat(tmp0,fstr(c,6));break;*/

				case  0xf5:strcpy(tmp0,"Music Key ");strcat(tmp0,scale_str(b));skey=b;b=1;break;

				case  0xf6:
					strcpy(tmp0," [                      ]");
					i=3;
					while(i<25){
						unsigned char a;
						a=bp[po+2];if(a==0){a=32;}tmp0[i++]=a;
						a=bp[po+3];if(a==0){a=32;}tmp0[i++]=a;
						if(bp[po+4]!=0xf7){break;}
						po+=4;
					}
					b=1;break;

				case  0xf8:
					strcpy(tmp0,"      ]");if(b==0){b=256;}
					strcat(tmp0,fstr(b,6));break;

				case  0xf9:strcpy(tmp0,"Repeat[");break;

				case  0xfc:
					strcpy(tmp0,"=========");
					strcat(tmp0,fstr(((c&3)*256+b)+1,4));
					strcat(tmp0," ===========");
					b=1;break;

				case  0xfd:
					strcpy(tmp0,"--------");
					strcat(tmp0,fstr(step_cluc(po),5));
					strcat(tmp0," -----------");
					b=1;break;

				case  0xfe:
					strcpy(tmp0,"      [End of Track]");
					break;
				default:
					strcpy(tmp0,fstr(a,7));strcat(tmp0,fstr(b,6));
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
					/* UNKNOWN SEQUENCE */
			}

			if(a>=0xdd && a<=0xdf){
				strcat(tmp0,fstr(b,5));
				if((cmdflag & 0x100)!=0){
					strcat(tmp0,"   ");strcat(tmp0,hex_s(c,2));
					strcat(tmp0,"H   ");strcat(tmp0,hex_s(d,2));
					strcat(tmp0,"H");
				}else{
					strcat(tmp0,fstr(c,6));strcat(tmp0,fstr(d,6));
				}
			}

			if( a<253 && a!=0xf6 ){cc=1;}
		}
		if(a>=0xfc || bp[po+4]==0xfc){meas++;step=1;}
		if( a<252 && b==0 ){tmp0[10]=32;tmp0[11]=32;tmp0[12]=32;}
brank:
		if(y<=0){break;}

		B_LOCATE(x,y);
		if(cc==0){strcat(tmp1,tmp0);H_PRINT(tmp1,"","");
		}else{if(cc==1){H_PRINT(tmp1,"",tmp0);}else{H_PRINT(tmp1,tmp0,"");}}

		if( line<0 ||(cplen<0 && cpadd==po) ){
			if( line<0 ){line=-line;}
			tcur(1,y,11,0);tcur(12,y,26,cc);
		}
		po=po+4;line--;y++;
	}
	cons_md(1);
	if(y<=0){strcat(tmp1,tmp0);}
	SUPER(sta);
	return(tmp1);
}

/***************************/

