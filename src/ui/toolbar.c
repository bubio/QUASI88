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

	E_BMP_RESET_OFF = E_BMP_SIZE_LARGE,
	E_BMP_RESET_ON,
	E_BMP_RESET_INS,
	E_BMP_OPENFILE_OFF,
	E_BMP_OPENFILE_ON,
	E_BMP_OPENFILE_INS,
	E_BMP_PAUSE_OFF,
	E_BMP_PAUSE_ON,
	E_BMP_PAUSE_INS,
	E_BMP_PAUSEGO_OFF,
	E_BMP_PAUSEGO_ON,
	E_BMP_PAUSEGO_INS,
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
	E_BMP_SELECTDISK_OFF,
	E_BMP_SELECTDISK_ON,
	E_BMP_SELECTDISK_INS,
	E_BMP_STATEFILE_OFF,
	E_BMP_STATEFILE_ON,
	E_BMP_STATEFILE_INS,
	E_BMP_KEYBOARD_OFF,
	E_BMP_KEYBOARD_ON,
	E_BMP_KEYBOARD_INS,
	E_BMP_MENU_OFF,
	E_BMP_MENU_ON,
	E_BMP_MENU_INS,
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

static void make_toolbar_bitmap(void)
{
	int i, w, h;
	const unsigned char *bmp_list[E_BMP_END] = {
		xsub_off,		/* E_BMP_XSUB_OFF */
		xsub_on,		/* E_BMP_XSUB_ON */
		xsub_ins,		/* E_BMP_XSUB_INS */

		reset_off,		/* E_BMP_RESET_OFF */
		reset_on,		/* E_BMP_RESET_ON */
		reset_ins,		/* E_BMP_RESET_INS */
		openfile_off,	/* E_BMP_OPENFILE_OFF */
		openfile_on,	/* E_BMP_OPENFILE_ON */
		openfile_ins,	/* E_BMP_OPENFILE_INS */
		pause_off,		/* E_BMP_PAUSE_OFF */
		pause_on,		/* E_BMP_PAUSE_ON */
		pause_ins,		/* E_BMP_PAUSE_INS */
		pausego_off,	/* E_BMP_PAUSEGO_OFF */
		pausego_on,		/* E_BMP_PAUSEGO_ON */
		pausego_ins,	/* E_BMP_PAUSEGO_INS */
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
		selectdisk_off,	/* E_BMP_SELECTDISK_OFF */
		selectdisk_on,	/* E_BMP_SELECTDISK_ON */
		selectdisk_ins,	/* E_BMP_SELECTDISK_INS */
		statefile_off,	/* E_BMP_STATEFILE_OFF */
		statefile_on,	/* E_BMP_STATEFILE_ON */
		statefile_ins,	/* E_BMP_STATEFILE_INS */
		keyboard_off,	/* E_BMP_KEYBOARD_OFF */
		keyboard_on,	/* E_BMP_KEYBOARD_ON */
		keyboard_ins,	/* E_BMP_KEYBOARD_INS */
		menu_off,		/* E_BMP_MENU_OFF */
		menu_on,		/* E_BMP_MENU_ON */
		menu_ins,		/* E_BMP_MENU_INS */
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


/***********************************************************************
 *
 ************************************************************************/
/* 画像のグループ (OFF画像/ON画像/無効画像で一つのグループ) */
enum e_grp {
	E_GRP_RESET,
	E_GRP_OPENFILE,
	E_GRP_PAUSE,
	E_GRP_PAUSEGO,
	E_GRP_SPEEDUP,
	E_GRP_XSUB,
	E_GRP_RESTORE,
	E_GRP_DRIVE1,
	E_GRP_DRIVE2,
	E_GRP_SELECTDISK,
	E_GRP_STATEFILE,
	E_GRP_KEYBOARD,
	E_GRP_MENU,
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
	unsigned char width;
} btn_define[E_GRP_END] = {
	{ E_BMP_RESET_OFF,			E_BMP_RESET_ON,			E_BMP_RESET_INS,		6,	},/* E_GRP_RESET		*/
	{ E_BMP_OPENFILE_OFF,		E_BMP_OPENFILE_ON,		E_BMP_OPENFILE_INS,		6,	},/* E_GRP_OPENFILE		*/
	{ E_BMP_PAUSE_OFF,			E_BMP_PAUSE_ON,			E_BMP_PAUSE_INS,		6,	},/* E_GRP_PAUSE		*/
	{ E_BMP_PAUSEGO_OFF,		E_BMP_PAUSEGO_ON,		E_BMP_PAUSEGO_INS,		6,	},/* E_GRP_PAUSEGO		*/
	{ E_BMP_SPEEDUP_OFF,		E_BMP_SPEEDUP_ON,		E_BMP_SPEEDUP_INS,		6,	},/* E_GRP_SPEEDUP		*/
	{ E_BMP_XSUB_OFF,			E_BMP_XSUB_ON,			E_BMP_XSUB_INS,			2,	},/* E_GRP_XSUB			*/
	{ E_BMP_RESTORE_OFF,		E_BMP_RESTORE_ON,		E_BMP_RESTORE_INS,		6,	},/* E_GRP_RESTORE		*/
	{ E_BMP_DRIVE1_OFF,			E_BMP_DRIVE1_ON,		E_BMP_DRIVE1_INS,		6,	},/* E_GRP_DRIVE1		*/
	{ E_BMP_DRIVE2_OFF,			E_BMP_DRIVE2_ON,		E_BMP_DRIVE2_INS,		6,	},/* E_GRP_DRIVE2		*/
	{ E_BMP_SELECTDISK_OFF,		E_BMP_SELECTDISK_ON,	E_BMP_SELECTDISK_INS,	6,	},/* E_GRP_SELECTDISK	*/
	{ E_BMP_STATEFILE_OFF,		E_BMP_STATEFILE_ON,		E_BMP_STATEFILE_INS,	6,	},/* E_GRP_STATEFILE	*/
	{ E_BMP_KEYBOARD_OFF,		E_BMP_KEYBOARD_ON,		E_BMP_KEYBOARD_INS,		6,	},/* E_GRP_KEYBOARD		*/
	{ E_BMP_MENU_OFF,			E_BMP_MENU_ON,			E_BMP_MENU_INS,			6,	},/* E_GRP_MENU			*/
	{ E_BMP_QUIT_OFF,			E_BMP_QUIT_ON,			E_BMP_QUIT_INS,			6,	},/* E_GRP_QUIT			*/
	{ E_BMP_FULLSCREEN_OFF,		E_BMP_FULLSCREEN_ON,	E_BMP_FULLSCREEN_INS,	6,	},/* E_GRP_FULLSCREEN	*/
	{ E_BMP_WINDOW_OFF,			E_BMP_WINDOW_ON,		E_BMP_WINDOW_INS,		6,	},/* E_GRP_WINDOW		*/
	{ E_BMP_BACK_OFF,			E_BMP_BACK_ON,			E_BMP_BACK_INS,			6,	},/* E_GRP_BACK			*/
	{ E_BMP_DUMMY,				E_BMP_DUMMY,			E_BMP_DUMMY,			6,	},/* E_GRP_DUMMY		*/
};


/***********************************************************************
 *
 ************************************************************************/
/* ツールバーに登録可能なボタン */
enum e_btn {
	E_BTN_ASKRESET,
	E_BTN_ASKOPENFILE,
	E_BTN_PAUSE,
	E_BTN_SPEEDUP,
	E_BTN_ASKSPEEDUP,
	E_BTN_DRIVE1,
	E_BTN_DRIVE2,
	E_BTN_ASKSELECTDISK,
	E_BTN_ASKSELECTDISK2,
	E_BTN_ASKSTATEFILE,
	E_BTN_FULLSCREEN,
	E_BTN_KEYBOARD,
	E_BTN_MENU,
	E_BTN_QUIT,
	E_BTN_END
};


/*	E_BTN_ASKRESET, */
static int sns_askreset(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_askquit() ||
		quasi88_is_askreset()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_askreset(void)
{
	if (quasi88_is_askreset()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_RESET;
	}
}
static void cb_askreset(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askreset()) {
		quasi88_exec();
	} else {
		quasi88_askreset();
	}
}

/*	E_BTN_ASKOPENFILE, */
static int sns_askopenfile(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_askopenfile() ||
		quasi88_is_askquit()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_askopenfile(void)
{
	if (quasi88_is_askopenfile()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_OPENFILE;
	}
}
static void cb_askopenfile(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askopenfile()) {
		quasi88_exec();
	} else {
		quasi88_askopenfile();
	}
}

/*	E_BTN_PAUSE, */
static int sns_pause(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_pause()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_pause(void)
{
	if (quasi88_is_pause()) {
		return E_GRP_PAUSEGO;
	} else {
		return E_GRP_PAUSE;
	}
}
static void cb_pause(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_pause()) {
		pause_event_key_on_esc();
	} else {
		quasi88_pause();
	}
}

/*	E_BTN_SPEEDUP, */
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
static int sns_speedup(void)
{
	if (quasi88_is_exec()) {
		return TRUE;
	} else {
		return FALSE;
	}
}
static int gid_speedup(void)
{
	if ((quasi88_cfg_now_no_wait()) ||
		(quasi88_cfg_now_wait_rate() != 100)) {
		return E_GRP_RESTORE;
	} else {
		return E_GRP_SPEEDUP;
	}
}
static void cb_speedup(UNUSED_WIDGET, UNUSED_PARM)
{
	if ((quasi88_cfg_now_no_wait()) ||
		(quasi88_cfg_now_wait_rate() != 100)) {
		toolbar_speedup_change(100);
	} else {
		toolbar_speedup_change(toolbar_speedup_rate);
	}
}

/*	E_BTN_ASKSPEEDUP, */
static int sns_askspeedup(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_askquit() ||
		quasi88_is_askspeedup()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_askspeedup(void)
{
	return E_GRP_XSUB;
}
static void cb_askspeedup(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askspeedup()) {
		quasi88_exec();
	} else {
		quasi88_askspeedup();
	}
		
}

/*	E_BTN_DRIVE1, */
static void cb_drive1_timeout(UNUSED_WIDGET, void *parm);
static int sns_drive1(void)
{
	if (quasi88_is_exec()) {
		if (disk_image_exist(DRIVE_1)) {
			return TRUE;
		}
	}
	return FALSE;
}
static int gid_drive1(void)
{
	return E_GRP_DRIVE1;
}
static void cb_drive1(Q8tkWidget *widget, UNUSED_PARM)
{
	int eject_stat = quasi88_disk_image_eject_before_insert(DRIVE_1);
	q8tk_timer_add(widget, cb_drive1_timeout, INT2P(eject_stat), 100);
}
static void cb_drive1_timeout(UNUSED_WIDGET, void *parm)
{
	int eject_stat = P2INT(parm);
	quasi88_disk_image_insert_after_eject(DRIVE_1, eject_stat);
}

/*	E_BTN_DRIVE2, */
static void cb_drive2_timeout(UNUSED_WIDGET, void *parm);
static int sns_drive2(void)
{
	if (quasi88_is_exec()) {
		if (disk_image_exist(DRIVE_2)) {
			return TRUE;
		}
	}
	return FALSE;
}
static int gid_drive2(void)
{
	return E_GRP_DRIVE2;
}
static void cb_drive2(Q8tkWidget *widget, UNUSED_PARM)
{
	int eject_stat = quasi88_disk_image_eject_before_insert(DRIVE_2);
	q8tk_timer_add(widget, cb_drive2_timeout, INT2P(eject_stat), 100);
}
static void cb_drive2_timeout(UNUSED_WIDGET, void *parm)
{
	int eject_stat = P2INT(parm);
	quasi88_disk_image_insert_after_eject(DRIVE_2, eject_stat);
}

/*	E_BTN_ASKSELECTDISK, */
/*	E_BTN_ASKSELECTDISK2, */
static int sns_askselectdisk(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_askquit() ||
		quasi88_is_askselectdisk()) {
		if (disk_image_exist(DRIVE_1) ||
			disk_image_exist(DRIVE_2)) {
			return TRUE;
		}
	}
	return FALSE;
}
static int gid_askselectdisk(void)
{
	if (quasi88_is_askselectdisk()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_SELECTDISK;
	}
}
static int gid_askselectdisk2(void)
{
	return E_GRP_XSUB;
}
static void cb_askselectdisk(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askselectdisk()) {
		quasi88_exec();
	} else {
		quasi88_askselectdisk();
	}
}

