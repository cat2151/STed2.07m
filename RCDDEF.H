/*--------------------------------------------------------------*/
/* rcddef.h							*/
/*--------------------------------------------------------------*/
/* RCD ドライバ・アクセス用構造体定義   RCD v3.01 以降用	*/
/*--------------------------------------------------------------*/
/*								*/
/* RCD を利用するプログラムは、					*/
/* 								*/
/* 本ファイルをインクルードして下さい。				*/
/* 								*/
/* extern struct RCD_HEAD *rcd;   <--- ポインタの外部定義をし、	*/
/* 								*/
/* rcdcheck をコールして構造体のアドレスを求めた後に、		*/
/* 								*/
/* rcd->fmt などのようにアクセスする。				*/
/* 								*/
/* ルーチンコールは、(*rcd->begin)() のようにする。		*/
/*								*/
/*--------------------------------------------------------------*/

#define	TRK_NUM	36			/* トラック数		*/
#define	CHL_NUM	34 			/* チャネル数		*/
					/*  32/33 は未使用	*/

struct RCD_HEAD {
	char	title[4];		/* "RCD "		*/
	char	version[4];		/* "N.NN"		*/
	int	staymark;

	char	data_valid;		/* RCP/MCP 有効時 1	*/
	char	tone_valid;		/* CM6/MTD 有効時 1	*/
	char	word_valid;		/* WRD 有効時 1		*/
	char	fmt;			/* MCP:0 RCP:1 R36:2	*/

	char	*data_adr;		/* MCP/RCP address	*/
	char	*tone_adr;		/* MTD/CM6 address	*/
	char	*word_adr;		/* WRD address		*/
	char	*gsd_adr;		/* GSD address		*/ /*(v2.92)*/

	void	(*init)( void );	/* パラメータ初期化	*/ /* void (v2.70)*/
	void	(*setup)( void );	/* 音色データ書き込み	*/ /* void (v2.70)*/
	void	(*begin)( void );	/* 演奏開始		*/ /* void (v2.70)*/
	void	(*end)( void );		/* 演奏終了		*/ /* void (v2.70)*/

	void	(*md_put)( char );	/* MIDI 1 byte 出力	*/ /* void (v2.70)*/
	int	(*md_get)( void );	/* MIDI 1 byte 入力　(-1)入力なし*/ /*(v2.70)*/
	void	(*md_put2)( char );	/* MIDI 1 byte 出力(dual)*/ /* void (v2.92)*/
	void	(*mix_out)( char * );	/* MIX OUT(buff adrs)end=$ff*/ /*(v3.01)*/

	int	act;			/* 1:演奏中		*/
	int	sts;			/* 0:RUN 1:STOP 2:SEARCH 3:CUE	*/ /*(v2.70)*/
	int	tar_trk;		/* TARGET TRACK		*/
	int	tar_bar;		/* TARGET BAR		*/

	char	dummy1[ 18 ];

	int	tempo;			/* 現在のテンポ		*/
	int	basetempo;		/* 元のテンポ		*/

	int	totalcount;		/* 音符ファイル長	*/
	char	filename[30];		/* 音符ファイル名	*/
	char	tonename[30];		/* 音色ファイル名	*/

	char	dummy2[ 72 ];

	int	bufcap;			/* バッファ容量		*/  /*(v2.70)*/

	int	MIDI_avl;		/* MIDI 出力有効(trk.no)*/  /*(v2.80)*/
	unsigned char MIDI_req[16];	/* MIDI 出力要求バッファ*/  /*(v2.80)*/
		/* (80) (ch) ...	:ﾁｬﾈﾙﾁｪﾝｼﾞ	*/
		/* (90) (ke) (ve)	:ﾉｰﾄｵﾝ		*/  /*(v2.92)*/
		/* (b0) (xx) (xx)	:ｺﾝﾄﾛｰﾙﾁｪﾝｼﾞ	*/
		/* (c0) (xx) ...	:ﾌﾟﾛｸﾞﾗﾑﾁｪﾝｼﾞ	*/
		/* (e0) (xx) (xx)	:ﾋﾟｯﾁﾍﾞﾝﾀﾞｰ	*/
		/* (f0) (hh) (mm) (ll) (xx) :ﾒﾓﾘﾗｲﾄ	*/
		/* (f1) (hh) (mm) (ll) (xx) (id):ﾒﾓﾘﾗｲﾄ	*/  /*(v2.92)*/

	char	LA_VOL;			/* LA part master vol   */ /*(v2.80)*/
	char	LA_RVB_Mode;		/* LA part reverb Mode  */ /*(v2.80)*/
	char	LA_RVB_Time;		/* LA part reverb Time  */ /*(v2.80)*/
	char	LA_RVB_Level;		/* LA part reverb Level */ /*(v2.80)*/

