/*
 * STed: edits.c (edit subroutine)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

static int	undof[4];

void	H_PRINT();
void	H_PRINT2();
int	vinput();
void	sinput();
void	spcut();
void	vdis();
void	sdis();
char	*fstr();
char	*spadd();
char	*jstrupr();
int	str_val();
void	fnc_dis();
void	msg();
void	msg_clr();
void	cons_md();
void	timtra();
void	fsel();
int	part_load();
int	part_save();
int	part_text_save();
void	dclr();
void	all_note_off();
int	ctc();
int	scale_no();
char	*keystr();
void	dplay();
void	cdplay();
void	cntplay();
void	trk_free();
void	trksize();
void	trk_no();
void	snsclr();
int	add_set();
void	twait();
int	str_search();
void	bunkatsu();
void	txerase();

int	trk_check();
int	trk_shift();
void	trk_delete();
int	same_ext_sub();
int	same_meas_conv();
int	size_max();
int	size_change();
int	size_add();
void	size_ref();

void	gra();
void	gra_c();
char	*trk_dis();


void	poplay();
void	partf();
int	replace();
int	rep_match();
void	sttrc();
int	find();
int	find_rep();
void	trk_cluc();
int	buf_meas();
int	step_cluc();
int	step_cluc2();
int	real_ad();
int	real_po();
int	real_cy();
int	step_no();
int	step_ad();
int	sttm_no();
int	sttm_ad();
int	padjust();
int	meas_no();
int	meas_no2();
int	meas_add();
int	dat_add();
int	meas_len();
void	undobuf();
void	same_shift();
int	same_meas_extr();
int	cpybuf_set();
void	dat_rev();
int	replicate();
void	bendc();
int	bendc2();
int	bank_no();
int	prog_no();
int	channele_no();
void	trk_next();
int	spc_code();
char	*ctrl_type();

/***************************/
/*
 *	=0 stop
 *	=1 all track point play
 *	=2 all track point play(manual)
 *	=3 single track meas play
 *	=4 single track point play
 *	=5 single track point play(manual)
 *	=6 sound edit one note
 *	=7 sound edit one chord
 *	=8 tr.exclu
 */

void	poplay(int po,int md)
{
	int	a1,a2;

	trk_cluc();

	if(md==2 || md==5){
		int	j;
		msg("START MEAS=");j=vinput(0,4);msg_clr();
		if(es!=0 || j<1){return;}
		po=meas_add(j);if( po<0 ){return;}
	}

	switch(md){
		case 0:all_note_off(2);break;

		case 1:
		case 2:cntplay(track,po);break;

		case 3:a1=po;while(a1>0 && trk[track][a1-4]<0xfc){a1=a1-4;}
			a2=po;
			while(a2<tr_len[track]-4){
				a2=a2+4;if( trk[track][a2]==0xfc ){a2=a2+4;break;}
				if( trk[track][a2]>0xfb ){break;}
			}
			cdplay(track,a1,a2-a1,0);break;

		case 4:
		case 5:a1=po;while(a1>0 && trk[track][a1-4]<0xfc){a1=a1-4;}
			a2=tr_len[track]-4;cdplay(track,a1,a2-a1,0);break;

		case 6:cdplay(track,po,4,0);break;

		case 7:a1=po;
			while(a1>0 && trk[track][a1-4]<=127 && trk[track][a1-3]==0){a1=a1-4;}
			cdplay(track,a1,(po-a1)+4,0);break;

		case 8:a1=po;
			if(trk[track][po]==0x98){
				while(trk[track][a1+4]==0xf7){a1=a1+4;}
				cdplay(track,po,(a1-po)+4,1);break;
			}
	}
	snsclr();
}

/***************************/
void	partf(int a,int ad)
{
	char	path2[128],tmp0[256];
	int	ex;
	int	e=edit_scr;

	cons_md(0);edit_scr=0;

	if( a==0 ){
		strcpy(path2,prt_path);fsel(prt_file,path2,0x006+e*0x200);
		if( es==0 && prt_file[0]!=0 ){
			strcpy(tmp0,path2);strcat(tmp0,prt_file);
			msg("パートデータのロード中です。");
			ex=part_load(tmp0);
			if(ex>0){
				if(trk_check(ex)==0){
					int	i;
					same_shift(ad,0,buf_meas(dat,ex));
					trk_shift(ad,ex);part_load(tmp0);
					for(i=0;i<ex;i++){trk[track][i+ad]=dat[i];}
					if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
					msg_clr();
				}
			}
		}
	}else{
		if( cplen<0 && cpadd!=ad){
			if(a==1){
				strcpy(path2,prt_path);fsel(prt_file,path2,0x106+e*0x200);
				if( es==0 && prt_file[0]!=0 ){
					strcpy(tmp0,path2);strcat(tmp0,prt_file);
					dclr();
					cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
					ex=same_ext_sub(track,cpadd,cpadd+cplen,0);

					msg("パートデータのセーブ中です。");
					if(part_save(tmp0,ex)==0){
						if(!(cnfflag&0x0008)){strcpy(prt_path,path2);}
						msg_clr();}
				}
			}else{
				char	part_file[128];
				part_file[0]=0;

				strcpy(path2,rcp_path);fsel(part_file,path2,0x107+e*0x200);
				if( part_file[0]!=0 && str_search(part_file,".")==0 ){
					strmfe(part_file,part_file,"TXT");}
				if( es==0 && part_file[0]!=0 ){
					strcpy(tmp0,path2);strcat(tmp0,part_file);
					cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}

					msg("テキスト形式で書き出し中です。");
					if(part_text_save(tmp0,cpadd,cplen)==0){msg_clr();}
				}
			}
		}
	}
	edit_scr=e;
	trk_cluc();
	fnc_dis(0);cons_md(1);
}

/***************************/
void	sttrc(int *po,int *cy,int flg)
{
	int	ad,rad,i,j,ii,jj,jjj=-1,skf,ofs,nn,md=8;
	int	me_ad[2048];
	int	max;
	int	lpad[18],lprt[18];
	unsigned char lpco[18];
	unsigned char	*ptr;

	ptr=(unsigned char *)trk[track];
	max=add_set(track,me_ad);

	skf=poft>>16;ofs=poft&0xffff;
	while( ( (B_SFTSNS()&16)!=0 || flg!=0 ) && rcd->active[track] && rcd->act!=0){
		if(B_KEYSNS()!=0){
			int	k=B_KEYINP()>>8,sh=B_SFTSNS();
			if(k==1){break;}
			if((sh&15)==0){
				if(k==0x57){rcd->sts=0;}
				if(k==0x58){rcd->sts=1;}
				if(k==0x59){all_note_off(2);break;}
			}
			if( k==0x3b || k==0x3d){
				int	i;
				trksize(track);i=meas_no(real_ad(*po,*cy));
				if( k==0x3b){trk_next(2);}else{trk_next(3);}
				*po=meas_add(i);if(*po<0){*po=tr_len[track]-4;}
				trk_no();trk_cluc();cplen=0;*cy=padjust(po,8);
				size_max(track);
				ptr=(unsigned char *)trk[track];
				max=add_set(track,me_ad);
				snsclr();
			}
		}

		ad=meas_add(ofs+1);if(ad<0){break;}
		nn=0;rad=0;

		j=rcd->bar[track];jj=j;i=rcd->step[track];ii=i;
		if(skf!=0){if(j==0){i=-1;}else{j--;}}

		while(j>0 || i>0){
			unsigned char	ca;
loop1:			ca=ptr[ad];ad+=4;if(ca==0xf7){goto loop1;}
			if(ca>0xfd ){ad-=4;break;}

			if((ca>=0xfc || ptr[ad]==0xfc)&& rad!=0){ad=rad;rad=0;
				if(j>0){j--;if(j==0 && i==0){break;}}
				goto loop1;
			}

			if( ca==0xfc ){
				int	ii0,jj0,ct;
				jj0=ad-4;
resame:
				ct=ptr[jj0+1]+(ptr[jj0+2]&3)*256;ii0=me_ad[ct];
				if(ct<max && jj0!=ii0){
					jj0=ii0;if(ptr[ii0]==0xfc){goto resame;}
					rad=ad;ad=ii0;
				}
				goto loop1;
			}

			if(j>0){
				if(ca>=0xfc || ptr[ad]==0xfc){
					j--;if(j==0 && i==0){break;}
				}
			}else{
				if(ca<0xf8){i--;if(i==0){ad-=4;break;}}
			}

			if(ca==0xf9 && nn<16){
				nn++;lpad[nn]=ad;lprt[nn]=rad;lpco[nn]=0;
			}
			if(ca==0xf8 && nn>0){
				lpco[nn]+=1;
				if(lpco[nn]!=ptr[ad+1-4]){ad=lpad[nn];rad=lprt[nn];goto loop1;}
				nn--;
			}

		}
		if(rad!=0){ad=rad-4;}

		if( ad>=*po && ad<real_ad(*po,12+(flg*12)) ){
			*cy=real_cy(*po,ad);
		}else{
			if( ad<*po || ad>real_ad(*po,30) || flg!=0){
				*cy=0;*po=ad;B_CLR_AL();trk_dis(6,ad,24);
			}else{
				*cy=12;
				while( real_ad(*po,13)<tr_len[track] && real_ad(*po,12)<ad){
					TXRASCPY((7*4)*256+(6*4),24*4,3);
loop3:					*po=*po+4;
					if(ptr[*po]==0xf7){goto loop3;}
					trk_dis(29,real_ad(*po,23),1);
				}
			}
		}

		if(flg==0){
			int ad2;ad2=real_ad(*po,*cy);
			trk_dis(6+*cy,ad2,-1);

			if(cy && ptr[ad2]<0xf0){
				int pp=*po,yy=*cy,ad3;
				while(yy){yy--;ad3=real_ad(pp,yy);
					if(ptr[ad3]>=0xf0 || ptr[ad3+1]!=0 ){break;}
					trk_dis(6+yy,ad3,-1);}
			}

			if(jj!=jjj){jjj=jj;if(grpmode!=0){gra(ad,md);md|=16;}}

			if(grpmode!=0){gra_c(ad);}
			while(jj==rcd->bar[track] && (ii==rcd->step[track] ||i<0)){
				if(B_KEYSNS()!=0){break;}
				if( !(rcd->active[track]) ){break;}
			}
			if(grpmode){txerase(392,589-512,767-327,1001-589);}
			trk_dis(6+*cy,ad2,1);

			if(cy && ptr[ad2]<0xf0){
				int pp=*po,yy=*cy,ad3;
				while(yy){yy--;ad3=real_ad(pp,yy);
					if(ptr[ad3]>=0xf0 || ptr[ad3+1]!=0 ){break;}
					trk_dis(6+yy,ad3,1);}
			}

		}
		flg=0;
	}

	LEDMOD(0,0);snsclr();
}