/*	E_BTN_ASKSTATEFILE, */
static int sns_askstatefile(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_askquit() ||
		quasi88_is_askstatefile()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_askstatefile(void)
{
	if (quasi88_is_askstatefile()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_STATEFILE;
	}
}
static void cb_askstatefile(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askstatefile()) {
		quasi88_exec();
	} else {
		quasi88_askstatefile();
	}
}

/*	E_BTN_FULLSCREEN, */
static int sns_fullscreen(void)
{
	if (quasi88_cfg_can_fullscreen() == FALSE) {
		return FALSE;
	}
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_pause() ||
		quasi88_is_askopenfile() ||
		quasi88_is_askquit() ||
		quasi88_is_askreset() ||
		quasi88_is_askselectdisk() ||
		quasi88_is_askspeedup() ||
		quasi88_is_askstatefile()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_fullscreen(void)
{
	if (quasi88_cfg_can_fullscreen() == FALSE) {
		return E_GRP_DUMMY;
	}
	if (quasi88_cfg_now_fullscreen()) {
		return E_GRP_WINDOW;
	} else {
		return E_GRP_FULLSCREEN;
	}
}
static void cb_fullscreen(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_cfg_can_fullscreen() == FALSE) {
		return;
	}
	if (quasi88_is_fullmenu()) {
		menu_set_fullscreen();
	} else {
		int now  = quasi88_cfg_now_fullscreen();
		int next = (now) ? FALSE : TRUE;
		quasi88_cfg_set_fullscreen(next);
	}
}

