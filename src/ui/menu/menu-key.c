/*===========================================================================
 *
 * メインページ キー設定
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "menu.h"

#include "memory.h"

#include "q8tk.h"

#include "menu-common.h"

#include "menu-key.h"
#include "menu-key-message.h"


static Q8tkWidget *keymap[103];
static int         keymap_num;
static Q8tkWidget *keymap_window_base;
static Q8tkWidget *keymap_window_focus;

/*----------------------------------------------------------------------*/
static Q8tkWidget *fkey_widget[20 + 1][2];

/* ファンクションキー設定 */
static int get_key_fkey(int fn_key)
{
	return (function_f[ fn_key ] < 0x20) ? function_f[ fn_key ] : FN_FUNC;
}
static void cb_key_fkey(Q8tkWidget *widget, void *fn_key)
{
	int i;
	const t_menudata *p = data_key_fkey_fn;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_key_fkey_fn); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			function_f[ P2INT(fn_key) ] = p->val;
			q8tk_combo_set_text(fkey_widget[P2INT(fn_key)][1],
								keymap_assign[0].str);
			return;
		}
	}
}

static int get_key_fkey2(int fn_key)
{
	return
		(function_f[ fn_key ] >= 0x20) ? function_f[ fn_key ] : KEY88_INVALID;
}
static void cb_key_fkey2(Q8tkWidget *widget, void *fn_key)
{
	int i;
	const t_keymap *q = keymap_assign;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(keymap_assign); i++, q++) {
		if (strcmp(q->str, combo_str) == 0) {
			function_f[ P2INT(fn_key) ] = q->code;
			q8tk_combo_set_text(fkey_widget[P2INT(fn_key)][0],
								data_key_fkey_fn[0].str[menu_lang]);
			return;
		}
	}
}


