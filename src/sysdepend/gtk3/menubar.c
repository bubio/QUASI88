/***********************************************************************
 * メニューバー処理
 ************************************************************************/

#include <gdk/gdkkeysyms.h>

#include "quasi88.h"
#include "fname.h"
#include "utility.h"
#include "device.h"
#include "event.h"

#include "initval.h"
#include "pc88main.h"			/* boot_basic, ...				*/
#include "memory.h"				/* use_pcg						*/
#include "soundbd.h"			/* sound_board					*/
#include "intr.h"				/* cpu_clock_mhz				*/
#include "keyboard.h"			/* mouse_mode					*/
#include "fdc.h"				/* fdc_wait						*/
#include "getconf.h"			/* config_save					*/
#include "screen.h"				/* SCREEN_INTERLACE_NO ...		*/
#include "emu.h"				/* cpu_timing, emu_reset()		*/
/*#include "menu.h"*/			/* menu_sound_restart()			*/
#include "drive.h"
#include "snddrv.h"



static GtkWidget *parent_window;
static int menubar_active;

static T_RESET_CFG menubar_reset_cfg;

#define LOCK_MENUBAR()			{								\
									int save = menubar_active;	\
									menubar_active = FALSE;

#define UNLOCK_MENUBAR()			menubar_active = save;		\
								}



/*
 * メニューの項目一覧           メニュー項目ごとに、番号を割り振る
 */
enum {
	M_TOP,

	/*----------------*/
	M_SYS,

	M_SYS_RESET,

	M_SYS_MODE,
	M_SYS_MODE_V2,
	M_SYS_MODE_V1H,
	M_SYS_MODE_V1S,
	M_SYS_MODE_N,
	M_SYS_MODE_4MH,
	M_SYS_MODE_8MH,
	M_SYS_MODE_SB,
	M_SYS_MODE_SB2,

	M_SYS_RESET_V2,
	M_SYS_RESET_V1H,
	M_SYS_RESET_V1S,

	M_SYS_MENU,

	M_SYS_SAVE,
	M_SYS_EXIT,

	/*----------------*/
	M_SET,

	M_SET_SPD,
	M_SET_SPD_DUMMY,
	M_SET_SPD_25,
	M_SET_SPD_50,
	M_SET_SPD_100,
	M_SET_SPD_200,
	M_SET_SPD_400,
	M_SET_SPD_MAX,

	M_SET_SUB,
	M_SET_SUB_DUMMY,
	M_SET_SUB_SOME,
	M_SET_SUB_OFT,
	M_SET_SUB_ALL,

	M_SET_FDCWAIT,

	M_SET_REF,
	M_SET_REF_DUMMY,
	M_SET_REF_60,
	M_SET_REF_30,
	M_SET_REF_20,
	M_SET_REF_15,

	M_SET_INT,
	M_SET_INT_DUMMY,
	M_SET_INT_NO,
	M_SET_INT_SKIP,
	M_SET_INT_YES,

	M_SET_SIZ,
	M_SET_SIZ_DUMMY,
	M_SET_SIZ_FULL,
	M_SET_SIZ_HALF,

	M_SET_PCG,

	M_SET_MO,
	M_SET_MO_DUMMY,
	M_SET_MO_NO,
	M_SET_MO_MOUSE,
	M_SET_MO_JOYMO,
	M_SET_MO_JOY,

	M_SET_CUR,
	M_SET_CUR_DUMMY,
	M_SET_CUR_DEF,
	M_SET_CUR_TEN,

	M_SET_NUMLOCK,
	M_SET_ROMAJI,

	/*----------------*/
	M_DRV,

	M_DRV_DRV1,
	M_DRV_DRV1_DUMMY,
	M_DRV_DRV1_1,
	M_DRV_DRV1_2,
	M_DRV_DRV1_3,
	M_DRV_DRV1_4,
	M_DRV_DRV1_5,
	M_DRV_DRV1_6,
	M_DRV_DRV1_7,
	M_DRV_DRV1_8,
	M_DRV_DRV1_9,
	M_DRV_DRV1_NO,
	M_DRV_DRV1_CHG,

	M_DRV_DRV2,
	M_DRV_DRV2_DUMMY,
	M_DRV_DRV2_1,
	M_DRV_DRV2_2,
	M_DRV_DRV2_3,
	M_DRV_DRV2_4,
	M_DRV_DRV2_5,
	M_DRV_DRV2_6,
	M_DRV_DRV2_7,
	M_DRV_DRV2_8,
	M_DRV_DRV2_9,
	M_DRV_DRV2_NO,
	M_DRV_DRV2_CHG,

	M_DRV_CHG,
	M_DRV_UNSET,

	/*----------------*/
	M_MISC,

	M_MISC_CAPTURE,

	M_MISC_CLOAD,
	M_MISC_CLOAD_S_DUMMY,
	M_MISC_CLOAD_S,
	M_MISC_CLOAD_U,

	M_MISC_CSAVE,
	M_MISC_CSAVE_S_DUMMY,
	M_MISC_CSAVE_S,
	M_MISC_CSAVE_U,

	M_MISC_SLOAD,
	M_MISC_SLOAD_1,
	M_MISC_SLOAD_2,
	M_MISC_SLOAD_3,
	M_MISC_SLOAD_4,
	M_MISC_SLOAD_5,
	M_MISC_SLOAD_6,
	M_MISC_SLOAD_7,
	M_MISC_SLOAD_8,
	M_MISC_SLOAD_9,

	M_MISC_SSAVE,
	M_MISC_SSAVE_1,
	M_MISC_SSAVE_2,
	M_MISC_SSAVE_3,
	M_MISC_SSAVE_4,
	M_MISC_SSAVE_5,
	M_MISC_SSAVE_6,
	M_MISC_SSAVE_7,
	M_MISC_SSAVE_8,
	M_MISC_SSAVE_9,

	M_MISC_TOOL,
	M_MISC_STATUS,

	/*----------------*/
	M_HELP,
	M_HELP_ABOUT,

	M_END
};
enum {							/* ラジオアイテムのグループ一覧 */
	GRP_BASIC,
	GRP_CLOCK,
	GRP_SB,
	GRP_SPEED,
	GRP_SUBCPU,
	GRP_REFRESH,
	GRP_INTERLACE,
	GRP_SIZE,
	GRP_MOUSE,
	GRP_CURSOR,
	GRP_FM,
	GRP_FRQ,
	GRP_BUF,
	GRP_DRIVE1,
	GRP_DRIVE2,
	GRP_CLOAD,
	GRP_CSAVE,
	GRP_END
};

static struct {					/* ウィジットなどの情報を保存する */
	GtkWidget *widget;
	GtkWidget *label;
	GtkWidget *submenu;
} mwidget[M_END];

static GSList *mlist[GRP_END];	/* ラジオアイテムのリストを保存する */

/*
 * コールバック関数
 */