/***************************/
int	replace(int po,int ln,int sh,int v)
{
	int	i,j,cl,ct,ctl,cd,nm,a,b,c,d,ln2;
	int	trg[8],trg2[8],ftg[8],ftg2[8];
	double	ml;
	char	rep[8][256];
	char	li[256],li2[256],tmp0[256],tmp1[256];
	char	si,rcvf,addf;
	unsigned char	*ptr=trk[track];

	if(ln<=0){return 1;}

	if(sh==0){strcpy(li2,repl);}else{if(sh==1){strcpy(li2,delt);}else{strcpy(li2,srch);}}

	while( (i=str_search(li2,"$<")) ){strcpy(&li2[i-1],&li2[i]);}
top:
	if((c=str_search(li2,"|"))==0){
		strcpy(li,li2);li2[0]=0;
	}else{
		strcpy(li,li2);li[c-1]=0;strcpy(li2,&li2[c]);
	}

/*if(li[0]==0){goto top;}*/

	rcvf=0;addf=0;
	memcpy_l(dat2,ptr,tr_len[track]);

	for(i=0;i<TRACK_SIZE;i++){dat[i]=0;}
	for(i=0;i<=7;i++){rep[i][0]=0;trg[i]=0;trg2[i]=0;ftg[i]=-1;ftg2[i]=0;}


	strcat(li,",");nm=0;tmp1[0]=0;
	if(li[0]=='@'){strcpy(li,&li[1]);rcvf=1;}
	if(li[0]=='%'){strcpy(li,&li[1]);addf=1;}
	while( nm<4 ){
		c=str_search(li,",");if( c==0 ){break;}
		strcpy(tmp0,li);tmp0[c-1]=0;strcpy(li,&li[c]);

		if(tmp0[0]=='/' && nm==0){strcpy(tmp1,tmp0);jstrupr(tmp1);}
		if(tmp0[0]==':'||tmp0[0]==0){ftg[nm]=-1;}

		d=str_search(tmp0,":");if(d>0){strcpy(rep[nm],&tmp0[d]);tmp0[d-1]=0;}

		if( tmp0[0]=='!'){ftg2[nm]|=1;strcpy(tmp0,&tmp0[1]);}
		if( tmp0[0]=='&'){ftg2[2]|=2;ftg[2]=99;strcpy(tmp0,&tmp0[1]);}
		if( tmp0[0]=='+'){ftg2[1]|=4;strcpy(tmp0,&tmp0[1]);}
		if( tmp0[0]=='-'){ftg2[1]|=8;strcpy(tmp0,&tmp0[1]);}

		if( tmp0[0]!=0){
			ftg[nm]=0;

			if(str_search(tmp0,">")>0){ftg[nm]=-2;}
			if(str_search(tmp0,"<")>0){ftg[nm]=-3;}
			if(str_search(tmp0,"_")>0){
				int a;
				ftg[nm]=-7;
				a=str_search(tmp0,"_");
				if(a){
					if(tmp0[a]>31){
						if(nm>0){
							trg2[nm]=str_val(&tmp0[a]);
						}else{
							trg2[nm]=ctc(256,&tmp0[a],0);
						}
					}else{trg2[nm]=9999;}

					tmp0[a-1]=0;
					if(tmp0[0]>31){
						if(nm>0){
							trg[nm]=str_val(tmp0);
						}else{
							trg[nm]=ctc(256,tmp0,0);
						}
					}else{trg[nm]=-9999;}

					if(trg[nm]>trg2[nm]){
						int a;
						a=trg[nm];trg[nm]=trg2[nm];trg2[nm]=a;
					}
				}
				goto skip0;
			}

			if(nm>0){
				trg[nm]=str_val(tmp0);
			}else{
				if(tmp0[1]=='*'){ftg[nm]=-4;tmp0[1]='0';}
				if(tmp0[2]=='*'){ftg[nm]=-4;tmp0[2]='0';}
				trg[nm]=ctc(256,tmp0,0);
				if(trg[nm]<0){msg("指定に間違いが有ります。");return -1;}
			}
		}
skip0:
		nm++;
	}

	if(tmp1[0]=='/'){
		ct=0;ctl=-1;
		if(tmp1[1]!=0 ){ct=spc_code(&tmp1[1],&ctl);}
		if(tmp1[1]=='C' && tmp1[2]=='M'){ct=0xf6;}

		if(tmp1[1]=='S'){ftg[0]=-5;ct=0;ctl=-1;goto skip;}
		if(tmp1[1]=='A' && tmp1[2]=='L' && tmp1[3]=='L'){
			ct=0;ctl=-1;if(rep[2][0]==0 && rep[3][0]==0){ftg[0]=-6;goto skip;}
		}

		if(ct==0){msg("指定に間違いが有ります。");return -1;}
		trg[0]=ct;ftg[0]=0;if(ctl>=0){trg[2]=ctl;ftg[2]=0;rep[2][0]=0;}


		if(str_search(tmp1,"_")>0 && (ct<0x98 ||ct==0xeb)){
			int a,nn=0;
			if(ct==0xeb){nn=2;}
			ftg[nn]=-7;
			a=str_search(tmp1,"_");
			if(a){
				if(tmp0[a]>31){
					trg2[nn]=str_val(&tmp0[a]);
				}else{trg2[nn]=9999;}

				if(ct<0x98){
					if(trg2[nn]>7){trg2[nn]=7;}
					trg2[nn]=0x90+trg2[nn];
					ftg[nn]=-8;
				}
				if(ct==0xeb){
					if(trg2[nn]>127){trg2[nn]=127;}
					ftg[nn]=-7;
				}
/*
				if(trg[nn]>trg2[nn]){
					int a;
					a=trg[nn];trg[nn]=trg2[nn];trg2[nn]=a;
				}
*/
			}
		}


		if(rep[0][0]=='/' && (ct==0xeb || (ct>127 && ct<0x98)||ct==0xed||ct==0xea)){
			ct=0;ctl=-1;
			if(rep[0][1]!=0 ){ct=spc_code(&rep[0][1],&ctl);}

			if((ct>127 && ct<0x98)||ct==0xeb||ct==0xed||ct==0xea){
				strcpy(rep[0],fstr(ct,3));
				if(ctl>=0){strcpy(rep[2],fstr(ctl,3));}

				if(trg[0]==0xea && ct!=0xea){
					strcpy(rep[3],"@1");if(ctl<0){strcpy(rep[2],"0");}}

				if(trg[0]!=0xea && ct==0xea){
					strcpy(rep[2],"@2");strcpy(rep[3],"0");}

				goto skip2;
			}else{msg("指定に間違いが有ります。");return -1;}
		}
skip:
		rep[0][0]=0;
	}
skip2:
	if(trg[0]==0xec || trg[0]==0xee){
		if( ftg[2]==-1){
			trg[2]=trg[3];trg2[2]=trg2[3];
			ftg[2]=ftg[3];ftg[3]=-1;
			ftg2[2]=ftg2[3];ftg2[3]=0;
		}
		if( rep[2][0]==0){strcpy(rep[2],rep[3]);rep[3][0]=0;}
		if(trg[0]==0xec ){ftg[3]=-1;ftg2[3]=0;}
		rep[4][0]=0;
	}

	if((trg[0]==0xec || trg[0]==0xe2)&& ftg[2]!=-1 && (cmdflag&0x20)!=0){trg[2]--;}

	if(trg[0]>252){msg("指定に間違いが有ります。");return -1;}
	undobuf(0);
	if(sh==0){msg("置換中...");}else{if(sh==1){msg("削除中...");}else{msg("検索中...");}}

	i=po;j=0;ln2=0;
	while(1){
		a=ptr[i];
		if( rep_match(ftg[0],ftg2[0],trg[0],trg2[0],a,0)!=0 ){
			b=ptr[i+1];c=ptr[i+2];d=ptr[i+3];
			if(a==0xee){c=d*128+c-8192;d=0;}

			if(trg[0]<128 && trg[1]>0 && a<128 && b==0){
				int aa=i;
				while(1){
					aa+=4;
					if(ptr[aa]>127){break;}
					if(ptr[aa+1]>0){b=ptr[aa+1];break;}
				}
			}

if((ftg2[1]&4)!=0){b=chd_check(track,i,0);}
if((ftg2[1]&8)!=0){b=chd_check(track,i,1);}

			if( rep_match(ftg[1],ftg2[1],trg[1],trg2[1],b,1)!=0\
			 && rep_match(ftg[2],ftg2[2],trg[2],trg2[2],c,2)!=0\
			 && rep_match(ftg[3],ftg2[3],trg[3],trg2[3],d,3)!=0 ){

if((ftg2[2]&2)!=0){
	int mm=tai_check(track,i);
	if((ftg2[2]&1)==0 && mm==0){goto next;}
	if((ftg2[2]&1)!=0 && mm==1){goto next;}
}

				if(sh==0){
					for(j=0;j<=3;j++){
						if(j>0 && rep[j][0]=='@'){
							cd=0;
							if(rep[j][1]=='0'){cd=b;}
							if(rep[j][1]=='1'){cd=c;}
							if(rep[j][1]=='2'){cd=d;}
							goto skip3;
						}

						if(j==1 && trg[0]<128 && trg[1]>0 &&\
						   a<128 && ptr[i+1]==0){j++;}

						strcpy(tmp0,rep[j]);
						if(a==0xee &&j>1){
							cd=(ptr[i+2]+ptr[i+3]*128)-8192;
						}else{cd=ptr[i+j];}

						while(tmp0[0]!=0){
							si=tmp0[0];
							if(si=='+'||si=='-'||si=='*'||si=='/'||si=='%'){
								strcpy(tmp0,&tmp0[1]);
								if(tmp0[0]==0){break;}
								ml=val(tmp0);


	while(tmp0[0]>=0x30 && tmp0[0]<=0x39){
		strcpy(tmp0,&tmp0[1]);
	}
	if(tmp0[0]=='_'){
		strcpy(tmp0,&tmp0[1]);
		ml=(rand()*(val(tmp0)-ml+1)/32767)+ml;
	}


								if(si=='+'){cd=cd+ml;}
								if(si=='-'){cd=cd-ml;}
								if(si=='*'){cd=cd*ml;}
								if(si=='/'){cd=cd/ml;}
								if(si=='%'){
									if(ml>300){ml=300;}
									if(ml<=0){ml=1;}
									cd=cd*(ml/100);
								}

								cl=255;
								for(ct=1;ct<strlen(tmp0);ct++){
									si=tmp0[ct];
									if(si=='+'||si=='-'||si=='*'||si=='/'||si=='%'){cl=ct+1;break;}
								}
								if(cl!=255){
									strcpy(tmp0,&tmp0[cl-1]);
								}else{
									tmp0[0]=0;
								}
							}else{
								if(tmp0[0]=='^'){strcpy(tmp0,&tmp0[1]);}
								if(j==0 && a<128){
									ct=ctc(60,tmp0,0);
									if(ct>=0){cd=ct;}
								}else{
									cd=str_val(tmp0);
									if((a==0xec || a==0xe2)&& j==2 && (cmdflag&0x20)!=0){cd--;}
								}
								tmp0[0]=0;
							}
						}
skip3:

if(a<0xf0 && j==1){dat[i+1]=cd>>8;cd=cd&255;}

						if( a<128 ){
							if(cd<0){cd=0;}
							if(j==0 || j==3){
								if(cd>127){cd=127;}
							}else{
								if(cd>255){cd=255;}
							}
						}else{
							if( a==0xee ){
								if( j>1 ){
									if(cd<-8192){cd=-8192;}
									if(cd>8191){cd=8191;}
								}else{
									if(cd<0){cd=0;}
									if(cd>255){cd=255;}
								}
							}else{
								if(cd<0){cd=0;}
								if(j>1 && a!=0xf6 && a!=0x98 && a!=0xf7 && a!=0xe7){
									if(cd>127){cd=127;}
								}else{
									if(cd>255){cd=255;}
								}
							}
						}
						if(a==0xee &&j>1){
							cd=cd+8192;ptr[i+2]=cd&0x7f;ptr[i+3]=cd>>7;
						}else{ptr[i+j]=cd;}
					}
				}
				dat[i]=1;
				if( sh==2 ){msg_clr();return i;}
			}
		}
next:

		if(INPOUT(0xff)==27 ){msg("中止しました。");return i;}

		if( v==0 ){
			i=i+4;if(i>=po+ln){break;}
		}else{
			i=i-4;if(i<0){break;}
		}
	}
	if( sh==2 ){msg_clr();return -1;}

	if(sh==1){
		int i,a=0;
		for(i=0;i<tr_len[track];i=i+4){

			if(i>=po+ln && ln2==0){ln2=a-po;}

			if(dat[i]==0){
				ptr[a++]=ptr[i];ptr[a++]=ptr[i+1];
				ptr[a++]=ptr[i+2];ptr[a++]=ptr[i+3];

				if(ptr[i]!=0x98 && ptr[i]!=0xf6){
					while(ptr[i+4]==0xf7){i+=4;}
				}else{
					while(ptr[i+4]==0xf7){i+=4;
						ptr[a++]=ptr[i];ptr[a++]=ptr[i+1];
						ptr[a++]=ptr[i+2];ptr[a++]=ptr[i+3];
					}
				}
			}else{
				if(rcvf && ptr[i]<0xf0 && ptr[i+1]!=0){
					ptr[a++]=0;ptr[a++]=ptr[i+1];
					ptr[a++]=0;ptr[a++]=0;
				}
				if(ptr[i]==0x98||ptr[i]==0xf6){
					while(ptr[i+4]==0xf7){i+=4;}
				}
			}
		}
		tr_len[track]=a;
	}
	if(sh==0){
		int i,a=0,ii,aa;
		cpleng=0;

		for(i=0;i<tr_len[track];i=i+4){

			if(i>=po+ln && ln2==0){ln2=a-po;}

			if(addf!=0 && dat[i]!=0 && dat2[i+4]!=0xf7 ){

				if(a>=TRACK_SIZE){msg("トラック容量不足です。");
					memcpy_l(ptr,dat2,tr_len[track]);goto ext;}

				cpbuf[a++]=dat2[i];cpbuf[a++]=0/*dat2[i+1]*/;
				cpbuf[a++]=dat2[i+2];cpbuf[a++]=dat2[i+3];
			}
			ii=i;aa=a;
loop1:
			if(a>=TRACK_SIZE){msg("トラック容量不足です。");
				memcpy_l(ptr,dat2,tr_len[track]);goto ext;}

			cpbuf[a++]=ptr[i];cpbuf[a++]=ptr[i+1];
			cpbuf[a++]=ptr[i+2];cpbuf[a++]=ptr[i+3];

			if(ptr[i+4]==0xf7){i+=4;goto loop1;}

			if( dat[ii+1] ){
				int ss=dat[ii+1]*256+ptr[ii+1];

				if(cpbuf[aa]<0xf0){cpbuf[aa+1]=240;ss-=240;}

				while(ss>0){
					if(a>=TRACK_SIZE){msg("トラック容量不足です。");
						memcpy_l(ptr,dat2,tr_len[track]);goto ext;}

					if(ss>240){
						cpbuf[a++]=0;cpbuf[a++]=240;
						cpbuf[a++]=0;cpbuf[a++]=0;
						ss-=240;
					}else{
						cpbuf[a++]=0;cpbuf[a++]=ss;
						cpbuf[a++]=0;cpbuf[a++]=0;
						ss=0;
					}
				}
			}
		}

		if(size_change(track,a)){goto ext;}

		tr_len[track]=a;
		memcpy_l(ptr,cpbuf,tr_len[track]);

	}
	if(ln2){ln=ln2;}
	if(li2[0]!=0){goto top;}

ext:
	msg_clr();

	snsclr();
	return i;
}

