/************************************************************************/
/* QUASI88 --- PC-8801 emulator                                         */
/*      Copyright (c) 1998-2024 Showzoh Fukunaga                        */
/*      All rights reserved.                                            */
/*                                                                      */
/*        このソフトは SDL2 環境で動作する PC-8801 のエミュレータです。 */
/*                                                                      */
/*        このソフトの作成にあたり、Marat Fayzullin氏作の fMSX、        */
/*      Nicola Salmoria氏 (MAME/XMAME project) 作の MAME/XMAME、        */
/*      ゆみたろ氏作の PC6001V のソースを参考にさせてもらいました。     */
/*                                                                      */
/*      ＊注意＊                                                        */
/*        サウンドドライバは、MAME/XMAME のソースを流用しています。     */
/*      この部分のソースの著作権は、MAME/XMAME チームあるいはソースに   */
/*      記載してある著作者にあります。                                  */
/*        FM音源ジェネレータは、fmgen のソースを流用しています。        */
/*      この部分のソースの著作権は、 cisc氏 にあります。                */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "quasi88.h"
#include "fname.h"
#include "debug.h"

#include "pc88main.h"
#include "pc88sub.h"
#include "memory.h"

#include "emu.h"
#include "drive.h"
#include "event.h"
#include "keyboard.h"
#include "monitor.h"
#include "snddrv.h"
#include "wait.h"
#include "statusbar.h"
#include "toolbar.h"
#include "suspend.h"
#include "snapshot.h"
#include "screen.h"
#include "menu.h"
#include "pause.h"
#include "z80.h"
#include "intr.h"

#include "alarm.h"
#include "q8tk.h"


int verbose_level   = DEFAULT_VERBOSE;		/* 冗長レベル */
int verbose_proc    = FALSE;				/* 処理の進行状況の表示 */
int verbose_z80     = FALSE;				/* Z80処理エラーを表示 */
int verbose_io      = FALSE;				/* 未実装I/Oアクセス報告 */
int verbose_pio     = FALSE;				/* PIO の不正使用を表示 */
int verbose_fdc     = FALSE;				/* FDイメージ異常を報告 */
int verbose_wait    = FALSE;				/* ウエイト時の異常を報告 */
int verbose_suspend = FALSE;				/* サスペンド時の異常を報告 */
int verbose_snd     = FALSE;				/* サウンドのメッセージ */

static int stateload_or_skip_at_startup(void);
static void quasi88_stop_core(void);
static void status_message_at_startup(void);

#define VERBOSE_PROC_PRINTF(str)	if (verbose_proc) printf(str); fflush(NULL)
#define ERROR_PRINTF(str)			fprintf(stderr, str)

/***********************************************************************
 *
 *                      QUASI88 メイン関数
 *
 ************************************************************************/
void quasi88(void)
{
	quasi88_start();
	quasi88_main();
	quasi88_stop();
}

/* メイン処理の初期化 ===================================================== */
static int proc = 0;

void quasi88_start(void)
{
	stateload_init();                   /* ステートロード関連初期化 */
	drive_init();                       /* ディスク制御のワーク初期化 */
	/* ↑ これらは、ステートロード開始までに初期化しておくこと */

	/* タイマー初期化 */
	proc = 0;
	if (wait_vsync_init()) {

		alarm_init();
		q8tk_init();
		submenu_init();
		statusbar_init();

		/* エミュ用メモリ初期化 */
		proc = 1;
		if (memory_allocate()) {

			/* 起動時のステートロード処理 */
			proc = 2;
			if (stateload_or_skip_at_startup()) {

				status_message_at_startup();

				/* グラフィックシステム初期化 */
				proc = 3;
				if (screen_init()) {

					/* システムイベント初期化 (screen_init の後で！) */
					event_init();

					/* サウンドドライバ初期化 */
					proc = 4;
					if (xmame_sound_start()) {

						proc = 5;

						set_signal();						/* INTシグナルの処理を設定 */
						imagefile_all_open(resume_flag);	/* イメージファイルを全て開く */

						/* エミュ用ワークを順次初期化   */
						pc88main_init((resume_flag) ? INIT_STATELOAD : INIT_POWERON);
						pc88sub_init((resume_flag) ? INIT_STATELOAD : INIT_POWERON);

						key_record_playback_init();			/* キー入力記録/再生 初期化 */
						screen_snapshot_init();				/* スナップショット関連初期化 */

						debuglog_init();
						profiler_init();
						emu_breakpoint_init();

						alarm_reset();

						VERBOSE_PROC_PRINTF("Running QUASI88...\n");
						return;

					} else {
						ERROR_PRINTF("sound system initialize failed!\n");
					}
				} else {
					ERROR_PRINTF("graphic system initialize failed!\n");
				}
			} else {

			}
		} else {
			ERROR_PRINTF("memory allocate failed!\n");
		}
	} else {
		ERROR_PRINTF("timer initialize failed!\n");
	}

	quasi88_exit(-1);
}

