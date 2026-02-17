/***********************************************************************
 *
 ************************************************************************/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "debug.h"

#include "pc88main.h"
#include "pc88sub.h"
#include "memory.h"

#include "emu.h"
#include "drive.h"
#include "event.h"
#include "snddrv.h"
#include "wait.h"
#include "statusbar.h"
#include "suspend.h"
#include "snapshot.h"
#include "soundbd.h"
#include "menu.h"
#include "intr.h"
#include "toolbar.h"

/***********************************************************************
 * 各種動作パラメータの変更
 *      これらの関数は、ショートカットキー処理や、機種依存部のイベント
 *      処理などから呼び出されることを *一応* 想定している。
 *
 *      メニュー画面の表示中に呼び出すと、メニュー表示と食い違いが生じる
 *      ので、メニュー中は呼び出さないように。エミュ実行中に呼び出すのが
 *      一番安全。うーん、いまいち。
 *
 *      if (mode == EXEC) {
 *          quasi88_disk_insert_and_reset(file, FALSE);
 *      }
 *
 ************************************************************************/

/***********************************************************************
 * QUASI88 起動中のリセット処理関数
 ************************************************************************/
void quasi88_get_reset_cfg(T_RESET_CFG *cfg)
{
	cfg->boot_basic      = boot_basic;
	cfg->boot_dipsw      = boot_dipsw;
	cfg->boot_from_rom   = boot_from_rom;
	cfg->boot_clock_4mhz = boot_clock_4mhz;
	cfg->set_version     = set_version;
	cfg->baudrate_sw     = baudrate_sw;
	cfg->use_extram      = use_extram;
	cfg->use_jisho_rom   = use_jisho_rom;
	cfg->sound_board     = sound_board;
}

void quasi88_reset(const T_RESET_CFG *cfg)
{
	int sb_changed = FALSE;
	int empty[2];

	if (verbose_proc) {
		printf("Reset QUASI88...start\n");
	}

	pc88main_term();
	pc88sub_term();

	if (cfg) {
		if (sound_board != cfg->sound_board) {
			sb_changed = TRUE;
		}

		boot_basic      = cfg->boot_basic;
		boot_dipsw      = cfg->boot_dipsw;
		boot_from_rom   = cfg->boot_from_rom;
		boot_clock_4mhz = cfg->boot_clock_4mhz;
		set_version     = cfg->set_version;
		baudrate_sw     = cfg->baudrate_sw;
		use_extram      = cfg->use_extram;
		use_jisho_rom   = cfg->use_jisho_rom;
		sound_board     = cfg->sound_board;
	}

	/* メモリの再確保が必要なら、処理する */
	if (memory_allocate_additional() == FALSE) {
		quasi88_exit(-1); /* 失敗！ */
	}

	if (sb_changed == FALSE) {
		/* サウンド出力のリセット */
		xmame_sound_reset();
	} else {
		/* サウンドドライバの再初期化 */
		quasi88_sound_restart(FALSE);
	}

	/* ワークの初期化 */
	pc88main_init(INIT_RESET);
	pc88sub_init(INIT_RESET);

	/* FDCの初期化 */
	empty[0] = drive_check_empty(0);
	empty[1] = drive_check_empty(1);
	drive_reset();
	if (empty[0]) {
		drive_set_empty(0);
	}
	if (empty[1]) {
		drive_set_empty(1);
	}

	/*if (xmame_has_sound()) { xmame_sound_reset(); }*/

	emu_reset();

	submenu_controll(CTRL_RESET);

	if (verbose_proc) {
		printf("Reset QUASI88...done\n");
	}
}



/***********************************************************************
 * QUASI88 起動中のステートロード処理関数
 *      TODO 引数で、ファイル名指定？
 ************************************************************************/
