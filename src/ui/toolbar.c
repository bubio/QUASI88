/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include "quasi88.h"
#include "initval.h"
#include "screen.h"
#include "event.h"

#include "drive.h"
#include "q8tk.h"
#include "menu-common.h"
#include "toolbar.h"

#include "menu.h"
#include "pause.h"


/* ビットマップの実体はこれ */

#include "toolbar-bmp.c"

#define ENABLE_BACK_BUTTON

/***********************************************************************
 *
 ************************************************************************/
/* ボタンの画像として使用する、ビットマップウィジット */
enum {
	E_BMP_SIZE_SMALL,

	E_BMP_XSUB_OFF = E_BMP_SIZE_SMALL,
	E_BMP_XSUB_ON,
	E_BMP_XSUB_INS,

	E_BMP_SIZE_LARGE,

	E_BMP_POWER_OFF = E_BMP_SIZE_LARGE,
	E_BMP_POWER_ON,
	E_BMP_POWER_INS,
	E_BMP_RESET_OFF,
	E_BMP_RESET_ON,
	E_BMP_RESET_INS,
	E_BMP_PAUSE_OFF,
	E_BMP_PAUSE_ON,
	E_BMP_PAUSE_INS,
	E_BMP_SPEEDUP_OFF,
	E_BMP_SPEEDUP_ON,
	E_BMP_SPEEDUP_INS,
	E_BMP_RESTORE_OFF,
	E_BMP_RESTORE_ON,
	E_BMP_RESTORE_INS,
	E_BMP_DRIVE1_OFF,
	E_BMP_DRIVE1_ON,
	E_BMP_DRIVE1_INS,
	E_BMP_DRIVE2_OFF,
	E_BMP_DRIVE2_ON,
	E_BMP_DRIVE2_INS,
	E_BMP_KEYBOARD_OFF,
	E_BMP_KEYBOARD_ON,
	E_BMP_KEYBOARD_INS,
	E_BMP_MENU_OFF,
	E_BMP_MENU_ON,
	E_BMP_MENU_INS,
	E_BMP_CONFIG_OFF,
	E_BMP_CONFIG_ON,
	E_BMP_CONFIG_INS,
	E_BMP_QUIT_OFF,
	E_BMP_QUIT_ON,
	E_BMP_QUIT_INS,
	E_BMP_FULLSCREEN_OFF,
	E_BMP_FULLSCREEN_ON,
	E_BMP_FULLSCREEN_INS,
	E_BMP_WINDOW_OFF,
	E_BMP_WINDOW_ON,
	E_BMP_WINDOW_INS,
	E_BMP_BACK_OFF,
	E_BMP_BACK_ON,
	E_BMP_BACK_INS,
	E_BMP_DUMMY,

	E_BMP_END
};

static Q8tkWidget *bmp[E_BMP_END];


/* 画像のグループ (OFF画像/ON画像/無効画像で一つのグループ) */
enum e_grp {
	E_GRP_POWER,
	E_GRP_RESET,
	E_GRP_PAUSE,
	E_GRP_SPEEDUP,
	E_GRP_XSUB,
	E_GRP_RESTORE,
	E_GRP_DRIVE1,
	E_GRP_DRIVE2,
	E_GRP_YSUB,
	E_GRP_KEYBOARD,
	E_GRP_MENU,
	E_GRP_CONFIG,
	E_GRP_QUIT,
	E_GRP_FULLSCREEN,
	E_GRP_WINDOW,
	E_GRP_BACK,
	E_GRP_DUMMY,
	E_GRP_END
};