/***************************/
int	rep_match(int f,int f2,int t,int t2,int a,int m)
{
if((f2&1)==0){
	if( f== 0 && ( a==t				)){return 1;}	/* 合同		*/
	if( f==-1 && ((m==0 && a<128) || m!=0		)){return 1;}	/* all note	*/
	if( f==-2 && ( a<t				)){return 1;}	/* <		*/
	if( f==-3 && ( a>t && ((m==0 && a<128)||m!=0)	)){return 1;}	/* >		*/
	if( f==-4 && ((a%12)==(t%12) && a<128		)){return 1;}	/* note*	*/
	if( f==-5 && ( a>127 && (a<0xf0 || a==0xf7)     )){return 1;}	/* all spc	*/

	if( f==-6 && ( a<0xfc && (a<0xf6 || a>0xf9)	)){return 1;}	/* all data	*/
/*	if( f==-7 && ( a>=t	&& a<=t2		)){return 1;}*/	/* 範囲		*/

	if(m==0){
		if( f==-7 && (a>=t && a<=t2	)&& a<128){return 1;}	/* 範囲	*/
	}else{
		if( f==-7 && (a>=t && a<=t2	)){return 1;}
	}

	if( f==-8 && (a>=t && a<=t2)&& a>=0x90 && a<0x98){return 1;}	/* 範囲(usrexc)*/

}else{
	if( f== 0 && a!=t){
		if(m!=0 ||(m==0 && t<128 && a<128)	){return 1;}	/* !合同	*/
		if(m!=0 ||(m==0 && t>127 && t<0xf0 && a>127 && a<0xf0)){return 1;}
	}
	if( f==-1 && ( a>127 && a<0xfc &&(a<0xf6 || a>0xf9))){return 1;}/* !all note	*/
	if( f==-2 && ( a>=t &&((m==0 && a<128)||m!=0)	)){return 1;}	/* !<		*/
	if( f==-3 && ( a<=t				)){return 1;}	/* !>		*/
	if( f==-4 && (!((a%12)==(t%12)) && a<128	)){return 1;}	/* !note*	*/
	if( f==-5 && ( a<128				)){return 1;}	/* !all spc	*/

	if(m==0){
		if( f==-7 && !(a>=t && a<=t2	)&& a<128){return 1;}	/* !範囲*/
	}else{
		if( f==-7 && !(a>=t && a<=t2	)){return 1;}
	}

	if( f==-8 && !(a>=t && a<=t2)&& a>=0x90 && a<0x98){return 1;}	/* !範囲(usrexc)*/

}
	if( f==99 ){return 1;}
	return(0);
}
/***************************/
int	tai_check(int tr,int ad)
{
	int	ln;
	unsigned char	a,b,c,d;

	unsigned char	*ptr;
	ptr=(unsigned char *)trk[tr];

	ln=tr_len[tr];
	a=ptr[ad];b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];

	if(a<128 && c>b && d>0){
		int ad2=ad,sum=0;

		while(ad2<ln){
			unsigned char	da=ptr[ad2];

			if(ad2>ad && a==da && ptr[ad2+2]!=0 && ptr[ad2+3]!=0 ){
				if(sum<c){return 1;}else{return 0;}
			}
			if(da<0xf0){sum+=ptr[ad2+1];if(sum>=c){return 0;}}
			if(da==0xfc||da==0xfe){return 1;}
			ad2+=4;
		}
	}
	return(0);
}