	char	PCM_VOL;		/* PCM part master vol  */ /*(v2.80)*/
	char	PCM_RVB_Mode;		/* PCM part reverb Mode */ /*(v2.80)*/
	char	PCM_RVB_Time;		/* PCM part reverb Time */ /*(v2.80)*/
	char	PCM_RVB_Level;		/* PCM part reverb Level*/ /*(v2.80)*/

	int	filter_mode;		/* 0:無効 1:PRGのみ 2:LA/PCMあり */ /*(v2.90)*/
	char	*filter_data;		/* Filter Dataアドレス	*/ /*(v2.90)*/

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

	char	dummyA[19];		/* 拡張用リザーブ	*/

	char	mt32_mode;		/* mt-32 mode		*/
	int	exc_wait;		/* exclusive send wait	*/
	char	tim_all;		/* timbre trans mode	*/

	char	gsd_valid;		/* GSD 有効時 1		*/  /*(v2.92)*/
	char	gsdname[30];		/* GSDファイル名	*/

	int	wordcap;		/* word バッファ容量	*/  /*(v2.92)*/

	char	dummyC[10];		/* 拡張用リザーブ	*/

	char	GS_VOL;			/* GS part master vol   */ /*(v2.93)*/
	char	GS_PAN;			/* GS part master pan   */ /*(v2.93)*/

	char	GS_RVB_Macro;		/* リバーブマクロ	*/ /*(v2.93)*/
	char	GS_RVB_Char;		/* リバーブ・キャラクター・コントロール*/
	char	GS_RVB_Prelpf;		/* リバーブ・ＰＲＥ・ＬＰＦ・コントロール*/
	char	GS_RVB_Level;		/* リバーブ・レベル・コントロール*/
	char	GS_RVB_Time;		/* リバーブ・タイム・コントロール*/
	char	GS_RVB_Delay;		/* リバーブ・DELAY・FEEDBACK・コントロール*/
	char	GS_RVB_Send;		/* REVERB SEND LEVEL TO CHOURUS */
	char	GS_RVB_PreDelay;	/* REVERB PREDLY T. */

	char	GS_CHO_Macro;		/* コーラスマクロ	*/
	char	GS_CHO_Prelpf;		/* コーラス・ＰＲＥ・ＬＰＦ・コントロール*/
	char	GS_CHO_Level;		/* コーラス・レベル・コントロール*/
	char	GS_CHO_Feed;		/* コーラス・フィード・バック*/
	char	GS_CHO_Delay;		/* コーラス・Ｄｅｌａｙ・コントロール*/
	char	GS_CHO_Rate;		/* コーラス・Ｒａｔｅ・コントロール*/
	char	GS_CHO_Depth;		/* コーラス・Ｄｅｐｔｈ・コントロール*/
	char	GS_CHO_Send;		/* CHORUS SEND LEVEL TO REVERB */
	char	GS_CHO_Send_Dly;

	char	GS_DLY_Macro;		/* ディレイマクロ	*/ /*(v3.01)*/
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

	char	dummyD[32-16];		/* 拡張用リザーブ	*/

	char	active[ TRK_NUM ];	/* トラック有効		*/
	char	trk_mask[ TRK_NUM ];	/* TRACK MASK		*/
	char	midich[ TRK_NUM ];	/* MIDI CH		*/

	int	noteptr;		/* ﾉｰﾄﾗﾝﾆﾝｸﾞﾎﾟｲﾝﾀ	*/
	unsigned char	*note_adr;	/* ﾉｰﾄﾗﾝﾆﾝｸﾞﾊﾞｯﾌｧｱﾄﾞﾚｽ	*/
	char	*top[ TRK_NUM ];	/* トラックデータ先頭	*/
	unsigned char *ptr[ TRK_NUM ];	/* ｶﾚﾝﾄﾄﾗｯｸﾎﾟｲﾝﾀ	*/

	char	flg_vel[ TRK_NUM ];	/* VELOCITY ON ﾌﾗｸﾞ	*/
	char	flg_off[ TRK_NUM ];	/* VELOCITY OFF ﾌﾗｸﾞ	*/
	char	flg_act;		/* ACTIVE OFF ﾌﾗｸﾞ	*/
	char	flg_bar;		/* BAR 変更 ﾌﾗｸﾞ	*/
	char	flg_step;		/* STEP 変更 ﾌﾗｸﾞ	*/
	char	flg_pbend;		/* PITCH BEND 変更 ﾌﾗｸﾞ	*/
	char	flg_vol;		/* VOLUME 変更 ﾌﾗｸﾞ	*/
	char	flg_prg;		/* PROGRAM 変更　ﾌﾗｸﾞ	*/
	char	flg_panpot;		/* PANPOT 変更 ﾌﾗｸﾞ	*/
	char	flg_midich;		/* MIDI CH 変更 ﾌﾗｸﾞ	*/