static void f_sys_reset(GtkMenuItem *widget, gpointer data);
static void f_sys_reset_as(GtkMenuItem *widget, gpointer data);
static void f_sys_basic(GtkRadioMenuItem *widget, gpointer data);
static void f_sys_clock(GtkRadioMenuItem *widget, gpointer data);
static void f_sys_sb(GtkRadioMenuItem *widget, gpointer data);
static void f_sys_menu(GtkMenuItem *widget, gpointer data);
static void f_sys_save(GtkMenuItem *widget, gpointer data);
static void f_sys_exit(GtkMenuItem *widget, gpointer data);
static void f_set_speed(GtkRadioMenuItem *widget, gpointer data);
static void f_set_nowait(GtkCheckMenuItem *widget, gpointer data);
static void f_set_subcpu(GtkRadioMenuItem *widget, gpointer data);
static void f_set_fdcwait(GtkCheckMenuItem *widget, gpointer data);
static void f_set_refresh(GtkRadioMenuItem *widget, gpointer data);
static void f_set_interlace(GtkRadioMenuItem *widget, gpointer data);
static void f_set_size(GtkRadioMenuItem *widget, gpointer data);
static void f_set_pcg(GtkCheckMenuItem *widget, gpointer data);
static void f_set_mouse(GtkRadioMenuItem *widget, gpointer data);
static void f_set_cursor(GtkRadioMenuItem *widget, gpointer data);
static void f_set_numlock(GtkCheckMenuItem *widget, gpointer data);
static void f_set_romaji(GtkCheckMenuItem *widget, gpointer data);
static void f_set_fm(GtkRadioMenuItem *widget, gpointer data);
static void f_set_frq(GtkRadioMenuItem *widget, gpointer data);
static void f_set_buf(GtkRadioMenuItem *widget, gpointer data);
static void f_drv_chg(GtkMenuItem *widget, gpointer data);
static void f_drv_drv1(GtkRadioMenuItem *widget, gpointer data);
static void f_drv_drv2(GtkRadioMenuItem *widget, gpointer data);
static void f_drv_unset(GtkMenuItem *widget, gpointer data);
static void f_misc_capture(GtkMenuItem *widget, gpointer data);
static void f_misc_cload_s(GtkMenuItem *widget, gpointer data);
static void f_misc_cload_u(GtkMenuItem *widget, gpointer data);
static void f_misc_csave_s(GtkMenuItem *widget, gpointer data);
static void f_misc_csave_u(GtkMenuItem *widget, gpointer data);
static void f_misc_sload(GtkMenuItem *widget, gpointer data);
static void f_misc_ssave(GtkMenuItem *widget, gpointer data);
static void f_misc_tool(GtkCheckMenuItem *widget, gpointer data);
static void f_misc_status(GtkCheckMenuItem *widget, gpointer data);
static void f_help_about(GtkMenuItem *widget, gpointer data);

/*
 * メニューの項目内容           親子関係のあるものは、親から順に
 */

typedef struct {
	enum {
		TP_SUB,
		TP_ITEM,
		TP_CHECK,
		TP_RADIO,
		TP_SEP
	}           type;
	int         id;
	const char  *label;
	int         parent;
	int         group;
	void        (*callback)();
	int         data;
} T_MENUTABLE;

