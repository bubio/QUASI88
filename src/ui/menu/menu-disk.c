/*===========================================================================
 *
 * メインページ ディスク
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "menu.h"

#include "pc88main.h"
#include "fdc.h"
#include "drive.h"
#include "image.h"
#include "statusbar.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-disk.h"
#include "menu-disk-message.h"

#include "menu-reset.h"
#include "menu-misc.h"

/*----------------------------------------------------------------------*/

typedef struct {
	Q8tkWidget	*list;					/* イメージ一覧のリスト		*/
	Q8tkWidget	*label[2];				/* ボタンのラベル (2個)		*/
	int			func[2];				/* ボタンの機能 IMG_xxx		*/
	Q8tkWidget	*stat_label;			/* 情報 - Busy/Ready		*/
	Q8tkWidget	*attr_label;			/* 情報 - RO/RW属性			*/
	Q8tkWidget	*num_label;				/* 情報 - イメージ数		*/
} T_DISK_INFO;

static T_DISK_INFO disk_info[2];		/* 2ドライブ分のワーク		*/

static char        disk_filename[ QUASI88_MAX_FILENAME ];

static int         disk_drv;			/* 操作するドライブの番号	*/
static int         disk_img;			/* 操作するイメージの番号	*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static void set_disk_widget(void);


/* BOOT from DISK で、DISK を CLOSE した時や、
   BOOT from ROM  で、DISK を OPEN した時は、 DIP-SW 設定を強制変更 */
static void disk_update_dipsw_b_boot(void)
{
	if (disk_image_exist(0) || disk_image_exist(1)) {
		q8tk_toggle_button_set_state(widget_dipsw_b_boot_disk, TRUE);
	} else {
		q8tk_toggle_button_set_state(widget_dipsw_b_boot_rom,  TRUE);
	}
	set_reset_dipsw_boot();

	/* リセットしないでメニューモードを抜けると設定が保存されないので・・・ */
	/* thanks floi ! */
	boot_from_rom = reset_req.boot_from_rom;
}


/*----------------------------------------------------------------------*/
/* イメージの属性を変更するダイアログより各ボタン押下時に表示される
 * 個別ダイアログにて、実行を行ったときに呼び出されるコマンド別の処理 */

enum {
	ATTR_RENAME,		/* drive[drv] のイメージ img をリネーム			*/
	ATTR_PROTECT,		/* drive[drv] のイメージ img をプロテクト		*/
	ATTR_FORMAT,		/* drive[drv] のイメージ img をアンフォーマット	*/
	ATTR_UNFORMAT,		/* drive[drv] のイメージ img をフォーマット		*/
	ATTR_APPEND,		/* drive[drv] に最後にイメージを追加			*/
	ATTR_CREATE			/* 新規にディスクイメージファイルを作成			*/
};

