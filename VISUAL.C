/*
 * STed: visual.c  (controller visal edit)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"graph.h"
#include	"math.h"
#include	"sted.h"

char curv_tbl[192]={
	  0,  4,  7, 10, 13, 17, 20, 24, 27, 30, 33, 36, 39, 42, 44, 46,
	 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 72, 74, 75, 77,
	 78, 80, 81, 82, 84, 85, 86, 87, 89, 90, 91, 92, 93, 94, 95, 96,
	 97, 98, 99,100,100,101,102,103,103,104,105,105,106,107,107,108,
	108,109,109,110,110,111,111,112,112,113,113,113,114,114,115,115,
	116,116,116,117,117,117,118,118,118,119,119,119,119,120,120,120,
	120,121,121,121,121,121,122,122,122,122,122,122,123,123,123,123,
	123,123,123,123,124,124,124,124,124,124,124,124,124,125,125,125,
	125,125,125,125,125,125,125,125,125,125,126,126,126,126,126,126,
	126,126,126,126,126,126,126,126,126,126,126,126,127,127,127,127,
	127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
	127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

int	yzm,vmo2,maxdot,excget;
int	XB,DM;

int	step_cluc2();
int	add_set();
int	meas_add();
int	meas_no();
void	all_note_off();
int	trk_ext_sub();
int	trk_mix_sub();
void	trk_cluc();
int	trk_shift();
void	trk_delete();
int	size_change();
void	poplay();
void	undobuf();
void	msg();
void	msg_clr();
void	key_rep_off();
void	key_rep_on();
int	key_shift();
char	*ctrl_type();
char	*prog_name();

void	txerase();
void	txxline();
void	txyline();
void	g_print();

int	vinput();
void	fnc_dis();
char	*fstr();
void	sdis();
void	sdis2();
void	vdis2();
void	snsclr();

void	vis_disp();
int	vis_bufset();
int	vis_ext();
int	vis_ext_mix();
int	vis_a_in();
int	vis_a_in_co();
int	vis_a_in_st();
void	vis_a_out();
void	vis_wave();
int	vis_cut();
int	vis_lnk();
void	vis_zom();
void	bunkatsu();

/***************************/
int	vis_edit(int po,int ln,int ssh)
{
	int	i,j,st,cx,ccx,ke=0,kc,sh,lpara=0,co,vmo=-1,xss=6,ys=1,yss=10;
	int	ma,mx,my,msf=-1,extmd=0,barlen,retad=0,eds=edit_scr,ost;

	int	para[769],step[769],step2[769];
	char	tmp9[64];
	unsigned char *ptr=trk[track];
	int	me_ad[2048];

if(cnfflag&0x4000){ssh=1-(ssh&1);}

	XB=320-8;DM=400;if(ssh&1){XB=0;DM=720;}else{if(eds){return po;}}

	barlen = tbase*4*beat1/beat2;

	yzm=1;vmo2=0;excget=0;
	add_set(track,me_ad);
	edit_scr=0;cons_md(0);

resel:
/*	msg("Visual Editor:");*/
	msg("EDIT MODE(P/V/E/M/N/C/K/T/R/U/X/nn/G) =");

	while(B_KEYSNS()==0){}
	ke=B_KEYINP()&0xff;
	if(ke>='a' && ke<='z'){ke-=32;}

	if(ke>='0' && ke<='9'){
		int n;
		n=vinput(ke-'0',0x503);
		if( es!=0 ){msg_clr();goto ext;}
		if( n<1 || n>127 ){goto resel;}
		vmo=n;
	}

	if(ke=='/'){
		char	tmp0[64];
		int	ct,ctl;
resin:
		B_LOCATE(38,1);B_PRINT("/");
		tmp0[0]=0;sinput(tmp0,0x804);

		if( es!=0 ){msg_clr();goto ext;}
		ct=spc_code(tmp0,&ctl);if(ct==0){goto resin;}
		vmo=ct;if(ctl>=0){vmo=ctl;}
	}

	if(ke=='G'){
		int	ct=trk[track][po];

		if(ct==0x98){excget=1;}
		if((ct>=0x90 && ct<=0x98)||(ct==0xde || ct==0xed)){
			ke=0;vmo=ct;vmo2=trk[track][po+2];goto start;}

		if(ct==0xee){
			ke='P';
		}else{
			if(ct==0xeb){ke=0;
				vmo=trk[track][po+2];
				goto start;
			}else{
				if(ct>=128 && ct<0xf0){vmo=ct;ke=0;goto start;
				}else{goto resel;}
			}
		}

	}


	if(ke==0x1b){msg_clr();return(po);}
	if(ke=='P'){vmo=0;}
	if(ke=='M'){vmo=1;}
	if(ke=='V'){vmo=7;}
	if(ke=='E'){vmo=11;}
	if(ke=='N'){vmo=10;}
	if(ke=='K'){vmo=0xed;}
	if(ke=='C'){vmo=0xea;}
	if(ke=='T'){vmo=0xe7;}
	if(ke=='X'){vmo=0x98;}
	if(ke=='U'){
		int n;
reue:
		msg("UserExc");B_PRINT(" No.=");n=vinput(0,0x803);
		if( es!=0 ){msg_clr();goto ext;}
		if( n<0 || n>7 ){goto reue;}
		vmo=0x90+n;
	}

	if(ke=='R'){
		int n;
reue2:
		msg("Rol.Para");B_PRINT(" Add.=");n=vinput(0,0x803);
		if( es!=0 ){msg_clr();goto ext;}
		if( n<0 || n>127 ){goto reue2;}
		vmo=0xde;vmo2=n;
	}


	if(vmo<0){goto resel;}

start:
	if(vmo==0){
		strcpy(tmp9,"PITCH");ys=16;yss=682;
	}else{
		if(vmo<128){
			strcpy(tmp9,ctrl_type(vmo));
			if(vmo==7 ){lpara=100;}
			if(vmo==11 ){lpara=127;}
			if(vmo==10 ){lpara=64;}
			if(vmo>=71 && vmo<=74){lpara=64;}
		}else{
			if(vmo==0x98){
				strcpy(tmp9,"Tr.Exclu");
/*
if(excget){
	int ad2=po,excc=0;
	while(trk[track][ad2+4]==0xf7){ad2+=4;excc+=4;}
	strcat(tmp9,"(");strcat(tmp9,nstr(excc>>1));strcat(tmp9,")");
}
*/

			}else{
				if(vmo<0x98){
					strcpy(tmp9,"UserExc ");tmp9[7]=vmo-0x90+'0';
				}else{
					if(vmo==0xe7){
						strcpy(tmp9,"TEMPO");lpara=64;
					}else{
						if(vmo==0xde){
							strcpy(tmp9,"Rol.Para");
						}else{
							if(vmo==0xea){
								strcpy(tmp9,"AFTER CH.");
							}else{
								if(vmo==0xed){
									strcpy(tmp9,"AFTER KEY");
								}else{
									strcpy(tmp9,"");
								}
							}
						}
					}
				}
			}
		}
	}
	cx=0;ccx=-1;


	if(ln==0){
		char	tmp0[64];
		int	st2;
		st2=barlen>>2;

		if(ptr[po]<127 && ptr[po+1]>1){/*ln=4;*/st2=ptr[po+1];}
rest:
		msg(tmp9);/*B_LOCATE(10,1);*/B_PRINT(" TOTAL ST=");

		if(ln){st=vis_a_in_st(po,ln);B_PRINT(nstr(st));ln=0;
		}else{
			st=vinput(st2,0x804);
			if( es!=0 ){msg_clr();goto ext;}
		}
		if( st<2 || st>barlen*8 ){goto rest;}
reco:
		st2=st;if(st2>767){st2=768;}
		co=st2*vis_reso/100;

		B_LOCATE(25,1);B_PRINT("COUNT=");strcpy(tmp0,nstr(co));sinput(tmp0,0x804);
		if( es!=0 ){msg_clr();goto ext;}
		co=atoi(tmp0);
		if(tmp0[0]=='/'){int co2=atoi(&tmp0[1]);if(co2>=1){co=st2/co2;}}
		if(tmp0[0]=='%'){int co2=atoi(&tmp0[1]);if(co2>=1){co=st2*co2/100;}}

		if( co<2 || co>st || co>768 ){goto reco;}

		i=po;
		while(i>0){
			unsigned char	d;
top:
			i=i-4;d=ptr[i];
			if(vmo==0){
				if(d==0xee){
					lpara=((ptr[i+3]*128+ptr[i+2])-8192);
					break;
				}
			}else{
				if(vmo<128){
					if(d==0xeb && ptr[i+2]==vmo){
						lpara=ptr[i+3];break;
					}
				}else{
					if(d==vmo){
						if(vmo==0xe7){
							lpara=ptr[i+2];break;
						}else{
							lpara=ptr[i+3];break;
						}
					}
				}
			}
			if( d>=0xfc && retad!=0){i=retad;retad=0;goto top;}

			if( d==0xfc ){
				int	ii,jj,ct;
				jj=i;
resame:
				ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct+1]-4;

				if(jj!=ii && ii>=0){
					jj=ii;if(ptr[ii]==0xfc){goto resame;}
					retad=i;i=ii;goto top;
				}
			}
		}

		ln=0;
		para[768]=lpara;para[0]=lpara;para[co-1]=lpara;
		vis_wave(st,co,cx,ccx,para,step,2,vmo);
	}else{
		co=vis_a_in(po,ln,para,step,vmo);
		para[768]=para[0];

		st=0;for(i=0;i<co;i++){st+=step[i];}

		if(vis_a_in_co(po,ln,vmo)==0 && vis_a_in_st(po,ln)>1){goto rest;}

		if(co<2||st<2){msg("データが少なすぎます。");return(po);}
		if(co>768||st>barlen*8){msg("データが多すぎます。");return(po);}
	}

	ost=st;

	if(co>192){xss=12;}
	if(co>192*2){xss=24;}
	if(ssh&1){B_CLR_AL();}