static T_MENUTABLE menutable[] = {
	{ TP_SUB,		M_SYS,				"System",				M_TOP,			0,				0,					0						},

	{ TP_ITEM,		M_SYS_RESET,		"Reset",				M_SYS,			0,				f_sys_reset,		0						},

	{ TP_SUB,		M_SYS_MODE,			"Mode",					M_SYS,			0,				0,					0						},
	{ TP_RADIO,		M_SYS_MODE_V2,		"V2",					M_SYS_MODE,		GRP_BASIC,		f_sys_basic,		BASIC_V2				},
	{ TP_RADIO,		M_SYS_MODE_V1H,		"V1H",					M_SYS_MODE,		GRP_BASIC,		f_sys_basic,		BASIC_V1H				},
	{ TP_RADIO,		M_SYS_MODE_V1S,		"V1S",					M_SYS_MODE,		GRP_BASIC,		f_sys_basic,		BASIC_V1S				},
	{ TP_RADIO,		M_SYS_MODE_N,		"N",					M_SYS_MODE,		GRP_BASIC,		f_sys_basic,		BASIC_N					},
	{ TP_SEP,		0,					0,						M_SYS_MODE,		0,				0,					0						},
	{ TP_RADIO,		M_SYS_MODE_4MH,		"4MHz",					M_SYS_MODE,		GRP_CLOCK,		f_sys_clock,		CLOCK_4MHZ				},
	{ TP_RADIO,		M_SYS_MODE_8MH,		"8MHz",					M_SYS_MODE,		GRP_CLOCK,		f_sys_clock,		CLOCK_8MHZ				},
	{ TP_SEP,		0,					0,						M_SYS_MODE,		0,				0,					0						},
	{ TP_RADIO,		M_SYS_MODE_SB,		"Sound Board",			M_SYS_MODE,		GRP_SB,			f_sys_sb,			SOUND_I					},
	{ TP_RADIO,		M_SYS_MODE_SB2,		"Sound Board II",		M_SYS_MODE,		GRP_SB,			f_sys_sb,			SOUND_II				},

	{ TP_SEP,		0,					0,						M_SYS,			0,				0,					0						},

	{ TP_ITEM,		M_SYS_RESET_V2,		"V2 mode",				M_SYS,			0,				f_sys_reset_as,		BASIC_V2				},
	{ TP_ITEM,		M_SYS_RESET_V1H,	"V1H mode",				M_SYS,			0,				f_sys_reset_as,		BASIC_V1H				},
	{ TP_ITEM,		M_SYS_RESET_V1S,	"V1S mode",				M_SYS,			0,				f_sys_reset_as,		BASIC_V1S				},

	{ TP_SEP,		0,					0,						M_SYS,			0,				0,					0						},

	{ TP_ITEM,		M_SYS_MENU,			"Menu",					M_SYS,			0,				f_sys_menu,			0						},

	{ TP_SEP,		0,					0,						M_SYS,			0,				0,					0						},

	{ TP_ITEM,		M_SYS_SAVE,			"Save Config",			M_SYS,			0,				f_sys_save,			0						},
	{ TP_ITEM,		M_SYS_EXIT,			"Exit",					M_SYS,			0,				f_sys_exit,			0						},

	/*------------------------------------------------------------------------------------------------------------------*/

	{ TP_SUB,		M_SET,				"Setting",				M_TOP,			0,				0,					0						},

	{ TP_SUB,		M_SET_SPD,			"Speed",				M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_SPD_DUMMY,	0,						M_SET_SPD,		GRP_SPEED,		0,					0						},
	{ TP_RADIO,		M_SET_SPD_25,		" 25%",					M_SET_SPD,		GRP_SPEED,		f_set_speed,		25						},
	{ TP_RADIO,		M_SET_SPD_50,		" 50%",					M_SET_SPD,		GRP_SPEED,		f_set_speed,		50						},
	{ TP_RADIO,		M_SET_SPD_100,		"100%",					M_SET_SPD,		GRP_SPEED,		f_set_speed,		100						},
	{ TP_RADIO,		M_SET_SPD_200,		"200%",					M_SET_SPD,		GRP_SPEED,		f_set_speed,		200						},
	{ TP_RADIO,		M_SET_SPD_400,		"400%",					M_SET_SPD,		GRP_SPEED,		f_set_speed,		400						},
	{ TP_SEP,		0,					0,						M_SET_SPD,		0,				0,					0						},
	{ TP_CHECK,		M_SET_SPD_MAX,		"No wait",				M_SET_SPD,		0,				f_set_nowait,		0						},

	{ TP_SUB,		M_SET_SUB,			"Sub-CPU",				M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_SUB_DUMMY,	0,						M_SET_SUB,		GRP_SUBCPU,		0,					0						},
	{ TP_RADIO,		M_SET_SUB_SOME,		"Run sometimes",		M_SET_SUB,		GRP_SUBCPU,		f_set_subcpu,		0						},
	{ TP_RADIO,		M_SET_SUB_OFT,		"Run often",			M_SET_SUB,		GRP_SUBCPU,		f_set_subcpu,		1						},
	{ TP_RADIO,		M_SET_SUB_ALL,		"Run always",			M_SET_SUB,		GRP_SUBCPU,		f_set_subcpu,		2						},

	{ TP_CHECK,		M_SET_FDCWAIT,		"Use FDC-Wait",			M_SET,			0,				f_set_fdcwait,		0						},

	{ TP_SEP,		0,					0,						M_SET,			0,				0,					0						},

	{ TP_SUB,		M_SET_REF,			"Refresh Rate",			M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_REF_DUMMY,	0,						M_SET_REF,		GRP_REFRESH,	0,					0						},
	{ TP_RADIO,		M_SET_REF_60,		"60fps",				M_SET_REF,		GRP_REFRESH,	f_set_refresh,		1						},
	{ TP_RADIO,		M_SET_REF_30,		"30fps",				M_SET_REF,		GRP_REFRESH,	f_set_refresh,		2						},
	{ TP_RADIO,		M_SET_REF_20,		"20fps",				M_SET_REF,		GRP_REFRESH,	f_set_refresh,		3						},
	{ TP_RADIO,		M_SET_REF_15,		"15fps",				M_SET_REF,		GRP_REFRESH,	f_set_refresh,		4						},

	{ TP_SUB,		M_SET_INT,			"Interlace",			M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_INT_DUMMY,	0,						M_SET_INT,		GRP_INTERLACE,	0,					0						},
	{ TP_RADIO,		M_SET_INT_NO,		"Non interlace",		M_SET_INT,		GRP_INTERLACE,	f_set_interlace,	SCREEN_INTERLACE_NO		},
	{ TP_RADIO,		M_SET_INT_SKIP,		"Skip Line",			M_SET_INT,		GRP_INTERLACE,	f_set_interlace,	SCREEN_INTERLACE_SKIP	},
	{ TP_RADIO,		M_SET_INT_YES,		"Interlace",			M_SET_INT,		GRP_INTERLACE,	f_set_interlace,	SCREEN_INTERLACE_YES	},

	{ TP_SUB,		M_SET_SIZ,			"Screen Size",			M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_SIZ_DUMMY,	0,						M_SET_SIZ,		GRP_SIZE,		0,					0						},
	{ TP_RADIO,		M_SET_SIZ_FULL,		"Normal size",			M_SET_SIZ,		GRP_SIZE,		f_set_size,			SCREEN_SIZE_FULL		},
	{ TP_RADIO,		M_SET_SIZ_HALF,		"Half size",			M_SET_SIZ,		GRP_SIZE,		f_set_size,			SCREEN_SIZE_HALF		},

	{ TP_CHECK,		M_SET_PCG,			"Use PCG-8100",			M_SET,			0,				f_set_pcg,			0						},

	{ TP_SEP,		0,					0,						M_SET,			0,				0,					0						},

	{ TP_SUB,		M_SET_MO,			"Mouse",				M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_MO_DUMMY,		0,						M_SET_MO,		GRP_MOUSE,		0,					0						},
	{ TP_RADIO,		M_SET_MO_NO,		"Nothing",				M_SET_MO,		GRP_MOUSE,		f_set_mouse,		MOUSE_NONE				},
	{ TP_RADIO,		M_SET_MO_MOUSE,		"Mouse",				M_SET_MO,		GRP_MOUSE,		f_set_mouse,		MOUSE_MOUSE				},
	{ TP_RADIO,		M_SET_MO_JOYMO,		"Mouse as joy",			M_SET_MO,		GRP_MOUSE,		f_set_mouse,		MOUSE_JOYMOUSE			},
	/*	{ TP_RADIO,		M_SET_MO_JOY,		"Joystick",				M_SET_MO,		GRP_MOUSE,		f_set_mouse,		MOUSE_JOYSTICK			},*/

	{ TP_SUB,		M_SET_CUR,			"Cursor Key",			M_SET,			0,				0,					0						},
	{ TP_RADIO,		M_SET_CUR_DUMMY,	0,						M_SET_CUR,		GRP_CURSOR,		0,					0						},
	{ TP_RADIO,		M_SET_CUR_DEF,		"Default",				M_SET_CUR,		GRP_CURSOR,		f_set_cursor,		0						},
	{ TP_RADIO,		M_SET_CUR_TEN,		"as Ten-key",			M_SET_CUR,		GRP_CURSOR,		f_set_cursor,		1						},

	{ TP_CHECK,		M_SET_NUMLOCK,		"Software Numlock",		M_SET,			0,				f_set_numlock,		0						},
	{ TP_CHECK,		M_SET_ROMAJI,		"Kana (Romaji)",		M_SET,			0,				f_set_romaji,		0						},

	/*------------------------------------------------------------------------------------------------------------------*/

	{ TP_SUB,		M_DRV,				"Disk",					M_TOP,			0,				0,					0						},

	{ TP_SUB,		M_DRV_DRV1,			"Drive 1:",				M_DRV,			0,				0,					0						},
	{ TP_RADIO,		M_DRV_DRV1_DUMMY,	0,						M_DRV_DRV1,		GRP_DRIVE1,		0,					0						},
	{ TP_RADIO,		M_DRV_DRV1_1,		"1",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			0						},
	{ TP_RADIO,		M_DRV_DRV1_2,		"2",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			1						},
	{ TP_RADIO,		M_DRV_DRV1_3,		"3",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			2						},
	{ TP_RADIO,		M_DRV_DRV1_4,		"4",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			3						},
	{ TP_RADIO,		M_DRV_DRV1_5,		"5",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			4						},
	{ TP_RADIO,		M_DRV_DRV1_6,		"6",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			5						},
	{ TP_RADIO,		M_DRV_DRV1_7,		"7",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			6						},
	{ TP_RADIO,		M_DRV_DRV1_8,		"8",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			7						},
	{ TP_RADIO,		M_DRV_DRV1_9,		"9",					M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			8						},
	{ TP_RADIO,		M_DRV_DRV1_NO,		"<No disk>",			M_DRV_DRV1,		GRP_DRIVE1,		f_drv_drv1,			-1						},
	{ TP_SEP,		0,					0,						M_DRV_DRV1,		0,				0,					0						},
	{ TP_ITEM,		M_DRV_DRV1_CHG,		"Change ...",			M_DRV_DRV1,		0,				f_drv_chg,			DRIVE_1					},

	{ TP_SUB,		M_DRV_DRV2,			"Drive 2:",				M_DRV,			0,				0,					0						},
	{ TP_RADIO,		M_DRV_DRV2_DUMMY,	0,						M_DRV_DRV2,		GRP_DRIVE2,		0,					0						},
	{ TP_RADIO,		M_DRV_DRV2_1,		"1",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			0						},
	{ TP_RADIO,		M_DRV_DRV2_2,		"2",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			1						},
	{ TP_RADIO,		M_DRV_DRV2_3,		"3",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			2						},
	{ TP_RADIO,		M_DRV_DRV2_4,		"4",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			3						},
	{ TP_RADIO,		M_DRV_DRV2_5,		"5",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			4						},
	{ TP_RADIO,		M_DRV_DRV2_6,		"6",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			5						},
	{ TP_RADIO,		M_DRV_DRV2_7,		"7",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			6						},
	{ TP_RADIO,		M_DRV_DRV2_8,		"8",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			7						},
	{ TP_RADIO,		M_DRV_DRV2_9,		"9",					M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			8						},
	{ TP_RADIO,		M_DRV_DRV2_NO,		"<No disk>",			M_DRV_DRV2,		GRP_DRIVE2,		f_drv_drv2,			-1						},
	{ TP_SEP,		0,					0,						M_DRV_DRV2,		0,				0,					0						},
	{ TP_ITEM,		M_DRV_DRV2_CHG,		"Change ...",			M_DRV_DRV2,		0,				f_drv_chg,			DRIVE_2					},

	{ TP_SEP,		0,					0,						M_DRV,			0,				0,					0						},
	{ TP_ITEM,		M_DRV_CHG,			"Set ...",				M_DRV,			0,				f_drv_chg,			-1						},
	{ TP_ITEM,		M_DRV_UNSET,		"Unset",				M_DRV,			0,				f_drv_unset,		-1						},

	/*------------------------------------------------------------------------------------------------------------------*/

	{ TP_SUB,		M_MISC,				"Misc",					M_TOP,			0,				0,					0						},

	{ TP_ITEM,		M_MISC_CAPTURE,		"Screen Capture",		M_MISC,			0,				f_misc_capture,		0						},

	{ TP_SUB,		M_MISC_CLOAD,		"Tape-Image [Load]",	M_MISC,			0,				0,					0						},
	{ TP_RADIO,		M_MISC_CLOAD_S_DUMMY,	0,					M_MISC_CLOAD,	GRP_CLOAD,		0,					0						},
	{ TP_RADIO,		M_MISC_CLOAD_S,		"Set ...",				M_MISC_CLOAD,	GRP_CLOAD,		f_misc_cload_s,		0						},
	{ TP_ITEM,		M_MISC_CLOAD_U,		"Unset",				M_MISC_CLOAD,	0,				f_misc_cload_u,		0						},

	{ TP_SUB,		M_MISC_CSAVE,		"Tape-Image [Save]",	M_MISC,			0,				0,					0						},
	{ TP_RADIO,		M_MISC_CSAVE_S_DUMMY,	0,					M_MISC_CSAVE,	GRP_CSAVE,		0,					0						},
	{ TP_RADIO,		M_MISC_CSAVE_S,		"Set ...",				M_MISC_CSAVE,	GRP_CSAVE,		f_misc_csave_s,		0						},
	{ TP_ITEM,		M_MISC_CSAVE_U,		"Unset",				M_MISC_CSAVE,	0,				f_misc_csave_u,		0						},

	{ TP_SEP,		0,					0,						M_MISC,			0,				0,					0						},

	{ TP_SUB,		M_MISC_SLOAD,		"State-Load",			M_MISC,			0,				0,					0						},
	{ TP_ITEM,		M_MISC_SLOAD_1,		"1",					M_MISC_SLOAD,	0,				f_misc_sload,		'1'						},
	{ TP_ITEM,		M_MISC_SLOAD_2,		"2",					M_MISC_SLOAD,	0,				f_misc_sload,		'2'						},
	{ TP_ITEM,		M_MISC_SLOAD_3,		"3",					M_MISC_SLOAD,	0,				f_misc_sload,		'3'						},
	{ TP_ITEM,		M_MISC_SLOAD_4,		"4",					M_MISC_SLOAD,	0,				f_misc_sload,		'4'						},
	{ TP_ITEM,		M_MISC_SLOAD_5,		"5",					M_MISC_SLOAD,	0,				f_misc_sload,		'5'						},
	{ TP_ITEM,		M_MISC_SLOAD_6,		"6",					M_MISC_SLOAD,	0,				f_misc_sload,		'6'						},
	{ TP_ITEM,		M_MISC_SLOAD_7,		"7",					M_MISC_SLOAD,	0,				f_misc_sload,		'7'						},
	{ TP_ITEM,		M_MISC_SLOAD_8,		"8",					M_MISC_SLOAD,	0,				f_misc_sload,		'8'						},
	{ TP_ITEM,		M_MISC_SLOAD_9,		"9",					M_MISC_SLOAD,	0,				f_misc_sload,		'9'						},

	{ TP_SUB,		M_MISC_SSAVE,		"State-Save",			M_MISC,			0,				0,					0						},
	{ TP_ITEM,		M_MISC_SSAVE_1,		"1",					M_MISC_SSAVE,	0,				f_misc_ssave,		'1'						},
	{ TP_ITEM,		M_MISC_SSAVE_2,		"2",					M_MISC_SSAVE,	0,				f_misc_ssave,		'2'						},
	{ TP_ITEM,		M_MISC_SSAVE_3,		"3",					M_MISC_SSAVE,	0,				f_misc_ssave,		'3'						},
	{ TP_ITEM,		M_MISC_SSAVE_4,		"4",					M_MISC_SSAVE,	0,				f_misc_ssave,		'4'						},
	{ TP_ITEM,		M_MISC_SSAVE_5,		"5",					M_MISC_SSAVE,	0,				f_misc_ssave,		'5'						},
	{ TP_ITEM,		M_MISC_SSAVE_6,		"6",					M_MISC_SSAVE,	0,				f_misc_ssave,		'6'						},
	{ TP_ITEM,		M_MISC_SSAVE_7,		"7",					M_MISC_SSAVE,	0,				f_misc_ssave,		'7'						},
	{ TP_ITEM,		M_MISC_SSAVE_8,		"8",					M_MISC_SSAVE,	0,				f_misc_ssave,		'8'						},
	{ TP_ITEM,		M_MISC_SSAVE_9,		"9",					M_MISC_SSAVE,	0,				f_misc_ssave,		'9'						},

	{ TP_SEP,		0,					0,						M_MISC,			0,				0,					0						},

	{ TP_CHECK,		M_MISC_TOOL,		"Show Tool",			M_MISC,			0,				f_misc_tool,		0						},
	{ TP_CHECK,		M_MISC_STATUS,		"Show Status",			M_MISC,			0,				f_misc_status,		0						},

	/*------------------------------------------------------------------------------------------------------------------*/
	{ TP_SUB,		M_HELP,				"Help",					M_TOP,			0,				0,					0						},

	{ TP_ITEM,		M_HELP_ABOUT,		"About",				M_HELP,			0,				f_help_about,		0						},
};


