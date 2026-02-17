/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include "quasi88.h"
#include "screen.h"

#include "q8tk.h"
#include "menu-common.h"

#include "pause.h"


int pause_msg = TRUE;

/*#define DISPLAY_CANCEL_BUTTON*/
/***********************************************************************
 *
 ************************************************************************/
#ifdef DISPLAY_CANCEL_BUTTON
static void cb_pause_cancel(UNUSED_WIDGET, UNUSED_PARM)
{
	pause_event_key_on_esc();
}
#endif

static void cb_pause_top_fn(UNUSED_WIDGET, void *p)
{
	int keycode = P2INT(p);
	switch (quasi88_get_last_key_function(keycode)) {
	case FN_MENU:
		pause_event_key_on_menu();
		break;
	case Q8TK_KEY_ESC:
	case FN_PAUSE:
		pause_event_key_on_esc();
		break;
	default:
		break;
	}
}

void pause_top(void)
{
	Q8tkWidget *d, *a, *l, *h;
	int i;
	const int key[] = {
		Q8TK_KEY_ESC, Q8TK_KEY_MENU,
		Q8TK_KEY_F6, Q8TK_KEY_F7, Q8TK_KEY_F8, Q8TK_KEY_F9, Q8TK_KEY_F10,
	};


	if (pause_msg) {
		d = q8tk_dialog_new();
		q8tk_dialog_set_style(d, -1, -1);
		q8tk_window_set_shadow_type(d, Q8TK_SHADOW_ETCHED_OUT);

		l = q8tk_label_new("  * * *   P A U S E   * * *  ");
		q8tk_widget_show(l);
		q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
		q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

#ifdef DISPLAY_CANCEL_BUTTON
		l = q8tk_button_new_with_label(" Cancel ");
		q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);
		q8tk_widget_show(l);
		q8tk_signal_connect(l, "clicked", cb_pause_cancel, NULL);
#endif

		h = Q8TK_DIALOG(d)->action_area;

	} else {
		d = q8tk_window_new(Q8TK_WINDOW_TOPLEVEL);

		h = q8tk_hbox_new();
		q8tk_widget_show(h);
		q8tk_container_add(d, h);
	}

	a = q8tk_accel_group_new();
	q8tk_accel_group_attach(a, d);

	for (i = 0; i < COUNTOF(key); i++) {
		Q8tkWidget *fake = q8tk_button_new();
		q8tk_box_pack_start(h, fake);
		q8tk_signal_connect(fake, "clicked", cb_pause_top_fn, INT2P(key[i]));
		q8tk_accel_group_add(a, key[i], fake, "clicked");
	}

	q8tk_widget_show(d);
	q8tk_grab_add(d);
}
