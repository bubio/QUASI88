/************************************************************************/
/*                                                                      */
/* この QUASI88 メニュー用 Tool Kit のAPIは、どうも似た処理の繰り返しが */
/* 多くなるので、似たような処理をまとめた関数を作ってみた。             */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "menu.h"

#include "q8tk.h"

#include "menu-common.h"
#include "menu-common-message.h"




/* ------------------------------------------------------------------------
 * フレームを生成する
 *
 * box      != NULL なら、これに PACK する。
 * label    フレームのラベル
 * widget   != NULL なら、これを乗せる。
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_FRAME(Q8tkWidget *box,
					   const char *label, Q8tkWidget *widget)
{
	Q8tkWidget *frame = q8tk_frame_new(label);

	if (widget) {
		q8tk_container_add(frame, widget);
	}

	q8tk_widget_show(frame);
	if (box) {
		q8tk_box_pack_start(box, frame);
	}

	return frame;
}


/* ------------------------------------------------------------------------
 * HBOXを生成する
 *
 * box      != NULL なら、これに PACK する。
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_HBOX(Q8tkWidget *box)
{
	Q8tkWidget *hbox = q8tk_hbox_new();

	q8tk_widget_show(hbox);
	if (box) {
		q8tk_box_pack_start(box, hbox);
	}

	return hbox;
}


/* ------------------------------------------------------------------------
 * VBOXを生成する
 *
 * box      != NULL なら、これに PACK する。
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_VBOX(Q8tkWidget *box)
{
	Q8tkWidget *vbox = q8tk_vbox_new();

	q8tk_widget_show(vbox);
	if (box) {
		q8tk_box_pack_start(box, vbox);
	}

	return vbox;
}


/* ------------------------------------------------------------------------
 * LABEL を生成する
 *
 * box      != NULL なら、これに PACK する。
 * label    ラベル
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_LABEL(Q8tkWidget *box, const char *label)
{
	Q8tkWidget *labelwidget = q8tk_label_new(label);

	q8tk_widget_show(labelwidget);
	if (box) {
		q8tk_box_pack_start(box, labelwidget);
	}

	return labelwidget;
}


/* ------------------------------------------------------------------------
 * VSEPATATOR を生成する
 *
 * box      != NULL なら、これに PACK する。
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_VSEP(Q8tkWidget *box)
{
	Q8tkWidget *vsep = q8tk_vseparator_new();

	q8tk_widget_show(vsep);
	if (box) {
		q8tk_box_pack_start(box, vsep);
	}

	return vsep;
}


/* ------------------------------------------------------------------------
 * HSEPATATOR を生成する
 *
 * box      != NULL なら、これに PACK する。
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_HSEP(Q8tkWidget *box)
{
	Q8tkWidget *hsep = q8tk_hseparator_new();

	q8tk_widget_show(hsep);
	if (box) {
		q8tk_box_pack_start(box, hsep);
	}

	return hsep;
}


/* ------------------------------------------------------------------------
 * ボタンを生成する
 *
 * box      != NULL なら、これに PACK する。
 * label    ラベル
 * callback "clicked" 時のコールバック関数
 * parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_BUTTON(Q8tkWidget *box,
						const char *label,
						Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *button = q8tk_button_new_with_label(label);

	q8tk_signal_connect(button, "clicked", callback, parm);
	q8tk_widget_show(button);
	if (box) {
		q8tk_box_pack_start(box, button);
	}

	return button;
}



/* ------------------------------------------------------------------------
 * チェックボタンを生成する
 *
 * box      != NULL なら、これに PACK する。
 * label    ラベル
 * on       真なら、チェック状態とする
 * callback "clicked" 時のコールバック関数
 * parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_CHECK_BUTTON(Q8tkWidget *box,
							  const char *label, int on,
							  Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *button = q8tk_check_button_new_with_label(label);

	if (on) {
		q8tk_toggle_button_set_state(button, TRUE);
	}

	q8tk_signal_connect(button, "toggled", callback, parm);
	q8tk_widget_show(button);
	if (box) {
		q8tk_box_pack_start(box, button);
	}

	return button;
}


/* ------------------------------------------------------------------------
 * ラジオボタンを生成する
 *
 * box      != NULL なら、これに PACK する。
 * button   グループを形成するボタン
 * label    ラベル
 * on       真なら、チェック状態とする
 * callback "clicked" 時のコールバック関数
 * parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_RADIO_BUTTON(Q8tkWidget *box,
							  Q8tkWidget *button,
							  const char *label, int on,
							  Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *b = q8tk_radio_button_new_with_label(button, label);

	q8tk_widget_show(b);
	q8tk_signal_connect(b, "clicked", callback, parm);

	if (on) {
		q8tk_toggle_button_set_state(b, TRUE);
	}

	if (box) {
		q8tk_box_pack_start(box, b);
	}

	return b;
}


/* ------------------------------------------------------------------------
 * コンボボックスを生成する
 *
 * box      != NULL なら、これに PACK する。
 * p        t_menudata 型配列の先頭ポインタ。
 * count    その配列の数。
 *          p[0] .. p[count-1] までのデータの文字列をコンボボックス化する。
 * initval  p[].val == initval の場合、その文字列を初期文字列とする。
 * initstr  上記該当が無い場合の初期文字列
 * width    表示サイズ。0で自動
 * act_callback "activate"時のコールバック関数
 * act_parm     そのパラメータ
 * chg_callback "changed"時のコールバック関数
 * chg_parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_COMBO(Q8tkWidget *box,
					   const t_menudata *p, int count,
					   int initval, const char *initstr, int width,
					   Q8tkSignalFunc act_callback, void *act_parm,
					   Q8tkSignalFunc chg_callback, void *chg_parm)
{
	int i;
	Q8tkWidget *combo = q8tk_combo_new();

	for (i = 0; i < count; i++, p++) {
		q8tk_combo_append_popdown_strings(combo, p->str[menu_lang], NULL);

		if (initval == p->val) {
			initstr = p->str[menu_lang];
		}
	}

	q8tk_combo_set_text(combo, initstr ? initstr : " ");
	q8tk_signal_connect(combo, "activate", act_callback, act_parm);
	if (chg_callback) {
		q8tk_combo_set_editable(combo, TRUE);
		q8tk_signal_connect(combo, "changed",  chg_callback, chg_parm);
	}
	q8tk_widget_show(combo);

	if (width) {
		q8tk_misc_set_size(combo, width, 0);
	}

	if (box) {
		q8tk_box_pack_start(box, combo);
	}

	return combo;
}


/* ------------------------------------------------------------------------
 * エントリーを生成する
 *
 * box      != NULL なら、これに PACK する。
 * length   入力文字列長。0で無限
 * width    表示文字列長。0で自動
 * text     初期文字列
 * act_callback "activate"時のコールバック関数
 * act_parm     そのパラメータ
 * chg_callback "changed"時のコールバック関数
 * chg_parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_ENTRY(Q8tkWidget *box,
					   int length, int width, const char *text,
					   Q8tkSignalFunc act_callback, void *act_parm,
					   Q8tkSignalFunc chg_callback, void *chg_parm)
{
	Q8tkWidget *e;

	e = q8tk_entry_new_with_max_length(length);

	if (width) {
		q8tk_misc_set_size(e, width, 1);
	}

	if (text) {
		q8tk_entry_set_text(e, text);
	}

	if (act_callback) {
		q8tk_signal_connect(e, "activate", act_callback, act_parm);
	}
	if (chg_callback) {
		q8tk_signal_connect(e, "changed",  chg_callback, chg_parm);
	}

	q8tk_misc_set_placement(e, 0, Q8TK_PLACEMENT_Y_CENTER);
	q8tk_widget_show(e);

	if (box) {
		q8tk_box_pack_start(box, e);
	}

	return e;
}


/*======================================================================*/

