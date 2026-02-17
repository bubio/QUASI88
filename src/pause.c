/***********************************************************************
 *
 * ポーズ (一時停止) モード
 *
 ************************************************************************/

/*  変数 pause_by_focus_out により処理が変わる
 *  ・pause_by_focus_out == 0 の時
 *          ESCが押されると解除。   画面中央に PAUSEと表示
 *  ・pause_by_focus_out != 0 の時
 *          X のマウスが画面内に入ると解除
 */

#include <stdio.h>

#include "quasi88.h"
#include "pause.h"

#include "emu.h"
#include "initval.h"
#include "statusbar.h"
#include "screen.h"
#include "wait.h"
#include "event.h"
#include "q8tk.h"
#include "toolbar.h"


/***********************************************************************
 * 一時停止
 ************************************************************************/
int need_focus = FALSE;						/* フォーカスアウト停止あり */

static int pause_by_focus_out = FALSE;

/*
 * エミュ処理中に、フォーカスが無くなった (-focus指定時は、ポーズ開始)
 */
void pause_event_focus_out_when_exec(void)
{
	if (need_focus) {
		/* -focus 指定時は ここで PAUSE する */
		pause_by_focus_out = TRUE;
		quasi88_pause();
	}
}

/*
 * ポーズ中に、フォーカスを得た
 */
void pause_event_focus_in_when_pause(void)
{
	if (pause_by_focus_out) {
		quasi88_exec();
	}
}

/*
 * ポーズ中に、ポーズ終了のキー(ESCキー)押下検知した
 */
void pause_event_key_on_esc(void)
{
	quasi88_exec();
}

/*
 * ポーズ中に、メニュー開始のキー押下検知した
 */
void pause_event_key_on_menu(void)
{
	quasi88_menu();
}



/*
 *
 */
void ui_pause_init(void)
{
	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	pause_top();
}

/*
 *
 */
void ui_pause_main(void)
{
	/* 一時停止を抜けたら、ワーク再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		pause_by_focus_out = FALSE;

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}



/***********************************************************************
 * リセット
 ************************************************************************/
/*
 *
 */
void ui_askreset_init(void)
{
	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	reset_top();
}

/*
 *
 */
void ui_askreset_main(void)
{
	/* 一時停止を抜けたら、ワーク再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}



/***********************************************************************
 * スピードアップ比率
 ************************************************************************/
/*
 *
 */
void ui_askspeedup_init(void)
{
	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	speedup_top();
}

/*
 *
 */
void ui_askspeedup_main(void)
{
	/* 一時停止を抜けたら、ワーク再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}



/***********************************************************************
 * ディスクイメージファイル変更
 ************************************************************************/
/*
 *
 */
void ui_askdiskchange_init(void)
{
	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	diskchange_top();
}

/*
 *
 */
void ui_askdiskchange_main(void)
{
	/* 一時停止を抜けたら、ワーク再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}



/***********************************************************************
 * 終了
 ************************************************************************/
/*
 *
 */
void ui_askquit_init(void)
{
	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	quit_top();
}

/*
 *
 */
void ui_askquit_main(void)
{
	/* 一時停止を抜けたら、ワーク再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}
