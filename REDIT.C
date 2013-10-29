/*
 * STed: redit.c (rhythm track edit)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"graph.h"
#include	"sted.h"

static char	rym[10][3]={"- ","1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 "};
static char	rym_s[10][4]={"OFF"," 1 "," 2 "," 3 "," 4 "," 5 "," 6 "," 7 "," 8 "," 9 "};
/*
static char	rym0[10][3]={"- ","1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 "};
static char	rym1[10][3]={"ìj","ìk","ìl","ìm","ìn","ìo","ìp","ìq","ìr","ìs"};
static char	rym[10][3];
*/

static unsigned char	rpat[64][128];
static unsigned char	rpar[65][5];
static 		int	radr[65];
static char	vlcl[256];
static int	dfstep=12,barstep,dfvel=9;

void	H_PRINT();
void	H_PRINT2();
int	vinput();
void	sinput();
void	spcut();
void	vdis();
void	sdis();
void	tcury();
char	*fstr();
char	*spadd();
void	fnc_dis();
void	msg();
void	msg_clr();
void	timtra();
void	all_note_off();
int	ctc();
void	dplay();
void	cdplay();
void	cntplay();
void	trk_free();
void	snsclr();
void	twait();

void	rtrk_ed();
void	rtrk_dis();
void	rhed_dis();
void	rsttrc();
char	*trk_dis();
void	trk_cluc();
int	step_cluc();
int	step_cluc2();
int	meas_add();
int	meas_no();
int	meas_len();
void	rhy_step();
void	rtopat();
void	pattor();
void	poplay();
int	trk_check();
int	trk_shift();
void	trk_delete();
void	same_shift();

