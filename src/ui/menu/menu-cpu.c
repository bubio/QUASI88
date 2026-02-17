/*===========================================================================
 *
 * メインページ CPU設定
 *
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "menu.h"

#include "emu.h"
#include "pc88main.h"
#include "memory.h"
#include "intr.h"
#include "fdc.h"
#include "soundbd.h"
#include "snddrv.h"

#include "q8tk.h"

#include "menu-common.h"

#include "menu-cpu.h"
#include "menu-cpu-message.h"



/*----------------------------------------------------------------------*/
/* SUB-CPU駆動 */
static int get_cpu_cpu(void)
{
	return cpu_timing;
}
static void cb_cpu_cpu(UNUSED_WIDGET, void *p)
{
	cpu_timing = P2INT(p);
}


static Q8tkWidget *menu_cpu_cpu(void)
{
	Q8tkWidget *vbox;

	vbox = PACK_VBOX(NULL);
	{
		PACK_RADIO_BUTTONS(vbox,
						   data_cpu_cpu, COUNTOF(data_cpu_cpu),
						   get_cpu_cpu(), cb_cpu_cpu);
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* 説明ダイアログ */
static Q8tkWidget *cpu_help_window_base;
static Q8tkWidget *cpu_help_window_focus;

static void help_finish(void);
static void cb_cpu_help_end(UNUSED_WIDGET, UNUSED_PARM)
{
	help_finish();
}

static void cpu_help_create(void)
{
	Q8tkWidget *w, *a, *swin, *x, *b, *z, *l;

	{
		/* メインとなるウインドウ */
		w = q8tk_window_new(Q8TK_WINDOW_DIALOG);
		a = q8tk_accel_group_new();
		q8tk_accel_group_attach(a, w);
	}

	{
		/* それにボックスを乗せる */
		x = q8tk_vbox_new();
		q8tk_container_add(w, x);
		q8tk_widget_show(x);

		/* ボックスには     */
		{
			/* SCRLウインドウと */
			swin  = q8tk_scrolled_window_new(NULL, NULL);
			q8tk_widget_show(swin);
			q8tk_scrolled_window_set_policy(swin,
											Q8TK_POLICY_NEVER,
											Q8TK_POLICY_AUTOMATIC);
			q8tk_misc_set_size(swin, 71, 20);
			q8tk_box_pack_start(x, swin);
		}
		{
			/* 終了ボタンを配置 */
			b = PACK_BUTTON(x,
							" O K ",
							cb_cpu_help_end, NULL);
			q8tk_misc_set_placement(b,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			q8tk_accel_group_add(a, Q8TK_KEY_ESC, b, "clicked");
		}
	}

	{
		/* SCRLウインドウに VBOXを作って 説明ラベルを配置 */
		int i;
		const char **s = (menu_lang == MENU_JAPAN) ? help_jp : help_en;
		z = q8tk_vbox_new();
		q8tk_container_add(swin, z);
		q8tk_widget_show(z);

		for (i = 0; s[i]; i++) {
			l = q8tk_label_new(s[i]);
			q8tk_widget_show(l);
			q8tk_box_pack_start(z, l);
		}
	}

	cpu_help_window_base = w;
	cpu_help_window_focus = b;
}

/* 説明ウインドウの表示 */

static void cb_cpu_help(UNUSED_WIDGET, UNUSED_PARM)
{
	if (cpu_help_window_base == NULL) {
		cpu_help_create();
	}

	q8tk_widget_show(cpu_help_window_base);
	q8tk_grab_add(cpu_help_window_base);

	q8tk_widget_set_focus(cpu_help_window_focus);
}

/* 説明ウインドウの消去 */

static void help_finish(void)
{
	q8tk_grab_remove(cpu_help_window_base);
	q8tk_widget_hide(cpu_help_window_base);
}



static Q8tkWidget *menu_cpu_help(void)
{
	Q8tkWidget *button;
	const t_menulabel *l = data_cpu;

	button = PACK_BUTTON(NULL,
						 GET_LABEL(l, DATA_CPU_HELP),
						 cb_cpu_help, NULL);
	q8tk_misc_set_placement(button,
							Q8TK_PLACEMENT_X_CENTER,
							Q8TK_PLACEMENT_Y_CENTER);
	return button;
}

/*----------------------------------------------------------------------*/
/* CPUクロック */
static double get_cpu_clock(void)
{
	return cpu_clock_mhz;
}
static void cb_cpu_clock(Q8tkWidget *widget, void *mode)
{
	int i;
	const t_menudata *p = data_cpu_clock_combo;
	const char *combo_str = q8tk_combo_get_text(widget);
	char buf[16], *conv_end;
	double val = 0;
	int fit = FALSE;

	/* COMBO BOX から ENTRY に一致するものを探す */
	for (i = 0; i < COUNTOF(data_cpu_clock_combo); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			val = (double) p->val / 1000000.0;
			fit = TRUE; /* 一致した値を適用 */
			break;
		}
	}

	/* COMBO BOX に該当がない場合 */
	if (fit == FALSE) {
		strncpy(buf, combo_str, 15);
		buf[15] = '\0';

		val = strtod(buf, &conv_end);

		if ((P2INT(mode) == 0) &&
			(strlen(buf) == 0 || val == 0.0)) {
			/* 空白 + ENTER や 0 + ENTER 時は デフォルト値を適用 */
			val = (boot_clock_4mhz) ? CONST_4MHZ_CLOCK : CONST_8MHZ_CLOCK;
			fit = TRUE;

		} else if (*conv_end != '\0') {
			/* 数字変換失敗なら その値は使えない */
			fit = FALSE;

		} else {
			/* 数字変換成功なら その値を適用する */
			fit = TRUE;
		}
	}

	/* 適用した値が有効範囲なら、セット */
	if (fit) {
		if (0.1 <= val && val < 1000.0) {
			cpu_clock_mhz = val;
			interval_work_init_all();
		}
	}

	/* COMBO ないし ENTER時は、値を再表示*/
	if (P2INT(mode) == 0) {
		sprintf(buf, "%8.4f", get_cpu_clock());
		q8tk_combo_set_text(widget, buf);
	}
}


static Q8tkWidget *menu_cpu_clock(void)
{
	Q8tkWidget *vbox, *hbox;
	const t_menulabel *p = data_cpu_clock;
	char buf[16];

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_CLOCK_CLOCK));

			sprintf(buf, "%8.4f", get_cpu_clock());
			PACK_COMBO(hbox,
					   data_cpu_clock_combo, COUNTOF(data_cpu_clock_combo),
					   (int) get_cpu_clock(), buf, 9,
					   cb_cpu_clock, INT2P(0),
					   cb_cpu_clock, INT2P(1));

			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_CLOCK_MHZ));

			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_CLOCK_INFO));
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* 速度／ウエイトなしにする */
static int get_cpu_nowait(void)
{
	return no_wait;
}
static void cb_cpu_nowait(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	no_wait = key;
}