/*	E_BTN_KEYBOARD, */
static int sns_keyboard(void)
{
	if (quasi88_cfg_can_touchkey() == FALSE) {
		return FALSE;
	}
	if (quasi88_is_monitor()) {
		return FALSE;
	}
	return TRUE;
}
static int gid_keyboard(void)
{
	if (quasi88_cfg_can_touchkey() == FALSE) {
		return E_GRP_DUMMY;
	} else {
		return E_GRP_KEYBOARD;
	}
}
static void cb_keyboard(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_cfg_can_touchkey() == FALSE) {
		return;
	}

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

/*	E_BTN_MENU, */
static int sns_menu(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_pause() ||
		quasi88_is_askopenfile() ||
		quasi88_is_askquit() ||
		quasi88_is_askreset() ||
		quasi88_is_askselectdisk() ||
		quasi88_is_askspeedup() ||
		quasi88_is_askstatefile()) {
		return TRUE;
	}
	return FALSE;
}
static void cb_menu(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_fullmenu()) {
		quasi88_exec();
	} else {
		quasi88_menu();
	}
}
static int gid_menu(void)
{
	if (quasi88_is_fullmenu()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_MENU;
	}
}

/*	E_BTN_QUIT, */
static int sns_askquit(void)
{
	if (quasi88_is_exec() ||
		quasi88_is_fullmenu() ||
		quasi88_is_pause() ||
		quasi88_is_askopenfile() ||
		quasi88_is_askquit() ||
		quasi88_is_askreset() ||
		quasi88_is_askselectdisk() ||
		quasi88_is_askspeedup() ||
		quasi88_is_askstatefile()) {
		return TRUE;
	}
	return FALSE;
}
static int gid_askquit(void)
{
	if (quasi88_is_askquit()) {
		return E_GRP_BACK;
	} else {
		return E_GRP_QUIT;
	}
}
static void cb_askquit(UNUSED_WIDGET, UNUSED_PARM)
{
	if (quasi88_is_askquit()) {
		quasi88_exec();
	} else {
		quasi88_askquit();
	}
}