/***************************/
/* rhythm track edit */
void rtrk_ed(int md,int sh)
{
	int	no,pg=0,ke,kc,cx,cy,i,j,py,yy,rc=5999;
	int	msf=-1;

	barstep = tbase * 4 * beat1 / beat2;

	if(sh&1){rhy_step();}

/*	if(cmdflag&0x20000){
		for(i=0;i<10;i++){strcpy(rym[i],rym1[i]);}
	}else{
		for(i=0;i<10;i++){strcpy(rym[i],rym0[i]);}
	}
*/
	for(i=0;i<256;i++){vlcl[i]=0;}
	j=1;
	for(i=1;i<10;i++){for(j=j;j<=rhy_vel[i];j++){vlcl[j]=i;}}
	for(i=j;i<128;i++){vlcl[j]=9;}

	no=meas_no(tr_pos[track][edit_scr&1][0]+tr_pos[track][edit_scr&1][2]*4);

	if(tr_len[track]>4){
		if(md!=0 && trk_check(4)==0){
			int	i;
			tr_len[track]=tr_len[track]+4;i=tr_len[track]-8;
			trk[track][i]=0xfd;trk[track][i+1]=0;trk[track][i+4]=0xfe;
			no=meas_no(i);
		}
	}
	edit_scr=edit_scr|2;edfield(0);trk_no();
	disp_vels();

	cx=0;cy=0;pg=trrhy[track]&31;if(pg>16){pg=16;}
	rtopat(no);rhed_dis(pg);rtrk_dis(pg,0,32);
if((cmdflag&0x200)==0){
	MS_INIT();MS_LIMIT(126,80,762,511-32);MS_CURON();
}
	while( 1){
		int ma=0,mx,my;
top:
		yy=0;if(cy==16){yy=16+(cx&1)*16;}
		py=cy+pg;
		if((B_SFTSNS()&16)!=0){rsttrc(&no,&cx,0,pg);}
/*
		fill(128+32,cy*16+655+yy,31*16+151+32,cy*16+655+yy,15);
		fill(cx*16+144+32,640,cx*16+144+32,15*16+655+50,15);
*/
		txxline(128+32,cy*16+143+yy,65*8,0,65535);
		txyline(cx*16+144+32,128,16*16+48,0,65535);

		B_LOCATE(8,26);if(cy<16 && rpar[cx][0]==1){B_PRINT(fstr(rpat[cx][py],3));}else{B_PRINT("   ");}
		B_LOCATE(8,27);if(cy<16 && rpar[cx][0]==1){
			if(rpat[cx][py]!=0){B_PRINT(fstr(rpat[cx][py+32],3));}else{B_PRINT("  0");}
		}else{B_PRINT("   ");}

		while(B_KEYSNS()==0){
			/*ma=MS_GETDT()&0xff00;mx=MS_CURGT();my=mx&0xffff;mx=mx>>16;*/
			ma=MS_GETDT()&0xff00;mspos(&mx,&my);
			if(ma==0){msf=-1;}
			if(ma && msf!=0){break;}

			rc++;
			if( rc==6000){
				sh=B_SFTSNS();
				/*if((sh&1)!=0){fnc_dis(1);}else{fnc_dis(0);}*/
				trk_free(meas_add(no));
			}

		}


		ke=0;kc=0;sh=0;
		if(B_KEYSNS()!=0){
			ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;sh=B_SFTSNS();
		}
		if((ke>=0x70 && ke<=0x73)||(ke>=0xf0 && ke<=0xf3)){rc=6000;}else{rc=0;}
/*
		fill(128+32,cy*16+655+yy,31*16+151+32,cy*16+655+yy,0);
		fill(cx*16+144+32,640,cx*16+144+32,15*16+655+50,0);
*/
		txxline(128+32,cy*16+143+yy,65*8,0,0);
		txyline(cx*16+144+32,128,16*16+48,0,0);

		if(ma!=0 && msf!=0){
			ke=0;kc=0;msf=0;
			if(mx>=50*8 && mx<50*8+30*8 && my>=80 && my<96 ){
				dfvel=(mx-50*8)/24;
				disp_vels();
				goto top;
			}
			if(mx>=20*8+4 && mx<20*8+64*8+4 && my>=8*16 && my<8*16+16*16 ){
				cx=(mx-(20*8+4))>>4;cy=(my-8*16)>>4;
				py=cy+pg;
				kc=0x30+dfvel;
			}
		}

/*		if( ke==0x70 ){fnc_dis(1);}
		if( ke==0xf0 ){fnc_dis(0);}*/

		if( ke==0x5a ){
			if((sh&16)!=0){pattor(no);rsttrc(&no,&cx,0,pg);}
		}

		if( ke==1 ){pattor(no);break;}

		if(rpar[cx][0]<2){
			if( kc==13 ){
				ke=0x3d;
				if(cy==16){
					B_LOCATE(cx*2+19,25+(cx&1));rpar[cx][2]=vinput(rpar[cx][2],0x803);
				}else{
					int	i;
					B_LOCATE(cx*2+20,8+cy);
					i=vinput(rpat[cx][py],0x803);if(i<128){rpat[cx][py]=i;}
					B_LOCATE(cx*2+20,8+cy);B_PRINT("   ");
					rpat[cx][py+32]=rhyno[py][1];
				}
				rpar[cx][0]=1;
				rtrk_dis(pg,cx,1);
			}
			if( kc>0x2f && kc<0x3a){
				if( cy<16 ){
					rpat[cx][py]=rhy_vel[kc-'0'];
					rpat[cx][py+32]=rhyno[py][1];
					if(ke!=0){
						ke=0x3d;
					}
				}else{
					int	a;
					B_LOCATE(cx*2+19,25+(cx&1));a=vinput(kc-'0',3);
					if( es==0 ){ke=0x3d;rpar[cx][2]=a;}
				}
				rpar[cx][0]=1;
				rtrk_dis(pg,cx,1);kc=0;
			}

			if( kc=='+' || kc=='.'|| kc=='>' || kc==' ' ){
				if( cy<16 ){
					int	i;
					i=vlcl[rpat[cx][py]]+1;if(i>9){i=0;}
					rpat[cx][py]=rhy_vel[i];rpat[cx][py+32]=rhyno[py][1];
				}else{
					if(kc=='>'){
						rpar[cx][2]=(rpar[cx][2]+10);
					}else{
						rpar[cx][2]=(rpar[cx][2]+1);
					}
				}
				rpar[cx][0]=1;
				rtrk_dis(pg,cx,1);
			}
			if( kc=='-'|| kc==','|| kc=='<' ){
				if( cy<16 ){
					int	i;
					i=vlcl[rpat[cx][py]]-1;if(i<0){i=9;}
					rpat[cx][py]=rhy_vel[i];rpat[cx][py+32]=rhyno[py][1];
				}else{
					if(kc=='<'){
						rpar[cx][2]=(rpar[cx][2]+246);
					}else{
						rpar[cx][2]=(rpar[cx][2]+255);
					}
				}
				rpar[cx][0]=1;
				rtrk_dis(pg,cx,1);
			}
		}

		if( ke==0x69 && tr_len[track]>4 ){
			int	i,po;
			pattor(no);rcpf=1;po=meas_add(no);rcplen=meas_len(po);
			for(i=0;i<rcplen;i++){rcpbuf[i]=trk[track][po+i];}
			same_shift(po,1,1);trk_delete(po,rcplen);

			if(no==1){break;}

			if(trk[track][po]==0xfe){no=meas_no(tr_len[track]-8);}
			B_CLR_AL();rtopat(no);rtrk_dis(pg,0,32);
		}

		if( ke==0x6a ){
			int	i,po;
			pattor(no);rcpf=1;po=meas_add(no);rcplen=meas_len(po);
			for(i=0;i<rcplen;i++){rcpbuf[i]=trk[track][po+i];}
			rtrk_dis(pg,0,32);
		}
		if( ke==0x6b ){
			if( rcpf>0 ){
				if(trk_check(rcplen)==0){
					int	i,po;
					pattor(no);po=meas_add(no);
					same_shift(po,0,1);trk_shift(po,rcplen);
					for(i=0;i<rcplen;i++){trk[track][i+po]=rcpbuf[i];}
					rtopat(no);
					rtrk_dis(pg,0,32);
				}
			}
		}
		if( ke==0x6c ){
			int	i,j;
			for(i=63;i>cx;i--){
				for(j=0;j<=127;j++){rpat[i][j]=rpat[i-1][j];}
				for(j=0;j<=3;j++){rpar[i][j]=rpar[i-1][j];}
			}
			rtrk_dis(pg,0,32);
		}
		if( ke==0x37 || kc==25 ){
			int	i,j;
			for(j=0;j<=127;j++){rlcbuf[j]=rpat[cx][j];}
			for(j=0;j<=3;j++){rlcbuf[j+128]=rpar[cx][j];}
			for(i=cx;i<=62;i++){
				for(j=0;j<=127;j++){rpat[i][j]=rpat[i+1][j];}
				for(j=0;j<=3;j++){rpar[i][j]=rpar[i+1][j];}
			}
			rtrk_dis(pg,0,32);
		}
		if( (ke==0x3a || kc==12) && rlcbuf[0]<255 ){
			int	i,j;
			for(i=63;i>cx;i--){
				for(j=0;j<=127;j++){rpat[i][j]=rpat[i-1][j];}
				for(j=0;j<=3;j++){rpar[i][j]=rpar[i-1][j];}
			}
			for(j=0;j<=127;j++){rpat[cx][j]=rlcbuf[j];}
			for(j=0;j<=3;j++){rpar[cx][j]=rlcbuf[j+128];}
			rtrk_dis(pg,0,32);
		}

		if( kc==0x13 || ke==0x3b){cx--;
			if(cx<0){
				cx=31;
leftl:
				if(rpar[cx][0]==0 && cx>0){cx--;goto leftl;}
			}
		}
		if( kc==0x04 || ke==0x3d){
			if(rpar[cx][0]==0){cx=0;}else{cx++;if(cx>31){cx=0;}}
		}
		if( kc==0x05 || ke==0x3c){cy--;if(cy<0){cy=16;}}
		if( kc==0x18 || ke==0x3e){cy++;if(cy>16){cy=0;}}

		if( (ke==0x38 || ke==0x39) && (sh&1)!=0){
			pg=16-pg;ke=0;rhed_dis(pg);rtrk_dis(pg,0,32);
		}

 		if( (ke==0x39 || kc==0x12) && no>1 ){
			pattor(no);no=no-1;rtopat(no);rtrk_dis(pg,0,32);
			snsclr();
		}
		if( (ke==0x38 || kc==0x03) && no<meas_no(tr_len[track]-4)-1 ){
			pattor(no);no=no+1;rtopat(no);rtrk_dis(pg,0,32);
			snsclr();
		}

		if( ke==0x63 && no>1 ){
			pattor(no);no=1;cy=0;cx=0;rtopat(no);rtrk_dis(pg,0,32);
		}
		if( ke==0x36 ){
			if((sh&1)==0){cx=0;cy=0;}else{pattor(no);rsttrc(&no,&cx,1,pg);}
		}
		if( ke==0x64 && tr_len[track]>4 ){
			pattor(no);no=meas_no(tr_len[track]-8);rtopat(no);rtrk_dis(pg,0,32);
		}

		if(kc=='Z' || kc=='z'){timtra();}

		if( kc=='M' || kc=='m' || ke==0x55 ){
			int	j;
			pattor(no);j=meas_add(no);
			if( (sh&2)==0 ){if( (sh&1)==0 ){poplay(j,3);}else{poplay(j,4);}
			}else{poplay(j,5);}
		}
		if( kc=='P' || kc=='p' || ke==0x56 ){
			int	j;
			pattor(no);j=meas_add(no);
			if( (sh&1)==0 ){poplay(j,1);}else{poplay(j,2);}
		}
		if( ke==0x57 && (sh&15)==0){rcd->sts=0;}
		if( ke==0x58 && (sh&15)==0){rcd->sts=1;}
		if( ke==0x59 && (sh&15)==0){poplay(0,0);}

		if( ke==0x10 && tr_len[track]>4 ){
			int	j;
			msg("JUMP MEAS=");j=vinput(0,4);
			if( es==0 && j>0 ){
				pattor(no);if( meas_add(j)>=0 ){no=j;}
				rtopat(no);rtrk_dis(pg,0,32);
			}
			msg_clr();
		}

		if( ke==0x53 && trk_check(4)==0){
			int	i;
			if(sh&1){rhy_step();}
			pattor(no);
			tr_len[track]=tr_len[track]+4;i=tr_len[track]-8;
			trk[track][i]=0xfd;trk[track][i+1]=0;trk[track][i+4]=0xfe;
			no=meas_no(i);rtopat(no);rtrk_dis(pg,0,32);
		}
		if( kc=='?' ){
			int i=edit_scr;
			pattor(no);
			B_CLR_AL();cons_md(0);edit_scr=0;
			fnc_dis(-1);rhy_as();edit_scr=i;B_CLR_AL();edfield(0);
			trk_no();rtopat(no);rhed_dis(pg);rtrk_dis(pg,0,32);
			rc=tm_lag-1;fnc_dis(0);
		}

		if( ke==0x54 ){
			B_CLR_AL();/*cons_md(0);*/
			fnc_dis(-1);help(1);B_CLR_AL();edfield(0);
			trk_no();disp_vels();rhed_dis(pg);rtrk_dis(pg,0,32);
			rc=tm_lag-1;fnc_dis(0);
		}

	}
	tr_pos[track][edit_scr&1][0]=meas_add(no);
	tr_pos[track][edit_scr&1][1]=0;tr_pos[track][edit_scr&1][2]=0;
	trrhy[track]=(trrhy[track]&0x80)+pg;trk_lin(track,-1,0);
	edit_scr=edit_scr&1;B_CLR_AL();
if((cmdflag&0x200)==0){
	MS_LIMIT(0,0,767,511);MS_CUROF();
}
}