static void sub_disk_attr_file_ctrl(int drv, int img, int cmd, char *c)
{
	int ro = FALSE;
	int result = -1;
	OSD_FILE *fp;

	if (cmd != ATTR_CREATE) {
		/* ドライブのファイルを変更する場合 そのファイルポインタを取得 */

		fp = drive[ drv ].fp;
		if (drive[ drv ].read_only) {
			ro = TRUE;
		}

	} else {
		/* 別のファイルを更新する場合 "r+b" でオープン */

		fp = osd_fopen(FTYPE_DISK, c, "r+b");
		if (fp == NULL) {
			/* だめなら "rb" でオープン */
			fp = osd_fopen(FTYPE_DISK, c, "rb");
			if (fp) {
				ro = TRUE;
			}
		}

		if (fp) {
			/* オープンできたら すでにドライブに開いてないかをチェックする */

			if (fp == drive[ 0 ].fp) {
				drv = 0;
			} else if (fp == drive[ 1 ].fp) {
				drv = 1;
			} else {
				drv = -1;
			}
		} else {
			/* オープンできない時は、新規に作成 */

			fp = osd_fopen(FTYPE_DISK, c, "ab");
			drv = -1;
		}
	}


	if (fp == NULL) {
		/* オープン失敗 */

		start_file_error_dialog(drv, ERR_CANT_OPEN);
		return;

	} else if (ro) {
		/* リードオンリーなので処理不可 */

		if (drv < 0) {
			osd_fclose(fp);
		}
		if (cmd != ATTR_CREATE) {
			start_file_error_dialog(drv, ERR_READ_ONLY);
		} else {
			start_file_error_dialog(-1, ERR_READ_ONLY);
		}
		return;

	} else if (drv >= 0 &&
			   drive[ drv ].detect_broken_image) {
		/* 壊れたイメージが含まれるので不可 */

		start_file_error_dialog(drv, ERR_MAYBE_BROKEN);
		return;
	}


#if 0
	if (cmd == ATTR_CREATE || cmd == ATTR_APPEND) {
		/* この処理に時間がかかるような場合、メッセージをだす？？ */
		/* この処理がそんなに時間がかかることはない？？ */
	}
#endif

	/* 開いたファイルに対して、処理 */
	switch (cmd) {
	case ATTR_RENAME:
		result = d88_write_name(fp, drv, img, c);
		break;
	case ATTR_PROTECT:
		result = d88_write_protect(fp, drv, img, c);
		break;
	case ATTR_FORMAT:
		result = d88_write_format(fp, drv, img);
		break;
	case ATTR_UNFORMAT:
		result = d88_write_unformat(fp, drv, img);
		break;
	case ATTR_APPEND:
	case ATTR_CREATE:
		result = d88_append_blank(fp, drv);
		break;
	}

	/* その結果 */
	switch (result) {
	case D88_SUCCESS:
		result = ERR_NO;
		break;
	case D88_NO_IMAGE:
		result = ERR_MAYBE_BROKEN;
		break;
	case D88_BAD_IMAGE:
		result = ERR_MAYBE_BROKEN;
		break;
	case D88_ERR_READ:
		result = ERR_MAYBE_BROKEN;
		break;
	case D88_ERR_SEEK:
		result = ERR_SEEK;
		break;
	case D88_ERR_WRITE:
		result = ERR_WRITE;
		break;
	default:
		result = ERR_UNEXPECTED;
		break;
	}

	/* 終了処理。なお、エラー時はメッセージを出す */

	if (drv < 0) {
		/* 新規オープンしたファイルを更新した場合 ファイルを閉じて終わり */

		osd_fclose(fp);

	} else {
		/* ドライブのファイルを更新した場合 メニュー画面を更新せねば */

		if (result == ERR_NO) {
			set_disk_widget();
			if (cmd != ATTR_CREATE) {
				disk_update_dipsw_b_boot();
			}
		}
	}

	if (result != ERR_NO) {
		start_file_error_dialog(drv, result);
	}

	return;
}

/*----------------------------------------------------------------------*/
/* 「名前変更」ダイアログ */

static void cb_disk_attr_rename_activate(UNUSED_WIDGET, void *p)
{
	char wk[16 + 1];

	/* dialog_destroy() の前に、エントリより変更する名前をゲット */
	if (P2INT(p)) {
		strncpy(wk, dialog_get_entry(), 16);
		wk[16] = '\0';
	}

	dialog_destroy();

	if (P2INT(p)) {
		sub_disk_attr_file_ctrl(disk_drv, disk_img, ATTR_RENAME, wk);
	}
}
static void sub_disk_attr_rename(const char *image_name)
{
	int save_code;
	const t_menulabel *l = data_disk_attr_rename;


	dialog_create();
	{
		dialog_set_title(GET_LABEL(l,
								   DATA_DISK_ATTR_RENAME_TITLE1 + disk_drv));

		save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
		dialog_set_title(image_name);
		q8tk_set_kanjicode(save_code);

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_RENAME_TITLE2));

		dialog_set_separator();

		save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
		dialog_set_entry(drive[disk_drv].image[disk_img].name,
						 16,
						 cb_disk_attr_rename_activate, (void *) TRUE);
		q8tk_set_kanjicode(save_code);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_RENAME_OK),
						  cb_disk_attr_rename_activate, (void *) TRUE);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_RENAME_CANCEL),
						  cb_disk_attr_rename_activate, (void *) FALSE);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/*----------------------------------------------------------------------*/
/* 「属性変更」ダイアログ */

