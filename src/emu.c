/***********************************************************************
 *
 * エミュモード
 *
 ************************************************************************/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "debug.h"
#include "emu.h"

#include "pc88main.h"
#include "pc88cpu.h"

#include "screen.h"
#include "keyboard.h"
#include "intr.h"
#include "event.h"
#include "menu.h"
#include "monitor.h"
#include "pause.h"
#include "wait.h"
#include "suspend.h"
#include "statusbar.h"
#include "toolbar.h"
#include "graph.h"
#include "snddrv.h"




break_drive_t break_point_fdc[NR_BPF];			/* FDC ブレークポイント */


int cpu_timing = DEFAULT_CPU;					/* SUB-CPU 駆動方式 */

int select_main_cpu = TRUE;						/* -cpu 0 実行するCPU
												 * 真なら MAIN CPUを実行 */

int dual_cpu_count = 0;							/* -cpu 1 同時処理STEP数 */
int CPU_1_COUNT    = 4000;						/* その、初期値 */

int cpu_slice_us = 5;							/* -cpu 2 処理時分割(us)
												 * 10>でSILPHEEDが動かん */

int trace_counter = 1;							/* TRACE 時のカウンタ */



static int main_state = 0;
static int sub_state  = 0;
#define JACKUP		(256)


static const char *get_basicmode_message(void);
static const char *get_statistics_message(void);

static int emu_mode_execute = GO;
static int emu_rest_step;

void set_emu_exec_mode(int mode)
{
	emu_mode_execute = mode;
}

/***********************************************************************
 * エミュレート処理の初期化関連
 ************************************************************************/

void emu_reset(void)
{
	select_main_cpu = TRUE;
	dual_cpu_count  = 0;

	main_state = 0;
	sub_state  = 0;
}


/* ブレークポイントのワーク初期化 (モニターモード用) */
void emu_breakpoint_init(void)
{
	memset(&break_point_fdc, 0, sizeof(break_point_fdc));
}




/***********************************************************************
 * CPU実行処理 (EXEC) の制御
 *      -cpu <n> に応じて、動作を変える。
 *
 *      STEP  時は、1step だけ実行する。
 *      TRACE 時は、指定回数分、1step 実行する。
 *
 *      ブレークポイント指定時は、1step実行の度に PC がブレークポイントに
 *      達したかどうかを確認する。
 *
 ************************************************************************/

#define INFINITY_STEP	(0)
#define ONLY_1STEP		(1)

/*---------------------------------------------------------------------------*/

static int infinity;


void emu_init(void)
{
	int target_step;					/* この step数に達するまで実行する */

	/*xmame_sound_update();*/
	xmame_update_video_and_audio();
	event_update();
	/*keyboard_update();*/



	/*set_screen_dirty_all();*/
	/*set_screen_dirty_palette();*/


	/* GO/TRACE/STEP/CHANGE に応じて処理の繰り返し回数を決定 */

	switch (emu_mode_execute) {
	default:
	case GO:
		/* 無限に実行 */
		target_step = 0;
		infinity    = INFINITY_STEP;
		break;

	case TRACE:
		/* 指定ステップ数実行 */
		target_step = trace_counter;
		infinity    = ONLY_1STEP;
		break;

	case STEP:
		/* 1ステップ実行 */
		target_step = 1;
		infinity    = ONLY_1STEP;
		break;

	case TRACE_CHANGE:
		/* 無限に実行 */
		target_step = 0;
		infinity    = INFINITY_STEP;
		break;
	}


	/* 実行する残りステップ数。
	 *  TRACE / STEP の時は、指定されたステップ数。
	 *  GO / TRACE_CHANGE なら 無限なので、 0。
	 *      なお、途中でメニューに遷移した場合、強制的に 1 がセットされる。
	 *      これにより無限に処理する場合でも、ループを抜けるようになる。
	 */
	emu_rest_step = target_step;
}


