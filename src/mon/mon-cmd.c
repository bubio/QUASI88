#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#ifdef USE_GNU_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


#ifdef USE_MONITOR

/***************************************************************
 * 命令の種類判定テーブル
 ****************************************************************/
t_monitor_cmd monitor_cmd[] = {
	{ "help",      "?", mon_help_help,      mon_exec_help,      0, },
	{ "menu",      "m", mon_help_menu,      mon_exec_menu,      0, },
	{ "quit",      "q", mon_help_quit,      mon_exec_quit,      0, },
	{ "go",        "g", mon_help_go,        mon_exec_go,        0, },
	{ "trace",     "t", mon_help_trace,     mon_exec_trace,     0, },
	{ "step",      "s", mon_help_step,      mon_exec_step,      0, },
	{ "next",      "n", mon_help_next,      mon_exec_next,      0, },
	{ "change",    0,   mon_help_change,    mon_exec_change,    0, },
	{ "return",    "ret",mon_help_return,    mon_exec_return,    0, },
	{ "break",     "b", mon_help_break,     mon_exec_break,     0, },
	{ "breakt",    "bt",mon_help_break,     mon_exec_breakt,    0, },
	{ "reset",     0,   mon_help_reset,     mon_exec_reset,     0, },
	{ "reg",       "r", mon_help_reg,       mon_exec_reg,       0, },
	{ "disasm",    "d", mon_help_disasm,    mon_exec_disasm,    mon_init_disasm,  },
	{ "fbreak",    0,   mon_help_fbreak,    mon_exec_fbreak,    0, },
	{ "read",      0,   mon_help_read,      mon_exec_read,      0, },
	{ "write",     0,   mon_help_write,     mon_exec_write,     0, },
	{ "dump",      0,   mon_help_dump,      mon_exec_dump,      mon_init_dump,    },
	{ "dumpext",   0,   mon_help_dumpext,   mon_exec_dumpext,   mon_init_dumpext, },
	{ "fill",      0,   mon_help_fill,      mon_exec_fill,      0, },
	{ "move",      0,   mon_help_move,      mon_exec_move,      0, },
	{ "search",    0,   mon_help_search,    mon_exec_search,    0, },
	{ "in",        0,   mon_help_in,        mon_exec_in,        0, },
	{ "out",       0,   mon_help_out,       mon_exec_out,       0, },
	{ "loadmem",   0,   mon_help_loadmem,   mon_exec_loadmem,   0, },
	{ "savemem",   0,   mon_help_savemem,   mon_exec_savemem,   0, },
	{ "text",      0,   mon_help_text,      mon_exec_text,      0, },
	{ "set",       0,   mon_help_set,       mon_exec_set,       0, },
	{ "show",      0,   mon_help_show,      mon_exec_show,      0, },
	{ "drive",     0,   mon_help_drive,     mon_exec_drive,     0, },
	{ "file",      0,   mon_help_file,      mon_exec_file,      0, },
	{ "tapeload",  0,   mon_help_tapeload,  mon_exec_tapeload,  0, },
	{ "tapesave",  0,   mon_help_tapesave,  mon_exec_tapesave,  0, },
	{ "printer",   0,   mon_help_printer,   mon_exec_printer,   0, },
	{ "serialin",  0,   mon_help_serialin,  mon_exec_serialin,  0, },
	{ "serialout", 0,   mon_help_serialout, mon_exec_serialout, 0, },
	{ "textscr",   0,   mon_help_textscr,   mon_exec_textscr,   0, },
	{ "loadbas",   0,   mon_help_loadbas,   mon_exec_loadbas,   0, },
	{ "savebas",   0,   mon_help_savebas,   mon_exec_savebas,   0, },
	{ "redraw",    0,   mon_help_redraw,    mon_exec_redraw,    0, },
	{ "resize",    0,   mon_help_resize,    mon_exec_resize,    0, },
	{ "statesave", 0,   mon_help_statesave, mon_exec_statesave, 0, },
	{ "stateload", 0,   mon_help_stateload, mon_exec_stateload, 0, },
	{ "snapshot",  0,   mon_help_snapshot,  mon_exec_snapshot,  0, },
	{ "loadfont",  0,   mon_help_loadfont,  mon_exec_loadfont,  0, },
	{ "savefont",  0,   mon_help_savefont,  mon_exec_savefont,  0, },
	{ "misc",      0,   mon_help_misc,      mon_exec_misc,      0, },
	{ 0,           0,   0,                  0,                  0, },
};



#ifdef USE_GNU_READLINE
/*--------------------------------------------------------------
 * readline
 *--------------------------------------------------------------*/