/***********************************************************************
 * メニューバー生成
 ************************************************************************/
void create_menubar(GtkWidget *target_window,
					GtkWidget **created_menubar)
{
	int i, show;
	T_MENUTABLE *p = &menutable[0];
	GtkWidget *menubar, *item, *submenu;

	parent_window = target_window;

	menubar_active = FALSE;

	menubar = gtk_menu_bar_new();
	mwidget[M_TOP].submenu = menubar;

	for (i = 0; i < COUNTOF(menutable); i++, p++) {

		show = TRUE;

		switch (p->type) {
		case TP_SUB:
			item = gtk_menu_item_new_with_label(p->label);

			submenu = gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
			mwidget[p->id].submenu = submenu;
			break;

		case TP_ITEM:
			item = gtk_menu_item_new_with_label(p->label);

			if (p->callback) {
				g_signal_connect(G_OBJECT(item), "activate",
								 G_CALLBACK(p->callback),
								 (gpointer)p->data);
			}
			break;

		case TP_CHECK:
			item = gtk_check_menu_item_new_with_label(p->label);

			if (p->callback) {
				g_signal_connect(G_OBJECT(item), "toggled",
								 G_CALLBACK(p->callback),
								 (gpointer)p->data);
			}
			break;

		case TP_RADIO:
			if (p->label) {
				item = gtk_radio_menu_item_new_with_label(mlist[p->group],
														  p->label);
			} else {
				item = gtk_radio_menu_item_new(mlist[p->group]);
				show = FALSE;
			}
			mlist[p->group]
				= gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));

			if (p->callback) {
				g_signal_connect(G_OBJECT(item), "toggled",
								 G_CALLBACK(p->callback),
								 (gpointer)p->data);
			}
			break;

		case TP_SEP:
			item = gtk_separator_menu_item_new();
			break;
		}

		mwidget[p->id].widget = item;

		if (show) {
			gtk_widget_show(item);
		}

		gtk_menu_shell_append(GTK_MENU_SHELL(mwidget[p->parent].submenu), item);
	}

	/* アクセラレータキーを設定する場合は、 target_window に対して登録する */

	*created_menubar = menubar;
}