static void cb_disk_attr_protect_clicked(UNUSED_WIDGET, void *p)
{
	char c;

	dialog_destroy();

	if (P2INT(p)) {
		if (P2INT(p) == 1) {
			c = DISK_PROTECT_TRUE;
		} else {
			c = DISK_PROTECT_FALSE;
		}

		sub_disk_attr_file_ctrl(disk_drv, disk_img, ATTR_PROTECT, &c);
	}
}
static void sub_disk_attr_protect(const char *image_name)
{
	int save_code;
	const t_menulabel *l = data_disk_attr_protect;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l,
								   DATA_DISK_ATTR_PROTECT_TITLE1 + disk_drv));

		save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
		dialog_set_title(image_name);
		q8tk_set_kanjicode(save_code);

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_PROTECT_TITLE2));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_PROTECT_SET),
						  cb_disk_attr_protect_clicked, (void *) 1);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_PROTECT_UNSET),
						  cb_disk_attr_protect_clicked, (void *) 2);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_PROTECT_CANCEL),
						  cb_disk_attr_protect_clicked, (void *) 0);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/*----------------------------------------------------------------------*/
/* 「(アン)フォーマット」ダイアログ */

static void cb_disk_attr_format_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		if (P2INT(p) == 1) {
			sub_disk_attr_file_ctrl(disk_drv, disk_img, ATTR_FORMAT,   NULL);
		} else {
			sub_disk_attr_file_ctrl(disk_drv, disk_img, ATTR_UNFORMAT, NULL);
		}
	}
}
static void sub_disk_attr_format(const char *image_name)
{
	int save_code;
	const t_menulabel *l = data_disk_attr_format;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l,
								   DATA_DISK_ATTR_FORMAT_TITLE1 + disk_drv));

		save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
		dialog_set_title(image_name);
		q8tk_set_kanjicode(save_code);

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_FORMAT_TITLE2));

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_FORMAT_WARNING));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_FORMAT_DO),
						  cb_disk_attr_format_clicked, (void *) 1);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_FORMAT_NOT),
						  cb_disk_attr_format_clicked, (void *) 2);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_FORMAT_CANCEL),
						  cb_disk_attr_format_clicked, (void *) 0);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/*----------------------------------------------------------------------*/
/* 「ブランクの追加」ダイアログ */

static void cb_disk_attr_blank_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		sub_disk_attr_file_ctrl(disk_drv, disk_img, ATTR_APPEND, NULL);
	}
}
static void sub_disk_attr_blank(void)
{
	const t_menulabel *l = data_disk_attr_blank;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_BLANK_TITLE1 + disk_drv));

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_BLANK_TITLE2));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_BLANK_OK),
						  cb_disk_attr_blank_clicked, (void *) TRUE);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_BLANK_CANCEL),
						  cb_disk_attr_blank_clicked, (void *) FALSE);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/*----------------------------------------------------------------------*/
/* 「イメージの属性を変更する」ボタン押下時の処理 */

static char disk_attr_image_name[20];
static void cb_disk_attr_clicked(UNUSED_WIDGET, void *p)
{
	char *name = disk_attr_image_name;

	dialog_destroy();

	switch (P2INT(p)) {
	case DATA_DISK_ATTR_RENAME:
		sub_disk_attr_rename(name);
		break;
	case DATA_DISK_ATTR_PROTECT:
		sub_disk_attr_protect(name);
		break;
	case DATA_DISK_ATTR_FORMAT:
		sub_disk_attr_format(name);
		break;
	case DATA_DISK_ATTR_BLANK:
		sub_disk_attr_blank();
		break;
	}
}
static void sub_disk_attr(void)
{
	int save_code;
	const t_menulabel *l = data_disk_attr;

	/* イメージ名をセット */
	sprintf(disk_attr_image_name,
			"\"%-16s\"", drive[disk_drv].image[disk_img].name);

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_TITLE1 + disk_drv));

		save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
		dialog_set_title(disk_attr_image_name);
		q8tk_set_kanjicode(save_code);

		dialog_set_title(GET_LABEL(l, DATA_DISK_ATTR_TITLE2));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_RENAME),
						  cb_disk_attr_clicked, (void *)DATA_DISK_ATTR_RENAME);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_PROTECT),
						  cb_disk_attr_clicked, (void *)DATA_DISK_ATTR_PROTECT);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_FORMAT),
						  cb_disk_attr_clicked, (void *)DATA_DISK_ATTR_FORMAT);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_BLANK),
						  cb_disk_attr_clicked, (void *)DATA_DISK_ATTR_BLANK);

		dialog_set_button(GET_LABEL(l, DATA_DISK_ATTR_CANCEL),
						  cb_disk_attr_clicked, (void *)DATA_DISK_ATTR_CANCEL);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}




