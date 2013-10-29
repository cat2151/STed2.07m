/*
 * STed: track.c (track subroutine)
 */

#include	"iocslib.h"
#include	"string.h"
#include	"sted.h"

void	msg();
void	msg_clr();
void	snsclr();

int	trk_mix_sub();
int	trk_ext_sub();
int	repeat_comp();
int	repeat_ext();
int	same_meas_conv();
int	same_ext_sub();
void	trk_opt_sub();
int	tai_compres();
void	trk_quan();
void	note_ctrl();

int	trk_check();
int	trk_shift();
void	trk_delete();
int	size_change();
int	size_max();
int	size_add();
void	size_ref();

/***************************/
int	trk_mix_sub(unsigned char *in1,unsigned char *in2)
{
	int	ad1=0,ad2=0,po=0,st1=0,st2=0,ef1=0,ef2=0,lsad=-1;
	unsigned char	a1,a2,b1,b2,c1,c2,d1,d2;

	while(po<TRACK_SIZE){

		if(st1==st2){
			if(st1){
				if(lsad>=0 && dat[lsad]<0xf0 && dat[lsad+1]+st1<=240){
					dat[lsad+1]+=st1;
				}else{
					lsad=po;
					dat[po++]=0;dat[po++]=st1;
					dat[po++]=0;dat[po++]=0;
				}
				st1=0;st2=0;
			}
		}else{
			if(st1>st2){
				if(st2){
					if(lsad>=0 && dat[lsad]<0xf0 && dat[lsad+1]+st2<=240){
						dat[lsad+1]+=st2;
					}else{
						lsad=po;
						dat[po++]=0;dat[po++]=st2;
						dat[po++]=0;dat[po++]=0;
					}
					st1-=st2;st2=0;
				}
			}else{
				if(st1){
					if(lsad>=0 && dat[lsad]<0xf0 && dat[lsad+1]+st1<=240){
						dat[lsad+1]+=st1;
					}else{
						lsad=po;
						dat[po++]=0;dat[po++]=st1;
						dat[po++]=0;dat[po++]=0;
					}
					st2-=st1;st1=0;
				}
			}
		}


		if(ef1==1 && ef2==1){break;}

		if(ef2){
			if(st1){
				if(lsad>=0 && dat[lsad]<0xf0 && dat[lsad+1]+st1<=240){
					dat[lsad+1]+=st1;
				}else{
					lsad=po;
					dat[po++]=0;dat[po++]=st1;
					dat[po++]=0;dat[po++]=0;
				}
				st1=0;st2=0;
			}
			while(in1[ad1]<0xfe && po<TRACK_SIZE){
				dat[po++]=in1[ad1++];dat[po++]=in1[ad1++];
				dat[po++]=in1[ad1++];dat[po++]=in1[ad1++];
			}
			break;
		}
		if(ef1){
			if(st2){
				if(lsad>=0 && dat[lsad]<0xf0 && dat[lsad+1]+st2<=240){
					dat[lsad+1]+=st2;
				}else{
					lsad=po;
					dat[po++]=0;dat[po++]=st2;
					dat[po++]=0;dat[po++]=0;
				}
				st1=0;st2=0;
			}
			while(in2[ad2]<0xfe && po<TRACK_SIZE){
				dat[po++]=in2[ad2++];dat[po++]=in2[ad2++];
				dat[po++]=in2[ad2++];dat[po++]=in2[ad2++];
			}
			break;
		}


		if(st1==0){
			a1=in1[ad1++];b1=in1[ad1++];c1=in1[ad1++];d1=in1[ad1++];

			if(a1==0x98 || a1==0xf6){
				lsad=po;
				dat[po++]=a1;dat[po++]=0;
				dat[po++]=c1;dat[po++]=d1;

				while(in1[ad1]==0xf7 && po<TRACK_SIZE){
					dat[po++]=in1[ad1++];dat[po++]=in1[ad1++];
					dat[po++]=in1[ad1++];dat[po++]=in1[ad1++];
				}
			}else{
				if(a1==0xfe){ef1=1;}
				if(a1<0xfe){
					if(a1<0xf0){
						if(a1>=0x80 || (c1!=0 && d1!=0)){
							lsad=po;
							dat[po++]=a1;dat[po++]=0;
							dat[po++]=c1;dat[po++]=d1;
						}
					}else{
						if(a1==0xfd && po!=0){
							if(dat[po-4]<0xf0 && dat[po-3]==0){
								dat[po  ]=dat[po-4];dat[po+1]=dat[po-3];
								dat[po+2]=dat[po-2];dat[po+3]=dat[po-1];

								dat[po-4]=0xfd;
								lsad=po;
								po+=4;
							}else{
								dat[po]=0xfd;
								lsad=-1;
								po+=4;
							}
						}else{
							lsad=-1;
							dat[po++]=a1;dat[po++]=b1;
							dat[po++]=c1;dat[po++]=d1;
							b1=0;
						}
					}
				}
			}
			if(a1<0xf0){st1+=b1;}
		}

		if(st2==0){
			a2=in2[ad2++];b2=in2[ad2++];c2=in2[ad2++];d2=in2[ad2++];

			if(a2==0x98 || a2==0xf6){
				lsad=po;
				dat[po++]=a2;dat[po++]=0;
				dat[po++]=c2;dat[po++]=d2;

				while(in2[ad2]==0xf7 && po<TRACK_SIZE){
					dat[po++]=in2[ad2++];dat[po++]=in2[ad2++];
					dat[po++]=in2[ad2++];dat[po++]=in2[ad2++];
				}
			}else{
				if(a2==0xfe){ef2=1;}
				if(a2<0xfd){
					if(a2<0xf0){
						if(a2>=0x80 || (c2!=0 && d2!=0)){
							lsad=po;
							dat[po++]=a2;dat[po++]=0;
							dat[po++]=c2;dat[po++]=d2;
						}
					}else{
						lsad=-1;
						dat[po++]=a2;dat[po++]=b2;
						dat[po++]=c2;dat[po++]=d2;
						b2=0;
					}
				}
			}
			if(a2<0xf0){st2+=b2;}
		}
	}
	return po;
}