static int get_cpu_wait(void)
{
	return wait_rate;
}
static void cb_cpu_wait(Q8tkWidget *widget, void *mode)
{
	int i;
	const t_menudata *p = data_cpu_wait_combo;
	const char *combo_str = q8tk_combo_get_text(widget);
	char buf[16], *conv_end;
	int val = 0;
	int fit = FALSE;

	/* COMBO BOX から ENTRY に一致するものを探す */
	for (i = 0; i < COUNTOF(data_cpu_wait_combo); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			val = p->val;
			fit = TRUE; /* 一致した値を適用 */
			break;
		}
	}

	/* COMBO BOX に該当がない場合 */
	if (fit == FALSE) {
		strncpy(buf, combo_str, 15);
		buf[15] = '\0';

		val = strtoul(buf, &conv_end, 10);

		if ((P2INT(mode) == 0) &&
			(strlen(buf) == 0 || val == 0)) {
			/* 空白 + ENTER や 0 + ENTER 時は デフォルト値を適用*/
			val = 100;
			fit = TRUE;

		} else if (*conv_end != '\0') {
			/* 数字変換失敗なら その値は使えない */
			fit = FALSE;

		} else {
			/* 数字変換成功なら その値を適用する */
			fit = TRUE;
		}
	}

	/* 適用した値が有効範囲なら、セット */
	if (fit) {
		if (5 <= val && val <= 5000) {
			wait_rate = val;
		}
	}

	/* COMBO ないし ENTER時は、値を再表示*/
	if (P2INT(mode) == 0) {
		sprintf(buf, "%4d", get_cpu_wait());
		q8tk_combo_set_text(widget, buf);
	}
}


