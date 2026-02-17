/*===========================================================================
 *
 * メインページ リセット
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "menu.h"

#include "memory.h"
#include "intr.h"
#include "soundbd.h"

#include "event.h"
#include "q8tk.h"

#include "menu-common.h"

#include "menu-reset.h"
#include "menu-reset-message.h"


static int menu_boot_clock_async;		/* リセット時にクロック指定同期する? */

T_RESET_CFG reset_req;					/* リセット時に要求する設定を、保存 */

/* 起動デバイスの制御に必要 */
static Q8tkWidget *widget_reset_boot;
Q8tkWidget        *widget_dipsw_b_boot_disk;
Q8tkWidget        *widget_dipsw_b_boot_rom;


/* メニュー下部のリセットと、リセットタグのリセットを連動させたいので、
 * 片方が選択されたら、反対のも選択されるように、ウィジットを記憶 */
Q8tkWidget *widget_reset_basic[2][4];
Q8tkWidget *widget_reset_clock[2][2];


/*----------------------------------------------------------------------*/
/* BASICモード */
static int get_reset_basic(void)
{
	return reset_req.boot_basic;
}
static void cb_reset_basic(UNUSED_WIDGET, void *p)
{
	if (reset_req.boot_basic != P2INT(p)) {
		reset_req.boot_basic = P2INT(p);

		q8tk_toggle_button_set_state(widget_reset_basic[ 1 ][ P2INT(p) ], TRUE);
	}
}


