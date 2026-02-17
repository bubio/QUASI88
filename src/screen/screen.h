#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED



typedef struct {
	unsigned	char	blue;					/* B面輝度 (0..7/255) */
	unsigned	char	red;					/* R面輝度 (0..7/255) */
	unsigned	char	green;					/* G面輝度 (0..7/255) */
	unsigned	char	padding;
} PC88_PALETTE_T;


/*
 * PC-88 Related
 */

extern PC88_PALETTE_T vram_bg_palette;			/* 背景パレット */
extern PC88_PALETTE_T vram_palette[8];			/* 各種パレット */

extern byte sys_ctrl;							/* OUT[30] SystemCtrl */
extern byte grph_ctrl;							/* OUT[31] GraphCtrl */
extern byte grph_pile;							/* OUT[53] 重ね合わせ */

#define SYS_CTRL_80				(0x01)			/* TEXT COLUMN80 / COLUMN40*/
#define SYS_CTRL_MONO			(0x02)			/* TEXT MONO     / COLOR   */

#define GRPH_CTRL_200			(0x01)			/* VRAM-MONO 200 / 400 line*/
#define GRPH_CTRL_64RAM			(0x02)			/* RAM   64K-RAM / ROM-RAM */
#define GRPH_CTRL_N				(0x04)			/* BASIC       N / N88     */
#define GRPH_CTRL_VDISP			(0x08)			/* VRAM  DISPLAY / UNDISP  */
#define GRPH_CTRL_COLOR			(0x10)			/* VRAM  COLOR   / MONO    */
#define GRPH_CTRL_25			(0x20)			/* TEXT  LINE25  / LINE20  */

#define GRPH_PILE_TEXT			(0x01)			/* 重ね合わせ 非表示 TEXT  */
#define GRPH_PILE_BLUE			(0x02)			/*                     B   */
#define GRPH_PILE_RED			(0x04)			/*                     R   */
#define GRPH_PILE_GREEN			(0x08)			/*                     G   */



/*
 * 描画処理用ワーク
 */

/* 描画差分管理 */

extern int  screen_dirty_tool_full;				/* ツールエリア全部更新 */
extern int  screen_dirty_tool_part;				/* ツールエリア差分更新 */
extern int  screen_dirty_stat_full;				/* ステータスエリア全部更新 */
extern int  screen_dirty_stat_part;				/* ステータスエリア差分更新 */
extern int  screen_dirty_menu;					/* ユーザエリア(MENU)差分更新*/
extern char screen_dirty_flag[ 0x4000 * 2 ];	/* ユーザエリア(EMU)差分更新 */
extern int  screen_dirty_all;					/* ユーザエリア全部更新 */
extern int  screen_dirty_palette;				/* 色情報 更新 */
extern int  screen_dirty_frame;					/* 全領域 更新 */

#define set_screen_dirty_tool_full()	screen_dirty_tool_full = TRUE
#define set_screen_dirty_tool_part()	screen_dirty_tool_part = TRUE
#define set_screen_dirty_stat_full()	screen_dirty_stat_full = TRUE
#define set_screen_dirty_stat_part()	screen_dirty_stat_part = TRUE
#define set_screen_dirty_menu()			screen_dirty_menu = TRUE
#define set_screen_dirty_flag(x)		screen_dirty_flag[x] = 1
#define set_screen_dirty_all()			screen_dirty_all = TRUE
#define set_screen_dirty_palette()		do {								\
											screen_dirty_palette = TRUE;	\
											screen_dirty_all = TRUE;		\
										} while(0)
#define set_screen_dirty_frame()		screen_dirty_frame = TRUE;


/* その他 */

extern int frameskip_rate;					/* 画面表示の更新間隔 */
extern int monitor_analog;					/* アナログモニター */
extern int use_auto_skip;					/* 自動フレームスキップ */



/*
 * 表示設定
 */

enum {
	SCREEN_INTERLACE_NO = 0,				/* インターレス表示しない */
	SCREEN_INTERLACE_YES = 1,				/* インターレス表示する */
	SCREEN_INTERLACE_SKIP = -1				/* 1ラインおき表示する */
};
extern int use_interlace;					/* インターレース表示 */

extern int use_half_interp;					/* 画面サイズ半分時、色補間する */

enum {										/* 画面サイズ             */
	SCREEN_SIZE_HALF,						/*              320 x 200 */
	SCREEN_SIZE_FULL,						/*              640 x 400 */
#ifdef	SUPPORT_DOUBLE
	SCREEN_SIZE_DOUBLE,						/*              1280x 800 */
#endif
	SCREEN_SIZE_END
};
extern int screen_size;						/* 画面サイズ指定 */

extern int use_fullscreen;					/* 全画面表示指定 */

extern double mon_aspect;					/* モニターのアスペクト比 */


extern int show_toolbar;					/* ツールバー表示有無 */
extern int show_statusbar;					/* ステータスバー表示有無 */


/*
 *
 */

enum {
	SHOW_MOUSE = 0,
	HIDE_MOUSE = 1,
	AUTO_MOUSE = 2
};
extern int hide_mouse;						/* マウスを隠すかどうか */
enum {
	UNGRAB_MOUSE = 0,
	GRAB_MOUSE	 = 1
};
extern int grab_mouse;						/* グラブするかどうか */

extern int use_swcursor;					/* メニュー専用カーソル表示有無 */


/*
 * 表示デバイス用ワーク
 */

