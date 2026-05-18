/***********************************************************************
 *
 ************************************************************************/
#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "drive.h"

#include "q8tk.h"
#include "menu-common.h"
#include "emu.h"
#include "statusbar.h"



static Q8tkWidget	*list[2];

/***********************************************************************
 *
 ************************************************************************/
static void cb_imageselect_exit(UNUSED_WIDGET, UNUSED_PARM)
{
	quasi88_exec();
}

static void cb_disk_image(UNUSED_WIDGET, void *p)
{
	int drv = (P2INT(p)) & 0xff;
	int img = (P2INT(p)) >> 8;

	if (img < 0) {
		/* img == -1 で <なし> */
		drive_set_empty(drv);

	} else {
		/* img >= 0 なら イメージ番号 */
		drive_unset_empty(drv);
		disk_change_image(drv, img);
	}

	statusbar_image_name();
}

static void set_disk_widget(void)
{
	int i, drv, save_code;
	Q8tkWidget *item;
	char wk[40];

	for (drv = 0; drv < 2; drv++) {
		/* イメージ名の LIST ITEM 生成 */
		q8tk_listbox_clear_items(list[drv], 0, -1);

		/* リストの先頭は必ず '<なし>' ITEM */
		item = q8tk_list_item_new_with_label("< EMPTY >                 ", 0);
		q8tk_widget_show(item);
		q8tk_container_add(list[drv], item);
		q8tk_signal_connect(item, "select",
							cb_disk_image, INT2P((-1 << 8) + drv));

		/* リストの次以降は 'イメージ名' ITEM */
		if (disk_image_exist(drv)) {
			/* ディスク挿入済 */

			save_code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
			{
				for (i = 0; i < disk_image_num(drv); i++) {
					sprintf(wk, "%3d  %-16s  %s ", /*イメージNo イメージ名 RW*/
							i + 1,
							drive[drv].image[i].name,
							(drive[drv].image[i].protect) ? "RO" : "RW");

					item = q8tk_list_item_new_with_label(wk, 0);
					q8tk_widget_show(item);
					q8tk_container_add(list[drv], item);
					q8tk_signal_connect(item, "select",
										cb_disk_image, INT2P((i << 8) + drv));
				}
			}
			q8tk_set_kanjicode(save_code);

			/* <なし> or 選択イメージ の ITEM をセレクト */
			if (drive_check_empty(drv)) {
				i = 0;
			} else {
				i = disk_image_selected(drv) + 1;
			}
			q8tk_listbox_select_item(list[drv], i);

		} else {
			/* ドライブ空っぽ */
			/* <なし> ITEM をセレクト */

			q8tk_listbox_select_item(list[drv], 0);
		}
	}
}

void askselectdisk_top(void)
{
	Q8tkWidget *d, *a, *l;

	d = q8tk_dialog_new();
	q8tk_dialog_set_style(d, -1, -1);
	q8tk_window_set_shadow_type(d, Q8TK_SHADOW_ETCHED_OUT);

	a = q8tk_accel_group_new();
	q8tk_accel_group_attach(a, d);

#if 0
	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("  * * *   Select Disk   * * *  ");
	q8tk_widget_show(l);
	q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);

	l = q8tk_label_new("");
	q8tk_widget_show(l);
	q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, l);
#endif

	{
		int i;
		Q8tkWidget *hbox = q8tk_hbox_new();
		q8tk_widget_show(hbox);
		q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, hbox);

		for (i = 0; i < 2; i ++) {
			Q8tkWidget *vbox, *swin;

			if (i != 0) {
				l = q8tk_label_new(" ");
				//l = q8tk_vseparator_new();
				q8tk_widget_show(l);
				q8tk_box_pack_end(hbox, l);
			}

			vbox = q8tk_vbox_new();
			q8tk_widget_show(vbox);
			q8tk_box_pack_end(hbox, vbox);

			l = q8tk_label_new((i == 0) ?
							   "< Drive 1: >   " : "< Drive 2: >   ");
			q8tk_widget_show(l);
			q8tk_misc_set_placement(l, Q8TK_PLACEMENT_X_RIGHT, 0);
			q8tk_box_pack_start(vbox, l);

			swin  = q8tk_scrolled_window_new(NULL, NULL);
			q8tk_widget_show(swin);
//			q8tk_scrolled_window_set_frame(swin, FALSE);
			q8tk_scrolled_window_set_adj_size(swin, 2);
			q8tk_scrolled_window_set_policy(swin,
											Q8TK_POLICY_NEVER,
											Q8TK_POLICY_AUTOMATIC);
			q8tk_misc_set_size(swin, 32, 16);

			list[i] = q8tk_listbox_new();
			q8tk_widget_show(list[i]);
			q8tk_container_add(swin, list[i]);

			q8tk_listbox_set_big(list[i], TRUE);

			q8tk_box_pack_start(vbox, swin);
		}
	}

	/* リストウィジットにイメージ名を登録 */
	set_disk_widget();

	l = q8tk_button_new_with_label(" Exit ");
	q8tk_widget_show(l);
	q8tk_signal_connect(l, "clicked", cb_imageselect_exit, NULL);
	q8tk_accel_group_add(a, Q8TK_KEY_ESC, l, "clicked");
	q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, l);

	q8tk_widget_show(d);
	q8tk_grab_add(d);

	/* 最後に追加したウィジットにフォーカス */
	q8tk_widget_set_focus(l);
}
