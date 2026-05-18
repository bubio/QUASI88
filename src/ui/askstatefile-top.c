/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "drive.h"

#include "event.h"
#include "q8tk.h"
#include "menu-common.h"
#include "emu.h"
#include "statusbar.h"
#include "suspend.h"



static int page;			/* 最後に開いたページを保存。次回はこれを表示 */
static int load_num = 1;
static int save_num = 1;
static Q8tkWidget *entry;
static Q8tkWidget *accel, *fsel, *ok_button, *cancel_button;;
enum {
	CMD_LOAD = 0,
	CMD_SAVE = 1,
	CMD_FILE
};
static const struct {
	char *page;
	char *select;
	char *exec;
} label[3] = {
	{ " State Load \x1f   ",   "Load", " State Load \x1f ", },
	{ "   \x1e State Save ",   "Save", " \x1e State Save ", },
	{ "     (File Name)     ", 0,      0,                   },
};

/***********************************************************************
 *
 ************************************************************************/
static void cb_statefile_cancel(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_exec();
}

static void cb_statefile_exec(UNUSED_WIDGET, void *p)
{
	int cmd = P2INT(p);

	if (cmd == CMD_LOAD) {
		quasi88_stateload(load_num + '0');
	} else {
		quasi88_statesave(save_num + '0');
	}

	quasi88_exec();
}

static void cb_notebook_changed(Q8tkWidget *widget, UNUSED_PARM)
{
	page = q8tk_notebook_current_page(widget);
}
static void cb_num(UNUSED_WIDGET, void *p)
{
	int cmd = P2INT(p) / 100;
	int num = P2INT(p) % 100;

	if (cmd == CMD_LOAD) {
		load_num = num;
	} else {
		save_num = num;
	}
}

