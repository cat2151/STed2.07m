/*************************************/
/* real time recording subroutine    */
/*************************************/

#include <stdlib.h>
#include <iocslib.h>
#include <doslib.h>
#include <string.h>
#include <graph.h>
#include "sted.h"

unsigned char	*note, *note_data;
int	trch,lastptr,pt,tr,len,cmd,d_byte;
int	step,barlen,barunit,stepsum,midi_ch,exc_no,exc_id;
int	nbuf[16], vbuf[16], gbuf[16];

int	metoro_trk,metoro_sw,topmeas,recmeas,btmmeas,ecount_sw;

void	msg();
void	msg_clr();
void	sdis();
char	*fstr();
char	*spadd();
char	*chstr();
int	strch();
int	vinput();
void	home2();
void	all_note_off();
void	snsclr();
void	g_print();
void	twait();
void	midi_clr();
void	rec_filter();

int	last_tempo();
int	lsp_para_set();
int	lsp_wait_chack();

void	hedset();
void	thedset();
int	rcp_buf_put();
int	rcp_buf_get();

int	trk_ext_sub();
int	trk_mix_sub();
int	size_max();
int	size_change();
int	size_add();
void	size_ref();

void	tai_compres();
void	trksize();

int	meas_add();
int	meas_no();
void	same_cluc();
int	step_cluc();

int	real_record();
void	put_delta();
int	get_delta();
void	rec_ch_change();
void	rec_md_change();
void	rec_mt_change();
void	rec_sy_change();
void	rec_meas_change();
void	rec_ch_disp();
void	mix_one();
void	mix_two();
int	countstart();
void	rec_conv();
int	rec_ext();
int	meas_adjust();
int	meas_adj_sub();

