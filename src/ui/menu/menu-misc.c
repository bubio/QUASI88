/*===========================================================================
 *
 * メインページ その他
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "menu.h"

#include "suspend.h"
#include "snddrv.h"
#include "snapshot.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-misc.h"
#include "menu-misc-message.h"

/*----------------------------------------------------------------------*/
/* ステートセーブ */

static Q8tkWidget *misc_suspend_entry;
static Q8tkWidget *misc_suspend_combo;

static char state_filename[ QUASI88_MAX_FILENAME ];


/* メッセージダイアログを消す */
static void cb_misc_suspend_dialog_ok(UNUSED_WIDGET, void *result)
{
	dialog_destroy();

	if (P2INT(result) == DATA_MISC_RESUME_OK ||
		P2INT(result) == DATA_MISC_RESUME_ERR) {

		quasi88_exec();
		/* ← q8tk_main_quit() 呼出済み */
	}
}

/* ステートロード/セーブ実行後のメッセージダイアログ */
static void sub_misc_suspend_dialog(int result)
{
	const t_menulabel *l = data_misc_suspend_err;
	char filename[60 + 11 + 1]; /* 11 == strlen("[DRIVE 1:] "), 1 は '\0' */
	int save_code;
	int i, width, len;
	const char *ptr[4];
	const char *none = "(No Image File)";
	const char *dev[] = {
		"[DRIVE 1:]", "[DRIVE 2:]",
		"[TapeLOAD]", "[TapeSAVE]",
	};

	dialog_create();
	{
		/* 結果表示 */
		dialog_set_title(GET_LABEL(l, result));

		/* 成功時はイメージ名表示 */
		if (result == DATA_MISC_SUSPEND_OK ||
			result == DATA_MISC_RESUME_OK) {

			dialog_set_title(GET_LABEL(l, DATA_MISC_SUSPEND_LINE));
			dialog_set_title(GET_LABEL(l, DATA_MISC_SUSPEND_INFO));

			save_code = q8tk_set_kanjicode(osd_kanji_code());

			width = 0;
			for (i = 0; i < 4; i++) {
				if (i < 2) {
					if ((ptr[i] = filename_get_disk(i)) == NULL) {
						ptr[i] = none;
					}
				} else {
					if ((ptr[i] = filename_get_tape(i - 2)) == NULL) {
						ptr[i] = none;
					}
				}
				len = sprintf(filename, "%.60s", ptr[i]); /* == max 60 */
				width = MAX(width, len);
			}

			for (i = 0; i < 4; i++) {
				sprintf(filename, "%s %-*.*s", dev[i], width, width, ptr[i]);
				dialog_set_title(filename);
			}

			q8tk_set_kanjicode(save_code);
		}

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_MISC_SUSPEND_AGREE),
						  cb_misc_suspend_dialog_ok, INT2P(result));

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/* ステートロード不可のメッセージダイアログ */
static void sub_misc_suspend_not_access(void)
{
	const t_menulabel *l = data_misc_suspend_err;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_MISC_RESUME_CANTOPEN));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_MISC_SUSPEND_AGREE),
						  cb_misc_suspend_dialog_ok,
						  (void *)DATA_MISC_SUSPEND_AGREE);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}



/* ステートセーブ 上書き確認時のメッセージダイアログ */
static void cb_misc_suspend_overwrite(UNUSED_WIDGET, UNUSED_PARM);
static void sub_misc_suspend_really(void)
{
	const t_menulabel *l = data_misc_suspend_err;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_MISC_SUSPEND_REALLY));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_MISC_SUSPEND_OVERWRITE),
						  cb_misc_suspend_overwrite, NULL);
		dialog_set_button(GET_LABEL(l, DATA_MISC_SUSPEND_CANCEL),
						  cb_misc_suspend_dialog_ok,
						  (void *)DATA_MISC_SUSPEND_CANCEL);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

static void cb_misc_suspend_overwrite(UNUSED_WIDGET, UNUSED_PARM)
{
	dialog_destroy();
	{
		if (quasi88_statesave(-1)) {
			/* 成功 */
			sub_misc_suspend_dialog(DATA_MISC_SUSPEND_OK);
		} else {
			/* 失敗 */
			sub_misc_suspend_dialog(DATA_MISC_SUSPEND_ERR);
		}
	}
}

