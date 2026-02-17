/*===========================================================================
 *
 * メインページ マウス
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "menu.h"

#include "screen.h"
#include "pc88main.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-mouse.h"
#include "menu-mouse-message.h"


/* 1 で、マウス表示設定の全組合せが選択できるメニューになる(デバッグ用) */
#define DEBUG_ALL_MOUSE_PATTERN		0


static void menu_mouse_mouse_setting(void);
static void menu_mouse_joy_setting(void);
static void menu_mouse_joy2_setting(void);
/*----------------------------------------------------------------------*/
/* マウス／ジョイスティック接続 */
static int get_mouse_mode(void)
{
	return mouse_mode;
}
static void cb_mouse_mode(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_mouse_mode;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_mouse_mode); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			mouse_mode = p->val;
			menu_mouse_mouse_setting();
			menu_mouse_joy_setting();
			menu_mouse_joy2_setting();
			return;
		}
	}
}


static Q8tkWidget *menu_mouse_mode(void)
{
	Q8tkWidget *hbox;

	hbox  = PACK_HBOX(NULL);
	{
		/* インデント */
		PACK_LABEL(hbox, " ");

		PACK_COMBO(hbox,
				   data_mouse_mode, COUNTOF(data_mouse_mode),
				   get_mouse_mode(), NULL, 0,
				   cb_mouse_mode, NULL,
				   NULL, NULL);
	}

	return hbox;
}

/*----------------------------------------------------------------------*/
/* シリアルマウス */
static int get_mouse_serial(void)
{
	return use_siomouse;
}
static void cb_mouse_serial(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	use_siomouse = key;

	sio_mouse_init(use_siomouse);
}


static Q8tkWidget *menu_mouse_serial(void)
{
	Q8tkWidget *hbox;

	hbox  = PACK_HBOX(NULL);
	{
		/* PACK_LABEL(hbox, " "); */

		PACK_CHECK_BUTTON(hbox,
						  GET_LABEL(data_mouse_serial, 0),
						  get_mouse_serial(),
						  cb_mouse_serial, NULL);
	}

	return hbox;
}

/*----------------------------------------------------------------------*/
/* システム設定: マウス入力タブ */
static int         mouse_mouse_widget_init_done;
static Q8tkWidget *mouse_mouse_widget_sel;			/* 左:割り当て選択 */
static Q8tkWidget *mouse_mouse_widget_sel_none;		/* 右:空白 */
static Q8tkWidget *mouse_mouse_widget_sel_tenkey;	/* 右:入れ替え */
static Q8tkWidget *mouse_mouse_widget_sel_key;		/* 右:キー割り当て設定 */
static Q8tkWidget *mouse_mouse_widget_con;			/* 左:接続中固定表示 */
static Q8tkWidget *mouse_mouse_widget_con_con;		/* 右:入れ替えと感度 */

static int get_mouse_mouse_key_mode(void)
{
	return mouse_key_mode;
}
static void cb_mouse_mouse_key_mode(UNUSED_WIDGET, void *p)
{
	mouse_key_mode = P2INT(p);

	menu_mouse_mouse_setting();
}

static Q8tkWidget *mouse_swap_widget[2];
static int get_mouse_swap(void)
{
	return mouse_swap_button;
}
static void cb_mouse_swap(Q8tkWidget *widget, void *p)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	if (mouse_swap_button != key) {
		mouse_swap_button = key;
		if (P2INT(p) >= 0) {
			q8tk_toggle_button_set_state(mouse_swap_widget[P2INT(p)], -1);
		}
	}
}

static int get_mouse_sensitivity(void)
{
	return mouse_sensitivity;
}
static void cb_mouse_sensitivity(Q8tkWidget *widget, UNUSED_PARM)
{
	int val = Q8TK_ADJUSTMENT(widget)->value;

	mouse_sensitivity = val;
}

static int get_mouse_mouse_key(int type)
{
	return mouse_key_assign[ type ];
}
static void cb_mouse_mouse_key(Q8tkWidget *widget, void *type)
{
	int i;
	const t_keymap *q = keymap_assign;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(keymap_assign); i++, q++) {
		if (strcmp(q->str, combo_str) == 0) {
			mouse_key_assign[ P2INT(type) ] = q->code;
			return;
		}
	}
}