static const struct {
	unsigned char off;
	unsigned char on;
	unsigned char ins;
} btn_define[E_GRP_END] = {
	{ E_BMP_POWER_OFF,			E_BMP_POWER_ON,			E_BMP_POWER_INS,		},/* E_GRP_POWER		*/
	{ E_BMP_RESET_OFF,			E_BMP_RESET_ON,			E_BMP_RESET_INS,		},/* E_GRP_RESET		*/
	{ E_BMP_PAUSE_OFF,			E_BMP_PAUSE_ON,			E_BMP_PAUSE_INS,		},/* E_GRP_PAUSE		*/
	{ E_BMP_SPEEDUP_OFF,		E_BMP_SPEEDUP_ON,		E_BMP_SPEEDUP_INS,		},/* E_GRP_SPEEDUP		*/
	{ E_BMP_XSUB_OFF,			E_BMP_XSUB_ON,			E_BMP_XSUB_INS,			},/* E_GRP_XSUB			*/
	{ E_BMP_RESTORE_OFF,		E_BMP_RESTORE_ON,		E_BMP_RESTORE_INS,		},/* E_GRP_RESTORE		*/
	{ E_BMP_DRIVE1_OFF,			E_BMP_DRIVE1_ON,		E_BMP_DRIVE1_INS,		},/* E_GRP_DRIVE1		*/
	{ E_BMP_DRIVE2_OFF,			E_BMP_DRIVE2_ON,		E_BMP_DRIVE2_INS,		},/* E_GRP_DRIVE2		*/
	{ E_BMP_XSUB_OFF,			E_BMP_XSUB_ON,			E_BMP_XSUB_INS,			},/* E_GRP_YSUB			*/
	{ E_BMP_KEYBOARD_OFF,		E_BMP_KEYBOARD_ON,		E_BMP_KEYBOARD_INS,		},/* E_GRP_KEYBOARD		*/
	{ E_BMP_MENU_OFF,			E_BMP_MENU_ON,			E_BMP_MENU_INS,			},/* E_GRP_MENU			*/
	{ E_BMP_CONFIG_OFF,			E_BMP_CONFIG_ON,		E_BMP_CONFIG_INS,		},/* E_GRP_CONFIG		*/
	{ E_BMP_QUIT_OFF,			E_BMP_QUIT_ON,			E_BMP_QUIT_INS,			},/* E_GRP_QUIT			*/
	{ E_BMP_FULLSCREEN_OFF,		E_BMP_FULLSCREEN_ON,	E_BMP_FULLSCREEN_INS,	},/* E_GRP_FULLSCREEN	*/
	{ E_BMP_WINDOW_OFF,			E_BMP_WINDOW_ON,		E_BMP_WINDOW_INS,		},/* E_GRP_WINDOW		*/
	{ E_BMP_BACK_OFF,			E_BMP_BACK_ON,			E_BMP_BACK_INS,			},/* E_GRP_BACK			*/
	{ E_BMP_DUMMY,				E_BMP_DUMMY,			E_BMP_DUMMY,			},/* E_GRP_DUMMY		*/
};


/* ツールバーに登録するボタン */
enum e_btn {
	E_BTN_RESET,
	E_BTN_PAUSE,
	E_BTN_SPEEDUP,
	E_BTN_XSUB,
	E_BTN_DRIVE1,
	E_BTN_DRIVE2,
	E_BTN_YSUB,
	E_BTN_FULLSCREEN,
	E_BTN_KEYBOARD,
	E_BTN_MENU,
	E_BTN_QUIT,
	E_BTN_END
};
static struct t_toolbtn {
	Q8tkWidget	*w;				/* ボタンウィジット */
	int			grp_id;			/* ボタンの画像グループ */
	char		sense_exec;		/* エミュの時、TRUEで有効表示 */
	char		sense_menu;		/* メニューの時、TRUEで有効表示 */
} btn[E_BTN_END];


/* ボタンウィジット button の画像を grp_id の画像に変える */
static void toolbar_change_button_bmp(Q8tkWidget *button, int grp_id)
{
	q8tk_button_set_bitmap(button,
						   bmp[ btn_define[ grp_id ].off ],
						   bmp[ btn_define[ grp_id ].on ],
						   bmp[ btn_define[ grp_id ].ins ]);
}