static Q8tkWidget *menu_key_fkey(void)
{
	int i;
	Q8tkWidget *vbox, *hbox;
	const t_menudata *p = data_key_fkey;

	vbox = PACK_VBOX(NULL);
	{
		for (i = 0; i < COUNTOF(data_key_fkey); i++, p++) {

			hbox = PACK_HBOX(vbox);
			{
				PACK_LABEL(hbox, GET_LABEL(p, 0));

				fkey_widget[p->val][0] =
					PACK_COMBO(hbox,
							   data_key_fkey_fn, COUNTOF(data_key_fkey_fn),
							   get_key_fkey(p->val), NULL, 42,
							   cb_key_fkey, INT2P(p->val),
							   NULL, NULL);

				fkey_widget[p->val][1] =
					MAKE_KEY_COMBO(hbox,
								   &data_key_fkey2[i],
								   get_key_fkey2,
								   cb_key_fkey2);
			}
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* テンキー、NUM Lock の変更 */
static int get_key_cfg(int type)
{
	switch (type) {

	case DATA_KEY_CFG_TENKEY:
		return  tenkey_emu;

	case DATA_KEY_CFG_NUMLOCK:
		return  quasi88_now_key_numlock();
	}
	return FALSE;
}
static void cb_key_cfg(Q8tkWidget *widget, void *type)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	switch (P2INT(type)) {

	case DATA_KEY_CFG_TENKEY:
		tenkey_emu  = (key) ? TRUE : FALSE;
		break;

	case DATA_KEY_CFG_NUMLOCK:
		quasi88_cfg_key_numlock((key) ? TRUE : FALSE);
		break;
	}
}


static Q8tkWidget *menu_key_cfg(void)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(NULL);
	{
		PACK_CHECK_BUTTONS(vbox,
						   data_key_cfg, COUNTOF(data_key_cfg),
						   get_key_cfg, cb_key_cfg);
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* ソフトウェアキーボード */
static void keymap_create(void);
static void keymap_start(void);
static void keymap_finish(void);

static void cb_key_softkeyboard(UNUSED_WIDGET, UNUSED_PARM)
{
	keymap_start();
}

static Q8tkWidget *menu_key_softkeyboard(void)
{
	Q8tkWidget *button;
	const t_menulabel *l = data_skey_set;

	button = PACK_BUTTON(NULL,
						 GET_LABEL(l, DATA_SKEY_BUTTON_SETUP),
						 cb_key_softkeyboard, NULL);

	return button;
}



/*----------------------------------------------------------------------*/
/* カーソルキー設定 */
/* original idea by floi, thanks ! */

static void menu_key_cursor_setting(void);

static int         key_cursor_widget_init_done;
static Q8tkWidget *key_cursor_widget_sel;
static Q8tkWidget *key_cursor_widget_sel_none;
static Q8tkWidget *key_cursor_widget_sel_key;

static int get_key_cursor_key_mode(void)
{
	return cursor_key_mode;
}
static void cb_key_cursor_key_mode(UNUSED_WIDGET, void *p)
{
	cursor_key_mode = P2INT(p);

	menu_key_cursor_setting();
}
static int get_key_cursor_key(int type)
{
	return cursor_key_assign[ type ];
}
static void cb_key_cursor_key(Q8tkWidget *widget, void *type)
{
	int i;
	const t_keymap *q = keymap_assign;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(keymap_assign); i++, q++) {
		if (strcmp(q->str, combo_str) == 0) {
			cursor_key_assign[ P2INT(type) ] = q->code;
			return;
		}
	}
}


static Q8tkWidget *menu_key_cursor(void)
{
	int i;
	Q8tkWidget *hbox, *vbox;

	key_cursor_widget_init_done = FALSE;

	hbox = PACK_HBOX(NULL);
	{
		{
			/* キー割り当て選択のラジオボタン */
			vbox = PACK_VBOX(hbox);
			PACK_RADIO_BUTTONS(vbox,
							   data_key_cursor_mode,
							   COUNTOF(data_key_cursor_mode),
							   get_key_cursor_key_mode(),
							   cb_key_cursor_key_mode);

			/* 位置調整のダミー */
			for (i = 0; i < 2; i++) {
				PACK_LABEL(vbox, "");
			}

			key_cursor_widget_sel = vbox;
		}

		/* 区切り棒 */
		PACK_VSEP(hbox);

		{
			/* 標準、2468を割り当て、の右画面 */
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, GET_LABEL(data_key, DATA_KEY_CURSOR_SPACING));

			key_cursor_widget_sel_none = vbox;
		}

		{
			/* 任意のキーをり当て、の右画面 */
			key_cursor_widget_sel_key =
				PACK_KEY_ASSIGN(hbox,
								data_key_cursor, COUNTOF(data_key_cursor),
								get_key_cursor_key, cb_key_cursor_key);
		}
	}

	key_cursor_widget_init_done = TRUE;
	menu_key_cursor_setting();

	return hbox;
}

static  void    menu_key_cursor_setting(void)
{
	if (key_cursor_widget_init_done == FALSE) {
		return;
	}

	{
		q8tk_widget_show(key_cursor_widget_sel);
		q8tk_widget_hide(key_cursor_widget_sel_none);
		q8tk_widget_hide(key_cursor_widget_sel_key);

		switch (cursor_key_mode) {
		case 0:
		case 1:
			q8tk_widget_show(key_cursor_widget_sel_none);
			break;
		case 2:
			q8tk_widget_show(key_cursor_widget_sel_key);
			break;
		}
	}
}

/*----------------------------------------------------------------------*/

Q8tkWidget *menu_key(void)
{
	Q8tkWidget *vbox, *hbox, *vbox2, *w;
	const t_menulabel *l = data_key;

	vbox = PACK_VBOX(NULL);
	{
		PACK_FRAME(vbox, GET_LABEL(l, DATA_KEY_FKEY), menu_key_fkey());

		hbox = PACK_HBOX(vbox);
		{
			PACK_FRAME(hbox, GET_LABEL(l, DATA_KEY_CURSOR), menu_key_cursor());

			vbox2 = PACK_VBOX(NULL);
			{
				PACK_LABEL(vbox2, GET_LABEL(l, DATA_KEY_SKEY2));

				w = menu_key_softkeyboard();
				q8tk_box_pack_start(vbox2, w);
				q8tk_misc_set_placement(w, Q8TK_PLACEMENT_X_CENTER,
										Q8TK_PLACEMENT_Y_CENTER);

				/* 位置調整のダミー */
				PACK_LABEL(vbox2, "");
			}
			PACK_FRAME(hbox, GET_LABEL(l, DATA_KEY_SKEY), vbox2);
		}

		w = PACK_FRAME(vbox, "", menu_key_cfg());
		q8tk_frame_set_shadow_type(w, Q8TK_SHADOW_NONE);
	}

	keymap_window_base = NULL;

	return vbox;
}


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 *
 * サブウインドウ  ソフトウェアキーボード
 *
 * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

/*----------------------------------------------------------------------*/

static int get_key_softkey(int code)
{
	return softkey_is_pressed(code);
}
static void cb_key_softkey(Q8tkWidget *button, void *code)
{
	if (Q8TK_TOGGLE_BUTTON(button)->active) {
		softkey_press(P2INT(code));
	} else {
		softkey_release(P2INT(code));
	}
}

static void cb_key_softkey_release(UNUSED_WIDGET, UNUSED_PARM)
{
	/* 全てのキーを離した状態にする */
	int i;
	for (i = 0; i < keymap_num; i++) {
		q8tk_toggle_button_set_state(keymap[i], FALSE);
	}
	softkey_release_all();

	/* ソフトウェアキーを閉じる */
	keymap_finish();
}

static void cb_key_softkey_end(UNUSED_WIDGET, UNUSED_PARM)
{
	/* 複数キー同時押し時のハードバグを再現 */
	softkey_bug();

	/* ソフトウェアキーを閉じる */
	keymap_finish();
}


/* ソフトウフェアキーボード ウインドウ生成・表示 */

static void keymap_create(void)
{
	Q8tkWidget *w, *a, *v, *s, *l, *h, *b1, *b2, *vx, *hx, *n;
	int i, j;
	int model = (ROM_VERSION < '8') ? 0 : 1;

    for (i = 0; i < COUNTOF(keymap); i++) {
		keymap[i] = NULL;
	}
	keymap_num = 0;

	/* メインとなるウインドウ */
	{
		w = q8tk_window_new(Q8TK_WINDOW_DIALOG);
		a = q8tk_accel_group_new();
		q8tk_accel_group_attach(a, w);
	}

	/* に、ボックスを乗せる */
	{
		v = q8tk_vbox_new();
		q8tk_container_add(w, v);
		q8tk_widget_show(v);
	}

	{
		/* ボックスには     */
		{
			/* スクロール付 WIN */
			s = q8tk_scrolled_window_new(NULL, NULL);
			q8tk_box_pack_start(v, s);
			q8tk_misc_set_size(s, 80, 21);
			q8tk_scrolled_window_set_policy(s,
											Q8TK_POLICY_AUTOMATIC,
											Q8TK_POLICY_NEVER);
			q8tk_widget_show(s);
		}
		{
			/* 見栄えのための空行*/
			l = q8tk_label_new("");
			q8tk_box_pack_start(v, l);
			q8tk_widget_show(l);
		}
		{
			/* ボタン配置用 HBOX */
			h = q8tk_hbox_new();
			q8tk_box_pack_start(v, h);
			q8tk_misc_set_placement(h, Q8TK_PLACEMENT_X_CENTER, 0);
			q8tk_widget_show(h);

			{
				/* HBOXには */
				const t_menulabel *l = data_skey_set;
				{
					/* ボタン 1 */
					b1 = q8tk_button_new_with_label(
							 GET_LABEL(l, DATA_SKEY_BUTTON_OFF));
					q8tk_signal_connect(b1, "clicked",
										cb_key_softkey_release, NULL);
					q8tk_box_pack_start(h, b1);
					q8tk_widget_show(b1);
				}
				{
					/* ボタン 2 */
					b2 = q8tk_button_new_with_label(
							 GET_LABEL(l, DATA_SKEY_BUTTON_QUIT));
					q8tk_signal_connect(b2, "clicked",
										cb_key_softkey_end, NULL);
					q8tk_box_pack_start(h, b2);
					q8tk_widget_show(b2);
					q8tk_accel_group_add(a, Q8TK_KEY_ESC, b2, "clicked");
				}
			}
		}
	}

	/* スクロール付 WIN に、キートップの文字のかかれた、ボタンを並べる */

	/* キー6列分を格納する VBOX を配置 */
	vx = q8tk_vbox_new();
	q8tk_container_add(s, vx);
	q8tk_widget_show(vx);


	/* キー6列分繰り返し */
	for (j = 0; j < 6; j++) {

		const t_keymap *p = keymap_line[ model ][ j ];

		/* キー複数個を格納するためのHBOXに */
		hx = q8tk_hbox_new();
		q8tk_box_pack_start(vx, hx);
		q8tk_widget_show(hx);

		/* キーを1個づつ配置しておく*/
		for (i = 0; p[ i ].str; i++) {

			if (p[i].code) {
				if (! (keymap_num < COUNTOF(keymap))) {
					fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
					break;
				}

				/* キートップ文字 (ボタン) */
				n = q8tk_toggle_button_new_with_label(p[i].str);
				if (get_key_softkey(p[i].code)) {
					q8tk_toggle_button_set_state(n, TRUE);
				}
				q8tk_signal_connect(n, "toggled",
									cb_key_softkey, INT2P(p[i].code));
				keymap[ keymap_num ++ ] = n;

			} else {
				/* パディング用空白 (ラベル) */
				n = q8tk_label_new(p[i].str);
			}

			q8tk_box_pack_start(hx, n);
			q8tk_widget_show(n);
		}
	}

	keymap_window_base = w;
	keymap_window_focus = b2;
}


static void keymap_start(void)
{
	if (keymap_window_base == NULL) {
		keymap_create();
	}

	q8tk_widget_show(keymap_window_base);
	q8tk_grab_add(keymap_window_base);

	q8tk_widget_set_focus(keymap_window_focus);
}

static void keymap_finish(void)
{
	q8tk_grab_remove(keymap_window_base);
	q8tk_widget_hide(keymap_window_base);
}
