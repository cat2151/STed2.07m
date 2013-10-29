/*--------------------------------------------------------------*/
/* rcddef.h							*/
/*--------------------------------------------------------------*/
/* RCD �h���C�o�E�A�N�Z�X�p�\���̒�`   RCD v3.01 �ȍ~�p	*/
/*--------------------------------------------------------------*/
/*								*/
/* RCD �𗘗p����v���O�����́A					*/
/* 								*/
/* �{�t�@�C�����C���N���[�h���ĉ������B				*/
/* 								*/
/* extern struct RCD_HEAD *rcd;   <--- �|�C���^�̊O����`�����A	*/
/* 								*/
/* rcdcheck ���R�[�����č\���̂̃A�h���X�����߂���ɁA		*/
/* 								*/
/* rcd->fmt �Ȃǂ̂悤�ɃA�N�Z�X����B				*/
/* 								*/
/* ���[�`���R�[���́A(*rcd->begin)() �̂悤�ɂ���B		*/
/*								*/
/*--------------------------------------------------------------*/

#define	TRK_NUM	36			/* �g���b�N��		*/
#define	CHL_NUM	34 			/* �`���l����		*/
					/*  32/33 �͖��g�p	*/

struct RCD_HEAD {
	char	title[4];		/* "RCD "		*/
	char	version[4];		/* "N.NN"		*/
	int	staymark;

	char	data_valid;		/* RCP/MCP �L���� 1	*/
	char	tone_valid;		/* CM6/MTD �L���� 1	*/
	char	word_valid;		/* WRD �L���� 1		*/
	char	fmt;			/* MCP:0 RCP:1 R36:2	*/

	char	*data_adr;		/* MCP/RCP address	*/
	char	*tone_adr;		/* MTD/CM6 address	*/
	char	*word_adr;		/* WRD address		*/
	char	*gsd_adr;		/* GSD address		*/ /*(v2.92)*/

	void	(*init)( void );	/* �p�����[�^������	*/ /* void (v2.70)*/
	void	(*setup)( void );	/* ���F�f�[�^��������	*/ /* void (v2.70)*/
	void	(*begin)( void );	/* ���t�J�n		*/ /* void (v2.70)*/
	void	(*end)( void );		/* ���t�I��		*/ /* void (v2.70)*/

	void	(*md_put)( char );	/* MIDI 1 byte �o��	*/ /* void (v2.70)*/
	int	(*md_get)( void );	/* MIDI 1 byte ���́@(-1)���͂Ȃ�*/ /*(v2.70)*/
	void	(*md_put2)( char );	/* MIDI 1 byte �o��(dual)*/ /* void (v2.92)*/
	void	(*mix_out)( char * );	/* MIX OUT(buff adrs)end=$ff*/ /*(v3.01)*/

	int	act;			/* 1:���t��		*/
	int	sts;			/* 0:RUN 1:STOP 2:SEARCH 3:CUE	*/ /*(v2.70)*/
	int	tar_trk;		/* TARGET TRACK		*/
	int	tar_bar;		/* TARGET BAR		*/

	char	dummy1[ 18 ];

	int	tempo;			/* ���݂̃e���|		*/
	int	basetempo;		/* ���̃e���|		*/

	int	totalcount;		/* �����t�@�C����	*/
	char	filename[30];		/* �����t�@�C����	*/
	char	tonename[30];		/* ���F�t�@�C����	*/

	char	dummy2[ 72 ];

	int	bufcap;			/* �o�b�t�@�e��		*/  /*(v2.70)*/

	int	MIDI_avl;		/* MIDI �o�͗L��(trk.no)*/  /*(v2.80)*/
	unsigned char MIDI_req[16];	/* MIDI �o�͗v���o�b�t�@*/  /*(v2.80)*/
		/* (80) (ch) ...	:������ݼ�	*/
		/* (90) (ke) (ve)	:ɰĵ�		*/  /*(v2.92)*/
		/* (b0) (xx) (xx)	:���۰���ݼ�	*/
		/* (c0) (xx) ...	:��۸�����ݼ�	*/
		/* (e0) (xx) (xx)	:�߯�����ް	*/
		/* (f0) (hh) (mm) (ll) (xx) :���ײ�	*/
		/* (f1) (hh) (mm) (ll) (xx) (id):���ײ�	*/  /*(v2.92)*/

