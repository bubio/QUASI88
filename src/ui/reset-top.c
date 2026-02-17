/***********************************************************************
 *
 ************************************************************************/
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "screen.h"

#include "intr.h"

#include "event.h"
#include "q8tk.h"
#include "q8tk-common.h"

#include "menu-reset.h"

/*#define USE_COMBO_BOX*/


static void set_reset_parm(void);

static T_RESET_CFG reset_cfg;

/***********************************************************************
 *
 ************************************************************************/
static void cb_reset_cancel(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_exec();
}

static void cb_reset_fn(UNUSED_WIDGET, void *p)
{
	int keycode = P2INT(p);
	switch (quasi88_get_last_key_function(keycode)) {
	case FN_MENU:
		quasi88_menu();
		break;
	case Q8TK_KEY_ESC:
		quasi88_exec();
		break;
	default:
		break;
	}
}

static void cb_reset(UNUSED_WIDGET, UNUSED_PARM)
{
	set_reset_parm();

	cpu_clock_mhz
		= reset_cfg.boot_clock_4mhz ? CONST_4MHZ_CLOCK : CONST_8MHZ_CLOCK;

	quasi88_reset(&reset_cfg);
	quasi88_exec();
}


#ifdef USE_COMBO_BOX

static const struct {
	int mode;
	const char *label;
} basic_list[] = {
	{ BASIC_V2,		"V2",	},
	{ BASIC_V1H,	"V1H",	},
	{ BASIC_V1S,	"V1S",	},
	{ BASIC_N,		"N",	},
}, clock_list[] = {
	{ CLOCK_4MHZ,	"4MHz",	},
	{ CLOCK_8MHZ,	"8MHz",	},
};

static Q8tkWidget *combo_basic;
static Q8tkWidget *combo_clock;

static void set_reset_parm(void)
{
	int i;
	const char *label;

	label = q8tk_combo_get_text(combo_basic);
	for (i = 0; i < COUNTOF(basic_list); i++) {
		if (strcmp(label, basic_list[i].label) == 0) {
			reset_cfg.boot_basic = basic_list[i].mode;
			break;
		}
	}

	label = q8tk_combo_get_text(combo_clock);
	for (i = 0; i < COUNTOF(clock_list); i++) {
		if (strcmp(label, clock_list[i].label) == 0) {
			reset_cfg.boot_clock_4mhz = clock_list[i].mode;
			break;
		}
	}
}

static void reset_top_core(Q8tkWidget *d)
{
	int i, match;
	Q8tkWidget *h, *l;

	h = q8tk_hbox_new();
	q8tk_widget_show(h);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, h);
	{
		l = q8tk_label_new(" Mode : ");
		q8tk_widget_show(l);
		q8tk_box_pack_start(h, l);

		l = q8tk_combo_new();
		match = 0;
		for (i = 0; i < COUNTOF(basic_list); i++) {
			q8tk_combo_append_popdown_strings(l, basic_list[i].label, NULL);
			if (reset_cfg.boot_basic == basic_list[i].mode) {
				match = i;
			}
		}
		q8tk_combo_set_text(l, basic_list[match].label);
		q8tk_misc_set_size(l, 8, 0);
		q8tk_widget_show(l);
		q8tk_box_pack_start(h, l);
		combo_basic = l;

		l = q8tk_label_new("    Clock : ");
		q8tk_widget_show(l);
		q8tk_box_pack_start(h, l);

		l = q8tk_combo_new();
		for (i = 0; i < COUNTOF(clock_list); i++) {
			q8tk_combo_append_popdown_strings(l, clock_list[i].label, NULL);
			if (reset_cfg.boot_clock_4mhz == clock_list[i].mode) {
				match = i;
			}
		}
		q8tk_combo_set_text(l, clock_list[match].label);
		q8tk_misc_set_size(l, 8, 0);
		q8tk_widget_show(l);
		q8tk_box_pack_start(h, l);
		combo_clock = l;
	}

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);
}

#else