int quasi88_stateload(int serial)
{
	int now_board, success;

	if (serial >= 0) {
		/* 連番指定あり (>=0) なら 連番を設定する */
		filename_set_state_serial(serial);
	}

	if (verbose_proc) {
		printf("Stateload...start (%s)\n", filename_get_state());
	}

	if (stateload_check_file_exist() == FALSE) {
		/* ファイルなし */
		if (quasi88_is_exec()) {
			statusbar_message(STATUS_INFO,
							  "State-Load file not found !",
							  "ステートロード失敗!(ファイルなし)");
		}
		/* メニューではダイアログ表示するので、ステータス表示は無しにする */

		if (verbose_proc) {
			printf("State-file not found\n");
		}
		return FALSE;
	}


	/* 念のため、ワークを終了状態に */
	pc88main_term();
	pc88sub_term();

	/* イメージファイルを全て閉じる */
	imagefile_all_close();

	/* 念のため、サウンドリセット */
	/*xmame_sound_reset();*/
	/* 念のため、全ワークリセット */
	/*quasi88_reset();*/


	now_board = sound_board;

	/* ステートロード実行 */
	success = stateload();

	if (now_board != sound_board) {
		/* サウンドボードが変わったら サウンドドライバの再初期化 */
		quasi88_sound_restart(FALSE);
	}

	if (verbose_proc) {
		if (success) {
			printf("Stateload...done\n");
		} else {
			printf("Stateload...Failed, Reset start\n");
		}
	}


	if (success) {
		/* ステートロード成功したら・・・ */

		/* イメージファイルを全て開く*/
		imagefile_all_open(TRUE);

		pc88main_init(INIT_STATELOAD);
		pc88sub_init(INIT_STATELOAD);

	} else {
		/* ステートロード失敗したら・・・ */

		/* とりあえずリセット */
		quasi88_reset(NULL);
	}


	if (quasi88_is_exec()) {
		if (success) {
			emu_status_message_set(STATUS_INFO,
								   "State-Load Successful",
								   "ステートロードしました");
		} else {
			emu_status_message_set(STATUS_INFO,
								   "State-Load Failed !  Reset...",
								   "ステートロード失敗!");
		}

		/* quasi88_loop の内部状態を INIT にするため、モード変更扱いとする */
		quasi88_event_flags |= EVENT_MODE_CHANGED;
	}
	/* メニューではダイアログ表示するので、ステータス表示は無しにする */

	return success;
}



/***********************************************************************
 * QUASI88 起動中のステートセーブ処理関数
 *      TODO 引数で、ファイル名指定？
 ************************************************************************/
int quasi88_statesave(int serial)
{
	int success;

	if (serial >= 0) {
		/* 連番指定あり (>=0) なら 連番を設定する */
		filename_set_state_serial(serial);
	}

	if (verbose_proc) {
		printf("Statesave...start (%s)\n", filename_get_state());
	}

	/* ステートセーブ実行 */
	success = statesave();

	if (verbose_proc) {
		if (success) {
			printf("Statesave...done\n");
		} else {
			printf("Statesave...Failed, Reset done\n");
		}
	}


	if (quasi88_is_exec()) {
		if (success) {
			statusbar_message(STATUS_INFO,
							  "State-Save Successful",
							  "ステートセーブしました");
		} else {
			statusbar_message(STATUS_INFO,
							  "State-Save Failed !",
							  "ステートセーブ失敗!");
		}
	}
	/* メニューではダイアログ表示するので、ステータス表示は無しにする */

	return success;
}



/***********************************************************************
 * 画面スナップショット保存
 *      TODO 引数で、ファイル名指定？
 ************************************************************************/
int quasi88_screen_snapshot(void)
{
	int success;

	success = screen_snapshot_save();


	if (success) {
		statusbar_message(STATUS_INFO,
						  "Screen Capture Saved",
						  "スクリーンショットを保存しました");
	} else {
		statusbar_message(STATUS_INFO,
						  "Screen Capture Failed !",
						  "スクリーンショット失敗!");
	}

	return success;
}



/***********************************************************************
 * サウンドデータのファイル出力
 *      TODO 引数で、ファイル名指定？
 ************************************************************************/
int quasi88_waveout(int start)
{
	int success;

	if (start) {
		success = waveout_save_start();

		if (success) {
			statusbar_message(STATUS_INFO,
							  "Sound Record Start ...",
							  "録音開始します");
		} else {
			statusbar_message(STATUS_INFO,
							  "Sound Record Failed !",
							  "録音失敗!");
		}

	} else {

		success = TRUE;

		waveout_save_stop();
		statusbar_message(STATUS_INFO,
						  "Sound Record Stopped",
						  "録音停止します");
	}

	return success;
}



