/************************************************************************/
/*                                                                      */
/* 画面の表示                                                           */
/*                                                                      */
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "screen.h"
#include "screen-common.h"
#include "graph.h"
#include "statusbar.h"
#include "toolbar.h"

#include "keyboard.h"
#include "crtcdmac.h"
#include "pc88main.h"
#include "event.h"

#include "suspend.h"

#include "alarm.h"
#include "q8tk.h"

#include "menu.h"
#include "pause.h"                      /* pause_event_focus_in_when_pause() */



PC88_PALETTE_T vram_bg_palette;				/* 背景パレット */
PC88_PALETTE_T vram_palette[8];				/* 各種パレット */

byte sys_ctrl;								/* OUT[30] SystemCtrl */
byte grph_ctrl;								/* OUT[31] GraphCtrl */
byte grph_pile;								/* OUT[53] 重ね合わせ */



int  screen_dirty_tool_full;				/* ツールエリア全部更新 */
int  screen_dirty_tool_part;				/* ツールエリア差分更新 */
int  screen_dirty_stat_full;				/* ステータスエリア全部更新 */
int  screen_dirty_stat_part;				/* ステータスエリア差分更新 */
int  screen_dirty_menu;						/* ユーザエリア(MENU)差分更新 */
char screen_dirty_flag[ 0x4000 * 2 ];		/* ユーザエリア(EMU)差分更新 */
int  screen_dirty_all;						/* ユーザエリア全部更新 */
int  screen_dirty_palette;					/* 色情報 更新 */
int  screen_dirty_frame;					/* 全領域 更新 */



int frameskip_rate = DEFAULT_FRAMESKIP;		/* 画面表示の更新間隔 */
int monitor_analog = TRUE;					/* アナログモニター */
int use_auto_skip  = TRUE;					/* 自動フレームスキップ */


int use_interlace = 0;						/* インターレース表示 */

static int enable_half_interp = FALSE;		/* HALF時、色補間可能か否か */
int        use_half_interp = TRUE;			/* HALF時、色補間する */
int        now_half_interp = FALSE;			/* 現在、色補完中なら真 */



typedef struct {							/* 画面サイズのリスト */
	int		w;
	int		h;
	int		user_h;
	int		tool_h;
	int		stat_h;
} SCREEN_SIZE_TABLE;

static const SCREEN_SIZE_TABLE screen_size_tbl[ SCREEN_SIZE_END ] = {

	{ (640 / 2), (480 / 2), (400 / 2), (64 / 2), (16 / 2), },	/* HALF */
	{ (640 * 1), (480 * 1), (400 * 1), (64 * 1), (16 * 1), },	/* FULL */
#ifdef	SUPPORT_DOUBLE
	{ (640 * 2), (480 * 2), (400 * 2), (64 * 2), (16 * 2), },	/* DOUBLE */
#endif
};


static int screen_size_max = SCREEN_SIZE_END - 1;	/* 変更可能な最大サイズ */
static int screen_size_min = SCREEN_SIZE_HALF;		/* 変更可能な最小サイズ */
int        screen_size     = SCREEN_SIZE_FULL;		/* 画面サイズ指定 */
int        now_screen_size;							/* 実際の、画面サイズ */


static int enable_fullscreen = 0;			/* 全画面表示可能かどうか */
int        use_fullscreen = FALSE; 			/* 全画面表示指定 */
static int now_fullscreen = FALSE;	 		/* 現在、全画面表示中なら真 */


double mon_aspect = 0.0;					/* モニターのアスペクト比指定 */


int WIDTH  = 0;								/* 描画バッファ横サイズ */
int HEIGHT = 0;								/* 描画バッファ縦サイズ */
int DEPTH  = 8;								/* 色ビット数 (8 or 16 or 32) */

char *screen_buf;							/* 描画バッファ先頭 */
char *screen_user_start;					/* ユーザエリア先頭 */
char *screen_tool_start;					/* ツールエリア先頭 */
char *screen_stat_start;					/* ステータスエリア先頭 */

int screen_user_dx;							/* メインエリア座標オフセット */
int screen_user_dy;
int screen_tool_dx;							/* ツールエリア座標オフセット */
int screen_tool_dy;
int screen_stat_dx;							/* ステータスエリア座標オフセット*/
int screen_stat_dy;

static int border_w;						/* ボーダー(枠)の幅 */
static int border_h;



int show_toolbar = TRUE;					/* ツールバー表示有無 */
int show_statusbar = TRUE;					/* ステータスバー表示有無 */



T_UPDATE_INFO update_info;					/* T_GRAPH_INFO の抜粋 */



int        hide_mouse = AUTO_MOUSE;			/* マウスを隠すかどうか */
int        grab_mouse = UNGRAB_MOUSE;		/* グラブするかどうか */
static int now_grabbing = FALSE;			/* 現在グラブ中かどうか */
static int now_mouse_show = TRUE;			/* 現在マウス表示中かどうか */
static int now_key_repeat = TRUE;			/* 現在キーリピート中かどうか */