/* ------------------------------------------------------------------------
 * チェックボタンを複数生成する
 *
 * box      これに PACK する。(NULLは禁止)
 * p        t_menudata 型配列の先頭ポインタ。
 * count    その配列の数。
 *          p[0] .. p[count-1] までのデータの文字列でチェックボタンを生成する。
 * f_initval 関数 (*f_initval)(p[].val) が真なら、チェック状態とする
 * callback "toggled"時のコールバック関数 パラメータは (void*)(p[].val)
 * ------------------------------------------------------------------------ */
void PACK_CHECK_BUTTONS(Q8tkWidget *box,
						const t_menudata *p, int count,
						int (*f_initval)(int),
						Q8tkSignalFunc callback)
{
	int i;
	Q8tkWidget *button;

	for (i = 0; i < count; i++, p++) {

		button = q8tk_check_button_new_with_label(p->str[menu_lang]);

		if ((*f_initval)(p->val)) {
			q8tk_toggle_button_set_state(button, TRUE);
		}

		q8tk_signal_connect(button, "toggled", callback, INT2P(p->val));

		q8tk_widget_show(button);
		q8tk_box_pack_start(box, button);
	}
}


/* ------------------------------------------------------------------------
 * ラジオボタンを複数生成する
 *
 * box      これに PACK する。(NULLは禁止)
 * p        t_menudata 型配列の先頭ポインタ。
 * count    その配列の数。
 *          p[0] .. p[count-1] までのデータの文字列でラジオボタンを生成する。
 * initval  p[].val == initval ならば、そのボタンをON状態とする
 * callback "clicked"時のコールバック関数 パラメータは (void*)(p[].val)
 * ------------------------------------------------------------------------ */