/*----------------------------------------------------------------------*/
/* 「両方に開く」「開く」 ボタン押下時の処理 */

static int disk_open_ro;
static int disk_open_cmd;
static void sub_disk_open_ok(void);

static void sub_disk_open(int cmd)
{
	const char *initial;
	int num;
	const t_menulabel *l
		= (disk_drv == 0) ? data_disk_open_drv1 : data_disk_open_drv2;

	disk_open_cmd = cmd;
	num = (cmd == IMG_OPEN) ? DATA_DISK_OPEN_OPEN : DATA_DISK_OPEN_BOTH;

	/* ディスクがあればそのファイルを、なければディスク用ディレクトリを取得 */
	initial = filename_get_disk_or_dir(disk_drv);

	START_FILE_SELECTION(GET_LABEL(l, num),
						 (menu_readonly) ? 1 : 0, /* ReadOnly の選択可否 */
						 initial,
						 sub_disk_open_ok,
						 disk_filename,
						 QUASI88_MAX_FILENAME,
						 &disk_open_ro);
}

static void sub_disk_open_ok(void)
{
	if (disk_open_cmd == IMG_OPEN) {

		if (quasi88_disk_insert(disk_drv, disk_filename, 0, disk_open_ro)
			== FALSE) {
			start_file_error_dialog(disk_drv, ERR_CANT_OPEN);

		} else {

			/* 反対側と同じファイルだった場合 */
			if (disk_same_file()) {
				int dst = disk_drv;
				int src = disk_drv ^ 1;
				int img;

				if (drive[ src ].empty) {
					/* 反対側ドライブ 空なら 最初のイメージ */
					img = 0;
				} else {

					if (disk_image_num(src) == 1) {
						/* イメージが1個の場合は ドライブ 空に  */
						img = -1;

					} else {
						/* イメージが複数あれば 次(前)イメージ */
						img = disk_image_selected(src)
							  + ((dst == DRIVE_1) ? -1 : +1);
						if ((img < 0) ||
							(disk_image_num(dst) - 1 < img)) {
							img = -1;
						}
					}
				}
				if (img < 0) {
					drive_set_empty(dst);
				} else {
					disk_change_image(dst, img);
				}
			}
		}

	} else { /* IMG_BOTH */

		if (quasi88_disk_insert_all(disk_filename, disk_open_ro) == FALSE) {
			disk_drv = 0;
			start_file_error_dialog(disk_drv, ERR_CANT_OPEN);
		}
	}

	if (filename_synchronize) {
		sub_misc_suspend_update();
		sub_misc_snapshot_update();
		sub_misc_waveout_update();
	}
	set_disk_widget();
	disk_update_dipsw_b_boot();
}

/*----------------------------------------------------------------------*/
/* 「閉じる」 ボタン押下時の処理 */

static void sub_disk_close(void)
{
	quasi88_disk_eject(disk_drv);

	if (filename_synchronize) {
		sub_misc_suspend_update();
		sub_misc_snapshot_update();
		sub_misc_waveout_update();
	}
	set_disk_widget();
	disk_update_dipsw_b_boot();
}

/*----------------------------------------------------------------------*/
/* 「(反対ドライブと同じファイルを)開く」 ボタン押下時の処理 */

static void sub_disk_copy(void)
{
	int dst = disk_drv;
	int src = disk_drv ^ 1;
	int img;

	if (! disk_image_exist(src)) {
		return;
	}

	if (drive[ src ].empty) {
		/* 反対側ドライブ 空なら 最初のイメージ */
		img = 0;

	} else {

		if (disk_image_num(src) == 1) {
			/* イメージが1個の場合は ドライブ 空に  */
			img = -1;

		} else {
			/* イメージが複数あれば 次(前)イメージ */
			img = disk_image_selected(src) + ((dst == DRIVE_1) ? -1 : +1);
			if ((img < 0) ||
				(disk_image_num(dst) - 1 < img)) {
				img = -1;
			}
		}
	}

	if (quasi88_disk_insert_A_to_B(src, dst, img) == FALSE) {
		start_file_error_dialog(disk_drv, ERR_CANT_OPEN);
	}

	if (filename_synchronize) {
		sub_misc_suspend_update();
		sub_misc_snapshot_update();
		sub_misc_waveout_update();
	}
	set_disk_widget();
	disk_update_dipsw_b_boot();
}