/****************************************************************************
 * モード切り替え時の、メニューバーの処理再設定
 *      エミュモードとメニューモードで、メニューバーの内容を変更する
 *****************************************************************************/
static void menubar_item_setup(void);
static void menubar_item_sensitive(int sensitive);

void menubar_setup(int active)
{
	if (active) {

		/* メニューバーの内容を設定 */
		menubar_item_setup();

		/* 使えなくした項目を使えるようにする (エミュモード開始時) */
		menubar_item_sensitive(TRUE);

		menubar_active = TRUE;

	} else {

		/* ほとんどの項目を使えなくする (メニューモード開始時) */
		menubar_item_sensitive(FALSE);

		menubar_active = FALSE;
	}
}






/* System -> Reset メニューアイテムのラベルを更新する */
static void update_sys_reset(void)
{
	char buf[32];

	LOCK_MENUBAR()

	strcpy(buf, "Reset   [");

	switch (menubar_reset_cfg.boot_basic) {
	case BASIC_V2:
		strcat(buf, "V2");
		break;
	case BASIC_V1H:
		strcat(buf, "V1H");
		break;
	case BASIC_V1S:
		strcat(buf, "V1S");
		break;
	case BASIC_N:
		strcat(buf, "N");
		break;
	}

	strcat(buf, " : ");

	switch (menubar_reset_cfg.boot_clock_4mhz) {
	case CLOCK_4MHZ:
		strcat(buf, "4MHz");
		break;
	case CLOCK_8MHZ:
		strcat(buf, "8MHz");
		break;
	}

	strcat(buf, "]");

	gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[M_SYS_RESET].widget), buf);

	UNLOCK_MENUBAR()
}