/***********************************************************************
 *
 ************************************************************************/
static void cb_tool_menu(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec() ||
		quasi88_is_pause() ||
		quasi88_is_askreset()) {
		quasi88_menu();
	}
	if (quasi88_is_fullmenu()) {
		quasi88_exec();
	}
}

static void cb_tool_pause(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		quasi88_pause();
	}
	if (quasi88_is_pause()) {
		pause_event_key_on_esc();
	}
}

static void cb_askreset(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu()) {
		quasi88_askreset();
	}
	if (quasi88_is_askreset()) {
		quasi88_exec();
	}
}

static void cb_askspeedup(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		quasi88_askspeedup();
	}
	if (quasi88_is_askspeedup()) {
		quasi88_exec();
	}
}

static void cb_askdiskchange(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		quasi88_askdiskchange();
	}
	if (quasi88_is_askdiskchange()) {
		quasi88_exec();
	}
}

static void cb_askquit(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec() ||
		quasi88_is_pause()) {
		quasi88_askquit();
	}
	if (quasi88_is_askquit()) {
		quasi88_exec();
	}
}

static int toolbar_speedup_rate = 0;
void toolbar_speedup_change(int rate)
{
	if (rate == 100) {
		if ((quasi88_cfg_now_no_wait()) ||
			(quasi88_cfg_now_wait_rate() != 100)) {
			if (quasi88_cfg_now_no_wait()) {
				quasi88_cfg_set_wait_rate(100);
				quasi88_cfg_set_no_wait(FALSE);
			} else {
				quasi88_cfg_set_no_wait(FALSE);
				quasi88_cfg_set_wait_rate(100);
			}
		}
	} else {
		toolbar_speedup_rate = rate;

		if (toolbar_speedup_rate == 0) {
			quasi88_cfg_set_wait_rate(100);
			quasi88_cfg_set_no_wait(TRUE);
		} else {
			quasi88_cfg_set_wait_rate(toolbar_speedup_rate);
			quasi88_cfg_set_no_wait(FALSE);
		}
	}

}

static void cb_tool_speedup(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		if ((quasi88_cfg_now_no_wait()) ||
			(quasi88_cfg_now_wait_rate() != 100)) {
			toolbar_speedup_change(100);
		} else {
			toolbar_speedup_change(toolbar_speedup_rate);
		}
	}
}

static void cb_tool_drive1_timeout(UNUSED_WIDGET, void *parm)
{
	int eject_stat = P2INT(parm);
	quasi88_disk_image_insert_after_eject(DRIVE_1, eject_stat);
}
static void cb_tool_drive1(Q8tkWidget *widget, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		int eject_stat = quasi88_disk_image_eject_before_insert(DRIVE_1);
		q8tk_timer_add(widget, cb_tool_drive1_timeout, INT2P(eject_stat), 100);
	}
}

static void cb_tool_drive2_timeout(UNUSED_WIDGET, void *parm)
{
	int eject_stat = P2INT(parm);
	quasi88_disk_image_insert_after_eject(DRIVE_2, eject_stat);
}
static void cb_tool_drive2(Q8tkWidget *widget, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		int eject_stat = quasi88_disk_image_eject_before_insert(DRIVE_2);
		q8tk_timer_add(widget, cb_tool_drive2_timeout, INT2P(eject_stat), 100);
	}
}

static void cb_tool_keyboard(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_notify_touchkey(TOUCHKEY_NOTIFY_REQ_ACTION);

	if (quasi88_now_key_numlock()) {
		quasi88_notify_touchkey(TOUCHKEY_NOTIFY_REQ_NUM_ON);
	}
	if (quasi88_now_key_caps()) {
		quasi88_notify_touchkey(TOUCHKEY_NOTIFY_REQ_CAPS_ON);
	}
	if (quasi88_now_key_kana()) {
		quasi88_notify_touchkey(TOUCHKEY_NOTIFY_REQ_KANA_ON);
	}
}