/*----------------------------------------------------------------------*/
/* イメージのリストアイテム選択時の、コールバック関数 */

static void cb_disk_image(UNUSED_WIDGET, void *p)
{
	int drv = (P2INT(p)) & 0xff;
	int img = (P2INT(p)) >> 8;

	if (img < 0) {
		/* img == -1 で <なし> */
		drive_set_empty(drv);

	} else {
		/* img >= 0 なら イメージ番号 */
		drive_unset_empty(drv);
		disk_change_image(drv, img);
	}

	statusbar_image_name();
}

/*----------------------------------------------------------------------*/
/* ドライブ毎に配置された二つのボタンの、コールバック関数
 * 「開く」「両方に開く」「(反対ドライブのを)開く」「閉じる」
 * 「イメージの属性を変更する」 のいずれか */

static void cb_disk_button(UNUSED_WIDGET, void *p)
{
	int drv    = (P2INT(p)) & 0xff;
	int button = (P2INT(p)) >> 8;

	disk_drv = drv;
	disk_img = disk_image_selected(drv);

	switch (disk_info[drv].func[button]) {
	case IMG_OPEN:
	case IMG_BOTH:
		sub_disk_open(disk_info[drv].func[button]);
		break;

	case IMG_CLOSE:
		sub_disk_close();
		break;

	case IMG_COPY:
		sub_disk_copy();
		break;

	case IMG_ATTR:
		/* イメージ<<なし>>選択時は無効 */
		if (! drive_check_empty(drv)) {
			sub_disk_attr();
		}
		break;
	}
}

/*----------------------------------------------------------------------*/
/* ファイルを開く毎に、disk_info[] に情報をセット
 * (イメージのリスト生成、ボタン・情報のラベルをセット) */