Q8List *PACK_RADIO_BUTTONS(Q8tkWidget *box,
						   const t_menudata *p, int count,
						   int initval, Q8tkSignalFunc callback)
{
	int i;
	Q8tkWidget *button = NULL;

	for (i = 0; i < count; i++, p++) {

		button = q8tk_radio_button_new_with_label(button, p->str[menu_lang]);

		q8tk_widget_show(button);
		q8tk_box_pack_start(box, button);
		q8tk_signal_connect(button, "clicked", callback, INT2P(p->val));

		if (initval == p->val) {
			q8tk_toggle_button_set_state(button, TRUE);
		}
	}
	return q8tk_radio_button_get_list(button);
}


/* ------------------------------------------------------------------------
 * HSCALEを生成する
 *
 * box      != NULL なら、これに PACK する。
 * p        t_volume 型配列の先頭ポインタ。この情報をもとにHSCALEを生成。
 * initval  初期値
 * callback "value_changed"時のコールバック関数
 * parm     そのパラメータ
 * ------------------------------------------------------------------------ */
Q8tkWidget *PACK_HSCALE(Q8tkWidget *box,
						const t_volume *p,
						int initval,
						Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *scale;

	scale = q8tk_hscale_new(NULL);

	q8tk_adjustment_clamp_page(scale->stat.scale.adj, p->min, p->max);
	q8tk_adjustment_set_increment(scale->stat.scale.adj, p->step, p->page);
	q8tk_adjustment_set_value(scale->stat.scale.adj, initval);

	q8tk_signal_connect(scale->stat.scale.adj, "value_changed", callback, parm);

	q8tk_adjustment_set_arrow(scale->stat.scale.adj, TRUE);
	/* q8tk_adjustment_set_length(scale->stat.scale.adj, 11); */
	q8tk_scale_set_draw_value(scale, TRUE);
	q8tk_scale_set_value_pos(scale, Q8TK_POS_LEFT);

	q8tk_widget_show(scale);

	if (box) {
		q8tk_box_pack_start(box, scale);
	}

	return scale;
}



/* ------------------------------------------------------------------------
 * キーアサイン変更用ウィジットを生成する
 * ------------------------------------------------------------------------ */