redraw:
	fnc_dis(5);

	fill(7+XB,544,767,1001,0);
	box(7+XB,604-16,767,1001,14,65535);
	sdis(1+(XB>>3),589-16," CONTROLLER VISUAL EDIT",95-(XB>>3),15,14);

	if(vmo){
		for(i=768-128;i<=768+128;i=i+16){
			line(7+XB,i,11+XB,i,14,0xffff);line(763,i,767,i,14,0xffff);
			line(13+XB,i,761,i,4,0x4444+0x1111);
		}
	}else{
		for(i=-12;i<=12;i++){
			j=768+(128*i/12);
			line(7+XB,j,11+XB,j,14,0xffff);line(763,j,767,j,14,0xffff);
			line(13+XB,j,761,j,4,0x4444+0x1111);
		}
	}

	for(i=768-128;i<=768+128;i=i+2){
		line(7+XB,i,9+XB,i,14,0xffff);line(765,i,767,i,14,0xffff);
	}
	line(7+XB,768,767,768,14,0xffff);


	{
		int gx,st2,co2,sts;

		st=0;for(i=0;i<co;i++){st+=step[i];}

		maxdot=DM;
		if(st<DM){maxdot=st;}
		if(st<200){maxdot=st*2;}
		if(st<100){maxdot=st*4;}
		if(st< 50){maxdot=st*8;}

		st2=st+1;co2=0;
		if(st<=DM){sts=8;}else{sts=(barlen>>2);}
		while(st2>sts){step2[co2++]=sts;st2-=sts;}
		if(st2>0){step2[co2++]=st2;}
		vis_zom(co2,st,maxdot,step2,step2);

		gx=18+XB;
		for(i=0;i<co2;i++){
			line(gx,768-128,gx,768+128,4,0xaaaa);

			if(st<barlen){
				g_print2(gx-32+4,608+4+8,fstr(sts*i,4),4);
			}
			gx+=step2[i];
		}


		st2=st+1;co2=0;
		while(st2>barlen){step2[co2++]=barlen;st2-=barlen;}
		if(st2>0){step2[co2++]=st2;}
		vis_zom(co2,st,maxdot,step2,step2);

		gx=18+XB;
		for(i=0;i<co2;i++){
			line(gx,768-128,gx,768+128,4,0xffff);
			g_print2(gx-32+4,608+4+8,fstr(barlen*i,4),4);
			gx+=step2[i];
		}
	}

	box(16+XB,768-132-8,763,768-132,14,0xffff);
	box(17+XB,768-132-4,762,768-132-4,1,0xaaaa);

	box(16+XB,768+132+8,763,768+132,14,0xffff);
	box(17+XB,768+132+4,762,768+132+4,2,0xaaaa);

	B_LOCATE(45,5);B_PRINT(tmp9);B_PRINT(" Edit Mode");
	B_LOCATE(80,29);B_PRINT("TOTAL ST=");B_PRINT(fstr(st,4));

	vis_disp(co,cx,ccx,para,step,0,1,vmo);