/* System -> Mode にチェックをつける */
static void update_sys_mode(void)
{
	int uItem;

	LOCK_MENUBAR()

	quasi88_get_reset_cfg(&menubar_reset_cfg);

	switch (menubar_reset_cfg.boot_basic) {
	case BASIC_V2:
		uItem = M_SYS_MODE_V2;
		break;
	case BASIC_V1H:
		uItem = M_SYS_MODE_V1H;
		break;
	case BASIC_V1S:
		uItem = M_SYS_MODE_V1S;
		break;
	case BASIC_N:
		uItem = M_SYS_MODE_N;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	switch (menubar_reset_cfg.boot_clock_4mhz) {
	case CLOCK_4MHZ:
		uItem = M_SYS_MODE_4MH;
		break;
	case CLOCK_8MHZ:
		uItem = M_SYS_MODE_8MH;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	switch (menubar_reset_cfg.sound_board) {
	case SOUND_I:
		uItem = M_SYS_MODE_SB;
		break;
	case SOUND_II:
		uItem = M_SYS_MODE_SB2;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Speed -> nnn% にチェックをつける */
static void update_setting_speed_rate(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_wait_rate();
	switch (i) {
	case 25:
		uItem = M_SET_SPD_25;
		break;
	case 50:
		uItem = M_SET_SPD_50;
		break;
	case 100:
		uItem = M_SET_SPD_100;
		break;
	case 200:
		uItem = M_SET_SPD_200;
		break;
	case 400:
		uItem = M_SET_SPD_400;
		break;
	default:
		uItem = M_SET_SPD_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Speed -> No wait にチェックをつける */
static void update_setting_speed_nowait(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_no_wait();
	uItem = M_SET_SPD_MAX;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}

/* Setting -> Sub-CPU にチェックをつける */
static void update_setting_subcpu(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_subcpu_mode();
	switch (i) {
	case 0:
		uItem = M_SET_SUB_SOME;
		break;
	case 1:
		uItem = M_SET_SUB_OFT;
		break;
	case 2:
		uItem = M_SET_SUB_ALL;
		break;
	default:
		uItem = M_SET_SUB_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Use FDC-Wait にチェックをつける */
static void update_setting_fdcwait(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_fdc_wait();
	uItem = M_SET_FDCWAIT;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}

/* Setting -> Refresh Rate にチェックをつける */
static void update_setting_refreshrate(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_frameskip_rate();
	switch (i) {
	case 1:
		uItem = M_SET_REF_60;
		break;
	case 2:
		uItem = M_SET_REF_30;
		break;
	case 3:
		uItem = M_SET_REF_20;
		break;
	case 4:
		uItem = M_SET_REF_15;
		break;
	default:
		uItem = M_SET_REF_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Interlace にチェックをつける */
static void update_setting_interlace(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_interlace();
	switch (i) {
	case SCREEN_INTERLACE_NO:
		uItem = M_SET_INT_NO;
		break;
	case SCREEN_INTERLACE_SKIP:
		uItem = M_SET_INT_SKIP;
		break;
	case SCREEN_INTERLACE_YES:
		uItem = M_SET_INT_YES;
		break;
	default:
		uItem = M_SET_INT_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Screen size にチェックをつける */
static void update_setting_screensize(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_size();
	switch (i) {
	case SCREEN_SIZE_FULL:
		uItem = M_SET_SIZ_FULL;
		break;
	case SCREEN_SIZE_HALF:
		uItem = M_SET_SIZ_HALF;
		break;
	default:
		uItem = M_SET_SIZ_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Use PCG にチェックをつける */
static void update_setting_usepcg(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_use_pcg();
	uItem = M_SET_PCG;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}

/* Setting -> Mouse にチェックをつける */
static void update_setting_mouse(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_mouse_mode();
	switch (i) {
	case MOUSE_NONE:
		uItem = M_SET_MO_NO;
		break;
	case MOUSE_MOUSE:
		uItem = M_SET_MO_MOUSE;
		break;
	case MOUSE_JOYMOUSE:
		uItem = M_SET_MO_JOYMO;
		break;
	case MOUSE_JOYSTICK:
		uItem = M_SET_MO_JOY;
		break;
	default:
		uItem = M_SET_MO_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Cursor key にチェックをつける */
static void update_setting_cursorkey(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_cursor_key_mode();
	switch (i) {
	case 0:
		uItem = M_SET_CUR_DEF;
		break;
	case 1:
		uItem = M_SET_CUR_TEN;
		break;
	default:
		uItem = M_SET_CUR_DUMMY;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Setting -> Software numlock にチェックをつける */
static void update_setting_numlock(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_key_numlock();
	uItem = M_SET_NUMLOCK;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}

/* Setting -> Kana(Romaji) にチェックをつける */
static void update_setting_romajilock(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_now_key_romaji();
	uItem = M_SET_ROMAJI;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}


/* Drive メニューアイテムを表示する・隠す */
#define	NR_MENU_IMAGE	(9)
static void update_drive_image(int drv);
static void update_drive(void)
{
	int uItem;
	char buf[64];
	int i, drv;
	const struct {
		int top;
		int end;
		int none;
	} menuidx[] = {
		{ M_DRV_DRV1_1, M_DRV_DRV1_NO, M_DRV_DRV1_DUMMY, },
		{ M_DRV_DRV2_1, M_DRV_DRV2_NO, M_DRV_DRV2_DUMMY, },
	};
	int has_image = FALSE;

	LOCK_MENUBAR()

	for (drv = 0; drv < NR_DRIVE; drv ++) {
		if (disk_image_exist(drv)) {

			/* イメージの数の分、メニューアイテムを表示する。
			   ラベルは、イメージ名にセットし直す。
			   イメージ名をUTF8への変換しないと・・・ */
			for (i = 0; i < MIN(disk_image_num(drv), NR_MENU_IMAGE); i++) {
				uItem = i + menuidx[drv].top;

				sprintf(buf, "%d  ", i + 1);
				my_strncat(buf, drive[drv].image[i].name, sizeof(buf));

				gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[uItem].widget),
										buf);
				gtk_widget_show(mwidget[uItem].widget);
			}
			for (; i < NR_MENU_IMAGE; i++) {
				uItem = i + menuidx[drv].top;
				gtk_widget_hide(mwidget[uItem].widget);
			}
			has_image = TRUE;

		} else {

			/* ディスクがない場合、メニューアイテムを隠す */
			for (i = 0; i < NR_MENU_IMAGE; i++) {
				uItem = i + menuidx[drv].top;
				gtk_widget_hide(mwidget[uItem].widget);
			}
		}

		/* 選択中イメージの、ラジオメニューをアクティブにする */
		update_drive_image(drv);
	}

	/* メニューの名前を変えたり、無効にしたり */

	for (drv = 0; drv < NR_DRIVE; drv ++) {
		const char *s;
		s = filename_get_disk_name(drv);

		if (s) {
			sprintf(buf, "Drive %d: ", drv + 1);
			my_strncat(buf, s, sizeof(buf));
		} else {
			sprintf(buf, "Drive %d:", drv + 1);
		}
		i = (drv == DRIVE_1) ? M_DRV_DRV1 : M_DRV_DRV2;
		gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[i].widget), buf);
	}

	gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[M_DRV_CHG].widget),
							(has_image) ? "Change ..." : "Set ...");
	gtk_widget_set_sensitive(mwidget[M_DRV_UNSET].widget,
							 (has_image) ? TRUE : FALSE);

	UNLOCK_MENUBAR()
}
static void update_drive_image(int drv)
{
	int uItem;
	int i;
	const struct {
		int top;
		int end;
		int none;
	} menuidx[] = {
		{ M_DRV_DRV1_1, M_DRV_DRV1_NO, M_DRV_DRV1_DUMMY, },
		{ M_DRV_DRV2_1, M_DRV_DRV2_NO, M_DRV_DRV2_DUMMY, },
	};

	LOCK_MENUBAR()

	if (disk_image_exist(drv) == FALSE ||
		drive_check_empty(drv)) {
		/* ファイルなし or 空を選択 → NO Disk をチェック */

		uItem = menuidx[drv].end;

	} else {
		i = disk_image_selected(drv);
		if (0 <= i && i <= NR_MENU_IMAGE) {
			/* 1..9番目選択 → その項目をチェック */
			uItem = i + menuidx[drv].top;
		} else {
			/* 10番目.. → チェックなし */
			uItem = menuidx[drv].none;
		}
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()
}

/* Tape Load メニューアイテムのラベルを変えたり使用不可にしたり */
static void update_misc_cload(void)
{
	int uItem;
	const char *s;
	char buf[64];

	LOCK_MENUBAR()

	s = filename_get_tape_name(CLOAD);

	/* テープありならファイル名を、なしならデフォルトのラベルを表示 */
	uItem = M_MISC_CLOAD_S;
	{
		if (s) {
			my_strncpy(buf, s, sizeof(buf));
		} else   {
			strcpy(buf, "Set ...");
		}
		gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[uItem].widget), buf);
	}

	/* テープありなら、ラジオメニューをアクティブに */
	if (s) {
		uItem = M_MISC_CLOAD_S;
	} else {
		uItem = M_MISC_CLOAD_S_DUMMY;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	/* テープありなら unset を表示、なしなら隠す */
	uItem = M_MISC_CLOAD_U;
	gtk_widget_set_sensitive(mwidget[uItem].widget, (s) ? TRUE : FALSE);

	UNLOCK_MENUBAR()
}

/* Tape Save メニューアイテムのラベルを変えたり使用不可にしたり */
static void update_misc_csave(void)
{
	int uItem;
	const char *s;
	char buf[64];

	LOCK_MENUBAR()

	s = filename_get_tape_name(CSAVE);

	/* テープありならファイル名を、なしならデフォルトのラベルを表示 */
	uItem = M_MISC_CSAVE_S;
	{
		if (s) {
			my_strncpy(buf, s, sizeof(buf));
		} else   {
			strcpy(buf, "Set ...");
		}
		gtk_menu_item_set_label(GTK_MENU_ITEM(mwidget[uItem].widget), buf);
	}

	/* テープありなら、ラジオメニューをアクティブに */
	if (s) {
		uItem = M_MISC_CSAVE_S;
	} else {
		uItem = M_MISC_CSAVE_S_DUMMY;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	/* テープありなら unset を表示、なしなら隠す */
	uItem = M_MISC_CSAVE_U;
	gtk_widget_set_sensitive(mwidget[uItem].widget, (s) ? TRUE : FALSE);

	UNLOCK_MENUBAR()
}

/* Misc -> Tool にチェックをつける */
static void update_misc_tool(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_toolbar();
	uItem = M_MISC_TOOL;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}

/* Misc -> Status にチェックをつける */
static void update_misc_status(void)
{
	int uItem;
	int i;

	LOCK_MENUBAR()

	i = quasi88_cfg_now_statusbar();
	uItem = M_MISC_STATUS;
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), i);

	UNLOCK_MENUBAR()
}




/*======================================================================
 * メニューバーの内容を再初期化
 *======================================================================*/
static void menubar_item_setup(void)
{
	int uItem;
	int i;
	GtkWidget *w;

	/* System -----------------------------------------------------------*/

	update_sys_mode();
	update_sys_reset();

	/* Setting ----------------------------------------------------------*/

	update_setting_speed_rate();

	update_setting_speed_nowait();

	update_setting_subcpu();

	update_setting_fdcwait();

	update_setting_refreshrate();

	update_setting_interlace();

	update_setting_screensize();

	update_setting_usepcg();

	update_setting_mouse();

	update_setting_cursorkey();

	update_setting_numlock();

	update_setting_romajilock();

	/* Drive ------------------------------------------------------------*/

	update_drive();

	/* Misc -------------------------------------------------------------*/

	update_misc_cload();

	update_misc_csave();

	update_misc_tool();

	update_misc_status();
}

/*======================================================================
 * メニューバー使用可能項目を設定
 *======================================================================*/
static void menubar_item_sensitive(int sensitive)
{
	gtk_widget_set_sensitive(mwidget[M_SYS_RESET    ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_MODE     ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_RESET_V2 ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_RESET_V1H].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_RESET_V1S].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_MENU     ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_SYS_SAVE     ].widget, sensitive);

	gtk_widget_set_sensitive(mwidget[M_SET ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_DRV ].widget, sensitive);
	gtk_widget_set_sensitive(mwidget[M_MISC].widget, sensitive);
}


/***********************************************************************
 * メニューバーコールバック関数
 ************************************************************************/

/*----------------------------------------------------------------------
 * System メニュー
 *----------------------------------------------------------------------*/

static void f_sys_reset(GtkMenuItem *widget, gpointer data)
{
	if (menubar_reset_cfg.boot_clock_4mhz) {
		cpu_clock_mhz = CONST_4MHZ_CLOCK;
	} else {
		cpu_clock_mhz = CONST_8MHZ_CLOCK;
	}

	if (drive_check_empty(DRIVE_1)) {
		menubar_reset_cfg.boot_from_rom = TRUE;
	} else {
		menubar_reset_cfg.boot_from_rom = FALSE;
	}

	quasi88_reset(&menubar_reset_cfg);
}

static void f_sys_reset_as(GtkMenuItem *widget, gpointer data)
{
	int uItem;

	LOCK_MENUBAR()

	menubar_reset_cfg.boot_basic = (int)data;
	switch (menubar_reset_cfg.boot_basic) {
	case BASIC_V2:
		uItem = M_SYS_MODE_V2;
		break;
	case BASIC_V1H:
		uItem = M_SYS_MODE_V1H;
		break;
	case BASIC_V1S:
		uItem = M_SYS_MODE_V1S;
		break;
	case BASIC_N:
		uItem = M_SYS_MODE_N;
		break;
	}
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(mwidget[uItem].widget), TRUE);

	UNLOCK_MENUBAR()

	update_sys_reset();
	f_sys_reset(0, 0);
}

static void f_sys_basic(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		menubar_reset_cfg.boot_basic = (int)data;
		update_sys_reset();
	}
}

static void f_sys_clock(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		menubar_reset_cfg.boot_clock_4mhz = (int)data;
		update_sys_reset();
	}
}

static void f_sys_sb(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		menubar_reset_cfg.sound_board = (int)data;
		update_sys_reset();
	}
}

static void f_sys_menu(GtkMenuItem *widget, gpointer data)
{
	quasi88_menu();
}

static void f_sys_save(GtkMenuItem *widget, gpointer data)
{
	config_save(NULL);
}

static void f_sys_exit(GtkMenuItem *widget, gpointer data)
{
	quasi88_quit();
}

/*----------------------------------------------------------------------
 * Setting メニュー
 *----------------------------------------------------------------------*/

static void f_set_speed(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_cfg_set_wait_rate((int)data);
	}
}

static void f_set_nowait(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_cfg_set_no_wait(active);
}

static void f_set_subcpu(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		if (quasi88_now_subcpu_mode() != (int)data) {
			quasi88_set_subcpu_mode((int)data);
		}
	}
}

static void f_set_fdcwait(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_set_fdc_wait(active);
}

static void f_set_refresh(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_cfg_set_frameskip_rate((int)data);
	}
}

static void f_set_interlace(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_cfg_set_interlace((int)data);
	}
}

static void f_set_size(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_cfg_set_size((int)data);
	}
}