static void cb_fselect(UNUSED_WIDGET, void *p)
{
	if (P2INT(p) == 1) {
		const char *fname = q8tk_file_selection_get_filename(fsel);
		filename_set_state(fname);
		q8tk_entry_set_text(entry, fname);
		q8tk_entry_set_position(entry, QUASI88_MAX_FILENAME);
	}

	q8tk_widget_destroy(accel);
	q8tk_grab_remove(fsel);
	q8tk_widget_destroy(fsel);
	accel = NULL;
	fsel = NULL;
}
static void cb_file_change(UNUSED_WIDGET, UNUSED_PARM)
{
	fsel = q8tk_file_selection_simple_new();
	{
		Q8tkWidget *hbox, *vbox, *wk;

		hbox = Q8TK_FILE_SELECTION(fsel)->simple_hbox;

		wk = q8tk_label_new("   ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);

		vbox = q8tk_vbox_new();
		q8tk_box_pack_start(hbox, vbox);
		q8tk_widget_show(vbox);
		{
			int i;
			for (i = 0; i < 17; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vbox, wk);
				q8tk_widget_show(wk);
			}

			/* [OK] */
			ok_button = q8tk_button_new_with_label("Select");
			q8tk_box_pack_start(vbox, ok_button);
			q8tk_widget_show(ok_button);
			q8tk_misc_set_placement(ok_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			/* [CANCEL] */
			cancel_button = q8tk_button_new_with_label("Cancel");
			q8tk_box_pack_start(vbox, cancel_button);
			q8tk_widget_show(cancel_button);
			q8tk_misc_set_placement(cancel_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
		}
	}

	q8tk_file_selection_set_filename(fsel, filename_get_state());

	q8tk_widget_show(fsel);
	q8tk_grab_add(fsel);

	q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->ok_button,
						"clicked", cb_fselect, (void *) 1);
	q8tk_signal_connect(ok_button,
						"clicked", cb_fselect, (void *) 1);
	q8tk_signal_connect(cancel_button,
						"clicked", cb_fselect, (void *) 0);
	q8tk_widget_set_focus(cancel_button);

	accel = q8tk_accel_group_new();

	q8tk_accel_group_attach(accel, fsel);
	q8tk_accel_group_add(accel, Q8TK_KEY_ESC, cancel_button, "clicked");
}

static Q8tkWidget *make_file_page(void)
{
	Q8tkWidget *v, *l, *b;

	v = q8tk_vbox_new();
	q8tk_widget_show(v);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(v, l);

	l = q8tk_label_new("State file name (current)");
	q8tk_widget_show(l);
	q8tk_box_pack_start(v, l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(v, l);

	{
		int save_code = q8tk_set_kanjicode(osd_kanji_code());

		entry = q8tk_entry_new_with_max_length(QUASI88_MAX_FILENAME);
		q8tk_misc_set_size(entry, 55, 1);
		q8tk_entry_set_text(entry, filename_get_state());
		q8tk_entry_set_position(entry, QUASI88_MAX_FILENAME);
		q8tk_entry_set_editable(entry, FALSE);
		q8tk_misc_set_placement(entry, 0, Q8TK_PLACEMENT_Y_CENTER);
		q8tk_widget_show(entry);
		q8tk_box_pack_start(v, entry);

		q8tk_set_kanjicode(save_code);
	}

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(v, l);

	b = q8tk_button_new_with_label(" Change ");
	q8tk_widget_show(b);
	q8tk_box_pack_start(v, b);
	q8tk_signal_connect(b, "clicked", cb_file_change, NULL);
	q8tk_misc_set_placement(b, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);

	return v;
}

static Q8tkWidget *make_loadsave_page(int cmd)
{
	int i, j;
	Q8tkWidget *b = NULL;
	Q8tkWidget *v = q8tk_vbox_new();
	q8tk_widget_show(v);

	for (j = 0; j < 2; j++) {
		Q8tkWidget *h = q8tk_hbox_new();
		q8tk_widget_show(h);
		q8tk_box_pack_start(v, h);

		for (i = 0; i < 5; i++) {
			int num = (j * 5 + i + 1) % 10;
			char str[16];
			sprintf(str, " %s %d. ", label[cmd].select, num);
			b = q8tk_radio_push_button_new_with_label(b, str);
			q8tk_widget_show(b);
			q8tk_box_pack_start(h, b);
			q8tk_signal_connect(b, "clicked", 
								cb_num, INT2P(cmd * 100 + num));
			if (((cmd == CMD_LOAD) && (num == load_num)) ||
				((cmd == CMD_SAVE) && (num == save_num))) {
				q8tk_toggle_button_set_state(b, TRUE);
			}
		}
	}

	b = q8tk_button_new_with_label(label[cmd].exec);
	q8tk_widget_show(b);
	q8tk_signal_connect(b, "clicked", cb_statefile_exec, INT2P(cmd));
	q8tk_box_pack_start(v, b);
	if (cmd == CMD_SAVE) {
		q8tk_misc_set_placement(b, Q8TK_PLACEMENT_X_RIGHT, 0);
	}
	return v;
}

void askstatefile_top(void)
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

	l = q8tk_label_new("  * * *   State File   * * *  ");
	q8tk_widget_show(l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	{
		Q8tkWidget *w[3];

		Q8tkWidget *notebook = q8tk_notebook_new();
		q8tk_widget_show(notebook);
		q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, notebook);

		w[CMD_FILE] = make_file_page();
		w[CMD_LOAD] = make_loadsave_page(CMD_LOAD);
		w[CMD_SAVE] = make_loadsave_page(CMD_SAVE);

		q8tk_notebook_append(notebook, w[CMD_LOAD], label[CMD_LOAD].page);
		q8tk_notebook_append(notebook, w[CMD_FILE], label[CMD_FILE].page);
		q8tk_notebook_append(notebook, w[CMD_SAVE], label[CMD_SAVE].page);

		q8tk_notebook_set_page(notebook, page);
		q8tk_signal_connect(notebook, "switch_page",
							cb_notebook_changed, NULL);
	}


	l = q8tk_button_new_with_label(" Cancel ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_statefile_cancel, NULL);
	q8tk_accel_group_add(a, Q8TK_KEY_ESC, l, "clicked");
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	q8tk_widget_show(d);
	q8tk_grab_add(d);

	/* 最後に追加したウィジットにフォーカス */
	q8tk_widget_set_focus(l);
}
