/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"


/*---------------------------------------------------------------------------
 * ウインドウ (WINDOW)
 *---------------------------------------------------------------------------
 * ・全てのウィジットの最も先祖になるコンテナウィジット。
 * ・子を一つ持てる。
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・このウィジットを表示するには、q8tk_grab_add()にて明示的に表示を指示する。
 * ・WINDOW は最大、MAX_WINDOW_LAYER個作成できるが、シグナルを受け付けるのは、
 *   最後に q8tk_grab_add() を発行したウインドウの子孫のみである。
 * ・Q8TK_WINDOW_TOPLEVEL のウィンドウが必ず必要。
 * ・シグナル
 *   なし
 * ・内部シグナル (ユーザ利用不可)
 *   "inactivate"    ポップアップウインドウにてウインドウ外をクリック時発生
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_window_new(int window_type)
 *    ウインドウの生成
 *    引数により、ウインドウの種類が決まる
 *    Q8TK_WINDOW_TOPLEVEL … ウインドウ枠を持たない。一番基礎のウインドウ向け
 *    Q8TK_WINDOW_DIALOG   … 出っぱったウインドウ枠をもつ。ダイアログ向け
 *    Q8TK_WINDOW_POPUP    … 単純なウインドウ枠をもつ。内部で使用
 *    Q8TK_WINDOW_TOOL     … TOOLBAR専用(ウインドウ枠を持たない)
 *    Q8TK_WINDOW_STAT     … STATUSBAR専用(ウインドウ枠を持たない)
 *
 * --------------------------------------------------------------------------
 * 【WINDOW】←→ [xxxx]    さまざまなウィジットを子に持てる
 *
 *---------------------------------------------------------------------------*/
/* ポップアップウインドウの範囲外にてマウスクリックしたら、シグナル発生 */
static void window_event_button_on(Q8tkWidget *widget)
{
	widget_signal_do(widget, "inactivate");
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_window_new(int window_type)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_WINDOW;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->stat.window.type = window_type;

	switch (window_type) {

	case Q8TK_WINDOW_TOPLEVEL:
		w->stat.window.no_frame = TRUE;
		break;

	case Q8TK_WINDOW_TOOL:
	case Q8TK_WINDOW_STAT:
		w->stat.window.no_frame = TRUE;
		w->stat.window.set_position = TRUE;
		break;

	case Q8TK_WINDOW_DIALOG:
		w->stat.window.no_frame = FALSE;
		w->stat.window.shadow_type = Q8TK_SHADOW_OUT;
		break;

	case Q8TK_WINDOW_POPUP:
		w->stat.window.no_frame = FALSE;
		w->stat.window.shadow_type = Q8TK_SHADOW_ETCHED_OUT;
		w->event_button_on = window_event_button_on;
		break;
	}

	return w;
}

void q8tk_window_set_shadow_type(Q8tkWidget *widget, int shadow_type)
{
	widget->stat.window.shadow_type = shadow_type;
}