static void cb_tool_fullscreen(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_exec()) {
		if (quasi88_cfg_can_fullscreen()) {
			int now  = quasi88_cfg_now_fullscreen();
			int next = (now) ? FALSE : TRUE;
			quasi88_cfg_set_fullscreen(next);
		}
	}
}


/***********************************************************************
 *
 ************************************************************************/
static void make_toolbar_bitmap(void)
{
	int i, w, h;
	const unsigned char *bmp_list[E_BMP_END] = {
		xsub_off,		/* E_BMP_XSUB_OFF */
		xsub_on,		/* E_BMP_XSUB_ON */
		xsub_ins,		/* E_BMP_XSUB_INS */

		power_off,		/* E_BMP_POWER_OFF */
		power_on,		/* E_BMP_POWER_ON */
		power_ins,		/* E_BMP_POWER_INS */
		reset_off,		/* E_BMP_RESET_OFF */
		reset_on,		/* E_BMP_RESET_ON */
		reset_ins,		/* E_BMP_RESET_INS */
		pause_off,		/* E_BMP_PAUSE_OFF */
		pause_on,		/* E_BMP_PAUSE_ON */
		pause_ins,		/* E_BMP_PAUSE_INS */
		speedup_off,	/* E_BMP_SPEEDUP_OFF */
		speedup_on,		/* E_BMP_SPEEDUP_ON */
		speedup_ins,	/* E_BMP_SPEEDUP_INS */
		restore_off,	/* E_BMP_RESTORE_OFF */
		restore_on,		/* E_BMP_RESTORE_ON */
		restore_ins,	/* E_BMP_RESTORE_INS */
		drive1_off,		/* E_BMP_DRIVE1_OFF */
		drive1_on,		/* E_BMP_DRIVE1_ON */
		drive1_ins,		/* E_BMP_DRIVE1_INS */
		drive2_off,		/* E_BMP_DRIVE2_OFF */
		drive2_on,		/* E_BMP_DRIVE2_ON */
		drive2_ins,		/* E_BMP_DRIVE2_INS */
		keyboard_off,	/* E_BMP_KEYBOARD_OFF */
		keyboard_on,	/* E_BMP_KEYBOARD_ON */
		keyboard_ins,	/* E_BMP_KEYBOARD_INS */
		menu_off,		/* E_BMP_MENU_OFF */
		menu_on,		/* E_BMP_MENU_ON */
		menu_ins,		/* E_BMP_MENU_INS */
		config_off,		/* E_BMP_CONFIG_OFF */
		config_on,		/* E_BMP_CONFIG_ON */
		config_ins,		/* E_BMP_CONFIG_INS */
		quit_off,		/* E_BMP_QUIT_OFF */
		quit_on,		/* E_BMP_QUIT_ON */
		quit_ins,		/* E_BMP_QUIT_INS */
		fullscreen_off,	/* E_BMP_FULLSCREEN_OFF */
		fullscreen_on,	/* E_BMP_FULLSCREEN_ON */
		fullscreen_ins,	/* E_BMP_FULLSCREEN_INS */
		window_off,		/* E_BMP_WINDOW_OFF */
		window_on,		/* E_BMP_WINDOW_ON */
		window_ins,		/* E_BMP_WINDOW_INS */
		back_off,		/* E_BMP_BACK_OFF */
		back_on,		/* E_BMP_BACK_ON */
		back_ins,		/* E_BMP_BACK_INS */

		dummy,			/* E_BMP_DUMMY */
	};

	/* 全てのBMP画像ウィジットを生成 */
	for (i = 0; i < E_BMP_END; i++) {
		if (i >= E_BMP_SIZE_LARGE) {
			w = TOOLBAR_BITMAP_WIDTH_L;
		} else {
			w = TOOLBAR_BITMAP_WIDTH_S;
		}
		h = TOOLBAR_BITMAP_HEIGHT;
		bmp[i] = q8tk_bitmap_new(w, h, bmp_list[i]);
	}
}