/***************************/
int	real_record()
{
	int	a,b,co=0,ad=0,stc,ost=0,bc,stm,mm,lastst,ecst;
	int	i,ccc,dtr=0,pm,pr=0,ecount=0;
	int	mtr_tempo=mtempo;
	unsigned char	*mbuf;
	unsigned char	ctrl_tbl[256];

	for(i=0;i<128;i++){ctrl_tbl[i]=i;}
	for(i=16;i<32;i++){
		if(rfilt[i][0]!=rfilt[i][1]){
			if(rfilt[i][0]<128 && rfilt[i][1]<128){
				ctrl_tbl[rfilt[i][0]]=rfilt[i][1];
			}
		}
	}

	pm=rcd->put_mode;
	cpleng=0;rcpf=0;
	topmeas=1;recmeas=1;btmmeas=999;metoro_sw=0;ecount_sw=0;

	note_data = cpbuf;note = note_data + 16;
	for(i=0;i<16;i++){note_data[i] = 0xff;}
	mbuf=rcpbuf;

	barlen = tbase * 4 * beat1 / beat2;

	ecst=tbase/24;
	all_note_off(1);

	fill(0,544,767,1001,0);
rewrite:
	box(0,564+4,767,564+128+2,14,65535);

	box(0,564+128+6,767,564+128+6+26,14,65535);
	box(0,564+128+6+30,767,1001,14,65535);

	sdis(0,548+4," REALTIME RECORDING",20,15,8);home2(512);

	C_WINDOW(0,32);

/*	B_LOCATE(2,31);B_PRINT("MIDI IN:");B_PRINT(mdinport());*/

	B_LOCATE(2,31);B_PRINT("RECORDING CH.: 1   OUT MODE:THRU   METORO:ON   SYNC:INT");

	B_LOCATE(76,31);B_PRINT("[ESC]で終了します。");


	B_LOCATE(23,12);B_PRINT("RECORD MEAS:   1- 999");

	B_LOCATE(52,12);B_PRINT("TEMPO:");B_PRINT(fstr(mtr_tempo,3));
	B_PRINT("   BEAT:");B_PRINT(fstr(beat1,2));B_PRINT(" /");B_PRINT(fstr(beat2,2));
	B_PRINT("    Used Buffer:  0%");

/*	B_LOCATE(90-11,14);B_PRINT("Used Buffer:  0%");*/

/*	B_PRINT("   TIMEBASE:");B_PRINT(fstr(tbase,3));*/


restart:
	rec_ch_disp();
	pr=countstart(0,topmeas);

	mm=0;
	for(i=0;i<36;i++){if(trmod[i]&2){mm++;}}
	if(mm==0){msg("レコーディングトラックが設定されていません。");}

	B_LOCATE(1,4);B_PRINT("RECORDING 待機中・・・");

	midi_clr();a=0;ccc=0;stm=0;

	while(1){
		a=(*rcd->md_get)();if((a>127 && a<0xf7) || a==0xfa){stm=2;break;}

		if( INPOUT(0xfe)!=0 ){
			int k=INPOUT(0xff);
			if(k==27){goto ext;}
			if(k==13 || k==32){pr=countstart(1,topmeas);break;}
			if(k=='c' || k=='C'){rec_ch_change(pm);}
			if(k=='o' || k=='O'){rec_md_change();}
			if(k=='m' || k=='M'){rec_mt_change(0);}
			if(k=='y' || k=='Y'){rec_sy_change();}
			if(k=='s' || k=='S'){rec_meas_change();goto restart;}
			if(k=='f' || k=='F'){rec_filter(1);goto rewrite;}
		}
	}

	B_LOCATE(11,4);B_PRINT("START       ");

	if(metoro_trk>=0){dtr=metoro_trk;}

	(*rcd->begin)();

	rec_mt_change(1);rec_mt_change(1);

	if(pr){co=pr;}

	lastst=(btmmeas+1-(topmeas))*barlen+pr;

	if(stm==0 && topmeas==1){
		int cc,cc2;
		B_LOCATE(1,6);B_PRINT("COUNT");

		co=barlen*2;lastst+=co;cc2=co/10;
		while(1){
			cc=rcd->stepcount;
			B_LOCATE(9,6);B_PRINT(fstr((co-cc)/cc2,4));

			if( INPOUT(0xfe)!=0 ){
				int k=INPOUT(0xff);
				if(k==27){(*rcd->end)();goto ext;}
				if(k=='m' || k=='M'){rec_mt_change(1);}
			}

			if(cc>=co){break;}
		}
		midi_clr();
	}
	ecount=co;

	B_LOCATE(1,6);B_PRINT("COUNTER START");
	B_LOCATE(1,14);B_PRINT("MEAS/STEP = ----/---");
	bc=0;


	if(a>127 && a<0xf7){
		if(ecount_sw){stc=ecount;}else{stc=rcd->stepcount;}
		co=stc;goto dtm;}

	if(topmeas>1){stm=topmeas;}

	while(1){
		int	nst=rcd->step[dtr];

		if(nst!=ost || beat1==1){
			B_LOCATE(13,14);B_PRINT(fstr(rcd->bar[dtr]-2+stm,4));
			if(beat1>1){B_LOCATE(18,14);B_PRINT(fstr(((nst+1)>>1),3));}
			ost=nst;

			B_LOCATE(91,12);B_PRINT(fstr(ad*100/work_size,3));
		}
		if( rcd->panel_tempo != mtr_tempo ) {
			mtr_tempo = rcd->panel_tempo;
			B_LOCATE( 58,12 );B_PRINT(fstr(mtr_tempo,3));
		}
top:
		a=(*rcd->md_get)();if(a==0xfc){break;}

		if(ecount_sw){if(a==0xf8){ecount+=ecst;}
			stc=ecount;}else{stc=rcd->stepcount;}

		if(stc>=lastst){break;}
start:
		if(a>=0 && a<0xf8){
			if(a==0xf5){ccc=0;
next0:
				b=(*rcd->md_get)();
				if(b<0||b>0xf7){
					if( INPOUT(0xfe)!=0 ){
						if(INPOUT(0xff)==27){break;}
					}
					goto next0;
				}
				goto top;
			}

			if(ccc==0 && a<128){goto next;}

			if(ccc==0xf0 || ccc==0xf7){
				if(a<0x80){
					dat[ad++]=a;mbuf[bc++]=a;
					goto top;
				}else{
					dat[ad++]=0xf7;mbuf[bc++]=0xf7;mbuf[bc++]=0xff;
					ccc=0;
					if(rfilt[10][0]==0){
						if(rec_putmd!=0){
							(*rcd->mix_out)(mbuf);
						}
					}
					if(a==0xf7){goto next;}
					goto dtm;
				}
			}else{
				if(a==0xf7){goto top;}
dtm:
				put_delta(ad,stc-co);co=stc;ad+=d_byte;
			}
			if(a>127){
				if(ad>0 && dat[ad-1]>=0x80 && dat[ad-1]<0xf8){ad--;}
				ccc=a;dat[ad++]=a;
				if(a==0xf0 || a==0xf7){mbuf[0]=a;bc=1;}
next3:
				a=(*rcd->md_get)();
				if(a<0 || a>0xf7){
					if( INPOUT(0xfe)!=0 ){
						if(INPOUT(0xff)==27){break;}
					}
					goto next3;
				}
			}

			if(ccc!=0 && a<128){
				int ct=(ccc&0xf0);
				switch( ccc & 0xf0 ) {
				case 0x80 :
				case 0x90 :
				case 0xa0 :
				case 0xb0 :
				case 0xe0 :
					dat[ad++]=a;
next2:
					b=(*rcd->md_get)();
					if(b<0||b>0xf7){
						if( INPOUT(0xfe)!=0 ){
							if(INPOUT(0xff)==27){break;}
						}
						goto next2;
					}
					if(b>127){a=b;ccc=a;dat[ad++]=0;goto start;}
					dat[ad++]=b;

					if(ct==0x90 || ct==0x80){
						if(rfilt[12][0]!=0){
							a+=(rfilt[12][1]-128);
							if(a>127 || a<0){break;}
							dat[ad-2]=a;
						}
					}
					if(ct==0x90 && b){
						if(rfilt[11][0]!=0){
							if(a>rfilt[11][1] || a<rfilt[11][2]){break;}
						}
						if(rfilt[13][0]!=0){
							b=rfilt[13][1];dat[ad-1]=b;
						}
					}
					if(ct==0xa0 && rfilt[3][0]!=0){break;}
					if(ct==0xb0){
						a=ctrl_tbl[a];dat[ad-2]=a;

						if(a==64 ){if(rfilt[4][0]!=0){break;}else{goto skip;}}
						if(a==1  ){if(rfilt[5][0]!=0){break;}else{goto skip;}}
						if(a==7  ){if(rfilt[6][0]!=0){break;}else{goto skip;}}
						if(a==11 ){if(rfilt[7][0]!=0){break;}else{goto skip;}}
						if(a==123){if(rfilt[8][0]!=0){break;}else{goto skip;}}
						if(rfilt[9][0]!=0){break;}
					}
skip:
					if(ct==0xe0 && rfilt[1][0]!=0){break;}
					if(rec_putmd!=0){mix_two(ccc,a,b);}
					break;
				case 0xc0 :
				case 0xd0 :
					dat[ad++]=a;
					if(ct==0xc0 && rfilt[0][0]!=0){break;}
					if(ct==0xd0 && rfilt[2][0]!=0){break;}
					if(rec_putmd!=0){mix_one(ccc,a);}
					break;
				case 0xf0 :
				case 0xf7 :
					dat[ad++]=a;mbuf[bc++]=a;
					break;
				}
			}
next:
			if(ad+512>=work_size){msg("レコーディングバッファが一杯になりました");break;}
		}
		if( INPOUT(0xfe)!=0 ){
			int k=INPOUT(0xff);
			if(k==27){break;}
			if(k=='m' || k=='M'){rec_mt_change(1);}

			if(k==0x17){if( rcd->tempo > 1 ) {rcd->tempo--;}}
			if(k==0x12){if( rcd->tempo < 255 ) {rcd->tempo++;}}
			if(k==0x15){rcd->tempo = rcd->basetempo;}
		}
	}
	if(ecount_sw){stc=ecount;}else{stc=rcd->stepcount;}
	if(stc>=lastst){stc=lastst;}

	stc-=co;put_delta(ad,stc);ad+=d_byte;
	dat[ad++]=0xff;dat[ad++]=0x2f;
	(*rcd->end)();

	B_LOCATE(1,6);B_PRINT("RECORDING 終了     ");

	if(stm==0 && topmeas==1){stc-=barlen*2;}
	B_PRINT("Total ST=");B_PRINT(fstr(stc+co,8));
/*
	B_PRINT("    Used Size=");B_PRINT(fstr(ad,6));B_PRINT("byte");
*/

	B_LOCATE(1,8);B_PRINT("CONVERT中・・・    ");
	if(stc+co>0){lastptr=ad;rec_conv(mm);}
	B_LOCATE(16,8);B_PRINT("終了    ");
	B_LOCATE(1,10);B_PRINT("何かキーを押して下さい。");

	while( INPOUT(0xff)==0){}

ext:
	B_CLR_AL();home2(0);
	rcd->put_mode=pm;

	return(0);
}