Q8tkWidget *MAKE_KEY_COMBO(Q8tkWidget *box,
						   const t_menudata *p,
						   int (*f_initval)(int),
						   Q8tkSignalFunc callback)
{
	{
		Q8tkWidget *label = q8tk_label_new(GET_LABEL(p, 0));
		q8tk_box_pack_start(box, label);
		q8tk_widget_show(label);
	}
	{
		int i;
		const t_keymap *k = keymap_assign;
		const char     *initstr = " ";
		int             initval = (*f_initval)(p->val);

		Q8tkWidget *combo = q8tk_combo_new();

		for (i = 0; i < COUNTOF(keymap_assign); i++, k++) {
			q8tk_combo_append_popdown_strings(combo, k->str, NULL);

			if (initval == k->code) {
				initstr = k->str;
			}
		}

		q8tk_combo_set_text(combo, initstr);
		q8tk_misc_set_size(combo, 6, 0);
		q8tk_signal_connect(combo, "activate", callback, INT2P(p->val));

		q8tk_box_pack_start(box, combo);
		q8tk_widget_show(combo);

		return combo;
	}
}

Q8tkWidget *PACK_KEY_ASSIGN(Q8tkWidget *box,
							const t_menudata *p, int count,
							int (*f_initval)(int),
							Q8tkSignalFunc callback)
{
	int i;
	Q8tkWidget *vbox, *hbox, *allbox;

	vbox = q8tk_vbox_new();
	{
		/* combo */
		{
			hbox = q8tk_hbox_new();
			{
				MAKE_KEY_COMBO(hbox, p, f_initval, callback);
				p++;
			}
			q8tk_widget_show(hbox);
			q8tk_box_pack_start(vbox, hbox);
		}

		/* ↑ */
		PACK_LABEL(vbox, GET_LABEL(p, 0));
		p++;

		/* combo ← → combo */
		{
			hbox = q8tk_hbox_new();
			{
				MAKE_KEY_COMBO(hbox, p, f_initval, callback);
				p++;
				MAKE_KEY_COMBO(hbox, p, f_initval, callback);
				p++;
			}
			q8tk_widget_show(hbox);
			q8tk_box_pack_start(vbox, hbox);
		}

		/* ↓ */
		PACK_LABEL(vbox, GET_LABEL(p, 0));
		p++;

		/* combo */
		{
			hbox = q8tk_hbox_new();
			{
				MAKE_KEY_COMBO(hbox, p, f_initval, callback);
				p++;
			}
			q8tk_widget_show(hbox);
			q8tk_box_pack_start(vbox, hbox);
		}
	}
	q8tk_widget_show(vbox);


	if (count < 6) {
		/* 方向キーだけで処理終わり */

		allbox = vbox;

	} else {
		/* 他にも処理するキーあり */

		allbox = q8tk_hbox_new();
		q8tk_box_pack_start(allbox, vbox);

		{
			vbox = q8tk_vbox_new();
			for (i = 6; i < count; i++) {
				if (p->val < 0) {
					PACK_LABEL(vbox, GET_LABEL(p, 0));
					p++;
				} else {
					hbox = q8tk_hbox_new();
					{
						MAKE_KEY_COMBO(hbox, p, f_initval, callback);
						p++;
					}
					q8tk_widget_show(hbox);
					q8tk_box_pack_start(vbox, hbox);
				}
			}
			q8tk_widget_show(vbox);
			q8tk_box_pack_start(allbox, vbox);
		}

		q8tk_widget_show(allbox);
	}


	if (box) {
		q8tk_box_pack_start(box, allbox);
	}

	return allbox;
}


/*======================================================================*/