void submenu_init(void)
{
#define E_SEP		(-2)
#define E_LABEL		(-1)
	static const struct {
		int type;		/* -2: セパレータ / -1:ラベル / 0..:ボタン   */
		int parm;		/* -              /  サイズ   / 画像グループ */
		int sense;		/* -              /  -        / sensitive    */
		void *p;		/* -              /  文字列   / 押下時の関数 */
	} layout[] = {
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_RESET,		E_GRP_RESET,		1,	cb_askreset,		},	/* E_GRP_POWER */
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_PAUSE,		E_GRP_PAUSE,		1,	cb_tool_pause,		},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_SPEEDUP,	E_GRP_SPEEDUP,		1,	cb_tool_speedup,	},
		{ E_BTN_XSUB,		E_GRP_XSUB,			1,	cb_askspeedup,		},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_SEP,			0,					0,	NULL,				},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_DRIVE2,		E_GRP_DRIVE2,		0,	cb_tool_drive2,		},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_DRIVE1,		E_GRP_DRIVE1,		0,	cb_tool_drive1, 	},
		{ E_BTN_YSUB,		E_GRP_YSUB,			1,	cb_askdiskchange,	},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_SEP,			0,					0,	NULL,				},
		{ E_LABEL,			8,					0,	NULL,				},
		{ E_BTN_KEYBOARD,	E_GRP_KEYBOARD,		1,	cb_tool_keyboard,	},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_FULLSCREEN,	E_GRP_FULLSCREEN,	1,	cb_tool_fullscreen,	},
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_MENU,		E_GRP_MENU,			1,	cb_tool_menu,		}, /* E_GRP_CONFIG */
		{ E_LABEL,			1,					0,	NULL,				},
		{ E_BTN_QUIT,		E_GRP_QUIT,			1,	cb_askquit,			},
	};
	int i, btn_id, grp_id;
	Q8tkWidget *win, *bar, *w;

	win = q8tk_window_new(Q8TK_WINDOW_TOOL);

	bar = q8tk_toolbar_new();
	q8tk_widget_show(bar);
	q8tk_container_add(win, bar);

	/* BMPウィジットを生成 */
	make_toolbar_bitmap();

	/* ボタンなどをツールバーに乗せていく */
	for (i = 0; i < COUNTOF(layout); i++) {

		switch (layout[i].type) {
		case E_SEP:
			/* type が E_SEP の場合、垂直セパレータを追加 */
			w = q8tk_vseparator_new();
			q8tk_statusbar_add(bar, w);
			break;

		case E_LABEL:
			/* type が E_LABEL の場合、parm文字のラベル(空白or文字列)を追加 */
			w = q8tk_item_label_new(layout[i].parm);
			if (layout[i].p) {
				q8tk_item_label_set(w, (char *)layout[i].p);
			}
			q8tk_statusbar_add(bar, w);
			break;

		default:
			/* 以外 (0..E_BTN_XXX) は、そのボタンを生成して追加 */

			btn_id = layout[i].type;
			grp_id = layout[i].parm;

			w = q8tk_button_new_from_bitmap(bmp[ btn_define[ grp_id ].off ],
											bmp[ btn_define[ grp_id ].on ],
											bmp[ btn_define[ grp_id ].ins ]);
			btn[ btn_id ].w = w;
			btn[ btn_id ].grp_id = grp_id;
			btn[ btn_id ].sense_exec = layout[i].sense;
			btn[ btn_id ].sense_menu = layout[i].sense;

			q8tk_toolbar_add(bar, w);
			if (layout[i].p) {
				q8tk_signal_connect(w, "clicked",
									(Q8tkSignalFunc)layout[i].p, NULL);
			}
			break;
		}
	}

	submenu_controll(CTRL_CHG_WAIT);

	q8tk_grab_add(win);
	q8tk_widget_show(win);

	set_screen_dirty_tool_full();
}