/*---------------------------------------------------------------------------
 * ダイアログ (DIALOG)
 *---------------------------------------------------------------------------
 * ・ダイアログウインドウを開く。
 *   +-------------------+
 *   | WARING !          |↑
 *   | Quit, really?     |この部分は、content_area (垂直ボックス)
 *   | press button.     |↓
 *   | ----------------- |
 *   | +------+ +------+ |
 *   | |  OK  | |CANCEL| |
 *   | +------+ +------+ |
 *   +-------------------+
 *     ← この部分は  →
 *        action_area (水平ボックス)
 * ・ダイアログを生成した後は、 content_area と action_area に任意の
 *   ウィジットを設定できる。q8tk_box_pack_start() / end() にて設定する。
 * ・このウインドウはモーダレスなので注意。
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_dialog_new(void)
 *    ダイアログの生成。
 *
 *    ここで生成したダイアログの content_area にウィジットを追加する場合の例。
 *        d = q8tk_dialog_new();
 *        q8tk_box_pack_start(Q8TK_DIALOG(d)->content_area, label);
 *
 *    ここで生成したダイアログの action_area にウィジットを追加する場合の例。
 *        d = q8tk_dialog_new();
 *        q8tk_box_pack_start(Q8TK_DIALOG(d)->action_area, button);
 *
 * void q8tk_dialog_set_style(Q8tkWidget *widget,
 *                            int frame_style, int separator_style)
 *    ダイアログ外枠のフレームと、エリアセパレータの見た目を設定する。
 *    frame_style は Q8TK_SHADOW_XXX、ただし -1 ならフレームなし
 *    separator_style は Q8TK_SEPARATOR_XXX、 ただし -1 ならセパレータなし
 * --------------------------------------------------------------------------
 *     ：……→【DIALOG】
 *     ↓
 * 【WINDOW】←→【FRAME】←→【VBOX】←→【VBOX(content_area)】←→ [ユーザ]
 *                                             ↑｜                    ↑↓
 *                                             ｜｜             ←   [ユーザ]
 *                                             ｜↓                    ↑↓
 *                                    ←  【HSEP】
 *                                             ↑｜
 *                                             ｜↓
 *                                    ←  【HBOX(action_area)】 ←→ [ユーザ]
 *                                                                     ↑↓
 *                                                              ←   [ユーザ]
 *                                                                     ↑↓
 *
 * q8tk_dialog_new()の返り値は 【WINDOW】 のウィジットである。
 * モーダルにする場合は、このウィジットを q8tk_grab_add() する。
 *
 * content_area、action_area はユーザが任意にウィジットを追加できる。
 *
 * q8tk_widget_destroy() では、内部で生成したウィジットのみが削除され、
 * ユーザが content_area、action_area に追加したウィジットは削除されない。
 * q8tk_widget_destroy_all_related() では、ユーザが追加したウィジットも
 * まとめて削除される。
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_dialog_new(void)
{
	Q8tkWidget *dialog, *window, *vbox, *frame, *hsep;

	dialog = malloc_widget();
	dialog->type = Q8TK_TYPE_DIALOG;
	dialog->sensitive = TRUE;

	window = q8tk_window_new(Q8TK_WINDOW_DIALOG);
	window->stat.window.work = dialog;

	frame = q8tk_frame_new("");
	q8tk_widget_show(frame);
	q8tk_frame_set_shadow_type(frame, Q8TK_SHADOW_NONE);
	q8tk_container_add(window, frame);
	dialog->stat.dialog.frame = frame;

	vbox = q8tk_vbox_new();
	q8tk_widget_show(vbox);
	q8tk_container_add(frame, vbox);
	dialog->stat.dialog.box = vbox;

	dialog->stat.dialog.content_area = q8tk_vbox_new();
	q8tk_box_pack_start(vbox, dialog->stat.dialog.content_area);
	q8tk_widget_show(dialog->stat.dialog.content_area);

	hsep = q8tk_hseparator_new();
	q8tk_hseparator_set_style(hsep, Q8TK_SEPARATOR_WEAK);
	q8tk_box_pack_start(vbox, hsep);
	q8tk_widget_show(hsep);
	dialog->stat.dialog.separator = hsep;

	dialog->stat.dialog.action_area = q8tk_hbox_new();
	q8tk_box_pack_start(vbox, dialog->stat.dialog.action_area);
	q8tk_widget_show(dialog->stat.dialog.action_area);
	q8tk_misc_set_placement(dialog->stat.dialog.action_area,
							Q8TK_PLACEMENT_X_RIGHT, Q8TK_PLACEMENT_Y_BOTTOM);

	return window;
}

void q8tk_dialog_set_style(Q8tkWidget *widget,
						   int frame_style, int separator_style)
{
	Q8tkWidget *w;

	w = Q8TK_DIALOG(widget)->frame;

	switch (frame_style) {
	case Q8TK_SHADOW_NONE:
	case Q8TK_SHADOW_IN:
	case Q8TK_SHADOW_OUT:
	case Q8TK_SHADOW_ETCHED_IN:
	case Q8TK_SHADOW_ETCHED_OUT:
		/* フレームタイプ設定。現在フレームなしならば生成する */
		if (w == NULL) {
			q8tk_container_remove(widget, Q8TK_DIALOG(widget)->box);

			w = q8tk_frame_new("");
			q8tk_widget_show(w);
			Q8TK_DIALOG(widget)->frame = w;

			q8tk_container_add(widget, w);
			q8tk_container_add(w, Q8TK_DIALOG(widget)->box);
		}
		q8tk_frame_set_shadow_type(w, frame_style);
		break;

	default:
		/* フレームなし設定。現在フレームありならば削除する */
		if (w) {
			q8tk_container_remove(w, Q8TK_DIALOG(widget)->box);
			q8tk_widget_destroy(w);
			Q8TK_DIALOG(widget)->frame = NULL;

			q8tk_container_add(widget, Q8TK_DIALOG(widget)->box);
		}
		break;
	}


	w = Q8TK_DIALOG(widget)->separator;

	switch (separator_style) {
	case Q8TK_SEPARATOR_NORMAL:
	case Q8TK_SEPARATOR_WEAK:
	case Q8TK_SEPARATOR_STRONG:
		/* セパレータタイプ設定。現在セパレータなしならば生成する */
		if (w == NULL) {
			q8tk_container_remove(Q8TK_DIALOG(widget)->box,
								  Q8TK_DIALOG(widget)->content_area);
			q8tk_container_remove(Q8TK_DIALOG(widget)->box,
								  Q8TK_DIALOG(widget)->action_area);

			w = q8tk_hseparator_new();
			q8tk_widget_show(w);
			Q8TK_DIALOG(widget)->separator = w;

			q8tk_box_pack_start(Q8TK_DIALOG(widget)->box,
								Q8TK_DIALOG(widget)->content_area);
			q8tk_box_pack_start(Q8TK_DIALOG(widget)->box,
								Q8TK_DIALOG(widget)->separator);
			q8tk_box_pack_start(Q8TK_DIALOG(widget)->box,
								Q8TK_DIALOG(widget)->action_area);
		}
		q8tk_hseparator_set_style(w, separator_style);
		break;

	default:
		/* セパレータなし設定。現在セパレータありならば削除する */
		if (w) {
			q8tk_container_remove(Q8TK_DIALOG(widget)->box, w);
			q8tk_widget_destroy(w);
			Q8TK_DIALOG(widget)->separator = NULL;
		}
		break;
	}
}
