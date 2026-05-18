/***********************************************************************
 *
 * モニターモード
 *
 ************************************************************************/

#include <stdio.h>
#include <signal.h>

#include "quasi88.h"
#include "monitor.h"
#include "mon-cmd.h"

#include "pc88cpu.h"
#include "graph.h"

#include "emu.h"



/***********************************************************************
 * SIGINT発生時 (Ctrl-C) 、モニターモードへ遷移するように設定
 *      起動時に -debug オプション指定時のみ。未指定時は終了する。
 ************************************************************************/

int debug_mode = FALSE;							/* デバッグ機能(モニター) */

char alt_char = 'X';							/* 代替文字 */



/* モニターモード以外の時に SIGINT(Ctrl-C)を受け取ったらモニターモードに移行 */

static void sigint_handler(int dummy)
{
	quasi88_monitor();
	signal(SIGINT, sigint_handler);
}

/* SIGTERM を受けとったら、終了する */

static void sigterm_handler(int dummy)
{
	quasi88_quit();
	z80main_cpu.icount = 0;
	z80sub_cpu.icount = 0;
}

/* 割り込み設定 */

void set_signal(void)
{
	if (debug_mode) {
		signal(SIGINT,  sigint_handler);
		signal(SIGTERM, sigterm_handler);
	} else {
		signal(SIGINT,  sigterm_handler);
		signal(SIGTERM, sigterm_handler);
	}
}


/***************************************************************
 * モニターモード メイン処理
 ****************************************************************/

#ifdef USE_MONITOR

void monitor_show_cpu_status(int is_main_cpu)
{
	if (is_main_cpu) {
		z80_status_show(&z80main_cpu, mon.reg.line, mon.reg.style);
	} else {
		z80_status_show(&z80sub_cpu,  mon.reg.line, mon.reg.style);
	}
}

static void monitor_firsttime(void)
{
	static int enter_monitor_mode_first = TRUE;

	if (enter_monitor_mode_first) {
		enter_monitor_mode_first = FALSE;

		/* 一番最初にモニターモードに入った時は、メッセージを表示 */
		printf("\n"
			   "*******************************************************************************\n"
			   "* QUASI88   - monitor mode -                                                  *\n"
			   "*                                                                             *\n"
			   "*    Enter  go   or g  : Return to emulator                                   *\n"
			   "*    Enter  menu or m  : Enter menu mode                                      *\n"
			   "*    Enter  quit or q  : Quit QUASI88                                         *\n"
			   "*                                                                             *\n"
			   "*    Enter  help or ?  : Display help                                         *\n"
			   "*******************************************************************************\n"
			   "\n");

		mon_var_init();

#ifdef USE_GNU_READLINE
		initialize_readline();
#endif
	}
}


void monitor_init(void)
{
	int i;

	monitor_firsttime();

	for (i = 0; monitor_cmd[i].cmd; i++) {
		if (monitor_cmd[i].init) {
			(monitor_cmd[i].init)();
		}
	}

	if (quasi88_event_flags & EVENT_DEBUG) {
		quasi88_event_flags &= ~EVENT_DEBUG;

		z80_system_break_clear(&z80main_cpu);
		z80_system_break_clear(&z80sub_cpu);

		/* ブレークした場合は、レジスタ表示 */
		switch (cpu_timing) {
		case -1:
		case 0:
			if (select_main_cpu) {
				monitor_show_cpu_status(TRUE);
			} else {
				monitor_show_cpu_status(FALSE);
			}
			break;

		case 1:
			monitor_show_cpu_status(TRUE);
			if (dual_cpu_count) {
				monitor_show_cpu_status(FALSE);
			}
			break;

		case 2:
			if (select_main_cpu) {
				monitor_show_cpu_status(TRUE);
			} else {
				monitor_show_cpu_status(FALSE);
			}
			break;
		}
	}

	fflush(NULL);
}



void monitor_main(void)
{
	screen_update_immidiate();

	/* モードが切り替わるまで、処理を続行 */
	while ((quasi88_event_flags & EVENT_MODE_CHANGED) == 0) {
		mon_command();
	}
}



#endif /* USE_MONITOR */
