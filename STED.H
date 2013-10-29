/*
 * MIDI Music Composer STed v2.07j : sted.h (header) 1997-07-20 by TURBO
 */

#include "rcddef.h"

extern void rcd_check( void );
extern struct RCD_HEAD	*rcd;
extern char rcd_version[];

/* etc */
char	hlp_path[128];				/* help file path & name	*/
char	fon_path[128];				/* font file path & name	*/

char	def_path[128];				/* .def path name		*/
char	rcp_path[128];				/* .rcp path name		*/
char	prt_path[128];				/* .prt path name		*/
char	trk_path[128];				/* .trk path name		*/

char	def_file[128];				/* .def file name		*/
char	rcp_file[128];				/* .rcp file name		*/
char	prt_file[128];				/* .prt file name		*/
char	trk_file[128];				/* .trk file name		*/

char	repl[128];				/* replace string		*/
char	delt[128];				/* delete string		*/
char	srch[128];				/* find string			*/

char	repl_t[26*6][40],repl_d[26*6][50];

int	tr_step[36];				/* track total step temp	*/
int	tr_alc[36];				/* track aloc size		*/
int	tr_len[36];				/* track used size		*/
int	tr_pos[36][2][4];			/* track cursor position	*/
int	tag[26+2][4];				/* tag jamp list buffer		*/

int	es,ecode,scyp;				/* input subroutine exit code	*/

int	btrack;					/* track set top track		*/
int	track,track1,track2;			/* edit track no.		*/
int	edit_scr;				/* 0=l_track 1=r_track 2=rhythm	*/
int	cmdflag,cnfflag,mdlflag;		/* STed2 system flag		*/
int	poft;

int	TRACK_SIZE,work_size;			/* buffer size			*/
int	buff_size,buff_free,buff_max;
int	cpcy,cpadd,cplen,cpleng,rcpf,rcplen;	/* copy flag			*/

void	*ErrorTrap_Old;				/* err trap vecter/flag		*/
int	ErrFlag;

/* cnf */
char	comment[64];				/* comment			*/

int	tm_lag;					/* graphic rewrite time lag	*/
char	inpmode;				/* editor input mode		*/
char	grpmode;				/* editor graphic mode		*/
char	thrumode;				/* edit midi in thru mode	*/
int	rec_getch,rec_putmd;			/* recoding ch./put mode	*/

int	vis_reso;				/**/

unsigned char	rfilt[32][4];			/* record filter */
unsigned char	pfilt[32][4];			/* play   filter */

char	mplay[16][16];
int	palet_dat[16];

unsigned char	keyst_table[2][128];		/* key# -> st/gt convert table	*/
unsigned char	stgt_tbl[60];			/* f.key -> st/gt table		*/

char	rhy_vel[16];

char	chcom_s[26*2][2][50];			/* child command comment	*/
char	chcom_c[26*2][2][16];			/* child command parameter	*/

/* def */
char	module[64];				/* module name			*/

int	lsp_wait;				/* last para. transfer wait	*/
int	bend_range;				/* piche bend range		*/

char	tim_asin[33];				/* tone list channle assign	*/
char	tim_head[400][24];			/* tone list title		*/
char	tim_sym[400][8];			/* tone list symbol		*/
char	tim_name[80*128][15];			/* tone name buffer		*/
short	tim_top[400];				/* tone name buffer address	*/

char	card_name[21][64];			/* pcm card name list		*/
char	card_no[2];				/* used pcm card no.		*/

short	gs_bank[18*128];			/* gs bank part no.(8+1 group)	*/
short	gs_mode[18];				/* gs bank mode    (8+1 group)	*/

char	rhy_stest[8];				/* rhythm sound test ch. & para	*/
char	rec_met[10];				/* recording metoro tone	*/

unsigned char	init_exc_data[258];		/* init exclusive data		*/

/*** rcp format head parameter ***/
char	mtitle[65];				/* music title			*/
char	memo[12][29];				/* memo				*/
int	tbase,mtempo,beat1,beat2,bkey,pbias;	/* common parameter		*/
char	cm6_file[128],gsd_file[128];		/* controol file name		*/

char	rhyna[32][15];				/* rhythm assign name		*/
unsigned char	rhyno[32][2];			/* rhythm assign key & gate	*/

char	user_exc_memo[8][25];			/* user exclusive memo		*/
unsigned char	user_exc_data[8][24];		/* user exclusive data		*/

unsigned char	trno[36];			/* track no.			*/
unsigned char	trmod[36];			/* track play mode		*/
unsigned char	trrhy[36];			/* track rhythm sw.		*/
unsigned char	mch[36];			/* track midi ch.		*/
unsigned char	trkey[36];			/* track key shift		*/
unsigned char	trst[36];			/* track st shift		*/
char	trkmemo[36][37];			/* track comment		*/

/* buffer */
unsigned char	lcbuf[1024*4+4];		/* delete line buffer		*/
unsigned char	rlcbuf[132];			/* delete rhythm line buffer	*/
unsigned char	cm6[22601],gsd[4096];		/* control file buffer		*/
unsigned char	hed[1414];			/* rcp header temporary		*/

unsigned char	*trk[36];			/* track buffer pointer		*/
unsigned char	*cpbuf;				/* track copy buffer		*/
unsigned char	*rcpbuf;			/* rhythm track copy buffer	*/
unsigned char	*dat;				/* temporary & recording buffer	*/
unsigned char	*dat2;				/* temporary			*/