extern int WIDTH;							/* 描画バッファ横サイズ */
extern int HEIGHT;							/* 描画バッファ縦サイズ */



/***********************************************************************
 * 画面処理の初期化・終了
 *      screen_init()   画面を生成する。起動時に呼ばれる。
 *      screen_exit()   後かたづけする。終了時に呼ばれる。
 ************************************************************************/
int  screen_init(void);
void screen_exit(void);


/***********************************************************************
 * モード切り替え時/ウインドウ生成時の、各種再設定
 *      全エリアを強制描画する必要があるので、その準備をする。
 *      grab_mouse 、 hide_mouse などに基づき、マウスの設定をする。
 *      キーリピートや、ステータスも設定する。
 *		new_screen は、モード切り替え時はFALSE、ウインドウ生成時はTRUE
 ************************************************************************/
void screen_switch(int new_screen);


/***********************************************************************
 * PC-8801の最終的な色を取得する
 ************************************************************************/
void screen_get_emu_palette(PC88_PALETTE_T pal[16]);


/***********************************************************************
 * 描画
 ************************************************************************/
void screen_update(void);					/* 描画	  (1/60sec毎)  */
void screen_update_immidiate(void);			/* 即描画 (モニター用) */


/***********************************************************************
 * フレームスキップ
 ************************************************************************/
void frameskip_blink_reset(void);			/* 点滅処理 再初期化 */
void frameskip_counter_reset(void);			/* フレームスキップ 再初期化 */
void frameskip_check(int on_time);			/* フレームスキップ 判定 */

int  quasi88_cfg_now_frameskip_rate(void);
void quasi88_cfg_set_frameskip_rate(int rate);



/***********************************************************************
 * HALFサイズ時の色補完の有効・無効関連の関数
 ***********************************************************************/
int  quasi88_cfg_can_interp(void);
int  quasi88_cfg_now_interp(void);
void quasi88_cfg_set_interp(int enable);

/***********************************************************************
 * INTERLACEの設定関連の関数
 ***********************************************************************/
int  quasi88_cfg_now_interlace(void);
void quasi88_cfg_set_interlace(int interlace_mode);

/***********************************************************************
 * ツールバー・ステータスバー表示設定関連の関数
 ***********************************************************************/
int  quasi88_cfg_now_toolbar(void);
void quasi88_cfg_set_toolbar(int show);
int  quasi88_cfg_now_statusbar(void);
void quasi88_cfg_set_statusbar(int show);

/***********************************************************************
 * 全画面設定・画面サイズ設定関連の関数
 ***********************************************************************/
int  quasi88_cfg_can_fullscreen(void);
int  quasi88_cfg_now_fullscreen(void);
void quasi88_cfg_set_fullscreen(int fullscreen);
int  quasi88_cfg_max_size(void);
int  quasi88_cfg_min_size(void);
int  quasi88_cfg_now_size(void);
void quasi88_cfg_set_size(int new_size);
void quasi88_cfg_set_size_large(void);
void quasi88_cfg_set_size_small(void);
void quasi88_cfg_reset_window(void);

/***********************************************************************
 * ???
 ***********************************************************************/
int quasi88_cfg_can_touchkey(void);
enum {
	TOUCHKEY_NOTIFY_REQ_ACTION,
	TOUCHKEY_NOTIFY_REQ_SHOW,
	TOUCHKEY_NOTIFY_REQ_HIDE,
	TOUCHKEY_NOTIFY_REQ_PREV,
	TOUCHKEY_NOTIFY_REQ_NEXT,
	TOUCHKEY_NOTIFY_REQ_SHIFT_OFF,
	TOUCHKEY_NOTIFY_REQ_SHIFT_ON,
	TOUCHKEY_NOTIFY_REQ_SHIFTL_OFF,
	TOUCHKEY_NOTIFY_REQ_SHIFTL_ON,
	TOUCHKEY_NOTIFY_REQ_SHIFTR_OFF,
	TOUCHKEY_NOTIFY_REQ_SHIFTR_ON,
	TOUCHKEY_NOTIFY_REQ_CTRL_OFF,
	TOUCHKEY_NOTIFY_REQ_CTRL_ON,
	TOUCHKEY_NOTIFY_REQ_GRAPH_OFF,
	TOUCHKEY_NOTIFY_REQ_GRAPH_ON,
	TOUCHKEY_NOTIFY_REQ_KANA_OFF,
	TOUCHKEY_NOTIFY_REQ_KANA_ON,
	TOUCHKEY_NOTIFY_REQ_CAPS_OFF,
	TOUCHKEY_NOTIFY_REQ_CAPS_ON,
	TOUCHKEY_NOTIFY_REQ_ROMAJI_OFF,
	TOUCHKEY_NOTIFY_REQ_ROMAJI_ON,
	TOUCHKEY_NOTIFY_REQ_NUM_OFF,
	TOUCHKEY_NOTIFY_REQ_NUM_ON,
	TOUCHKEY_NOTIFY_REQ_END
};
void quasi88_notify_touchkey(int request);
int quasi88_info_draw_count(void);

/* 指定された座標がどのエリアに位置するかを返す */
enum {
	AREA_TOOLBAR,		/* ツールバーの領域 */
	AREA_STATUSBAR,		/* ステータスバーの領域 */
	AREA_BORDER,		/* エミュメイン周辺の領域 */
	AREA_USER,			/* エミュメイン画面の領域 */
};
int get_area_of_coord(int x, int y);

#endif /* SCREEN_H_INCLUDED */