static void f_set_pcg(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_set_use_pcg(active);
}

static void f_set_mouse(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_set_mouse_mode((int)data);
	}
}

static void f_set_cursor(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		quasi88_set_cursor_key_mode((int)data);
	}
}

static void f_set_numlock(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_cfg_key_numlock(active);
}

static void f_set_romaji(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_cfg_key_romaji(active);
}

/*----------------------------------------------------------------------
 * Drive メニュー
 *----------------------------------------------------------------------*/

static void f_drv_chg(GtkMenuItem *widget, gpointer data)
{
	GtkWidget *fsel;
	gint res;
	const char *headline;

	if (menubar_active == FALSE) {
		return;
	}

	switch ((int)data) {
	case DRIVE_1:
		headline = "Open Disk-Image-File (Drive 1:)";
		break;
	case DRIVE_2:
		headline = "Open Disk-Image-File (Drive 2:)";
		break;
	default:
		headline = "Open Disk-Image-File";
		break;
	}

	fsel = gtk_file_chooser_dialog_new(headline,
									   GTK_WINDOW(parent_window),
									   GTK_FILE_CHOOSER_ACTION_OPEN,
									   "_Cancel",
									   GTK_RESPONSE_CANCEL,
									   "_Open",
									   GTK_RESPONSE_ACCEPT,
									   NULL);

	res = gtk_dialog_run(GTK_DIALOG(fsel));

	if (res == GTK_RESPONSE_ACCEPT) {
		int ok = FALSE;
		int ro = FALSE;
		int data_drv_chg = (int)data;

		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fsel));

		if ((data_drv_chg == DRIVE_1) || (data_drv_chg == DRIVE_2)) {

			ok = quasi88_disk_insert(data_drv_chg, filename, 0, ro);

		} else if (data_drv_chg < 0) {

			ok = quasi88_disk_insert_all(filename, ro);

		}

		/* すでにファイルを閉じているので、失敗してもメニューバー更新 */
		update_drive();

		g_free (filename);
	}

	gtk_widget_destroy(fsel);
}