/***************************/
inline void	mix_one(int c,int a)
{
	char	mbuf[3];

	mbuf[0]=c;mbuf[1]=a;mbuf[2]=0xff;
	(*rcd->mix_out)(mbuf);
}

static inline void	mix_two(int c,int a,int b)
{
	char	mbuf[4];

	mbuf[0]=c;mbuf[1]=a;mbuf[2]=b;mbuf[3]=0xff;
	(*rcd->mix_out)(mbuf);
}

/***************************/
inline void	rec_ch_disp()
{
	int	c;

	c=rec_getch;
/*	if(rcd->ch_port[c]!=rcd->put_mode){c+=16;}
	B_LOCATE(16,31);B_PRINT(chstr(c+1));*/
	B_LOCATE(16,31);B_PRINT(fstr(c+1,2));

	B_LOCATE(30,31);if(rec_putmd==0){B_PRINT("OFF ");}else{B_PRINT("THRU");}
	B_LOCATE(44,31);if(metoro_sw==0){B_PRINT("ON  ");}else{B_PRINT("OFF ");}
	B_LOCATE(54,31);if(ecount_sw==0){B_PRINT("INT ");}else{B_PRINT("EXT ");}

	B_LOCATE(45-10,12);B_PRINT(fstr(topmeas,4));
	B_LOCATE(45-10+5,12);B_PRINT(fstr(btmmeas,4));

}

/***************************/
inline void	rec_ch_change(int pm)
{
	int	c;

	msg("RECORDING MIDI CH.=");

	c=vinput(rec_getch+1,0x802);if(c>0 && c<17){rec_getch=c-1;}
	msg_clr();rec_ch_disp();

	if(rec_getch>=0){rcd->put_mode=rcd->ch_port[rec_getch];}else{rcd->put_mode=pm;}
}

/***************************/
inline void	rec_md_change()
{
	rec_putmd=1-rec_putmd;
	rec_ch_disp();
}