static Q8tkWidget *menu_cpu_wait(void)
{
	Q8tkWidget *vbox, *hbox, *button;
	const t_menulabel *p = data_cpu_wait;
	char buf[16];

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_WAIT_RATE));

			sprintf(buf, "%4d", get_cpu_wait());
			PACK_COMBO(hbox,
					   data_cpu_wait_combo, COUNTOF(data_cpu_wait_combo),
					   get_cpu_wait(), buf, 5,
					   cb_cpu_wait, INT2P(0),
					   cb_cpu_wait, INT2P(1));

			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_WAIT_PERCENT));

			PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_WAIT_INFO));
		}

		button = PACK_CHECK_BUTTON(vbox,
								   GET_LABEL(p, DATA_CPU_WAIT_NOWAIT),
								   get_cpu_nowait(),
								   cb_cpu_nowait, NULL);
		q8tk_misc_set_placement(button,
								Q8TK_PLACEMENT_X_RIGHT,
								Q8TK_PLACEMENT_Y_CENTER);
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* ブースト */
static int get_cpu_boost(void)
{
	return boost;
}
static void cb_cpu_boost(Q8tkWidget *widget, void *mode)
{
	int i;
	const t_menudata *p = data_cpu_boost_combo;
	const char *combo_str = q8tk_combo_get_text(widget);
	char buf[16], *conv_end;
	int val = 0;
	int fit = FALSE;

	/* COMBO BOX から ENTRY に一致するものを探す */
	for (i = 0; i < COUNTOF(data_cpu_boost_combo); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			val = p->val;
			fit = TRUE; /* 一致した値を適用 */
			break;
		}
	}

	/* COMBO BOX に該当がない場合 */
	if (fit == FALSE) {
		strncpy(buf, combo_str, 15);
		buf[15] = '\0';

		val = strtoul(buf, &conv_end, 10);

		if ((P2INT(mode) == 0) &&
			(strlen(buf) == 0 || val == 0)) {
			/* 空白 + ENTER や 0 + ENTER 時は デフォルト値を適用*/
			val = 1;
			fit = TRUE;

		} else if (*conv_end != '\0') {
			/* 数字変換失敗なら その値は使えない */
			fit = FALSE;

		} else {
			/* 数字変換成功なら その値を適用する */
			fit = TRUE;
		}
	}

	/* 適用した値が有効範囲なら、セット */
	if (fit) {
		if (1 <= val && val <= 100) {
			if (boost != val) {
				boost_change(val);
			}
		}
	}

	/* COMBO ないし ENTER時は、値を再表示*/
	if (P2INT(mode) == 0) {
		sprintf(buf, "%4d", get_cpu_boost());
		q8tk_combo_set_text(widget, buf);
	}
}