/***************************/
int	trk_ext_sub(int tr,char *exbuf)
{
	int	po;

	po=same_ext_sub(tr,0,tr_len[tr],0);
	if(po>TRACK_SIZE){msg("トラック容量不足です。");return 0;}
	po=repeat_ext(exbuf,po);
	if(po>TRACK_SIZE){msg("トラック容量不足です。");return 0;}

	return po;
}

/***************************/
int	repeat_comp(int tr)
{
	int	i=0,po=0,ln;

	ln=tr_len[tr];
	dclr();

	while(i<ln){
		int ll=0,ii;ii=i;
		while(trk[tr][ii]<0xf0){ii+=4;ll+=4;}

		if(trk[tr][i]<0xf0 && ll>4){
			while(ll>0){
				int blk=4,lpc=0;

				while(blk<ll){
					int cf=0,ii=i+blk,j;lpc=1;
					while(cf==0){
						for(j=0;j<blk;j++){
							if(trk[tr][i+j]>=0xf0||trk[tr][i+j]!=trk[tr][ii+j]){
								cf=1;break;
							}
						}
						if(cf==0){lpc++;}
						ii+=blk;
					}
					if((lpc>3)||(lpc>2 && blk>4)||(lpc>1 && blk>8)){
						goto cv;
					}
					blk+=4;
				}

cv:
				if((lpc>3)||(lpc>2 && blk>4)||(lpc>1 && blk>8)){
					int j;
					trk[tr][po]=0xf9;po+=4;
					for(j=0;j<blk;j++){trk[tr][po++]=trk[tr][i+blk+j];}
					trk[tr][po]=0xf8;trk[tr][po+1]=lpc;po+=4;
					i+=blk*lpc;
					break;
				}else{
					trk[tr][po++]=trk[tr][i++];trk[tr][po++]=trk[tr][i++];
					trk[tr][po++]=trk[tr][i++];trk[tr][po++]=trk[tr][i++];
					ll-=4;
				}

			}
		}else{
			trk[tr][po++]=trk[tr][i++];trk[tr][po++]=trk[tr][i++];
			trk[tr][po++]=trk[tr][i++];trk[tr][po++]=trk[tr][i++];
		}
	}

	tr_len[tr]=po;
	return(po);
}