/***************************/
inline void	rec_mt_change(int md)
{
	int a;

	a=metoro_sw;
	if(metoro_trk>=0){
		a=1-a;metoro_sw=a;
		if(md){rcd->trk_mask[metoro_trk]=metoro_sw;}
	}
	rec_ch_disp();
}


/***************************/
/*
inline void	rec_rm_change()
{
	int a;

	a=rcd->midi_clock;
	a=1-a;rcd->midi_clock=a;
	rec_ch_disp();
}
*/
/***************************/
inline void	rec_sy_change()
{
	ecount_sw=1-ecount_sw;
	rec_ch_disp();
}

/***************************/
inline void	rec_meas_change(int pm)
{
	int	c;

p1:
	msg("RECORDING START MEAS NO.=");

	c=vinput(topmeas,0x804);
	if(c<1 && c>999){goto p1;}
	if( es!=0 ){msg_clr();return;}
	topmeas=c;
	rec_ch_disp();
p2:
	B_LOCATE(32,1);B_PRINT("END MEAS NO.=");

	c=vinput(btmmeas,0x804);
	if(c<topmeas && c>999){goto p2;}
	if( es!=0 ){goto p1;}
	btmmeas=c;
	rec_ch_disp();

	msg_clr();
}

/***************************/
int	countstart(int md,int smeas)
{
	int	bpo,met=-1,pr=0;
	int	me_ad[2048];

	static	unsigned char	metp[4*14]={
		0xe6,0,16,0, 0xec,0,0,0,   0xf9,0,0,0,  0xf9,0,0,0,  0xfd,0,0,0,
		64,48,2,127, 0,0,0,0,      0xf9,0,0,0,  64,48,2,117, 0,0,0,0,
		0xf8,3,0,0,  0xf8,128,0,0, 0xf8,16,0,0, 0xfe,0,0,0};

	static	unsigned char	restbar[4*10]={
		0xf9,0,0,0, 64,48,0,0, 0,0,0,0, 0xf8,4,0,0, 0xfd,0,0,0,
		0xf9,0,0,0, 64,48,0,0, 0,0,0,0, 0xf8,4,0,0, 0xfd,0,0,0};


	metp[2]=rec_met[0];/*metp[6]=rec_met[1];*/
	if(rec_met[1]>=0){metp[4]=0xec;metp[6]=rec_met[1];}else{metp[4]=0;metp[6]=0;}

	metp[20]=rec_met[5];metp[21]=barlen/beat1;metp[22]=rec_met[6];metp[23]=rec_met[7];
	metp[32]=rec_met[2];metp[33]=barlen/beat1;metp[34]=rec_met[3];metp[35]=rec_met[4];
	metp[41]=beat1-1;if(beat1==1){metp[41]=1;metp[33]=0;metp[34]=0;metp[35]=0;}

	restbar[5]=barlen/beat1;restbar[25]=barlen/beat1;
	metp[25]=0;metp[37]=0;restbar[9]=0;restbar[29]=0;

	if(barlen/beat1>240){metp[25]=255;metp[37]=255;restbar[9]=255;restbar[29]=255;}


	all_note_off(1);hedset(36);bpo=rcp_buf_put(hed,0,1414);

	if(smeas<2){
		int i;
		poft=0;

		for(i=0 ;i< 36;i++){
			if((trmod[i]&1)!=0 || tr_len[i]==4){	/*mute & rec*/
				if(met<0){
					thedset(0,4*14+44);hed[5]=0x80;hed[6]=0;hed[7]=0;
					bpo=rcp_buf_put(hed,bpo,44);bpo=rcp_buf_put(metp,bpo,4*14);
					met=i;
				}else{
					thedset(i,48);dat[0]=0xfe;
					bpo=rcp_buf_put(hed,bpo,44);bpo=rcp_buf_put(dat,bpo,4);
				}
			}else{					/*play & mix*/

				int	ln=0;
				if(md!=0){ln=4*10;}
				same_cluc(i,ln);thedset(i,tr_len[i]+44+ln);
				bpo=rcp_buf_put(hed,bpo,44);
				if(md!=0){bpo=rcp_buf_put(restbar,bpo,4*10);}
				bpo=rcp_buf_put(trk[i],bpo,tr_len[i]);
/*
				if(md==0){
					same_cluc(i,0);thedset(i,tr_len[i]+44);
					bpo=rcp_buf_put(hed,bpo,44);
				}else{
					same_cluc(i,4*10);thedset(i,tr_len[i]+44+4*10);
					bpo=rcp_buf_put(hed,bpo,44);bpo=rcp_buf_put(restbar,bpo,4*10);
				}
				bpo=rcp_buf_put(trk[i],bpo,tr_len[i]);
*/
			}
		}
	}else{
		int i,po,meas=smeas,dumy=track,tempo;

		po=meas_add(meas);pr=lsp_wait_chack(meas);tempo=last_tempo(meas);
		poft=meas-1;if(po>0){poft+=0x10000;}

		for(i=0 ;i< 36;i++){
			if((trmod[i]&1)!=0 || tr_len[i]==4){	/*mute & rec*/
				if(met<0){
					int	ln=0,ln2=0;

					if(md){ln2=meas_adj_sub(0,pr);ln+=ln2;ln+=4*14;}

					thedset(0,ln+44);hed[5]=0x80;hed[6]=0;hed[7]=0;
					bpo=rcp_buf_put(hed,bpo,44);
					if(ln2){bpo=rcp_buf_put(dat,bpo,ln2);}
					bpo=rcp_buf_put(metp,bpo,4*14);
					met=i;

				}else{
					thedset(i,48);dat[0]=0xfe;
					bpo=rcp_buf_put(hed,bpo,44);bpo=rcp_buf_put(dat,bpo,4);
				}
			}else{					/*play & mix*/
				int co=0,ad,j;

				track=i;ad=meas_add(meas);if(ad<0){ad=tr_len[i]-4;}
				add_set(i,me_ad);
				co=lsp_para_set(i,ad,me_ad,pr);
				if(po>0){
					if( tempo>=0){
						dat[co]=0xe7;dat[co+1]=1;dat[co+2]=tempo;dat[co+3]=0;
						co=co+4;
					}
					dat[co]=0xfd;co=co+4;
				}
				j=ad;
				while(1){
					dat[co]=trk[i][j];dat[co+1]=trk[i][j+1];
					dat[co+2]=trk[i][j+2];dat[co+3]=trk[i][j+3];
					if(dat[co]==0xfc){
						int	same,ad;
						same=dat[co+1]+(dat[co+2]&3)*256;
						if(meas<(same+1)){
							if(po>0){same++;}
							ad=dat_add(co+4,same+2-meas);
							if(ad>co){goto sameext;}
							ad+=44;dat[co+2]=ad&0xff;dat[co+3]=ad>>8;
						}else{
sameext:
							ad=me_ad[same];
							while(trk[i][ad]<0xfc && co<work_size){
								dat[co]=trk[i][ad];dat[co+1]=trk[i][ad+1];
								dat[co+2]=trk[i][ad+2];dat[co+3]=trk[i][ad+3];
								co=co+4;ad=ad+4;
							}
							dat[co]=0xfd;
						}
					}
					if(dat[co]==0x98){
						while(trk[i][j+4]==0xf7 && co<work_size){
							co+=4;j+=4;
							dat[co]=trk[i][j];dat[co+1]=trk[i][j+1];
							dat[co+2]=trk[i][j+2];dat[co+3]=trk[i][j+3];
						}
					}
					if( dat[co]==0xfe || co+4>=work_size ){break;}
					co=co+4;
					if(trk[i][j]<0xfc && trk[i][j+4]==0xfc){dat[co]=0xfd;co=co+4;}
					j=j+4;
				}
				dat[co]=0xfe;co=co+4;

				/*same_cluc(i,0);*/thedset(i,co+44);
				bpo=rcp_buf_put(hed,bpo,44);
				bpo=rcp_buf_put(dat,bpo,co);
			}
		}
		track=dumy;
	}

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return 0;}

	rcd->fmt=2;rcd->totalcount=bpo;
	rcd->data_valid=1;rcd->word_valid=0;rcd->tone_valid=0;rcd->gsd_valid=0;