/***************************/
void	rtrk_dis(int p,int ad,int line)
{
	char	tmp0[256],tmp1[256];
	unsigned int	lc[16];
	int	po,i,xx,pp,sta,ff=0;

	po=ad;pp=p;

	if(line!=32){
		if(line<0){line=-line;ff=1;}
		for(i=po;i<(po+line);i++){
			xx=21+i*2;
			if(rpar[i][0]==2){
				int	j;
				unsigned char	a;
				a=rpat[i][0];strcpy(tmp0,"/ ");
				if(a==0xf8){tmp0[0]=']';}
				if(a==0xf9){tmp0[0]='[';}
				B_COLOR(1);for(j=8;j<=23;j++){B_LOCATE(xx,j);B_PRINT(tmp0);}
				B_LOCATE(xx-2,25+(i&1));
				if(a<0xf0){B_PRINT(fstr(rpar[i][2],3));}else{B_PRINT("   ");}
				if( ff ){tcury(xx,8,16,1);}

			}else{
				if(rpar[i][0]==0){
					int	j;
					B_COLOR(2);
					for(j=0;j<=15;j++){B_LOCATE(xx,8+j);B_PRINT("¥");}
					B_LOCATE(xx-2,25+(i&1));B_PRINT("   ");
					if( ff ){tcury(xx,8,16,2);}
				}else{
					int	j;
					if(rpar[i][2]==0){B_COLOR(2);}else{B_COLOR(3);}
					for(j=0;j<=15;j++){
						B_LOCATE(xx,8+j);B_PRINT(rym[vlcl[rpat[i][pp+j]]]);
					}
					B_LOCATE(xx-2,25+(i&1));B_PRINT(fstr(rpar[i][2],3));
					if( ff ){
						if(rpar[i][2]==0){tcury(xx,8,16,2);
						}else{tcury(xx,8,16,3);}
				}	}
			}
		}
	}else{
		int	i,j;
		sta=SUPER(0);
		for(j=0;j<=15;j++){
			strcpy(tmp0,fstr(p+j+1,2));strcat(tmp0," ");strcat(tmp0,rhyna[p+j]);
			B_LOCATE(1,j+8);H_PRINT(tmp0,"","");

			B_LOCATE(21,j+8);strcpy(tmp0,"/ ");

			for(i=0;i<32;i++){
				if(rpar[i][0]==2){
					unsigned char	a;
					a=rpat[i][0];tmp0[0]='/';
					if(a==0xf8){tmp0[0]=']';}
					if(a==0xf9){tmp0[0]='[';}
					H_PRINT2("","",tmp0);
				}else{
					if(rpar[i][0]==0){
						H_PRINT2("","¥ ","");
					}else{
						if(rpar[i][2]==0){
							H_PRINT2("",rym[vlcl[rpat[i][pp+j]]],"");
						}else{
							H_PRINT2(rym[vlcl[rpat[i][pp+j]]],"","");
						}
					}
				}
			}
		}
		tmp0[0]=0;tmp1[0]=0;
		for(i=0;i<32;i=i+2){
			if(rpar[i][2]==0 || (rpar[i][0]==2 && rpat[i][0]>=0xf0)){
				strcat(tmp0,"    ");
			}else{
				strcat(tmp0,fstr(rpar[i][2],4));
			}

			if(rpar[i+1][2]==0 || (rpar[i+1][0]==2 && rpat[i+1][0]>=0xf0)){
				strcat(tmp1,"    ");
			}else{
				strcat(tmp1,fstr(rpar[i+1][2],4));
			}
		}
		B_LOCATE(18,25);H_PRINT(tmp0,"","");B_LOCATE(20,26);H_PRINT(tmp1,"","");
		SUPER(sta);
		snsclr();
	}

	{
		int	i=63,a=0,cu=0,ct,b=1;
		while(i>=0){
			unsigned char	c=rpat[i][0];
			if( c<0xf8 ){
				if(b==1){a=a+rpar[i][2];}else{a=a+rpar[i][2]*b;}
			}else{
				if( c==0xf8 ){
					if(cu<15){
						ct=rpat[i][1];if(ct==0){ct=256;}
						lc[cu]=b;b=b*ct;cu++;
					}
				}
				if( c==0xf9 ){
					if(cu>0){cu--;b=lc[cu];}
				}
			}
			i=i-1;
		}
		strcpy(tmp0,"TOTAL:");strcat(tmp0,fstr(a,5));
		B_COLOR(3);B_LOCATE(69,29);B_PRINT(tmp0);
	}
}