static Q8tkWidget *menu_mouse_mouse(void)
{
	int i;
	Q8tkWidget *hbox, *vbox;

	mouse_mouse_widget_init_done = FALSE;

	hbox = PACK_HBOX(NULL);
	{
		/* 区切り左エリア: パターン1 割り当て選択 */
		{
			vbox = PACK_VBOX(hbox);
			PACK_RADIO_BUTTONS(vbox,
							   data_mouse_mouse_key_mode,
							   COUNTOF(data_mouse_mouse_key_mode),
							   get_mouse_mouse_key_mode(),
							   cb_mouse_mouse_key_mode);

			/* 位置調整のダミー */
			for (i = 0; i < 5; i++) {
				PACK_LABEL(vbox, "");
			}

			mouse_mouse_widget_sel = vbox;
		}

		/* 区切り左エリア: パターン2 接続中の固定表示 */
		{
			vbox = PACK_VBOX(hbox);

			/* 位置調整のダミー */
			PACK_LABEL(vbox, "");

			PACK_LABEL(vbox, GET_LABEL(data_mouse, DATA_MOUSE_CONNECTING));

			/* 位置調整のダミー */
			for (i = 0; i < 6; i++) {
				PACK_LABEL(vbox, "");
			}

			mouse_mouse_widget_con = vbox;
		}

		/* 区切り棒 */
		PACK_VSEP(hbox);

		/* 区切り右エリア: パターン1 空白 */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			mouse_mouse_widget_sel_none = vbox;
		}

		/* 区切り右エリア: パターン2 ボタン入れ替え */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			mouse_swap_widget[0] =
				PACK_CHECK_BUTTON(vbox,
								  GET_LABEL(data_mouse, DATA_MOUSE_SWAP_MOUSE),
								  get_mouse_swap(),
								  cb_mouse_swap, (void *)1);

			mouse_mouse_widget_sel_tenkey = vbox;
		}

		/* 区切り右エリア: パターン3 キー割り当て設定 */
		{
			mouse_mouse_widget_sel_key =
				PACK_KEY_ASSIGN(hbox,
								data_mouse_mouse, COUNTOF(data_mouse_mouse),
								get_mouse_mouse_key, cb_mouse_mouse_key);
		}

		/* 区切り右エリア: パターン4 ボタン入れ替えと感度 */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			mouse_swap_widget[1] =
				PACK_CHECK_BUTTON(vbox,
								  GET_LABEL(data_mouse, DATA_MOUSE_SWAP_MOUSE),
								  get_mouse_swap(),
								  cb_mouse_swap, (void *) 0);

			/* 位置調整のダミー */
			for (i = 0; i < 4; i++) {
				PACK_LABEL(vbox, "");
			}

			/* マウス感度 */
			{
				const t_volume *p = data_mouse_sensitivity;
				Q8tkWidget *hbox2, *scale;

				hbox2 = PACK_HBOX(vbox);
				{
					PACK_LABEL(hbox2, GET_LABEL(p, 0));

					scale = PACK_HSCALE(hbox2,
										p,
										get_mouse_sensitivity(),
										cb_mouse_sensitivity, NULL);

					q8tk_adjustment_set_length(scale->stat.scale.adj, 20);
				}
			}

			mouse_mouse_widget_con_con = vbox;
		}

	}

	mouse_mouse_widget_init_done = TRUE;
	menu_mouse_mouse_setting();

	return hbox;
}