/*	strcpy(rcd->filename,"");strcpy(rcd->tonename,"");strcpy(rcd->gsdname,"");*/

	(*rcd->init)();

	if(cm6_file[0]==0 && gsd_file[0]==0){
		if(init_exc_data[0]==255){(*rcd->setup)();
		}else{(*rcd->mix_out)(init_exc_data);twait(5);}
	}

	metoro_trk=met;
	return pr;
}

/***************************/
unsigned char	*strncpy0( unsigned char *dst, unsigned char *src, int n )
{
	int i;

	for( i = 0; i < n; i++ ) {dst[i] = src[i];}
	dst[n] = 0;return( dst );
}

int get_delta( int p )
{
	register int val, i, d;

	val = 0;d_byte = 0;

	for( i = 0; i < 4; i++ ) {
		d = dat[p++];d_byte++;
		val = ( val << 7 ) + ( d & 0x7f );
		if( d < 0x80 ) {return( val );}
	}

	return(0);
}

void put_delta( int p ,int val)
{
/*	register long buffer;*/
	register unsigned int buffer;

	d_byte=0;

	buffer = val & 0x7f;
	while((val >>= 7) > 0){
		buffer <<= 8;buffer |= 0x80;
		buffer += (val & 0x7f);
	}
	while(1){
		dat[p++]=buffer & 0xff;d_byte++;
		if(buffer & 0x80){buffer >>= 8; }else{ break;}
	}
}

void entry( int d0, int d1, int d2, int d3 )
{
	if( len+4 >= TRACK_SIZE ){msg("トラックサイズを越えました。" );return;}

	note[len++] = d0;note[len++] = d1;note[len++] = d2;note[len++] = d3;

}

