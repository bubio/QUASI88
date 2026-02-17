/*===========================================================================
 *
 * メインページ テープ
 *
 *===========================================================================*/

#include <stdio.h>

#include "quasi88.h"
#include "fname.h"
#include "initval.h"
#include "menu.h"

#include "pc88main.h"
#include "file-op.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-tape.h"
#include "menu-tape-message.h"


/*----------------------------------------------------------------------*/
/* ロード用テープイメージ・セーブ用テープイメージ */
int         tape_mode;
static char tape_filename[ QUASI88_MAX_FILENAME ];

static Q8tkWidget *tape_name[2];
static Q8tkWidget *tape_rate[2];

static Q8tkWidget *tape_button_eject[2];
static Q8tkWidget *tape_button_rew;

/*----------------------------------------------------------------------*/
static void set_tape_name(int c)
{
	const char *p = filename_get_tape(c);

	q8tk_entry_set_text(tape_name[ c ], (p ? p : "(No File)"));
}
static void set_tape_rate(int c)
{
	char buf[16];
	long cur, end;

	if (c == CLOAD) {

		if (sio_tape_pos(&cur, &end)) {
			if (end == 0) {
				sprintf(buf, "   END ");
			} else {
				sprintf(buf, "   %3ld%%", cur * 100 / end);
			}
		} else {
			sprintf(buf, "   ---%%");
		}

		q8tk_label_set(tape_rate[ c ], buf);
	}
}


/*----------------------------------------------------------------------*/
/* 「取出し」ボタン押下時の処理 */

static void cb_tape_eject_do(UNUSED_WIDGET, void *c)
{
	if (P2INT(c) == CLOAD) {
		quasi88_load_tape_eject();
	} else {
		quasi88_save_tape_eject();
	}

	set_tape_name(P2INT(c));
	set_tape_rate(P2INT(c));

	q8tk_widget_set_sensitive(tape_button_eject[P2INT(c)], FALSE);
	if (P2INT(c) == CLOAD) {
		q8tk_widget_set_sensitive(tape_button_rew, FALSE);
	}
	q8tk_widget_set_focus(NULL);
}

/*----------------------------------------------------------------------*/
/* 「巻戻し」ボタン押下時の処理 */

static void cb_tape_rew_do(UNUSED_WIDGET, void *c)
{
	if (P2INT(c) == CLOAD) {
		/* イメージを巻き戻す */
		if (quasi88_load_tape_rewind()) {
			/* 成功 */
			/* EMPTY */
		} else {
			/* 失敗 */
			set_tape_name(P2INT(c));
		}
		set_tape_rate(P2INT(c));
	}
}

/*----------------------------------------------------------------------*/
/* 「ファイル変更」ボタン押下時の処理 */

static void sub_tape_open(void);
static void sub_tape_open_do(void);
static void cb_tape_open_warn_clicked(Q8tkWidget *, void *);

static void cb_tape_open(UNUSED_WIDGET, void *c)
{
	const char *initial;
	const t_menulabel *l
		= (P2INT(c) == CLOAD) ? data_tape_load : data_tape_save;

	/* 今から生成するファイルセレクションは LOAD用か SAVE用か を覚えておく */
	tape_mode = P2INT(c);

	initial = filename_get_tape_or_dir(tape_mode);

	START_FILE_SELECTION(GET_LABEL(l, DATA_TAPE_FSEL),
						 -1, /* ReadOnly の選択は不可 */
						 initial,

						 sub_tape_open,
						 tape_filename,
						 QUASI88_MAX_FILENAME,
						 NULL);
}

static void sub_tape_open(void)
{
	const t_menulabel *l = data_tape_save;

	switch (osd_file_stat(tape_filename)) {

	case FILE_STAT_NOEXIST:
		if (tape_mode == CLOAD) {
			/* ファイル無いのでエラー */
			start_file_error_dialog(-1, ERR_CANT_OPEN);

		} else {
			/* ファイル無いので新規作成 */
			sub_tape_open_do();
		}
		break;

	case FILE_STAT_DIR:
		/* ディレクトリなので、開いちゃだめ */
		start_file_error_dialog(-1, ERR_CANT_OPEN);
		break;

	default:
		if (tape_mode == CSAVE) {
			/* すでにファイルが存在します。イメージを追記しますか？ */
			dialog_create();
			{
				dialog_set_title(GET_LABEL(l, DATA_TAPE_WARN_0));

				dialog_set_title(GET_LABEL(l, DATA_TAPE_WARN_1));

				dialog_set_separator();

				dialog_set_button(GET_LABEL(l, DATA_TAPE_WARN_APPEND),
								  cb_tape_open_warn_clicked, (void *)TRUE);

				dialog_set_button(GET_LABEL(l, DATA_TAPE_WARN_CANCEL),
								  cb_tape_open_warn_clicked, (void *)FALSE);

				dialog_accel_key(Q8TK_KEY_ESC);
			}
			dialog_start();
		} else {
			sub_tape_open_do();
		}
		break;
	}
}

