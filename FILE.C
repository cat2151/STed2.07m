/*
 * STed: file.c (file & play)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"basic0.h"
#include	"BASIC.h"
#include	"sted.h"

			/* 123456789012345678901234567890 */
/*static char	sted_id[]="STedV2.0(C)TURBO\r\n";*/		/* 18byte */

static char	rcp_id[] ="RCM-PC98V2.0(C)COME ON MUSIC\r\n";	/* 30byte */
static char	trk_id[] ="STedV2.0TRACK\r\n";			/* 15byte */
static char	prt_id[] ="RCPPR";				/*  5byte */

static char	rex_id[] ="RCM-PC98 USER EXCLUSIVE\r\n";	/* 25byte */
static char	res_id[] ="STedV2.0U_EXC\r\n";			/* 15byte */

static char	ras_id[] ="STedV2.0R_ASS\r\n";			/* 15byte */

/*prt rex -> grt gex v2.5g*/

void	msg();
void	msg_clr();
void	snsclr();
void	fnc_dis();
void	dpl();
void	tpl();
void	spcut();
char	*spadd();
char	*jstrupr();
int	str_val();
int	ctc();
int	strch();
int	meas_no();
int	meas_add();
int	dat_add();
int	meas_len();
int	add_set();
int	size_change();
void	trksize_c();
char	*trk_dis();
int	str_search();

int	dload();
int	dsave();
int	trk_load();
int	trk_save();
int	part_load();
int	part_save();
int	rex_load();
int	rex_save();
int	timsave();
int	timload();
int	gsdload();

void	dplay();
void	cdplay();
void	cntplay();
int	last_tempo();
int	lsp_para_set();
int	lsp_wait_chack();

void	uex_read();
void	uex_set();

void	hedset();
void	thedset();
void	hedread();
int	thedread();
char	*sread();
void	sset();
void	tim_buf_trns();
void	tim_var_read();
void	tim_name_read();
void	tim_name_set();
void	gsd_buf_trns();
void	gsd_var_read();

int	rcp_buf_put();
int	rcp_buf_get();

void	dclr();
void	same_cluc();
int	gomi_cut();
void	dinit();

void	all_note_off();
void	temporary_set();

void	cminit();
void	gsinit();

void	rhinit();
void	rhinit2();
void	user_exc_init();
void	user_exc_init2();
void	stgt_init();
void	stgt_init2();
void	asin_change();
void	asin_init();

/* ================================================== file */
int	dload(char *fna,int md)
{
	int	i,j,size,size2,ln,md2=0,stra=0,max;
	FILE	*fp;

	if(str_search(fna,".R36")>0 || str_search(fna,".r36")>0){md2=1;}

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}

	fread(hed,1,1414,fp);

	if( strcmp(sread(0,30),rcp_id)!=0 ){
		msg("違うフォーマットのファイルです。");fclose(fp);return(-1);}

	if(hed[0x1e6]==36){md2=1;}else{md2=0;}
	if(md2==0){max=18;}else{max=36;}
	if(md==1){stra=18;max=36;}else{dinit();hedread();}

	for(i=stra;i<max;i++){
		fread(hed,1,44,fp);size=thedread(i);size2=0;
		if( size>TRACK_SIZE ){size2=size;size=TRACK_SIZE;}

		if(size_change(i,size)){break;}
		ln=fread(trk[i],1,size,fp);

		if( ln!=size ){if(i==18){break;}msg("ファイルが壊れています。");size=ln;break;}

		size=gomi_cut(trk[i],size);
		tr_len[i]=size;trk[i][size-4]=0xfe;

		if( size2>0){
			msg("トラック容量不足です。");size2-=size;
			while( size2>0 ){
				if( size2>TRACK_SIZE ){j=TRACK_SIZE;}else{j=size2;}
				fread(dat,1,j,fp);size2=size2-j;
			}
		}
	}
	fclose(fp);
	trksize_c();
	return(0);
}