/* メイン処理のメインループ =============================================== */

void quasi88_main(void)
{
	for (;;) {

		/* 終了の応答があるまで、繰り返し呼び続ける */

		if (quasi88_loop() == QUASI88_LOOP_EXIT) {
			break;
		}

	}

	/* quasi88_loop() は、 1フレーム (VSYNC 1周期分≒1/60秒) おきに、
	   QUASI88_LOOP_ONE を返してくる。
	   この戻り値を判断して、なんらかの処理を加えてもよい。

	   また、内部処理の事情で QUASI88_LOOP_BUSY を返してくることもあるが、
	   この場合は気にせずに、繰り返し呼び出しを続行すること */

}

/* メイン処理の後片付け =================================================== */

void quasi88_stop()
{
	VERBOSE_PROC_PRINTF("Shutting down.....\n");

	quasi88_stop_core();
}

static void quasi88_stop_core(void)
{
	if (proc >= 1) {
		if (proc >= 2) {
			if (proc >= 3) {
				if (proc >= 4) {
					if (proc >= 5) {
						profiler_exit();
						debuglog_exit();
						screen_snapshot_exit();
						key_record_playback_exit();
						pc88main_term();
						pc88sub_term();
						imagefile_all_close();
						xmame_sound_stop();
					}
					/* サウンド初期化でNGの場合、ここから↓ */
					event_exit();
					screen_exit();
				}
				/* グラフィック初期化でNGの場合、ここから↓ */
			}
			/* ステートロード処理でNGの場合、ここから↓ */
			memory_free();
		}
		/* メモリ初期化でNGの場合、ここから↓ */
		q8tk_exit();
		alarm_exit();
		wait_vsync_exit();
	}
	/* タイマーの初期化でNGの場合、ここから↓ */


	/* この関数を続けて呼んでも問題無いように、クリアしておく */
	proc = 0;
}


/***********************************************************************
 * QUASI88 途中終了処理関数
 *      exit() の代わりに呼ぼう。
 ************************************************************************/

#define MAX_ATEXIT		(32)
static void (*exit_function[MAX_ATEXIT])(void);

/*
 * 関数を最大 MAX_ATEXIT 個、登録できる。ここで登録した関数は、
 * quasi88_exit() を呼び出した時に、登録した順と逆順で、呼び出される。
 */
void quasi88_atexit(void (*function)(void))
{
	int i;
	for (i = 0; i < MAX_ATEXIT; i++) {
		if (exit_function[i] == NULL) {
			exit_function[i] = function;
			return;
		}
	}
	printf("quasi88_atexit: out of array\n");
	quasi88_exit(-1);
}

/*
 * quasi88 を強制終了する。
 * quasi88_atexit() で登録した関数を呼び出した後に、 exit() する
 */
void quasi88_exit(int status)
{
	int i;

	quasi88_stop_core();

	for (i = MAX_ATEXIT - 1; i >= 0; i--) {
		if (exit_function[i]) {
			(*exit_function[i])();
			exit_function[i] = NULL;
		}
	}

	exit(status);
}





/***********************************************************************
 * QUASI88メインループ制御
 *      QUASI88_LOOP_EXIT が返るまで、無限に呼び出すこと。
 * 戻り値
 *      QUASI88_LOOP_EXIT … 終了時
 *      QUASI88_LOOP_ONE  … 1フレーム経過時 (ウェイトが正確ならば約1/60秒周期)
 *      QUASI88_LOOP_BUSY … 上記以外の、なんらかのタイミング
 ************************************************************************/

#define EXEC			0x00
#define MONITOR			0x10
#define MENU			0x20
#define QUIT			0x30
#define MAJOR_MODE		0xf0
#define MINOR_MODE		0x0f

int quasi88_event_flags = EVENT_MODE_CHANGED;
static int mode         = EXEC;				/* 現在のモード */
static int next_mode    = EXEC;				/* モード切替要求時の、次モード */