static void menu_mouse_mouse_setting(void)
{
	if (mouse_mouse_widget_init_done == FALSE) {
		return;
	}

	if (mouse_mode == MOUSE_NONE ||
		mouse_mode == MOUSE_JOYSTICK) {
		/* マウスはポートに未接続 */

		q8tk_widget_show(mouse_mouse_widget_sel);
		q8tk_widget_hide(mouse_mouse_widget_sel_none);
		q8tk_widget_hide(mouse_mouse_widget_sel_tenkey);
		q8tk_widget_hide(mouse_mouse_widget_sel_key);

		switch (mouse_key_mode) {
		case 0:
			q8tk_widget_show(mouse_mouse_widget_sel_none);
			break;
		case 1:
			q8tk_widget_show(mouse_mouse_widget_sel_tenkey);
			break;
		case 2:
			q8tk_widget_show(mouse_mouse_widget_sel_key);
			break;
		}

		q8tk_widget_hide(mouse_mouse_widget_con);
		q8tk_widget_hide(mouse_mouse_widget_con_con);

	} else {
		/* マウスはポートに接続中 */

		q8tk_widget_hide(mouse_mouse_widget_sel);
		q8tk_widget_hide(mouse_mouse_widget_sel_none);
		q8tk_widget_hide(mouse_mouse_widget_sel_tenkey);
		q8tk_widget_hide(mouse_mouse_widget_sel_key);

		q8tk_widget_show(mouse_mouse_widget_con);
		q8tk_widget_hide(mouse_mouse_widget_con_con);

		if (mouse_mode == MOUSE_JOYMOUSE) {
			q8tk_widget_show(mouse_mouse_widget_sel_tenkey);
		} else {
			q8tk_widget_show(mouse_mouse_widget_con_con);
		}
	}
}

/*----------------------------------------------------------------------*/
/* システム設定: ジョイスティック入力タブ */
static int         mouse_joy_widget_init_done;
static Q8tkWidget *mouse_joy_widget_sel;			/* 左:割り当て選択 */
static Q8tkWidget *mouse_joy_widget_sel_none;		/* 右:空白 */
static Q8tkWidget *mouse_joy_widget_sel_tenkey;		/* 右:入れ替え */
static Q8tkWidget *mouse_joy_widget_sel_key;		/* 右:キー割り当て設定 */
static Q8tkWidget *mouse_joy_widget_con;			/* 左:接続中固定表示 */

static int get_mouse_joy_key_mode(void)
{
	return joy_key_mode;
}
static void cb_mouse_joy_key_mode(UNUSED_WIDGET, void *p)
{
	joy_key_mode = P2INT(p);

	menu_mouse_joy_setting();
}

static int get_joy_swap(void)
{
	return joy_swap_button;
}
static void cb_joy_swap(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	joy_swap_button = key;
}

static int get_mouse_joy_key(int type)
{
	return joy_key_assign[ type ];
}
static void cb_mouse_joy_key(Q8tkWidget *widget, void *type)
{
	int i;
	const t_keymap *q = keymap_assign;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(keymap_assign); i++, q++) {
		if (strcmp(q->str, combo_str) == 0) {
			joy_key_assign[ P2INT(type) ] = q->code;
			return;
		}
	}
}


static Q8tkWidget *menu_mouse_joy(void)
{
	int i;
	Q8tkWidget *hbox, *vbox;

	mouse_joy_widget_init_done = FALSE;

	hbox = PACK_HBOX(NULL);
	{
		/* 区切り左エリア: パターン1 割り当て選択 */
		{
			vbox = PACK_VBOX(hbox);
			PACK_RADIO_BUTTONS(vbox,
							   data_mouse_joy_key_mode,
							   COUNTOF(data_mouse_joy_key_mode),
							   get_mouse_joy_key_mode(),
							   cb_mouse_joy_key_mode);

			/* 位置調整のダミー */
			for (i = 0; i < 5; i++) {
				PACK_LABEL(vbox, "");
			}

			mouse_joy_widget_sel = vbox;
		}

		/* 区切り左エリア: パターン2 接続中の固定表示 */
		{
			vbox = PACK_VBOX(hbox);

			/* 位置調整のダミー */
			PACK_LABEL(vbox, "");

			PACK_LABEL(vbox, GET_LABEL(data_mouse, DATA_MOUSE_CONNECTING));

			/* 位置調整のダミー */
			for (i = 0; i < 6; i++) {
				PACK_LABEL(vbox, "");
			}

			mouse_joy_widget_con = vbox;
		}

		/* 区切り棒 */
		PACK_VSEP(hbox);

		/* 区切り右エリア: パターン1 空白 */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			mouse_joy_widget_sel_none = vbox;
		}

		/* 区切り右エリア: パターン2 ボタン入れ替え */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			PACK_CHECK_BUTTON(vbox,
							  GET_LABEL(data_mouse, DATA_MOUSE_SWAP_JOY),
							  get_joy_swap(),
							  cb_joy_swap, NULL);

			mouse_joy_widget_sel_tenkey = vbox;
		}

		/* 区切り右エリア: パターン3 キー割り当て設定 */
		{
			mouse_joy_widget_sel_key =
				PACK_KEY_ASSIGN(hbox,
								data_mouse_joy, COUNTOF(data_mouse_joy),
								get_mouse_joy_key, cb_mouse_joy_key);
		}
	}

	mouse_joy_widget_init_done = TRUE;
	menu_mouse_joy_setting();

	return hbox;
}