/*----------------------------------------------------------------------*/
/* 「セーブ」押下時時 (ステートセーブ) */
static void cb_misc_suspend_save(UNUSED_WIDGET, UNUSED_PARM)
{
#if 0 /* いちいち上書き確認してくるのはうざい？ */
	if (statesave_check_file_exist()) {
		/* ファイルある */
		sub_misc_suspend_really();
	} else
#endif
	{
		if (quasi88_statesave(-1)) {
			/* 成功 */
			sub_misc_suspend_dialog(DATA_MISC_SUSPEND_OK);
		} else {
			/* 失敗 */
			sub_misc_suspend_dialog(DATA_MISC_SUSPEND_ERR);
		}
	}
}

/*----------------------------------------------------------------------*/
/* 「ロード」押下時処理 (ステートロード) */
static void cb_misc_suspend_load(UNUSED_WIDGET, UNUSED_PARM)
{
	if (stateload_check_file_exist() == FALSE) {
		/* ファイルなし */
		sub_misc_suspend_not_access();

	} else {
		if (quasi88_stateload(-1)) {
			/* 成功 */
			sub_misc_suspend_dialog(DATA_MISC_RESUME_OK);
		} else {
			/* 失敗 */
			sub_misc_suspend_dialog(DATA_MISC_RESUME_ERR);
		}
	}
}

/*----------------------------------------------------------------------*/
/* ファイル名前変更。エントリー changed (入力)時に呼ばれる。
 * (ファイルセレクションでの変更時はこれは呼ばれない) */

static void sub_misc_suspend_combo_update(void)
{
	int i;
	char buf[4];

	/* ステートファイルの連番に応じてコンボ変更 */
	i = filename_get_state_serial();
	if ('0' <= i && i <= '9') {
		buf[0] = i;
	} else                      {
		buf[0] = ' ';
	}
	buf[1] = '\0';

	if (*(q8tk_combo_get_text(misc_suspend_combo)) != buf[0]) {
		q8tk_combo_set_text(misc_suspend_combo, buf);
	}
}

static void cb_misc_suspend_entry_change(Q8tkWidget *widget, UNUSED_PARM)
{
	filename_set_state(q8tk_entry_get_text(widget));

	sub_misc_suspend_combo_update();
}

/*----------------------------------------------------------------------*/
/* ファイル変更ダイアログ (ファイルセレクション) */

extern void sub_misc_suspend_update(void);
static void sub_misc_suspend_change(void);

static void cb_misc_suspend_fsel(UNUSED_WIDGET, UNUSED_PARM)
{
	const t_menulabel *l = data_misc_suspend;

	START_FILE_SELECTION(GET_LABEL(l, DATA_MISC_SUSPEND_FSEL),
						 -1, /* ReadOnly の選択は不可 */
						 q8tk_entry_get_text(misc_suspend_entry),
						 sub_misc_suspend_change,
						 state_filename,
						 QUASI88_MAX_FILENAME,
						 NULL);
}

static void sub_misc_suspend_change(void)
{
	filename_set_state(state_filename);
	q8tk_entry_set_text(misc_suspend_entry, state_filename);

	sub_misc_suspend_combo_update();
}

extern void sub_misc_suspend_update(void)
{
	q8tk_entry_set_text(misc_suspend_entry, filename_get_state());

	sub_misc_suspend_combo_update();
}

/* 連番のコンボボックス操作コールバック */
static void cb_misc_suspend_num(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_misc_suspend_num;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_misc_suspend_num); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {

			filename_set_state_serial(p->val);

			q8tk_entry_set_text(misc_suspend_entry, filename_get_state());
			return;
		}
	}
}

/*----------------------------------------------------------------------*/