/***********************************************************************
 * ドラッグアンドドロップ
 *      TODO 戻り値をもう一工夫
 ************************************************************************/
int quasi88_drag_and_drop(const char *filename)
{
	if (quasi88_is_exec() ||
		(quasi88_is_pause())) {

		if (quasi88_disk_insert_all(filename, FALSE)) {

			statusbar_message(STATUS_INFO,
							  "Disk Image Set and Reset",
							  "ディスクセットOK。リセットします");

			quasi88_reset(NULL);

			if (quasi88_is_pause()) {
				quasi88_exec();
			}

		} else {

			statusbar_message(STATUS_WARN,
							  "D&D Failed !  Disk Unloaded ...",
							  "ディスクをセットできません !!");
		}

		return TRUE;
	}

	return FALSE;
}



/***********************************************************************
 * サウンドドライバの再初期化 (サウンド関連の設定が変更された場合)
 ************************************************************************/
void quasi88_sound_restart(int output)
{
	/* 中断したサウンドを復帰後に、サウンドを停止させて、サウンド再初期化 */
	xmame_sound_resume();
	xmame_sound_stop();
	xmame_sound_start();


	/* サウンドドライバを再初期化すると、WAV出力が継続できない場合がある */
	if (xmame_wavout_damaged()) {
		quasi88_waveout(FALSE);
		XPRINTF("*** Waveout Stop ***\n");
	}


	/* 強制リスタート時は、ポートの再初期化は、呼び出し元にて実施する。
	   そうでない場合は、ここで再初期化 */
	if (output) {
		sound_output_after_stateload();
	}


	XPRINTF("*** Sound Setting Is Applied ***\n\n");
}



/***********************************************************************
 * ウェイトの比率設定
 * ウェイトの有無設定
 ************************************************************************/
int quasi88_cfg_now_wait_rate(void)
{
	return wait_rate;
}
void quasi88_cfg_set_wait_rate(int rate)
{
	char str[32];
	long dt;

	if (rate < 5) {
		rate = 5;
	}
	if (rate > 5000) {
		rate = 5000;
	}

	if (wait_rate != rate) {
		wait_rate = rate;

		if (quasi88_is_exec()) {

			sprintf(str, "WAIT  %4d[%%]", wait_rate);

			statusbar_message(STATUS_INFO, str, 0);

			dt = (long)((1000000.0 / (CONST_VSYNC_FREQ * wait_rate / 100)));
			wait_vsync_setup(dt, wait_by_sleep);
		}

		submenu_controll(CTRL_CHG_WAIT);
	}
}
int quasi88_cfg_now_no_wait(void)
{
	return no_wait;
}
void quasi88_cfg_set_no_wait(int enable)
{
	char str[32];
	long dt;

	if (no_wait != enable) {
		no_wait = enable;

		if (quasi88_is_exec()) {

			if (no_wait) {
				sprintf(str, "WAIT  OFF");
			} else {
				sprintf(str, "WAIT  ON");
			}

			statusbar_message(STATUS_INFO, str, 0);

			dt = (long)((1000000.0 / (CONST_VSYNC_FREQ * wait_rate / 100)));
			wait_vsync_setup(dt, wait_by_sleep);
		}

		submenu_controll(CTRL_CHG_NOWAIT);
	}
}



/***********************************************************************
 * ディスクイメージファイル設定
 *      ・両ドライブに挿入
 *      ・指定ドライブに挿入
 *      ・反対ドライブのイメージファイルを、挿入
 *      ・両ドライブ取り出し
 *      ・指定ドライブ取り出し
 ************************************************************************/