static void menu_mouse_joy_setting(void)
{
	if (mouse_joy_widget_init_done == FALSE) {
		return;
	}

	if (mouse_mode == MOUSE_NONE ||
		mouse_mode == MOUSE_MOUSE ||
		mouse_mode == MOUSE_JOYMOUSE) {
		/* ジョイスティックはポートに未接続 */

		q8tk_widget_show(mouse_joy_widget_sel);
		q8tk_widget_hide(mouse_joy_widget_sel_none);
		q8tk_widget_hide(mouse_joy_widget_sel_tenkey);
		q8tk_widget_hide(mouse_joy_widget_sel_key);

		switch (joy_key_mode) {
		case 0:
			q8tk_widget_show(mouse_joy_widget_sel_none);
			break;
		case 1:
			q8tk_widget_show(mouse_joy_widget_sel_tenkey);
			break;
		case 2:
			q8tk_widget_show(mouse_joy_widget_sel_key);
			break;
		}

		q8tk_widget_hide(mouse_joy_widget_con);

	} else {
		/* ジョイスティックはポートに接続中 */

		q8tk_widget_hide(mouse_joy_widget_sel);
		q8tk_widget_hide(mouse_joy_widget_sel_none);
		q8tk_widget_show(mouse_joy_widget_sel_tenkey);
		q8tk_widget_hide(mouse_joy_widget_sel_key);

		q8tk_widget_show(mouse_joy_widget_con);
	}
}

/*----------------------------------------------------------------------*/
/* システム設定: ジョイスティック(2)入力タブ */
static int         mouse_joy2_widget_init_done;
static Q8tkWidget *mouse_joy2_widget_sel;			/* 左:割り当て選択 */
static Q8tkWidget *mouse_joy2_widget_sel_none;		/* 右:空白 */
static Q8tkWidget *mouse_joy2_widget_sel_tenkey;	/* 右:入れ替え */
static Q8tkWidget *mouse_joy2_widget_sel_key;		/* 右:キー割り当て設定 */

static int get_mouse_joy2_key_mode(void)
{
	return joy2_key_mode;
}
static void cb_mouse_joy2_key_mode(UNUSED_WIDGET, void *p)
{
	joy2_key_mode = P2INT(p);

	menu_mouse_joy2_setting();
}

static int get_joy2_swap(void)
{
	return joy2_swap_button;
}
static void cb_joy2_swap(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	joy2_swap_button = key;
}

static int get_mouse_joy2_key(int type)
{
	return joy2_key_assign[ type ];
}
static void cb_mouse_joy2_key(Q8tkWidget *widget, void *type)
{
	int i;
	const t_keymap *q = keymap_assign;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(keymap_assign); i++, q++) {
		if (strcmp(q->str, combo_str) == 0) {
			joy2_key_assign[ P2INT(type) ] = q->code;
			return;
		}
	}
}