/***************************/
int	chd_check(int tr,int ad,int md)
{
	int	ln,sum=1;
	unsigned char	a,b,c,d;

	unsigned char	*ptr;
	ptr=(unsigned char *)trk[tr];

	ln=tr_len[tr];
	if(md!=0){
		while(ad<=ln){
			a=ptr[ad];b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];
			if(a>=0xf0){break;}
			if(a<128 && c>0 && d>0){sum++;}
			if(b>0){break;}
			ad+=4;
		}
	}else{
		while(ad>0){
			ad-=4;
			a=ptr[ad];b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];
			if(a>=0xf0){break;}
			if(b>0){break;}
			if(a<128 && c>0 && d>0){sum++;}
		}
	}
	return(sum);
}

/***************************/
int	find(int po,int h)
{
	int	i,j,ct,mo=0,im=0;
	char	tmp0[256],tmp1[256];

	strcpy(tmp0,srch);

	if( (i=str_search(tmp0,"$<"))){
		strcpy(&tmp0[i-1],&tmp0[i]);
	}

	jstrupr(tmp0);
	if(srch[0]!=0){mo=1;}

	if(tmp0[0]=='$'||tmp0[0]==0){
		if(tmp0[0]!=0){strcpy(tmp0,&tmp0[1]);strcpy(tmp1,&srch[1]);}

		if(tmp0[0]=='S'&&tmp0[1]=='T'&&tmp0[2]=='!'&&tmp0[3]=='='){
			mo=2;if(tmp0[4]!=0){im=str_val(&tmp0[4]);}else{
			im = tbase * 4 * beat1 / beat2;}
		}
		if(mo==0){
			msg("SPCIAL CONTROLER");
		}else{
			if(mo==1){msg("指定文字列");}else{msg("指定値外 TOTAL ST");}
		}

		B_PRINT(" 検索中...");
		i=po;j=-1;h=h&1;
		while(1){
			if(INPOUT(0xff)==27 ){msg("中止しました。");return(i);snsclr();}
			if( h==0 ){
next:
				i=i+4;if(i>=tr_len[track]){break;}
				if(trk[track][i]==0xf7){goto next;}
			}else{
back:
				i=i-4;if(i<0){break;}
				if(trk[track][i]==0xf7){goto back;}
			}
			if(mo==0){
				ct=trk[track][i];if(ct>127 && ct<252){j=i;break;}
			}else{
				if(mo==1){
					if(str_search(trk_dis(-1,i,1),tmp1)>0){j=i;break;}
				}else{
					if(trk[track][i]==0xfd){
						if(step_cluc(i)!=im){j=i;break;}
					}
				}
			}
		}
	}else{
		j=po;
		if((h&1)==0){
			po=po+4;if(po<tr_len[track]){j=replace(po,tr_len[track]-po,2,0);}
		}else{
			po=po-4;if(po>=0){j=replace(po,po,2,1);}
		}
	}

	msg_clr();if(j<0){msg("見付かりません");}
	snsclr();return(j);

}

/***************************/
int	find_rep(int po)
{
	int	i,j,ct,flg,h;

	i=po;j=-1;h=-1;flg=1;

	if(trk[track][i]==0xf9){h=0;}/*[*/
	if(trk[track][i]==0xf8){h=1;}/*]*/
	if(h<0){snsclr();return(j);}

	msg("対REPEAT 検索中...");

	while(1){
		if(INPOUT(0xff)==27 ){msg("中止しました。");return(i);snsclr();}
		if( h==0 ){
next:
			i=i+4;if(i>=tr_len[track]){break;}
			if(trk[track][i]==0xf7){goto next;}

			if(trk[track][i]==0xf9){flg+=1;}/*[*/
			if(trk[track][i]==0xf8){flg-=1;}/*]*/
		}else{
back:
			i=i-4;if(i<0){break;}
			if(trk[track][i]==0xf7){goto back;}

			if(trk[track][i]==0xf9){flg-=1;}/*[*/
			if(trk[track][i]==0xf8){flg+=1;}/*]*/
		}

		ct=trk[track][i];if(flg==0){j=i;break;}
	}

	msg_clr();if(j<0){msg("見付かりません");}
	snsclr();return(j);
}

/***************************/
void	trk_cluc(void)
{
	int	i,ch,meas=1,a,ln;
	unsigned char	*ptr=trk[track];

	ln=tr_len[track];ch=mch[track];
	for(i=0;i<ln;i=i+4){
		a=ptr[i];
		if(a>0xe5){
			if( a==0xe6 || a==0xe2 ){ch=ptr[i+2];}
			if( a==0xec ){ptr[i+3]=ch;}
			if( a==0xfc){
				if(i!=0){if(ptr[i-4]<0xfc){meas++;}}
				meas++;
			}
			if( a==0xfd ){ptr[i+2]=meas>>8;ptr[i+3]=meas & 0xff;meas++;}
			if( a==0xfe ){tr_len[track]=i+4;}
		}
	}

	ptr[ln-4]=0xfe;
}

/***************************/
int	buf_meas(unsigned char *pp,int ln)
{
	int	i,co=0;
	for(i=0 ;i<ln;i=i+4){
		if( pp[i]>=0xfc || (i+4<ln && pp[i+4]==0xfc) ){co++;}
	}
	return(co);
}

/***************************/
int	step_cluc(int ad)
{
	int	lc[16],lp[16],lx[16];
	int	a=0,b=1,i,pp=0,ct,tr;
	unsigned char	c,d;
	unsigned char	*ptr;
	tr=track;

	ptr=(unsigned char *)trk[tr];

	i=ad;
	while(i>0){
		i=i-4;
		c=ptr[i];
		if( c<0xf0 ){
			d=ptr[i+1];
			if(d){if(b==1){a=a+d;}else{a=a+d*b;}}
		}else{
			if( c>0xfb ){break;}
			if( c==0xf8 ){
				if(pp<16){
					ct=ptr[i+1];if(ct==0||ct==255){ct=1;}
					lc[pp]=b;lp[pp]=a;lx[pp]=ct;b=b*ct;pp++;
				}
			}
			if( c==0xf9 ){
				if(pp>0){pp--;b=lc[pp];}
			}
		}
	}
	for(i=pp-1;i>=0;i--){a=lp[i]+((a-lp[i])/lx[i]);}
	return(a);
}