	char	LA_VOL;			/* LA part master vol   */ /*(v2.80)*/
	char	LA_RVB_Mode;		/* LA part reverb Mode  */ /*(v2.80)*/
	char	LA_RVB_Time;		/* LA part reverb Time  */ /*(v2.80)*/
	char	LA_RVB_Level;		/* LA part reverb Level */ /*(v2.80)*/

	char	PCM_VOL;		/* PCM part master vol  */ /*(v2.80)*/
	char	PCM_RVB_Mode;		/* PCM part reverb Mode */ /*(v2.80)*/
	char	PCM_RVB_Time;		/* PCM part reverb Time */ /*(v2.80)*/
	char	PCM_RVB_Level;		/* PCM part reverb Level*/ /*(v2.80)*/

	int	filter_mode;		/* 0:���� 1:PRG�̂� 2:LA/PCM���� */ /*(v2.90)*/
	char	*filter_data;		/* Filter Data�A�h���X	*/ /*(v2.90)*/

	int	play_mode;		/* 0:normal 1:slow 2:fast 3:slow2 4:fast2 */ /*(v2.92)*/
	int	mute_mode;		/* 0:off 1:cm64 2:sc55	*/ /*(v2.92)*/
	int	init_mode;		/* 0:off 1:cm64 2:sc55 3:cm+sc */ /*(v2.92)*/
	char	scan_mode;		/* key scan flag 0:off 1:on */
	char	rsmd_mode;		/* midi port flag 0:midi 1:midi+rs/sb 2:rs/sb */
	short	fade_time;		/* fade out speed	*/
	char	fade_count;		/* fade out start flag 128:start 0:end */
	char	moduletype;		/* panel display 0:cm64 1:sc55 2:cm+sc */
	char	fade_mode;		/* fade out mode 0:exclusive a:volume 2:expres*/
	char	timer_mode;		/* RS-MIDI timer mode 0:OPM Timer-A 1:OPM Timer-B */
	char	midi_clock;		/* MIDI clock out 0:disable 1:enable */
	char	put_mode;		/* md_put port mode 0:normal 1:midi 2:rs */
	char	rcd_type;		/* 0:midi+rs-232c 1:midi+polyphon 3:polyphon only */
	char	sc55_fix;		/* 0:off 1:sc55 capi.down emulate */ /*(v3.01)*/

	char	dummyA[19];		/* �g���p���U�[�u	*/

	char	mt32_mode;		/* mt-32 mode		*/
	int	exc_wait;		/* exclusive send wait	*/
	char	tim_all;		/* timbre trans mode	*/

	char	gsd_valid;		/* GSD �L���� 1		*/  /*(v2.92)*/
	char	gsdname[30];		/* GSD�t�@�C����	*/

	int	wordcap;		/* word �o�b�t�@�e��	*/  /*(v2.92)*/

	char	dummyC[10];		/* �g���p���U�[�u	*/

	char	GS_VOL;			/* GS part master vol   */ /*(v2.93)*/
	char	GS_PAN;			/* GS part master pan   */ /*(v2.93)*/

	char	GS_RVB_Macro;		/* ���o�[�u�}�N��	*/ /*(v2.93)*/
	char	GS_RVB_Char;		/* ���o�[�u�E�L�����N�^�[�E�R���g���[��*/
	char	GS_RVB_Prelpf;		/* ���o�[�u�E�o�q�d�E�k�o�e�E�R���g���[��*/
	char	GS_RVB_Level;		/* ���o�[�u�E���x���E�R���g���[��*/
	char	GS_RVB_Time;		/* ���o�[�u�E�^�C���E�R���g���[��*/
	char	GS_RVB_Delay;		/* ���o�[�u�EDELAY�EFEEDBACK�E�R���g���[��*/
	char	GS_RVB_Send;		/* REVERB SEND LEVEL TO CHOURUS */
	char	GS_RVB_PreDelay;	/* REVERB PREDLY T. */

