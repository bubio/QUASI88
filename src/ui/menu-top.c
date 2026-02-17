/*===========================================================================
 *
 * メニュー メインウインドウ
 *
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "quasi88.h"
#include "fname.h"
#include "initval.h"
#include "getconf.h"
#include "menu.h"

#include "screen.h"
#include "monitor.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-reset.h"
#include "menu-cpu.h"
#include "menu-graph.h"
#include "menu-volume.h"
#include "menu-disk.h"
#include "menu-key.h"
#include "menu-mouse.h"
#include "menu-tape.h"
#include "menu-misc.h"
#include "menu-about.h"

#include "menu-top.h"
#include "menu-top-message.h"


static int menu_last_page = 0;				/* 前回時のメニュータグを記憶 */

/*----------------------------------------------------------------------*/
/* NOTEBOOK に張り付ける、各ページ */
static struct {
	int			data_num;
	Q8tkWidget	*(*menu_create)(void);
} menu_page[] = {
	{ DATA_TOP_RESET,	menu_reset,		},
	{ DATA_TOP_CPU,		menu_cpu,		},
	{ DATA_TOP_GRAPH,	menu_graph,		},
#ifdef USE_SOUND
	{ DATA_TOP_VOLUME,	menu_volume,	},
#endif
	{ DATA_TOP_DISK,	menu_disk,		},
	{ DATA_TOP_KEY,		menu_key,		},
	{ DATA_TOP_MOUSE,	menu_mouse,		},
	{ DATA_TOP_TAPE,	menu_tape,		},
	{ DATA_TOP_MISC,	menu_misc,		},
	{ DATA_TOP_ABOUT,	menu_about,		},
};

/*----------------------------------------------------------------------*/
/* NOTEBOOK の各ページを、ファンクションキーで選択出来るように、
 * アクセラレータキーを設定する。そのため、ダミーウィジット利用 */

#define cb_note_fake(fn,n)												\
static void cb_note_fake_##fn(UNUSED_WIDGET, Q8tkWidget *notebook)		\
{																		\
	q8tk_notebook_set_page(notebook, n);								\
}
cb_note_fake(f1, 0)
cb_note_fake(f2, 1)
cb_note_fake(f3, 2)
cb_note_fake(f4, 3)
cb_note_fake(f5, 4)
cb_note_fake(f6, 5)
cb_note_fake(f7, 6)
cb_note_fake(f8, 7)
cb_note_fake(f9, 8)
cb_note_fake(f10, 9)

/* 以下のアクセラレータキー処理は、 floi氏 提供。 Thanks ! */
static void cb_note_fake_prev(UNUSED_WIDGET, Q8tkWidget *notebook)
{
	int n = q8tk_notebook_current_page(notebook) - 1;
	if (n < 0) {
		n = COUNTOF(menu_page) - 1;
	}
	q8tk_notebook_set_page(notebook, n);
}

static void cb_note_fake_next(UNUSED_WIDGET, Q8tkWidget *notebook)
{
	int n = q8tk_notebook_current_page(notebook) + 1;
	if (COUNTOF(menu_page) <= n) {
		n = 0;
	}
	q8tk_notebook_set_page(notebook, n);
}

static struct {
	int		key;
	void	(*cb_func)(Q8tkWidget *, Q8tkWidget *);
} menu_fkey[] = {
	{ Q8TK_KEY_F1,		cb_note_fake_f1,	},
	{ Q8TK_KEY_F2,		cb_note_fake_f2,	},
	{ Q8TK_KEY_F3,		cb_note_fake_f3,	},
	{ Q8TK_KEY_F4,		cb_note_fake_f4,	},
	{ Q8TK_KEY_F5,		cb_note_fake_f5,	},
	{ Q8TK_KEY_F6,		cb_note_fake_f6,	},
	{ Q8TK_KEY_F7,		cb_note_fake_f7,	},
	{ Q8TK_KEY_F8,		cb_note_fake_f8,	},
	{ Q8TK_KEY_F9,		cb_note_fake_f9,	},
	{ Q8TK_KEY_F10,		cb_note_fake_f10,	},

	{ Q8TK_KEY_HOME,	cb_note_fake_prev,	},
	{ Q8TK_KEY_END,		cb_note_fake_next,	},
};