if((cmdflag&0x200)==0){
	MS_INIT();MS_LIMIT(18+XB,129-8-4,762,256+127+8+4);MS_CURON();
}

	while(1){
		vis_disp(co,cx,ccx,para,step,1,1,vmo);
loop:
		B_LOCATE(42,26);B_PRINT("STEP=");B_PRINT(fstr(cx+1,4));
				B_PRINT("/");B_PRINT(fstr(co,4));
		B_LOCATE(42,27);if(ccx>=0){B_PRINT("SUB =");B_PRINT(fstr(ccx,4));}else{B_PRINT("         ");}

		B_LOCATE(60,26);B_PRINT("STEP TIME=");B_PRINT(fstr(step[cx],4));
		B_LOCATE(77,26);B_PRINT("PARA=");
		if(vmo==0){B_PRINT(fstr((para[cx]),5));}else{B_PRINT(fstr((para[cx]),3));}
/*
		if(vmo==10){
			int pp=para[cx]-64;
			B_PRINT("(");
			if(pp==0){B_PRINT("C");}else{
			if(pp<0){pp=(-pp);B_PRINT("L");}else{B_PRINT("R");}}
			B_PRINT(fstr(pp,2));B_PRINT(")");
		}
*/
		B_LOCATE(66,29);B_PRINT("Y-ZOOM=");B_PRINT(fstr(yzm,2));

		while(B_KEYSNS()==0){
			/*msstat(&mx,&my,&ma,&mb);mspos(&mx,&my);*/
			/*ma=MS_GETDT()&0xff00;mx=MS_CURGT();my=mx&0xffff;mx=mx>>16;*/
			ma=MS_GETDT()&0xff00;mspos(&mx,&my);
			if(ma==0){msf=-1;}

			if(ma!=0 && my>128-12 && my<256+128+12 ){
				int	i,px;px=18+XB;

				vis_zom(co,st,maxdot,step,step2);

				for(i=0;i<co;i++){
					if(mx>=px && mx<(px+step2[i])){
						if(my>128 && (my<=256 ||((vmo==0||yzm>1) && my<256+128)) ){
							int aa=para[i];
							if(vmo==0){
								para[i]=(-(my-256))*64/yzm;
							}else{
								if(yzm<2){
									para[i]=-(my-256);
								}else{
									para[i]=-(my-(256+128))>>1;
								}
							}
							if(msf>=0 && msf!=i){
								vis_wave(st,co,i,msf,para,step,2,vmo);
								aa=para[i]+1;
							}
							cx=i;msf=i;

							if(aa!=para[i]){
								vis_disp(co,i,ccx,para,step,0,1,vmo);
							}
						}
						if( my<=128-4 ){
							if(ccx>=0){ccx=-1;}else{ccx=i;}
							vis_disp(co,cx,ccx,para,step,0,1,vmo);

							while((MS_GETDT()&0xff00)!=0){}
						}
						if( my>128+256+4 ){
							cx=i;
							vis_disp(co,cx,ccx,para,step,0,1,vmo);

							while((MS_GETDT()&0xff00)!=0){}
						}
					}
					px+=step2[i];
				}
				goto loop;
			}
		}
		ke=B_KEYINP();kc=ke & 0xff;ke=ke>>8;sh=B_SFTSNS();
		if(kc>='a' && kc<='z'){kc-=32;}

		vis_disp(co,cx,ccx,para,step,1,0,vmo);

		if( ke==1 || kc==13 ){break;}

		if( ke==0x70 ){fnc_dis(6);}
		if( ke==0xf0 ){fnc_dis(5);}

		if( kc==32 ){
			if(ccx<0){
				ccx=cx;
			}else{
				vis_disp(co,cx,ccx,para,step,1,2,vmo);
				ccx=-1;
			}
		}
		if( kc=='Y' ){
			if(yzm<3){yzm++;}else{yzm*=2;if(yzm>6){yzm=1;}}
			if(vmo!=0 && yzm>2){yzm=1;};
			vis_disp(co,cx,ccx,para,step,0,1,vmo);
		}
		if( kc==0x13 || ke==0x3b ){
			if(cx==0 || (sh&6)){vis_disp(co,cx,ccx,para,step,1,2,vmo);}
			if((sh&6)==0){cx--;}else{cx-=xss;}
			if(cx<0){cx=co-1;}
		}
		if( kc==0x04 || ke==0x3d){
			if(cx==co-1||(sh&6)){vis_disp(co,cx,ccx,para,step,1,2,vmo);}
			if((sh&6)==0){cx++;}else{cx+=xss;}
			if(cx>=co){cx=0;}
		}
		if( ke==0x3f){
			if((sh&2)==0){
				para[cx]=0;
			}else{
				int j;
				for(j=0;j<co;j++){para[j]=para[512];}
				vis_disp(co,cx,ccx,para,step,0,1,vmo);
			}
		}

		if( kc>='0' && kc<='9' && vmo!=0){para[cx]=(kc-'0')*14;}

		if( kc==0x18 || ke==0x3e){
			if((sh&6)==0){para[cx]=para[cx]-ys;}else{
				para[cx]=para[cx]-yss;
				if(vmo==0){
					int i;i=para[cx];if(i<0){i+=yss;}
					i=i/700;if((i&1)==0){para[cx]--;}
				}
			}
			if(para[cx]<-8192){para[cx]=-8192;}
			if(vmo!=0 && para[cx]<0){para[cx]=0;}
		}
		if( kc==0x05 || ke==0x3c){
			if((sh&6)==0){para[cx]=para[cx]+ys;}else{
				if(vmo==0){
					int i;i=para[cx];if(i<0){i+=yss;}
					i=i/700;if((i&1)==0){para[cx]++;}
				}
				para[cx]=para[cx]+yss;
			}
			if(para[cx]>8191){para[cx]=8191;}
			if(vmo!=0 && para[cx]>127){para[cx]=127;}
		}
/*
	 	if( ke==0x39 ){
			if((sh&1)==0){para[cx]=para[cx]-ys*4;}else{para[cx]=para[cx]-yss*4;}
			if(para[cx]<-8192){para[cx]=-8192;}
			if(vmo!=0 && para[cx]<0){para[cx]=0;}
		}
		if( ke==0x38 ){
			if((sh&1)==0){para[cx]=para[cx]+ys*4;}else{para[cx]=para[cx]+yss*4;}
			if(para[cx]>8191){para[cx]=8191;}
			if(vmo!=0 && para[cx]>127){para[cx]=127;}
		}
*/

/*f1*/
		if( ke>=0x63 && ke<=0x64){
			vis_disp(co,cx,ccx,para,step,1,2,vmo);
		}
		if( ke==0x63 && (sh&1)==0){cx=0;ke=0;}
		if( ke==0x64 && (sh&1)==0){cx=co-1;ke=0;}

		if( ke>=0x63 && ke<=0x6c ){
			vis_wave(st,co,cx,ccx,para,step,ke-0x63+(sh&1)*16,vmo);
			vis_disp(co,cx,ccx,para,step,0,1,vmo);
			if((sh&1)==0){fnc_dis(5);}else{fnc_dis(6);}
		}

		if( kc=='X' ){
			int j;
			vis_disp(co,cx,ccx,para,step,1,2,vmo);
			msg("STEP=");j=vinput(2,0x804);msg_clr();
			if( es==0 && j>0 ){
				co=vis_cut(co,j,para,step);extmd=1;
			}
			vis_disp(co,cx,ccx,para,step,0,1,vmo);
			fnc_dis(5);
		}
		if( kc=='Z' ){
			int j;
			vis_disp(co,cx,ccx,para,step,1,2,vmo);
rezm:
			msg("STEP=");j=vinput(st,0x804);msg_clr();

			if( es==0 && j>0 ){
				if(j>barlen*8){msg("データが多すぎます。");goto rezm;}
				else{vis_zom(co,st,j,step,step);extmd=1;}
			}
			goto redraw;
		}

		if( kc=='I' ){
			int j;
			vis_disp(co,cx,ccx,para,step,1,2,vmo);
reis:
			if((sh&1)!=0){
				msg("Change STEP=");j=vinput(step[cx],0x804);msg_clr();

				if( es==0 && j>0 ){
					if(st+j>barlen*8){msg("データが多すぎます。");goto reis;}
					else{
						step[cx]=j;extmd=1;}
				}
			}else{
				if(co<768){
					msg("Insert STEP=");j=vinput(0,0x804);msg_clr();

					if( es==0 && j>0 ){
						if(st+j>barlen*8){msg("データが多すぎます。");goto reis;
						}else{
							for(i=co;i>cx;i--){step[i]=step[i-1];para[i]=para[i-1];}
							co++;st+=j;
							step[cx]=j;extmd=1;}
					}
				}
			}
			goto redraw;

		}

		if( kc=='D' && co>1 ){
			if((sh&1)!=0){
				int j;
				vis_disp(co,cx,ccx,para,step,1,2,vmo);

				msg("Delete STEP=");j=vinput(step[cx],0x804);msg_clr();

				if( es==0 && j>0 ){
					st-=j;
					while(j){
						if(step[cx]>j){step[cx]-=j;j=0;
						}else{
							j-=step[cx];
							for(i=cx;i<co;i++){step[i]=step[i+1];para[i]=para[i+1];}
							co--;
						}
					}
					extmd=1;
					goto redraw;
				}
			}else{
				if(st>step[cx]){
					st-=step[cx];
					for(i=cx;i<co;i++){step[i]=step[i+1];para[i]=para[i+1];}
					co--;
					extmd=1;
					goto redraw;
				}
			}
		}

		if( kc=='C' ){
			co=vis_lnk(co,para,step);extmd=1;cx=0;ccx=-1;
			vis_disp(co,cx,ccx,para,step,0,1,vmo);
			fnc_dis(5);
		}

		if( ke==0x10 ){
			int j;
			vis_disp(co,cx,ccx,para,step,1,2,vmo);
			msg("JUMP POSITION=");j=vinput(0,3);msg_clr();
			if( es==0 && j>0 ){if( j<=co ){cx=j-1;}else{cx=co-1;}}
		}

		if(kc=='M'||ke==0x55 ||ke==0x3a){
			if(((sh&1)==0 && ln==0) || trk[track][po]==0xfe){
				int cc;
				cc=vis_bufset(po,co,para,step,vmo,0);
				if(trk_shift(po,cc)==0){
					memcpy_l(&ptr[po],dat2,cc);
					poplay(po,3);trk_delete(po,cc);
				}
			}else{
				int ll;ll=tr_len[track];
				cpleng=0;
				memcpy_l(cpbuf,trk[track],ll);
				if(vis_ext_mix(po,co,para,step,vmo,ost)==0){
				poplay(po,3);}

				memcpy_l(trk[track],cpbuf,ll);
				tr_len[track]=ll;
			}
		}

	}
	B_CLR_AL();fnc_dis(0);fill(7+XB,544,767,1001,0);snsclr();