static Q8tkWidget *menu_reset_basic(void)
{
	Q8tkWidget *box;
	Q8List *list;

	box = PACK_VBOX(NULL);
	{
		list = PACK_RADIO_BUTTONS(box,
								  data_reset_basic, COUNTOF(data_reset_basic),
								  get_reset_basic(), cb_reset_basic);

		/* リストを手繰って、全ウィジットを取得 */
		widget_reset_basic[0][BASIC_V2 ] = list->data;
		list = list->next;
		widget_reset_basic[0][BASIC_V1H] = list->data;
		list = list->next;
		widget_reset_basic[0][BASIC_V1S] = list->data;
		list = list->next;
		widget_reset_basic[0][BASIC_N  ] = list->data;
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* CPU クロック */
static int get_reset_clock(void)
{
	return reset_req.boot_clock_4mhz;
}
static void cb_reset_clock(UNUSED_WIDGET, void *p)
{
	if (reset_req.boot_clock_4mhz != P2INT(p)) {
		reset_req.boot_clock_4mhz = P2INT(p);

		q8tk_toggle_button_set_state(widget_reset_clock[ 1 ][ P2INT(p) ], TRUE);
	}
}
static int get_reset_clock_async(void)
{
	return menu_boot_clock_async;
}
static void cb_reset_clock_async(Q8tkWidget *widget, UNUSED_PARM)
{
	int async = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	menu_boot_clock_async = async;
}


static Q8tkWidget *menu_reset_clock(void)
{
	Q8tkWidget *box, *box2;
	Q8List *list;

	box = PACK_VBOX(NULL);
	{
		list = PACK_RADIO_BUTTONS(box,
								  data_reset_clock, COUNTOF(data_reset_clock),
								  get_reset_clock(), cb_reset_clock);

		/* リストを手繰って、全ウィジットを取得 */
		widget_reset_clock[0][CLOCK_4MHZ] = list->data;
		list = list->next;
		widget_reset_clock[0][CLOCK_8MHZ] = list->data;

		/* 空行 */
		PACK_LABEL(box, "");

		box2 = PACK_HBOX(box);
		{
			/* インデント */
			PACK_LABEL(box2, "  ");

			PACK_CHECK_BUTTON(box2,
							  GET_LABEL(data_reset_clock_async, 0),
							  get_reset_clock_async(),
							  cb_reset_clock_async, NULL);
		}
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* サウンドボード */
static int get_reset_sound(void)
{
	return reset_req.sound_board;
}
static void cb_reset_sound(UNUSED_WIDGET, void *p)
{
	reset_req.sound_board = P2INT(p);
}


static Q8tkWidget *menu_reset_sound(void)
{
	Q8tkWidget *box;

	box = PACK_VBOX(NULL);
	{
		PACK_RADIO_BUTTONS(box,
						   data_reset_sound, COUNTOF(data_reset_sound),
						   get_reset_sound(), cb_reset_sound);
		/* 空行 */
		PACK_LABEL(box, "");
		PACK_LABEL(box, "");
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* 起動 */
void set_reset_dipsw_boot(void)
{
	const t_menulabel *l = data_reset_boot;

	if (widget_reset_boot) {
		q8tk_label_set(widget_reset_boot,
					   (reset_req.boot_from_rom
						? GET_LABEL(l, 1) : GET_LABEL(l, 0)));
	}
}


static Q8tkWidget *menu_reset_boot(void)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(NULL);
	{
		widget_reset_boot = PACK_LABEL(vbox, "");
		set_reset_dipsw_boot();
	}
	return vbox;
}

/*----------------------------------------------------------------------*/
/* その他 */
static Q8tkWidget *reset_detail_widget;
static int         reset_detail_hide;
static Q8tkWidget *reset_detail_button;

static void cb_reset_detail(UNUSED_WIDGET, UNUSED_PARM)
{
	reset_detail_hide ^= 1;

	if (reset_detail_hide) {
		q8tk_widget_hide(reset_detail_widget);
	} else {
		q8tk_widget_show(reset_detail_widget);
	}
	q8tk_label_set(reset_detail_button->child,
				   GET_LABEL(data_reset_detail, reset_detail_hide));
}


static Q8tkWidget *menu_reset_detail(void)
{
	Q8tkWidget *box;

	box = PACK_VBOX(NULL);
	{
		PACK_LABEL(box, "");

		reset_detail_hide = 0;
		reset_detail_button = PACK_BUTTON(box,
										  "",
										  cb_reset_detail, NULL);
		cb_reset_detail(NULL, NULL);

		PACK_LABEL(box, "");
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* ディップスイッチ */
static void dipsw_create(void);
static void dipsw_start(void);
static void dipsw_finish(void);

static void cb_reset_dipsw(UNUSED_WIDGET, UNUSED_PARM)
{
	dipsw_start();
}


static Q8tkWidget *menu_reset_dipsw(void)
{
	Q8tkWidget *button;

	button = PACK_BUTTON(NULL,
						 GET_LABEL(data_reset, DATA_RESET_DIPSW_BTN),
						 cb_reset_dipsw, NULL);
	q8tk_misc_set_placement(button, Q8TK_PLACEMENT_X_CENTER, 0);

	return button;
}

/*----------------------------------------------------------------------*/
/* ROMバージョン */
static int get_reset_version(void)
{
	return reset_req.set_version;
}
static void cb_reset_version(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_reset_version;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_reset_version); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			reset_req.set_version = p->val;
			break;
		}
	}
}


static Q8tkWidget *menu_reset_version(void)
{
	Q8tkWidget *box, *combo;
	char wk[4];

	wk[0] = get_reset_version();
	wk[1] = '\0';

	box = PACK_VBOX(NULL);
	{
		combo = PACK_COMBO(box,
						   data_reset_version, COUNTOF(data_reset_version),
						   get_reset_version(), wk, 8,
						   cb_reset_version, NULL,
						   NULL, NULL);
		/* 空行 */
		PACK_LABEL(box, "");
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* 拡張RAM */
static int get_reset_extram(void)
{
	return reset_req.use_extram;
}
static void cb_reset_extram(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_reset_extram;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_reset_extram); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			reset_req.use_extram = p->val;
			break;
		}
	}
}


static Q8tkWidget *menu_reset_extram(void)
{
	Q8tkWidget *box, *combo;
	char wk[16];

	sprintf(wk, "  %5dKB", use_extram * 128);

	box = PACK_VBOX(NULL);
	{
		combo = PACK_COMBO(box,
						   data_reset_extram, COUNTOF(data_reset_extram),
						   get_reset_extram(), wk, 0,
						   cb_reset_extram, NULL,
						   NULL, NULL);
		/* 空行 */
		PACK_LABEL(box, "");
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* 辞書ROM */
static int get_reset_jisho(void)
{
	return reset_req.use_jisho_rom;
}
static void cb_reset_jisho(UNUSED_WIDGET, void *p)
{
	reset_req.use_jisho_rom = P2INT(p);
}


static Q8tkWidget *menu_reset_jisho(void)
{
	Q8tkWidget *box;

	box = PACK_VBOX(NULL);
	{
		PACK_RADIO_BUTTONS(box,
						   data_reset_jisho, COUNTOF(data_reset_jisho),
						   get_reset_jisho(), cb_reset_jisho);
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* 機種、BASICモード、クロック、などを表示 */
static Q8tkWidget *menu_reset_current(void)
{
	static const char *type[] = {
		"PC-8801",
		"PC-8801",
		"PC-8801",
		"PC-8801mkII",
		"PC-8801mkIISR",
		"PC-8801mkIITR/FR/MR",
		"PC-8801mkIITR/FR/MR",
		"PC-8801mkIITR/FR/MR",
		"PC-8801FH/MH",
		"PC-8801FA/MA/FE/MA2/FE2/MC",
	};
	static const char *basic[] = { " N ", "V1S", "V1H", " V2", };
	static const char *clock[] = { "8MHz", "4MHz", };
	const char *t = "";
	const char *b = "";
	const char *c = "";
	int i;
	char wk[100], ext[40];

	i = (ROM_VERSION & 0xff) - '0';
	if (0 <= i && i < COUNTOF(type)) {
		t = type[ i ];
	}

	i = get_reset_basic();
	if (0 <= i && i < COUNTOF(basic)) {
		b = basic[ i ];
	}

	i = get_reset_clock();
	if (0 <= i && i < COUNTOF(clock)) {
		c = clock[ i ];
	}

	ext[0] = 0;
	{
		if (sound_port) {
			if (ext[0] == 0) {
				strcat(ext, "(");
			} else {
				strcat(ext, ", ");
			}
			if (sound_board == SOUND_I) {
				strcat(ext, "OPN");
			} else {
				strcat(ext, "OPNA");
			}
		}

		if (use_extram) {
			if (ext[0] == 0) {
				strcat(ext, "(");
			} else {
				strcat(ext, ", ");
			}
			sprintf(wk, "%dKB", use_extram * 128);
			strcat(ext, wk);
			strcat(ext, GET_LABEL(data_reset_current, 0));
		}

		if (use_jisho_rom) {
			if (ext[0] == 0) {
				strcat(ext, "(");
			} else {
				strcat(ext, ", ");
			}
			strcat(ext, GET_LABEL(data_reset_current, 1));
		}
	}
	if (ext[0]) {
		strcat(ext, ")");
	}


	sprintf(wk, " %-30s  %4s  %4s  %30s ",
			t, b, c, ext);

	return PACK_LABEL(NULL, wk);
}

/*----------------------------------------------------------------------*/
/* この設定でリセットする */
void cb_reset_now(UNUSED_WIDGET, UNUSED_PARM)
{
	/* CLOCK設定と、CPUクロックを同期させる */
	if (menu_boot_clock_async == FALSE) {
		cpu_clock_mhz
			= reset_req.boot_clock_4mhz ? CONST_4MHZ_CLOCK : CONST_8MHZ_CLOCK;
	}

	/* reset_req の設定に基づき、リセット → 実行 */
	quasi88_reset(&reset_req);

	quasi88_exec(); /* ← q8tk_main_quit() 呼出済み */

#if 0
	printf("boot_dipsw      %04x\n", boot_dipsw);
	printf("boot_from_rom   %d\n", boot_from_rom);
	printf("boot_basic      %d\n", boot_basic);
	printf("boot_clock_4mhz %d\n", boot_clock_4mhz);
	printf("ROM_VERSION     %c\n", ROM_VERSION);
	printf("baudrate_sw     %d\n", baudrate_sw);
#endif
}

/*======================================================================*/

Q8tkWidget *menu_reset(void)
{
	Q8tkWidget *hbox, *vbox;
	Q8tkWidget *w, *f;
	const t_menulabel *l = data_reset;

	vbox = PACK_VBOX(NULL);
	{
		f = PACK_FRAME(vbox, "", menu_reset_current());
		q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_ETCHED_OUT);

		hbox = PACK_HBOX(vbox);
		{
			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_RESET_BASIC), menu_reset_basic());

			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_RESET_CLOCK), menu_reset_clock());

			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_RESET_SOUND), menu_reset_sound());

			w = PACK_FRAME(hbox,
						   GET_LABEL(l, DATA_RESET_BOOT), menu_reset_boot());
			q8tk_frame_set_shadow_type(w, Q8TK_SHADOW_ETCHED_IN);
		}

		PACK_LABEL(vbox, GET_LABEL(l, DATA_RESET_NOTICE));

		hbox = PACK_HBOX(vbox);
		{
			reset_detail_widget = PACK_HBOX(NULL);

			q8tk_box_pack_start(hbox, menu_reset_detail());
			q8tk_box_pack_start(hbox, reset_detail_widget);
			{
				PACK_VSEP(reset_detail_widget);

				f = PACK_FRAME(reset_detail_widget,
							   GET_LABEL(l, DATA_RESET_DIPSW),
							   menu_reset_dipsw());

				q8tk_misc_set_placement(f, 0, Q8TK_PLACEMENT_Y_CENTER);

				f = PACK_FRAME(reset_detail_widget,
							   GET_LABEL(l, DATA_RESET_VERSION),
							   menu_reset_version());

				q8tk_misc_set_placement(f, 0, Q8TK_PLACEMENT_Y_BOTTOM);

				f = PACK_FRAME(reset_detail_widget,
							   GET_LABEL(l, DATA_RESET_EXTRAM),
							   menu_reset_extram());

				q8tk_misc_set_placement(f, 0, Q8TK_PLACEMENT_Y_BOTTOM);

				f = PACK_FRAME(reset_detail_widget,
							   GET_LABEL(l, DATA_RESET_JISHO),
							   menu_reset_jisho());

				q8tk_misc_set_placement(f, 0, Q8TK_PLACEMENT_Y_BOTTOM);
			}

		}

		hbox = PACK_HBOX(vbox);
		{
			w = PACK_LABEL(hbox, GET_LABEL(l, DATA_RESET_INFO));
			q8tk_misc_set_placement(w, 0, Q8TK_PLACEMENT_Y_CENTER);

			w = PACK_BUTTON(hbox,
							GET_LABEL(data_reset, DATA_RESET_NOW),
							cb_reset_now, NULL);
		}
		q8tk_misc_set_placement(hbox, Q8TK_PLACEMENT_X_RIGHT, 0);
	}

	/* ディップスイッチウインドウ生成 */
	dipsw_create();

	return vbox;
}