int quasi88_loop(void)
{
	static int init_done = FALSE;
	int do_wait = FALSE;
	int major_mode = mode & MAJOR_MODE;

	/* イニシャル処理 ================================================= */
	if (init_done == FALSE) {
		profiler_lapse(PROF_LAPSE_RESET);

		/* モード変更時は、必ずイニシャルが必要。モード変更フラグをクリア */
		quasi88_event_flags &= ~EVENT_MODE_CHANGED;
		mode = next_mode;
		major_mode = mode & MAJOR_MODE;

		/* 例外的なモード変更時の処理 */
		switch (mode) {
#ifndef USE_MONITOR
		case MONITOR:	/* ありえないけど、念のため */
			mode = MENU | MENU_MODE_PAUSE;
			major_mode = mode & MAJOR_MODE;
			break;
#endif
		case QUIT:		/* QUIT なら、メインループ終了 */
			return QUASI88_LOOP_EXIT;
		}

		/* モード別イニシャル処理 */
		if (major_mode == EXEC) {
			xmame_sound_resume();
		} else                    {
			xmame_sound_suspend();
		}

		quasi88_set_status();

		screen_switch(FALSE);
		event_switch();
		keyboard_switch();

		switch (major_mode) {
		case EXEC:
			emu_init();
			submenu_controll(CTRL_MODE_EXEC);
			break;

		case MENU:
			menu_init();
			if (quasi88_is_pause()) {
				submenu_controll(CTRL_MODE_MENU_PAUSE);
			} else if (quasi88_is_askreset()) {
				submenu_controll(CTRL_MODE_MENU_ASKRESET);
			} else if (quasi88_is_askspeedup()) {
				submenu_controll(CTRL_MODE_MENU_ASKSPEEDUP);
			} else if (quasi88_is_askdiskchange()) {
				submenu_controll(CTRL_MODE_MENU_ASKDISKCHANGE);
			} else if (quasi88_is_askquit()) {
				submenu_controll(CTRL_MODE_MENU_ASKQUIT);
			} else {
				submenu_controll(CTRL_MODE_MENU_FULLMENU);
			}
			break;

#ifdef USE_MONITOR
		case MONITOR:
			monitor_init();
			submenu_controll(CTRL_MODE_MONITOR);
			break;
#endif
		}

		wait_vsync_switch();

		/* イニシャル処理完了 */
		init_done = TRUE;
	}


	/* メイン処理 ================================================= */

	/* イベント処理 */
	event_update();

	/* アラーム処理 */
	alarm_update();

	/* Q8TK 1フレーム分処理 */
	q8tk_one_frame();


	switch (major_mode) {
	case EXEC:
		profiler_lapse(PROF_LAPSE_RESET);
		emu_main();
		break;
#ifdef  USE_MONITOR
	case MONITOR:
		monitor_main();
		break;
#endif
	case MENU:
		menu_main();
		break;
	}

	{
		profiler_lapse(PROF_LAPSE_SND);

		/* サウンド出力 */
		xmame_sound_update();

		profiler_lapse(PROF_LAPSE_AUDIO);

		/* サウンド出力 その2 */
		xmame_update_video_and_audio();
	}

	/* モード変更が発生していたら、再度イニシャル必要 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {
		init_done = FALSE;
	}

	/* 描画タイミングならばここで描画。その後ウェイトへ */
	if (quasi88_event_flags & EVENT_FRAME_UPDATE) {
		quasi88_event_flags &= ~EVENT_FRAME_UPDATE;
		statusbar_update();
		screen_update();
		do_wait = TRUE;
	} else {
		do_wait = FALSE;
	}

	/* ただし、モニター遷移時や終了時は、ウェイトしない */
	if (quasi88_event_flags & (EVENT_DEBUG | EVENT_QUIT)) {
		do_wait = FALSE;
		init_done = FALSE;
	}


	/* ウェイト処理 ================================================= */
	if (do_wait) {
		int stat = WAIT_JUST;

		profiler_lapse(PROF_LAPSE_IDLE);
		if ((major_mode == EXEC) && (no_wait)) {
			/* ウェイトなし */
			/* EMPTY */;
		} else {
			stat = wait_vsync_update();
		}


		/* WAIT_YET はありえないが、念のためガード */
		if (stat == WAIT_YET) {
			return QUASI88_LOOP_BUSY;
		}


		/* ウェイト時間を元に、フレームスキップの有無を決定 */
		if (major_mode == EXEC) {
			frameskip_check((stat == WAIT_JUST) ? TRUE : FALSE);
		}
	}


	return QUASI88_LOOP_ONE;
}



/*======================================================================
 * QUASI88 のモード制御
 *======================================================================*/
/* QUASI88のモードを設定する */
static void set_mode(int newmode)
{
	if (mode != newmode) {

		/* メニューから他モードの切替はQ8TK の終了が必須 */
		if ((mode & MAJOR_MODE) == MENU) {
			q8tk_event_quit();
		}

		next_mode = newmode;
		quasi88_event_flags |= EVENT_MODE_CHANGED;
		CPU_BREAKOFF();
	}
}