static Q8tkWidget *menu_cpu_boost(void)
{
	Q8tkWidget *hbox;
	char buf[8];
	const t_menulabel *p = data_cpu_boost;

	hbox = PACK_HBOX(NULL);
	{
		PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_BOOST_MAGNIFY));

		sprintf(buf, "%4d", get_cpu_boost());
		PACK_COMBO(hbox,
				   data_cpu_boost_combo, COUNTOF(data_cpu_boost_combo),
				   get_cpu_boost(), buf, 5,
				   cb_cpu_boost, (void *)0,
				   cb_cpu_boost, (void *)1);

		PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_BOOST_UNIT));

		PACK_LABEL(hbox, GET_LABEL(p, DATA_CPU_BOOST_INFO));
	}

	return hbox;
}

/*----------------------------------------------------------------------*/
/* (各種設定の変更) */
static int get_cpu_misc(int type)
{
	switch (type) {
	case DATA_CPU_MISC_FDCWAIT:
		return (fdc_wait == 0) ? FALSE : TRUE;

	case DATA_CPU_MISC_HSBASIC:
		return highspeed_mode;

	case DATA_CPU_MISC_MEMWAIT:
		return memory_wait;

	case DATA_CPU_MISC_CMDSING:
		return use_cmdsing;
	}
	return FALSE;
}
static void cb_cpu_misc(Q8tkWidget *widget, void *p)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	switch (P2INT(p)) {
	case DATA_CPU_MISC_FDCWAIT:
		fdc_wait = (key) ? 1 : 0;
		return;

	case DATA_CPU_MISC_HSBASIC:
		highspeed_mode = (key) ? TRUE : FALSE;
		return;

	case DATA_CPU_MISC_MEMWAIT:
		memory_wait = (key) ? TRUE : FALSE;
		return;

	case DATA_CPU_MISC_CMDSING:
		use_cmdsing = (key) ? TRUE : FALSE;
#ifdef  USE_SOUND
		xmame_dev_beep_cmd_sing((byte) use_cmdsing);
#endif
		return;
	}
}


static Q8tkWidget *menu_cpu_misc(void)
{
	int i;
	Q8tkWidget *vbox, *l;
	const t_menudata *p = data_cpu_misc;

	vbox = PACK_VBOX(NULL);
	{
		for (i = 0; i < COUNTOF(data_cpu_misc); i++, p++) {
			if (p->val >= 0) {
				PACK_CHECK_BUTTON(vbox,
								  GET_LABEL(p, 0),
								  get_cpu_misc(p->val),
								  cb_cpu_misc, INT2P(p->val));
			} else {
				l = PACK_LABEL(vbox, GET_LABEL(p, 0));
				q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_RIGHT, 0);
			}
		}
	}

	return vbox;
}

/*======================================================================*/

Q8tkWidget *menu_cpu(void)
{
	Q8tkWidget *vbox, *hbox, *vbox2;
	Q8tkWidget *f;
	const t_menulabel *l = data_cpu;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_FRAME(hbox, GET_LABEL(l, DATA_CPU_CPU), menu_cpu_cpu());

			f = PACK_FRAME(hbox, "              ", menu_cpu_help());
			q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_NONE);
		}

		hbox = PACK_HBOX(vbox);
		{
			vbox2 = PACK_VBOX(hbox);
			{
				PACK_FRAME(vbox2,
						   GET_LABEL(l, DATA_CPU_CLOCK), menu_cpu_clock());

				PACK_FRAME(vbox2,
						   GET_LABEL(l, DATA_CPU_WAIT), menu_cpu_wait());

				PACK_FRAME(vbox2,
						   GET_LABEL(l, DATA_CPU_BOOST), menu_cpu_boost());
			}

			f = PACK_FRAME(hbox, "", menu_cpu_misc());
			q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_NONE);
		}
	}

	cpu_help_window_base = NULL;

	return vbox;
}

/*
  TODO
  menu_cpu_clock() の大外の vbox は不要
  cb_cpu_help_end() で help_finish() を呼び出しているが、これを
  コールバック関数にすればいいのでは？
*/