/* ------------------------------------------------------------------------
 * ファイルセレクションを生成する
 *
 * 生成後、ウインドウをグラブする
 * CANCEL時は、なにも処理せずにグラブを離す。
 * OK 時は、 (*ok_button)()を呼び出す。この時、ファイル名は
 * get_filename に、リードオンリー属性は get_ro にセットされている。
 * なお、呼び出した時点ではすでにグラブを離す。
 *
 * label            ラベル
 * select_ro        >=0なら、ReadOnly選択可(1でチェック)
 * filename         初期ファイル(ディレクトリ)名
 * ok_button        OK時に呼び出す関数
 * get_filename     OK時にファイル名をこのバッファにセット
 * sz_get_filename  このバッファのサイズ
 * get_ro           select_ro が >=0 の時、リードオンリー選択情報がここにセット
 * ------------------------------------------------------------------------ */
static struct {
	void		(*ok_button)(void);		/* OK押下時の呼び出す関数	*/
	char		*get_filename;			/* 選択したファイル名格納先	*/
	int			sz_get_filename;		/* そのバッファサイズ		*/
	int			*get_ro;				/* RO かどうかのフラグ		*/
	Q8tkWidget	*accel;
} FSEL;
static void cb_fsel_ok(UNUSED_WIDGET, Q8tkWidget *f);
static void cb_fsel_cancel(UNUSED_WIDGET, Q8tkWidget *f);

void START_FILE_SELECTION(const char *label,		/* タイトル			*/
						  int select_ro,			/* RO選択状態		*/
						  const char *filename,		/* 初期ファイル名	*/
						  void (*ok_button)(void),
						  char *get_filename,
						  int  sz_get_filename,
						  int  *get_ro)
{
	Q8tkWidget *f;

	f = q8tk_file_selection_new(label, select_ro);
	q8tk_widget_show(f);
	q8tk_grab_add(f);

	if (filename) {
		q8tk_file_selection_set_filename(f, filename);
	}

	q8tk_signal_connect(Q8TK_FILE_SELECTION(f)->ok_button,
						"clicked", cb_fsel_ok, f);
	q8tk_signal_connect(Q8TK_FILE_SELECTION(f)->cancel_button,
						"clicked", cb_fsel_cancel, f);
	q8tk_widget_set_focus(Q8TK_FILE_SELECTION(f)->cancel_button);

	FSEL.ok_button       = ok_button;
	FSEL.get_filename    = get_filename;
	FSEL.sz_get_filename = sz_get_filename;
	FSEL.get_ro          = (select_ro >= 0) ? get_ro : NULL;

	FSEL.accel = q8tk_accel_group_new();

	q8tk_accel_group_attach(FSEL.accel, f);
	q8tk_accel_group_add(FSEL.accel, Q8TK_KEY_ESC,
						 Q8TK_FILE_SELECTION(f)->cancel_button, "clicked");
}


static void cb_fsel_cancel(UNUSED_WIDGET, Q8tkWidget *f)
{
	q8tk_grab_remove(f);
	q8tk_widget_destroy(f);
	q8tk_widget_destroy(FSEL.accel);
}

static void cb_fsel_ok(UNUSED_WIDGET, Q8tkWidget *f)
{
	*FSEL.get_filename = '\0';
	strncat(FSEL.get_filename, q8tk_file_selection_get_filename(f),
			FSEL.sz_get_filename - 1);

	if (FSEL.get_ro) {
		*FSEL.get_ro = q8tk_file_selection_get_readonly(f);
	}

	q8tk_grab_remove(f);
	q8tk_widget_destroy(f);
	q8tk_widget_destroy(FSEL.accel);

	if (FSEL.ok_button) {
		(*FSEL.ok_button)();
	}
}


/*======================================================================*/