static const struct {
	char			code;					/* 識別子 */
	unsigned char	btn_id;					/* ボタン番号 */
	int 			(*get_grp_id)(void);	/* 画像グループ取得関数 */
	int 			(*get_sense)(void);		/* 操作可否取得関数 */
	void			*p;						/* 押下時のコールバック関数 */
} btn_attr[E_BTN_END] = {
	{ 'R', E_BTN_ASKRESET,       gid_askreset,       sns_askreset,      cb_askreset,      },
	{ 'O', E_BTN_ASKOPENFILE,    gid_askopenfile,    sns_askopenfile,   cb_askopenfile,   },
	{ 'P', E_BTN_PAUSE,          gid_pause,          sns_pause,         cb_pause,         },
	{ 'U', E_BTN_SPEEDUP,        gid_speedup,        sns_speedup,       cb_speedup,       },
	{ 'u', E_BTN_ASKSPEEDUP,     gid_askspeedup,     sns_askspeedup,    cb_askspeedup,    },
	{ '1', E_BTN_DRIVE1,         gid_drive1,         sns_drive1,        cb_drive1,        },
	{ '2', E_BTN_DRIVE2,         gid_drive2,         sns_drive2,        cb_drive2,        },
	{ 'D', E_BTN_ASKSELECTDISK,  gid_askselectdisk,  sns_askselectdisk, cb_askselectdisk, },
	{ 'd', E_BTN_ASKSELECTDISK2, gid_askselectdisk2, sns_askselectdisk, cb_askselectdisk, },
	{ 'S', E_BTN_ASKSTATEFILE,   gid_askstatefile,   sns_askstatefile,  cb_askstatefile,  },
	{ 'F', E_BTN_FULLSCREEN,     gid_fullscreen,     sns_fullscreen,    cb_fullscreen,    },
	{ 'K', E_BTN_KEYBOARD,       gid_keyboard,       sns_keyboard,      cb_keyboard,      },
	{ 'M', E_BTN_MENU,           gid_menu,           sns_menu,          cb_menu,          },
	{ 'Q', E_BTN_QUIT,           gid_askquit,        sns_askquit,       cb_askquit,       },
};