static Q8tkWidget *menu_misc_suspend(void)
{
	Q8tkWidget *vbox, *hbox;
	Q8tkWidget *w, *e;
	const t_menulabel *l = data_misc_suspend;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			{
				PACK_LABEL(hbox, GET_LABEL(data_misc, DATA_MISC_SUSPEND));
			}
			{
				int save_code = q8tk_set_kanjicode(osd_kanji_code());

				e = PACK_ENTRY(hbox,
							   QUASI88_MAX_FILENAME, 74 - 11,
							   filename_get_state(),
							   NULL, NULL,
							   cb_misc_suspend_entry_change, NULL);
				/* q8tk_entry_set_position(e, 0); */
				misc_suspend_entry = e;

				q8tk_set_kanjicode(save_code);
			}
		}

		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, "    ");

			PACK_BUTTON(hbox,
						GET_LABEL(l, DATA_MISC_SUSPEND_SAVE),
						cb_misc_suspend_save, NULL);

			PACK_LABEL(hbox, " ");
			PACK_VSEP(hbox);
			PACK_LABEL(hbox, " ");

			PACK_BUTTON(hbox,
						GET_LABEL(l, DATA_MISC_SUSPEND_LOAD),
						cb_misc_suspend_load, NULL);

			w = PACK_LABEL(hbox, GET_LABEL(l, DATA_MISC_SUSPEND_NUMBER));
			q8tk_misc_set_placement(w,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			w = PACK_COMBO(hbox,
						   data_misc_suspend_num,
						   COUNTOF(data_misc_suspend_num),
						   filename_get_state_serial(), " ", 0,
						   cb_misc_suspend_num, NULL,
						   NULL, NULL);
			q8tk_misc_set_placement(w,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
			misc_suspend_combo = w;

			PACK_LABEL(hbox, "  ");

			PACK_BUTTON(hbox,
						GET_LABEL(l, DATA_MISC_SUSPEND_CHANGE),
						cb_misc_suspend_fsel, NULL);
		}
	}

	return vbox;
}



/*----------------------------------------------------------------------*/
/* 画面保存 */

static Q8tkWidget *misc_snapshot_entry;

static char snap_filename[ QUASI88_MAX_FILENAME ];


/*----------------------------------------------------------------------*/
/* 画面保存実行 (「開始」クリック時) */
static void cb_misc_snapshot_do(void)
{
	/* 念のため、スナップショットのファイル名を再設定 */
	filename_set_snap_base(q8tk_entry_get_text(misc_snapshot_entry));

	quasi88_screen_snapshot();

	/* スナップショットのファイル名は、実行時に変わることがあるので再設定 */
	q8tk_entry_set_text(misc_snapshot_entry, filename_get_snap_base());
}

/*----------------------------------------------------------------------*/
/* 画像形式 */
static int get_misc_snapshot_format(void)
{
	return snapshot_format;
}
static void cb_misc_snapshot_format(UNUSED_WIDGET, void *p)
{
	snapshot_format = P2INT(p);
}



/*----------------------------------------------------------------------*/
/* ファイル名前変更。エントリー changed (入力)時に呼ばれる。
 * (ファイルセレクションでの変更時はこれは呼ばれない) */

static void cb_misc_snapshot_entry_change(Q8tkWidget *widget, UNUSED_PARM)
{
	filename_set_snap_base(q8tk_entry_get_text(widget));
}

/*----------------------------------------------------------------------*/
/* ベース名変更ダイアログ (ファイルセレクション) */

extern void sub_misc_snapshot_update(void);
static void sub_misc_snapshot_change(void);

static void cb_misc_snapshot_fsel(UNUSED_WIDGET, UNUSED_PARM)
{
	const t_menulabel *l = data_misc_snapshot;

	START_FILE_SELECTION(GET_LABEL(l, DATA_MISC_SNAPSHOT_FSEL),
						 -1, /* ReadOnly の選択は不可 */
						 q8tk_entry_get_text(misc_snapshot_entry),
						 sub_misc_snapshot_change,
						 snap_filename,
						 QUASI88_MAX_FILENAME,
						 NULL);
}

static void sub_misc_snapshot_change(void)
{
	filename_set_snap_base(snap_filename);
	q8tk_entry_set_text(misc_snapshot_entry, snap_filename);
}

extern void sub_misc_snapshot_update(void)
{
	q8tk_entry_set_text(misc_snapshot_entry, filename_get_snap_base());
}


/*----------------------------------------------------------------------*/
#ifdef USE_SSS_CMD
/* コマンド実行状態変更 */
static int get_misc_snapshot_c_do(void)
{
	return snapshot_cmd_do;
}
static void cb_misc_snapshot_c_do(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	snapshot_cmd_do = key;
}

/* コマンド変更。エントリー changed (入力)時に呼ばれる。 */
static void cb_misc_snapshot_c_entry_change(Q8tkWidget *widget, UNUSED_PARM)
{
	strncpy(snapshot_cmd, q8tk_entry_get_text(widget), SNAPSHOT_CMD_SIZE - 1);
	snapshot_cmd[ SNAPSHOT_CMD_SIZE - 1 ] = '\0';
}
#endif /* USE_SSS_CMD */

