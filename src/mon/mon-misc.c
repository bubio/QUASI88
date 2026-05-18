#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "graph.h"

#include "event.h"
#include "suspend.h"
#include "snapshot.h"


#ifdef USE_MONITOR

/*--------------------------------------------------------------
 * help [<cmd>]
 *      ヘルプを表示する
 *--------------------------------------------------------------*/
void mon_help_help(int oneline)
{
	if (oneline) {
		printf(
			"print help"
			);
	} else {
		printf(
			"  help [<cmd>]\n"
			"    print help\n"
			"    <cmd> ... command for help\n"
			"              [omit]... print short help for all commands.\n"
			);
	}
}

void mon_exec_help(void)
{
	int i;
	char *cmd = NULL;

	/* [cmd] */
	if (mon_exist_argv()) {
		cmd = argv.str;
		mon_shift();
	}
	if (mon_exist_argv()) {
		error();
	}



	if (cmd == NULL) {
		/* 引数なし。全ヘルプ表示 */

		printf("help\n");
		for (i = 0; monitor_cmd[i].cmd; i++) {
			printf("  %-10s ", monitor_cmd[i].cmd);
			(monitor_cmd[i].help)(TRUE);
			printf("\n");
		}
		printf("     Note: type \"help <command-name>\" for more details.\n");

	} else {
		/* 引数のコマンドのヘルプ表示 */

		for (i = 0; monitor_cmd[i].cmd; i++) {
			if (strcmp(cmd, monitor_cmd[i].cmd) == 0) {
				break;
			}
		}
		if (monitor_cmd[i].cmd == NULL) {
			error();
		}
		(monitor_cmd[i].help)(FALSE);

	}

}


/*--------------------------------------------------------------
 * menu
 *      メニューモード
 *--------------------------------------------------------------*/
void mon_help_menu(int oneline)
{
	if (oneline) {
		printf(
			"enter menu-mode"
			);
	} else {
		printf(
			"  menu\n"
			"    enter menu-mode.\n"
			);
	}
}

void mon_exec_menu(void)
{
	quasi88_menu();
}



/*--------------------------------------------------------------
 * quit
 *      QUASI88の終了
 *--------------------------------------------------------------*/
void mon_help_quit(int oneline)
{
	if (oneline) {
		printf(
			"quit quasi88"
			);
	} else {
		printf(
			"  quit\n"
			"    quit QUASI88.\n"
			);
	}
}

void mon_exec_quit(void)
{
	quasi88_quit();
}



/*--------------------------------------------------------------
 * redraw
 *      画面を再描画
 *--------------------------------------------------------------*/
void mon_help_redraw(int oneline)
{
	if (oneline) {
		printf(
			"redraw screen"
			);
	} else {
		printf(
			"  redraw\n"
			"    redraw QUASI88 screen.\n"
			);
	}
}

void mon_exec_redraw(void)
{
	screen_update_immidiate();
}



/*--------------------------------------------------------------
 * resize <screen_size>
 *      画面サイズを変更
 *--------------------------------------------------------------*/
void mon_help_resize(int oneline)
{
	if (oneline) {
		printf(
			"resize screen"
			);
	} else {
		printf(
			"  resize [<screen-size>]\n"
			"    resize screen.\n"
#ifdef SUPPORT_DOUBLE
			"    <screen_size> ... screen size FULL|HALF|DOUBLE|FULLSCREEN|WINDOW\n"
			"                      [omit]... change screen size HALF,FULL,DOUBLE...\n"
#else
			"    <screen_size> ... screen size FULL|HALF|FULLSCREEN|WINDOW\n"
			"                      [omit]... change screen size HALF<->FULL\n"
#endif
			);
	}
}

void mon_exec_resize(void)
{
	int command = -1;

	if (mon_exist_argv()) {
		if (! mon_argv_is(ARGV_RESIZE)) {
			error();
		}
		command = argv.val;
		mon_shift();
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	switch (command) {
	case -1:
		/* resize */
		quasi88_cfg_set_size_large();
		break;
	case ARG_FULL:
		/* resize full */
		quasi88_cfg_set_size(SCREEN_SIZE_FULL);
		break;
	case ARG_HALF:
		/* resize half */
		quasi88_cfg_set_size(SCREEN_SIZE_HALF);
		break;
	case ARG_DOUBLE:
		/* resize double */
#ifdef SUPPORT_DOUBLE
		quasi88_cfg_set_size(SCREEN_SIZE_DOUBLE);
#endif
		break;
	case ARG_FULLSCREEN:
		/* resize fullscreen */
		if (quasi88_cfg_can_fullscreen()) {
			quasi88_cfg_set_fullscreen(TRUE);
		}
		break;
	case ARG_WINDOW:
		/* resize window */
		if (quasi88_cfg_now_fullscreen()) {
			quasi88_cfg_set_fullscreen(FALSE);
		}
		break;
	}

	screen_update_immidiate();

	return;
}





/*--------------------------------------------------------------
 * statesave [<filename>]
 *      ステートセーブ
 *--------------------------------------------------------------*/
void mon_help_statesave(int oneline)
{
	if (oneline) {
		printf(
			"state-save QUASI88"
			);
	} else {
		printf(
			"  statesave [<filename>]\n"
			"    statesave QUASI88\n"
			"    <filename> ... specify state-file filename.\n"
			"                   omit, default filename\n"
			);
	}
}

void mon_exec_statesave(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		filename = NULL;
	}

	if (filename) {
		if (strlen(filename) >= QUASI88_MAX_FILENAME) {
			printf("filename too long\n");
			return;
		}
		filename_set_state(filename);
	}

	if (quasi88_statesave(-1)) {
		printf("statesave succsess\n"
			   "  state-file = %s\n", filename_get_state());
	} else {
		printf("statesave failed\n");
	}
	return;
}