static Q8tkWidget *menu_mouse_joy2(void)
{
	int i;
	Q8tkWidget *hbox, *vbox;

	mouse_joy2_widget_init_done = FALSE;

	hbox = PACK_HBOX(NULL);
	{
		/* 区切り左エリア: パターン1 割り当て選択 */
		{
			vbox = PACK_VBOX(hbox);
			PACK_RADIO_BUTTONS(vbox,
							   data_mouse_joy2_key_mode,
							   COUNTOF(data_mouse_joy2_key_mode),
							   get_mouse_joy2_key_mode(),
							   cb_mouse_joy2_key_mode);

			/* 位置調整のダミー */
			for (i = 0; i < 5; i++) {
				PACK_LABEL(vbox, "");
			}

			mouse_joy2_widget_sel = vbox;
		}

		/* 区切り棒 */
		PACK_VSEP(hbox);

		/* 区切り右エリア: パターン1 空白 */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			mouse_joy2_widget_sel_none = vbox;
		}

		/* 区切り右エリア: パターン2 ボタン入れ替え */
		{
			vbox = PACK_VBOX(hbox);

			PACK_LABEL(vbox, "");

			PACK_CHECK_BUTTON(vbox,
							  GET_LABEL(data_mouse, DATA_MOUSE_SWAP_JOY2),
							  get_joy2_swap(),
							  cb_joy2_swap, NULL);

			mouse_joy2_widget_sel_tenkey = vbox;
		}

		/* 区切り右エリア: パターン3 キー割り当て設定 */
		{
			mouse_joy2_widget_sel_key =
				PACK_KEY_ASSIGN(hbox,
								data_mouse_joy2, COUNTOF(data_mouse_joy2),
								get_mouse_joy2_key, cb_mouse_joy2_key);
		}
	}

	mouse_joy2_widget_init_done = TRUE;
	menu_mouse_joy2_setting();

	return hbox;
}

static void menu_mouse_joy2_setting(void)
{
	if (mouse_joy2_widget_init_done == FALSE) {
		return;
	}

	{
		q8tk_widget_show(mouse_joy2_widget_sel);
		q8tk_widget_hide(mouse_joy2_widget_sel_none);
		q8tk_widget_hide(mouse_joy2_widget_sel_tenkey);
		q8tk_widget_hide(mouse_joy2_widget_sel_key);

		switch (joy2_key_mode) {
		case 0:
			q8tk_widget_show(mouse_joy2_widget_sel_none);
			break;
		case 1:
			q8tk_widget_show(mouse_joy2_widget_sel_tenkey);
			break;
		case 2:
			q8tk_widget_show(mouse_joy2_widget_sel_key);
			break;
		}
	}
}

/*----------------------------------------------------------------------*/
/* システム設定: ※タブ */
static Q8tkWidget *menu_mouse_about(void)
{
	Q8tkWidget *vbox;
	char wk[128];
	int joy_num = event_get_joystick_num();

	vbox = PACK_VBOX(NULL);
	{
		sprintf(wk, GET_LABEL(data_mouse, DATA_MOUSE_DEVICE_NUM), joy_num);

		/* ノートブックのサイズを統一するために、ここで目一杯のサイズを確保 */
		PACK_LABEL(vbox, "                                                                          ");
		PACK_LABEL(vbox, wk);
	}
	return vbox;
}

/*----------------------------------------------------------------------*/
/* システム設定 のノートページ */
static int menu_mouse_last_page = 0;		/* 前回時のタグを記憶 */

static void cb_mouse_notebook_changed(Q8tkWidget *widget, UNUSED_PARM)
{
	menu_mouse_last_page = q8tk_notebook_current_page(widget);
}

static Q8tkWidget *menu_mouse_device(void)
{
	Q8tkWidget *notebook, *w;

	notebook = q8tk_notebook_new();
	{
		w = menu_mouse_mouse();
		q8tk_notebook_append(notebook, w,
							 GET_LABEL(data_mouse, DATA_MOUSE_DEVICE_MOUSE));

		w = menu_mouse_joy();
		q8tk_notebook_append(notebook, w,
							 GET_LABEL(data_mouse, DATA_MOUSE_DEVICE_JOY));

		w = menu_mouse_joy2();
		q8tk_notebook_append(notebook, w,
							 GET_LABEL(data_mouse, DATA_MOUSE_DEVICE_JOY2));

		w = menu_mouse_about();
		q8tk_notebook_append(notebook, w,
							 GET_LABEL(data_mouse, DATA_MOUSE_DEVICE_ABOUT));
	}

	q8tk_notebook_set_page(notebook, menu_mouse_last_page);

	q8tk_signal_connect(notebook, "switch_page",
						cb_mouse_notebook_changed, NULL);
	q8tk_widget_show(notebook);

	return notebook;
}