/***********************************************************************
 *
 ************************************************************************/
static void (*menu_callback)(int controll) = NULL;
void quasi88_set_memu_callback(void (*callback)(int controll))
{
	menu_callback = callback;
}

static void submenu_update_sensitive(int submenu_disable)
{
	int i;
	int is_menu = quasi88_is_menu_mode();

	if (submenu_disable) {
		/* 全てのボタンを操作可否を、不可状態にする */
		for (i = 0; i < E_BTN_END; i++) {
			if (btn[i].w) {
				q8tk_widget_set_sensitive(btn[i].w, FALSE);
			}
		}
	} else {
		/* 全てのボタンを操作可否を、元の状態にする */
		for (i = 0; i < E_BTN_END; i++) {
			q8tk_widget_set_sensitive(btn[i].w,
									  (is_menu ?
									   btn[i].sense_menu : btn[i].sense_exec));
		}
	}
}

static int change_button_bmp_if_need(struct t_toolbtn *toolbtn, int grp_id)
{
	if (toolbtn->grp_id != grp_id) {
		toolbar_change_button_bmp(toolbtn->w, grp_id);

		toolbtn->grp_id = grp_id;
		toolbtn->sense_exec = ((grp_id == E_GRP_DUMMY) ? FALSE : TRUE);

		return TRUE;
	}
	return FALSE;
}