/*--------------------------------------------------------------
 * stateload [<filename>]
 *      ステートロード
 *--------------------------------------------------------------*/
void mon_help_stateload(int oneline)
{
	if (oneline) {
		printf(
			"state-load QUASI88"
			);
	} else {
		printf(
			"  stateload [<filename>]\n"
			"    stateload QUASI88\n"
			"    <filename> ... specify state-file filename.\n"
			"                   omit, default filename\n"
			);
	}
}

void mon_exec_stateload(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		filename = NULL;
	}

	if (filename) {
		if (strlen(filename) >= QUASI88_MAX_FILENAME) {
			printf("filename too long\n");
			return;
		}
		filename_set_state(filename);
	}

	if (stateload_check_file_exist() == FALSE) {
		printf("state-file \"%s\" not exist or broken\n",
			   filename_get_state());
	} else {
		if (quasi88_stateload(-1)) {
			printf("stateload succsess\n"
				   "  state-file = %s\n", filename_get_state());
			quasi88_exec();
		} else {
			printf("stateload failed\n");
		}
	}
	return;
}



/*--------------------------------------------------------------
 * snapshot
 *      スクリーンスナップショットの保存
 *--------------------------------------------------------------*/
void mon_help_snapshot(int oneline)
{
	if (oneline) {
		printf(
			"save screen snapshot"
			);
	} else {
		printf(
			"  snapshot [<format>]\n"
			"    save screen snapshot\n"
			"    <format> ... select format \"BMP\", \"PPM\", \"RAW\".\n"
			"                 omit, current format\n"
			);
	}
}

void mon_exec_snapshot(void)
{
	int format = -1;

	if (mon_exist_argv()) {
		if (! mon_argv_is(ARGV_SNAPSHOT)) {
			error();
		}
		format = argv.val;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		format = -1;
	}

	switch (format) {
	case ARG_BMP:
		snapshot_format = 0;
		break;
	case ARG_PPM:
		snapshot_format = 1;
		break;
	case ARG_RAW:
		snapshot_format = 2;
		break;
	}

	if (screen_snapshot_save() == 0) {
		printf("save-snapshot failed\n");
	}
	return;
}



/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
void mon_help_misc(int oneline)
{
	if (oneline) {
		printf(
			"for debug"
			);
	} else {
		printf(
			"  misc ... this is for debug. don't mind!\n"
			);
	}
}

void mon_exec_misc(void)
{
	/*
	  int ch;
	  extern const char *mixer_get_name(int ch);
	  extern mixer_get_mixing_level(int ch);
	    for( ch=0; ch<16 ; ch++ ){
	      const char *name = mixer_get_name(ch);
	      if(name) printf( "%d[ch] %s\t:%d\n", ch,name,mixer_get_mixing_level(ch));
	    }
	*/
#if 0
	FILE *fp;
	fp = fopen("log.main", "wb");
	fwrite(main_ram,            sizeof(byte),  0x0f000,  fp);
	fwrite(main_high_ram,       sizeof(byte),  0x01000,  fp);
	fclose(fp);

	fp = fopen("log.high", "wb");
	fwrite(&main_ram[0xf000],  sizeof(byte),   0x1000,  fp);
	fclose(fp);

	fp = fopen("log.sub", "wb");
	fwrite(&sub_romram[0x4000], sizeof(byte),   0x4000,  fp);
	fclose(fp);

	fp = fopen("log.vram", "wb");
	fwrite(main_vram,           sizeof(byte), 4 * 0x4000,  fp);
	fclose(fp);
#endif

	/*
	  extern void mon_exec_fdc(void);
	  monitor_fdc();
	*/
	/*
	  int line = 0;
	  if( mon_exist_argv() ){
	    line = argv.val;
	    mon_shift();
	  }
	  printf( "line=%d\n",line );
	  attr_misc(line);
	*/

	printf("no action\n");
}

#endif /* USE_MONITOR */