static void f_drv_drv1(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		if ((int)data <  0) {
			quasi88_disk_image_empty(DRIVE_1);
		} else {
			quasi88_disk_image_select(DRIVE_1, (int)data);
		}
	}
}

static void f_drv_drv2(GtkRadioMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
		if ((int)data <  0) {
			quasi88_disk_image_empty(DRIVE_2);
		} else {
			quasi88_disk_image_select(DRIVE_2, (int)data);
		}
	}
}

static void f_drv_unset(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_disk_eject_all();

	update_drive();
}

/*----------------------------------------------------------------------
 * Misc メニュー
 *----------------------------------------------------------------------*/

static void f_misc_capture(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_screen_snapshot();
}

static void f_misc_cload_s(GtkMenuItem *widget, gpointer data)
{
	GtkWidget *fsel;
	gint res;

	if (menubar_active == FALSE) {
		return;
	}

	if (filename_get_tape(CLOAD)) {
		return;		/* テープありなら戻る */
	}

	fsel = gtk_file_chooser_dialog_new("Open Tape-Image-File for LOAD",
									   GTK_WINDOW(parent_window),
									   GTK_FILE_CHOOSER_ACTION_OPEN,
									   "_Cancel",
									   GTK_RESPONSE_CANCEL,
									   "_Open",
									   GTK_RESPONSE_ACCEPT,
									   NULL);

	res = gtk_dialog_run(GTK_DIALOG(fsel));

	if (res == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fsel));
		int ok = quasi88_load_tape_insert(filename);

		/* すでにファイルを閉じているので、失敗してもメニューバー更新 */
		update_misc_cload();

		g_free (filename);
	}

	gtk_widget_destroy(fsel);
}

static void f_misc_cload_u(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_load_tape_eject();

	update_misc_cload();
}

static void f_misc_csave_s(GtkMenuItem *widget, gpointer data)
{
	GtkWidget *fsel;
	gint res;

	if (menubar_active == FALSE) {
		return;
	}

	if (filename_get_tape(CSAVE)) {
		return;		/* テープありなら戻る */
	}

	fsel = gtk_file_chooser_dialog_new("Open Tape-Image-File for SAVE (append)",
									   GTK_WINDOW(parent_window),
									   GTK_FILE_CHOOSER_ACTION_SAVE,
									   "_Cancel",
									   GTK_RESPONSE_CANCEL,
									   "_Open",
									   GTK_RESPONSE_ACCEPT,
									   NULL);

	res = gtk_dialog_run(GTK_DIALOG(fsel));

	if (res == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fsel));
		int ok = quasi88_save_tape_insert(filename);

		/* すでにファイルを閉じているので、失敗してもメニューバー更新 */
		update_misc_csave();

		g_free (filename);
	}

	gtk_widget_destroy(fsel);
}

static void f_misc_csave_u(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_save_tape_eject();

	update_misc_csave();
}

static void f_misc_sload(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_stateload((int) data);

	/* 設定やファイル名が変更されたはずなので、メニューバーを全て更新 */
	menubar_setup(TRUE);
}

static void f_misc_ssave(GtkMenuItem *widget, gpointer data)
{
	if (menubar_active == FALSE) {
		return;
	}

	quasi88_statesave((int) data);
}

static void f_misc_tool(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_cfg_set_toolbar(active);
}

static void f_misc_status(GtkCheckMenuItem *widget, gpointer data)
{
	int active;

	if (menubar_active == FALSE) {
		return;
	}

	active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))
		? TRUE : FALSE;

	quasi88_cfg_set_statusbar(active);
}

/*----------------------------------------------------------------------
 * Help メニュー
 *----------------------------------------------------------------------*/

static void f_help_about(GtkMenuItem *widget, gpointer data)
{
	GtkWidget *dialog, *label, *content_area;
	int i;
	static const char *message[] = {
		"",
		"QUASI88  ver. " Q_VERSION "  <" Q_COMMENT ">",
		"  " Q_COPYRIGHT,
		"",
#ifdef  USE_SOUND
		"MAME Sound-Engine included",
		"  " Q_MAME_COPYRIGHT,
#ifdef  USE_FMGEN
		"FM Sound Generator (fmgen) included",
		"  " Q_FMGEN_COPYRIGHT,
#endif
		"",
#endif
	};

	dialog = gtk_dialog_new_with_buttons("About...",
										 GTK_WINDOW(parent_window),
										 (GTK_DIALOG_MODAL |
										  GTK_DIALOG_DESTROY_WITH_PARENT),
										 "_OK",
										 GTK_RESPONSE_ACCEPT,
										  NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	/*----*/

	for (i = 0; i < COUNTOF(message); i++) {
		label = gtk_label_new(message[i]);
		gtk_container_add(GTK_CONTAINER(content_area) , label);
	}

	/*----*/

	gtk_widget_set_size_request(dialog, 400, -1);
	gtk_widget_show_all(dialog);

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
}

/***********************************************************************
 *
 ************************************************************************/
void menubar_controll(int controll)
{
	switch (controll) {

	case CTRL_RESET:
		update_sys_mode();
		update_sys_reset();
		break;

	case CTRL_CHG_SUBCPU:
		update_setting_subcpu();
		break;

	case CTRL_CHG_FDCWAIT:
		update_setting_fdcwait();
		break;

	case CTRL_CHG_INTERLACE:
		update_setting_interlace();
		break;

	case CTRL_CHG_PCG:
		update_setting_usepcg();
		break;

	case CTRL_CHG_MOUSE:
		update_setting_mouse();
		break;

	case CTRL_CHG_CURSOR:
		update_setting_cursorkey();
		break;

	case CTRL_CHG_FRAMERATE:
		update_setting_refreshrate();
		break;

	case CTRL_CHG_VOLUME:
		/* DO NOTHING */
		break;

	case CTRL_CHG_SCREENSIZE:
		update_setting_screensize();
		break;

	case CTRL_CHG_FULLSCREEN:
		/* DO NOTHING */
		break;

	case CTRL_CHG_WAIT:
		update_setting_speed_rate();
		break;

	case CTRL_CHG_NOWAIT:
		update_setting_speed_nowait();
		break;

	case CTRL_CHG_DRIVE1_EMPTY:
	case CTRL_CHG_DRIVE1_IMAGE:
		update_drive_image(DRIVE_1);
		break;

	case CTRL_CHG_DRIVE2_EMPTY:
	case CTRL_CHG_DRIVE2_IMAGE:
		update_drive_image(DRIVE_2);
		break;

	case CTRL_CHG_CAPSLOCK:
		/* DO NOTHING */
		break;
	case CTRL_CHG_KANALOCK:
		/* DO NOTHING */
		break;

	case CTRL_CHG_ROMAJILOCK:
		update_setting_romajilock();
		break;

	case CTRL_CHG_NUMLOCK:
		update_setting_numlock();
		break;

	case CTRL_CHG_TOOLBAR:
		update_misc_tool();
		break;

	case CTRL_CHG_STATUSBAR:
		update_misc_status();
		break;
	}
}