/***********************************************************************
 *
 ************************************************************************/

/* ツールバーに乗せるボタンの情報 */
/* (ボタン数は最大で E_BTN_END個。同じ種類のボタンを乗せてはいけない) */
static struct t_toolbtn {
	Q8tkWidget		*w;				/* ボタンウィジット */
	unsigned char	grp_id;			/* ボタンの画像グループ */
} btn[E_BTN_END];


/* ツールバーに乗せるボタンなどの識別子情報 */
char *toolbar_layout = NULL;

/* デフォルトは、これ */
static const char *toolbar_layout_default = " R O P Uu | 2 1d | K F M Q";


void toolbar_init(void)
{
	int i, width = 0;
	Q8tkWidget *win, *bar, *w;
	const char *p;

	win = q8tk_window_new(Q8TK_WINDOW_TOOL);

	bar = q8tk_toolbar_new();
	q8tk_widget_show(bar);
	q8tk_container_add(win, bar);

	/* BMPウィジットを生成 */
	make_toolbar_bitmap();

	/* ボタンなどをツールバーに乗せていく */
	if (toolbar_layout == NULL) {
		p = toolbar_layout_default;
	} else {
		p = toolbar_layout;
	}
	for (; *p; p++) {

		if ((*p) == '|') {

			/* '|' なら垂直セパレータを追加 */
			w = q8tk_vseparator_new();
			q8tk_statusbar_add(bar, w);
			width += 1;

		} else if ((*p) == ' ') {

			/* ' ' なら空白のラベルを追加 */
			w = q8tk_item_label_new(1);
			q8tk_statusbar_add(bar, w);
			width += 1;

		} else {

			/* 以外なら、id に一致するボタンを生成して追加 */
			for (i = 0; i < COUNTOF(btn_attr); i++) {
				if ((*p) == btn_attr[i].code) {

					/* id に一致したら、そのボタンを生成して追加 */

					int btn_id = btn_attr[i].btn_id;

					int grp_id = (*btn_attr[i].get_grp_id)();
					int sense = (*btn_attr[i].get_sense)();

					w = q8tk_button_new_from_bitmap(
						bmp[ btn_define[ grp_id ].off ],
						bmp[ btn_define[ grp_id ].on ],
						bmp[ btn_define[ grp_id ].ins ]);
					btn[ btn_id ].w = w;
					btn[ btn_id ].grp_id = grp_id;

					q8tk_toolbar_add(bar, w);
					if (btn_attr[i].p) {
						q8tk_signal_connect(w, "clicked",
											(Q8tkSignalFunc) btn_attr[i].p,
											NULL);
					}
					q8tk_widget_set_sensitive(w, sense);

					width += btn_define[grp_id].width;
					break;
				}
			}
			/* id に一致しなかったら、無視 */
		}

		/* これ以上ボタンなどを追加しても表示できないので、スキップする */
		if (width >= 80) {
			break;
		}
	}

	toolbar_controll(CTRL_CHG_WAIT);

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

void toolbar_controll(int controll)
{
	/* 引数は、現在未使用 */
	int i;
	for (i = 0; i < E_BTN_END; i++) {
		if (btn[i].w) {
			/* ツールバーの全ボタンの、画像と操作可否を再設定 */
			
			int grp_id = (*btn_attr[i].get_grp_id)();
			int sense = (*btn_attr[i].get_sense)();

			if (btn[i].grp_id != grp_id) {
				btn[i].grp_id = grp_id;
				q8tk_button_set_bitmap(btn[i].w,
									   bmp[ btn_define[ grp_id ].off ],
									   bmp[ btn_define[ grp_id ].on ],
									   bmp[ btn_define[ grp_id ].ins ]);

			}

			if (btn[i].grp_id == E_GRP_DUMMY) {
				sense = FALSE;
			}
			q8tk_widget_set_sensitive(btn[i].w, sense);
		}
	}

	if (menu_callback) {
		(menu_callback)(controll);
	}
}