/***************************/
int	step_cluc2(int tr,int top,int len)
{
	int	lx[16],me_ad[2048];
	int	i,pp,a,ct,max,retad=0;
	unsigned char	c;
	unsigned char	*ptr;

	ptr=(unsigned char *)trk[tr];
	max=add_set(tr,me_ad);

	pp=0;a=0;i=top;
	while(i<top+len){
top:
		c=ptr[i];
		if( c<0xf0 ){
			a+=ptr[i+1];
		}else{
			if( c==0xf9 ){
				if(pp<16){
					lx[pp]=a;a=0;pp++;
				}
			}
			if( c==0xf8 ){
				if(pp>0){pp--;
					ct=ptr[i+1];if(ct==0||ct==255){ct=1;}
					a=lx[pp]+(a*ct);
				}
			}
			if( c>=0xfc && retad!=0){i=retad;retad=0;goto top;}
			if( c==0xfc ){
				int	ii,jj,ct;
				jj=i;
resame:
				ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct];

				if(ct<max && jj!=ii){
					jj=ii;if(ptr[ii]==0xfc){goto resame;}
					retad=i+4;i=ii;goto top;
				}
			}
		}
		i+=4;
	}

	for(i=0;i<pp;i++){a+=lx[i];}
	return(a);
}

/***************************/
int	real_ad(int po,int cy)
{
	while(cy>0){
loop:		po=po+4;if(trk[track][po]==0xf7){goto loop;}
		cy=cy-1;
	}
	return(po);
}

int	real_po(int po,int cy)
{
	while(cy>0){
loop:		po=po-4;if(trk[track][po]==0xf7){goto loop;}
		cy=cy-1;
	}
	return(po);
}

int	real_cy(int po,int ad)
{
	int	cy=0;
	while(ad>po){
loop:		ad=ad-4;if(trk[track][ad]==0xf7){goto loop;}
		cy=cy+1;
	}
	return(cy);
}

/***************************/
int	step_no(int ad)
{
	int	po;
	int	step=0+1;
	unsigned char	*bp=trk[track];

	po=ad;
	while(po>0){
		unsigned char	e;
		po=po-4;e=bp[po];

		if(e<0xf0 &&(po==0||bp[po-4+1]!=0||bp[po-4]>=0xf0)){
			if(bp[po-4]==0xf7){
				while(bp[po-4]==0xf7){po-=4;}
				if(bp[po-4]==0x98 && bp[po-4+1]==0){goto next1;}
			}
			step++;
		}
next1:
		if( e>=0xfc ){break;}
	}
	return(step);
}

int	step_ad(int ad,int step)
{
	int	po;
	unsigned char	*bp=trk[track];

	po=ad;
	while(step>0){
		unsigned char	a;
redis:
		a=bp[po];
		if(a==0xf7){po+=4;goto redis;}
		if(a>=0xfc ){break;}

		if(a<0xf0 &&(po==0||bp[po-4+1]!=0||bp[po-4]>=0xf0)){
			if(bp[po-4]==0xf7){
				int	pp=po-4;
				while(bp[pp]==0xf7){pp-=4;}
				if(bp[pp]==0x98 && bp[pp+1]==0){goto next2;}
			}
			step--;if(step==0){break;}
		}
next2:
		po=po+4;
	}
	return(po);
}

int	sttm_no(int ad)
{
	return step_cluc(ad);
}

int	sttm_ad(int ad,int step)
{
	int	po;
	unsigned char	*bp=trk[track];

	if(ad<0){return ad;}

	po=ad;
	while(1){
		unsigned char	a;
		if(step_cluc(po)>=step){break;}
redis:
		a=bp[po];
		if(a==0xf7){po+=4;goto redis;}
		if(a>=0xfc ){break;}

		po=po+4;
	}
	return(po);
}

/***************************/
int	padjust(int *po,int i)
{
	int	y=0;

	if(*po<0){*po=tr_len[track]-4;}
	if(tr_len[track]<25*4){i=23;}

	while(i>0 && *po>0){
loop:		*po=*po-4;if(trk[track][*po]==0xf7){goto loop;}
		y++;i--;
	}
	trk_dis(6,*po,24);return(y);
}

/***************************/
int	meas_no(int ad)
{
	int	co=1,j;
	j=ad;
	while(j>0 ){
		j=j-4;
		if( trk[track][j]>=0xfc || trk[track][j+4]==0xfc ){co++;}
	}
	return(co);
}

/***************************/
/*
int	meas_no2(int ad)
{
	int	lx[16];
	int	i,pp,a,ct;
	unsigned char	c;
	unsigned char	*ptr;

	ptr=(unsigned char *)trk[track];

	pp=0;a=1;i=0;
	while(i<ad){
		c=ptr[i];
		if( c>=0xfc || (i+4<ad && ptr[i+4]==0xfc) ){
			a+=1;
		}
		if( c==0xf9 ){
			if(pp<16){
				lx[pp]=a;a=0;pp++;
			}
		}
		if( c==0xf8 ){
			if(pp>0){pp--;
				ct=ptr[i+1];if(ct==0||ct==255){ct=1;}
				a=lx[pp]+(a*ct);
			}
		}
		i+=4;
	}

	for(i=0;i<pp;i++){a+=lx[i];}
	return(a);
}
*/
/***************************/
int	meas_add(int me)
{
	int	co=1,j=0,ad=0,tr;
	if( me>1){
		tr=track;
		while(j<tr_len[tr]-4 ){
			if( trk[tr][j]>=0xfc || trk[tr][j+4]==0xfc ){
				co++;if( co==me ){ad=j+4;break;}
			}
			j=j+4;
		}
		if(ad==0){ad=-1;}
	}
	return(ad);
}

/***************************/
int	dat_add(int ln,int me)
{
	int	co=1,j=0,ad=0;
	if( me>1){
		while(j<ln ){
			if( dat[j]>=0xfc ){
				co++;if( co==me ){ad=j+4;break;}
			}
			j=j+4;
		}
		if(ad==0){ad=-1;}
	}
	return(ad);
}

/***************************/
int	meas_len(int j)
{
	int	co=0,d;

	d=trk[track][j];
	while(j<tr_len[track] ){
		if( d==0xfe ){break;}
		co++;
		if( d>0xfb ){break;}
		j=j+4;d=trk[track][j];
	}
	return(co*4);
}

/***************************/
int	add_set(int tr,int *add)
{
	int	co=0,i=0;

	add[0]=0;
	while(i<tr_len[tr]){
		if(trk[tr][i]>=0xfc||trk[tr][i+4]==0xfc){
			co++;if(co<2048){add[co]=i+4;}
		}
		i=i+4;
	}
	for(i=co+1;i<2048;i++){add[i]=0;}
	return(co);
}

/***************************/
void	undobuf(int a)
{
	int	i,d;

	if(a==0){
		memcpy_l(rcpbuf,trk[track],tr_len[track]);
		undof[0]=track;undof[1]=tr_len[track];rcpf=-1;
	}else{
		if(rcpf>=0 || track!=undof[0]){
			msg("UNDOバッファのデータは無効です。");return;}

		if(size_change(track,undof[1])){return;}

		for(i=0;i<undof[1];i++){
			d=trk[track][i];trk[track][i]=rcpbuf[i];rcpbuf[i]=d;
		}
		d=tr_len[track];tr_len[track]=undof[1];undof[1]=d;
		msg("UNDOしました。");
	}
	snsclr();
}

/***************************/
void	same_shift(int spo,int m,int cu)
{
	int	i,me,me2,ec,po,ad,of;
	unsigned char	a,b,c,d;
	int	add[2048];
	int	sad[1024];
	int	tr=track;

	if(cu==0){return;}

	msg_clr();me=meas_no(spo)-1;me2=me+cu;

	add_set(tr,add);for(i=0;i<1024;i++){sad[i]=0;}
	if(me<1024){
		if(m==0){
/*insert*/
			int	i,j;
			if(spo>0 && trk[tr][spo-4]<0xfc && trk[tr][spo]<0xfc){
				me++;}

			if(trk[tr][spo]>0xfc){me++;me2++;}

			j=1024-cu;if(j<0){j=0;}
			for(i=j;i<1024;i++){sad[i]=-1;}
		}else{
/*delete*/
			int	i,j;
			if(trk[tr][spo]>0xfc && trk[tr][spo+4]>0xfc){me++;me2++;}
			j=me2;if(j>1024){j=1024;}
			for(i=me;i<j;i++){sad[i]=-1;}
			spo=add[me2];
		}
	}

	i=0;po=0;
	while(i<tr_len[tr]){
		if(po>=TRACK_SIZE){po+=4;break;}
		a=trk[tr][i];b=trk[tr][i+1];c=trk[tr][i+2];d=trk[tr][i+3];
		if(a==0xfc && i>=spo){
			int	smn=b+(c&3)*256;
			if(sad[smn]<0){
				ad=add[smn];if(m==1){sad[smn]=meas_no(i);}
				if(i>0 && trk[tr][i-4]<0xfc){dat[po]=0xfd;po=po+4;}

				while(trk[tr][ad]<0xfc){
					if(po>=TRACK_SIZE){po+=4;break;}
					dat[po]=trk[tr][ad];dat[po+1]=trk[tr][ad+1];
					dat[po+2]=trk[tr][ad+2];dat[po+3]=trk[tr][ad+3];
					ad=ad+4;po=po+4;
				}
				if(po>TRACK_SIZE){break;}
				dat[po]=0xfd;po=po+4;
			}else{
				if(sad[smn]>0){of=sad[smn]-1;b=of&0xff;c=of>>8;}
				dat[po]=a;dat[po+1]=b;dat[po+2]=c;dat[po+3]=d;po=po+4;
			}
		}else{
			dat[po]=a;dat[po+1]=b;dat[po+2]=c;dat[po+3]=d;po=po+4;
		}
		i=i+4;
	}

	if(size_change(tr,po)){return;}
	if(po<=TRACK_SIZE){tr_len[tr]=po;memcpy_l(trk[tr],dat,po);}

	ec=0;
	for(i=spo ;i< tr_len[tr];i=i+4){
		a=trk[tr][i];
		if( a==0xfc ){
			int	smn=trk[tr][i+1]+(trk[tr][i+2]&3)*256;
			if(me<=smn ){
				if(m==0){
					if(smn+cu<1024){smn+=cu;}else{smn=1023;ec++;}
				}else{
					if(me2>smn ){smn=0;ec++;}else{smn=smn-cu;}
				}
			}
			trk[tr][i+1]=smn&0xff;trk[tr][i+2]=smn>>8;
		}
	}
	if(ec!=0){
		msg("SAME MEASが");B_PRINT(fstr(ec,4));B_PRINT("箇所無効になりました。");
	}

	snsclr();
}

