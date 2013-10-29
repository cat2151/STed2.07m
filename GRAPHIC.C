/*
 * STed: graph.c (grapihc subroutine)
 */

#include	"iocslib.h"
#include	"doslib.h"
#include	"stdlib.h"
#include	"string.h"
#include	"graph.h"
#include	"sted.h"

#define		TXRAM	0xe00000
#define		GVRAM	0xc00000
#define		PL1	(0x20000/2)
#define		PL2	(0x20000)

void	g_print();
void	tg_copy();
void	rev_area();
void	txerase();

/***************************/
/*
void	g_print(int x,int y,unsigned char *s,int c)
{
	int	sta;
	short	j;
	unsigned short	a;
	unsigned char	pat;

	unsigned short	*gptr= (unsigned short*)(GVRAM);
	unsigned short	*gptr2;
	unsigned char	*pp;

	struct	FNTBUF	*fntb,buf;
	fntb=&buf;

	sta=SUPER(0);

	gptr+=(y*1024+x);

	while(*s!=0){
		a=*s++;if( 127<a && (161>a || 224<a) ){a=a*256+*s++;}
		FNTGET(8,a,fntb);gptr2=gptr;pp=&fntb->buffer;j=16;
		if( fntb->xl==16 ){
			while(j){
				pat=*pp++;
				if(pat!=0){
					if((pat&128)){gptr2[0]=c;}if((pat& 64)){gptr2[1]=c;}
					if((pat& 32)){gptr2[2]=c;}if((pat& 16)){gptr2[3]=c;}
					if((pat&  8)){gptr2[4]=c;}if((pat&  4)){gptr2[5]=c;}
					if((pat&  2)){gptr2[6]=c;}if((pat&  1)){gptr2[7]=c;}
				}
				pat=*pp++;
				if(pat!=0){
					if((pat&128)){gptr2[ 8]=c;}if((pat& 64)){gptr2[ 9]=c;}
					if((pat& 32)){gptr2[10]=c;}if((pat& 16)){gptr2[11]=c;}
					if((pat&  8)){gptr2[12]=c;}if((pat&  4)){gptr2[13]=c;}
					if((pat&  2)){gptr2[14]=c;}if((pat&  1)){gptr2[15]=c;}
				}
				gptr2+=1024;j--;
			}
			gptr+=16;
		}else{
			while(j){
				pat=*pp++;
				if(pat!=0){
					if((pat&128)){gptr2[0]=c;}if((pat& 64)){gptr2[1]=c;}
					if((pat& 32)){gptr2[2]=c;}if((pat& 16)){gptr2[3]=c;}
					if((pat&  8)){gptr2[4]=c;}if((pat&  4)){gptr2[5]=c;}
					if((pat&  2)){gptr2[6]=c;}if((pat&  1)){gptr2[7]=c;}
				}
				gptr2+=1024;j--;
			}
			gptr+=8;
		}
	}
	SUPER(sta);
}
*/
/***************************/
/*
void	tcur(int a,int b,int c,int cs)
{
	struct	TREVPTR	*txrev,buf;

	if(edit_scr==1){a+=56;}

	txrev=&buf;txrev->x=a*8;txrev->y=b*16;txrev->x1=c*8;txrev->y1=16;

	if(cs!=2){txrev->vram_page=0;TXREV(txrev);}
	if(cs!=1){txrev->vram_page=1;TXREV(txrev);}
}
void	tcury(int a,int b,int c,int cs)
{
	struct	TREVPTR	*txrev,buf;

	if(edit_scr==1){a+=56;}

	txrev=&buf;txrev->x=a*8;txrev->y=b*16;txrev->x1=8;txrev->y1=c*16;

	if(cs!=2){txrev->vram_page=0;TXREV(txrev);}
	if(cs!=1){txrev->vram_page=1;TXREV(txrev);}
}
*/
void	tcur(int a,int b,int c,int cs)
{
	txcur(a,b,c,1,cs);
}
void	tcury(int a,int b,int c,int cs)
{
	txcur(a,b,1,c,cs);
}

/***************************/
void	txerase(int x,int y,int x1,int y1)
{
	struct	TXFILLPTR	*txfil,buf;
	unsigned short	sv;

	txfil=&buf;txfil->x=x;txfil->y=y;txfil->x1=x1;txfil->y1=y1;txfil->fill_patn=0;
	sv=B_WPEEK((unsigned short*)0xe8002a);B_WPOKE((unsigned short*)0xe8002a,0x133);
	txfil->vram_page=0;TXFILL(txfil);
	B_WPOKE((unsigned short*)0xe8002a,sv);
}

/***************************/
/*
void	txfill(int x,int y,int x1,int y1,int page,int pat)
{
	struct	TXFILLPTR	*tfill,buf;

	tfill=&buf;tfill->x;tfill->y;tfill->x1=x1;tfill->y1=y1;tfill->fill_patn=pat;
	tfill->vram_page=page;TXFILL(tfill);
}
*/
/***************************/
void	txxline(int x,int y,int x1,int page,int style)
{
	struct	XLINEPTR	*txline,buf;

	txline=&buf;txline->x=x;txline->y=y;txline->x1=x1;txline->line_style=style;
	txline->vram_page=page;TXXLINE(txline);
}

/***************************/
void	txyline(int x,int y,int y1,int page,int style)
{
	struct	YLINEPTR	*tyline,buf;

	tyline=&buf;tyline->x=x;tyline->y=y;tyline->y1=y1;tyline->line_style=style;
	tyline->vram_page=page;TXYLINE(tyline);
}

/***************************/