/***************************/
void	rhed_dis(int p)
{
	int	i;

	for(i=0;i<=15;i++){
		B_LOCATE(1,i+8);B_PRINT(fstr(p+i+1,2));B_PRINT(" ");
		B_PRINT(spadd(rhyna[p+i],14));}

}

/***************************/
void	rhy_step()
{
	int	i;
/*
bain:	msg("BAR ST=");i=vinput(barstep,0x803);msg_clr();
	if(i<48||i>480){goto stin;}
	barstep=i;
stin:
	B_LOCATE(12,1);
	B_PRINT("STEP ST=");i=vinput(dfstep,0x803);msg_clr();
*/

stin:	msg("STEP ST=");i=vinput(dfstep,0x803);msg_clr();
	if(i<6||(i>96 && i!=192)){goto stin;}
	dfstep=i;
}

/**************************0fl 2st 3cl   mode 0=nul 1=onpu 2=spcial */
void	rtopat(int me)
{
	char	tmp0[256];
	int	ad,i,j,xp=0;

	ad=meas_add(me);
	B_LOCATE(2,5);strcpy(tmp0,"MEAS=");strcat(tmp0,fstr(meas_no(ad),4));B_PRINT(tmp0);
	B_LOCATE(14,5);
	if(trk[track][ad]==0xfc){
		strcpy(tmp0,"=== SAME MEAS");
		strcat(tmp0,fstr((trk[track][ad+2]&3)*256+trk[track][ad+1]+1,4));
		strcat(tmp0," ===");B_PRINT(tmp0);
	}else{
		B_PRINT("                     ");
	}

	for(i=0;i<=63;i++){
		for(j=0;j<=63;j++){rpat[i][j]=0;}
		for(j=0;j<=4;j++){rpar[i][j]=0;}
	}
	rpar[64][0]=0;

	if(trk[track][ad]>=0xfe ||(trk[track][ad]>=0xfc && trk[track][ad+4]>=0xfe)){
		for(j=0;j<(barstep/dfstep);j++){rpar[j][2]=dfstep;rpar[j][0]=1;}
		return;
	}
	radr[0]=ad;

	while(1){
		unsigned char	a,b,c,d;
		a=trk[track][ad];b=trk[track][ad+1];c=trk[track][ad+2];d=trk[track][ad+3];
		if(xp>63){
			msg("‚±‚Ì¬ß‚ÍSTEP‚ª‘½‚·‚¬‚Ü‚·B•ÒWo—ˆ‚Ü‚¹‚ñB");
			xp++;rpar[64][0]=1;break;}
		if(a>0xfb){if(rpar[xp][0]){xp++;}
			break;}

		if(a<128){
			rpar[xp][0]=1;rpar[xp][4]++;
			if( c>0 && d>0){
				int	mf=0;
				for(i=0;i<=31;i++){
					if(rhyno[i][0]==a){
						rpat[xp][i]=d;rpat[xp][i+32]=c;mf=1;break;}
				}
				if(mf==0){
					int	p;
					p=rpar[xp][3];if(p<64){p=64;}
					rpat[xp][p]=a;rpat[xp][p+1]=0;
					rpat[xp][p+2]=c;rpat[xp][p+3]=d;
					rpar[xp][3]=p+4;
					if(p==124){if(b==0){xp++;}}
				}else{
					int	p;
					p=rpar[xp][3];if(p<4){rpar[xp][3]=4;}
				}
			}
			if(b>0){rpar[xp][2]=b;radr[xp++]=ad+4;}
		}else{
			int	p;
			if(rpar[xp][0]==1||(rpar[xp][0]==2&&(a==0xf8||a==0xf9))){
				radr[xp++]=ad+4;}

			rpar[xp][0]=2;p=rpar[xp][3];
			rpat[xp][p]=a;rpat[xp][p+1]=b;rpat[xp][p+2]=c;rpat[xp][p+3]=d;
			if(a<0xf6){rpar[xp][2]=rpar[xp][2]+b;}
			rpar[xp][3]=p+4;rpar[xp][4]++;

			if((a==0xf7 && trk[track][ad+4]!=0xf7)||a==0xf8 || a==0xf9 ||
			   trk[track][ad+4]<128||p==124||b>0){
				radr[xp++]=ad+4;
			}
		}
		ad=ad+4;
	}
	if(xp){
		for(i=xp;i<=63;i++){
			for(j=0;j<=63;j++){rpat[i][j]=0;}
			for(j=0;j<=4;j++){rpar[i][j]=0;}
		}
	}
}