void emu_main(void)
{
	int wk;

	profiler_lapse(PROF_LAPSE_INPUT);

	keyboard_update();


	profiler_lapse(PROF_LAPSE_CPU);

	switch (emu_mode_execute) {

	/*------------------------------------------------------------------------*/
	case GO:	/* ひたすら実行する */
	case TRACE:	/* 指定したステップ、実行する */
	case STEP:	/* 1ステップだけ、実行する */

		for (;;) {

			switch (cpu_timing) {

			case -1:
			case 0:
				/* select_main_cpu で指定されたほうのCPUを無限実行 */
				if (select_main_cpu) {
					z80_emu(&z80main_cpu, infinity);
				} else {
					z80_emu(&z80sub_cpu,  infinity);
				}
				break;

			case 1:
				/* dual_cpu_count==0 ならメインCPUを無限実行、
				 *               !=0 ならメインサブを交互実行 */
				if (dual_cpu_count == 0) {
					z80_emu(&z80main_cpu, infinity);
				} else {
					z80_emu(&z80main_cpu, ONLY_1STEP);
					z80_emu(&z80sub_cpu,  ONLY_1STEP);
					dual_cpu_count --;
				}
				break;

			case 2:
				/* メインCPU、サブCPUを交互に 5us ずつ実行 */
				if ((main_state <= 0) && (sub_state <= 0)) {
					main_state += (int) ((cpu_clock_mhz * cpu_slice_us) * JACKUP);
					sub_state  += (int) ((3.9936        * cpu_slice_us) * JACKUP);
				}

				if (infinity == INFINITY_STEP) {
					/* 通常時: 5us実行 (最低でも1ステップは実行) */
					if (main_state > 0) {
						wk = MAX(ONLY_1STEP, (main_state / JACKUP));
						main_state -= (z80_emu(&z80main_cpu, wk)) * JACKUP;
					}
					if (sub_state > 0) {
						wk = MAX(ONLY_1STEP, (sub_state / JACKUP));
						sub_state -= (z80_emu(&z80sub_cpu, wk)) * JACKUP;
					}
				} else {
					/* TRACE/STEP実行時: 選択CPU側は1ステップだけ実行 */
					if (main_state > 0) {
						if (select_main_cpu) {
							wk = ONLY_1STEP;
						} else {
							wk = MAX(ONLY_1STEP, (main_state / JACKUP));
						}
						main_state -= (z80_emu(&z80main_cpu, wk)) * JACKUP;
					}
					if (sub_state > 0) {
						if (! select_main_cpu) {
							wk = ONLY_1STEP;
						} else {
							wk = MAX(ONLY_1STEP, (sub_state / JACKUP));
						}
						sub_state -= (z80_emu(&z80sub_cpu, wk)) * JACKUP;
					}
				}
				break;
			}

			/* TRACE/STEP実行時、規定ステップ実行完了したら、モニターに遷移する */
			if (emu_rest_step) {
				if (-- emu_rest_step <= 0) {
					quasi88_debug();
				}
			}

			/* サウンド出力タイミングであれば、処理 */
			if (quasi88_event_flags & EVENT_AUDIO_UPDATE) {
				quasi88_event_flags &= ~EVENT_AUDIO_UPDATE;

				;
			}

			/* ビデオ出力タイミングであれば、CPU処理は一旦中止。上位に抜ける */
			if (quasi88_event_flags & EVENT_FRAME_UPDATE) {
				return;
			}

			/* モニター遷移時や終了時は、 CPU処理は一旦中止。上位に抜ける */
			if (quasi88_event_flags & (EVENT_DEBUG | EVENT_QUIT)) {
				return;
			}

			/* モード切替が発生しても、上位には抜けない。ビデオ出力まで待つ */
			/* (抜けると、 エミュ → 描画 → ウェイト の流れが崩れるので…) */
		}
		break;

	/*------------------------------------------------------------------------*/
	case TRACE_CHANGE: /* CPUが切り替わるまで処理をする */
		if (cpu_timing > 0) {
			quasi88_monitor();
			break;
		}

		wk = select_main_cpu;
		while (wk == select_main_cpu) {
			if (select_main_cpu) {
				z80_emu(&z80main_cpu, infinity);
			} else {
				z80_emu(&z80sub_cpu,  infinity);
			}

			if (emu_rest_step) {
				if (-- emu_rest_step <= 0) {
					quasi88_debug();
				}
			}

			if (quasi88_event_flags & EVENT_FRAME_UPDATE) {
				return;
			}
			if (quasi88_event_flags & (EVENT_DEBUG | EVENT_QUIT)) {
				return;
			}
		}
		quasi88_debug();
		break;
	}
}