int use_swcursor = FALSE;					/* メニュー専用カーソル表示有無 */


/* 現在の、補正済みマウス座標
 *   ツールエリア左上を原点とした (0,0) - (640,480) の座標系で補正している。
 *   画面サイズやボーダーの有無に関わらず、この座標系を使う。
 *   つまりユーザエリア(EMU画面)内は (0,64) - (640,464) の値をとる。
 *   なお、範囲外にはみ出ることもある。 */
static int sys_mouse_x;
static int sys_mouse_y;
static int sys_mouse_abs = TRUE;

static void check_half_interp(void);
static void screen_attr_setup(int stat, int grab_if_on_exec);

/***********************************************************************
 * 画面処理の初期化・終了
 ************************************************************************/
static const T_GRAPH_SPEC *spec;

static int open_window(void);
static int open_window_or_exit(void);

int screen_init(void)
{
	int i;
	int w, h, max, min;

	spec = graph_init();
	if (spec == NULL) {
		return FALSE;
	}


	/* spec によって、ウインドウの最大・最小サイズを決定 */

	min = -1;
	for (i = 0; i < SCREEN_SIZE_END; i++) {
		if (i == SCREEN_SIZE_HALF && spec->forbid_half) {
			continue;
		}

		if (spec->window_max_width  >= screen_size_tbl[ i ].w &&
			spec->window_max_height >= screen_size_tbl[ i ].h) {
			min = i;
			break;
		}
	}
	max = -1;
	for (i = SCREEN_SIZE_END - 1; i >= 0; i--) {
		if (spec->window_max_width  >= screen_size_tbl[ i ].w &&
			spec->window_max_height >= screen_size_tbl[ i ].h) {
			max = i;
			break;
		}
	}
	if (min < 0 || max < 0 || max < min) {
		if (verbose_proc) {
			printf("  Not found drawable window size (bug?)\n");
		}
		return FALSE;
	}
	screen_size_max = max;
	screen_size_min = min;


	/* spec によって、全画面可能かどうかを決定 */

	if (spec->forbid_half) {
		i = SCREEN_SIZE_FULL;
	} else {
		i = SCREEN_SIZE_HALF;
	}

	if (spec->fullscreen_max_width  >= screen_size_tbl[ i ].w &&
		spec->fullscreen_max_height >= screen_size_tbl[ i ].h) {
		enable_fullscreen = TRUE;
	} else {
		enable_fullscreen = FALSE;
	}



	/* screen_size, WIDTH, HEIGHT にコマンドラインで指定したウインドウサイズが
	   セット済みなので、それをもとにボーダー(枠)のサイズを算出する */

	w = screen_size_tbl[ screen_size ].w;
	h = screen_size_tbl[ screen_size ].user_h;

	border_w = ((MAX(WIDTH,  w) - w) / 2) & ~7;        /* 8の倍数 */
	border_h = ((MAX(HEIGHT, h) - h) / 2);

	if (open_window()) {
		clear_all_screen();
		put_image_all();

		submenu_controll(CTRL_SETUP_FULLSCREEN);
		submenu_controll(CTRL_SETUP_KEYBOARD);
		return TRUE;
	} else {
		return FALSE;
	}
}


void screen_exit(void)
{
	graph_exit();
}



/*----------------------------------------------------------------------
 * ウインドウの生成
 *----------------------------------------------------------------------*/
/*
  以下の変数に基づき、画面を生成し、以下の変数をセットする
        use_fullscreen, enable_fullscreen               → now_fullscreen
        screen_size   , screen_size_max, screen_size_min
        border_w     , border_h
 */