/* ------------------------------------------------------------------------
 * ダイアログをを生成する
 *
 * ダイアログは、以下の構成とする
 * +-----------------------------------+
 * |               見出し 1            |   見出しラベル   (1個以上)
 * |                 ：                |
 * |               見出し 2            |
 * |                 ：                |
 * |          [チェックボタン]         |   チェックボタン (1個以上)
 * | --------------------------------- |   セパレータ     (1個以上)
 * | [エントリ] [ボタン] …… [ボタン] |   エントリ       (最大1個)
 * +-----------------------------------+   ボタン         (1個以上)
 *
 * ラベル、セパレータ、ボタン、エントリは全部合わせて最大で、 DIA_MAX 個まで。
 * 最後に追加したウィジット (ボタンかエントリ) にフォーカスがくる。
 * ------------------------------------------------------------------------ */

static Q8tkWidget *dialog_last_widget;
static Q8tkWidget *dialog_main;
static Q8tkWidget *dialog_entry;
static Q8tkWidget *dialog_accel;


/* ダイアログ作成開始 */

void dialog_create(void)
{
	Q8tkWidget *d = q8tk_dialog_new();
	Q8tkWidget *a = q8tk_accel_group_new();

	q8tk_misc_set_placement(Q8TK_DIALOG(d)->action_area,
							Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_CENTER);

	q8tk_accel_group_attach(a, d);


	dialog_last_widget = NULL;
	dialog_entry = NULL;

	dialog_accel = a;
	dialog_main  = d;
}

/* ダイアログにラベル（見出し）を追加。複数個、追加できる */