/*----------------------------------------------------------------------*/
/* 簡易リセットボタン ＋ モニターボタン */
static Q8tkWidget *monitor_widget;
static Q8tkWidget *quickres_widget;

static int        top_misc_stat = 1;
static Q8tkWidget *top_misc_button;

static Q8tkWidget *menu_top_misc_quickres(void);
static Q8tkWidget *menu_top_misc_monitor(void);

static void cb_top_misc_stat(UNUSED_WIDGET, UNUSED_PARM)
{
	top_misc_stat ^= 1;
	if (top_misc_stat) {
		q8tk_widget_hide(quickres_widget);
		q8tk_label_set(top_misc_button->child,  ">>");
		q8tk_widget_show(monitor_widget);
	} else {
		q8tk_widget_show(quickres_widget);
		q8tk_label_set(top_misc_button->child,  "<<");
		q8tk_widget_hide(monitor_widget);
	}
}
static Q8tkWidget *menu_top_button_misc(void)
{
	Q8tkWidget *box;

	box = PACK_HBOX(NULL);

	quickres_widget = menu_top_misc_quickres();
	monitor_widget  = menu_top_misc_monitor();

	top_misc_button = PACK_BUTTON(NULL,
								  "<<",
								  cb_top_misc_stat, NULL);

	q8tk_box_pack_start(box, quickres_widget);
	q8tk_box_pack_start(box, top_misc_button);
	q8tk_box_pack_start(box, monitor_widget);
	PACK_LABEL(box, "     ");

	top_misc_stat ^= 1;
	cb_top_misc_stat(0, 0);

	return box;
}

/*----------------------------------------------------------------------*/
/* 簡易リセットボタン   */
static int get_quickres_basic(void)
{
	return reset_req.boot_basic;
}
static void cb_quickres_basic(UNUSED_WIDGET, void *p)
{
	if (reset_req.boot_basic != P2INT(p)) {
		reset_req.boot_basic = P2INT(p);

		q8tk_toggle_button_set_state(widget_reset_basic[ 0 ][ P2INT(p) ], TRUE);
	}
}
static int get_quickres_clock(void)
{
	return reset_req.boot_clock_4mhz;
}
static void cb_quickres_clock(UNUSED_WIDGET, void *p)
{
	if (reset_req.boot_clock_4mhz != P2INT(p)) {
		reset_req.boot_clock_4mhz = P2INT(p);

		q8tk_toggle_button_set_state(widget_reset_clock[ 0 ][ P2INT(p) ], TRUE);
	}
}

static Q8tkWidget *menu_top_misc_quickres(void)
{
	Q8tkWidget *box;
	Q8List     *list;

	box = PACK_HBOX(NULL);
	{
		list = PACK_RADIO_BUTTONS(PACK_VBOX(box),
								  data_quickres_basic, COUNTOF(data_quickres_basic),
								  get_quickres_basic(), cb_quickres_basic);

		/* リストを手繰って、全ウィジットを取得 */
		widget_reset_basic[1][BASIC_V2 ] = list->data;
		list = list->next;
		widget_reset_basic[1][BASIC_V1H] = list->data;
		list = list->next;
		widget_reset_basic[1][BASIC_V1S] = list->data;
		list = list->next;
		widget_reset_basic[1][BASIC_N  ] = list->data;


		list = PACK_RADIO_BUTTONS(PACK_VBOX(box),
								  data_quickres_clock, COUNTOF(data_quickres_clock),
								  get_quickres_clock(), cb_quickres_clock);

		/* リストを手繰って、全ウィジットを取得 */
		widget_reset_clock[1][CLOCK_4MHZ] = list->data;
		list = list->next;
		widget_reset_clock[1][CLOCK_8MHZ] = list->data;


		PACK_BUTTON(box,
					GET_LABEL(data_quickres_reset, 0),
					cb_reset_now, NULL);

		PACK_VSEP(box);
	}
	q8tk_widget_hide(box);

	return box;
}

/*----------------------------------------------------------------------*/
/* モニターボタン */
static void cb_top_monitor(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_monitor(); /* ← q8tk_main_quit() 呼出済み */
}