static int open_window(void)
{
	int size, found = FALSE, with_border = FALSE;
	int w = 0, h = 0;
	int user_h, tool_h, stat_h;
	const T_GRAPH_INFO *info;

	remove_palette();

	if (enable_fullscreen == FALSE) {	/* 全画面不可なら、全画面指示は却下 */
		use_fullscreen = FALSE;
	}


	/* フルスクリーン表示が可能なサイズを、大きいほうから探し出す */

	if (use_fullscreen) {
		for (size = screen_size; size >= screen_size_min; size--) {

			w = screen_size_tbl[ size ].w;
			h = screen_size_tbl[ size ].h;

			/* ボーダー込みで表示可能かどうか判定 */
			if ((w + border_w * 2) <= spec->fullscreen_max_width &&
				(h + border_h * 2) <= spec->fullscreen_max_height) {
				with_border = TRUE;
				found = TRUE;
				break;
			}

			/* ボーダーなしなら表示可能かどうか判定 */
			if (w <= spec->fullscreen_max_width &&
				h <= spec->fullscreen_max_height) {
				found = TRUE;
				break;
			}
		}

		if (found == FALSE) {
			/* 表示可能サイズ無しなら全画面不可 */
			use_fullscreen = FALSE;
		}
	}

	/* ウインドウ表示が表示な可能サイズを、大きいほうから探し出す */

	if (use_fullscreen == FALSE) {
		for (size = screen_size; size >= screen_size_min; size--) {

			w = screen_size_tbl[ size ].w;
			h = screen_size_tbl[ size ].h;

			/* ボーダー込みで表示可能かどうか判定 */
			if ((w + border_w * 2) <= spec->window_max_width &&
				(h + border_h * 2) <= spec->window_max_height) {
				with_border = TRUE;
				found = TRUE;
				break;
			}

			/* ボーダーなしなら表示可能かどうか判定 */
			if (w <= spec->window_max_width &&
				h <= spec->window_max_height) {
				found = TRUE;
				break;
			}
		}
	}


	if (found == FALSE) {
		/* これはありえないハズだが、念のため… */
		return FALSE;
	}

	/* ツールバーやステータスバーを表示するかどうかで、サイズを補正 */

	user_h = screen_size_tbl[ size ].user_h;

	if (show_toolbar) {
		tool_h = screen_size_tbl[ size ].tool_h;
	} else {
		tool_h = 0;
	}
	if (show_statusbar) {
		stat_h = screen_size_tbl[ size ].stat_h;
	} else {
		stat_h = 0;
	}

	h = user_h + tool_h + stat_h;

	if (with_border) {
		w += border_w * 2;
		h += border_h * 2;
	}

	/* サイズが決まったので、いよいよ画面を生成する */
	now_screen_size = size;

	info = graph_setup(w, h, use_fullscreen, (float)mon_aspect);

	if (info) {
		int area_w, area_h, area_dx, area_dy;

		/* サイズの諸言を計算 */
		WIDTH  = info->byte_per_line / info->byte_per_pixel;
		HEIGHT = info->height;
		DEPTH  = info->byte_per_pixel * 8;

		area_w = screen_size_tbl[ size ].w;
		area_h = user_h + tool_h + stat_h;

		area_dx = (info->width  - area_w) / 2;
		area_dy = (HEIGHT       - area_h) / 2;

		screen_tool_dx = area_dx;
		screen_user_dx = area_dx;
		screen_stat_dx = area_dx;

		screen_tool_dy = area_dy;
		screen_user_dy = area_dy + tool_h;
		screen_stat_dy = area_dy + tool_h + user_h;

		if (info->fullscreen) {
			now_fullscreen = TRUE;
		} else {
			now_fullscreen = FALSE;
		}


		/* 使える色の数をチェック */
		if (info->nr_color >= 214) {
			/* いっぱい使える */
			enable_half_interp = TRUE;

		} else if (info->nr_color >= 28) {
			/* 半分モードの色補間はだめ */
			enable_half_interp = FALSE;

		} else {
			/* ぜんぜん色が足りない */
			return FALSE;
		}

		/* HALFサイズ時の色補完有無を設定 */
		check_half_interp();


		/* スクリーンバッファの、描画開始位置を設定 */
		screen_buf = (char *)info->buffer;
		screen_user_start = &screen_buf[(WIDTH * screen_user_dy + area_dx)
										* info->byte_per_pixel ];
		if (show_toolbar) {
			screen_tool_start = &screen_buf[(WIDTH * screen_tool_dy + area_dx)
											* info->byte_per_pixel ];
		} else {
			screen_tool_start = NULL;
		}
		if (show_statusbar) {
			screen_stat_start = &screen_buf[(WIDTH * screen_stat_dy + area_dx)
											* info->byte_per_pixel ];
		} else {
			screen_stat_start = NULL;
		}


		/* その他の情報を保持 */
		update_info.write_only = info->write_only;
		update_info.draw_start  = info->draw_start;
		update_info.draw_finish = info->draw_finish;
		update_info.dont_frameskip = info->dont_frameskip;
		update_info.broken_mouse = info->broken_mouse;


		/* メニュー用の色ピクセルを定義 */
		menu_palette(enable_half_interp);


		/* 転送・表示関数のリストを設定 */
		set_vram2screen_list();


		screen_switch(TRUE);

		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 * ウインドウの再生成。失敗したら exit
 */
static int open_window_or_exit(void)
{
	int old = now_fullscreen;

	if (open_window() == FALSE) {
		fprintf(stderr, "Sorry : Graphic System Fatal Error !!!\n");

		quasi88_exit(1);
	}

	/* 全画面に失敗したら、全画面の要求はオフする */
	if (use_fullscreen && !now_fullscreen) {
		use_fullscreen = now_fullscreen;
	}

	/* 全画面とウインドウが切り替わったら真を返す */
	return (old != now_fullscreen) ? TRUE : FALSE;
}



/*----------------------------------------------------------------------*/

/* HALFサイズ色補間の有無ワークを設定 */
static void check_half_interp(void)
{
	if (now_screen_size == SCREEN_SIZE_HALF &&
		enable_half_interp  &&
		use_half_interp) {
		/* 現在 HALFサイズでフィルタリング可能で色補完あり */
		now_half_interp = TRUE;

	} else {
		now_half_interp = FALSE;
	}
}

/* マウスの座標を、画面サイズに合わせて補正する (ツールエリア左上が原点) */
static void mouse_movement_adjust(int *x, int *y)
{
	(*x) -= screen_user_dx;
	(*y) -= screen_user_dy;

	switch (quasi88_cfg_now_size()) {
	case SCREEN_SIZE_HALF:
		(*x) *= 2;
		(*y) *= 2;
		break;

	case SCREEN_SIZE_FULL:
		break;

#ifdef SUPPORT_DOUBLE
	case SCREEN_SIZE_DOUBLE:
		(*x) /= 2;
		(*y) /= 2;
		break;
#endif
	}

	(*y) += 64;
}

/* 指定された座標がどのエリアに位置するかを返す */
int get_area_of_coord(int x, int y)
{
	if (show_toolbar &&
		BETWEEN(0, x, 640) &&
		BETWEEN(0, y, 63)) {
		/* ツールバーのエリア */
		return AREA_TOOLBAR;
	}

	if (show_statusbar &&
		BETWEEN(0, x, 640) &&
		BETWEEN(464, y, 480)) {
		/* ステータスバーのエリア */
		return AREA_STATUSBAR;
	}

	if (BETWEEN(0, x, 640) &&
		BETWEEN(64, y, 480)) {
		return AREA_USER;
	} else {
		return AREA_BORDER;
	}
}

/* マウス座標がユーザエリアなら真を返す (ツール/テータスバーエリアなら偽) */
#define	is_mouse_in_user_area(x, y)								\
	((get_area_of_coord((x), (y)) == AREA_USER) ? TRUE : FALSE)



/***********************************************************************
 * モード切り替え時/ウインドウ生成時の、各種再設定
 ************************************************************************/
void screen_switch(int new_screen)
{
	char *title;

	/* 全エリア強制描画の準備 */

	set_screen_dirty_frame();			/* 全領域 初期化(==更新) */
	set_screen_dirty_palette();			/* 色情報 初期化(==更新) */

	frameskip_counter_reset();			/* 次回描画 */


	/* 現在のマウス位置を取得 */

	event_get_mouse_pos(&sys_mouse_x, &sys_mouse_y);
	mouse_movement_adjust(&sys_mouse_x, &sys_mouse_y);


	/* マウス、キーリピートの設定 */

	screen_attr_setup((new_screen) ? 1 : 0,
					  (grab_mouse == GRAB_MOUSE) ? TRUE : FALSE);


	/* ウインドウタイトルを表示 */

	title = Q_TITLE " ver " Q_VERSION;
	graph_set_window_title(title);
}



/***********************************************************************
 * マウス表示、グラブ、キーリピートを設定
 ************************************************************************/
/*------------------------------------------------------------------------
 * 引数 stat (=呼び出しタイミング)     0:モード切り替え時
 *                                     1:ウインドウ生成時
 *                                     2:マウスクリック時
 *                                     3:マウス移動時
 *                                     4:タイムアップ時時
 *      grab_if_on_exec (=グラブ要求)  0:グラブ要求なし
 *                                     1:グラブ要求あり
 *
 * ・モード切り替え時／
 *   ウインドウ生成時 … 各々の発生時に、この関数を呼び出す。
 *                       グラブ要求は、 grab_mouse == GRAB_MOUSE なら要求あり、
 *                       以外は、要求なしとする。
 *                       これに基づき、グラブ要否やマウス表示などが決まる。
 *
 * ・マウスクリック時 … grab_mouse == AUTO_MOUSE かつ マウス位置がユーザエリア
 *                       かつ マウス左or中クリックしたら、この関数を呼び出す。
 *                       グラブ要求は、左クリックなら要求あり、右クリックなら
 *                       要求なしとする。
 *                       これに基づき、グラブ要否やマウス表示などが決まる。
 *
 * ・マウス移動       … マウス移動時に、この関数を呼び出す。
 *                       グラブ要求は、無視される (現状維持)
 *                       マウス位置によって、マウス表示/非表示が決まる。
 *
 * ・タイムアウト     … この関数が呼び出されないまま 2秒経過すると、
 *                       タイマーコールバック処理より、この関数を呼び出す。
 *                       グラブ要求は、無視される (現状維持)
 *                       マウス非表示となる。
 *                       タイマーは hide_mouse == AUTO_MOUSE 時のみ動作する。
 *
 * グラブ要否、マウス表示、キーリピートは、以下の条件で決まる。
 *
 *           |グラブ|グラブ要否  |マウス位置  |マウス表示|キー    |
 *           |要求  |            |            |          |リピート|
 *      -----+------+------------+------------+----------+--------+
 *      EMU  |なし  |グラブしない|ユーザエリア|設定による|しない  |
 *           |      |            +------------+----------+        |
 *           |      |            |ツールエリア|表示      |        |
 *           +------+------------+------------+----------+        |
 *           |あり  |グラブする  |ユーザエリア|非設定    |        |
 *           |      |            +------------+----------+        |
 *           |      |            |ツールエリア|表示      |        |
 *      -----+------+------------+------------+----------+--------+
 *      MENU |-     |グラブしない|-           |表示      |する    |
 *      -----+-------------------+------------+----------+--------+
 *
 * 現在と状態が変化すれば、システムにこれらを要求する。
 * (ウインドウ生成時は、現在の状態によらずシステムに要求する)。
 * 要求の結果、現在マウス表示中か、グラブ中か、を取得する。
 *
 * それに応じて SWカーソルを使うかどうかなどを最終的に決める。
 *
 *------------------------------------------------------------------------*/

static void *dummy_alarm_id;
static void attr_setup_timeup(void *unused1, void *unused2)
{
	if (quasi88_has_focus()) {
		screen_attr_setup(4, FALSE);
	}
}

static void screen_attr_setup(int stat, int grab_if_on_exec)
{
	int prev_grabbing, request_grab;
	int show, grab, repeat;
	int in_user_area = is_mouse_in_user_area(sys_mouse_x, sys_mouse_y);

	alarm_remove(&dummy_alarm_id);

	if (stat >= 3) {
		/* マウス移動時・タイムアウト時は、グラブ状態そのまま */
		request_grab = now_grabbing;
	} else {
		/* 以外は、要求されたグラブ状態にする */
		request_grab = grab_if_on_exec;
	}


	if (quasi88_is_exec()) {
		/* EMU では */
		/* オートリピートオフ */
		repeat = FALSE;

		/* グラブは要求された通り */
		if (request_grab) {
			grab = TRUE;
		} else {
			grab = FALSE;
		}

		if (in_user_area) {
			/* マウス表示は、ツールバー以外のエリアでは設定に従う */
			if (request_grab) {
				/* ただしグラブ中は常に非表示 */
				show = FALSE;
			} else {
				if (hide_mouse == HIDE_MOUSE) {
					show = FALSE;
				} else if (hide_mouse == SHOW_MOUSE) {
					show = TRUE;
				} else {
					if (stat == 4) {
						show = FALSE;
					} else {
						show = TRUE;
						alarm_add(&dummy_alarm_id, NULL, attr_setup_timeup,
								  2000, FALSE);
					}
				}
			}
		} else {
			/* マウス表示は、ツールバーのエリアでは、常に表示 */
			if ((hide_mouse == HIDE_MOUSE) &
				(use_swcursor)) {
				show = FALSE;
			} else {
				show = TRUE;
			}
		}

	} else {
		/* MENU では */
		/* オートリピートオン、グラブなし、マウス表示 */
		repeat = TRUE;
		grab = FALSE;
		if (use_swcursor) {
			show = FALSE;
		} else {
			show = TRUE;
		}
	}

	/* システムにマウス表示などを指示 */
	prev_grabbing = now_grabbing;

	if ((stat == 1) ||
		(now_mouse_show != show) ||
		(now_grabbing != grab) ||
		(now_key_repeat != repeat)) {
		/* ウインドウ生成時、または、前回と状態が変わった時に、属性変更する */

		int result_show, result_grab;

		graph_set_attribute(show, grab, repeat,
							&result_show, &result_grab);
#if 0
		{
			static int t=0;
			printf("%04d: %c(%3d,%3d)%d : M %d->%d, G %d->%d\n", t++,
				   in_user_area ? 'U' : 'T',
				   sys_mouse_x, sys_mouse_y, sys_mouse_abs,
				   show, result_show, grab, result_grab);
		}
#endif

		now_mouse_show = result_show;
		now_grabbing = result_grab;
		now_key_repeat = repeat;
	}


	/* 結果、マウスが非表示になったら、Q8TKのソフトウェアカーソルを表示 */
	if (quasi88_is_exec()) {
		if (in_user_area) {
			q8tk_set_cursor(FALSE);
		} else {
			if (now_mouse_show) {
				q8tk_set_cursor(FALSE);
			} else {
				q8tk_set_cursor(TRUE);
			}
		}

	} else {
		if (now_mouse_show) {
			q8tk_set_cursor(FALSE);
		} else {
			q8tk_set_cursor(TRUE);
		}
	}

	/* 結果、グラブ状態が変わったら、ステータスバーに表示する */
	if ((stat == 0) ||
		(stat == 1) ||
		(prev_grabbing != now_grabbing)) {

		if (now_grabbing) {
			if (stat == 2) {
				/* マウスクリック時 */
				statusbar_message(STATUS_WARN,
								  "Mouse Captured(Cancel:Ctrl+Shift)",
								  "マウスキャプチャ/Ctrl+Shiftで解放/");
			} else {
				/* モード切り替え、ウインドウ生成、マウス移動時 */
				statusbar_message(STATUS_INFO,
								  "Mouse Captured-Mode",
								  "マウスキャプチャモードです");
			}

		} else {
			if (stat == 2) {
				/* マウスクリック時 */
				statusbar_message(STATUS_INFO,
								  "Mouse Released",
								  "マウス解放しました");
			}
		}
	}
}






/***********************************************************************
 * HALFサイズ時の色補完の有効・無効関連の関数
 ***********************************************************************/

/* 色補完の可否を返す */
int quasi88_cfg_can_interp(void)
{
	return enable_half_interp;
}

/* 色補完の現在状態を返す */
int quasi88_cfg_now_interp(void)
{
	return use_half_interp;
}

/* 色補完の有無を設定する */
void quasi88_cfg_set_interp(int enable)
{
	use_half_interp = enable;
	check_half_interp();
	set_vram2screen_list();
	set_screen_dirty_all();				/* メイン領域 初期化(==更新) */
	set_screen_dirty_tool_full();
	set_screen_dirty_stat_full();
	frameskip_counter_reset();			/* 次回描画 */
}



/***********************************************************************
 * INTERLACEの設定関連の関数
 ***********************************************************************/

/* 現在のインタレース状態を返す */
int quasi88_cfg_now_interlace(void)
{
	return use_interlace;
}

/* インタレース状態を設定する */
void quasi88_cfg_set_interlace(int interlace_mode)
{
	use_interlace = interlace_mode;
	set_vram2screen_list();
	set_screen_dirty_frame();			/* 全領域 初期化(==更新) */
	frameskip_counter_reset();			/* 次回描画 */

	submenu_controll(CTRL_CHG_INTERLACE);
}



/***********************************************************************
 * ツールバー・ステータスバー表示設定関連の関数
 ***********************************************************************/

/* ツールバー表示の現在状態を返す */
int quasi88_cfg_now_toolbar(void)
{
	return show_toolbar;
}

/* ツールバーの表示を設定する */
void quasi88_cfg_set_toolbar(int show)
{
	if (show_toolbar != show) {
		show_toolbar = show;
		if (open_window_or_exit()) {
			submenu_controll(CTRL_CHG_FULLSCREEN);
		}

		submenu_controll(CTRL_CHG_TOOLBAR);
	}
}

/* ステータスバー表示の現在状態を返す */
int quasi88_cfg_now_statusbar(void)
{
	return show_statusbar;
}

/* ステータスバーの表示を設定する */
void quasi88_cfg_set_statusbar(int show)
{
	if (show_statusbar != show) {
		show_statusbar = show;
		if (open_window_or_exit()) {
			submenu_controll(CTRL_CHG_FULLSCREEN);
		}

		submenu_controll(CTRL_CHG_STATUSBAR);
	}
}



/***********************************************************************
 * 全画面設定・画面サイズ設定関連の関数
 ***********************************************************************/

/* 全画面の可否を返す */
int quasi88_cfg_can_fullscreen(void)
{
	return enable_fullscreen;
}

/* 全画面の現在状態を返す */
int quasi88_cfg_now_fullscreen(void)
{
	return now_fullscreen;
}

/* 全画面の切替を設定する */
void quasi88_cfg_set_fullscreen(int fullscreen)
{
	use_fullscreen = fullscreen;

	if (now_fullscreen != use_fullscreen) {
		if (open_window_or_exit()) {
			submenu_controll(CTRL_CHG_FULLSCREEN);
		}
	}
}

/* 画面サイズの最大を返す */
int quasi88_cfg_max_size(void)
{
	return screen_size_max;
}

/* 画面サイズの最小を返す */
int quasi88_cfg_min_size(void)
{
	return screen_size_min;
}

/* 画面サイズの現在状態を返す */
int quasi88_cfg_now_size(void)
{
	return now_screen_size;
}

/* 画面サイズを設定する */
void quasi88_cfg_set_size(int new_size)
{
	screen_size = new_size;

	if (now_screen_size != screen_size) {
		if (open_window_or_exit()) {
			submenu_controll(CTRL_CHG_FULLSCREEN);
		}

		submenu_controll(CTRL_CHG_SCREENSIZE);
	}
}

/* 画面サイズを大きくする */
void quasi88_cfg_set_size_large(void)
{
	if (++screen_size > screen_size_max) {
		screen_size = screen_size_min;
	}

	if (open_window_or_exit()) {
		submenu_controll(CTRL_CHG_FULLSCREEN);
	}

	submenu_controll(CTRL_CHG_SCREENSIZE);
}

/* 画面サイズを小さくする */
void    quasi88_cfg_set_size_small(void)
{
	if (--screen_size < screen_size_min) {
		screen_size = screen_size_max;
	}

	if (open_window_or_exit()) {
		submenu_controll(CTRL_CHG_FULLSCREEN);
	}

	submenu_controll(CTRL_CHG_SCREENSIZE);
}


/* ウインドウ/全画面を再度開く */
void    quasi88_cfg_reset_window(void)
{
	if (open_window_or_exit()) {
		submenu_controll(CTRL_CHG_FULLSCREEN);
	}
}



/****************************************************************************
 * 【機種依存部より、マウス移動時に呼び出される。】
 *
 *      abs_coord が、真なら、x,y はマウスの移動先の座標を示す。
 *              座標は、graph_setup() の戻り値にて応答した 画面サイズ
 *              width × height に対する値をセットすること。
 *              (なお、範囲外の値でも可とする)
 *
 *      abs_coord が 偽なら、x,y はマウスの移動量を示す。
 *****************************************************************************/
void quasi88_mouse_move(int x, int y, int abs_coord)
{
	int dx, dy;

#if 0
	printf("%c (%4d,%4d)\n", (abs_coord) ? 'A' : 'R', x, y);
#endif

	if (abs_coord) {
		mouse_movement_adjust(&x, &y);

		dx = x - sys_mouse_x;
		dy = y - sys_mouse_y;

		sys_mouse_x = x;
		sys_mouse_y = y;
		sys_mouse_abs = TRUE;

	} else {
#if 0
		int min_x =   0 - 16;
		int max_x = 640 + 16;
		int min_y =   0 - 16;
		int max_y = 480 + 16;
#else
		int min_x =   0;
		int max_x = 640;
		int min_y =  64;
		int max_y = 463;
#endif
		/* サイズによる補正は無し？ */

		dx = x;
		dy = y;

		sys_mouse_x += x;
		if (sys_mouse_x < min_x) {
			sys_mouse_x = min_x;
		}
		if (sys_mouse_x > max_x) {
			sys_mouse_x = max_x;
		}

		sys_mouse_y += y;
		if (sys_mouse_y < min_y) {
			sys_mouse_y = min_y;
		}
		if (sys_mouse_y > max_y) {
			sys_mouse_y = max_y;
		}

		sys_mouse_abs = FALSE;
	}

	/* PC-88 のポートに反映する */
	keyboard_mouse_move(dx, dy, abs_coord);

	/* マウスカーソル表示非表示を設定 */
	screen_attr_setup(3, 0);

	/* マウス動作をメニュー処理に反映する */
	q8tk_event_mouse_moved(sys_mouse_x, sys_mouse_y);

#if 0
	printf("(%4d,%4d)\n", sys_mouse_x, sys_mouse_y);
#endif
}

/******************************************************************************
 * 【機種依存部より、マウスのボタン押下時に呼び出される】
 *
 *      code は、キーコードで、 KEY88_MOUSE_L <= code <= KEY88_MOUSE_DOWN
 *      on   は、キー押下なら真、キー開放なら偽
 *****************************************************************************/
void quasi88_mouse(int code, int on)
{
	int is_exec = quasi88_is_exec();
	int in_user_area = is_mouse_in_user_area(sys_mouse_x, sys_mouse_y);

	/* 自動グラブで、マウスクリック時はグラブする */
	if ((is_exec) &&
		(in_user_area) &&
		(grab_mouse == AUTO_MOUSE)) {

		if ((code == KEY88_MOUSE_L) && (now_grabbing == FALSE)) {
			screen_attr_setup(2, TRUE);
		}
		if ((code == KEY88_MOUSE_M) && (now_grabbing)) {
			screen_attr_setup(2, FALSE);
		}
	}


	if ((is_exec) &&
		(in_user_area)) {
		if (on) {
			;
		} else {
			q8tk_event_mouse_off(code);
		}

		keyboard_mouse_button(code, on);

	} else {
		if (on) {
			q8tk_event_mouse_on(code);
		} else {
			q8tk_event_mouse_off(code);
		}
	}

#if 0
	if (on) {
		printf("(%4d,%4d) +%d\n", mouse_x, mouse_y, code);
	} else {
		printf("(%4d,%4d) -%d\n", mouse_x, mouse_y, code);
	}
#endif
}


/******************************************************************************
 *
 *****************************************************************************/
void quasi88_grab(void)
{
	if (now_grabbing == FALSE) {
		screen_attr_setup(2, TRUE);
	}
}

void quasi88_ungrab(void)
{
	if (now_grabbing) {
		screen_attr_setup(2, FALSE);
	}
}


/****************************************************************************
 * 【機種依存部より、ウインドウタッチ時に呼び出される。】
 *
 *      x,y はタッチしたの座標を示す。
 *              座標は、graph_setup() の戻り値にて応答した 画面サイズ
 *              width * height に対する値をセットすること。
 *              (なお、範囲外の値でも可とする)
 *****************************************************************************/
void quasi88_touch(int x, int y)
{
	mouse_movement_adjust(&x, &y);

	if (quasi88_is_exec() &&
		(is_mouse_in_user_area(x, y) == FALSE)) {

		q8tk_event_mouse_moved(x, y);

		q8tk_event_mouse_on(KEY88_MOUSE_L);
		q8tk_event_mouse_off(KEY88_MOUSE_L);
	}
}


/***********************************************************************
 * タッチ関連の関数
 ***********************************************************************/

void quasi88_touch_as_mouse(int start)
{
	if (start) {
		q8tk_event_touch_start();
	} else {
		q8tk_event_touch_stop();
	}
}

/***********************************************************************
 *
 ***********************************************************************/

/* タッチキーボードの操作可否を返す */
int quasi88_cfg_can_touchkey(void)
{
	if (now_fullscreen) {
		return spec->touchkey_fullscreen;
	} else {
		return spec->touchkey_window;
	}
}

void quasi88_notify_touchkey(int request)
{
	if (spec->touchkey_notify) {
		(spec->touchkey_notify)(request);
	}
}



/***********************************************************************
 * システムイベント処理 EXPOSE / FOCUS-IN / FOCUS-OUT
 ***********************************************************************/

/*======================================================================
 *
 *======================================================================*/
void quasi88_expose(void)
{
	set_screen_dirty_frame();			/* 全領域 更新 */

	frameskip_counter_reset();			/* 次回描画 */
}


/*======================================================================
 *
 *======================================================================*/
static int now_focus_in;

void quasi88_focus_in(void)
{
	now_focus_in = TRUE;

	if (quasi88_is_pause()) {

		pause_event_focus_in_when_pause();

	}
}

void quasi88_focus_out(void)
{
	now_focus_in = FALSE;

	now_mouse_show = TRUE;

	if (quasi88_is_exec()) {

		pause_event_focus_out_when_exec();

	}
}

int quasi88_has_focus(void)
{
	return now_focus_in;
}














/***********************************************************************
 * ステートロード／ステートセーブ
 ************************************************************************/

#define SID		"SCRN"

static	T_SUSPEND_W		suspend_screen_work[] = {
	{ TYPE_CHAR,		&vram_bg_palette.blue,	},
	{ TYPE_CHAR,		&vram_bg_palette.red,	},
	{ TYPE_CHAR,		&vram_bg_palette.green,	},

	{ TYPE_CHAR,		&vram_palette[0].blue,	},
	{ TYPE_CHAR,		&vram_palette[0].red,	},
	{ TYPE_CHAR,		&vram_palette[0].green,	},
	{ TYPE_CHAR,		&vram_palette[1].blue,	},
	{ TYPE_CHAR,		&vram_palette[1].red,	},
	{ TYPE_CHAR,		&vram_palette[1].green,	},
	{ TYPE_CHAR,		&vram_palette[2].blue,	},
	{ TYPE_CHAR,		&vram_palette[2].red,	},
	{ TYPE_CHAR,		&vram_palette[2].green,	},
	{ TYPE_CHAR,		&vram_palette[3].blue,	},
	{ TYPE_CHAR,		&vram_palette[3].red,	},
	{ TYPE_CHAR,		&vram_palette[3].green,	},
	{ TYPE_CHAR,		&vram_palette[4].blue,	},
	{ TYPE_CHAR,		&vram_palette[4].red,	},
	{ TYPE_CHAR,		&vram_palette[4].green,	},
	{ TYPE_CHAR,		&vram_palette[5].blue,	},
	{ TYPE_CHAR,		&vram_palette[5].red,	},
	{ TYPE_CHAR,		&vram_palette[5].green,	},
	{ TYPE_CHAR,		&vram_palette[6].blue,	},
	{ TYPE_CHAR,		&vram_palette[6].red,	},
	{ TYPE_CHAR,		&vram_palette[6].green,	},
	{ TYPE_CHAR,		&vram_palette[7].blue,	},
	{ TYPE_CHAR,		&vram_palette[7].red,	},
	{ TYPE_CHAR,		&vram_palette[7].green,	},

	{ TYPE_BYTE,		&sys_ctrl,				},
	{ TYPE_BYTE,		&grph_ctrl,				},
	{ TYPE_BYTE,		&grph_pile,				},

	{ TYPE_INT,			&frameskip_rate,		},
	{ TYPE_INT,			&monitor_analog,		},
	{ TYPE_INT,			&use_auto_skip,			},
	/*	{ TYPE_INT,		&frame_counter,			}, 初期値でも問題ないだろう */
	/*	{ TYPE_INT,		&blink_ctrl_cycle,		}, 初期値でも問題ないだろう */
	/*	{ TYPE_INT,		&blink_ctrl_counter,	}, 初期値でも問題ないだろう */

	{ TYPE_INT,			&use_interlace,			},
	{ TYPE_INT,			&use_half_interp,		},

	{ TYPE_END,			0,						},
};


int statesave_screen(void)
{
	if (statesave_table(SID, suspend_screen_work) == STATE_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int stateload_screen(void)
{
	if (stateload_table(SID, suspend_screen_work) == STATE_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}