void flash( int dt )
{
	int dx, i, j;

	while( dt > 0 ) {
		if( dt <= 0xf0 ) {dx = dt;} else {dx = 0xf0;}
		if( dx + barunit > barlen ) {dx = barlen - barunit;}

		for( i = 0; i < 16; i++ ) {
			if( gbuf[i] ) {
				if( gbuf[i] <= 0xf0 ) {
					entry( nbuf[i], 0, gbuf[i], vbuf[i] );
					for( j = i; j < 15; j++ ) {
						nbuf[j] = nbuf[j+1];
						vbuf[j] = vbuf[j+1];
						gbuf[j] = gbuf[j+1];
					}
					nbuf[15] = vbuf[15] = gbuf[15] = 0;
					i--;
				} else {
					entry( nbuf[i], 0, dx + 1, vbuf[i] );
					gbuf[i] -= dx;
				}
			} else {
				break;
			}
		}

		if( note[ len - 4 ] !=0xde &&
		    note[ len - 4 ] < 0xf0 && ( note[ len - 3 ] + dx ) <= 0xf0 ) {
			note[ len - 3 ] += dx ;
		} else {

			if( note[ len - 4 ] == 0xf7 ){
				int cc=len-4;
				while(note[cc]==0xf7){cc-=4;}

				if( ( note[ cc + 1 ] + dx ) <= 0xf0 ) {
					note[ cc + 1 ] += dx ;
				} else {
					entry( 0x00, dx, 0, 0 );
				}
			} else {
				entry( 0x00, dx, 0, 0 );
			}
		}

		dt -= dx;

		if( ( barunit += dx ) == barlen ) {
			entry( 0xfd, 0x00, 0x00, 0x00 );
			barunit = 0;
		}
	if( len+4 >= TRACK_SIZE ){break;}

	}

	stepsum = 0;
}

void proc_dummy( int dt )
{
	stepsum += dt;
}

void check( int data )
{
	cmd = data & 0xf0;

	data &= 0x0f;
	if( midi_ch != data ) {midi_ch = data;}
}

int search( int pp )
{
	int n, dt, cc, ch, ccmd, last;

	n = dat[pp];
	ccmd = cmd;
	last = lastptr;
	pp += 2;
	dt = 0;
	ch = midi_ch;

	while( pp < last ) {
		dt += get_delta( pp );
		pp += d_byte;
		if( ( cc = dat[pp] ) < 0x80 ) {
			if( ccmd == 0x80 && cc == n && ch == midi_ch ) {
				return( dt );
			} else {
				if( ccmd == 0x90 && cc == n &&
				    dat[pp+1] == 0x00 && ch == midi_ch ) {
					return( dt );
				} else {
					pp += 2;
					if(ccmd>=0xc0 && ccmd<=0xdf){pp--;}
				}
			}
		} else {
			switch( cc & 0xf0 ) {
			case 0x80 :
				if( dat[pp+1] == n && cc == ( 0x80 + midi_ch ) ) {
					return( dt );
				}
				ch = cc & 0x0f;
				pp += 3;
				break;
			case 0x90 :
				if( dat[pp+1] == n && dat[pp+2] == 0x00 &&
				   cc == ( 0x90 + midi_ch ) ) { 
					return( dt );
				}
				pp += 3;ch = cc & 0x0f;break;
			case 0xa0 :
			case 0xb0 :
			case 0xe0 :
				pp += 3;ch = cc & 0x0f;break;
			case 0xc0 :
			case 0xd0 :
				pp += 2;ch = cc & 0x0f;break;
			case 0xf0 :
				switch( cc ) {
				case 0xf0 :
				case 0xf7 :
					pp += 1;while(dat[pp++]!=0xf7){}
					break;
				case 0xff :
					return(0);
					break;
				default :
					B_PRINT("フォーマットが異常です。\r\n" );
					return(0);
					break;
				}
				break;
			}
			ccmd = cc & 0xf0;
		}
	}
	return( 0 );
}

void ent_dat( int nt, int vl, int gt )
{
	int i;

	if( gt <= 0xf0 ) {
		entry( nt, 0, gt,vl );return;
	} else {
		entry( nt, 0, 255,vl );
	}

	for( i = 0; i < 16; i++ ) {
		if( !gbuf[i] || nbuf[i] == nt ) {
			nbuf[i] = nt;vbuf[i] = vl;gbuf[i] = gt;return;
		}
	}

	for( i = 0; i < 15; i++ ) {
		nbuf[i] = nbuf[i+1];vbuf[i] = vbuf[i+1];gbuf[i] = gbuf[i+1];
	}

	nbuf[15] = nt;vbuf[15] = vl;gbuf[15] = gt;
}

void note_off( int p )
{
	/* do nothing */
}

void note_on( int p )
{
	int gt;

	if( midi_ch != trch ) {return;}

	if(rfilt[11][0]!=0){
		if(dat[p]>rfilt[11][1] || dat[p]<rfilt[11][2]){return;}
	}

	flash( stepsum );

	if( dat[p+1] ) {
		if( !( gt = search( p ) ) ) {return;}
		ent_dat( dat[p], dat[p+1], gt );
	}
}