static void set_disk_widget(void)
{
	int i, drv, save_code;
	Q8tkWidget *item;
	T_DISK_INFO *w;
	const t_menulabel *inf = data_disk_info;
	const t_menulabel *l   = data_disk_image;
	const t_menulabel *btn;
	char wk[40], wk2[20];
	const char *s;


	for (drv = 0; drv < 2; drv++) {
		w = &disk_info[drv];

		if (menu_swapdrv == FALSE) {
			btn = (drv == 0)
				  ? data_disk_button_drv1swap : data_disk_button_drv2swap;
		} else {
			btn = (drv == 0)
				  ? data_disk_button_drv1 : data_disk_button_drv2;
		}

		/* イメージ名の LIST ITEM 生成 */

		q8tk_listbox_clear_items(w->list, 0, -1);

		/* リストの先頭は必ず '<なし>' ITEM */
		item = q8tk_list_item_new_with_label(GET_LABEL(l,
													   DATA_DISK_IMAGE_EMPTY), 0);
		q8tk_widget_show(item);
		q8tk_container_add(w->list, item);
		q8tk_signal_connect(item, "select",
							cb_disk_image, INT2P((-1 << 8) + drv));

		/* リストの次以降は 'イメージ名' ITEM */
		if (disk_image_exist(drv)) {
			/* ディスク挿入済 */

			save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
			{
				for (i = 0; i < disk_image_num(drv); i++) {
					sprintf(wk, "%3d  %-16s  %s ", /*イメージNo イメージ名 RW*/
							i + 1,
							drive[drv].image[i].name,
							(drive[drv].image[i].protect) ? "RO" : "RW");

					item = q8tk_list_item_new_with_label(wk, 0);
					q8tk_widget_show(item);
					q8tk_container_add(w->list, item);
					q8tk_signal_connect(item, "select",
										cb_disk_image, INT2P((i << 8) + drv));
				}
			}
			q8tk_set_kanjicode(save_code);

			/* <なし> or 選択イメージ の ITEM をセレクト */
			if (drive_check_empty(drv)) {
				i = 0;
			} else {
				i = disk_image_selected(drv) + 1;
			}
			q8tk_listbox_select_item(w->list, i);

		} else {
			/* ドライブ空っぽ */
			/* <なし> ITEM をセレクト */

			q8tk_listbox_select_item(w->list, 0);
		}

		/* ボタンのラベルをセット 「閉じる」「属性変更」 / 「開く」「開く」 */

		if (disk_image_exist(drv)) {
			w->func[0] = IMG_CLOSE;
			w->func[1] = IMG_ATTR;
		} else {
			w->func[0] = (disk_image_exist(drv ^ 1)) ? IMG_COPY : IMG_BOTH;
			w->func[1] = IMG_OPEN;
		}
		q8tk_label_set(w->label[0], GET_LABEL(btn, w->func[0]));
		q8tk_label_set(w->label[1], GET_LABEL(btn, w->func[1]));

		/* 情報の状態ラベルをセット Busy/Ready */

		if (get_drive_ready(drv)) {
			s = GET_LABEL(inf, DATA_DISK_INFO_STAT_READY);
		} else {
			s = GET_LABEL(inf, DATA_DISK_INFO_STAT_BUSY);
		}
		q8tk_label_set(w->stat_label, s);
		q8tk_label_set_reverse(w->stat_label,
							   (get_drive_ready(drv)) ? FALSE : TRUE
							   /* BUSYなら反転表示 */);

		/* 情報の属性ラベルをセット - RO/RW */

		if (disk_image_exist(drv)) {
			if (drive[drv].read_only) {
				s = GET_LABEL(inf, DATA_DISK_INFO_ATTR_RO);
			} else {
				s = GET_LABEL(inf, DATA_DISK_INFO_ATTR_RW);
			}
		} else {
			s = "";
		}
		q8tk_label_set(w->attr_label, s);
		q8tk_label_set_color(w->attr_label,
							 (drive[drv].read_only) ? Q8GR_PALETTE_RED : -1, -1
							 /* ReadOnlyなら赤色表示 */);

		/* 情報の総数ラベルをセット - イメージ数 */

		if (disk_image_exist(drv)) {
			if (drive[drv].detect_broken_image) {
				/* 破損あり */
				s = GET_LABEL(inf, DATA_DISK_INFO_NR_BROKEN);

			} else if (drive[drv].over_image ||
					   disk_image_num(drv) > 99) {
				/* イメージ多過ぎ */
				s = GET_LABEL(inf, DATA_DISK_INFO_NR_OVER);

			} else {
				s = "";
			}
			sprintf(wk, "%2d%s",
					(disk_image_num(drv) > 99) ? 99 : disk_image_num(drv), s);
			sprintf(wk2, "%9.9s", wk); /* 9文字右詰めに変換 */

		} else {
			wk2[0] = '\0';
		}
		q8tk_label_set(w->num_label,  wk2);
	}
}


/*----------------------------------------------------------------------*/
/* 「ブランクの作成」 ボタン押下時の処理 */

static void sub_disk_blank_ok(void);
static void cb_disk_blank_warn_clicked(Q8tkWidget *, void *);


static void cb_disk_blank(UNUSED_WIDGET, UNUSED_PARM)
{
	const char *initial;
	const t_menulabel *l = data_disk_blank;

	/* ディスクがあればそのファイルを、なければディスク用ディレクトリを取得 */
	initial = filename_get_disk_or_dir(DRIVE_1);

	START_FILE_SELECTION(GET_LABEL(l, DATA_DISK_BLANK_FSEL),
						 -1, /* ReadOnly の選択は不可 */
						 initial,
						 sub_disk_blank_ok,
						 disk_filename,
						 QUASI88_MAX_FILENAME,
						 NULL);
}

static void sub_disk_blank_ok(void)
{
	const t_menulabel *l = data_disk_blank;

	switch (osd_file_stat(disk_filename)) {

	case FILE_STAT_NOEXIST:
		/* ファイルを新規に作成し、ブランクを作成 */
		sub_disk_attr_file_ctrl(0, 0, ATTR_CREATE, disk_filename);
		break;

	case FILE_STAT_DIR:
		/* ディレクトリなので、ブランクは追加できない */
		start_file_error_dialog(-1, ERR_CANT_OPEN);
		break;

	default:
		/* すでにファイルが存在します。ブランクを追加しますか？ */
		dialog_create();
		{
			dialog_set_title(GET_LABEL(l, DATA_DISK_BLANK_WARN_0));

			dialog_set_title(GET_LABEL(l, DATA_DISK_BLANK_WARN_1));

			dialog_set_separator();

			dialog_set_button(GET_LABEL(l, DATA_DISK_BLANK_WARN_APPEND),
							  cb_disk_blank_warn_clicked, (void *) TRUE);

			dialog_set_button(GET_LABEL(l, DATA_DISK_BLANK_WARN_CANCEL),
							  cb_disk_blank_warn_clicked, (void *) FALSE);

			dialog_accel_key(Q8TK_KEY_ESC);
		}
		dialog_start();
		break;
	}
}