int quasi88_disk_insert_all(const char *filename, int ro)
{
	int success;

	quasi88_disk_eject_all();

	success = quasi88_disk_insert(DRIVE_1, filename, 0, ro);

	if (success) {

		if (disk_image_num(DRIVE_1) > 1) {
			quasi88_disk_insert_A_to_B(DRIVE_1, DRIVE_2, 1);
		}
	}

	imagefile_status();

	return success;
}
int quasi88_disk_insert(int drv, const char *filename, int image, int ro)
{
	int success = FALSE;

	quasi88_disk_eject(drv);

	if (strlen(filename) < QUASI88_MAX_FILENAME) {

		if (disk_insert(drv, filename, image, ro) == 0) {
			success = TRUE;
		} else {
			success = FALSE;
		}

		if (success) {

			if (drv == DRIVE_1) {
				boot_from_rom = FALSE;
			}

			strcpy(file_disk[ drv ], filename);
			readonly_disk[ drv ] = ro;

			if (filename_synchronize) {
				filename_init_state(TRUE);
				filename_init_snap(TRUE);
				filename_init_wav(TRUE);
			}
		}
	}

	imagefile_status();

	return success;
}
int quasi88_disk_insert_A_to_B(int src, int dst, int img)
{
	int success;

	quasi88_disk_eject(dst);

	if (disk_insert_A_to_B(src, dst, img) == 0) {
		success = TRUE;
	} else {
		success = FALSE;
	}

	if (success) {
		strcpy(file_disk[ dst ], file_disk[ src ]);
		readonly_disk[ dst ] = readonly_disk[ src ];

		if (filename_synchronize) {
			filename_init_state(TRUE);
			filename_init_snap(TRUE);
			filename_init_wav(TRUE);
		}
	}

	imagefile_status();

	return success;
}
void quasi88_disk_eject_all(void)
{
	int drv;

	for (drv = 0; drv < 2; drv++) {
		quasi88_disk_eject(drv);
	}

	boot_from_rom = TRUE;

	imagefile_status();
}
void quasi88_disk_eject(int drv)
{
	if (disk_image_exist(drv)) {
		disk_eject(drv);
		memset(file_disk[ drv ], 0, QUASI88_MAX_FILENAME);

		if (filename_synchronize) {
			filename_init_state(TRUE);
			filename_init_snap(TRUE);
			filename_init_wav(TRUE);
		}
	}

	imagefile_status();
}

/***********************************************************************
 * ディスクイメージファイル設定
 *      ・ドライブを一時的に空の状態にする
 *      ・ドライブのイメージを変更する
 *      ・ドライブのイメージを前のイメージに変更する
 *      ・ドライブのイメージを次のイメージに変更する
 ************************************************************************/
enum {
	TYPE_SELECT,
	TYPE_EMPTY,
	TYPE_NEXT,
	TYPE_PREV
};

static void disk_image_sub(int drv, int type, int img)
{
	int d;

	if (disk_image_exist(drv)) {
		switch (type) {

		case TYPE_EMPTY:
			drive_set_empty(drv);
			break;

		case TYPE_NEXT:
		case TYPE_PREV:
			if (type == TYPE_NEXT) {
				d = +1;
			} else {
				d = -1;
			}

			img = disk_image_selected(drv) + d;
			/* FALLTHROUGH */

		default:
			if (img < 0) {
				img = disk_image_num(drv) - 1;
			}
			if (img >= disk_image_num(drv)) {
				img = 0;
			}

			drive_unset_empty(drv);
			disk_change_image(drv, img);

			break;
		}
	}

	imagefile_status();
}
void quasi88_disk_image_select(int drv, int img)
{
	disk_image_sub(drv, TYPE_SELECT, img);
}
void quasi88_disk_image_empty(int drv)
{
	disk_image_sub(drv, TYPE_EMPTY, 0);
}
void quasi88_disk_image_next(int drv)
{
	disk_image_sub(drv, TYPE_NEXT, 0);
}
void quasi88_disk_image_prev(int drv)
{
	disk_image_sub(drv, TYPE_PREV, 0);
}

/***********************************************************************
 * ドライブを一時的に空の状態にして
 * その後、次のディスクイメージに変更する
 ************************************************************************/