/*----------------------------------------------------------------------*/
static Q8tkWidget *menu_misc_snapshot(void)
{
	Q8tkWidget *hbox, *vbox, *hbox2, *vbox2, *vbox3, *hbox3;
	Q8tkWidget *e;
	const t_menulabel *l = data_misc_snapshot;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			{
				PACK_LABEL(hbox, GET_LABEL(data_misc, DATA_MISC_SNAPSHOT));
			}
			{
				int save_code = q8tk_set_kanjicode(osd_kanji_code());

				e = PACK_ENTRY(hbox,
							   QUASI88_MAX_FILENAME, 74 - 11,
							   filename_get_snap_base(),
							   NULL, NULL,
							   cb_misc_snapshot_entry_change, NULL);
				/* q8tk_entry_set_position(e, 0); */
				misc_snapshot_entry = e;

				q8tk_set_kanjicode(save_code);
			}
		}

		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, "    ");

			PACK_BUTTON(hbox,
						GET_LABEL(l, DATA_MISC_SNAPSHOT_BUTTON),
						cb_misc_snapshot_do, NULL);

			PACK_LABEL(hbox, " ");
			PACK_VSEP(hbox);

			vbox2 = PACK_VBOX(hbox);
			{
				hbox2 = PACK_HBOX(vbox2);
				{
					vbox3 = PACK_VBOX(hbox2);

					PACK_LABEL(vbox3, "");

					hbox3 = PACK_HBOX(vbox3);
					{
						PACK_LABEL(hbox3,
								   GET_LABEL(l, DATA_MISC_SNAPSHOT_FORMAT));

						PACK_RADIO_BUTTONS(PACK_HBOX(hbox3),
										   data_misc_snapshot_format,
										   COUNTOF(data_misc_snapshot_format),
										   get_misc_snapshot_format(),
										   cb_misc_snapshot_format);

						PACK_LABEL(hbox3,
								   GET_LABEL(l, DATA_MISC_SNAPSHOT_PADDING));
					}

					PACK_BUTTON(hbox2,
								GET_LABEL(l, DATA_MISC_SNAPSHOT_CHANGE),
								cb_misc_snapshot_fsel, NULL);
				}

#ifdef USE_SSS_CMD
				if (snapshot_cmd_enable) {
					hbox2 = PACK_HBOX(vbox2);
					{
						PACK_CHECK_BUTTON(hbox2,
										  GET_LABEL(l, DATA_MISC_SNAPSHOT_CMD),
										  get_misc_snapshot_c_do(),
										  cb_misc_snapshot_c_do, NULL);

						PACK_LABEL(hbox2, " ");

						PACK_ENTRY(hbox2,
								   SNAPSHOT_CMD_SIZE, 38, snapshot_cmd,
								   NULL, NULL,
								   cb_misc_snapshot_c_entry_change, NULL);
					}
				}
#endif /* USE_SSS_CMD */
			}
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* サウンド保存 */

static Q8tkWidget *misc_waveout_entry;
static Q8tkWidget *misc_waveout_start;
static Q8tkWidget *misc_waveout_stop;
static Q8tkWidget *misc_waveout_change;

static char wave_filename[ QUASI88_MAX_FILENAME ];


static void sub_misc_waveout_sensitive(void)
{
	if (xmame_wavout_opened() == FALSE) {
		q8tk_widget_set_sensitive(misc_waveout_start,  TRUE);
		q8tk_widget_set_sensitive(misc_waveout_stop,   FALSE);
		q8tk_widget_set_sensitive(misc_waveout_change, TRUE);
		q8tk_widget_set_sensitive(misc_waveout_entry,  TRUE);
	} else {
		q8tk_widget_set_sensitive(misc_waveout_start,  FALSE);
		q8tk_widget_set_sensitive(misc_waveout_stop,   TRUE);
		q8tk_widget_set_sensitive(misc_waveout_change, FALSE);
		q8tk_widget_set_sensitive(misc_waveout_entry,  FALSE);
	}
	q8tk_widget_set_focus(NULL);
}
/*----------------------------------------------------------------------*/
/* サウンド保存開始 (「開始」クリック時) */
static void cb_misc_waveout_start(void)
{
	/* 念のため、サウンド出力のファイル名を再設定 */
	filename_set_wav_base(q8tk_entry_get_text(misc_waveout_entry));

	quasi88_waveout(TRUE);

	/* スナップショットのファイル名は、実行時に変わることがあるので再設定 */
	q8tk_entry_set_text(misc_waveout_entry, filename_get_wav_base());

	sub_misc_waveout_sensitive();
}

