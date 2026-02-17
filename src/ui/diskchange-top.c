/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "initval.h"
#include "drive.h"

#include "event.h"
#include "q8tk.h"
#include "menu-common.h"
#include "toolbar.h"
#include "emu.h"
#include "statusbar.h"



static char       disk_filename[ QUASI88_MAX_FILENAME ];
static Q8tkWidget *accel, *fsel;

static void cb_fselect(UNUSED_WIDGET, void *p);

/***********************************************************************
 *
 ************************************************************************/
void diskchange_top(void)
{
	/* ディスクがあればそのファイルを、なければディスク用ディレクトリを取得 */
	const char *initial = filename_get_disk_or_dir(DRIVE_1);

	fsel = q8tk_file_selection_simple_new();
	q8tk_widget_show(fsel);
	q8tk_grab_add(fsel);

	if (initial && initial[0]) {
		q8tk_file_selection_set_filename(fsel, initial);
	}

	q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->ok_button,
						"clicked", cb_fselect, (void *) 1);
	q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->cancel_button,
						"clicked", cb_fselect, (void *) 0);
	q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->eject_button,
						"clicked", cb_fselect, (void *) 2);
	q8tk_widget_set_focus(Q8TK_FILE_SELECTION(fsel)->cancel_button);

	accel = q8tk_accel_group_new();

	q8tk_accel_group_attach(accel, fsel);
	q8tk_accel_group_add(accel, Q8TK_KEY_ESC,
						 Q8TK_FILE_SELECTION(fsel)->cancel_button, "clicked");
}

static void cb_fselect(UNUSED_WIDGET, void *p)
{
	int drv_bit = q8tk_file_selection_simple_get_drive(fsel);
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