int quasi88_disk_image_eject_before_insert(int drv)
{
	int eject_stat = 0;

	if (disk_image_exist(drv)) {
		if (drive_check_empty(drv)) {
			/* エンプティのまま */
			eject_stat = FALSE;
		} else {
			/* エンプティに変化した */
			drive_set_empty(drv);
			eject_stat = TRUE;
		}
	}

	imagefile_status();

	return eject_stat;
}
void quasi88_disk_image_insert_after_eject(int drv, int eject_stat)
{

	if (disk_image_exist(drv)) {
		int do_empty = FALSE;
		int img = disk_image_selected(drv);

		if (eject_stat) {
			img ++;
		} else {
			/* DO NOTHING */
		}

		if ((img < 0) || (img >= disk_image_num(drv))) {
			do_empty = TRUE;
			img = 0;
		} else {
			if (disk_image_exist(drv ^ 1) &&
				disk_same_file() &&
				drive_check_empty(drv ^ 1) == FALSE &&
				disk_image_selected(drv ^ 1) == img) {
				img ++;
				if ((img < 0) || (img >= disk_image_num(drv))) {
					do_empty = TRUE;
					img = 0;
				} 
			}
		}

		disk_change_image(drv, img);

		if (do_empty) {
			drive_set_empty(drv);
		} else {
			drive_unset_empty(drv);
		}
	}

	imagefile_status();
}







/*======================================================================
 * テープイメージファイル設定
 *              ・ロード用テープイメージファイルセット
 *              ・ロード用テープイメージファイル巻き戻し
 *              ・ロード用テープイメージファイル取り外し
 *              ・セーブ用テープイメージファイルセット
 *              ・セーブ用テープイメージファイル取り外し
 *======================================================================*/
int quasi88_load_tape_insert(const char *filename)
{
	quasi88_load_tape_eject();

	if (strlen(filename) < QUASI88_MAX_FILENAME &&
		sio_open_tapeload(filename)) {

		strcpy(file_tape[ CLOAD ], filename);
		return TRUE;

	}
	return FALSE;
}
int quasi88_load_tape_rewind(void)
{
	if (sio_tape_rewind()) {

		return TRUE;

	}
	quasi88_load_tape_eject();
	return FALSE;
}
void quasi88_load_tape_eject(void)
{
	sio_close_tapeload();
	memset(file_tape[ CLOAD ], 0, QUASI88_MAX_FILENAME);
}

int quasi88_save_tape_insert(const char *filename)
{
	quasi88_save_tape_eject();

	if (strlen(filename) < QUASI88_MAX_FILENAME &&
		sio_open_tapesave(filename)) {

		strcpy(file_tape[ CSAVE ], filename);
		return TRUE;

	}
	return FALSE;
}
void quasi88_save_tape_eject(void)
{
	sio_close_tapesave();
	memset(file_tape[ CSAVE ], 0, QUASI88_MAX_FILENAME);
}

/*======================================================================
 * シリアル・パラレルイメージファイル設定
 *              ・シリアル入力用ファイルセット
 *              ・シリアル入力用ファイル取り外し
 *              ・シリアル出力用ファイルセット
 *              ・シリアル出力用ファイル取り外し
 *              ・プリンタ出力用ファイルセット
 *              ・プリンタ入力用ファイルセット
 *======================================================================*/
int quasi88_serial_in_connect(const char *filename)
{
	quasi88_serial_in_remove();

	if (strlen(filename) < QUASI88_MAX_FILENAME &&
		sio_open_serialin(filename)) {

		strcpy(file_sin, filename);
		return TRUE;

	}
	return FALSE;
}
void quasi88_serial_in_remove(void)
{
	sio_close_serialin();
	memset(file_sin, 0, QUASI88_MAX_FILENAME);
}
int quasi88_serial_out_connect(const char *filename)
{
	quasi88_serial_out_remove();

	if (strlen(filename) < QUASI88_MAX_FILENAME &&
		sio_open_serialout(filename)) {

		strcpy(file_sout, filename);
		return TRUE;

	}
	return FALSE;
}
void quasi88_serial_out_remove(void)
{
	sio_close_serialout();
	memset(file_sout, 0, QUASI88_MAX_FILENAME);
}
int quasi88_printer_connect(const char *filename)
{
	quasi88_printer_remove();

	if (strlen(filename) < QUASI88_MAX_FILENAME &&
		printer_open(filename)) {

		strcpy(file_prn, filename);
		return TRUE;

	}
	return FALSE;
}
void quasi88_printer_remove(void)
{
	printer_close();
	memset(file_prn, 0, QUASI88_MAX_FILENAME);
}