/***************************/
int	repeat_ext(unsigned char *obuf,int ln)
{
	int	i,po=0;
	unsigned char	a,b,c,d;

	i=0;
	while(i<ln){
		if(po>=TRACK_SIZE){break;}
		a=dat[i];b=dat[i+1];c=dat[i+2];d=dat[i+3];

		if(a==0xf8){
			unsigned char	da;
			int	co,ad;
			if(b==0){co=256;}else{co=b;}
			if(co>=255){goto skip;}
			ad=po;da=0;
			while(ad>0){
				ad=ad-4;
				da=obuf[ad];
				if(da==0xf9){break;}
			}
			if(da==0xf9){
				int	ln,i1,i2;
				ln=(po-ad)-4;
				if(ad+ln*co>=TRACK_SIZE){po=TRACK_SIZE;break;}
				po=ad;
				for(i2=0;i2<ln;i2++){obuf[ad+i2]=obuf[ad+i2+4];}
				for(i1=0;i1<co;i1++){
					for(i2=0;i2<ln;i2++){obuf[po]=obuf[ad+i2];po++;}
				}
			}else{
				obuf[po]=a;obuf[po+1]=b;obuf[po+2]=c;obuf[po+3]=d;po=po+4;
			}
			if(po>TRACK_SIZE){break;}
		}else{
skip:
			obuf[po]=a;obuf[po+1]=b;obuf[po+2]=c;obuf[po+3]=d;po=po+4;
		}
		i=i+4;
	}
	snsclr();
	return(po);
}

/***************************/
int	same_meas_conv(int tr)
{
	int	i,j=0,k,match,count=0;
	int	po=0,m_max,ln,co=0,me=0,ad;
	int	add[2048],add2[1024],len[1024];

	m_max=add_set(tr,add);if(m_max<2){return(1);}
	undobuf(0);dclr();

	for(i=0;i<tr_len[tr];i=i+4){
		if(trk[tr][i]<128){
			if(trk[tr][i+2]==0||trk[tr][i+3]==0){
				trk[tr][i]=0;trk[tr][i+2]=0;trk[tr][i+3]=0;
			}
		}
	}

	msg("SAME MEAS 変換中...");

	for(i=0;i<m_max;i++){
		if(i<2048){po=add[i];}else{po=meas_add(i+1);}
		ln=meas_len(po);match=0;
		if(ln>4 && trk[tr][po+ln-4]==0xfc){ln=ln-4;}

		if(trk[tr][po]<0xfc && me>0){
			j=0;while(j<me ){
				if(ln==len[j] && dat[add2[j]]<0xfc){
					match=1;ad=add2[j];
					k=0;while(k<ln){
						if(trk[tr][po+k]!=dat[ad]){match=0;break;}
						k++;ad++;
					}
				}
				if(match!=0){break;}
				j++;
			}
		}
		if(match!=0 && j<1024 && add2[j]<65536-44){
			if(me<1024){add2[me]=co;len[me]=4;me++;}
			dat[co]=0xfc;dat[co+1]=j&0xff;dat[co+2]=j>>8;co=co+4;count++;
		}else{
			if(me<1024){add2[me]=co;len[me]=ln;me++;}
			for(k=0;k<ln;k++){dat[co]=trk[tr][po];po++;co++;}
		}
	}
	dat[co]=0xfe;co=co+4;

	for(i=0;i<co;i=i+4){
		if(dat[i]==0xfc){
			j=add2[dat[i+1]+(dat[i+2]&3)*256];
			if(dat[j]==0xfc){dat[i+1]=dat[j+1];dat[i+2]=dat[j+2];}
		}
	}

	if(size_change(tr,co+4)){return(0);}

	memcpy_l(trk[tr],dat,co);
	tr_len[tr]=co;

	if(count>0){
		msg(fstr(count,4));B_PRINT(" 小節変換しました。");
	}else{
		msg("重複した小節は有りません。");
	}
	snsclr();
	return(0);
}