if((cmdflag&0x200)==0){
	MS_LIMIT(0,0,767,511);MS_CUROF();
}
	if(kc==13){
		if(ln==0){
			int	ad;
			co=vis_lnk(co,para,step);
			ad=po;

			if(((sh&1)==0 && ln==0) || trk[track][po]==0xfe){
				int mm,cc;
				undobuf(0);

				if(trk[track][po]==0xfe){mm=1;}else{mm=0;}
				cc=vis_bufset(po,co,para,step,vmo,mm);

				if(trk_check(cc)==0){
					line_ins_aft(po);trk_shift(po,cc);
					memcpy_l(&ptr[po],dat2,cc);
					ad+=cc;
				}
				po=ad;
			}else{
				vis_ext_mix(po,co,para,step,vmo,ost);
			}
		}else{
			if(extmd==0){
				vis_a_out(po,ln,para,step,vmo);
			}else{
				co=vis_lnk(co,para,step);
				vis_ext_mix(po,co,para,step,vmo,ost);
			}
		}
	}

	edit_scr=eds;
	if(ssh&1){edfield(0);}
ext:
	edit_scr=eds;cons_md(1);
	return(po);
}

/***************************/
int	vis_bufset(int po,int co,int *para,int *step,int vmo,int bf)
{
	int	ad=0,i,j,barlen,st=0,s,excc=0;

if(excget){
	int ad2=po;
	while(trk[track][ad2+4]==0xf7){ad2+=4;excc+=4;}
}

	barlen = tbase*4*beat1/beat2;

	if(bf){
		st=step_cluc(po);
		while(st>=barlen){st-=barlen;dat2[ad]=0xfd;ad+=4;}
	}

	for(i=0;i<co;i++){
		if(vmo==0){
			dat2[ad]=0xee;
			j=(para[i])+8192;if(j<0){j=0;}
			if(j>16383){j=16383;}
			dat2[ad+2]=j&0x7f;dat2[ad+3]=j>>7;
		}else{
			if(vmo<128){
				dat2[ad]=0xeb;dat2[ad+2]=vmo;
			}else{
				dat2[ad]=vmo;dat2[ad+2]=0;dat2[ad+3]=0;
			}
			j=para[i];if(j<0){j=0;}
			if(vmo==0xea || vmo==0xe7){dat2[ad+2]=j;}else{dat2[ad+3]=j;}
/*			if(vmo==0xde){dat2[ad+2]=vmo2;}*/
if(vmo<=0x98 || vmo==0xed || vmo==0xde ){dat[ad+2]=vmo2;}

if(excget){
	int ad2=po+4,i2;
	dat2[ad+1]=0;

	for(i2=0;i2<excc;i2++){dat2[ad+4]=trk[track][ad2++];ad++;}

	dat2[ad]=0;dat2[ad+1]=0;dat2[ad+2]=0;dat2[ad+3]=0;
}
		}

		s=step[i];
top:
		dat2[ad+1]=s;
		ad=ad+4;

		if(bf){
			if(st+s<barlen){
				st+=s;
			}else{
				if(st+s==barlen){
					dat2[ad]=0xfd;ad+=4;st=0;
				}else{
					int ss=barlen-st;

					dat2[ad-3]=ss;
					dat2[ad]=0xfd;ad+=4;
					s-=ss;st=0;

					if(s){
						dat2[ad]=0;dat2[ad+2]=0;dat2[ad+3]=0;
						goto top;
					}
				}
			}
		}

	}
	return(ad);
}