void submenu_controll(int controll)
{
	struct t_toolbtn *toolbtn;
	int grp_id, i;
	static char submenu_disable = FALSE;

	switch (controll) {

	case CTRL_MODE_EXEC:
	case CTRL_MODE_MONITOR:
	case CTRL_MODE_QUIT:
		change_button_bmp_if_need(&btn[E_BTN_RESET], E_GRP_RESET);
		if ((quasi88_cfg_now_no_wait()) ||
			(quasi88_cfg_now_wait_rate() != 100)) {
			grp_id = E_GRP_RESTORE;
		} else {
			grp_id = E_GRP_SPEEDUP;
		}
		change_button_bmp_if_need(&btn[E_BTN_SPEEDUP], grp_id);
		change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
		change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
		submenu_update_sensitive(submenu_disable);
		break;

	case CTRL_MODE_MENU_FULLMENU:
	case CTRL_MODE_MENU_PAUSE:
	case CTRL_MODE_MENU_ASKRESET:
	case CTRL_MODE_MENU_ASKSPEEDUP:
	case CTRL_MODE_MENU_ASKDISKCHANGE:
	case CTRL_MODE_MENU_ASKQUIT:
		for (i = 0; i < E_BTN_END; i++) {
			if (i == E_BTN_KEYBOARD) {
				continue;
			}
			btn[i].sense_menu = FALSE;
		}
		switch (controll) {
		case CTRL_MODE_MENU_PAUSE:
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
			btn[E_BTN_PAUSE].sense_menu = TRUE;
			btn[E_BTN_MENU].sense_menu = TRUE;
			btn[E_BTN_QUIT].sense_menu = TRUE;
			break;
		case CTRL_MODE_MENU_ASKRESET:
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
#ifdef      ENABLE_BACK_BUTTON
			change_button_bmp_if_need(&btn[E_BTN_RESET], E_GRP_BACK);
			btn[E_BTN_RESET].sense_menu = TRUE;
#endif
			btn[E_BTN_MENU].sense_menu = TRUE;
			break;
		case CTRL_MODE_MENU_ASKSPEEDUP:
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
			btn[E_BTN_XSUB].sense_menu = TRUE;
			break;
		case CTRL_MODE_MENU_ASKDISKCHANGE:
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
			btn[E_BTN_YSUB].sense_menu = TRUE;
			break;
		case CTRL_MODE_MENU_FULLMENU:
			change_button_bmp_if_need(&btn[E_BTN_RESET], E_GRP_RESET);
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_QUIT);
#ifdef      ENABLE_BACK_BUTTON
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_BACK);
			btn[E_BTN_MENU].sense_menu = TRUE;
#endif
			btn[E_BTN_RESET].sense_menu = TRUE;
			break;
		case CTRL_MODE_MENU_ASKQUIT:
			change_button_bmp_if_need(&btn[E_BTN_RESET], E_GRP_RESET);
			change_button_bmp_if_need(&btn[E_BTN_MENU], E_GRP_MENU);
#ifdef      ENABLE_BACK_BUTTON
			change_button_bmp_if_need(&btn[E_BTN_QUIT], E_GRP_BACK);
			btn[E_BTN_QUIT].sense_menu = TRUE;
#endif
			break;
		default:
			break;
		}

		submenu_update_sensitive(submenu_disable);
		break;


	case CTRL_SETUP_FULLSCREEN:
		/* フルスクリーン可能なら、ボタンを切り替え画像にする
		 * フルスクリーン不可なら、ボタンを空白画像(ボタンなし)にする */
		toolbtn = &btn[E_BTN_FULLSCREEN];
		if (quasi88_cfg_can_fullscreen()) {
			if (quasi88_cfg_now_fullscreen()) {
				grp_id = E_GRP_WINDOW;
			} else {
				grp_id = E_GRP_FULLSCREEN;
			}
		} else {
			grp_id = E_GRP_DUMMY;
		}
		if (change_button_bmp_if_need(toolbtn, grp_id)) {
			submenu_update_sensitive(submenu_disable);
		}
		break;

	case CTRL_SETUP_KEYBOARD:
		/* 仮想キーボード可能なら、ボタンを開始画像にする
		 * 仮想キーボード不可なら、ボタンを空白画像(ボタンなし)にする */
		toolbtn = &btn[E_BTN_KEYBOARD];
		if (quasi88_cfg_can_touchkey()) {
			grp_id = E_GRP_KEYBOARD;
		} else {
			grp_id = E_GRP_DUMMY;
		}
		if (change_button_bmp_if_need(toolbtn, grp_id)) {
			submenu_update_sensitive(submenu_disable);
		}
		break;


	case CTRL_CHG_FULLSCREEN:
		toolbtn = &btn[E_BTN_FULLSCREEN];
		if (quasi88_cfg_now_fullscreen()) {
			grp_id = E_GRP_WINDOW;
		} else {
			grp_id = E_GRP_FULLSCREEN;
		}
		toolbar_change_button_bmp(toolbtn->w, grp_id);
		submenu_controll(CTRL_SETUP_KEYBOARD);
		break;

	case CTRL_CHG_WAIT:
	case CTRL_CHG_NOWAIT:
		toolbtn = &btn[E_BTN_SPEEDUP];
		if ((quasi88_cfg_now_no_wait()) ||
			(quasi88_cfg_now_wait_rate() != 100)) {
			grp_id = E_GRP_RESTORE;
		} else {
			grp_id = E_GRP_SPEEDUP;
		}
		toolbar_change_button_bmp(toolbtn->w, grp_id);
		break;

	case CTRL_CHG_DRIVE1_EMPTY:
	case CTRL_CHG_DRIVE1_IMAGE:
		toolbtn = &btn[E_BTN_DRIVE1];
		toolbtn->sense_exec = (controll == CTRL_CHG_DRIVE1_IMAGE) ? TRUE : FALSE;

		submenu_update_sensitive(submenu_disable);
		break;

	case CTRL_CHG_DRIVE2_EMPTY:
	case CTRL_CHG_DRIVE2_IMAGE:
		toolbtn = &btn[E_BTN_DRIVE2];
		toolbtn->sense_exec = (controll == CTRL_CHG_DRIVE2_IMAGE) ? TRUE : FALSE;

		submenu_update_sensitive(submenu_disable);
		break;
	}

	if (menu_callback) {
		(menu_callback)(controll);
	}
}