/***************************/
int	same_ext_sub(int tr,int p1,int p2,int md)
{
	int	i,po=0,ad,max,count=0;
	unsigned char	a,b,c,d;
	int	add[2048];

	max=add_set(tr,add);

	i=p1;
	while(i<p2){
		if(po>=work_size){break;}
		a=trk[tr][i];b=trk[tr][i+1];c=trk[tr][i+2];d=trk[tr][i+3];
		if(a==0xfc && b+(c&3)*256<max){
next:			ad=add[b+(c&3)*256];
			if(i==ad){
				if(po>0 && dat[po-4]<0xfc){dat[po]=0xfd;po+=4;}
				a=0xfd;goto skip;}
			if(trk[tr][ad]==0xfc){
				b=trk[tr][ad+1];c=trk[tr][ad+2];
				if(ad==add[b+(c&3)*256]){goto skip;}
				goto next;
			}

			while(trk[tr][ad]<0xfc){
				if(po>=work_size){po+=4;break;}
				dat[po]=trk[tr][ad];dat[po+1]=trk[tr][ad+1];
				dat[po+2]=trk[tr][ad+2];dat[po+3]=trk[tr][ad+3];
				ad=ad+4;po=po+4;
			}
			if(po>work_size){break;}
			dat[po]=0xfd;po=po+4;count++;
		}else{
skip:
			dat[po]=a;dat[po+1]=b;dat[po+2]=c;dat[po+3]=d;po=po+4;
		}
		i=i+4;
		if(dat[po-4]<0xfc && trk[tr][i]==0xfc){dat[po]=0xfd;po=po+4;}
	}

	if(md){
		if(count>0){
			msg(fstr(count,4));B_PRINT(" 小節展開しました。");
		}else{
			msg("展開する小節は有りません。");
		}
	}

	snsclr();
	return(po);
}

/***************************/
void	trk_opt_sub(int tr,int po1,int po2)
{
	unsigned char	*ptr;
	int	ad,ad2=0,po3;
	po2+=po1;po3=po2;

	po2=tai_compres(tr,po1,po2);

	ptr=(unsigned char *)trk[tr];

	for(ad=0;ad<tr_len[tr];ad+=4){
		/*if(ad>=po1 && ad<po2){*/
		if(ad>po1 && ad<po2){
			if(ptr[ad]<128 && ptr[ad+1]==0 &&(ptr[ad+2]==0||ptr[ad+3]==0)){goto next;}

			if(ad2>po1 &&
				(ptr[ad2-4]==0xee || (ptr[ad2-4]==0xeb &&
				(ptr[ad2-2]>=7 || ptr[ad2-2]<=11 ))) &&

				ptr[ad2-4]==ptr[ad] && ptr[ad2-2]==ptr[ad+2] &&
				ptr[ad2-1]==ptr[ad+3] && ptr[ad2-3]+ptr[ad+1]<=240){
				ptr[ad2-3]+=ptr[ad+1];
			}else{
				if(ptr[ad]<128 && (ptr[ad+2]==0 || ptr[ad+3]==0)&& ad2>po1 &&
				(ptr[ad2-4]!=0xde||(ad2>4 && ptr[ad2-8]==0xde &&
				 ptr[ad2-4]==0xde && ptr[ad2-6]+1!=ptr[ad2-2]))&&

				(ptr[ad2-4]<0xf0 ||(ptr[ad2-4]<128 &&(ptr[ad2-3]==0||
				 ptr[ad2-2]==0||ptr[ad2-1]==0)))&& ptr[ad2-3]+ptr[ad+1]<=240){
					ptr[ad2-3]+=ptr[ad+1];
				}else{
					if(ptr[ad]<128 && (ptr[ad+2]==0 || ptr[ad+3]==0)&& ad2>po1 &&
					   ptr[ad2-4] == 0xf7 ){
						int cc=ad2-4;
						while(ptr[cc]==0xf7){cc-=4;}

						if( ptr[cc]!=0x98 ){goto rpls;}

						if( ptr[cc+1]+ptr[ad+1]<=240 ){
							ptr[cc+1]+=ptr[ad+1];
						} else {
							goto rpls;
						}

					}else{
rpls:
						ptr[ad2]=ptr[ad];ptr[ad2+1]=ptr[ad+1];
						ptr[ad2+2]=ptr[ad+2];ptr[ad2+3]=ptr[ad+3];
						ad2+=4;
					}
				}
			}
next:
			if(ad==po2-4){po3=ad2;}
		}else{
			ptr[ad2]=ptr[ad];ptr[ad2+1]=ptr[ad+1];
			ptr[ad2+2]=ptr[ad+2];ptr[ad2+3]=ptr[ad+3];
			ad2+=4;
		}
	}
	tr_len[tr]=ad2;

}