/*----------------------------------------------------------------------*/
/* サウンド出力停止 (「停止」クリック時) */
static void cb_misc_waveout_stop(void)
{
	quasi88_waveout(FALSE);

	sub_misc_waveout_sensitive();
}

/*----------------------------------------------------------------------*/
/* ファイル名前変更。エントリー changed (入力)時に呼ばれる。
 * (ファイルセレクションでの変更時はこれは呼ばれない) */

static void cb_misc_waveout_entry_change(Q8tkWidget *widget, UNUSED_PARM)
{
	filename_set_wav_base(q8tk_entry_get_text(widget));
}

/*----------------------------------------------------------------------*/
/* ファイル選択処理。ファイルセレクションを使用 */

extern void sub_misc_waveout_update(void);
static void sub_misc_waveout_change(void);

static void cb_misc_waveout_fsel(UNUSED_WIDGET, UNUSED_PARM)
{
	const t_menulabel *l = data_misc_waveout;

	START_FILE_SELECTION(GET_LABEL(l, DATA_MISC_WAVEOUT_FSEL),
						 -1, /* ReadOnly の選択は不可 */
						 q8tk_entry_get_text(misc_waveout_entry),
						 sub_misc_waveout_change,
						 wave_filename,
						 QUASI88_MAX_FILENAME,
						 NULL);
}

static void sub_misc_waveout_change(void)
{
	filename_set_wav_base(wave_filename);
	q8tk_entry_set_text(misc_waveout_entry, wave_filename);
}

extern void sub_misc_waveout_update(void)
{
	q8tk_entry_set_text(misc_waveout_entry, filename_get_wav_base());
}


/*----------------------------------------------------------------------*/
static Q8tkWidget *menu_misc_waveout(void)
{
	Q8tkWidget *hbox, *vbox;
	Q8tkWidget *e;
	const t_menulabel *l = data_misc_waveout;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			{
				PACK_LABEL(hbox, GET_LABEL(data_misc, DATA_MISC_WAVEOUT));
			}
			{
				int save_code = q8tk_set_kanjicode(osd_kanji_code());

				e = PACK_ENTRY(hbox,
							   QUASI88_MAX_FILENAME, 63,
							   filename_get_wav_base(),
							   NULL, NULL,
							   cb_misc_waveout_entry_change, NULL);
				/* q8tk_entry_set_position(e, 0); */
				misc_waveout_entry = e;

				q8tk_set_kanjicode(save_code);
			}
		}

		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, "    ");

			misc_waveout_start =
				PACK_BUTTON(hbox,
							GET_LABEL(l, DATA_MISC_WAVEOUT_START),
							cb_misc_waveout_start, NULL);

			PACK_LABEL(hbox, " ");
			PACK_VSEP(hbox);
			PACK_LABEL(hbox, " ");

			misc_waveout_stop =
				PACK_BUTTON(hbox,
							GET_LABEL(l, DATA_MISC_WAVEOUT_STOP),
							cb_misc_waveout_stop, NULL);

			PACK_LABEL(hbox, GET_LABEL(l, DATA_MISC_WAVEOUT_PADDING));

			misc_waveout_change =
				PACK_BUTTON(hbox,
							GET_LABEL(l, DATA_MISC_WAVEOUT_CHANGE),
							cb_misc_waveout_fsel, NULL);
		}
	}

	sub_misc_waveout_sensitive();

	return vbox;
}

/*----------------------------------------------------------------------*/
/* ファイル名合わせる */
static int get_misc_sync(void)
{
	return filename_synchronize;
}
static void cb_misc_sync(Q8tkWidget *widget, UNUSED_PARM)
{
	filename_synchronize = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
}



/*======================================================================*/

Q8tkWidget *menu_misc(void)
{
	Q8tkWidget *vbox;
	Q8tkWidget *w;

	vbox = PACK_VBOX(NULL);
	{
		PACK_HSEP(vbox);

		q8tk_box_pack_start(vbox, menu_misc_suspend());

		PACK_HSEP(vbox);

		q8tk_box_pack_start(vbox, menu_misc_snapshot());

		PACK_HSEP(vbox);

		w = menu_misc_waveout();

		if (xmame_has_sound()) {
			q8tk_box_pack_start(vbox, w);
			PACK_HSEP(vbox);
		}

		PACK_CHECK_BUTTON(vbox,
						  GET_LABEL(data_misc_sync, 0),
						  get_misc_sync(),
						  cb_misc_sync, NULL);
	}

	return vbox;
}