/*----------------------------------------------------------------------
 *
 *----------------------------------------------------------------------*/
static const char *get_basicmode_message(void)
{
	static const char *str[] = {
		"N 4MHz  ",
		"V1S 4MHz",
		"V1H 4MHz",
		"V2 4MHz ",
		"N 8MHz  ",
		"V1S 8MHz",
		"V1H 8MHz",
		"V2 8MHz ",
	};
	int stat = 0;
	switch (boot_basic) {
	case BASIC_N:
		stat += 0;
		break;
	case BASIC_V1S:
		stat += 1;
		break;
	case BASIC_V1H:
		stat += 2;
		break;
	case BASIC_V2:
		stat += 3;
		break;
	}
	if (boot_clock_4mhz == FALSE) {
		stat += 4;
	}

	return str[stat];
}

static const char *get_statistics_message(void)
{
	static char buf[32];
	static int prev_drawn_count;
	static int prev_vsync_count;
	int now_drawn_count;
	int now_vsync_count;

	now_drawn_count = quasi88_info_draw_count();
	now_vsync_count = quasi88_info_vsync_count();

	if (quasi88_is_exec()) {
		sprintf(buf, " %2dfps (%2dHz)",
				now_drawn_count - prev_drawn_count,
				now_vsync_count - prev_vsync_count);
	} else {
		buf[0] = '\0';
	}

	prev_drawn_count = now_drawn_count;
	prev_vsync_count = now_vsync_count;

	return buf;
}

/* MENU -> EMU になったら、ステータスメッセージがデフォルトになるので、
 * その時に一時メッセージを表示したい場合の処理 */
static struct {
	int timeout;
	const char *ascii;
	const char *japanese;
} emu_stat_msg;
void emu_status_message_set(int timeout,
							const char *message_ascii,
							const char *message_japanese)
{
	if (timeout > 0) {
		emu_stat_msg.timeout = timeout;
		emu_stat_msg.ascii = message_ascii;
		emu_stat_msg.japanese = message_japanese;
	} else {
		emu_status_message_clear();
	}
}
void emu_status_message_clear(void)
{
	memset(&emu_stat_msg, 0, sizeof(emu_stat_msg));
}


void emu_status(void)
{
	if (show_statistics == FALSE) {
		statusbar_message(0, get_basicmode_message(), 0);
	} else {
		statusbar_message_cyclic(1000, get_statistics_message);
	}

	if (emu_stat_msg.timeout) {
		statusbar_message(emu_stat_msg.timeout,
						  emu_stat_msg.ascii, emu_stat_msg.japanese);
		emu_status_message_clear();
	}
}


int quasi88_now_subcpu_mode(void)
{
	return cpu_timing;
}

void quasi88_set_subcpu_mode(int mode)
{
	cpu_timing = mode;
	emu_reset();
	/* 他に再初期化すべきものはないのか？ */

	toolbar_controll(CTRL_CHG_SUBCPU);
}



/***********************************************************************
 * ステートロード／ステートセーブ
 ************************************************************************/

#define SID		"EMU "

static T_SUSPEND_W suspend_emu_work[] = {
	{ TYPE_INT,   &cpu_timing,            },
	{ TYPE_INT,   &select_main_cpu,       },
	{ TYPE_INT,   &dual_cpu_count,        },
	{ TYPE_INT,   &CPU_1_COUNT,           },
	{ TYPE_INT,   &cpu_slice_us,          },
	{ TYPE_INT,   &main_state,            },
	{ TYPE_INT,   &sub_state,             },

	{ TYPE_END,   0                       },
};


int statesave_emu(void)
{
	if (statesave_table(SID, suspend_emu_work) == STATE_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int stateload_emu(void)
{
	if (stateload_table(SID, suspend_emu_work) == STATE_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}