void aft_key( int p )
{
	if( midi_ch != trch  ) {return;}
	if(rfilt[3][0]!=0){return;}
	flash( stepsum );
	entry( 0xea, 0x00, dat[p], dat[p+1] );
}

void ctl_chg( int p )
{
	if( midi_ch != trch ) {return;}

	if(dat[p]==64 ){if(rfilt[4][0]!=0){return;}else{goto skip;}}
	if(dat[p]==1  ){if(rfilt[5][0]!=0){return;}else{goto skip;}}
	if(dat[p]==7  ){if(rfilt[6][0]!=0){return;}else{goto skip;}}
	if(dat[p]==11 ){if(rfilt[7][0]!=0){return;}else{goto skip;}}
	if(dat[p]==123){if(rfilt[8][0]!=0){return;}else{goto skip;}}
	if(rfilt[9][0]!=0){return;}
skip:
	flash( stepsum );
	entry( 0xeb, 0x00, dat[p], dat[p+1] );
}

void prg_chg( int p )
{
	if( midi_ch != trch ) {return;}
	if(rfilt[0][0]!=0){return;}
	flash( stepsum );
	entry( 0xec, 0x00, dat[p], 0x00 );
}

void aft_tch( int p )
{
	if( midi_ch != trch  ) {return;}
	if(rfilt[2][0]!=0){return;}
	flash( stepsum );
	entry( 0xed, 0x00, dat[p], 0x00 );
}

void pt_bend( int p )
{
	if( midi_ch != trch  ) {return;}
	if(rfilt[1][0]!=0){return;}
	flash( stepsum );
	entry( 0xee, 0x00, dat[p], dat[p+1] );
}

void run_sts( int p )
{
	if( midi_ch != trch  ) {return;}
	flash( stepsum );

	switch( cmd ) {
	case 0x80 : note_off(p); break;
	case 0x90 : note_on(p);  break;
	case 0xa0 : aft_key(p);  break;
	case 0xb0 : ctl_chg(p);  break;
	case 0xc0 : prg_chg(p);  break;
	case 0xd0 : aft_tch(p);  break;
	case 0xe0 : pt_bend(p);  break;
	}
}

int num_check( int num, int limit )
{
	if( num > limit ) {return( limit );} else {return( num );}
}

void proc_excl( unsigned char *text , int length,int code)
{
	int h, m, l, hh, mm, ll, i;

	if(rfilt[10][0]!=0){return;}
	flash( stepsum );

	text[ length ] = 0xf7 ;

	if( length < 9 || length>8+4 ||
	    text[0] != 0x41 || text[3] != 0x12 ||
	    text[ length-1 ] != 0xf7 ) {

		if( text[0] == 0x41 && text[3] == 0x12 && length<9 ){return;}

		if(code==0xf0){
nor:
			i=0;
			entry( 0x98, 0, 0, 0 );
			while(i<length){
				entry( 0xf7, 0, text[i], text[i+1] );
				if( text[i] == 0xf7 || text[ i+1 ] == 0xf7 ) {break;}
				i=i+2;
			}
		}else{
			if(len==0 || note[len-4]!=0xf7){goto nor;}
			i=0;

			if(note[len-2]==0xf7){note[len-2]=text[i++];}
			if(note[len-1]==0xf7){note[len-1]=text[i++];}

			while(i<length){
				entry( 0xf7, 0, text[i], text[i+1] );
				if( text[i] == 0xf7 || text[ i+1 ] == 0xf7 ) {break;}
				i=i+2;
			}
		}
		return;
	}

	if( exc_no != text[1] || exc_id != text[2] ) {
		exc_no = text[1];exc_id = text[2];
		entry( 0xdf, 0, exc_no, exc_id );
	}

	h = text[4]; m = text[5]; l = text[6];
	hh = mm = ll = -1;

	i=7;
	while(text[i]!=0xf7 && text[i+1]!=0xf7 && i<512){
		if( hh != h || mm != m  ) {
			entry( 0xdd, 0x00, h, m );
			hh = h; mm = m; ll = l;
		}
		entry( 0xde, 0x00, l, text[i] );
		if( l++ == 0x7f ) {l = 0;m++;}
		i++;
	}
}