static void sub_tape_open_do(void)
{
	int result, c = tape_mode;

	/* テープを開く */
	if (c == CLOAD) {
		result = quasi88_load_tape_insert(tape_filename);
	} else {
		result = quasi88_save_tape_insert(tape_filename);
	}

	set_tape_name(c);
	set_tape_rate(c);


	if (result == FALSE) {
		start_file_error_dialog(-1, ERR_CANT_OPEN);
	}

	q8tk_widget_set_sensitive(tape_button_eject[(int)c],
							  (result) ? TRUE : FALSE);
	if ((int)c == CLOAD) {
		q8tk_widget_set_sensitive(tape_button_rew,
								  (result) ? TRUE : FALSE);
	}
}

static void cb_tape_open_warn_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		sub_tape_open_do();
	}
}




/*----------------------------------------------------------------------*/

INLINE Q8tkWidget *menu_tape_image_unit(const t_menulabel *l, int c)
{
	int save_code;
	Q8tkWidget *vbox, *hbox, *w, *e;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			w = PACK_LABEL(hbox, GET_LABEL(l, DATA_TAPE_FOR));
			q8tk_misc_set_placement(w, Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			{
				save_code = q8tk_set_kanjicode(osd_kanji_code());

				e = PACK_ENTRY(hbox,
							   QUASI88_MAX_FILENAME, 65, NULL,
							   NULL, NULL, NULL, NULL);
				q8tk_entry_set_editable(e, FALSE);

				tape_name[ c ] = e;
				set_tape_name(c);

				q8tk_set_kanjicode(save_code);
			}
		}

		hbox = PACK_HBOX(vbox);
		{
			PACK_BUTTON(hbox,
						GET_LABEL(l, DATA_TAPE_CHANGE),
						cb_tape_open, INT2P(c));

			PACK_VSEP(hbox);

			tape_button_eject[c] = PACK_BUTTON(hbox,
											   GET_LABEL(l, DATA_TAPE_EJECT),
											   cb_tape_eject_do, INT2P(c));

			if (c == CLOAD) {
				tape_button_rew = PACK_BUTTON(hbox,
											  GET_LABEL(l, DATA_TAPE_REWIND),
											  cb_tape_rew_do, INT2P(c));
			}
			if (c == CLOAD) {
				w = PACK_LABEL(hbox, "");
				q8tk_misc_set_placement(w, Q8TK_PLACEMENT_X_CENTER,
										Q8TK_PLACEMENT_Y_CENTER);
				tape_rate[ c ] = w;
				set_tape_rate(c);
			}

			if (c == CLOAD) {
				if (tape_readable() == FALSE) {
					q8tk_widget_set_sensitive(tape_button_eject[c], FALSE);
					q8tk_widget_set_sensitive(tape_button_rew, FALSE);
				}
			} else {
				if (tape_writable() == FALSE) {
					q8tk_widget_set_sensitive(tape_button_eject[c], FALSE);
				}
			}
		}
	}

	return vbox;
}

static Q8tkWidget *menu_tape_image(void)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(NULL);
	{
		q8tk_box_pack_start(vbox, menu_tape_image_unit(data_tape_load, CLOAD));

		PACK_HSEP(vbox);

		q8tk_box_pack_start(vbox, menu_tape_image_unit(data_tape_save, CSAVE));
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* テープロードの処理方法 */
static int get_tape_intr(void)
{
	return cmt_intr;
}
static void cb_tape_intr(UNUSED_WIDGET, void *p)
{
	cmt_intr = P2INT(p);
}


static Q8tkWidget *menu_tape_intr(void)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(NULL);
	{
		PACK_RADIO_BUTTONS(vbox,
						   data_tape_intr, COUNTOF(data_tape_intr),
						   get_tape_intr(), cb_tape_intr);
	}

	return vbox;
}

/*======================================================================*/

Q8tkWidget *menu_tape(void)
{
	Q8tkWidget *vbox;
	const t_menulabel *l = data_tape;

	vbox = PACK_VBOX(NULL);
	{
		PACK_FRAME(vbox, GET_LABEL(l, DATA_TAPE_IMAGE), menu_tape_image());

		PACK_FRAME(vbox, GET_LABEL(l, DATA_TAPE_INTR), menu_tape_intr());
	}

	return vbox;
}