/* QUASI88のモードを切り替える */
void quasi88_exec(void)
{
	set_mode(EXEC);
	set_emu_exec_mode(GO);
}

void quasi88_exec_step(void)
{
	set_mode(EXEC);
	set_emu_exec_mode(STEP);
}

void quasi88_exec_trace(void)
{
	set_mode(EXEC);
	set_emu_exec_mode(TRACE);
}

void quasi88_exec_trace_change(void)
{
	set_mode(EXEC);
	set_emu_exec_mode(TRACE_CHANGE);
}

void quasi88_menu(void)
{
	set_mode(MENU | MENU_MODE_FULLMENU);
}

void quasi88_pause(void)
{
	set_mode(MENU | MENU_MODE_PAUSE);
}

void quasi88_askreset(void)
{
	set_mode(MENU | MENU_MODE_ASKRESET);
}

void quasi88_askspeedup(void)
{
	set_mode(MENU | MENU_MODE_ASKSPEEDUP);
}

void quasi88_askdiskchange(void)
{
	set_mode(MENU | MENU_MODE_ASKDISKCHANGE);
}

void quasi88_askquit(void)
{
	set_mode(MENU | MENU_MODE_ASKQUIT);
}

void quasi88_monitor(void)
{
#ifdef  USE_MONITOR
	set_mode(MONITOR);
#else
	set_mode(MENU | MENU_MODE_PAUSE);
#endif
}

void quasi88_debug(void)
{
#ifdef  USE_MONITOR
	set_mode(MONITOR);
	quasi88_event_flags |= EVENT_DEBUG;
#else
	set_mode(MENU | MENU_MODE_PAUSE);
#endif
}

void quasi88_quit(void)
{
	set_mode(QUIT);
	quasi88_event_flags |= EVENT_QUIT;
}

/* QUASI88のモードを取得する */
int quasi88_is_exec(void)
{
	return ((mode & MAJOR_MODE) == EXEC) ? TRUE : FALSE;
}
int quasi88_is_monitor(void)
{
	return ((mode & MAJOR_MODE) == MONITOR) ? TRUE : FALSE;
}
int quasi88_is_menu_mode(void)
{
	return ((mode & MAJOR_MODE) == MENU) ? TRUE : FALSE;
}
int quasi88_is_fullmenu(void)
{
	return (mode == (MENU | MENU_MODE_FULLMENU)) ? TRUE : FALSE;
}
int quasi88_is_pause(void)
{
	return (mode == (MENU | MENU_MODE_PAUSE)) ? TRUE : FALSE;
}
int quasi88_is_askreset(void)
{
	return (mode == (MENU | MENU_MODE_ASKRESET)) ? TRUE : FALSE;
}
int quasi88_is_askspeedup(void)
{
	return (mode == (MENU | MENU_MODE_ASKSPEEDUP)) ? TRUE : FALSE;
}
int quasi88_is_askdiskchange(void)
{
	return (mode == (MENU | MENU_MODE_ASKDISKCHANGE)) ? TRUE : FALSE;
}
int quasi88_is_askquit(void)
{
	return (mode == (MENU | MENU_MODE_ASKQUIT)) ? TRUE : FALSE;
}
int quasi88_get_menu_mode(void)
{
	return (mode & MINOR_MODE);
}





/***********************************************************************
 * 適切な位置に移動せよ
 ************************************************************************/
void wait_vsync_switch(void)
{
	long dt;

	/* dt < 1000000us (1sec) でないとダメ */
	if (quasi88_is_exec()) {
		dt = (long)((1000000.0 / (CONST_VSYNC_FREQ * wait_rate / 100)));
		wait_vsync_setup(dt, wait_by_sleep);
	} else {
		dt = (long)(1000000.0 / CONST_VSYNC_FREQ);
		wait_vsync_setup(dt, TRUE);
	}
}


static int stateload_or_skip_at_startup(void)
{
	if (resume_flag) {
		if (stateload() == FALSE) {
			fprintf(stderr, "stateload: Failed ! (filename = %s)\n",
					filename_get_state());
			return FALSE;
		}
		VERBOSE_PROC_PRINTF("Stateload...OK\n");
	}
	return TRUE;
}


void quasi88_set_status(void)
{
	if (quasi88_is_menu_mode()) {
		menu_status();
	} else {
		emu_status();
	}
}


static void status_message_at_startup(void)
{
	if (resume_flag == 0) {
		emu_status_message_set(STATUS_INFO, "<F12> key to MENU", 0);
	} else {
		emu_status_message_set(STATUS_INFO,
							   "State-Load Successful",
							   "ステートロードしました");
	}
}