	char	flg_song;		/* SONG ﾃﾞｰﾀ ﾌﾗｸﾞ	*/
	char	flg_system;		/* SYSTEM ｴﾘｱ変更ﾌﾗｸﾞ	*/ /*(v2.80)*/

	char	flg_expres;		/* EXPRESSION 変更 ﾌﾗｸﾞ	*/
	char	flg_modula;		/* MODULATION 変更 ﾌﾗｸﾞ	*/
	char	flg_bank;		/* BANK 変更 ﾌﾗｸﾞ	*/
	char	flg_replay;		/* REPLAY ﾌﾗｸﾞ		*/

	char	flg_gssys;		/* GS SYSTEM ｴﾘｱ変更ﾌﾗｸﾞ*/ /*(v2.93)*/
	char	flg_gsrev;		/* GSREV 変更 ﾌﾗｸﾞ	*/ /*(v2.93)*/
	char	flg_gscho;		/* GSCHO 変更 ﾌﾗｸﾞ	*/ /*(v2.93)*/
	char	flg_gsinfo;		/* GS info 変更 ﾌﾗｸﾞ	*/ /*(v2.93l)*/
	char	flg_gsinst;		/* GS inst 変更 ﾌﾗｸﾞ	*/ /*(v2.93l)*/
	char	flg_gspanel;		/* GS panel 変更 ﾌﾗｸﾞ	*/ /*(v2.93l)*/

	char	flg_hold1;		/* HOLD1 変更 ﾌﾗｸﾞ	*/
	char	flg_gsdly;		/* GSDLY 変更 ﾌﾗｸﾞ	*/ /*(v3.00q)*/
	char	flg_bankl;		/* BANK L変更 ﾌﾗｸﾞ	*/ /*(v3.01n)*/

	char	dummyE[9];		/* 拡張用リザーブ	*/

	int	panel_tempo;		/* パネル上のテンポ値	*/
	int	bar[ TRK_NUM ];		/* 小節番号		*/
	int	step[ TRK_NUM ];	/* ステップ番号		*/
	char	vel[ TRK_NUM ];		/* ベロシティ値		*/

	int	stepcount;		/* 演奏開始時からのSTEP COUNT	*/ /*(v2.92)*/
	short	loopcount;		/* 255/256回リピートのCOUNT	*/ /*(v3.00f)*/

	char	dummyF[12-2];		/* 拡張用リザーブ	*/

	char	song[20];		/* SONGデータ(コメント)	*/
	char	dummyG[60];		/* 拡張用リザーブ	*/

	char	gs_info[18];		/* gs patch name	*/ /*v2.93l*/
	char	gs_inst[34];		/* gs comment		*/ /*v2.93l*/
	char	gs_panel[64];		/* gs panel		*/ /*v2.93l*/

	char	ch_port[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のi.f.種類 1:MIDI 2:RS-232C/POLYPHON */
	char	ch_part[ CHL_NUM ];	/* ﾁｬﾈﾙ毎の音源種類 0:LA 1:PCM 2:他(RHYTHM) */
	int	ch_pbend[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のPITCH BEND値	*/
	char	ch_vol[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のVOLUME値	*/
	char	ch_panpot[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のPANPOT値	*/
	char	flg_ch_prg[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のPROG.CHGフラグ*/ /*v2.93j*/
	unsigned char ch_prg[ CHL_NUM ];/* ﾁｬﾈﾙ毎のPROGRAM番号	*/
	char	ch_reverb[ CHL_NUM ];	/* リバーブ OFF/ON	*/ /*(v2.80)*/
	char	ch_expr[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のEXPRSSION値	*/ /*v2.92*/
	char	ch_modu[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のMODULATION値	*/ /*v2.92*/
	char	ch_bank[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のBANK値	*/ /*v2.92*/
	char	ch_gsrev[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のGS REVERB値	*/ /*v2.93*/
	char	ch_gscho[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のGS CHORUS値	*/ /*v2.93*/
	char	ch_hold1[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のHOLD1値	*/ /*v2.93*/
	char	ch_gsdly[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のGS DELAY値	*/ /*v3.00q*/
	char	ch_bankl[ CHL_NUM ];	/* ﾁｬﾈﾙ毎のGS BANK L.値	*/ /*v3.01n*/

	char	dummyH[34*1];		/* 拡張用リザーブ	*/
};