void dialog_set_title(const char *label)
{
	Q8tkWidget *l = q8tk_label_new(label);

	q8tk_box_pack_start(Q8TK_DIALOG(dialog_main)->content_area, l);
	q8tk_widget_show(l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
}

/* ダイアログにチェックボタンを追加 (引数…ボタン名,状態,コールバック関数) */

void dialog_set_check_button(const char *label, int on,
							 Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *b = q8tk_check_button_new_with_label(label);

	if (on) {
		q8tk_toggle_button_set_state(b, TRUE);
	}

	q8tk_box_pack_start(Q8TK_DIALOG(dialog_main)->content_area, b);
	q8tk_widget_show(b);
	q8tk_signal_connect(b, "toggled", callback, parm);

	dialog_last_widget = b;
}

/* ダイアログにセパレータを追加。 */

void dialog_set_separator(void)
{
#if 0
	Q8tkWidget *s = q8tk_hseparator_new();
#else
	Q8tkWidget *s = q8tk_label_new("");
#endif

	q8tk_box_pack_start(Q8TK_DIALOG(dialog_main)->content_area, s);
	q8tk_widget_show(s);
}

/* ダイアログにボタンを追加 (引数…ボタンの名称,コールバック関数) */

void dialog_set_button(const char *label,
					   Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *b = q8tk_button_new_with_label(label);

	q8tk_box_pack_start(Q8TK_DIALOG(dialog_main)->action_area, b);
	q8tk_widget_show(b);
	q8tk_signal_connect(b, "clicked", callback, parm);

	dialog_last_widget = b;
}

/* ダイアログにエントリを追加 (引数…初期文字列,最大文字数,コールバック関数) */

void dialog_set_entry(const char *text, int max_length,
					  Q8tkSignalFunc callback, void *parm)
{
	Q8tkWidget *e = q8tk_entry_new_with_max_length(max_length);

	q8tk_box_pack_start(Q8TK_DIALOG(dialog_main)->action_area, e);
	q8tk_widget_show(e);
	q8tk_signal_connect(e, "activate", callback, parm);
	q8tk_misc_set_size(e, max_length + 1, 0);
	q8tk_misc_set_placement(e, 0, Q8TK_PLACEMENT_Y_CENTER);
	q8tk_entry_set_text(e, text);

	dialog_entry = e;

	dialog_last_widget = e;
}

/* ダイアログ内の、エントリの文字列をとり出す */

const char *dialog_get_entry(void)
{
	return q8tk_entry_get_text(dialog_entry);
}

/* 直前に追加したダイアログのボタンに、ショートカットキーを設定 */

void dialog_accel_key(int key)
{
	q8tk_accel_group_add(dialog_accel, key, dialog_last_widget, "clicked");
}

/* ダイアログ表示開始 (グラブされる。フォーカスは最後に追加したボタンへ) */

void dialog_start(void)
{
	q8tk_widget_show(dialog_main);
	q8tk_grab_add(dialog_main);

	if (dialog_last_widget) {
		q8tk_widget_set_focus(dialog_last_widget);
	}
}

/* ダイアログを消去 (ウインドウを消去し、グラブを解除する) */

void dialog_destroy(void)
{
	q8tk_grab_remove(dialog_main);
	q8tk_widget_destroy_all_related(dialog_main);

	dialog_main  = NULL;
	dialog_entry = NULL;
	dialog_accel = NULL;
	dialog_last_widget = NULL;
}


/*===========================================================================
 * ファイル操作エラーメッセージのダイアログ処理
 *===========================================================================*/
static void cb_file_error_dialog_ok(UNUSED_WIDGET, UNUSED_PARM)
{
	dialog_destroy();
}

void start_file_error_dialog(int drv, int result)
{
	char wk[128];
	const t_menulabel *l = (drv < 0) ? data_err_file : data_err_drive;

	if (result == ERR_NO) {
		return;
	}
	if (drv < 0) {
		sprintf(wk, GET_LABEL(l, result));
	} else {
		sprintf(wk, GET_LABEL(l, result), drv + 1);
	}

	dialog_create();
	{
		dialog_set_title(wk);
		dialog_set_separator();
		dialog_set_button(GET_LABEL(l, ERR_NO),
						  cb_file_error_dialog_ok, NULL);
	}
	dialog_start();
}


/* キー配列変更コンボボックス用 のウィジット生成用データ */

const t_keymap keymap_assign[ SZ_KEYMAP_ASSIGN ] = {
	{ "(none)",		KEY88_INVALID,		},
	{ "0 (10)",		KEY88_KP_0,			},
	{ "1 (10)",		KEY88_KP_1,			},
	{ "2 (10)",		KEY88_KP_2,			},
	{ "3 (10)",		KEY88_KP_3,			},
	{ "4 (10)",		KEY88_KP_4,			},
	{ "5 (10)",		KEY88_KP_5,			},
	{ "6 (10)",		KEY88_KP_6,			},
	{ "7 (10)",		KEY88_KP_7,			},
	{ "8 (10)",		KEY88_KP_8,			},
	{ "9 (10)",		KEY88_KP_9,			},
	{ "* (10)",		KEY88_KP_MULTIPLY,	},
	{ "+ (10)",		KEY88_KP_ADD,		},
	{ "= (10)",		KEY88_KP_EQUAL,		},
	{ ", (10)",		KEY88_KP_COMMA,		},
	{ ". (10)",		KEY88_KP_PERIOD,	},
	{ "- (10)",		KEY88_KP_SUB,		},
	{ "/ (10)",		KEY88_KP_DIVIDE,	},
	{ "A     ",		KEY88_a,			},
	{ "B     ",		KEY88_b,			},
	{ "C     ",		KEY88_c,			},
	{ "D     ",		KEY88_d,			},
	{ "E     ",		KEY88_e,			},
	{ "F     ",		KEY88_f,			},
	{ "G     ",		KEY88_g,			},
	{ "H     ",		KEY88_h,			},
	{ "I     ",		KEY88_i,			},
	{ "J     ",		KEY88_j,			},
	{ "K     ",		KEY88_k,			},
	{ "L     ",		KEY88_l,			},
	{ "M     ",		KEY88_m,			},
	{ "N     ",		KEY88_n,			},
	{ "O     ",		KEY88_o,			},
	{ "P     ",		KEY88_p,			},
	{ "Q     ",		KEY88_q,			},
	{ "R     ",		KEY88_r,			},
	{ "S     ",		KEY88_s,			},
	{ "T     ",		KEY88_t,			},
	{ "U     ",		KEY88_u,			},
	{ "V     ",		KEY88_v,			},
	{ "W     ",		KEY88_w,			},
	{ "X     ",		KEY88_x,			},
	{ "Y     ",		KEY88_y,			},
	{ "Z     ",		KEY88_z,			},
	{ "0     ",		KEY88_0,			},
	{ "1 (!) ",		KEY88_1,			},
	{ "2 (\") ",	KEY88_2,			},
	{ "3 (#) ",		KEY88_3,			},
	{ "4 ($) ",		KEY88_4,			},
	{ "5 (%) ",		KEY88_5,			},
	{ "6 (&) ",		KEY88_6,			},
	{ "7 (') ",		KEY88_7,			},
	{ "8 (() ",		KEY88_8,			},
	{ "9 ()) ",		KEY88_9,			},
	{ ", (<) ",		KEY88_COMMA,		},
	{ "- (=) ",		KEY88_MINUS,		},
	{ ". (>) ",		KEY88_PERIOD,		},
	{ "/ (?) ",		KEY88_SLASH,		},
	{ ": (*) ",		KEY88_COLON,		},
	{ "; (+) ",		KEY88_SEMICOLON,	},
	{ "@ (~) ",		KEY88_AT,			},
	{ "[ ({) ",		KEY88_BRACKETLEFT,	},
	{ "\\ (|) ",	KEY88_YEN,			},
	{ "] (}) ",		KEY88_BRACKETRIGHT,	},
	{ "^     ",		KEY88_CARET,		},
	{ "  (_) ",		KEY88_UNDERSCORE,	},
	{ "space ",		KEY88_SPACE,		},
	{ "RETURN",		KEY88_RETURN,		},
	{ "SHIFT ",		KEY88_SHIFT,		},
	{ "CTRL  ",		KEY88_CTRL,			},
	{ "CAPS  ",		KEY88_CAPS,			},
	{ "kana  ",		KEY88_KANA,			},
	{ "GRPH  ",		KEY88_GRAPH,		},
	{ "HM-CLR",		KEY88_HOME,			},
	{ "HELP  ",		KEY88_HELP,			},
	{ "DELINS",		KEY88_INS_DEL,		},
	{ "STOP  ",		KEY88_STOP,			},
	{ "COPY  ",		KEY88_COPY,			},
	{ "ESC   ",		KEY88_ESC,			},
	{ "TAB   ",		KEY88_TAB,			},
	{ "\036     ",	KEY88_UP,			},
	{ "\037     ",	KEY88_DOWN,			},
	{ "\035     ",	KEY88_LEFT,			},
	{ "\034     ",	KEY88_RIGHT,		},
	{ "ROLLUP",		KEY88_ROLLUP,		},
	{ "ROLLDN",		KEY88_ROLLDOWN,		},
	{ "f1    ",		KEY88_F1,			},
	{ "f2    ",		KEY88_F2,			},
	{ "f3    ",		KEY88_F3,			},
	{ "f4    ",		KEY88_F4,			},
	{ "f5    ",		KEY88_F5,			},
#if 1
	{ "f6    ",		KEY88_F6,			},
	{ "f7    ",		KEY88_F7,			},
	{ "f8    ",		KEY88_F8,			},
	{ "f9    ",		KEY88_F9,			},
	{ "f10   ",		KEY88_F10,			},
	{ "BS    ",		KEY88_BS,			},
	{ "INS   ",		KEY88_INS,			},
	{ "DEL   ",		KEY88_DEL,			},
	{ "henkan",		KEY88_HENKAN,		},
	{ "kettei",		KEY88_KETTEI,		},
	{ "PC    ",		KEY88_PC,			},
	{ "zenkak",		KEY88_ZENKAKU,		},
	{ "RET  L",		KEY88_RETURNL,		},
	{ "RET  R",		KEY88_RETURNR,		},
	{ "SHIFTL",		KEY88_SHIFTL,		},
	{ "SHIFTR",		KEY88_SHIFTR,		},
#endif
};