void proc_next( void )
{
	int meta, code,i;
	unsigned char *text=rcpbuf;

	for(i=0;i<16;i++){nbuf[i] = vbuf[i] = gbuf[i] = 0;}
	pt = 0;len = 0;barunit = 0; stepsum = 0;
	exc_no=0x10;exc_id=0x16;

	cmd = 0x00;
	step = 0;

	while(1) {
		if( pt >= lastptr || len+4 >= TRACK_SIZE ){break;}

		proc_dummy( get_delta(pt) );
		pt += d_byte;

		if( ( code = dat[pt] ) == 0xff ) {
			pt++;meta = dat[pt++];
			if( tr == 0 ) {	flash( stepsum );}
			pt += 1;
			if( meta == 0x2f ) {return;}
		} else {
			switch( code & 0xf0 ) {
			case 0x80: check( dat[pt] ); note_off(pt+1); pt += 3; break;
			case 0x90: check( dat[pt] ); note_on(pt+1);  pt += 3; break;
			case 0xa0: check( dat[pt] ); aft_key(pt+1);  pt += 3; break;
			case 0xb0: check( dat[pt] ); ctl_chg(pt+1);  pt += 3; break;
			case 0xc0: check( dat[pt] ); prg_chg(pt+1);  pt += 2; break;
			case 0xd0: check( dat[pt] ); aft_tch(pt+1);  pt += 2; break;
			case 0xe0: check( dat[pt] ); pt_bend(pt+1);  pt += 3; break;
			case 0xf0:
				if( code != 0xf0 && code != 0xf7 ) {
					B_PRINT("不正MIDIメッセージ\r\n");
					return;
				}
				pt++;
				if( ( tr == 0 ) ) {
					int ll=1,pt2=pt;
					while(dat[pt++]!=0xf7){ll++;}
					strncpy0( text, dat + pt2, ll);
					proc_excl( text,ll ,code);
				}
				break;
			default :  run_sts(pt); pt += 2;
				if(cmd>=0xc0 && cmd<=0xdf){pt--;}
				break;
			}
		}
	}
}

void track_end( void )
{
	flash( stepsum );
	entry( 0xfe, 0, 0, 0 );
}

void rec_conv( int mm )
{
	int i;

	memcpy_l(dat2,dat,lastptr);

	size_ref();
/*	B_PRINT("ch.:");*/

	if(mm==1){
		tr=0;trch=midi_ch = rec_getch ;
		/*B_PRINT(fstr(tr+1,3));*/
		proc_next();track_end();

		for(i=0;i<36;i++){
			if(trmod[i]&2){
				memcpy_l(dat,dat2,lastptr);
				if(rec_ext(i)){break;}
			}
		}
	}else{
		for(tr=0;tr<16;tr++){
			trch=midi_ch = tr;
			/*B_PRINT(fstr(tr+1,3));*/
			proc_next();track_end();

			for(i=0;i<36;i++){
				if((trmod[i]&2)!=0 && tr==((mch[i]-1)&15)){
					memcpy_l(dat,dat2,lastptr);
					if(rec_ext(i)){break;}
				}
			}
		}
	}
}

/***************************/
int	rec_ext(int i)
{
	int ln,ad=0,ad2=0,ln2=0,dumy=track;
	track=i;

	ln=trk_ext_sub(i,rcpbuf);if(ln==0){return 1;}

	if(size_change(i,ln)){return 1;}
	memcpy_l(trk[i],rcpbuf,ln);tr_len[i]=ln;

	if(topmeas>1){
		/*ad=meas_add(topmeas);if(ad<0){ad=tr_len[i]-4;}*/
		ad=meas_adjust(i,topmeas);
	}


	if(trmod[i]==2 && tr_len[i]>4){
		ln=trk_mix_sub(rcpbuf+ad,note);
		if(size_change(i,ad+ln+4)){return 1;}

		dat[ln]=0xfe;ln+=4;tr_len[i]=ad+ln;
		memcpy_l(&trk[i][ad],dat,ln);
	}else{
		ad2=meas_add(btmmeas+1);if(ad2<0){ad2=tr_len[i]-4;}

		ln2=tr_len[i]-ad2;
		memcpy_l(rcpbuf,&trk[i][ad2],ln2);
		if(ln2<=4){ln2=0;}

		if(size_change(i,ad+len+ln2)){return 1;}

		tr_len[i]=ad+len;
		memcpy_l(&trk[i][ad],note,len);

		if(ln2>4){
			ad2=ad+len-4;
			tr_len[i]=ad+len+ln2;
			memcpy_l(&trk[i][ad2],rcpbuf,ln2);
		}

	}

	tai_compres(i,ad,tr_len[i]);
	trksize(i);
	track=dumy;

	return 0;
}

int	meas_adjust(int i,int topm)
{
	int	ad=0,ln=0,lft=0,dumy=track;track=i;

	if(topm>1){
		ad=meas_add(topm);
		if(ad<0){
			ad=tr_len[track]-4;
			lft=topm-meas_no(ad);

			if(step_cluc(ad)!=0){
				lft--;ln=meas_adj_sub(ln,barlen-step_cluc(ad));
			}
			while(lft>0){lft--;ln=meas_adj_sub(ln,barlen);}
			dat[ln]=0xfe;ln+=4;

			if(size_change(track,ad+ln)){return -1;}

			tr_len[track]=ad+ln;
			memcpy_l(&trk[track][ad],dat,ln);
		}
		ad=meas_add(topm);if(ad<0){ad=tr_len[track]-4;}

	}
	track=dumy;
	return ad;
}

int	meas_adj_sub(int ln,int st)
{
	int ss=st;

	while(ss>240){ss-=240;dat[ln++]=0;dat[ln++]=240;dat[ln++]=0;dat[ln++]=0;}
	dat[ln++]=0;dat[ln++]=ss;dat[ln++]=0;dat[ln++]=0;
	dat[ln++]=0xfd;dat[ln++]=0;dat[ln++]=0;dat[ln++]=0;

	return ln;
}

/***************************/