/***************************/
int	same_meas_extr(int tr,int ad,int ln)
{
	int	po;

	if(ln==0){return(0);}
	undobuf(0);msg("SAME MEAS 展開中...");po=same_ext_sub(tr,ad,ad+ln,1);

	if(ad>0 && trk[tr][ad]==0xfc && trk[tr][ad-4]<0xfc){
		memcpy_l(&dat[4],dat,po);dat[0]=0xfd;po+=4;
	}

	if(size_change(tr,tr_len[tr]-ln+po)){return 1;}
	trk_delete(ad,ln);trk_shift(ad,po);
	memcpy_l(&trk[tr][ad],dat,po);

	return(0);
}

/***************************/
int	cpybuf_set(int tr,int ad,int ln)
{
	int	po;

	po=same_ext_sub(tr,ad,ad+ln,0);

	if(po<=TRACK_SIZE){
		memcpy_l(cpbuf,dat,po);
	}else{
		msg("コピーバッファ容量を越えるので、展開しません。");
		memcpy_l(cpbuf,&trk[track][ad],ln);
		return(ln);
	}
	return(po);
}

/***************************/
void	dat_rev(int tr,int ad,int ln)
{
	int	po,i,i2,co,c;

	if(ln<8){return;}

	i=ad;po=ln-4;
	while(po>=0){
		unsigned char	a=cpbuf[po];
		if(a>=0xf0){
			if(a==0xf8 || a==0xf9){a=497-a;}
			if(a==0xf7 || a==0xf6 || a==0x98){
				c=0;i2=po;
				while(a==0xf6 || a==0x98 || a==0xf7){
					c+=4;if(i2==0){break;}
					i2-=4;a=cpbuf[i2];
				}
				for(i2=0;i2<c;i2++){trk[tr][i+i2]=cpbuf[po-c+4+i2];}
				i+=c;po-=c;
			}else{
				trk[tr][i]=a;trk[tr][i+1]=cpbuf[po+1];
				trk[tr][i+2]=cpbuf[po+2];trk[tr][i+3]=cpbuf[po+3];
				i+=4;po-=4;
			}
		}else{
			unsigned char	b=cpbuf[po+1];
			if(b!=0){
				c=0;i2=po;
				while(1){
					c+=4;if(i2==0){break;}
					i2-=4;b=cpbuf[i2+1];
					if(b!=0){break;}
				}
				for(i2=0;i2<c;i2++){trk[tr][i+i2]=cpbuf[po-c+4+i2];}
				i+=c;po-=c;
			}else{
				trk[tr][i]=a;trk[tr][i+1]=cpbuf[po+1];
				trk[tr][i+2]=cpbuf[po+2];trk[tr][i+3]=cpbuf[po+3];
				i+=4;po-=4;
			}
		}
	}

	i=ad+ln-4;
	while(i>=ad){
		if(trk[tr][i]==0xf8){
			i2=i-4;co=1;
			while(i2>=ad){
				unsigned char	a=trk[tr][i2];
				if(a==0xf8){co++;}
				if(a==0xf9){co--;if(co==0){trk[tr][i+1]=trk[tr][i2+1];break;}}
				i2-=4;
			}
		}
		i-=4;
	}
}

/***************************/
int	replicate(int ad,int sh)
{
	int	top,end,topad,endad,times,bias,dummy,a,i,j,escrn,dist_no;;

	dummy=track;escrn=edit_scr;edit_scr=0;cons_md(0);
	dist_no=meas_no(ad);

rip1:	msg("Replicate TR.=");

	if(sh&1){
		B_PRINT("all");
	}else{
		track=vinput(dummy+1,0x802)-1;
		if( es!=0 ){track=dummy;edit_scr=escrn;cons_md(1);msg_clr();return(1);}
		if( track<0 || track>35){goto rip1;}
	}

rip2:	B_LOCATE(18,1);B_PRINT("TOP=");top=vinput(1,0x804);
	if(sh&1){
		if( es!=0 ){track=dummy;edit_scr=escrn;cons_md(1);msg_clr();return(1);}
	}
	if( es!=0 ){goto rip1;}

	if(top<1){goto rip2;}

rip3:	B_LOCATE(27,1);B_PRINT("END=");end=vinput(top,0x804);
	if( es!=0 ){goto rip2;}
	if( end<top){goto rip3;}

rip4:	B_LOCATE(36,1);B_PRINT("TIMES=");times=vinput(1,0x803);
	if( es!=0 ){goto rip3;}
	if(times<1 || times>255){goto rip4;}

rip5:	B_LOCATE(45,1);B_PRINT("BIAS=");bias=vinput(0,0x803);
	if( es!=0 ){goto rip4;}
	if(bias<-24 || bias>24){goto rip5;}


loop:
	topad=meas_add(top);if(topad<0){topad=tr_len[track]-4;}
	endad=meas_add(end+1);if(endad<0){endad=tr_len[track]-4;}

	if(tr_len[track]>4 && topad>=0){
		cpleng=cpybuf_set(track,topad,endad-topad);

		if(cpleng>0 && cpbuf[cpleng-4]!=0xfd){cpbuf[cpleng]=0xfd;cpleng+=4;}
		for(i=0;i<cpleng;i=i+4){a=cpbuf[i];if(a<127){a=a+bias;if(a>=0 && a<128){cpbuf[i]=a;}}}

		if((sh&1)==0){
			track=dummy;
		}else{
			ad=meas_add(dist_no);if(ad<0){ad=tr_len[track]-4;}
		}

		undobuf(0);

		if(size_change(track,tr_len[track]+cpleng)){return 1;}

		j=times;
		while(j!=0){
			if(trk_shift(ad,cpleng)==0){
				same_shift(ad,0,buf_meas(cpbuf,cpleng));
				for(i=0;i<cpleng;i++){trk[track][i+ad]=cpbuf[i];}
			}
			j--;
		}
	}

	trk_lin(track,-1,0);
	if(sh&1){
		track++;if(track>35){track=0;}
		if(track!=dummy){goto loop;}
	}

	track=dummy;
	size_max(track);
	edit_scr=escrn;cons_md(1);msg_clr();return(0);
}

/***************************/
int	replicate2(int ad,int sh)
{
	int	top,/*end,topad,endad,*/times,bias,dummy,a,i,j,escrn;

	dummy=track;escrn=edit_scr;edit_scr=0;cons_md(0);


	if( cplen<0 && cpadd!=ad){
		cplen=abs(cpadd-ad);if(cpadd>ad ){cpadd=ad;}
		cpleng=cpybuf_set(track,cpadd,cplen);
		if(cpleng>0 && cpbuf[cpleng-4]!=0xfd){cpbuf[cpleng]=0xfd;cpleng+=4;}
	}else{return 1;}


rip1:	msg("Replicate2 TR.=");

	if(sh&1){
		B_PRINT("all");
	}else{
		track=vinput(dummy+1,0x802)-1;
		if( es!=0 ){track=dummy;edit_scr=escrn;cons_md(1);msg_clr();return(1);}
		if( track<0 || track>35){goto rip1;}
	}

rip2:	B_LOCATE(18,1);B_PRINT("TOP=");top=vinput(1,0x804);
	if(sh&1){
		if( es!=0 ){track=dummy;edit_scr=escrn;cons_md(1);msg_clr();return(1);}
	}
	if( es!=0 ){goto rip1;}

	if(top<1){goto rip2;}


rip4:	B_LOCATE(36,1);B_PRINT("TIMES=");times=vinput(1,0x803);
	if( es!=0 ){goto rip2;}
	if(times<1 || times>255){goto rip4;}

rip5:	B_LOCATE(45,1);B_PRINT("BIAS=");bias=vinput(0,0x803);
	if( es!=0 ){goto rip4;}
	if(bias<-24 || bias>24){goto rip5;}


	for(i=0;i<cpleng;i=i+4){a=cpbuf[i];if(a<127){a=a+bias;if(a>=0 && a<128){cpbuf[i]=a;}}}
loop:
	/*if(tr_len[track]>4 && meas_add(top)>=0){*/
	if((sh&1)==0 || mch[track]!=0){

		ad=meas_add(top);if(ad<0){ad=tr_len[track]-4;}

		undobuf(0);

		if(size_change(track,tr_len[track]+cpleng)){return 1;}

		j=times;
		while(j!=0){
			if(trk_shift(ad,cpleng)==0){
				same_shift(ad,0,buf_meas(cpbuf,cpleng));
				for(i=0;i<cpleng;i++){trk[track][i+ad]=cpbuf[i];}
			}
			j--;
		}
	}

	trk_lin(track,-1,0);
	if(sh&1){
		track++;if(track>35){track=0;}
		if(track!=dummy){goto loop;}
	}

	track=dummy;
	size_max(track);
	edit_scr=escrn;cplen=0;cons_md(1);msg_clr();return(0);
}