/***************************/
int	dsave(char *fna)
{
	int	i,ef=0,md2=0,max;
	FILE	*fp;

	dclr();
	if(str_search(fna,".R36")>0 || str_search(fna,".r36")>0){md2=1;}

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	for(i=18;i<36;i++){
		if(tr_len[i]>4 || trkmemo[i][0]!=0){md2=1;}
	}

	if(md2==0){max=18;}else{max=36;}

	hedset(max);fwrite(hed,1,1414,fp);

	i=0;
	while(i<max && ef==0){
		thedset(i,tr_len[i]+44);fwrite(hed,1,44,fp);
		if(fwrite(trk[i],1,tr_len[i],fp)!=tr_len[i]){ef=1;break;}
		i++;
	}
	fclose(fp);

	if(ef!=0){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	part_load(char *fna)
{
	int	ln,sz;
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	fread(hed,1,8,fp);ln=fread(dat,1,work_size,fp);fclose(fp);
	if( strcmp(sread(0,5),prt_id)!=0 ){
		msg("違うフォーマットのファイルです。");return(-1);}
	sz=hed[7]*256+hed[6];

	if( ln<sz ){msg("ファイルが壊れています。");sz=-1;}
	return(sz);
}

/***************************/
int	part_save(char *fna,int ln)
{
	int	fr;
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}
	sset(prt_id,0,5);hed[5]=track+1;hed[6]=ln&0xff;hed[7]=ln>>8;
	fwrite(hed,1,8,fp);fr=fwrite(dat,1,ln,fp);fclose(fp);

	if( fr!=ln ){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	trk_load(char *fna)
{
	int	ln,sz;
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}

	fread(hed,1,16,fp);
	if( strcmp(sread(0,15),trk_id)!=0 ){
		msg("違うフォーマットのファイルです。");fclose(fp);return(-1);}

	fread(hed,1,44,fp);ln=fread(dat,1,work_size,fp);fclose(fp);
	sz=hed[0]+hed[1]*256-44;
	if( ln<sz ){msg("ファイルが壊れています。");return(-1);}

	sz=gomi_cut(dat,sz);
	if(size_change(track,sz)){return -1;}

	thedread(track);memcpy_l(trk[track],dat,ln);tr_len[track]=sz;
	tr_pos[track][0][0]=0;tr_pos[track][0][1]=0;tr_pos[track][0][2]=0;
	tr_pos[track][1][0]=0;tr_pos[track][1][1]=0;tr_pos[track][1][2]=0;

	return(0);
}

/***************************/
int	trk_save(char *fna)
{
	int	fr,ln;
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	dclr();
	sset(trk_id,0,15);hed[15]=0x1a;fwrite(hed,1,16,fp);
	ln=tr_len[track];thedset(track,ln+44);fwrite(hed,1,44,fp);
	fr=fwrite(trk[track],1,ln,fp);fclose(fp);

	if( fr!=ln ){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	part_text_save(char *fna,int ad,int ln)
{
	int	i,j=0,fr,ll;
	FILE	*fp;
	char	tmp0[4096+1024];
	unsigned char a;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	i=ad;
	while(ln>0){
		int	lp;
		if(INPOUT(0xff)==27 ){msg("中止しました。");j=-1;break;}

		lp=0;tmp0[0]=0;
		while(lp<64 && ln>0){
			a=trk[track][i];

			if(a!=0xf7){
				strcat(tmp0,trk_dis(0,i,1));strcat(tmp0,"\r\n");
			}
			lp++;i+=4;ln-=4;
		}
		ll=strlen(tmp0);fr=fwrite(tmp0,1,ll,fp);

		if(fr<ll){msg("ディスクの空きが無くなりました。\x07");j=-1;break;}
	}
	fclose(fp);
	snsclr();return(j);
}

/***************************/
void	uex_read(int po,int no)
{
	int	ad,j;

	ad=po+no*48;if(po==16){ad=po;}

	strcpy(user_exc_memo[no],sread(ad,24));
	if(hed[ad+24]==0 && hed[ad+47]==0){
		user_exc_data[no][0]=0xf7;
	}else{
		for(j=0;j<24;j++){user_exc_data[no][j]=hed[ad+j+24];}
	}
}

void	uex_set(int po,int no)
{
	int	ad,j;

	ad=po+no*48;if(po==16){ad=po;}

	sset(user_exc_memo[no],ad,24);
	for(j=0;j<24;j++){hed[ad+j+24]=user_exc_data[no][j];}
}

/***************************/
int	rex_load(char *fna,int mo)
{
	int	ln,i,sz;
	FILE	*fp;

	if(mo<0){sz=410;}else{sz=48+16;}

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	ln=fread(hed,1,sz,fp);fclose(fp);
	if( ln!=sz ){msg("ファイルが壊れています。");return -1;}

	if(mo<0){
		if( strcmp(sread(0,25),rex_id)!=0 ){
			msg("違うフォーマットのファイルです。");return(-1);}

		for(i=0;i<8;i++){uex_read(26,i);}
	}else{
		if( strcmp(sread(0,15),res_id)!=0 ){
			msg("違うフォーマットのファイルです。");return(-1);}

		uex_read(16,mo);
	}

	return(0);
}

/***************************/
int	rex_save(char *fna,int mo)
{
	int	ln,i,sz;
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	if(mo<0){sz=410;}else{sz=48+16;}

	if(mo<0){
		sset(rex_id,0,25);hed[25]=0x1a;
		for(i=0;i<8;i++){uex_set(26,i);}
	}else{
		sset(res_id,0,15);hed[15]=0x1a;
		uex_set(16,mo);
	}

	ln=fwrite(hed,1,sz,fp);fclose(fp);

	if( ln!=sz ){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	ras_load(char *fna)
{
	int	ln,i;
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	ln=fread(hed,1,528,fp);fclose(fp);
	if( ln!=528 ){msg("ファイルが壊れています。");return -1;}

	if( strcmp(sread(0,15),ras_id)!=0 ){
		msg("違うフォーマットのファイルです。");return(-1);}

	for(i=0;i<32;i++){
		strcpy(rhyna[i],sread(0x10+i*16,14));
		rhyno[i][0]=hed[0x10+i*16+14];rhyno[i][1]=hed[0x10+i*16+15];
	}

	return(0);
}

/***************************/
int	ras_save(char *fna)
{
	int	ln,i;
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}

	sset(ras_id,0,15);hed[15]=0x1a;

	for(i=0;i<32;i++){
		sset(rhyna[i],0x10+i*16,14);
		hed[0x10+i*16+14]=rhyno[i][0];hed[0x10+i*16+15]=rhyno[i][1];
	}

	ln=fwrite(hed,1,528,fp);fclose(fp);

	if( ln!=528 ){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	timload(char *fna)
{
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	fread(cm6,1,22601,fp);fclose(fp);
	tim_name_read();asin_change();
	return(0);
}

/***************************/
int	timsave(char *fna)
{
	int	fr;
	FILE	*fp;

	if(!(fp= fopen(fna,"wb"))){msg("ファイルがオープン出来ません。");return(-1);}
	fr=fwrite(cm6,1,22601,fp);fclose(fp);
	if( fr!=22601 ){msg("ファイルの書き込みに失敗しました。\x07");unlink(fna);return(-1);}
	return(0);
}

/***************************/
int	gsdload(char *fna)
{
	FILE	*fp;

	if(!(fp= fopen(fna,"rb"))){msg("ファイルが見付かりません。");return(-1);}
	fread(gsd,1,4096,fp);fclose(fp);
	return(0);
}

/***************************/
char	*sread(int a,int b)
{
static	char	s[256];
	int	i;

	for(i=0;i<b;i++){s[i]=hed[i+a];}
	if(knck(s,b-1)){b--;}

	s[b]=0;spcut(s);return(s);
}

/***************************/
void	sset(char *s,int a,int b)
{
	int	i;

	for(i=0;i<b;i++){hed[a+i]=32;}
	while(*s!=0){hed[a++]=*s++;}
}

/***************************/
void	hedset(int max)
{
	int	i;

	hed[30]=0;hed[31]=0;sset(rcp_id,0,30);sset(mtitle,0x20,64);
	i=str_search(cm6_file,".");if(i>8){cm6_file[8]=0;strmfe(cm6_file,cm6_file,"CM6");}
	i=str_search(gsd_file,".");if(i>8){gsd_file[8]=0;strmfe(gsd_file,gsd_file,"GSD");}

	for(i=0x1b0;i<=0x1bf;i++){hed[i]=0x40;}
	for(i=0x1e8;i<=0x1f5;i++){hed[i]=0;}

	for(i=0x1c6;i<0x1e5;i++){hed[i]=0;}
	i=strlen(cm6_file);if(i!=0){sset(cm6_file,0x1c6,12);}
	i=strlen(gsd_file);if(i!=0){sset(gsd_file,0x1d6,12);}

	hed[0x1e6]=max;
	hed[0x1e7]=tbase>>8;hed[0x1c0]=tbase;

	hed[0x1c1]=mtempo;hed[0x1c2]=beat1;hed[0x1c3]=beat2;
	hed[0x1c4]=bkey;hed[0x1c5]=pbias;
	for(i=0;i<12;i++){sset(memo[i],0x60+i*28,28);}

	for(i=1;i<17;i++){
		int a1=tim_asin[i],a2=tim_asin[i+16];
		if(a1>15){a1=0;}
		if(a2>15){a2=0;}
		hed[0x1f6-1+i]=a2*16+a1;
	}

	for(i=0;i<32;i++){
		sset(rhyna[i],0x206+i*16,14);
		hed[0x206+i*16+14]=rhyno[i][0];hed[0x206+i*16+15]=rhyno[i][1];
	}
	for(i=0;i<8;i++){
		uex_set(0x406,i);
	}
}

/***************************/
void	hedread()
{
	int	i,co;

	strcpy(mtitle,sread(0x20,64));
	strcpy(cm6_file,sread(0x1c6,16));strcpy(gsd_file,sread(0x1d6,16));
	if(strlen(cm6_file)>15){gsd_file[0]=0;}

	tbase= hed[0x1c0];
	if( hed[0x1e6]!=0){tbase+=(hed[0x1e7]<<8);}

	mtempo= hed[0x1c1];beat1= hed[0x1c2];beat2= hed[0x1c3];
	if(beat2<2 || beat2>16){beat2=4;}
	if(beat1<1 || beat1>beat2*2){beat1=beat2;}

	bkey=  hed[0x1c4];pbias= hed[0x1c5];
	for(i=0 ;i<= 11;i++){strcpy(memo[i],sread(0x60+i*28,28));}

/*	asin_init();*/

	co=0;
	if((cnfflag&0x0080)==0){
		if((hed[0x1f6]&15)!=0 || (hed[0x1f6]==0x70 && hed[0x1f6+1]==0x71)){
			for(i=1 ;i<= 16;i++){
				int	d=hed[0x1f6-1+i];
				if(d!=0){co++;}
				tim_asin[i]=d&15;tim_asin[i+16]=(d>>4)&15;
/*
if((d&15)!=0){tim_asin[i]=d&15;}
if((d>>4)!=0){tim_asin[i+16]=d>>4;}
*/
			}
		}
	}
	if(co==0){asin_init();}

	for(i=0;i<32;i++){
		strcpy(rhyna[i],sread(0x206+i*16,14));
		rhyno[i][0]=hed[0x206+i*16+14];rhyno[i][1]=hed[0x206+i*16+15];
	}
	for(i=0;i<8;i++){
		uex_read(0x406,i);
	}
}

/***************************/
void	thedset(int tr,int sz)
{
	hed[0]=(sz&0xfc)+((sz>>16)&3);hed[1]=(sz>>8)&0xff;
	hed[2]=trno[tr];hed[3]=trrhy[tr];hed[4]=mch[tr]-1;
	hed[6]=trst[tr];hed[5]=trkey[tr];hed[7]=trmod[tr];if(hed[7]==3){hed[7]=4;}
	sset(trkmemo[tr],8,36);
}

/***************************/
int	thedread(int tr)
{
	int	size;

	size=(hed[0]&0xfc)+((hed[0]&3)*256+hed[1])*256-44;
	trno[tr]=hed[2];trrhy[tr]=hed[3];mch[tr]=hed[4]+1;if(mch[tr]>32){mch[tr]=0;}
	trkey[tr]=hed[5];trst[tr]=hed[6];trmod[tr]=hed[7];if(trmod[tr]==3){trmod[tr]=4;}
	strcpy(trkmemo[tr],sread(8,36));
	return(size);
}

/***************************/
int	bufload()
{
	int	bpo=0,i,size,size2,max;

	if( (rcd->data_valid==0 && rcd->tone_valid==0 ) ||(rcd->fmt!=2 && rcd->fmt!=1)){
		msg("バッファに有効なデータが有りません。");return(-1);
	}

	if( rcd->data_valid!=0 ){
		bpo=rcp_buf_get(hed,0,1414);
		if( strcmp(sread(0,30),rcp_id)!=0 ){
			msg("バッファに有効なデータが有りません。");return(-1);
		}
		dinit();
		hedread();

		if(rcd->fmt==1){max=18;}else{max=36;}
		if(hed[0x1e6]==36){max=36;}else{max=18;}

		for(i=0;i<max;i++){
			bpo=rcp_buf_get(hed,bpo,44);
			if((rcd->totalcount)<bpo){break;}

			size=thedread(i);size2=0;
			if( size>TRACK_SIZE ){size2=size;size=TRACK_SIZE;}
			if(size_change(i,size)){break;}

			bpo=rcp_buf_get(trk[i],bpo,size);
			size=gomi_cut(trk[i],size);
			tr_len[i]=size;trk[i][size-4]=0xfe;
			if( size2>0){msg("トラック容量不足です。");bpo=bpo+(size2-size);}
		}
		strcpy(rcp_file,rcd->filename);
		if(rcp_file[0]!=0){
			if(max==18){
				if(str_search(rcp_file,".R36")==0 && str_search(rcp_file,".r36")==0){
					strmfe(rcp_file,rcp_file,"RCP");
				}
			}else{
				if(str_search(rcp_file,".RCP")==0 && str_search(rcp_file,".rcp")==0){
					strmfe(rcp_file,rcp_file,"R36");
				}
			}
		}
		if( rcd->tone_valid==0 ){cminit();}
		if( rcd->gsd_valid==0 ){gsinit();}
	}

	if( rcd->tone_valid!=0 ){
		tim_var_read();
		if(cm6_file[0]!=0){strmfe(cm6_file,cm6_file,"CM6");asin_change();}
	}
	if( rcd->gsd_valid!=0 ){
		gsd_var_read();
		if(gsd_file[0]!=0){strmfe(gsd_file,gsd_file,"GSD");}
	}
	trksize_c();
	return(0);
}

/***************************/
int	bufset()
{
	int	bpo,i;

	all_note_off(1);rcd->data_valid=0;dclr();
	hedset(36);bpo=rcp_buf_put(hed,0,1414);

	for(i=0;i<36;i++){
		same_cluc(i,0);
		thedset(i,tr_len[i]+44);bpo=rcp_buf_put(hed,bpo,44);
		bpo=rcp_buf_put(trk[i],bpo,tr_len[i]);
	}

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return(-1);}

	rcd->fmt=2;rcd->totalcount=bpo;rcd->data_valid=1;rcd->word_valid=0;
	rcd->tone_valid=0;rcd->gsd_valid=0;
	if(cm6_file[0]!=0){tim_buf_trns();}
	if(gsd_file[0]!=0){gsd_buf_trns();}
	strcpy(rcd->filename,rcp_file);
	strcpy(rcd->tonename,cm6_file);
	strcpy(rcd->gsdname,gsd_file);
	return(0);
}

/***************************/
int	bufload_trk(int tr)
{
	int	bpo=0,size;

	if( (rcd->data_valid==0) || rcd->fmt!=99 ){
		msg("バッファに有効なデータが有りません。");return(-1);
	}

	bpo=rcp_buf_get(hed,bpo,44);size=thedread(tr);
	if( size>TRACK_SIZE ){size=TRACK_SIZE;msg("トラック容量不足です。");}
	if(size_change(tr,size)){goto ext;}

	bpo=rcp_buf_get(trk[tr],bpo,size);
	size=gomi_cut(trk[tr],size);
	tr_len[tr]=size;trk[tr][size-4]=0xfe;

/*	trksize_c();*/
ext:
	rcd->fmt=0;rcd->data_valid=0;rcd->word_valid=0;
	return(0);
}

int	bufset_trk(int tr)
{
	int	bpo=0;

	all_note_off(1);rcd->fmt=0;rcd->data_valid=0;/*dclr();*/

	same_cluc(tr,0);
	thedset(tr,tr_len[tr]+44);bpo=rcp_buf_put(hed,bpo,44);
	bpo=rcp_buf_put(trk[tr],bpo,tr_len[tr]);

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return(-1);}

	rcd->fmt=99;rcd->totalcount=bpo;rcd->data_valid=1;rcd->word_valid=0;
	return(0);
}

/***************************/
/* ================================================== play */
void	dplay(int mo,int tr,int meas)
{
	int	bpo,i,f;
	char	tmp1[256];
	FILE	*fp;

	if((cmdflag&0x8000)!=0 && mo<2){mo=1-mo;}

	if(rcd->act!=0 && mo!=0 && meas==0){
		mo+=8;
		if(mplay[mo][0]=='&'){goto start;}
		if(mplay[mo][0]=='%'){goto resm;}
	}
start:
	all_note_off(1);rcd->data_valid=0;
	msg_clr();poft=0;
	hedset(36);bpo=rcp_buf_put(hed,0,1414);

	for(i=0;i<36;i++){
		if((trmod[i]&1)!=0){
			thedset(i,48);dat[0]=0xfe;
			bpo=rcp_buf_put(hed,bpo,44);
			bpo=rcp_buf_put(dat,bpo,4);
		}else{
			same_cluc(i,0);
			thedset(i,tr_len[i]+44);
			bpo=rcp_buf_put(hed,bpo,44);
			bpo=rcp_buf_put(trk[i],bpo,tr_len[i]);
		}
	}

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return;}

	rcd->fmt=2;rcd->totalcount=bpo;
	rcd->data_valid=1;rcd->word_valid=0;
	rcd->tone_valid=0;rcd->gsd_valid=0;

	(*rcd->init)();
	if(cm6_file[0]!=0){tim_buf_trns();}
	if(gsd_file[0]!=0){gsd_buf_trns();}

	if(cm6_file[0]==0 && gsd_file[0]==0){
		if(init_exc_data[0]==255){(*rcd->setup)();
		}else{(*rcd->mix_out)(init_exc_data);twait(5);}
	}

	if(mo!=0 && meas==0){
		strcpy(tmp1,rcp_path);strcat(tmp1,rcp_file);strmfe(tmp1,tmp1,"wrd");
		if(fp=fopen(tmp1,"rb")){
			int	i,j,sz;
			unsigned char	a;

			sz = rcd->wordcap;
			j=fread(dat,1,work_size,fp);fclose(fp);bpo=0;
			for(i=0;i<j;i++){
				a=dat[i];
				if(a!=0x0a){
					if(a==0x0d ){a=0;}
					rcd->word_adr[bpo]=a;bpo++;
					if(bpo+1>=sz){break;}
				}
			}
			rcd->word_adr[bpo]=26;rcd->word_valid=1;
		}
resm:
		strcpy(rcd->filename,rcp_file);
		strcpy(rcd->tonename,cm6_file);
		strcpy(rcd->gsdname,gsd_file);

		fnc_dis(-1);C_FNKMOD(0);dpl(0);B_COLOR(3);
		if(mplay[mo][0]=='%' || mplay[mo][0]=='&'){
			strcpy(tmp1,&mplay[mo][1]);
			if(rcd->act==0 || mplay[mo][0]=='&'){(*rcd->begin)();}
		}else{strcpy(tmp1,mplay[mo]);}
		f=exe(tmp1);
		B_COLOR(3);fnc_dis(-1);C_FNKMOD(3);B_CLR_AL();C_CUROFF();
		CRTMOD(16+256);OnGraphic();dpl(1);tpl(1);
		WINDOW(0,0,1023,1023);

		if( f<0 ){msg("パネルが起動出来ません。");}
	}else{
		if(meas>1){rcd->tar_trk = tr;rcd->tar_bar = meas-1;rcd->sts = 2;}
		(*rcd->begin)();
	}
	return;
}

/************************** meas play */
void	cdplay(int tr,int po,int ln,int md)
{
	int	bpo,co,i,j,ad,meas,tempo,pr,dumy=track;
	int	me_ad[2048];

	all_note_off(1+1);rcd->data_valid=0;msg_clr();if(ln==0){return;}
	hedset(36);bpo=rcp_buf_put(hed,0,1414);
	meas=meas_no(po);if((cmdflag&0x4000)==0){pr=lsp_wait_chack(meas);}
	tempo=last_tempo(meas);
	poft=meas-1;if(md==0){if(po>0){poft+=0x10000;}}

	for(i=0;i<36;i++){
		track=i;co=0;

		if((trmod[i]&1)==0){
			if((cmdflag&0x4000)==0 && tr!=i){
				ad=meas_add(meas);if(ad<0){ad=tr_len[i]-4;}
				add_set(i,me_ad);
				co=lsp_para_set(i,ad,me_ad,pr);
				j=ad;
			}

			if( tr==i ){
				add_set(i,me_ad);

				if(md==0){
					if((cmdflag&0x4000)==0){
						co=lsp_para_set(i,po,me_ad,pr);
					}else{
						co=lsp_para_set(i,po,me_ad,lsp_para_set(i,po,me_ad,-1));
					}
				}
				j=po;

				if(po>0){
					if( tempo>=0){
						dat[co]=0xe7;dat[co+1]=1;dat[co+2]=tempo;dat[co+3]=0;
						co=co+4;
					}
					dat[co]=0xfd;co=co+4;
				}

				while(ln>0){
					dat[co]=trk[i][j];dat[co+1]=trk[i][j+1];
					dat[co+2]=trk[i][j+2];dat[co+3]=trk[i][j+3];
					if(dat[co]==0xfc){
						int	same,ad;
						same=dat[co+1]+(dat[co+2]&3)*256;
						if(meas<same+1){
							if(po>0){same++;}
							ad=dat_add(co+4,same+1+1-meas);
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
							co+=4;j+=4;ln-=4;
							dat[co]=trk[i][j];dat[co+1]=trk[i][j+1];
							dat[co+2]=trk[i][j+2];dat[co+3]=trk[i][j+3];
						}
					}
					if(dat[co]==0xfe || co+4>=work_size ){break;}
					co=co+4;
					if(trk[i][j]<0xfc && trk[i][j+4]==0xfc){dat[co]=0xfd;co=co+4;}
					ln=ln-4;j=j+4;
				}
			}
		}
		dat[co]=0xfe;co=co+4;
		thedset(i,co+44);bpo=rcp_buf_put(hed,bpo,44);
		bpo=rcp_buf_put(dat,bpo,co);
	}

	track=dumy;

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return;}

	rcd->fmt=2;rcd->totalcount=bpo;rcd->data_valid=1;rcd->word_valid=0;
	rcd->tone_valid=0;rcd->gsd_valid=0;

	(*rcd->init)();
	if(cm6_file[0]==0 && gsd_file[0]==0){
		if(init_exc_data[0]==255){(*rcd->setup)();
		}else{(*rcd->mix_out)(init_exc_data);twait(5);}
	}
	(*rcd->begin)();
	return;
}

/************************** point play */
void	cntplay(int tr,int po)
{
	int	bpo,i,j,co,ad,meas,dumy,pr,tempo;
	int	me_ad[2048];

	if((cmdflag&0x80)!=0){dplay(0,tr,meas_no(po));return;}
	all_note_off(1+1);rcd->data_valid=0;msg_clr();dumy=track;
	hedset(36);bpo=rcp_buf_put(hed,0,1414);
	meas=meas_no(po);pr=lsp_wait_chack(meas);tempo=last_tempo(meas);
	poft=meas-1;if(po>0){poft+=0x10000;}

	for(i=0;i<36;i++){
		co=0;
		if((trmod[i]&1)==0){
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
						ad=dat_add(co+4,same+1+1-meas);
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
		}
		dat[co]=0xfe;co=co+4;
		thedset(i,co+44);bpo=rcp_buf_put(hed,bpo,44);
		bpo=rcp_buf_put(dat,bpo,co);
	}

	track=dumy;

	if((rcd->bufcap)<bpo){msg("RCD バッファ容量不足です。");return;}

	rcd->fmt=2;rcd->totalcount=bpo;rcd->data_valid=1;rcd->word_valid=0;
	rcd->tone_valid=0;rcd->gsd_valid=0;

	(*rcd->init)();
	if(cm6_file[0]==0 && gsd_file[0]==0){
		if(init_exc_data[0]==255){(*rcd->setup)();
		}else{(*rcd->mix_out)(init_exc_data);twait(5);}
	}
	(*rcd->begin)();
	return;
}

/***************************/
int	last_tempo(int meas)
{
	int	i,ad,dumy,tm_me=-1,tmpo=-1,b;

	dumy=track;

	for(i=0;i<36;i++){
		if((trmod[i]&1)==0){
			track=i;ad=meas_add(meas);if(ad<0){ad=tr_len[i]-4;}
			while(ad>=0 ){
				if( trk[i][ad]==0xe7 ){
					b=meas_no(ad);if(b>tm_me){tmpo=trk[i][ad+2];tm_me=b;}
					break;
				}
				ad=ad-4;
			}
		}
	}
	track=dumy;
	return(tmpo);
}

/***************************/
int	lsp_para_set(int tr,int po,int *me_ad,int wcc)
{
	int	co=0,j,jj=0,i,pa=-1,exf=0;
	int	wa=0,wa2=0,retad=0;
	unsigned char	d;
	unsigned char	*ptr;
	int	ctad[256];
	int	rba=0;

	if(po==0 || lsp_wait<0){return(0);}
	for(i=0;i<128;i++){ctad[i]=-1;}

	ptr=(unsigned char *)trk[tr];

	if(wcc>0){
		int tst=0;
		wa=(mtempo>>6)*(tbase/48)*lsp_wait+1;wa2=wa>>1;wcc=(wcc>>3)*wa;

		while(jj<po &&(jj==0||tst<=16||ptr[jj]==0xe6||ptr[jj]==0xf6
			||(ptr[jj]>=0x90 && ptr[jj]<=0x98) )){

			if(ptr[jj]==0xde||(ptr[jj]>=0x90 && ptr[jj]<=0x98)){exf=1;}

			if(ptr[jj]<0xf0){
				if(ptr[jj]==0xde){jj+=4;break;}

				tst+=ptr[jj+1];
				if(tst>16){break;}
			}

			jj=jj+4;

			while(ptr[jj]==0xf7){jj+=4;}
			if(ptr[jj+1]>2 && exf){break;}
		}
		if(exf==0){dat[0]=0;dat[1]=40;dat[2]=0;dat[3]=0;co=4;}
	}

	for(j=0;j<po;j=j+4){
top:
		if(exf!=0 && j>=jj){dat[co]=0;dat[co+1]=40;dat[co+2]=0;dat[co+3]=0;co+=4;exf=0;}
		d=ptr[j];

		if( d>127 ){
			if( d==0xeb ){
				d=ptr[j+2];
				if( d==0 || d==32 || d==6 || d==38 || (d>97 && d<102) ){
					if(co>=0 && dat[co-4]==0xeb && dat[co-2]==d){
						dat[co-1]=ptr[j+3];
					}else{
						dat[co]=0xeb;dat[co+1]=0;dat[co+2]=d;
						dat[co+3]=ptr[j+3];
						if(wcc>0){dat[co+1]=wa2;wcc=wcc-wa2;wa2=wa-wa2;}
						co=co+4;
					}
				}else{
					if(d!=121){ctad[d]=j;}
					else{for(d=0;d<128;d++){ctad[d]=-1;}}
				}
			}else{
				if( d==0xf6 ){
					while(ptr[j+4]==0xf7){j+=4;}
					goto top;
				}

/*
   41 10 42 12 cs 00 01 01 01 ss F7 <--- PART A01の受信ポートをBにする
   41 10 42 12 cs 00 01 17 00 ss F7 <--- PART B01の受信ポートをAにする
*/
				if( d==0xdd ){rba=ptr[j+2]*256+ptr[j+3];}
				if( d==0xde ){if(rba==1 && ptr[j+2]<0x17+16){goto outp;}}
				if( d==0x98 ){
					if(ptr[j+6]==0x41 && ptr[j+10]==0x42 &&(
					  (ptr[j+14]==0x83 && ptr[j+15]==0 && ptr[j+18]==0x01 && ptr[j+22]<0x17+16)||
					  (ptr[j+14]==0 && ptr[j+15]==0x01 && ptr[j+18]<0x17+16)
					)){goto outp;}
				}
				if( d>=0x90 && d<=0x97 ){
					unsigned char *pt2;
					pt2=(unsigned char *)user_exc_data[d&0xf];

					if(pt2[0]==0x41 && pt2[2]==0x42 &&(
					  (pt2[4]==0x83 && pt2[5]==0 && pt2[6]==0x01 && pt2[7]<0x17+16)||
					  (pt2[4]==0 && pt2[5]==0x01 && pt2[6]<0x17+16)
					)){goto outp;}
				}


				if( d==0xe6 ){
outp:
/*control*/
	for(i=127;i>0;i--){
		int	j=ctad[i];
		if(j>=0){
			dat[co]=0xeb;dat[co+1]=0;dat[co+2]=i;dat[co+3]=ptr[j+3];
			if(wcc>0){dat[co+1]=wa2;wcc=wcc-wa2;wa2=wa-wa2;}
			co=co+4;ctad[i]=-1;
		}
	}
/*pitch*/
	if(pa>=0){
		dat[co]=0xee;dat[co+1]=0;dat[co+2]=ptr[pa+2];dat[co+3]=ptr[pa+3];
		if(wcc>0){dat[co+1]=wa2;wcc=wcc-wa2;wa2=wa-wa2;}
		co=co+4;pa=-1;
	}

				}
				if( d<0xee && d!=0xe7 ){
					dat[co]=d;dat[co+1]=0;dat[co+2]=ptr[j+2];
					dat[co+3]=ptr[j+3];
				if((d==0xdd || d==0xdf || d==0xe6)||(d==0xde && \
				(ptr[j+4]==0xde && ptr[j+2]+1==ptr[j+6]))){
					if(wcc<0){goto skip02;}
					if(wcc>0){goto skip01;}
				}else{
					if(wcc>0){
						if(d==0xde){co+=4;dat[co]=0;dat[co+1]=wa;dat[co+2]=0;dat[co+3]=0;
						wcc-=wa;goto skip01;}
						if(wa2>0){dat[co+1]=wa2;}
					}
					if(wcc<0){
						if(d==0xde){co+=4;}
					}
				}
				wcc=wcc-wa2;wa2=wa-wa2;
skip01:
					co=co+4;
				}
skip02:
				if( d>=0xfc && retad!=0){j=retad;retad=0;goto top;}

				if( d==0xfc ){
					int	ii,jj,ct;
					jj=j;
resame:
					ct=ptr[jj+1]+(ptr[jj+2]&3)*256;ii=me_ad[ct];

					if(jj!=ii){
						jj=ii;if(ptr[ii]==0xfc){goto resame;}
						retad=j+4;j=ii;goto top;
					}
				}
				if( d==0xee ){pa=j;}

			}
		}
	}
/*control*/
	for(i=127;i>0;i--){
		int	j=ctad[i];
		if(j>=0){
			dat[co]=0xeb;dat[co+1]=0;dat[co+2]=i;dat[co+3]=ptr[j+3];
			if(wcc>0){dat[co+1]=wa2;wcc=wcc-wa2;wa2=wa-wa2;}
			co=co+4;
		}
	}
/*pitch*/
	if(pa>=0){
		dat[co]=0xee;dat[co+1]=0;dat[co+2]=ptr[pa+2];dat[co+3]=ptr[pa+3];
		if(wcc>0){dat[co+1]=wa2;wcc=wcc-wa2;wa2=wa-wa2;}
		co=co+4;
	}
	if(wcc>0){
		while(wcc>0){
			if(wcc>255){dat[co+1]=255;wcc=wcc-255;}else{dat[co+1]=wcc;wcc=0;}
			dat[co]=0;dat[co+2]=0;dat[co+3]=0;co=co+4;
		}
	}

	return(co);
}

/*
0xE6:M.CH 0xEC:PROG 0xed:AFT.K 0xea:AFT.C 0xee:PITCH 0xdd:R.BASE 0xde:R.PARA 0xe7:TEMPO
0xeb:CONTROL(1:MODULATION 7:VOLUME 10:PANPOT 11:EXPRESSION 64:HOLD 121:RESET A.C.)
*/

/***************************/
int	lsp_wait_chack(int meas)
{
	int	cc=0,i,a,ad,dummy;
	int	me_ad[2048];

	dummy=track;
	for(i=0;i<36;i++){
		if((trmod[i]&1)==0){
			track=i;add_set(i,me_ad);ad=meas_add(meas);if(ad<0){ad=tr_len[i]-4;}
			a=lsp_para_set(track,ad,me_ad,-1);if( a>cc ){cc=a;}
		}
	}
	track=dummy;
	return(cc);
}

/***************************/
void	tim_buf_trns()
{
	memcpy(rcd->tone_adr,cm6,22601);strcpy(rcd->tonename,cm6_file);
	rcd->fmt=2;rcd->tone_valid=1;
}

void	tim_var_read()
{
	memcpy(cm6,rcd->tone_adr,22601);strcpy(cm6_file,rcd->tonename);
	tim_name_read();
}

void	tim_name_read()
{
	int	i,j;

	for(i=0;i<64;i++){
		tim_name[i+128][10]=0;
		for(j=0;j<10;j++){tim_name[i+128][j]=cm6[0xe34+i*256+j];}
	}
}

void	gsd_buf_trns()
{
	memcpy(rcd->gsd_adr,gsd,4096);strcpy(rcd->gsdname,gsd_file);
	rcd->fmt=2;rcd->gsd_valid=1;
}

void	gsd_var_read()
{
	memcpy(gsd,rcd->gsd_adr,4096);strcpy(gsd_file,rcd->gsdname);
}

/***************************/
int	rcp_buf_put(char *ptr,int po,int ln)
{
	char	*p;
	if((rcd->bufcap)<(po+ln)){return(po+ln);}
	p=rcd->data_adr;p+=po;
	if(ln>1414){memcpy_l(p,ptr,ln);}else{memcpy(p,ptr,ln);}
	po=po+ln;return(po);
}

int	rcp_buf_get(char *ptr,int po,int ln)
{
	char	*p;
	if((rcd->bufcap)<(po+ln)){return(po+ln);}
	p=rcd->data_adr;p+=po;
	if(ln>1414){memcpy_l(ptr,p,ln);}else{memcpy(ptr,p,ln);}
	po=po+ln;return(po);
}

/* ================================================== data */
void	dclr()
{
	int	i,j;
	unsigned char	a;

	for(i=0;i<36;i++){
		same_cluc(i,0);
		for(j=0;j<tr_len[i];j=j+4){
			a=trk[i][j];
			if(a>127){
				switch(a){
					case  0xe7:if(trk[i][j+2]==0){trk[i][j+2]=1;}break;

					case  0xe6:
					case  0xea:
					case  0xec:trk[i][j+3]=0;break;

					case  0xf6:
					case  0xf7:trk[i][j+1]=0;break;

					case  0xf8:trk[i][j+2]=0;trk[i][j+3]=0;break;

					case  0xf9:
					case  0xfd:trk[i][j+1]=1;trk[i][j+2]=0;trk[i][j+3]=0;break;

					case  0xfe:trk[i][j+1]=0xfe;trk[i][j+2]=0xfe;trk[i][j+3]=0xfe;
							tr_len[i]=j+4;break;
				}
			}
		}
	}
}

/***************************/
void	same_cluc(int tr,int ofset)
{
	int	j=0,max;
	unsigned char	a;
	int	add[2048];

	max=add_set(tr,add);
	while(j<tr_len[tr]-4 ){
		a=trk[tr][j];
		if(a==0xfc){
			int no,ad=j;
resame:
			no=trk[tr][ad+1]+(trk[tr][ad+2]&3)*256;
			if(no<max){
				ad=add[no];
				if(j!=ad && trk[tr][ad]==0xfc ){
					goto resame;
				}

				trk[tr][j+1]=no&0xff;
				trk[tr][j+2]=(trk[tr][j+2]&0xfc)+((no>>8)&3);

			}else{ad=0;}

			ad+=44+ofset;trk[tr][j+2]=(trk[tr][j+2]&3)+(ad&0xfc);trk[tr][j+3]=ad>>8;
		}
		j=j+4;
	}
}

/***************************/
int	gomi_cut(unsigned char *ptr,int size)
{
	int	ad=0,po=0;
	unsigned char	da,db,dc,dd;

	while(ad<size){
		da=ptr[ad++];db=ptr[ad++];dc=ptr[ad++];dd=ptr[ad++];

		if(da==0x98 || da==0xf6){
			ptr[po++]=da;ptr[po++]=db;
			ptr[po++]=dc;ptr[po++]=dd;

			while(ptr[ad]==0xf7 && ad<size){
				ptr[po++]=ptr[ad++];ptr[po++]=ptr[ad++];
				ptr[po++]=ptr[ad++];ptr[po++]=ptr[ad++];
			}

			while(ptr[ad]==0xfd && ptr[ad+4]==0xf7 && ad<size){
				ptr[po++]=ptr[ad+4];ptr[po++]=ptr[ad+5];
				ptr[po++]=ptr[ad+6];ptr[po++]=ptr[ad+7];

				ptr[ad+4]=0xfd;ad+=4;
			}

		}else{
			if(da!=0xf7){
				ptr[po++]=da;ptr[po++]=db;
				ptr[po++]=dc;ptr[po++]=dd;
			}
		}
	}
	return po;
}

/***************************/
void	dinit()
{
	unsigned char	*ptr=trk[0];

	int	i,j;

	for(i=0;i<24;i++){tag[i][0]=-1;}

	track=0;btrack=0;
	buff_free=buff_size;

	for(i=0;i<36;i++){
		trk[i]=ptr;ptr+=4;tr_alc[i]=4;buff_free-=4;

		trk[i][0]=0xfe;tr_len[i]=4;tr_step[i]=0;

		for(j=0;j<4;j++){tr_pos[i][0][j]=0;tr_pos[i][1][j]=0;}
		trmod[i]=0;trno[i]=i+1;trrhy[i]=0;
		mch[i]=i+1;if(i>15){mch[i]=0;}

		trkey[i]=0;trst[i]=0;trkmemo[i][0]=0;
	}

	trrhy[9]=128;trkey[9]=128;

	for(i=0;i<12;i++){memo[i][0]=0;}
	mtitle[0]=0;cm6_file[0]=0;gsd_file[0]=0;
	mtempo=120;tbase=48;beat1=4;beat2=4;bkey=0;pbias=0;

	cminit();gsinit();rhinit();
	user_exc_init();asin_init();stgt_init();
}

/***************************/