static Q8tkWidget *menu_top_misc_monitor(void)
{
	Q8tkWidget *box;

	box = PACK_HBOX(NULL);
	{
		PACK_LABEL(box, "  ");

		if (debug_mode) {
			PACK_BUTTON(box,
						GET_LABEL(data_top_monitor, DATA_TOP_MONITOR_BTN),
						cb_top_monitor, NULL);
		} else {
			PACK_LABEL(box,
					   GET_LABEL(data_top_monitor, DATA_TOP_MONITOR_PAD));
		}

		PACK_LABEL(box, "     ");
	}
	q8tk_widget_hide(box);

	return box;
}
/*----------------------------------------------------------------------*/
/* ステータス */
static int get_top_toolbar(void)
{
	return quasi88_cfg_now_toolbar();
}
static void cb_top_toolbar(Q8tkWidget *widget, UNUSED_PARM)
{
	int on = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	quasi88_cfg_set_toolbar(on);
}

static int get_top_status(void)
{
	return quasi88_cfg_now_statusbar();
}
static void cb_top_status(Q8tkWidget *widget, UNUSED_PARM)
{
	int on = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	quasi88_cfg_set_statusbar(on);
}

static void menu_top_status(Q8tkWidget *base_hbox)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(base_hbox);
	{
		PACK_CHECK_BUTTON(vbox,
						  GET_LABEL(data_top_status, DATA_TOP_STATUS_TOOLBAR),
						  get_top_toolbar(),
						  cb_top_toolbar, NULL);

		PACK_CHECK_BUTTON(vbox,
						  GET_LABEL(data_top_status, DATA_TOP_STATUS_STATUS),
						  get_top_status(),
						  cb_top_status, NULL);

		PACK_LABEL(vbox, GET_LABEL(data_top_status, DATA_TOP_STATUS_LABEL));
	}
}
/*----------------------------------------------------------------------*/
/* メインウインドウ下部のボタン */
static void sub_top_savecfg(void);
static void sub_top_quit(void);

static void cb_top_button(UNUSED_WIDGET, void *p)
{
	switch (P2INT(p)) {

	case DATA_TOP_SAVECFG:
		sub_top_savecfg();
		break;

	case DATA_TOP_EXIT:
		quasi88_exec(); /* ← q8tk_main_quit() 呼出済み */
		break;

	case DATA_TOP_QUIT:
		sub_top_quit();
		return;
	}
}

static Q8tkWidget *menu_top_button(Q8tkWidget *accel)
{
	int i;
	Q8tkWidget *hbox, *w;
	const t_menudata *p = data_top_button;

	hbox = PACK_HBOX(NULL);
	{
		w = menu_top_button_misc();
		q8tk_box_pack_start(hbox, w);

		menu_top_status(hbox);

		for (i = 0; i < COUNTOF(data_top_button); i++, p++) {

			w = PACK_BUTTON(hbox, GET_LABEL(p, 0),
							cb_top_button, INT2P(p->val));

			if (p->val == DATA_TOP_QUIT) {
				q8tk_accel_group_add(accel, Q8TK_KEY_MENU, w, "clicked");
			}
			if (p->val == DATA_TOP_EXIT) {
				q8tk_accel_group_add(accel, Q8TK_KEY_ESC, w, "clicked");
			}
		}
	}
	q8tk_misc_set_placement(hbox, Q8TK_PLACEMENT_X_RIGHT, 0);

	return hbox;
}


/*----------------------------------------------------------------------*/
/* 設定保存ボタン押下時の、確認ダイアログ */

static char *top_savecfg_filename;

static void cb_top_savecfg_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		config_save(top_savecfg_filename);
	}

	free(top_savecfg_filename);
	top_savecfg_filename = NULL;
}
static int get_top_savecfg_auto(void)
{
	return save_config;
}
static void cb_top_savecfg_auto(Q8tkWidget *widget, UNUSED_PARM)
{
	int parm = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	save_config = parm;
}