/***************************/
int	tai_compres(int tr,int po1,int po2)
{
	int	i,ln,ad,ch,po3=po2;
	unsigned char	a,b,c,d;

	unsigned char	*ptr;
	ptr=(unsigned char *)trk[tr];

	ch=(mch[tr])&0xff;
	ad=0;

	ln=tr_len[tr];
	for(i=0;i<ln;i+=4){
		a=ptr[i];b=ptr[i+1];c=ptr[i+2];d=ptr[i+3];

		if(i>=po1 && i<po2){

			if(a<128 && c>b && d>0){
				int cc=ch,ad2,sum;
				ad2=i;sum=0;

				while(ad2<ln){
					unsigned char	da=ptr[ad2];
					if(da==0xe6){cc=ptr[ad2+2];}

					if(ad2>i && cc==ch && a==da && ptr[ad2+2]!=0 && ptr[ad2+3]!=0 ){
						if(sum+ptr[ad2+2]<=255){
							c=ptr[ad2+2]+sum;
							/*if(ptr[i+2]<c)*/{
								ptr[i+2]=c;}
							ptr[ad2+2]=0;
							ptr[ad2+3]=0;
						}else{
							break;
						}
					}

					if(da<0xf0){sum+=ptr[ad2+1];if(sum>255 || sum>=c){break;}}

					if(da>=0xfc||da==0xf8||da==0xf9||da==0xe2||da==0xec){break;}
					ad2+=4;
				}
			}

			if(ptr[i]>=0xfc){ch=-1;}

			if(ptr[i]==0xe6){
				if(ptr[i+2]==ch){
					ptr[i]=0;ptr[i+2]=0;ptr[i+3]=0;
				}else{
					ch=ptr[i+2];

					if(ad && ptr[ad-4]==0xe6){
						ptr[ad-2]=ptr[i+2];
						ptr[i]=0;ptr[i+2]=0;ptr[i+3]=0;
					}
				}
			}

			if(i>po1 && (ptr[i]<0x80 &&(ptr[i+2]==0||ptr[i+3]==0))&&
				ad>0 && ptr[ad-4]<0xf0 && ptr[ad-3]+ptr[i+1]<=240){
					ptr[ad-3]+=ptr[i+1];
			}else{
				ptr[ad++]=ptr[i];ptr[ad++]=ptr[i+1];
				ptr[ad++]=ptr[i+2];ptr[ad++]=ptr[i+3];
			}
		}else{
			ptr[ad++]=ptr[i];ptr[ad++]=ptr[i+1];
			ptr[ad++]=ptr[i+2];ptr[ad++]=ptr[i+3];
		}
		if(i==po2-4){po3=ad;}
	}

	tr_len[tr]=ad;
	return(po3);
}

