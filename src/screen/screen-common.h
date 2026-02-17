#ifndef SCREEN_COMMON_H_INCLUDED
#define SCREEN_COMMON_H_INCLUDED

/* 表示更新エリアをセットする構造体 */
typedef struct {
	int x0;
	int y0;
	int x1;
	int y1;
} T_RECTANGLE;

typedef struct {
	int exist;
	int offset_x;
	int offset_y;
	T_RECTANGLE rectangle;
} T_UPDATE_RECTANGLE;


/* T_GRAPH_INFO から必要な情報だけを覚えておくワーク */
typedef struct {
	int		write_only;
	int		broken_mouse;
	void	(*draw_start)(void);
	void	(*draw_finish)(void);
	int     (*dont_frameskip)(void);
} T_UPDATE_INFO;
extern T_UPDATE_INFO update_info;		/* T_GRAPH_INFO の抜粋 */


/* 表示デバイス用ワーク */
/*extern int WIDTH;						 * 描画バッファ横サイズ */
/*extern int HEIGHT;					 * 描画バッファ縦サイズ */
extern int DEPTH;						/* 色ビット数 (8/16/32) */

extern char *screen_buf;				/* 描画バッファ先頭 */
extern char *screen_user_start;			/* ユーザエリア先頭 */
extern char *screen_tool_start;			/* ツールエリア先頭 */
extern char *screen_stat_start;			/* ステータスエリア先頭 */

extern int screen_user_dx;				/* メインエリア 座標オフセット */
extern int screen_user_dy;
extern int screen_tool_dx;				/* ツールエリア 座標オフセット */
extern int screen_tool_dy;
extern int screen_stat_dx;				/* ステータスエリア 座標オフセット */
extern int screen_stat_dy;


/*
 *  |<-------------------------- WIDTH --------------------------->|
 *
 *  @---------------------------------------------------------+----+  --
 *  |       ^ ^ ^                                             |////|  ^
 *  |<------|-|-|->|<--------- 画面幅 -------->|              |////|  |
 *  |.._dx  | | v                                             |////|  |
 *  |       | | -  %---------------------------+  --          |////|  |
 *  |       | v    | [Tool Area]               |  ^           |////|  |
 *  |       | -    #---------------------------+  |           |////|  |
 *  |       |      |                           |  |           |////|  |
 *  |       |      |                           |  |           |////|  |
 *  |  .._dy|      |                           |  |           |////|  |
 *  |       |      | [Main Area]               | 画面高       |////| HEIGHT
 *  |       |      |                           |  |           |////|  |
 *  |       |      |                           |  |           |////|  |
 *  |       v      |                           |  |           |////|  |
 *  |       -      $---------------------------+  v           |////|  |
 *  |              | [Status Area]             |  |           |////|  |
 *  |              +---------------------------+  --          |////|  |
 *  |                                                         |////|  |
 *  |                                                         |////|  |
 *  |                                                         |////|  v
 *  +---------------------------------------------------------+----+  --
 *
 *  screen_buf   描画バッファ全域の、先頭ポインタ == 上記 @
 *  WIDTH        描画バッファ全域の、横ピクセル数 (非表示部分含む)
 *  HEIGHT       〃                  縦ピクセル数
 *  DEPTH        描画バッファ全域の、色深度 (バッファのビット幅、8/16/32)
 *
 *  screen_size  画面バッファのサイズインデックス番号
 *  画面幅       画面バッファの、横ピクセル数 (320/640/1280)
 *  画面高       〃              縦ピクセル数 (200+α/400+α/800+α)
 *              いずれも、画面サイズ (screen_size) および、
 *              ツールバー、ステータスバーの表示非表示によりサイズが決まる
 *
 *  screen_user_start   メインエリアの、先頭ポインタ             (上記 #)
 *  screen_tool_start   ツールエリアの、先頭ポインタ or NULL     (上記 %)
 *  screen_stat_start   ステータスエリアの、先頭ポインタ or NULL (上記 $)
 *
 *  screen_user_dx      メインエリアの、座標オフセット
 *  screen_user_dy      〃
 *  screen_tool_dx      ツールエリアの、座標オフセット
 *  screen_tool_dy      〃
 *  screen_stat_dx      ステータスエリアの、座標オフセット
 *  screen_stat_dy      〃
 *
 */

extern int now_half_interp;					/* 現在、色補完中なら真 */
extern int now_screen_size;					/* 実際の、画面サイズ */


extern Ulong color_pixel[16];				/* EMUピクセル値 */
extern Ulong color_half_pixel[16][16];		/* EMU色補完時のピクセル値 */
extern Ulong menu_pixel[12];				/* MENUピクセル値 */
extern Ulong menu_half_pixel[12][12];		/* MENU色補完時のピクセル値 */
extern Ulong black_pixel;					/* 黒のピクセル値 */



void remove_palette(void);
void menu_palette(int half_interp);
void emu_palette(PC88_PALETTE_T syspal[], int half_interp);

void set_vram2screen_list(void);
void put_image_all(void);
void clear_all_screen(void);

#endif /* SCREEN_COMMON_H_INCLUDED */