int	b2_set(int ad,int ln)
{
	while(ln>240){
		ln-=240;
		dat2[ad++]=0;dat2[ad++]=240;
		dat2[ad++]=0;dat2[ad++]=0;
	}
	if(ln){
		dat2[ad++]=0;dat2[ad++]=ln;
		dat2[ad++]=0;dat2[ad++]=0;
	}
	return ad;
}

/***************************/
int	vis_ext_mix(int po,int co,int *para,int *step,int vmo,int ost)
{
	int	ad,i,ec,ttc;

	ttc=1;for(i=0;i<co-1;i++){ttc+=step[i];}
	if(ost>ttc){ttc=ost;}

	ad=po;
	while(ad<tr_len[track] && ttc>0){
		if(vmo==0){
			if(trk[track][ad]==0xee){
				trk[track][ad]=0;trk[track][ad+2]=0;trk[track][ad+3]=0;
			}
		}else{
			if(vmo<128){
				if(trk[track][ad]==0xeb && trk[track][ad+2]==vmo){
					trk[track][ad]=0;trk[track][ad+2]=0;trk[track][ad+3]=0;
				}
			}else{
				if(trk[track][ad]==vmo){
					trk[track][ad]=0;trk[track][ad+2]=0;trk[track][ad+3]=0;
if(excget){
					if(vmo==0x98){
						while(trk[track][ad+4]==0xf7){
							trk[track][ad]=0;trk[track][ad+1]=0;
							trk[track][ad+2]=0;trk[track][ad+3]=0;
							ad+=4;
						}
					}
}
				}
			}
		}
		if(trk[track][ad]<0xf0){ttc-=trk[track][ad+1];}
		ad+=4;
	}

	ad=vis_bufset(po,co,para,step,vmo,0);
	dat2[ad]=0xfe;
	ec=trk_mix_sub(trk[track]+po,dat2);

	if(size_change(track,po+ec)){return 1;}

	dat[ec]=0xfe;ec+=4;
	memcpy_l(&trk[track][po],dat,ec);
	tr_len[track]=po+ec;
	return(0);
}