/***************************/
void	trk_quan(int tr,int po1,int po2)
{
	unsigned char	*ptr;
	int	ad,ad2=0,st=0,rt=0,hf,tt1=0,ofs=0,cf=0;

	ptr=(unsigned char *)trk[tr];

	while(st<2 || st>48){
		msg("QUANTIZE  RESOLUTION=");st=vinput(6,0x803);
		if(es!=0){msg_clr();return;}
	}
	while(rt<50 || rt>100){
		B_LOCATE(25,1);B_PRINT("RATE(%)=");rt=vinput(100,0x803);
		if(es!=0){msg_clr();return;}
	}
	hf=st>>1;

	undobuf(0);

	for(ad=0;ad<tr_len[tr];ad+=4){
		unsigned char a,b,c,d;
		a=ptr[ad];b=ptr[ad+1];c=ptr[ad+2];d=ptr[ad+3];

		if(ad>=po1 && ad<po2){
			if(a<0xf0){
				if(a<0x80){
					while(tt1>=st){tt1-=st;}

					if(tt1>0){
						int vt=0,ad3;

						if(tt1<hf || st==2){
							vt=-((tt1*rt)/100);
						}else{
							vt=((st-tt1)*rt)/100;
						}
						ad3=ad2;
						while(vt!=0 && ad3>0){
							ad3-=4;
							if(dat[ad3]>=0xfc){break;}

							if(dat[ad3]<0xf0){
								while(vt<0 && dat[ad3+1]>0){
									dat[ad3+1]--;vt++;
									ofs++;tt1--;
								}
								while(vt>0 && dat[ad3+1]<255){
									dat[ad3+1]++;vt--;
									ofs--;tt1++;
								}
								if(dat[ad3]>=0x80){break;}
							}
						}
					}
					cf=0;
				}else{
					cf+=b;
				}
				tt1+=b;

				if(ptr[ad]<0x80 && ptr[ad+1]>0){
					while(ofs>0 && b<255){b++;tt1++;ofs--;}
					while(ofs<0 && b>0  ){b--;tt1--;ofs++;}
				}
				if(tt1==st){cf=0;}
			}
		}

		dat[ad2++]=a;dat[ad2++]=b;dat[ad2++]=c;dat[ad2++]=d;

		if((a>=0xfc || ad+4==po2) && (ofs!=0)){
			int ad3=ad2-4;

			while(ofs!=0 && ad3>0){
				ad3-=4;
				if(dat[ad3]>=0xfc){break;}

				if(dat[ad3]<0xf0){
					while(ofs<0 && dat[ad3+1]>0){
						dat[ad3+1]--;ofs++;
					}
					while(ofs>0 && dat[ad3+1]<255){
						dat[ad3+1]++;ofs--;
					}
				}
			}
		}
	}

	memcpy_l(trk[tr],dat,ad2);tr_len[tr]=ad2;
	msg_clr();
}

