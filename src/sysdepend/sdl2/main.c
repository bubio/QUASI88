/************************************************************************/
/*                                                                      */
/*                              QUASI88                                 */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#include "quasi88.h"
#include "device.h"

#include "getconf.h"	/* config_init */
#include "suspend.h"	/* stateload_system */
#include "menu.h"		/* menu_about_osd_msg */
#include "keyboard.h"	/* romaji_type */
#include "monitor.h"	/* debug_mode */


/***********************************************************************
 * オプション
 ************************************************************************/
static int oo_env(const char *var, const char *str)
{
	if (SDL_setenv(var, str, 0) != 0) {
		/* 失敗したら警告したほうが親切だけど… */
	}
	return 0;
}

static int o_videodrv(char *str)
{
	return oo_env("SDL_VIDEODRIVER", str);
}

static int o_audiodrv(char *str)
{
	return oo_env("SDL_AUDIODRIVER", str);
}

static int invalid_arg;
static const T_CONFIG_TABLE sdl2_options[] = {

	/* 300..349: システム依存オプション */

	/* -- GRAPHIC -- */
	{ 300, "swrender",		X_FIX,	&use_software_rendering,	TRUE,	0,				0,				0			},
	{ 300, "hwrender",		X_FIX,	&use_software_rendering,	FALSE,	0,				0,				0			},
	{ 301, "desktopfs",		X_FIX,	&use_desktop_fullscreen,	TRUE,	0,				0,				0			},
	{ 301, "nodesktopfs",	X_FIX,	&use_desktop_fullscreen,	FALSE,	0,				0,				0			},

	/* -- INPUT -- */
	{ 311, "use_joy",		X_FIX,	&use_joydevice,		TRUE,			0,				0,				0			},
	{ 311, "nouse_joy",		X_FIX,	&use_joydevice,		FALSE,			0,				0,				0			},
	{ 312, "keyboard",		X_INT,	&keyboard_type,		0,				2,				0,				0			},
	{ 313, "keyconf",		X_STR,	&file_keyboard,		0,				0,				0,				0			},
	{ 314, "menukey",		X_FIX,	&use_menukey,		TRUE,			0,				0,				0			},
	{ 314, "nomenukey",		X_FIX,	&use_menukey,		FALSE,			0,				0,				0			},
	{ 315, "use_ime",		X_FIX,	&use_ime,			TRUE,			0,				0,				0			},
	{ 315, "nouse_ime",		X_FIX,	&use_ime,			FALSE,			0,				0,				0			},
	{ 316, "touchkey",		X_FIX,	&enable_touchkey,	TRUE,			0,				0,				0			},
	{ 316, "notouchkey",	X_FIX,	&enable_touchkey,	FALSE,			0,				0,				0			},
	{ 317, "separate",		X_FIX,	&separate_touchkey,	TRUE,			0,				0,				0			},
	{ 317, "noseparate",	X_FIX,	&separate_touchkey,	FALSE,			0,				0,				0			},

	/* -- SYSTEM -- */
	{ 320, "videodrv",		X_STR,	NULL,				0,				0,				o_videodrv,		0			},
	{ 321, "audiodrv",		X_STR,	NULL,				0,				0,				o_audiodrv,		0			},


	/* -- 無視 -- (他システムの引数つきオプション) */


	/* 終端 */
	{   0, NULL,			X_INV,	0,					0,				0,				0,				0			},
};

static void help_msg_sdl2(FILE *fp)
{
	fprintf(
		fp,
		"  ** GRAPHIC (SDL2 depend) **\n"
		"    -hwrender/-swrender     use Hardware/Software rendering [-hwrender]\n"
		"    -desktopfs/-nodesktopfs Use/Not use desktop-fullscreen [-desktopfs]\n"
		"  ** INPUT (SDL2 depend) **\n"
		"    -use_joy/-nouse_joy     Enable/Disabel system joystick [-use_joy]\n"
		"    -keyboard <0|1|2>       Set keyboard type (0:config/1:106key/2:101key) [1]\n"
		"    -keyconf <filename>     Specify keyboard configuration file <filename>\n"
		"    -menukey/-nomenukey     Right Windows/Command key to menu [-nomenukey]\n"
		"    -use_ime/-nouse_ime     Enable/Disable IME in menu [-nouseime]\n"
		"    -touchkey/-notouchkey   Support/Unsupport touch keyboard [-touchkey]\n"
		"    -separate/-noseparate   Use/Nouse new window for touch keyboard [-separate]\n"
		"  ** SYSTEM (SDL2 depend) **\n"
		"    -videodrv <drv>         do setenv(\"SDL_VIDEODRIVER=drv\")\n"
		"    -audiodrv <drv>         do setenv(\"SDL_AUDIODRIVER=drv\")\n"
	);
}



/***********************************************************************
 * Windows のコンソール処理
 ************************************************************************/