/***************************/
void	bendc(int ad,int sh)
{
	int	stt[256];

	int	ct=8192,ct2=8192,st,i,j,cc,ss,ctlch=0;
	unsigned char	da=trk[track][ad],db=trk[track][ad+4];
	unsigned char	ca=0xee,cf=trk[track][ad+2];

	if((sh&1)==0){
		if( da==0xeb && db==0xeb ){
			if( cf!=trk[track][ad+6] || cf>=120){return;}
			msg(ctrl_type(cf));ctlch=1;ca=0xeb;
		}else{
			if( da==0xde && db==0xde ){
				msg("Rol.Para");ctlch=1;ca=0xde;
			}else{
				if( (da>127 && da!=0xee) || (db>127 && db!=0xee) ){return;}
				msg("PITCH BEND");
			}
		}

		if(ctlch==0){
			if(da==0xee){ct=trk[track][ad+2]+trk[track][ad+3]*128;}
			if(db==0xee){ct2=trk[track][ad+6]+trk[track][ad+7]*128;}
			if(da<128 && db<128){ct2+=((db-da)*1365)>>1;}

			if(ct2-ct>8192 || ct2-ct<-8192){msg_clr();return;}
		}else{
			ct=trk[track][ad+3];ct2=trk[track][ad+7];
		}

		ct2=(ct2-ct);if(ct2>8191){ct2=8191;}
		st=trk[track][ad+1];

		B_PRINT(" COUNT=");j=vinput(0,3);msg_clr();
		if( es!=0 || j<2 || j>st ){return;}

		undobuf(0);
		if(trk_shift(ad,j*4-4)!=0){return;}

		bunkatsu(st,j,stt);

		cc=0;ss=ct;
		for(i=0;i<j;i++){
			unsigned char	ea,eb;
			if(ct2!=0){ss=ct+(ct2*cc/j);cc++;}
			trk[track][ad]=ca;trk[track][ad+1]=stt[i];
			if(ctlch==0){ea=ss&0x7f;eb=(ss>>7)&0x7f;}else{ea=cf;eb=ss;}
			trk[track][ad+2]=ea;trk[track][ad+3]=eb;
			ad=ad+4;
		}
	}else{
		unsigned char	ea=trk[track][ad+2];
		unsigned char	eb=trk[track][ad+3];
		unsigned char	tmp[64];
		int ke,md=0,co;

		if(da>127 || db>127){msg_clr();return;}

/*		msg("NOTE");
		msg_clr();*/

resel:
		msg("Mode: (w=1/w+b=2)");

		while(B_KEYSNS()==0){}
		ke=B_KEYINP()&0xff;
		msg_clr();

		if(ke==0x1b){return;}
		if(ke=='1'){md=1;}
		if(ke=='2'){md=2;}
		if(md<=0){goto resel;}


		ct=da;ct2=db;ct2=(ct2-ct);
		st=trk[track][ad+1];

		j=abs(ct2);
		if( j<2 || j>st ){return;}

		co=0;
		for(i=0;i<j;i++){
			strcpy(tmp,keystr(ct+(ct2*i/j)));
			if(tmp[1]==' '||md==2){co++;}
		}

		undobuf(0);
		if(trk_shift(ad,co*4-4)!=0){return;}
		bunkatsu(st,co,stt);

		co=0;ss=ct;
		for(i=0;i<j;i++){
			ss=ct+(ct2*i/j);
			strcpy(tmp,keystr(ss));
			if(tmp[1]==' '||md==2){
				trk[track][ad]=ss;trk[track][ad+1]=stt[co];
				trk[track][ad+2]=ea;trk[track][ad+3]=eb;
				ad=ad+4;co++;
			}
		}
	}
}

/***************************/
int	bendc2(int ad,char *s)
{
	int	po,j=0,a;
	unsigned char	c;

	a=ctc(-1,s,0);
	if( a<0 ){return(0);}
	if( a!=0 ){
		po=ad;j=-1;
		while(po>=0){
			c=trk[track][po];if( c<128 ){j=c;break;}
			po=po-4;
		}
		if( j<0 ){es=1;}else{j=((ctc(j,s,/*scale_no(ad)*/0)-j)*1365)>>1;}
	}else{
		j=str_val(s);
	}
	return(j);
}

/***************************/
int	bank_no(int j)
{
	int	bank=-1,bank_l=-1;

	while(j>0){
		unsigned char a;
		j-=4;
		a=trk[track][j];

		if(a==0xeb){
			if(trk[track][j+2]==0 && bank<0){bank=trk[track][j+3];}
			if(trk[track][j+2]==32 && bank_l<0){bank_l=trk[track][j+3];}
		}
		if(a==0xe2){
			if(bank<0){bank=trk[track][j+3];}
		/*	if(bank_l<0){bank_l=0;}*/
		}
	}
	if(bank<0 || bank>127){bank=0;}
	if(bank_l<0 || bank_l>127){bank_l=0;}
	return(bank_l*256+bank);
}

/***************************/
int	prog_no(int j)
{
	int	prog=0;

	while(j>=0){
		if(trk[track][j]==0xec){prog=trk[track][j+2];break;}
		if(trk[track][j]==0xe2){prog=trk[track][j+2];break;}
		j-=4;
	}
	return(prog);
}

/***************************/
int	channele_no(int j)
{
	int	ch=mch[track];

	while(j>0){
		j-=4;
		if(trk[track][j]==0xe6){ch=trk[track][j+2];break;}
		if(trk[track][j]==0xec){ch=trk[track][j+3];break;}
	}
	return(ch);
}

/***************************/
void	trk_next(int m)
{
	int	trk=track;
trkloop:
	if((m&1)==0){
		trk--;if(trk<0){trk=35;}
	}else{
		trk++;if(trk>35){trk=0;}
	}
	if(track!=trk &&(tr_len[trk]==4||(m>=2 && !rcd->active[trk]))){goto trkloop;}
	if(track!=trk){cplen=0;}
	track=trk;
}

/***************************/
int	spc_code(char *s,int *ctl)
{
	unsigned char	a,b;
	int 	c1=0xeb,c2=-1;


	jstrupr(s);a=s[0];b=s[1];
	if(a=='/'){a=s[1];b=s[2];}

	if( a=='@' ){c2=0;if( b=='@'||b=='L'){c2=32;}}		/* BANK SELECT	*/
	if( a=='D' ){c2=1;}
	if( a=='V' ){c2=7;}
	if( a=='N' ){c2=10;}
	if( a=='E' ){c2=11;}
	if( a=='H' ){c2=64;}
	if( a=='A' ){c2=121;}

	if( a=='F' && b>='1' && b<='5' ){c2=b-'1'+91;}		/* EFFECT 1-5	*/
	if( a=='P' ){	if( b=='T' ){c2=5;}			/* PORTA.TM	*/
			if( b=='O' ){c2=65;}			/* PORTAMEN	*/
			if( b=='C' ){c2=84;}}			/* POR.CONT	*/
	if( a=='S' ){	if( b=='S' ){c2=66;}			/* SOSTENUT	*/
			if( b=='T' ){c2=67;}}			/* SOFT		*/
	if( a=='E' ){	if( b=='M' ){c2=6;}			/* ENTRY M.	*/
			if( b=='L' ){c2=38;}}			/* ENTRY L.	*/
	if( a=='N'){	if( b=='M' ){c2=99;}			/* NRPN MSB	*/
			if( b=='L' ){c2=98;}}			/* NRPN LSB	*/
	if( a=='R' ){	if( b=='M' ){c2=101;}			/* RPN MSB	*/
			if( b=='L' ){c2=100;}}			/* RPN LSB	*/

	if( a=='B' ){	if( b=='R' ){c2=2;}}			/* BREATH	*/

	if( a=='R' ){	if( b=='S' ){c2=71;}}			/* Resonanc	*/
	if( a=='R' ){	if( b=='T' ){c2=72;}}			/* Rel.Time	*/
	if( a=='A' ){	if( b=='T' ){c2=73;}}			/* Att.Time	*/
	if( a=='C' ){	if( b=='O' ){c2=74;}}			/* Cutoff F	*/

	if(c2<0){
		c1=0;
		if( a=='U' && b>='0' && b<='7'){c1=0x90+b-'0';}	/* UserExc 0-7	*/
		if( a=='X' ){c1=0x98;}
		if( a=='B' ){c1=0xdd;}
		if( a=='R' ){c1=0xde;}
		if( a=='I' ){c1=0xdf;}
		if( a=='T' ){c1=0xe7;}
		if( a=='C' ){c1=0xea;}
		if( a=='S' ){
			if( b>='0' && b<='9' ){c1=0xc0+b-'0';}
			if( b>='A' && b<='F' ){c1=0xca+b-'A';}
			if( b=='4' ){c1=0xdc;}
		}
		if( a=='L' ){c1=0xeb;if(s[1]!=0){c2=atoi(&s[1]);}}
		if( a>='0' && a<='9' ){c1=0xeb;c2=atoi(s);if(c2>127){c2=-1;}}

		if( a=='G' ){
			if( b=='@' || b=='M' ){c1=0xe2;if(s[2]!=0){c2=atoi(&s[2]);}
			}else{c1=0xec;if(s[1]!=0){c2=atoi(&s[1]);}}
		}
		if( a=='K' ){c1=0xed;}
		if( a=='P' ){c1=0xee;}
		if( a=='M' ){c1=0xe6;}
/*
		if( a=='M' ){
			c1=0xe6;
			if( (b>='0' && b<='9') || b=='A' || b=='B' ){
				if(s[2]!=0){
					c2=strch(1,&s[2]);
				}else{
					if(s[1]!=0){c2=strch(1,&s[1]);}
				}
			}
		}
*/
	}
	*ctl=c2;return(c1);
}

