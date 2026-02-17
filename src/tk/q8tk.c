/************************************************************************/
/*                                                                      */
/* QUASI88 メニュー用 Tool Kit                                          */
/*                                                                      */
/*      GTK+ の API を真似て作りました。ややこしすぎて、ドキュメントは  */
/*      書けません………。                                              */
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quasi88.h"

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"
#include "alarm.h"

#include "event.h"
#include "wait.h"
#include "menu.h"
#include "screen.h"


static void setup_default_kanji_code(void);

/***********************************************************************
 * 初期化と終了
 ************************************************************************/

static	int						q8tk_disp_init_flag;
static	int						q8tk_main_loop_flag;

#define set_main_loop_flag(f)	q8tk_main_loop_flag = (f)
#define get_main_loop_flag()	q8tk_main_loop_flag


/*--------------------------------------------------------------
 * デバッグ用
 *--------------------------------------------------------------*/
void q8tk_debug(int num)
{
	int i, cnt[3] = { 0, 0, 0, };
	for (i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i]) {
			cnt[0] ++;
		}
	}
	for (i = 0; i < MAX_LIST; i++) {
		if (list_table[i]) {
			cnt[1] ++;
		}
	}
	for (i = 0; i < BITMAP_TABLE_SIZE; i++) {
		if (bitmap_table[i]) {
			cnt[2] ++;
		}
	}
	printf("%d: W %d, L %d, B %d\n", num, cnt[0], cnt[1], cnt[2]);
}

/*--------------------------------------------------------------
 * 初期化
 *--------------------------------------------------------------*/
void q8tk_init(void)
{
	int i;

#if 0
	printf("Q8Adjust   %d\n",(int)sizeof(Q8Adjust));
	printf("Q8tkWidget %d\n",(int)sizeof(Q8tkWidget));
	printf(" union     %d\n",(int)sizeof(((Q8tkWidget *)0)->stat));
#endif

	MAX_WIDGET = 0;
	widget_table = NULL;

	MAX_LIST = 0;
	list_table = NULL;

	window_occupy = 0;

	for (i = 0; i < TOTAL_WINDOW_LAYER; i++) {
		window_layer[i] = NULL;
		focus_widget[i] = NULL;
	}
	window_layer_level = 0;

	set_main_loop_flag(FALSE);
	set_construct_flag(FALSE);

	set_drag_widget(NULL);
	widget_focus_list_init();
	widget_scrollin_init();


	q8gr_init();

	widget_bitmap_table_init();

	/* Q8TK 文字コードセット */
	setup_default_kanji_code();

	q8tk_now_shift_on = FALSE;

	q8tk_disp_init_flag = TRUE;
}

/*--------------------------------------------------------------
 * メインモードの開始と終了
 *--------------------------------------------------------------*/
void q8tk_main_start(void)
{
	int i;

	window_occupy = 1;

	for (i = 0; i < MAX_WINDOW_LAYER; i++) {
		window_layer[i] = NULL;
		focus_widget[i] = NULL;
	}
	window_layer_level = 0;

	set_main_loop_flag(TRUE);
	set_construct_flag(FALSE);

	set_drag_widget(NULL);
	widget_focus_list_init();
	widget_scrollin_init();

	q8gr_init();

	q8tk_now_shift_on = FALSE;

	q8tk_disp_init_flag = TRUE;
}

void q8tk_main_stop(void)
{
	int i;

	set_drag_widget(NULL);
	widget_focus_list_reset();
	widget_scrollin_init();

	for (i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i] &&
			(widget_table[i])->occupy == window_occupy) {
			widget_destroy_core(widget_table[i], 0);
		}
	}
	for (i = 0; i < MAX_LIST; i++) {
		if (list_table[i] &&
			(list_table[i])->occupy == window_occupy) {
			free(list_table[i]);
			list_table[i] = NULL;
		}
	}
	for (i = 0; i < MAX_WINDOW_LAYER; i++) {
		window_layer[i] = NULL;
		focus_widget[i] = NULL;
	}
	window_layer_level = 0;

	q8gr_init();


	window_occupy = 0;
}

/*--------------------------------------------------------------
 * 終了
 *--------------------------------------------------------------*/