#if defined(_WIN32) && !defined(_CONSOLE)
#ifdef USE_MONITOR
static BOOL WINAPI MyHandler(DWORD event)
{
	switch (event) {
	case CTRL_C_EVENT:
		/* Ctrl-C は横取りしたいので、 FALSE を返す */
		return FALSE;

	default:
		/* case CTRL_CLOSE_EVENT: */
		/* case CTRL_BREAK_EVENT: */
		/* case CTRL_LOGOFF_EVENT: */
		/* case CTRL_SHUTDOWN_EVENT: */
		/* その他は、強制終了させたいのだが
		   これだと ↓ 異常終了する。なんかやり方がまずいのか … */

		/* quasi88_exit(0); */

		/* return FALSE; */
		return TRUE;
	}
}
static void win32_setup_console(void)
{
	/* Ctrl-C を横取りする */
	SetConsoleCtrlHandler(MyHandler, TRUE);
}
#else
#define win32_setup_console()
#endif

static BOOL use_consele = FALSE;
static BOOL win32_has_console(void)
{
	return use_consele;
}
static int win32_open_console(void)
{
	if (use_consele == FALSE) {
		/* コンソールを開く */
		use_consele = AllocConsole();
		if (use_consele) {

			/* コンソールに標準入出力を紐づける */
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
			freopen("CONIN$", "r", stdin);
#if 0
			/* コンソールの出力コードページを UTF-8 にする */
			SetConsoleOutputCP(CP_UTF8);
#endif
			{
				/* コンソールを閉じれないようにする
				 * ( 閉じたときの強制終了処理がうまくいかないので… ) */
				HMENU hmenu = GetSystemMenu(GetConsoleWindow(), FALSE);
				RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
			}
#ifdef _DEBUG
			printf("**Debug build**\n\n");
			fflush(NULL);
#endif
		}
	}
	return use_consele;
}
static void win32_close_console(void)
{
	if (use_consele) {
		FreeConsole();
		/* freopen した際の FILE は fclose したほうがいいのか ? */
		use_consele = FALSE;
	}
}
#else
#define	win32_open_console()				(FALSE)
#define	win32_close_console()
#define win32_setup_console()
#endif


static FILE *output_fp_sdl2(void)
{
#define CONFIG_LOG_FIELNAME					"quasi88-startup-log.txt"

	/* オプション解析中のエラーメッセージを出力する FILE を返す関数。
	 * 基本的には、 stderr に出力すればいいと思うが、
	 * use_textfile = TRUE としておけば、ログファイルを生成してそこ出力する */
#if 1
	static int use_textfile = FALSE;		/* stderr を使う */
#else
	static int use_textfile = TRUE;			/* ログファイルを生成 */
#endif

	static FILE *fp = NULL;

#if defined(_WIN32) && !defined(_CONSOLE)
	/* Win32 では、コンソールを使うようなオプションが指定されているならば、
	 * まずは、コンソールを開いてみて成功すればそこへ出力する */
	if (win32_has_console()) {
		return stderr;
	}
	if (fp == NULL) {
		use_textfile = TRUE;
		if (debug_mode || verbose_proc) {
			if (win32_open_console()) {
				return stderr;
			}
		}
	}
#endif

	if (use_textfile) {
		use_textfile = FALSE;
		if (fp == NULL) {
			fp = fopen(CONFIG_LOG_FIELNAME, "w");
		}
	}

	return fp;
}



/***********************************************************************
 * メイン処理
 ************************************************************************/
static void finish(void);

int main(int argc, char *argv[])
{
	int x = 1;

#ifdef _WIN32
	/* 一部の初期値を改変 (いいやり方はないかな…) */
	romaji_type = 1;					/* ローマ字変換の規則を MS-IME風に */
#endif


	/* 環境初期化 & 引数処理 */
	if (config_init(argc, argv, sdl2_options, help_msg_sdl2, output_fp_sdl2)) {

		/* -debug, -verbose オプション指定時は、コンソールを開く */
		if (debug_mode || verbose_proc) {
			if (win32_open_console()) {
				if (debug_mode) {
					win32_setup_console();
				}
			}
		}

		/* SDL2関連の初期化 */
		if (sdl2_init()) {

			/* quasi88() 実行中に強制終了した際のコールバック関数を登録する */
			quasi88_atexit(finish);

			/* PC-8801 エミュレーション */
			quasi88();

			/* SDL2関連後始末 */
			sdl2_exit();
		}

		win32_close_console();

		/* 引数処理後始末 */
		config_exit();
	}

	return 0;
}



/*
 * 強制終了時のコールバック関数 (quasi88_exit()呼出時に、処理される)
 */
static void finish(void)
{
	/* SDL2関連後始末 */
	sdl2_exit();

	win32_close_console();

	/* 引数処理後始末 */
	config_exit();
}



/***********************************************************************
 * ステートロード／ステートセーブ
 ************************************************************************/

/* 他の情報すべてがロード or セーブされた後に呼び出される。
 * 必要に応じて、システム固有の情報を付加してもいいかと。
 */

int stateload_system(void)
{
	return TRUE;
}
int statesave_system(void)
{
	return TRUE;
}



/***********************************************************************
 * メニュー画面に表示する、システム固有メッセージ
 ************************************************************************/

int menu_about_osd_msg(int req_japanese,
					   int *result_code, const char *message[])
{
	return FALSE;
}
