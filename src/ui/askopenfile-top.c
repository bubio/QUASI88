/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "drive.h"

#include "event.h"
#include "q8tk.h"
#include "menu-common.h"
#include "emu.h"
#include "statusbar.h"



static char       disk_filename[ QUASI88_MAX_FILENAME ];
static Q8tkWidget *accel, *fsel;
static Q8tkWidget *eject_button, *set_button, *cancel_button;

static void cb_mode(UNUSED_WIDGET, void *p);
static void cb_fselect(UNUSED_WIDGET, void *p);

static const struct {
	char *label;
	int mode;
} table[3] = {
	{ "1: & 2:", 0x03, },
	{ "1:     ", 0x01, },
	{ "2:     ", 0x02, },
};

/***********************************************************************
 *
 ************************************************************************/
void askopenfile_top(void)
{
	int i;
	Q8tkWidget *hbox, *vbox, *wk;

	/* ディスクがあればそのファイルを、なければディスク用ディレクトリを取得 */
	const char *initial = filename_get_disk_or_dir(DRIVE_1);

	fsel = q8tk_file_selection_simple_new();
	{
		hbox = Q8TK_FILE_SELECTION(fsel)->simple_hbox;

		/* ファイルセレクションの右に、すきま */
		wk = q8tk_label_new(" ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);

		/* そのさらに右に、各種ウィジット */
		vbox = q8tk_vbox_new();
		q8tk_box_pack_start(hbox, vbox);
		q8tk_widget_show(vbox);
		{
			Q8tkWidget *f, *vv, *b;

			wk = q8tk_label_new("* Open File *");
			q8tk_box_pack_start(vbox, wk);
			q8tk_widget_show(wk);

			wk = q8tk_label_new(" (Disk Image)");
			q8tk_box_pack_start(vbox, wk);
			q8tk_widget_show(wk);

			for (i = 0; i < 1; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vbox, wk);
				q8tk_widget_show(wk);
			}

			wk = q8tk_label_new(" Target");
			q8tk_box_pack_start(vbox, wk);
			q8tk_widget_show(wk);

			f = q8tk_frame_new("  Drive ");
			q8tk_box_pack_start(vbox, f);
			q8tk_widget_show(f);

			{
				vv = q8tk_vbox_new();
				q8tk_container_add(f, vv);
				q8tk_widget_show(vv);

				/* o 1 & 2 */
				/* o 1     */
				/* o 2     */
				b = NULL;
				for (i = 0; i < COUNTOF(table); i++) {
					wk = q8tk_label_new("");
					q8tk_box_pack_start(vv, wk);
					q8tk_widget_show(wk);

					b = q8tk_radio_button_new_with_label(b, table[i].label);
					q8tk_box_pack_start(vv, b);
					q8tk_widget_show(b);
					q8tk_signal_connect(b, "clicked",
										cb_mode, INT2P(table[i].mode));
					if (i == 0) {
						q8tk_toggle_button_set_state(b, TRUE);
					}
				}
			}

			for (i = 0; i < 2; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vbox, wk);
				q8tk_widget_show(wk);
			}

			/* [EJECT] */
			eject_button = q8tk_button_new_with_label("Eject ");
			q8tk_box_pack_start(vbox, eject_button);
			q8tk_widget_show(eject_button);
			q8tk_misc_set_placement(eject_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			/* [SET] */
			set_button = q8tk_button_new_with_label("Insert");
			q8tk_box_pack_start(vbox, set_button);
			q8tk_widget_show(set_button);
			q8tk_misc_set_placement(set_button,
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
	q8tk_widget_show(fsel);
	q8tk_grab_add(fsel);

	if (initial && initial[0]) {
		q8tk_file_selection_set_filename(fsel, initial);
	}

	q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->ok_button,
						"clicked", cb_fselect, (void *) 1);
	q8tk_signal_connect(cancel_button,
						"clicked", cb_fselect, (void *) 0);
	q8tk_signal_connect(set_button,
						"clicked", cb_fselect, (void *) 1);
	q8tk_signal_connect(eject_button,
						"clicked", cb_fselect, (void *) 2);
	q8tk_widget_set_focus(cancel_button);

	accel = q8tk_accel_group_new();

	q8tk_accel_group_attach(accel, fsel);
	q8tk_accel_group_add(accel, Q8TK_KEY_ESC, cancel_button, "clicked");
}

static int drv_bit;
static void cb_mode(UNUSED_WIDGET, void *p)
{
	drv_bit = P2INT(p);
}

static void cb_fselect(UNUSED_WIDGET, void *p)
{
	int drv = (drv_bit == 0x01) ? DRIVE_1 : DRIVE_2;
	int success;

	if (drv_bit) {

		if (P2INT(p) == 1) {
			/* OK */
			disk_filename[0] = '\0';
			strncat(disk_filename, q8tk_file_selection_get_filename(fsel),
					QUASI88_MAX_FILENAME - 1);

			if (drv_bit == 0x03) {
				success = quasi88_disk_insert_all(disk_filename, FALSE);

			} else {
				success = quasi88_disk_insert(drv, disk_filename, 0, FALSE);
			}
			if (success) {
				emu_status_message_set(STATUS_INFO,
									   "Disk image set OK",
									   "ディスクをセットしました");
			} else {
				emu_status_message_set(STATUS_WARN,
									   "Can't open disk image file !!",
									   "ディスクイメージが開けません !!");
			}

		} else if (P2INT(p) == 2) {
			/* EJECT */
			if (drv_bit == 0x03) {
				quasi88_disk_eject_all();
			} else {
				if (! disk_image_exist(drv ^ 1)) {
					quasi88_disk_eject_all();
				} else {
					quasi88_disk_eject(drv);
				}
			}
			emu_status_message_set(STATUS_INFO,
								   "Disk image eject OK",
								   "ディスクを取り出しました");
		}
	}

	q8tk_grab_remove(fsel);
	q8tk_widget_destroy(fsel);
	q8tk_widget_destroy(accel);
	fsel = NULL;
	accel = NULL;

	quasi88_exec();
}