/***************************/
char	*ctrl_type(int a)
{
static char	tmp0[16];
int	cc;

	switch(a){
		case 0  :strcpy(tmp0,"BANK MSB");break;
		case 1  :strcpy(tmp0,"MODULAT ");break;
		case 2  :strcpy(tmp0,"BREATH  ");break;
		case 4  :strcpy(tmp0,"FOOT C. ");break;
		case 5  :strcpy(tmp0,"PORTA.TM");break;
		case 7  :strcpy(tmp0,"VOLUME  ");break;
		case 8  :strcpy(tmp0,"BALANCE ");break;
		case 10 :strcpy(tmp0,"PANPOT  ");break;
		case 11 :strcpy(tmp0,"EXPRESS ");break;

		case 32 :strcpy(tmp0,"BANK LSB");break;
/*		case 33 :strcpy(tmp0,"MODULA.L");break;
		case 34 :strcpy(tmp0,"BREATH.L");break;
		case 36 :strcpy(tmp0,"FOOT C.L");break;
		case 37 :strcpy(tmp0,"PORTA.TL");break;
		case 39 :strcpy(tmp0,"VOLUME.L");break;
		case 40 :strcpy(tmp0,"BALANC.L");break;
		case 42 :strcpy(tmp0,"PANPOT.L");break;
		case 43 :strcpy(tmp0,"EXPRES.L");break;*/

/*70:patch sel  102|*/

		case 64 :strcpy(tmp0,"HOLD1   ");break;	/*HOLD1(Damper)*/
		case 65 :strcpy(tmp0,"PORTAMEN");break;
		case 66 :strcpy(tmp0,"SOSTENUT");break;
		case 67 :strcpy(tmp0,"SOFT    ");break;
		case 69 :strcpy(tmp0,"HOLD2   ");break;	/*HOLD2(Freez )*/

		case 71 :strcpy(tmp0,"Resonanc");break;
		case 72 :strcpy(tmp0,"Rel.Time");break;
		case 73 :strcpy(tmp0,"Att.Time");break;
		case 74 :strcpy(tmp0,"Cutoff F");break;	/* Cutoff Freq. */

		case 84 :strcpy(tmp0,"POR.CONT");break;	/*legart*/

/*tg300*/	case 89 :strcpy(tmp0,"VariSend");break;	/*Variation   94?*/
		case 90 :strcpy(tmp0,"DryLevel");break;

		case 91 :strcpy(tmp0,"REVERB  ");break;
/*		case 92 :strcpy(tmp0,"EFFECT2 ");break;*/
		case 93 :strcpy(tmp0,"CHORUS  ");break;
		case 94 :strcpy(tmp0,"DELAY   ");break;
/*		case 95 :strcpy(tmp0,"EFFECT5 ");break;*/

		case 6  :strcpy(tmp0,"DATA MSB");break;
		case 38 :strcpy(tmp0,"DATA LSB");break;
		case 96 :strcpy(tmp0,"DATA INC");break;
		case 97 :strcpy(tmp0,"DATA DEC");break;
		case 98 :strcpy(tmp0,"NRPN LSB");break;
		case 99 :strcpy(tmp0,"NRPN MSB");break;
		case 100:strcpy(tmp0,"RPN  LSB");break;
		case 101:strcpy(tmp0,"RPN  MSB");break;

		case 120:strcpy(tmp0,"SOND OFF");break;
		case 121:strcpy(tmp0,"RES.ALL ");break;
		case 122:strcpy(tmp0,"LOCAL C.");break;	/*local control*/
		case 123:strcpy(tmp0,"NOTE OFF");break;
		case 124:strcpy(tmp0,"OMNI OFF");break;
		case 125:strcpy(tmp0,"OMNI ON ");break;
		case 126:strcpy(tmp0,"MONO Mod");break;
		case 127:strcpy(tmp0,"POLY Mod");break;

		default :strcpy(tmp0,"CONTROL ");
	}
/*10 30 50 汎用操作子 h-12341234 5678
*/
	cc=(mdlflag&15);
/*05rw*/
	if(cc==0x4){
		switch(a){
			case 12 :strcpy(tmp0,"Efect1CT");break;
			case 13 :strcpy(tmp0,"Efect2CT");break;
			case 92 :strcpy(tmp0,"Efect1SW");break;
			case 94 :strcpy(tmp0,"Efect2SW");break;
		}
	}
/*
	if(cc==0){
		switch(a){
			case 92 :strcpy(tmp0,"Tremolo ");break;
			case 94 :strcpy(tmp0,"Cereste ");break;
			case 95 :strcpy(tmp0,"Phaser  ");break;
		}
	}
*/
	return(tmp0);
}

/***************************/
/*
char	*rcpc_type(int a)
{
static char	tmp0[16];

	switch( a){
		case  0x90:
		case  0x91:
		case  0x92:
		case  0x93:
		case  0x94:
		case  0x95:
		case  0x96:
		case  0x97:strcpy(tmp0,"UserExc ");tmp0[7]=a-0x90+'0';break;
		case  0x98:strcpy(tmp0,"Tr.Exclu");break;

		case  0xc0:strcpy(tmp0,"DX7FUNC");break;
		case  0xc1:strcpy(tmp0,"DX.PARA");break;
		case  0xc2:strcpy(tmp0,"DX.PREF");break;
		case  0xc3:strcpy(tmp0,"TX.FUNC");break;

		case  0xc5:strcpy(tmp0,"FB-01 P");break;
		case  0xc6:strcpy(tmp0,"FB-01 S");break;
		case  0xc7:strcpy(tmp0,"TX81Z V");break;
		case  0xc8:strcpy(tmp0,"TX81Z A");break;
		case  0xc9:strcpy(tmp0,"TX81Z P");break;
		case  0xca:strcpy(tmp0,"TX81Z S");break;
		case  0xcb:strcpy(tmp0,"TX81Z E");break;
		case  0xcc:strcpy(tmp0,"DX7-2 R");break;
		case  0xcd:strcpy(tmp0,"DX7-2 A");break;
		case  0xce:strcpy(tmp0,"DX7-2 P");break;
		case  0xcf:strcpy(tmp0,"TX802 P");break;

	case  0xd0:strcpy(tmp0,"Yam.Base");break;
	case  0xd1:strcpy(tmp0,"Yam.Para");break;
	case  0xd2:strcpy(tmp0,"Yam.Dev#");break;
	case  0xd3:strcpy(tmp0,"XG Para");break;

		case  0xdc:strcpy(tmp0,"MKS-7  ");break;
		case  0xdd:strcpy(tmp0,"Rol.Base");break;
		case  0xde:strcpy(tmp0,"Rol.Para");break;
		case  0xdf:strcpy(tmp0,"Rol.Dev#");break;

		case  0xe2:strcpy(tmp0,"BankProg");break;

	case  0xe1:strcpy(tmp0,"BnkL&Prg");break;
	case  0xe2:strcpy(tmp0,"BnkM&Prg");break;
??e4 p.bank?
		case  0xe5:strcpy(tmp0,"KeyScan");break;
		case  0xe6:strcpy(tmp0,"MIDI CH.");break;
		case  0xe7:strcpy(tmp0,"TEMPO");break;

		case  0xea:strcpy(tmp0,"AFTER C.");break;
		case  0xeb:strcpy(tmp0,"CONTROL ");break;
		case  0xec:strcpy(tmp0,"PROGRAM ");break;
		case  0xed:strcpy(tmp0,"AFTER K.");break;
		case  0xee:strcpy(tmp0,"PITCH");break;
		case  0xef:strcpy(tmp0,"CMU-800");break;
f4 *segno?
		case  0xf5:strcpy(tmp0,"Music Key ");break;

		default:strcpy(tmp0,fstr(a,7));
	}
}
*/
/***************************/