	char	GS_CHO_Macro;		/* �R�[���X�}�N��	*/
	char	GS_CHO_Prelpf;		/* �R�[���X�E�o�q�d�E�k�o�e�E�R���g���[��*/
	char	GS_CHO_Level;		/* �R�[���X�E���x���E�R���g���[��*/
	char	GS_CHO_Feed;		/* �R�[���X�E�t�B�[�h�E�o�b�N*/
	char	GS_CHO_Delay;		/* �R�[���X�E�c���������E�R���g���[��*/
	char	GS_CHO_Rate;		/* �R�[���X�E�q�������E�R���g���[��*/
	char	GS_CHO_Depth;		/* �R�[���X�E�c���������E�R���g���[��*/
	char	GS_CHO_Send;		/* CHORUS SEND LEVEL TO REVERB */
	char	GS_CHO_Send_Dly;

	char	GS_DLY_Macro;		/* �f�B���C�}�N��	*/ /*(v3.01)*/
	char	GS_DLY_Prelpf;
	char	GS_DLY_Time_C;
	char	GS_DLY_Time_L;
	char	GS_DLY_Time_R;
	char	GS_DLY_Lev_C;
	char	GS_DLY_Lev_L;
	char	GS_DLY_Lev_R;
	char	GS_DLY_Level;
	char	GS_DLY_Feed;
	char	GS_DLY_Send_Rev;

	char	GS_EQ_Low_Freq;
	char	GS_EQ_Low_Gain;
	char	GS_EQ_High_Freq;
	char	GS_EQ_High_Gain;

	char	dummyD[32-16];		/* �g���p���U�[�u	*/

	char	active[ TRK_NUM ];	/* �g���b�N�L��		*/
	char	trk_mask[ TRK_NUM ];	/* TRACK MASK		*/
	char	midich[ TRK_NUM ];	/* MIDI CH		*/

	int	noteptr;		/* ɰ����ݸ��߲��	*/
	unsigned char	*note_adr;	/* ɰ����ݸ��ޯ̧���ڽ	*/
	char	*top[ TRK_NUM ];	/* �g���b�N�f�[�^�擪	*/
	unsigned char *ptr[ TRK_NUM ];	/* �����ׯ��߲��	*/

	char	flg_vel[ TRK_NUM ];	/* VELOCITY ON �׸�	*/
	char	flg_off[ TRK_NUM ];	/* VELOCITY OFF �׸�	*/
	char	flg_act;		/* ACTIVE OFF �׸�	*/
	char	flg_bar;		/* BAR �ύX �׸�	*/
	char	flg_step;		/* STEP �ύX �׸�	*/
	char	flg_pbend;		/* PITCH BEND �ύX �׸�	*/
	char	flg_vol;		/* VOLUME �ύX �׸�	*/
	char	flg_prg;		/* PROGRAM �ύX�@�׸�	*/
	char	flg_panpot;		/* PANPOT �ύX �׸�	*/
	char	flg_midich;		/* MIDI CH �ύX �׸�	*/

	char	flg_song;		/* SONG �ް� �׸�	*/
	char	flg_system;		/* SYSTEM �ر�ύX�׸�	*/ /*(v2.80)*/

	char	flg_expres;		/* EXPRESSION �ύX �׸�	*/
	char	flg_modula;		/* MODULATION �ύX �׸�	*/
	char	flg_bank;		/* BANK �ύX �׸�	*/
	char	flg_replay;		/* REPLAY �׸�		*/