static void cb_disk_blank_warn_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		/* ファイルに、ブランクを追記 */
		sub_disk_attr_file_ctrl(0, 0, ATTR_CREATE, disk_filename);
	}
}

/*----------------------------------------------------------------------*/
/* 「ファイル名確認」 ボタン押下時の処理 */

static void cb_disk_fname_dialog_ok(UNUSED_WIDGET, UNUSED_PARM)
{
	dialog_destroy();
}

static void cb_disk_fname(UNUSED_WIDGET, UNUSED_PARM)
{
	const t_menulabel *l = data_disk_fname;
	char filename[66 + 5 + 1]; /* 5 == strlen("[1:] "), 1 は '\0' */
	int save_code;
	int i, width, len;
	const char *ptr[2];
	const char *none = "(No Image File)";

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_TITLE));
		dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_LINE));

		{
			save_code = q8tk_set_kanjicode(osd_kanji_code());

			width = 0;
			for (i = 0; i < 2; i++) {
				if ((ptr[i] = filename_get_disk(i)) == NULL) {
					ptr[i] = none;
				}
				len = sprintf(filename, "%.66s", ptr[i]); /* == max 66 */
				width = MAX(width, len);
			}

			for (i = 0; i < 2; i++) {
				sprintf(filename, "[%d:] %-*.*s", i + 1, width, width, ptr[i]);
				dialog_set_title(filename);
			}

			q8tk_set_kanjicode(save_code);
		}

		if (disk_image_exist(0) && disk_same_file()) {
			dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_SAME));
		}

		if ((disk_image_exist(0) && drive[0].read_only) ||
			(disk_image_exist(1) && drive[1].read_only)) {
			dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_SEP));
			dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_RO));

			if (fdc_ignore_readonly == FALSE) {
				dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_RO_1));
				dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_RO_2));
			} else {
				dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_RO_X));
				dialog_set_title(GET_LABEL(l, DATA_DISK_FNAME_RO_Y));
			}
		}


		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_FNAME_OK),
						  cb_disk_fname_dialog_ok, NULL);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}


/*----------------------------------------------------------------------*/
/* ドライブ表示位置 左右入れ換え */

static void cb_disk_dispswap_clicked(UNUSED_WIDGET, UNUSED_PARM)
{
	dialog_destroy();
}

static int get_disk_dispswap(void)
{
	return menu_swapdrv;
}
static void cb_disk_dispswap(Q8tkWidget *widget, UNUSED_PARM)
{
	const t_menulabel *l = data_disk_dispswap;
	int parm = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	menu_swapdrv = parm;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_DISK_DISPSWAP_INFO_1));
		dialog_set_title(GET_LABEL(l, DATA_DISK_DISPSWAP_INFO_2));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_DISK_DISPSWAP_OK),
						  cb_disk_dispswap_clicked, NULL);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}

/*----------------------------------------------------------------------*/
/* ステータスにイメージ名表示 */

static int get_disk_dispstatus(void)
{
	return statusbar_get_imagename_show();
}
static void cb_disk_dispstatus(Q8tkWidget *widget, UNUSED_PARM)
{
	const t_menulabel *l = data_disk_dispstatus;
	int parm = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	statusbar_set_imagename_show(parm);
}

/*======================================================================*/

