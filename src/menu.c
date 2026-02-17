/***********************************************************************
 *
 * メニューモード
 *
 ************************************************************************/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "fname.h"
#include "menu.h"
#include "pause.h"

#include "pc88main.h"
#include "pc88sub.h"
#include "soundbd.h"

#include "screen.h"
#include "emu.h"
#include "statusbar.h"
#include "snddrv.h"

#include "event.h"
#include "q8tk.h"



int menu_lang		= MENU_JAPAN;				/* メニューの言語 */
int menu_readonly	= FALSE;					/* ディスク選択ダイアログの
												 * 初期状態は ReadOnly ? */
int menu_swapdrv	= FALSE;					/* ドライブの表示順序 */
int menu_thema		= MENU_THEMA_LIGHT;			/* メニューのテーマ */



#include "menu-common.h"
#include "menu-top.h"
#include "toolbar.h"

#include "menu-volume.h"






/****************************************************************/
/* ワーク                                                       */
/****************************************************************/

/* メニュー終了時に、サウンドの環境が変わってないか確認のため、初期値保存 */
static void sd_cfg_save(void);
static int  sd_cfg_has_changed(void);


static int cpu_timing_save;					/* メニュー開始時の -cpu 値 記憶 */


/****************************************************************/
/* メニューモード メイン処理                                    */
/****************************************************************/

void ui_fullmenu_init(void)
{
	/* 現在の、サウンドの設定を保存 */
	sd_cfg_save();

	cpu_timing_save = cpu_timing;



	/* Q8TK メインモード開始 */
	q8tk_main_start();

	/* Q8TK メニュー生成 */
	menu_top();
}



void ui_fullmenu_main(void)
{
	/* メニューを抜けたら、メニューで変更した内容に応じて、再初期化 */
	if (quasi88_event_flags & EVENT_MODE_CHANGED) {

		if (quasi88_event_flags & EVENT_QUIT) {

			/* QUASI88終了時は、なにもしない      */
			/* (再初期化してもすぐに終了なので…) */

		} else {

#ifdef  USE_SOUND
			if (sd_cfg_has_changed()) {
				/* サウンド関連の設定に変更があればサウンドドライバの再初期化 */
				quasi88_sound_restart(TRUE);

				///* メニューモードでこの関数が呼ばれた場合に備えて、ワークリセット */
				//sd_cfg_save();

			}
#endif

			if (cpu_timing_save != cpu_timing) {
				emu_reset();
			}

			pc88main_bus_setup();
			pc88sub_bus_setup();
		}

		/* Q8TK メインモード終了 */
		q8tk_main_stop();

	} else {

		quasi88_event_flags |= EVENT_FRAME_UPDATE;
	}
}


/*---------------------------------------------------------------------------*/
/*
 * 現在のサウンドの設定値を記憶する (メニュー開始時に呼び出す)
 */
static void sd_cfg_save(void)
{
	int i;
	T_SNDDRV_CONFIG *p;


	memset(&sd_cfg_init, 0, sizeof(sd_cfg_init));
	memset(&sd_cfg_now,  0, sizeof(sd_cfg_now));

	sd_cfg_init.sound_board = sound_board;

#ifdef  USE_SOUND
	sd_cfg_init.sample_freq = xmame_cfg_get_sample_freq();
	sd_cfg_init.use_samples = xmame_cfg_get_use_samples();

#ifdef  USE_FMGEN
	sd_cfg_init.use_fmgen = xmame_cfg_get_use_fmgen();
#endif

	p = xmame_config_get_sndopt_tbl();

	if (p == NULL) {

		i = 0;

	} else {

		for (i = 0; i < NR_SD_CFG_LOCAL; i++, p++) {
			if (p->type == SNDDRV_NULL) {
				break;
			}

			sd_cfg_init.local[i].info = p;

			switch (p->type) {
			case SNDDRV_INT:
				sd_cfg_init.local[i].val.i = *((int *)(p->work));
				break;

			case SNDDRV_FLOAT:
				sd_cfg_init.local[i].val.f = *((float *)(p->work));
				break;
			}
		}
	}

	sd_cfg_init.local_cnt = i;

#endif

	sd_cfg_now = sd_cfg_init;
}

/*
 * サウンドの設定値が、記憶した値と違うと、真を返す (メニュー終了時にチェック)
 */
static int sd_cfg_has_changed(void)
{
#ifdef  USE_SOUND
	int i;
	T_SNDDRV_CONFIG *p;

	/* サウンドボードの変更だけ、チェックするワークが違う・・・ */
	if (sd_cfg_init.sound_board != sound_board) {
		return TRUE;
	}


#ifdef  USE_FMGEN
	if (sd_cfg_init.use_fmgen != sd_cfg_now.use_fmgen) {
		return TRUE;
	}
#endif

	if (sd_cfg_init.sample_freq != sd_cfg_now.sample_freq) {
		return TRUE;
	}

	if (sd_cfg_init.use_samples != sd_cfg_now.use_samples) {
		return TRUE;
	}

	for (i = 0; i < sd_cfg_init.local_cnt; i++) {

		p = sd_cfg_init.local[i].info;

		switch (p->type) {
		case SNDDRV_INT:
			if (sd_cfg_init.local[i].val.i != sd_cfg_now.local[i].val.i) {
				return TRUE;
			}
			break;

		case SNDDRV_FLOAT:
			if (sd_cfg_init.local[i].val.f != sd_cfg_now.local[i].val.f) {
				return TRUE;
			}
			break;
		}
	}
#endif

	return FALSE;
}




/***********************************************************************
 *
 ************************************************************************/
void menu_init(void)
{
	menu_set_thema(menu_thema);

	switch (quasi88_get_menu_mode()) {
	case MENU_MODE_FULLMENU:
		ui_fullmenu_init();
		break;
	case MENU_MODE_PAUSE:
		ui_pause_init();
		break;
	case MENU_MODE_ASKRESET:
		ui_askreset_init();
		break;
	case MENU_MODE_ASKSPEEDUP:
		ui_askspeedup_init();
		break;
	case MENU_MODE_ASKDISKCHANGE:
		ui_askdiskchange_init();
		break;
	case MENU_MODE_ASKQUIT:
		ui_askquit_init();
		break;
	}
}

void menu_main(void)
{
	switch (quasi88_get_menu_mode()) {
	case MENU_MODE_FULLMENU:
		ui_fullmenu_main();
		break;
	case MENU_MODE_PAUSE:
		ui_pause_main();
		break;
	case MENU_MODE_ASKRESET:
		ui_askreset_main();
		break;
	case MENU_MODE_ASKSPEEDUP:
		ui_askspeedup_main();
		break;
	case MENU_MODE_ASKDISKCHANGE:
		ui_askdiskchange_main();
		break;
	case MENU_MODE_ASKQUIT:
		ui_askquit_main();
		break;
	}
}

void menu_status(void)
{
	emu_status_message_clear();

	statusbar_message(0, "<ESC> key to return", 0);
}



/***********************************************************************
 *
 ************************************************************************/
int  menu_get_thema(void)
{
	return menu_thema;
}
void menu_set_thema(int thema)
{
	menu_thema = thema;
	if (menu_thema == MENU_THEMA_CLASSIC) {
		q8tk_set_solid(TRUE);
	} else {
		q8tk_set_solid(FALSE);
	}
}