	char	flg_gssys;		/* GS SYSTEM �ر�ύX�׸�*/ /*(v2.93)*/
	char	flg_gsrev;		/* GSREV �ύX �׸�	*/ /*(v2.93)*/
	char	flg_gscho;		/* GSCHO �ύX �׸�	*/ /*(v2.93)*/
	char	flg_gsinfo;		/* GS info �ύX �׸�	*/ /*(v2.93l)*/
	char	flg_gsinst;		/* GS inst �ύX �׸�	*/ /*(v2.93l)*/
	char	flg_gspanel;		/* GS panel �ύX �׸�	*/ /*(v2.93l)*/

	char	flg_hold1;		/* HOLD1 �ύX �׸�	*/
	char	flg_gsdly;		/* GSDLY �ύX �׸�	*/ /*(v3.00q)*/
	char	flg_bankl;		/* BANK L�ύX �׸�	*/ /*(v3.01n)*/

	char	dummyE[9];		/* �g���p���U�[�u	*/

	int	panel_tempo;		/* �p�l����̃e���|�l	*/
	int	bar[ TRK_NUM ];		/* ���ߔԍ�		*/
	int	step[ TRK_NUM ];	/* �X�e�b�v�ԍ�		*/
	char	vel[ TRK_NUM ];		/* �x���V�e�B�l		*/

	int	stepcount;		/* ���t�J�n�������STEP COUNT	*/ /*(v2.92)*/
	short	loopcount;		/* 255/256�񃊃s�[�g��COUNT	*/ /*(v3.00f)*/

	char	dummyF[12-2];		/* �g���p���U�[�u	*/

	char	song[20];		/* SONG�f�[�^(�R�����g)	*/
	char	dummyG[60];		/* �g���p���U�[�u	*/

	char	gs_info[18];		/* gs patch name	*/ /*v2.93l*/
	char	gs_inst[34];		/* gs comment		*/ /*v2.93l*/
	char	gs_panel[64];		/* gs panel		*/ /*v2.93l*/

	char	ch_port[ CHL_NUM ];	/* ���ٖ���i.f.��� 1:MIDI 2:RS-232C/POLYPHON */
	char	ch_part[ CHL_NUM ];	/* ���ٖ��̉������ 0:LA 1:PCM 2:��(RHYTHM) */
	int	ch_pbend[ CHL_NUM ];	/* ���ٖ���PITCH BEND�l	*/
	char	ch_vol[ CHL_NUM ];	/* ���ٖ���VOLUME�l	*/
	char	ch_panpot[ CHL_NUM ];	/* ���ٖ���PANPOT�l	*/
	char	flg_ch_prg[ CHL_NUM ];	/* ���ٖ���PROG.CHG�t���O*/ /*v2.93j*/
	unsigned char ch_prg[ CHL_NUM ];/* ���ٖ���PROGRAM�ԍ�	*/
	char	ch_reverb[ CHL_NUM ];	/* ���o�[�u OFF/ON	*/ /*(v2.80)*/
	char	ch_expr[ CHL_NUM ];	/* ���ٖ���EXPRSSION�l	*/ /*v2.92*/
	char	ch_modu[ CHL_NUM ];	/* ���ٖ���MODULATION�l	*/ /*v2.92*/
	char	ch_bank[ CHL_NUM ];	/* ���ٖ���BANK�l	*/ /*v2.92*/
	char	ch_gsrev[ CHL_NUM ];	/* ���ٖ���GS REVERB�l	*/ /*v2.93*/
	char	ch_gscho[ CHL_NUM ];	/* ���ٖ���GS CHORUS�l	*/ /*v2.93*/
	char	ch_hold1[ CHL_NUM ];	/* ���ٖ���HOLD1�l	*/ /*v2.93*/
	char	ch_gsdly[ CHL_NUM ];	/* ���ٖ���GS DELAY�l	*/ /*v3.00q*/
	char	ch_bankl[ CHL_NUM ];	/* ���ٖ���GS BANK L.�l	*/ /*v3.01n*/

	char	dummyH[34*1];		/* �g���p���U�[�u	*/
};