/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 *
 * サブウインドウ  DIPSW
 *
 * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

static Q8tkWidget *dipsw_window_base;
static Q8tkWidget *dipsw_window_focus;

/*----------------------------------------------------------------------*/
/* 初期設定 */
static int get_dipsw_b(int p)
{
	int shift = data_dipsw_b[p].val;

	return ((p << 1) | ((reset_req.boot_dipsw >> shift) & 1));
}
static void cb_dipsw_b(UNUSED_WIDGET, void *p)
{
	int shift = data_dipsw_b[ P2INT(p) >> 1 ].val;
	int on = P2INT(p) & 1;

	if (on) {
		reset_req.boot_dipsw |= (1 << shift);
	} else {
		reset_req.boot_dipsw &= ~(1 << shift);
	}
}
static int get_dipsw_b2(void)
{
	return (reset_req.boot_from_rom) ? TRUE : FALSE;
}
static void cb_dipsw_b2(UNUSED_WIDGET, void *p)
{
	if (P2INT(p)) {
		reset_req.boot_from_rom = TRUE;
	} else {
		reset_req.boot_from_rom = FALSE;
	}

	set_reset_dipsw_boot();
}


static Q8tkWidget *menu_dipsw_b(void)
{
	int i;
	Q8tkWidget *vbox, *hbox;
	Q8tkWidget *b = NULL;
	const t_dipsw *pd;
	const t_menudata *p;

	vbox = PACK_VBOX(NULL);
	{
		pd = data_dipsw_b;
		for (i = 0; i < COUNTOF(data_dipsw_b); i++, pd++) {

			hbox = PACK_HBOX(vbox);
			{
				PACK_LABEL(hbox, GET_LABEL(pd, 0));

				PACK_RADIO_BUTTONS(hbox,
								   pd->p, 2,
								   get_dipsw_b(i), cb_dipsw_b);
			}
		}

		hbox = PACK_HBOX(vbox);
		{
			pd = data_dipsw_b2;
			p  = pd->p;

			PACK_LABEL(hbox, GET_LABEL(pd, 0));

			for (i = 0; i < 2; i++, p++) {
				b = PACK_RADIO_BUTTON(hbox,
									  b,
									  GET_LABEL(p, 0),
									  (get_dipsw_b2() == p->val) ? TRUE : FALSE,
									  cb_dipsw_b2, INT2P(p->val));

				/*これらのボタンは覚えておく */
				if (i == 0) {
					widget_dipsw_b_boot_disk = b;
				} else {
					widget_dipsw_b_boot_rom  = b;
				}
			}
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* RS-232C設定 */
static int get_dipsw_r(int p)
{
	int shift = data_dipsw_r[p].val;

	return ((p << 1) | ((reset_req.boot_dipsw >> shift) & 1));
}
static void cb_dipsw_r(UNUSED_WIDGET, void *p)
{
	int shift = data_dipsw_r[ P2INT(p) >> 1 ].val;
	int on    = P2INT(p) & 1;

	if (on) {
		reset_req.boot_dipsw |= (1 << shift);
	} else {
		reset_req.boot_dipsw &= ~(1 << shift);
	}
}
static int get_dipsw_r_baudrate(void)
{
	return reset_req.baudrate_sw;
}
static void cb_dipsw_r_baudrate(Q8tkWidget *widget, UNUSED_PARM)
{
	int i;
	const t_menudata *p = data_dipsw_r_baudrate;
	const char *combo_str = q8tk_combo_get_text(widget);

	for (i = 0; i < COUNTOF(data_dipsw_r_baudrate); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			reset_req.baudrate_sw = p->val;
			return;
		}
	}
}


static Q8tkWidget *menu_dipsw_r(void)
{
	int i;
	Q8tkWidget *vbox, *hbox;
	const t_dipsw *pd;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, GET_LABEL(data_dipsw_r2, 0));

			PACK_COMBO(hbox,
					   data_dipsw_r_baudrate,
					   COUNTOF(data_dipsw_r_baudrate),
					   get_dipsw_r_baudrate(), NULL, 8,
					   cb_dipsw_r_baudrate, NULL,
					   NULL, NULL);
		}

		pd = data_dipsw_r;
		for (i = 0; i < COUNTOF(data_dipsw_r); i++, pd++) {

			hbox = PACK_HBOX(vbox);
			{
				PACK_LABEL(hbox, GET_LABEL(data_dipsw_r, i));

				PACK_RADIO_BUTTONS(hbox,
								   pd->p, 2,
								   get_dipsw_r(i), cb_dipsw_r);
			}
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/

static void cb_reset_dipsw_end(UNUSED_WIDGET, UNUSED_PARM)
{
	dipsw_finish();
}

static void dipsw_create(void)
{
	Q8tkWidget *w, *a, *f, *x, *b;
	const t_menulabel *l = data_reset;

	/* メインとなるウインドウ */
	{
		w = q8tk_window_new(Q8TK_WINDOW_DIALOG);
		a = q8tk_accel_group_new();
		q8tk_accel_group_attach(a, w);
	}
	/* に、フレームを乗せて */
	{
		f = q8tk_frame_new(GET_LABEL(l, DATA_RESET_DIPSW_SET));
		q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_OUT);
		q8tk_container_add(w, f);
		q8tk_widget_show(f);
	}
	/* それにボックスを乗せる */
	{
		x = q8tk_vbox_new();
		q8tk_container_add(f, x);
		q8tk_widget_show(x);
		/* ボックスには     */
		{
			/* DIPSWメニュー と */
			Q8tkWidget *vbox;
			const t_menulabel *l = data_dipsw;

			vbox = PACK_VBOX(NULL);
			{
				PACK_FRAME(vbox, GET_LABEL(l, DATA_DIPSW_B), menu_dipsw_b());

				PACK_FRAME(vbox, GET_LABEL(l, DATA_DIPSW_R), menu_dipsw_r());
			}

			q8tk_box_pack_start(x, vbox);
		}
		{
			/* 終了ボタンを配置 */
			b = PACK_BUTTON(x,
							GET_LABEL(l, DATA_RESET_DIPSW_QUIT),
							cb_reset_dipsw_end, NULL);

			q8tk_accel_group_add(a, Q8TK_KEY_ESC, b, "clicked");
		}
	}

	dipsw_window_base = w;
	dipsw_window_focus = b;
}

static void dipsw_start(void)
{
	q8tk_widget_show(dipsw_window_base);
	q8tk_grab_add(dipsw_window_base);

	q8tk_widget_set_focus(dipsw_window_focus);
}

static void dipsw_finish(void)
{
	q8tk_grab_remove(dipsw_window_base);
	q8tk_widget_hide(dipsw_window_base);
}