/***************************/
int	vis_a_in(int po,int ln,int *para,int *step,int vmo)
{
	int	ad,i,j,no=0,st=0;
	unsigned char *ptr=trk[track];

	ad=po;
	for(i=0;i<ln;i+=4){
		if(vmo==0){
			if(ptr[ad]==0xee){
				j=ptr[ad+2]+ptr[ad+3]*128;
				step[no]=0;para[no++]=(j-8192);
			}
		}else{
			if(vmo<128){
				if(ptr[ad]==0xeb){
					unsigned char cc=ptr[ad+2];
					step[no]=0;if(vmo==cc){para[no++]=ptr[ad+3];}
				}
			}else{
				if(ptr[ad]==vmo){
					step[no]=0;para[no++]=ptr[ad+3];
					if(vmo==0xea||vmo==0xe7){para[no-1]=ptr[ad+2];}
				}
			}
		}
		if(no>0 && ptr[ad]<0xf0 && ptr[ad+1]>0){
			st+=ptr[ad+1];

			if(step[no-1]+ptr[ad+1]<=240){
				step[no-1]+=ptr[ad+1];
			}else{
				step[no]=step[no-1];
				para[no]=para[no-1];
				no++;
			}
		}
		ad=ad+4;
	}
	return(no);
}

/***************************/
int	vis_a_in_co(int po,int ln,int vmo)
{
	int	ad,i,no=0;
	unsigned char *ptr=trk[track];

	ad=po;
	for(i=0;i<ln;i+=4){
		if(vmo==0){
			if(ptr[ad]==0xee){no++;}
		}else{
			if(vmo<128){
				if(ptr[ad]==0xeb && ptr[ad+2]==vmo){no++;}
			}else{
				if(ptr[ad]==vmo){no++;}
			}
		}
		ad=ad+4;
	}
	return(no);
}

