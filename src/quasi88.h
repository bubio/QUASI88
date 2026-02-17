#ifndef QUASI88_H_INCLUDED
#define QUASI88_H_INCLUDED


/*----------------------------------------------------------------------
 * システム・環境依存の定義
 *----------------------------------------------------------------------*/
#include "config.h"


/*----------------------------------------------------------------------
 * ファイルシステム依存の定義
 *----------------------------------------------------------------------*/
#include "filename.h"


/* QUASI88 が内部で保持可能なパス込みのファイル名バイト数 (NUL含む) */
#define QUASI88_MAX_FILENAME	(1024)


/*----------------------------------------------------------------------
 * バージョン情報
 *----------------------------------------------------------------------*/
#include "version.h"



/*----------------------------------------------------------------------
 * 共通定義
 *----------------------------------------------------------------------*/

typedef unsigned char	Uchar;
typedef unsigned short	Ushort;
typedef unsigned int	Uint;
typedef unsigned long	Ulong;

typedef unsigned char	byte;
typedef unsigned short	word;
typedef signed   char	offset;


typedef unsigned char	bit8;
typedef unsigned short	bit16;
typedef unsigned int	bit32;


#define COUNTOF(arr)	(int)(sizeof(arr)/sizeof((arr)[0]))
#define OFFSETOF(s, m)	((size_t)(&((s *)0)->m))
#undef  MAX
#define MAX(a,b)		(((a)>(b))?(a):(b))
#undef  MIN
#define MIN(a,b)		(((a)<(b))?(a):(b))
#undef  ABS
#define ABS(x)			(((x) >= 0)? (x) : -(x))
#define SGN(x)			(((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))
#define BETWEEN(l,x,h)	((l) <= (x) && (x) <= (h))


#ifdef LSB_FIRST						/* リトルエンディアン */

typedef union {
	struct {
		byte l, h;
	}	B;
	word W;
} pair;

#else									/* ビッグエンデイアン */

typedef union {
	struct {
		byte h, l;
	}	B;
	word W;
} pair;

#endif



#ifndef TRUE
#define TRUE	(1)
#endif
#ifndef FALSE
#define FALSE	(0)
#endif



#ifndef INLINE
#define INLINE	static
#endif



/*----------------------------------------------------------------------
 * 変数 (verbose_*)、関数
 *----------------------------------------------------------------------*/
extern int verbose_level;					/* 冗長レベル */
extern int verbose_proc;					/* 処理の進行状況の表示 */
extern int verbose_z80;						/* Z80処理エラーを表示 */
extern int verbose_io;						/* 未実装I/Oアクセス報告 */
extern int verbose_pio;						/* PIO の不正使用を表示 */
extern int verbose_fdc;						/* FDイメージ異常を報告 */
extern int verbose_wait;					/* ウエイト時の異常を報告 */
extern int verbose_suspend;					/* サスペンド時の異常を報告 */
extern int verbose_snd;						/* サウンドのメッセージ */


enum {
	EVENT_NONE			= 0x0000,
	EVENT_FRAME_UPDATE	= 0x0001,
	EVENT_AUDIO_UPDATE	= 0x0002,
	EVENT_MODE_CHANGED	= 0x0004,
	EVENT_DEBUG			= 0x0008,
	EVENT_QUIT			= 0x0010
};
extern int quasi88_event_flags;


#define INIT_POWERON	(0)
#define INIT_RESET		(1)
#define INIT_STATELOAD	(2)

void quasi88(void);

void quasi88_start(void);
void quasi88_main(void);
void quasi88_stop(void);
enum {
	QUASI88_LOOP_EXIT,
	QUASI88_LOOP_ONE,
	QUASI88_LOOP_BUSY
};
int  quasi88_loop(void);

void quasi88_atexit(void (*function)(void));
void quasi88_exit(int status);

void quasi88_exec(void);
void quasi88_exec_step(void);
void quasi88_exec_trace(void);
void quasi88_exec_trace_change(void);
void quasi88_menu(void);
void quasi88_pause(void);
void quasi88_askreset(void);
void quasi88_askspeedup(void);
void quasi88_askdiskchange(void);
void quasi88_askquit(void);
void quasi88_monitor(void);
void quasi88_debug(void);
void quasi88_quit(void);
int  quasi88_is_exec(void);
int  quasi88_is_monitor(void);
int  quasi88_is_menu_mode(void);
int  quasi88_is_fullmenu(void);
int  quasi88_is_pause(void);
int  quasi88_is_askreset(void);
int  quasi88_is_askspeedup(void);
int  quasi88_is_askdiskchange(void);
int  quasi88_is_askquit(void);
int  quasi88_get_menu_mode(void);
void quasi88_set_status(void);



/*----------------------------------------------------------------------
 * その他       (実体は、 quasi88.c  にて定義してある)
 *----------------------------------------------------------------------*/
void wait_vsync_switch(void);



/*----------------------------------------------------------------------
 *
 *----------------------------------------------------------------------*/
enum {
	CTRL_NO,

	CTRL_SETUP_FULLSCREEN,
	CTRL_SETUP_KEYBOARD,

	CTRL_RESET,
	CTRL_MODE_EXEC,
	CTRL_MODE_MONITOR,
	CTRL_MODE_QUIT,
	CTRL_MODE_MENU_FULLMENU,
	CTRL_MODE_MENU_PAUSE,
	CTRL_MODE_MENU_ASKRESET,
	CTRL_MODE_MENU_ASKSPEEDUP,
	CTRL_MODE_MENU_ASKDISKCHANGE,
	CTRL_MODE_MENU_ASKQUIT,

	CTRL_CHG_SUBCPU,
	CTRL_CHG_FDCWAIT,
	CTRL_CHG_INTERLACE,
	CTRL_CHG_PCG,
	CTRL_CHG_MOUSE,
	CTRL_CHG_CURSOR,
	CTRL_CHG_FRAMERATE,
	CTRL_CHG_VOLUME,
	CTRL_CHG_SCREENSIZE,
	CTRL_CHG_FULLSCREEN,
	CTRL_CHG_WAIT,
	CTRL_CHG_NOWAIT,
	CTRL_CHG_DRIVE1_EMPTY,
	CTRL_CHG_DRIVE1_IMAGE,
	CTRL_CHG_DRIVE2_EMPTY,
	CTRL_CHG_DRIVE2_IMAGE,
	CTRL_CHG_CAPSLOCK,
	CTRL_CHG_KANALOCK,
	CTRL_CHG_ROMAJILOCK,
	CTRL_CHG_NUMLOCK,
	CTRL_CHG_TOOLBAR,
	CTRL_CHG_STATUSBAR,

	CTRL_END
};



#endif /* QUASI88_H_INCLUDED */
