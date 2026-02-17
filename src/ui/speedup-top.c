/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include "quasi88.h"

#include "q8tk.h"
#include "menu-common.h"
#include "toolbar.h"



/***********************************************************************
 *
 ************************************************************************/
static void cb_speedup_cancel(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_exec();
}

static void cb_speedup(UNUSED_WIDGET, void *parm)
{
	toolbar_speedup_change(P2INT(parm));
	quasi88_exec();
}

void speedup_top(void)
{
	Q8tkWidget *d, *a, *l;

	d = q8tk_dialog_new();
	q8tk_dialog_set_style(d, -1, -1);
	q8tk_window_set_shadow_type(d, Q8TK_SHADOW_ETCHED_OUT);

	a = q8tk_accel_group_new();
	q8tk_accel_group_attach(a, d);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("  * * *   Speed-Up   * * *  ");
	q8tk_widget_show(l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_button_new_with_label("  x 1  ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup, INT2P(100));
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label(" x1.25 ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup, INT2P(125));
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label("  x 2  ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup, INT2P(200));
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label("  x 4  ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup, INT2P(400));
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label("  MAX  ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup, INT2P(0));
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	l = q8tk_button_new_with_label(" Cancel ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_speedup_cancel, NULL);
	q8tk_accel_group_add(a, Q8TK_KEY_ESC, l, "clicked");
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);


	q8tk_widget_show(d);
	q8tk_grab_add(d);

	/* 最後に追加したのウィジット (取消ボタン) にフォーカス */
	q8tk_widget_set_focus(l);
}