static char *command_generator(char *text, int state)
{
	static int count, len;
	char *name;

	/* この関数、最初は state=0 で呼び出されるらしいので、
	   その時に変数を初期化する。 */
	if (state == 0) {
		count = 0;
		len = strlen(text);
	}

	while (monitor_cmd[count].cmd) {
		/* コマンド名を検索 */

		name = monitor_cmd[count].cmd;
		count ++;

		if (strncmp(name, text, len) == 0) {
			char *p = malloc(strlen(name) + 1);
			if (p) {
				strcpy(p, name);
			}
			return p;
		}
	}

	return NULL;
}

static char **fileman_completion(char *text, int start, int end)
{
	char **matches = NULL;

	int i = 0;
	char c;
	/* "set " と入力された場合 */
	while ((c = rl_line_buffer[i])) {
		if (c == ' ' || c == '\t') {
			i++;
			continue;
		} else {
			break;
		}
	}
	if (strncmp(&rl_line_buffer[i], "set", 3) == 0  &&  start > (i + 3)) {
#ifdef RL_READLINE_VERSION /* ? */
		matches = rl_completion_matches(text, (rl_compentry_func_t *)set_arg_generator);
#else
		matches = completion_matches(text, set_arg_generator);
#endif
	} else {

		if (start == 0) {
			/* 行頭での入力の場合 */
#ifdef RL_READLINE_VERSION /* ? */
			matches = rl_completion_matches(text, (rl_compentry_func_t *)command_generator);
#else
			matches = completion_matches(text, command_generator);
#endif
		}
	}
	return matches;
}

void initialize_readline(void)
{
	rl_readline_name = "QUASI88";  /*よくわからんが ~/.inputrc に関係あるらしい*/
	rl_attempted_completion_function = (CPPFunction *)fileman_completion;
}

#endif /* USE_GNU_READLINE */


/***************************************************************
 * 入力処理
 ****************************************************************/
void mon_command(void)
{
#ifndef USE_GNU_READLINE

	printf("QUASI88> ");
	if (fgets(d_buf, MON_MAX_CHRS, stdin) == NULL) {
		/* ^D が入力されたら無視 */
		clearerr(stdin);
		return;
	}

#else
	{
		/* GNU readline の仕様がいまいちわからん。 */
		/* man で斜め読みしてみたが、英語は理解しがたい。 */
		char *p, *chk;
		HIST_ENTRY *ph;
		p = readline("QUASI88> ");
		if (p == NULL) {
			printf("\n");
			break;
		} else {

			ph = previous_history();
			if (*p == '\0' && ph != NULL) {
				/* リターンキーで直前のコマンドを実行 */
				strncpy(d_buf, ph->line, MON_MAX_CHRS - 1);
			} else {
				strncpy(d_buf, p, MON_MAX_CHRS - 1);
			}
			d_buf[ MON_MAX_CHRS - 1 ] = '\0';

			/* 空行じゃなければ履歴に残す */
			chk = p;
			while (*chk) {
				if (*chk == ' ' || *chk == '\t') {
					chk++;
					continue;
				} else if (ph != NULL && strcmp(chk, ph->line) == 0) {
					/* 同じコマンドは履歴に残さない */
					break;
				} else {
					add_history(chk);
					break;
				}
			}
		}
		free(p);
		/* このあたりの処理は、peachさんにより改良されました */
	}
#endif

	/* 引数を分解 */
	mon_getarg();

	if (d_argc == 0) {
		/* 空行の場合 */
		;
	} else {
		int i;
		for (i = 0; monitor_cmd[i].cmd; i++) {
			if (strcmp(d_argv[0], monitor_cmd[i].cmd) == 0) {
				break;
			}
			if ((monitor_cmd[i].short_cmd) &&
				strcmp(d_argv[0], monitor_cmd[i].short_cmd) == 0) {
				break;
			}
		}
		if (monitor_cmd[i].cmd == NULL) {
			/* 無効命令の場合 */
			printf("Invalid command : %s\n", d_argv[0]);
		} else {
			/* 引数が ? の場合 */
			if (d_argc == 2 && strcmp(d_argv[1], "?") == 0) {
				(monitor_cmd[i].help)(FALSE);
			} else {
				/* 通常の命令の場合 */
				mon_shift();
				fflush(NULL);
				(monitor_cmd[i].exec)();
			}
		}
	}
}


/***************************************************************
 * モニター変数
 ****************************************************************/
t_monitor_var mon;

void mon_var_init(void)
{
	mon.reg.style = 1;
	mon.reg.line = 3;

	mon.search.addr = -1;
	mon.search.size = -1;
	mon.search.bank = ARG_MAIN;
	mon.search.data = 0;
}

#endif /* USE_MONITOR */