/***************************/
void	note_ctrl(int po1,int po2)
{
	int	i,j,k,ad=0,ln=0,md=0,ke,st=0,tr=track;
	int	stt[256],stt2[256];
	unsigned char	a,b,c,d;

	unsigned char	*ptr;
	ptr=(unsigned char *)trk[tr];

	for(i=0;i<256;i++){stt[i]=0;stt2[i]=0;}

	if(cplen<0){
		int ad=po1,ln;
		ln=abs(cpadd-ad);if(!(cpadd>ad)){ad=cpadd;}
		po1=ad;po2=ad+ln;
	}else{/*po1=ad;*/po2=tr_len[track]-4;}

resel:
/*	msg("Mode: Sort(U/D) or Shift(G/S) =");
	msg("Setting Mode(Line/C/Rnd/O.R/ST,GT O.R) =");
*/
	msg("Setting Mode(L/C/R/O/T /U/D/G/S) =");

	while(B_KEYSNS()==0){}
	ke=B_KEYINP()&0xff;

	if(ke==0x1b){msg_clr();return;}
	if(ke=='U'||ke=='u'){md=1;}
	if(ke=='D'||ke=='d'){md=2;}
	if(ke=='G'||ke=='g'){md=3;}
	if(ke=='S'||ke=='s'){md=4;}

	if(ke=='L'||ke=='l'){md=16;}
	if(ke=='C'||ke=='c'){md=17;}
	if(ke=='R'||ke=='r'){md=18;}
	if(ke=='O'||ke=='o'){md=19;}
	if(ke=='T'||ke=='t'){md=20;}

	if(md<=0){goto resel;}


	if(md>=16){
		if(velo_ctrl(po1,po2-po1+4,md-15)!=0){md=0;goto resel;}
		goto ext;
	}



	while((md==3||md==4) &&(st<1||st>256)){
		msg("SHIFT ST =");
		st=vinput(0,3);if(es!=0){msg_clr();return;}
	}

	undobuf(0);

	for(i=po1;i<po2;i=i+4){
		a=ptr[i];b=ptr[i+1];c=ptr[i+2];d=ptr[i+3];

		if(a<128){
			if(ln==0){ad=i>>2;}
			ln++;
		}
		if(ln!=0 && (a>127 || (md<3 && a<128 && b>0)||
				      (md>2 && a<128 && b>2)||i==po2-4 )){

			unsigned int	*ptr2=(unsigned int *)ptr+ad,tm;

			if(ln>1){
				if(md==1 || md==2){
					if(a<128){ptr[i+1]=0;}
					for(j=0;j<ln-1;j++){
						for(k=j+1;k<ln;k++){
							if((md==1 && ptr2[j]>ptr2[k])||
							(md==2 && ptr2[j]<ptr2[k])){
								tm=ptr2[j];
								ptr2[j]=ptr2[k];
								ptr2[k]=tm;
							}
						}
					}
					if(a<128){ptr[i+1]=b;}
				}else{
					int ii=i-ln*4 +4 ;
					bunkatsu(st,ln-1,stt);
					bunkatsu(st,ln-1,&stt2[1]);
					for(j=0;j<ln;j++){
						if(md==3){
							if(ptr[ii+1]+stt[j]<=240 &&
							   ptr[ii+2]>stt2[j] &&
							   ptr[i +1]>stt[j]){
								ptr[ii+1]+=stt[j];
								ptr[ii+2]-=stt2[j];
								ptr[i +1]-=stt[j];
								stt2[j+1]+=stt2[j];
							}
						}else{
							if(ptr[ii+1]+stt[j]<=240 &&
							   ptr[i +1]>stt[j]){
								ptr[ii+1]+=stt[j];
								ptr[i +1]-=stt[j];
								stt2[j+1]+=stt2[j];
							}
						}
						ii+=4;
					}
				}

			}
			ln=0;
		}
	}
ext:
	cplen=0;
	msg_clr();
}

/***************************/
int	velo_ctrl(int ad,int ln,int vmo)
{
	int	ct,st=-128,st2=-128,i,cc=0,ke;

	unsigned char	*ptr;
	ptr=(unsigned char *)trk[track];

/*	if(cplen<0){
		ln=abs(cpadd-ad);if(!(cpadd>ad)){ad=cpadd;}
	}else{return 1;}
*/
	if( ln<8 ){return 0;}

	msg("Velocity Setting");

	while((vmo<4 && st<0)||st<-127 || st>127){
		B_LOCATE(18,1);
		if(vmo<3){B_PRINT("START=");}else{B_PRINT("LOW=");}

		st=vinput(0,3);if(es!=0){msg_clr();return 0;}
	}
	while(st2<0 || st2>127 || (vmo>2 && st2<st)){
		B_LOCATE(28,1);
		if(vmo<3){B_PRINT("END=");}else{B_PRINT("HIGH=");}
		st2=vinput(0,3);if(es!=0){msg_clr();return 0;}
	}

	undobuf(0);

	ct=-1;
	for(i=ad;i<ad+ln;i=i+4){
		if(ptr[i]<128 && ptr[i+2]!=0 && ptr[i+3]!=0){ct++;}
	}
	if(ct<2){return 0;}

	st2=(st2-st);
	for(i=ad;i<ad+ln;i=i+4){
		if(ptr[i]<128 && ptr[i+2]!=0 && ptr[i+3]!=0){
			if(st2!=0){
				if(vmo==1){
					ptr[i+3]=st+(st2*cc/ct);cc++;
				}else{
					if(vmo==2){

			int	i2,ct2=0,cc2=0,j2;

			i2=i;
			while(ptr[i2+1]==0 && ptr[i2+4]<128 && ptr[i2+6]!=0 && ptr[i2+7]!=0){
				i2+=4;ct2++;
			}
			if(ct2>0){
				for(j2=0;j2<=ct2;j2++){
					if(ptr[i]<128 && ptr[i+2]!=0 && ptr[i+3]!=0){
						ptr[i+3]=st+(st2*cc2/ct2);cc2++;
					}
					i+=4;
				}
				i-=4;
			}else{ptr[i+3]=st+st2;}

					}else{
						if(vmo==3){
							ptr[i+3]=st+(rand()*(st2+1)/32767);
						}else{
							if(vmo==4){
								int aa;
								aa=ptr[i+3];
								aa+=(rand()*(st2+1)/32767)+st;

								if(aa<=0){aa=1;}
								if(aa>127){aa=127;}
								ptr[i+3]=aa;
							}else{
								int aa,bb,f;
next:
								aa=ptr[i+1];
								bb=ptr[i+2];
								f=(rand()*(st2+1)/32767)+st;
								if(bb){aa+=f;}
								if(bb){bb+=f;}

								if(aa<0||aa>255||bb<0||bb>255){goto next;}
								ptr[i+1]=aa;ptr[i+2]=bb;
							}
						}
					}
				}
			}else{ptr[i+3]=st;}
		}
	}
	cplen=0;
	msg_clr();
	return 0;
}