/***************************/
int	vis_a_in_st(int po,int ln)
{
	int	ad,i,st=0;
	unsigned char *ptr=trk[track];

	ad=po;
	for(i=0;i<ln;i+=4){
		if(ptr[ad]<0xf0 && ptr[ad+1]>0){
			st+=ptr[ad+1];
		}
		ad=ad+4;
	}
	return(st);
}

/***************************/
void	vis_a_out(int po,int ln,int *para,int *step,int vmo)
{
	int	ad,i,j,no=0;
	unsigned char *ptr=trk[track];

	ad=po;
	for(i=0;i<ln;i+=4){
		if(vmo==0){
			if(ptr[ad]==0xee){
				j=(para[no++])+8192;
				ptr[ad+2]=j&0x7f;ptr[ad+3]=j>>7;
			}
		}else{
			if(vmo<128){
				if(ptr[ad]==0xeb){
					if(vmo==ptr[ad+2]){ptr[ad+3]=para[no++];}
				}
			}else{
				if(ptr[ad]==vmo){
					if(vmo==0xea||vmo==0xe7){ptr[ad+2]=para[no++];}
					else{ptr[ad+3]=para[no++];}
/*					if(vmo==0xde){ptr[ad+2]=vmo2;}*/
if(vmo<=0x98 || vmo==0xed || vmo==0xde ){dat[ad+2]=vmo2;}
				}
			}
		}
		ad=ad+4;
	}
}

/***************************/
void	vis_disp(int co,int cx,int ccx,int *para,int *step,int mo,int c,int vmo)
{
	int	b,i,gx,op,yy,cl,of,st=0;
	int	step2[769];

	if(c==0){cl=0;of=1;}else{cl=65535;of=2;}
	for(i=0;i<co;i++){st+=step[i];}

	vis_zom(co,st,maxdot,step,step2);

	if(mo==0){txerase(10+XB,768-512-128,758-XB-10+4,257);}
	gx=18+XB;op=para[768];if(vmo==0){op=(op*yzm)>>6;}else{if(yzm>1){op=(op<<1)-127;}}

	i=0;
	while(gx<760 && i<co){
		int	pp=para[i];if(vmo==0){pp=(pp*yzm)>>6;}else{if(yzm>1){pp=(pp<<1)-127;}}
		b=step2[i];

		if(mo==0 ||(i>=cx-of && i<=cx+of)){
			if(c!=2 && i==cx){txyline(gx,256-128,257,1,cl);}

			if(b>1 && pp>=-128 && pp<=128){txxline(gx+1,256-pp,b-1,0,cl);}

			yy=op-pp;if(yy<0){op-=yy;yy=-yy;}
			if(b<2 && yy==0){yy=1;}	

			if(op>128){yy-=(op-128);op-=(op-128);}

			if(op<-128){yy=0;op-=(-128-op);}
			if(op-yy<-128){yy-=(-128-(op-yy));}

			if(yy>0){txyline(gx,256-op,yy,0,cl);}

		}
		if(mo==0 || i==ccx){
			if(c!=2 && i==ccx){txyline(gx,256-128,257,0,cl&0xaaaa);}
		}
		op=pp;gx=gx+b;i++;
	}
	snsclr();
}