Q8tkWidget *menu_disk(void)
{
	Q8tkWidget *hbox, *vbox, *swin, *lab, *btn;
	Q8tkWidget *f, *vx, *hx;
	T_DISK_INFO *w;
	int i, j, k;
	const t_menulabel *l;


	hbox = PACK_HBOX(NULL);
	{
		/* DRIVE[1:] 、 DRIVE[2:] の表示エリア (配置順は設定で可変) */
		for (k = 0; k < COUNTOF(disk_info); k++) {

			if (menu_swapdrv == FALSE) {
				i = k ^ 1;
			} else {
				i = k;
			}

			w = &disk_info[i];
			{
				vbox = PACK_VBOX(hbox);
				{
					/* 見出し DRIVE[n:] */
					lab = PACK_LABEL(vbox, GET_LABEL(data_disk_image_drive, i));
					if (menu_swapdrv == FALSE) {
						q8tk_misc_set_placement(lab, Q8TK_PLACEMENT_X_RIGHT, 0);
					}

					/* イメージ選択用リストボックスのスクロールドウインドウ */
					{
						swin  = q8tk_scrolled_window_new(NULL, NULL);
						q8tk_widget_show(swin);
						q8tk_scrolled_window_set_policy(swin,
														Q8TK_POLICY_NEVER,
														Q8TK_POLICY_AUTOMATIC);
						q8tk_misc_set_size(swin, 29, 11);

						w->list = q8tk_listbox_new();
						q8tk_widget_show(w->list);
						q8tk_container_add(swin, w->list);

						q8tk_box_pack_start(vbox, swin);
					}

					/* ボタン2個 (名称は動的に変更させるので、空ラベルを確保) */
					for (j = 0; j < 2; j++) {
						w->label[j] = q8tk_label_new("");
						q8tk_widget_show(w->label[j]);
						btn = q8tk_button_new();
						q8tk_widget_show(btn);
						q8tk_container_add(btn, w->label[j]);
						q8tk_signal_connect(btn, "clicked",
											cb_disk_button,
											INT2P((j << 8) + i));

						q8tk_box_pack_start(vbox, btn);
					}
				}
			}

			PACK_VSEP(hbox);
		}

		/* ドライブ情報やその他操作の表示エリア */
		{
			vbox = PACK_VBOX(hbox);
			{
				l = data_disk_info;

				/* ドライブ情報を順に表示 */
				for (i = 0; i < COUNTOF(disk_info); i++) {
					w = &disk_info[i];

					/* 表示する値は動的に変更させるので、空ラベルを確保 */
					vx = PACK_VBOX(NULL);
					{
						hx = PACK_HBOX(vx);
						{
							PACK_LABEL(hx, GET_LABEL(l, DATA_DISK_INFO_STAT));
							w->stat_label = PACK_LABEL(hx, "");
						}

						hx = PACK_HBOX(vx);
						{
							PACK_LABEL(hx, GET_LABEL(l, DATA_DISK_INFO_ATTR));
							w->attr_label = PACK_LABEL(hx, "");
						}

						hx = PACK_HBOX(vx);
						{
							PACK_LABEL(hx, GET_LABEL(l, DATA_DISK_INFO_NR));
							w->num_label = PACK_LABEL(hx, "");
							q8tk_misc_set_placement(w->num_label,
													Q8TK_PLACEMENT_X_RIGHT, 0);
						}
					}

					f = PACK_FRAME(vbox,
								   GET_LABEL(data_disk_info_drive, i), vx);
					q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_IN);
				}

				/* 以下、プッシュボタンやチェックボタン */
				hx = PACK_HBOX(vbox);
				{
					PACK_BUTTON(hx,
								GET_LABEL(data_disk_fname, DATA_DISK_FNAME),
								cb_disk_fname, NULL);
				}

				PACK_CHECK_BUTTON(vbox,
								  GET_LABEL(data_disk_dispswap,
											DATA_DISK_DISPSWAP),
								  get_disk_dispswap(),
								  cb_disk_dispswap, NULL);

				PACK_CHECK_BUTTON(vbox,
								  GET_LABEL(data_disk_dispstatus, 0),
								  get_disk_dispstatus(),
								  cb_disk_dispstatus, NULL);

#if 0
				/* 位置調整のためダミーを何個か */
				for (i = 0; i < 1; i++) {
					PACK_LABEL(vbox, "");
				}
#endif

				PACK_BUTTON(vbox,
							GET_LABEL(data_disk_image, DATA_DISK_IMAGE_BLANK),
							cb_disk_blank, NULL);
			}
		}
	}

	/* イメージ選択用リストボックスのリスト生成、空ラベルへのデータセット */
	set_disk_widget();

	return hbox;
}