void q8tk_exit(void)
{
	int i;
	for (i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i]) {
			widget_destroy_core(widget_table[i], 0);
		}
	}
	if (widget_table) {
		free(widget_table);
	}
	widget_table = NULL;
	MAX_WIDGET = 0;

	for (i = 0; i < MAX_LIST; i++) {
		if (list_table[i]) {
			free(list_table[i]);
		}
	}
	if (list_table) {
		free(list_table);
	}
	list_table = NULL;
	MAX_LIST = 0;

	widget_bitmap_table_term();
}



/***********************************************************************
 * メイン
 ************************************************************************/
/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
static	int		cursor_exist = 0;
static	int		cursor_timer = 0;

#define Q8GR_CURSOR_BLINK				(30)
#define Q8GR_CURSOR_CYCLE				(60)

#define Q8GR_MOUSE_AUTO_REPEAT			(6)


void q8gr_set_cursor_exist(int exist_flag)
{
	cursor_exist = exist_flag;
}
int q8gr_get_cursor_exist(void)
{
	return cursor_exist;
}
int q8gr_get_cursor_blink(void)
{
	return (cursor_timer < Q8GR_CURSOR_BLINK) ? TRUE : FALSE;
}
void q8gr_set_cursor_blink(void)
{
	cursor_timer = 0;
}

/*--------------------------------------------------------------
 * q8tk_one_frame
 * 戻り値が、真なら、ウェイト後に何度でも呼び出す。偽なら、終了。
 *--------------------------------------------------------------*/
int q8tk_one_frame(void)
{
	if (alarm_is_lock() == FALSE) {

		if (cursor_exist) {
			/* カーソル表示中なら点滅タイマ更新 */
			cursor_timer ++;
			if (cursor_timer > Q8GR_CURSOR_CYCLE) {
				cursor_timer = 0;
			}

			/* カーソル点滅切り替わりのタイミングをチェック */
			if (cursor_timer == 0 ||
				cursor_timer == Q8GR_CURSOR_BLINK) {
				set_construct_flag(TRUE);
			}

		} else {
			cursor_timer = 0;
		}
	}

	/* 画面構成変更時は、描画 */
	if (get_construct_flag()) {

		widget_construct();
		set_construct_flag(FALSE);

		if (q8tk_disp_init_flag) {
			set_screen_dirty_palette();
			q8tk_disp_init_flag = FALSE;
		}
		set_screen_dirty_menu();
		set_screen_dirty_tool_part();
		set_screen_dirty_stat_part();

#if 0
		q8tk_debug(9999);
#endif
	}


	if (get_main_loop_flag()) {

		return TRUE;

	} else {

		return FALSE;
	}
}

/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
void q8tk_main_quit(void)
{
	set_main_loop_flag(FALSE);
}

/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
void q8tk_set_solid(int is_solid)
{
	q8gr_set_solid((is_solid) ? TRUE : FALSE);
}

/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
/* このファイルで使われている文字コードをチェックする・・・ まずいやり方 */
static const char *menu_kanji_code = "漢字";
static const char menu_kanji_code_euc[]  = { 0xb4, 0xc1, 0xbb, 0xfa, 0x00, };
static const char menu_kanji_code_sjis[] = { 0x8a, 0xbf, 0x8e, 0x9a, 0x00, };
static const char menu_kanji_code_utf8[] = { 0xe6, 0xbc, 0xa2, 0xe5, 0xad, 0x97, 0x00, };

static void setup_default_kanji_code(void)
{
	if (strcmp(menu_kanji_code, menu_kanji_code_euc) == 0) {
		q8tk_set_kanjicode(Q8TK_KANJI_EUC);

	} else if (strcmp(menu_kanji_code, menu_kanji_code_sjis) == 0) {
		q8tk_set_kanjicode(Q8TK_KANJI_SJIS);

	} else if (strcmp(menu_kanji_code, menu_kanji_code_utf8) == 0) {
		q8tk_set_kanjicode(Q8TK_KANJI_UTF8);

	} else {
		q8tk_set_kanjicode(Q8TK_KANJI_ANK);
	}
}