/***************************/
void	vis_wave(int st,int co,int cx,int ccx,int *para,int *step,int md,int vmo)
{
	int	i,ct,ct2,ct3,ct4,max_p,min_p,xx,ln,sh,sst,xp;
	int	xc=2;
	float f;

	sh=md>>4;md=md & 0xf;
	xx=cx;ln=ccx-cx+1;if(ccx<cx){xx=ccx;ln=cx-ccx+1;}
	if(ccx<0){xx=0;ln=co;}
	if(ln<2){return;}

	ct=para[xx];ct2=para[xx+ln-1];ct3=(ct2-ct);
	if( md>=3 && md<=5 ){
		if(ct3<0){sh=1-sh;}
		if(ct==ct2){md=2;sh=0;}
	}
	ct4=ct3;if(ct4<0){ct4--;}else{ct4++;}

	if(ccx<0){bunkatsu(st,ln,&step[xx]);}
	max_p=0;min_p=65535;sst=0;xp=0;
	for(i=0;i<ln;i++){
		if(max_p<para[xx+i]){max_p=para[xx+i];}
		if(min_p>para[xx+i]){min_p=para[xx+i];}

		sst+=step[xx+i];
	}

	if(sh!=0 && md>=7){
reco:
		msg("wave count=");i=vinput(0,4);msg_clr();
		if( es!=0 ){return;}
		xc=i*2;
		if( xc<2 || xc>ln){goto reco;}
	}

	for(i=0;i<ln;i++){
		if(md==0){if(vmo!=0){para[xx+i]=127-para[xx+i];}
			else{para[xx+i]=(8191-(para[xx+i]+8191));}}
		if(md==1){int a;if(i>=(ln>>1)){break;}
			a=para[xx+i];para[xx+i]=para[xx+(ln-1-i)];para[xx+(ln-1-i)]=a;}

		if(md==2){							/*LINE&RND*/
			if(sh==0 || ct==ct2){para[xx+i]=ct+(ct3*i/(ln-1));
			}else{
				para[xx+i]=min_p+(rand()*(max_p-min_p+1)/32767);
			}
		}


		if(md==4 && i>0 && i<ln-1){					/*WAVE B*/
			if(sh==0){para[xx+i]=ct+ct4*sin(PID2*i/(ln-1));}
			else{para[xx+i]=ct2-(ct4*cos(PID2*i/(ln-1)));}
		}
		if(md==5){							/*WAVE C*/
			if(sh==0){para[xx+i]=ct+(ct3*curv_tbl[191*i/(ln-1)]/127);
			}else{para[xx+i]=ct2-(ct3*curv_tbl[191-(191*i/(ln-1))]/127);}
		}

		if(md==6){							/*OKA&TANI*/
			if(sh==0){f=min_p+(sin(PI*i/(ln-1))*(max_p-min_p));
			}else{f=max_p-(sin(PI*i/(ln-1))*(max_p-min_p));}
			goto fix;
		}
		if(md==7){							/*SIN WAVE*/
			f=((max_p+min_p)>>1)+(sin(PI*i*xc/(ln-1))*((max_p-min_p+1)>>1));
fix:
			if(f>0){f+=.5;}
			if(f<0){f-=.5;if(vmo){f=-f;}}
			para[xx+i]=f;
		}
		if(md==8 && max_p!=min_p){					/*triangle*/
			int a;

			a=((max_p+min_p)>>1)+(max_p-min_p)*xc*(i+1)/ln;
			while(a<min_p||a>max_p){
				if(a>max_p){a=max_p-(a-max_p);}
				if(a<min_p){a=min_p-(a-min_p);}
			}
			para[xx+i]=a;
		}
		if(md==9){
			int a;
			if(((int)(i/(ln/xc))&1)==0){a=max_p;}else{a=min_p;}
			para[xx+i]=a;
		}
		xp+=step[xx+i];
	}
}

void	bunkatsu(int st,int co,int *da)
{
	int i,c=0,d;

	for(i=1;i<=co;i++){
		d=(st*i/co);da[i-1]=d-c;c+=(d-c);
	}
}

/***************************/
int	vis_cut(int co,int st,int *para,int *step)
{
	int	para2[768],step2[768];
	int	cc=0,i,a,b;

	for(i=0;i<co;i++){
		a=step[i];b=para[i];

		while(a>st && cc<768){step2[cc]=st;para2[cc]=b;cc++;a-=st;}
		if(a && cc<768){step2[cc]=a;para2[cc]=b;cc++;}
	}

	memcpy(para,para2,cc*4);
	memcpy(step,step2,cc*4);
	return cc;
}

int	vis_lnk(int co,int *para,int *step)
{
	int	cc=0,i;

	for(i=0;i<co;i++){
		if(cc>0 && para[cc-1]==para[i] && (step[cc-1]+step[i])<=240){
			step[cc-1]+=step[i];
		}else{
			para[cc]=para[i];step[cc]=step[i];cc++;
		}
	}

	return cc;
}

void	vis_zom(int co,int in_step,int out_step,int *step,int *step2)
{
	int	i,st=0,stx=0,dt;

/*	if( in_step != out_step ) */{
		for(i=0;i<co;i++){
			dt=step[i];

			st += dt;
			dt = ( st * out_step / in_step ) - stx;
			stx += dt;

			step2[i]=dt;
		}
	}
}

/***************************/