/***************************/
int	trk_check(int ln)
{
	if(ln<=0){return 0;}

	if(tr_len[track]+ln<=tr_alc[track]){return(0);}
	ln-=(tr_alc[track]-tr_len[track]);
	return(size_add(track,ln));
}

/***************************/
int	trk_shift(int ad,int ln)
{
	if(trk_check(ln)!=0){return(-1);}
	memcpy_l(&trk[track][ad+ln],&trk[track][ad],tr_len[track]-ad);
	tr_len[track]=tr_len[track]+ln;
	return(0);
}

/***************************/
void	trk_delete(int ad,int ln)
{
	tr_len[track]=tr_len[track]-ln;
	memcpy_l(&trk[track][ad],&trk[track][ad+ln],tr_len[track]-ad);
}

/***************************/
int	size_change(int tr,int ln)
{
	if(tr_alc[tr]>=ln){return 0;}
	ln=ln-tr_alc[tr];
	return(size_add(tr,ln));
}

/***************************/
int	size_max(int tr)
{
	int fr;

	size_ref();fr=buff_free;
	if(fr+tr_alc[tr]>TRACK_SIZE){fr=TRACK_SIZE-tr_alc[tr];}
	return(size_add(tr,fr));
}

/***************************/
int	size_add(int tr,int size)
{
	unsigned char	*ptr_s,*ptr_d;

	if(buff_free<size){
		msg("トラックバッファ容量不足です。");return(-1);}

	if(tr_alc[tr]+size>TRACK_SIZE){
		msg("トラック容量不足です。");return(-1);}

	if(tr<35){
		int	i;

		for(i=35;i>tr;i--){
			ptr_s=trk[i];ptr_d=ptr_s+size;trk[i]=ptr_d;
			memcpy_l(ptr_d,ptr_s,tr_len[i]);
		}
	}

	tr_alc[tr]+=size;
	buff_free-=size;

	return 0;
}

/***************************/
void	size_ref()
{
	unsigned char	*ptr_s,*ptr_d;
	int	i,left=0,ln;

	for(i=1;i<36;i++){
		ln=tr_alc[i-1]-tr_len[i-1];
		left+=ln;
		buff_free+=ln;

		if(left>0){
			ptr_s=trk[i];ptr_d=ptr_s-left;
			trk[i]=ptr_d;tr_alc[i-1]-=ln;
			memcpy_l(ptr_d,ptr_s,tr_len[i]);
		}
	}
	ln=tr_alc[35]-tr_len[35];
	tr_alc[35]-=ln;buff_free+=ln;
}

/***************************/