/***************************/
void	pattor(int me)
{
	int	ad,i,c,p,po=0;

	if(rpar[64][0]!=0){return;}

	for(i=0;i<=63;i++){
		p=rpar[i][0];

		if(p==2){
			c=rpar[i][3];
			if(c>0){
				int	j;
				for(j=0;j<c;j++){dat[po+j]=rpat[i][j];}
				po=po+c;
			}
		}
		if(p<=1 && (rpar[i][2]>0||rpar[i][3]>0) ){
			int	j,cf=0,c;
			for(j=0;j<=31;j++){if(rpat[i][j]>0){cf++;}}
			if(cf!=0){
				for(j=0;j<=31;j++){
					if(rpat[i][j]>0){
						dat[po]=rhyno[j][0];dat[po+1]=0;
						dat[po+2]=rpat[i][j+32];dat[po+3]=rpat[i][j];
						po=po+4;
					}
				}
			}else{
				if(rpar[i][3]<64){
					dat[po]=0;dat[po+1]=rpar[i][2];dat[po+2]=0;dat[po+3]=0;
					po=po+4;
				}
			}
			c=rpar[i][3]-64;
			if(c>0){
				int	j;
				for(j=0;j<c;j++){dat[po+j]=rpat[i][64+j];}
				po=po+c;
			}
			dat[po-3]=rpar[i][2];
		}
	}
	dat[po]=0xfd;po+=4;

	ad=meas_add(me);c=meas_len(ad);
	if(trk[track][ad]==0xfc){return;}
	if(po<c){trk_delete(ad,c-po);}
	if(po>c){if(trk_shift(ad,po-c)!=0){return;}}
	for(i=0;i<po;i++){trk[track][ad+i]=dat[i];}
}