/*----------------------------------------------------------------------*/
/* マウスカーソルの表示非表示 */

/* デバッグ用：全マウス設定の組合せを検証 */
static int  get_mouse_debug_hide(void)
{
	return hide_mouse;
}
static int  get_mouse_debug_grab(void)
{
	return grab_mouse;
}
static void cb_mouse_debug_hide(UNUSED_WIDGET, void *p)
{
	hide_mouse = P2INT(p);
}
static void cb_mouse_debug_grab(UNUSED_WIDGET, void *p)
{
	grab_mouse = P2INT(p);
}

static Q8tkWidget *menu_mouse_debug(void)
{
	Q8tkWidget *hbox, *hbox2;

	hbox = PACK_HBOX(NULL);
	{
		hbox2 = PACK_HBOX(hbox);
		{
			PACK_RADIO_BUTTONS(hbox2,
							   data_mouse_debug_hide,
							   COUNTOF(data_mouse_debug_hide),
							   get_mouse_debug_hide(), cb_mouse_debug_hide);
		}
		PACK_VSEP(hbox); /* 区切り棒 */
		PACK_VSEP(hbox); /* 区切り棒 */
		hbox2 = PACK_HBOX(hbox);
		{
			PACK_RADIO_BUTTONS(hbox2,
							   data_mouse_debug_grab,
							   COUNTOF(data_mouse_debug_grab),
							   get_mouse_debug_grab(), cb_mouse_debug_grab);
		}
	}
	return hbox;
}



static int get_mouse_misc(void)
{
	if (grab_mouse == AUTO_MOUSE) {
		return -2;
	} else if (grab_mouse == GRAB_MOUSE) {
		return -1;
	} else {
		return hide_mouse;
	}
}
static void cb_mouse_misc(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_mouse_misc;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_mouse_misc); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {

			if (p->val == -2) {
				grab_mouse = AUTO_MOUSE;
				/*hide_mouse = SHOW_MOUSE;*/

			} else if (p->val == -1) {
				grab_mouse = GRAB_MOUSE;
				/*hide_mouse = HIDE_MOUSE;*/

			} else {
				grab_mouse = UNGRAB_MOUSE;
				hide_mouse = p->val;
			}
			return;
		}
	}
}


static Q8tkWidget *menu_mouse_misc(void)
{
	Q8tkWidget *vbox, *hbox;

	vbox = PACK_VBOX(NULL);
	{
#if (DEBUG_ALL_MOUSE_PATTERN == 0)
		/* 通常時 */

		hbox  = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, GET_LABEL(data_mouse_misc_msg, 0));

			PACK_COMBO(hbox,
					   data_mouse_misc, COUNTOF(data_mouse_misc),
					   get_mouse_misc(), NULL, 0,
					   cb_mouse_misc, NULL,
					   NULL, NULL);
		}
#else
		/* デバッグ用 */

		q8tk_box_pack_start(vbox, menu_mouse_debug());
#endif
	}

	return vbox;
}

/*----------------------------------------------------------------------*/

Q8tkWidget *menu_mouse(void)
{
	Q8tkWidget *vbox, *hbox, *vbox2, *w;
	const t_menulabel *l = data_mouse;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_MOUSE_MODE),   menu_mouse_mode());
			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_MOUSE_SERIAL), menu_mouse_serial());
		}

		/* 空行 */
		PACK_LABEL(vbox, "");

		PACK_LABEL(vbox, GET_LABEL(l, DATA_MOUSE_SYSTEM));

		vbox2 = PACK_VBOX(vbox);
		{
			w = menu_mouse_device();
			q8tk_box_pack_start(vbox2, w);
			q8tk_misc_set_placement(w, Q8TK_PLACEMENT_X_RIGHT, 0);

			w = menu_mouse_misc();
			q8tk_box_pack_start(vbox2, w);
			q8tk_misc_set_placement(w, Q8TK_PLACEMENT_X_RIGHT, 0);
		}
	}

	return vbox;
}