static void sub_top_savecfg(void)
{
	const t_menulabel *l = data_top_savecfg;

	top_savecfg_filename = filename_alloc_global_cfgname();

	if (top_savecfg_filename) {
		dialog_create();
		{
			dialog_set_title(GET_LABEL(l, DATA_TOP_SAVECFG_TITLE));
			dialog_set_title(GET_LABEL(l, DATA_TOP_SAVECFG_INFO));
			dialog_set_title("");
			dialog_set_title(top_savecfg_filename);
			dialog_set_title("");
			dialog_set_check_button(GET_LABEL(l, DATA_TOP_SAVECFG_AUTO),
									get_top_savecfg_auto(),
									cb_top_savecfg_auto, NULL);

			dialog_set_separator();

			dialog_set_button(GET_LABEL(l, DATA_TOP_SAVECFG_OK),
							  cb_top_savecfg_clicked, (void *)TRUE);

			dialog_accel_key(Q8TK_KEY_MENU);

			dialog_set_button(GET_LABEL(l, DATA_TOP_SAVECFG_CANCEL),
							  cb_top_savecfg_clicked, (void *)FALSE);

			dialog_accel_key(Q8TK_KEY_ESC);
		}
		dialog_start();
	}
}


/*----------------------------------------------------------------------*/
/* QUITボタン押下時の、確認ダイアログ */

static void cb_top_quit_clicked(UNUSED_WIDGET, void *p)
{
	dialog_destroy();

	if (P2INT(p)) {
		quasi88_quit(); /* ← q8tk_main_quit() 呼出済み */
	}
}
static void sub_top_quit(void)
{
	const t_menulabel *l = data_top_quit;

	dialog_create();
	{
		dialog_set_title(GET_LABEL(l, DATA_TOP_QUIT_TITLE));

		dialog_set_separator();

		dialog_set_button(GET_LABEL(l, DATA_TOP_QUIT_OK),
						  cb_top_quit_clicked, (void *)TRUE);

		dialog_accel_key(Q8TK_KEY_MENU);

		dialog_set_button(GET_LABEL(l, DATA_TOP_QUIT_CANCEL),
						  cb_top_quit_clicked, (void *)FALSE);

		dialog_accel_key(Q8TK_KEY_ESC);
	}
	dialog_start();
}


/*----------------------------------------------------------------------*/
/* メニューのノートページが変わったら */

static void cb_top_notebook_changed(Q8tkWidget *widget, UNUSED_PARM)
{
	menu_last_page = q8tk_notebook_current_page(widget);
}

/*======================================================================*/
Q8tkWidget *menu_top(void)
{
	int i;
	const t_menudata *l = data_top;
	Q8tkWidget *win, *accel, *vbox, *notebook, *w, *fake;

	/* 現在の、リセット情報を取得 */
	quasi88_get_reset_cfg(&reset_req);


	win = q8tk_window_new(Q8TK_WINDOW_TOPLEVEL);
	accel = q8tk_accel_group_new();
	q8tk_accel_group_attach(accel, win);
	q8tk_grab_add(win);
	q8tk_widget_show(win);

	vbox = PACK_VBOX(NULL);
	{
		{
			/* 各メニューをノートページに乗せていく */

			notebook = q8tk_notebook_new();
			{
				for (i = 0; i < COUNTOF(menu_page); i++) {

					w = (*menu_page[i].menu_create)();
					q8tk_notebook_append(notebook, w,
										 GET_LABEL(l, menu_page[i].data_num));

					if (i < COUNTOF(menu_fkey)) {
						fake = q8tk_button_new();
						q8tk_signal_connect(fake, "clicked",
											menu_fkey[i].cb_func, notebook);
						q8tk_accel_group_add(accel, menu_fkey[i].key,
											 fake, "clicked");
					}
				}

				for (; i < COUNTOF(menu_fkey); i++) {
					fake = q8tk_button_new();
					q8tk_signal_connect(fake, "clicked",
										menu_fkey[i].cb_func, notebook);
					q8tk_accel_group_add(accel, menu_fkey[i].key,
										 fake, "clicked");
				}
			}
			q8tk_signal_connect(notebook, "switch_page",
								cb_top_notebook_changed, NULL);
			q8tk_widget_show(notebook);
			q8tk_box_pack_start(vbox, notebook);

			/* 実験：トップのノートブックは、フォーカスを適宜クリア
			         (タグ部分のアンダーライン表示がなくなる。ただそれだけ) */
			q8tk_notebook_hook_focus_lost(notebook, TRUE);
		}
		{
			/* おつぎは、ボタン */

			w = menu_top_button(accel);
			q8tk_box_pack_start(vbox, w);
		}
	}
	q8tk_container_add(win, vbox);


	/* ノートブックの初期状態、前回メニュー終了時と同じページ */
	q8tk_notebook_set_page(notebook, menu_last_page);

	/* ノートブックを返します */
	return notebook;
}