/***************************/
void	rsttrc(int *no,int *cx,int flg,int pg)
{
	int	ad,rad,i,j,ii,jj,jjj=0,a,skf,ofs,nn=0;
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
		}

		ad=meas_add(ofs+1);if(ad<0){break;}
		nn=0;rad=0;

		j=rcd->bar[track];jj=j;i=rcd->step[track];ii=i;
		if(skf!=0){if(j==0){i=0;}else{j--;}}

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

		if(ptr[ad]==0xfe && ad>0){ad-=4;}
		a=meas_no(ad);

		if(jjj!=a){rtopat(a);rtrk_dis(pg,0,32);jjj=a;}
		*no=a;

		for(i=0;i<32;i++){
			if(radr[i]>ad){break;}
		}
		if(i>31){i=0;}

		*cx=i;
		if(flg==0){
			rtrk_dis(pg,*cx,-1);
			while(jj==rcd->bar[track] && ii==rcd->step[track]){
				if(B_KEYSNS()!=0){break;}
				if( !(rcd->active[track]) ){break;}
			}
			rtrk_dis(pg,*cx,1);
		}
		flg=0;
	}
	LEDMOD(0,0);snsclr();
}

/***************************/
void	disp_vels()
{
	int	i,col;

	for(i=0;i<10;i++){
		col=0;if(i==dfvel){col=8;}
		sdis2(50+i*3,592,rym_s[i],3,15,col);
	}
}

/***************************/