static const struct {
	int mode;
	const char *label;
} basic_list[] = {
	{ BASIC_N,		"  N   ", },
	{ BASIC_V1S,	" V1S  ", },
	{ BASIC_V1H,	" V1H  ", },
	{ BASIC_V2,		"  V2  ", },
}, clock_list[] = {
	{ CLOCK_4MHZ,	" 4MHz ", },
	{ CLOCK_8MHZ,	" 8MHz ", },
};

static int req_mode;
static int req_clock;

static void set_reset_parm(void)
{
	reset_cfg.boot_basic = req_mode;
	reset_cfg.boot_clock_4mhz = req_clock;
}

static void cb_mode(UNUSED_WIDGET, void *p)
{
	req_mode = P2INT(p);
}

static void cb_clock(UNUSED_WIDGET, void *p)
{
	req_clock = P2INT(p);
}

static void reset_top_core(Q8tkWidget *d)
{
	int i;
	Q8tkWidget *h, *l;

	h = q8tk_hbox_new();
	q8tk_widget_show(h);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, h);
	{
		Q8tkWidget *b = NULL;

		l = q8tk_label_new(" Mode  ");
		q8tk_widget_show(l);
		q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_LEFT, Q8TK_PLACEMENT_Y_CENTER);
		q8tk_box_pack_start(h, l);

		for (i = 0; i < COUNTOF(basic_list); i++) {
			b = q8tk_radio_push_button_new_with_label(b, basic_list[i].label);
			q8tk_widget_show(b);
			q8tk_box_pack_start(h, b);
			q8tk_signal_connect(b, "clicked",
								cb_mode, INT2P(basic_list[i].mode));
			if (reset_cfg.boot_basic == basic_list[i].mode) {
				q8tk_toggle_button_set_state(b, TRUE);
			}
		}

		l = q8tk_label_new("      "); /* padding */
		q8tk_widget_show(l);
		q8tk_box_pack_start(h, l);
	}
	h = q8tk_hbox_new();
	q8tk_widget_show(h);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, h);
	{
		Q8tkWidget *b = NULL;

		l = q8tk_label_new(" Clock ");
		q8tk_widget_show(l);
		q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_LEFT, Q8TK_PLACEMENT_Y_CENTER);
		q8tk_box_pack_start(h, l);

		for (i = 0; i < COUNTOF(clock_list); i++) {
			b = q8tk_radio_push_button_new_with_label(b, clock_list[i].label);
			q8tk_widget_show(b);
			q8tk_box_pack_start(h, b);
			q8tk_signal_connect(b, "clicked",
								cb_clock, INT2P(clock_list[i].mode));
			if (reset_cfg.boot_clock_4mhz == clock_list[i].mode) {
				q8tk_toggle_button_set_state(b, TRUE);
			}
		}
	}
}

#endif


void reset_top(void)
{
	int i;
	Q8tkWidget *d, *a, *l;
	const int key[] = {
		Q8TK_KEY_ESC, Q8TK_KEY_MENU,
		Q8TK_KEY_F6, Q8TK_KEY_F7, Q8TK_KEY_F8, Q8TK_KEY_F9, Q8TK_KEY_F10,
	};

	/* 現在の、リセット情報を取得 */
	quasi88_get_reset_cfg(&reset_cfg);


	d = q8tk_dialog_new();
	q8tk_dialog_set_style(d, -1, -1);
	q8tk_window_set_shadow_type(d, Q8TK_SHADOW_ETCHED_OUT);

	a = q8tk_accel_group_new();
	q8tk_accel_group_attach(a, d);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("  * * *   R e s e t   * * *  ");
	q8tk_widget_show(l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);


	reset_top_core(d);


	l = q8tk_button_new_with_label(" Reset  ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_reset, NULL);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label(" Cancel ");
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_reset_cancel, NULL);

	for (i = 0; i < COUNTOF(key); i++) {
		Q8tkWidget *fake = q8tk_button_new();
		q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, fake);
		q8tk_signal_connect(fake, "clicked", cb_reset_fn, INT2P(key[i]));
		q8tk_accel_group_add(a, key[i], fake, "clicked");
	}

	q8tk_widget_show(d);
	q8tk_grab_add(d);

	/* 最後に追加したウィジット (取消ボタン) にフォーカス */
	q8tk_widget_set_focus(l);
}
