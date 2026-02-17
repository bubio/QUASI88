/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"


/*---------------------------------------------------------------------------
 * コンボボックス (COMBO BOX)
 *---------------------------------------------------------------------------
 * ・エントリ領域を持つ。
 * ・子は持てない。
 * ・q8tk_combo_append_popdown_strings() により、その文字列をラベルに持った
 *   リストアイテムを内部で生成する。これは一覧選択のポップアップウインドウ
 *   にて表示される。
 * ・シグナル
 *   "activate"    リストアイテムが選択されたことにより、
 *                 エントリ領域の文字列に変更があった時に発生
 *                 ないし、エントリ部にてリターンキー入力があった時に発生
 *   "changed"     エントリ部にて文字入力、文字削除があった時に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_combo_new(void)
 *    コンボボックスの生成。生成時は、入力は不可になっている。
 *
 * void q8tk_combo_append_popdown_strings(Q8tkWidget *combo,
 *                                        const char *entry_str,
 *                                        const char *disp_str)
 *    一覧選択時のポップアップウインドウを構成するための、リストアイテムを
 *    生成する。このリストアイテムは、 disp_str の文字列をラベルに持つ。
 *        一覧選択時のポップアップウインドウにてこのリストアイテムが選択
 *        された場合、 コンボボックスのエントリ領域に entry_str がセット
 *        される。("activate"シグナルが発生する)
 *    disp_str == NULL の場合は、 disp_str は entry_str と同じになる。
 *
 * const char *q8tk_combo_get_text(Q8tkWidget *combo)
 *    エントリ領域に入力されている文字列を返す
 *
 * void q8tk_combo_set_text(Q8tkWidget *combo, const char *text)
 *    文字列をエントリ領域に設定する (シグナルは発生しない)。
 *
 * void q8tk_combo_set_editable(Q8tkWidget *combo, int editable)
 *    エントリ領域の入力可・不可を設定する。 editable が 真なら入力可。
 *
 * void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
 *    コンボボックスの表示サイズ width を指定する。 height は無効。
 * --------------------------------------------------------------------------
 * 【COMBO BOX】
 *         ｜
 *         └──【ENTRY】
 *
 * q8tk_combo_append_popdown_strings にて LIST ITEM と LABEL を生成する。
 *
 * 【COMBO BOX】
 *     ｜  ｜
 *     ｜  └──【ENTRY】
 *     ｜
 *     └────  LIST  →【LIST ITEM】←→【LABEL】
 *                 ↑↓
 *                 LIST  →【LIST ITEM】←→【LABEL】
 *                 ↑↓
 *                 LIST  →【LIST ITEM】←→【LABEL】
 *                 ↑↓
 *
 * コンボボックスがマウスでクリックされると、以下のようなウインドウ
 * (Q8TK_WINDOW_POPUP) を自動的に生成し、そのウインドウが q8tk_grab_add()
 * される。つまり、このウインドウへの操作以外は、出来なくなる。
 *
 * 【WINDOW】←→【LIST BOX】←→【LIST ITEM】←→【LABEL】
 *     ｜                            ↑↓
 *     ｜                    ←  【LIST ITEM】←→【LABEL】
 *     ｜                            ↑↓
 *     ｜                    ←  【LIST ITEM】←→【LABEL】
 *     ｜                            ↑↓
 *     ｜
 *     └─【ACCEL GROUP】←→【ACCEL KEY】 → 【BUTTON(ダミー)】
 *
 * この LIST ITEM と LABEL は q8tk_combo_append_popdown_strings() にて
 * 自動生成されたリストアイテム (とラベル) である。
 *
 * リストアイテムのいずれか一つが選択されると、ここで自動生成した
 * ウィジット(リストアイテムは除く)は全て削除され、もとの表示に戻る。
 * この時、選択したリストアイテムの持つ文字列が、エントリ領域に
 * セットされ、同時にシグナルが発生する。
 *
 * ESCキーが押された場合も、元の表示の戻る。(この場合は当然シグナルなし)
 * これを実現するためにアクセラレータキーを利用しているが、アクセラレータ
 * キーは、ウィジットにシグナルを送ることしか出来ないので、ダミーのボタン
 * (非表示) を生成して、このボタンにシグナルを送るようにしている。
 * このボタンはシグナルと受けると、全てのウィジットを削除する。
 *
 * リストが長くなった場合、リストボックスの表示が画面に収まらないことが
 * あるので、その場合は WINDOW と LIST BOX の間にスクロールドウインドウ
 * を生成する。(画面に収まるかどうかの判断は結構いい加減)
 *
 *---------------------------------------------------------------------------*/

/* ポップアップウインドウで、LIST ITEM 選択 or ESCキー押下 or ウインドウ外を
 * マウスクリックした時のコールバック関数。ポップアップウインドウを削除する */
static void combo_fake_callback(UNUSED_WIDGET, Q8tkWidget *parent)
{
	Q8List *l;

	l = parent->stat.combo.list;
	/* l->data は LIST ITEM */

	while (l) {
		q8tk_signal_handlers_destroy((Q8tkWidget *)(l->data));
		l = l->next;
	}

	q8tk_grab_remove(parent->stat.combo.popup_window);

	if (parent->stat.combo.popup_scrolled_window) {
		q8tk_widget_destroy(parent->stat.combo.popup_scrolled_window);
	}

	/* LIST BOX は削除するが、 LIST ITEM は削除せずに残す */
	q8tk_widget_destroy(parent->stat.combo.popup_list);
	q8tk_widget_destroy(parent->stat.combo.popup_window);
	q8tk_widget_destroy(parent->stat.combo.popup_fake);
	q8tk_widget_destroy(parent->stat.combo.popup_accel_group);
}

/* ポップアップウインドウの LIST ITEM をマウスクリックした時のコールバック */
static void combo_event_list_callback(Q8tkWidget *list_item,
									  Q8tkWidget *parent)
{
	if (strcmp(parent->stat.combo.entry->name, list_item->child->name) == 0) {
		/* 現在の入力と、選択したリストの文字列が同じなので、シグナルは無し */
		;
	} else {
		/* 現在の入力と、選択したリストの文字列が同じなので、シグナルは無し */
		q8tk_entry_set_text(parent->stat.combo.entry,
							list_item->child->name);
		widget_signal_do(parent, "activate");
	}

	combo_fake_callback(NULL, parent);
}

/* コンボの一覧表示ボタンをマウスクリックしたら、ポップアップウインドウ生成 */
static void combo_event_button_on(Q8tkWidget *widget)
{
	int selected = FALSE;
	Q8List *l;
	int scrolled = FALSE;
	int y_pos;					/* ポップアップウインドウ表示位置 Y座標 */
	int widget_y_pos;			/* COMBO BOX の表示位置 Y座標 */

	/* Y座標を0起点に補正しておく。y_posを算出したらそちらも補正・・・? */
	widget_y_pos = widget->y - Q8GR_TVRAM_USER_Y;

	widget->stat.combo.popup_window = q8tk_window_new(Q8TK_WINDOW_POPUP);
	widget->stat.combo.popup_scrolled_window = NULL;
	widget->stat.combo.popup_list = q8tk_listbox_new();

	l = widget->stat.combo.list;
	/* l->data は LIST ITEM */

	while (l) {
		q8tk_container_add(widget->stat.combo.popup_list,
						   (Q8tkWidget *)(l->data));

		/* エントリの文字列の一致するリストアイテムを、選択状態とする。
		 * q8tk_listbox_select_child() は
		 * シグナル発生するのでシグナル登録前に処理 */

		if (selected == FALSE) {
			/* l->data->child は LABEL */

			if (strcmp(widget->stat.combo.entry->name,
					   ((Q8tkWidget *)(l->data))->child->name) == 0) {
				q8tk_listbox_select_child(widget->stat.combo.popup_list,
										  (Q8tkWidget *)(l->data));
				selected = TRUE;
			}
		}

		q8tk_signal_connect((Q8tkWidget *)(l->data), "select",
							combo_event_list_callback, widget);
		l = l->next;
	}


	q8tk_widget_show(widget->stat.combo.popup_list);


	if (widget_y_pos + widget->stat.combo.nr_items + 1 < Q8GR_TVRAM_USER_H) {
		/* 下部に表示しても、画面からはみ出さない → 通常の WINDOW 生成 */
		/* (左辺の +1 は、ウインドウの上縁の分。下縁ははみ出てもいいや) */

		scrolled = FALSE;
		y_pos    = widget_y_pos + 1;

	} else if (widget_y_pos - widget->stat.combo.nr_items - 1 >= 0) {
		/* 上部に表示すれば、画面からはみ出さない → 通常の WINDOW 生成 */
		/* (左辺の -1 は、ウインドウの下縁の分。上縁ははみ出てもいいや) */

		scrolled = FALSE;
		y_pos    = widget_y_pos - widget->stat.combo.nr_items - 2;

	} else {
		/* どっちに表示しても、画面からはみ出す → SCROLLED WINDOW 生成 */

		if (widget_y_pos <= Q8GR_TVRAM_USER_H / 2) {
			/* 下部のほうが広いので、下部に表示 */
			scrolled = TRUE;
			y_pos    = widget_y_pos + 1;

		} else {
			/* 上部のほうが広いので、上部に表示 (上縁ははみ出させよう) */
			scrolled = TRUE;
			y_pos    = -1;
		}
	}

	if (scrolled) {
		/* SCROLLED WINDOW の場合 */
		int height;

		if (y_pos > widget_y_pos) {
			/* 下部 */
			height = (24 + 1) - 2 - widget_y_pos;
		} else {
			/* 上部 */
			height = widget_y_pos - 2 + 1;
		}
		if (height < 3) {
			/* ありえないが、念の為 */
			height = 3;
		}

		widget->stat.combo.popup_scrolled_window =
			q8tk_scrolled_window_new(NULL, NULL);
		q8tk_container_add(widget->stat.combo.popup_scrolled_window,
						   widget->stat.combo.popup_list);

		/* 上下で、アクティブなITEMの表示位置を変えてもいいかも。↓これを± */
		q8tk_listbox_set_placement(widget->stat.combo.popup_list, +1, +1);

		q8tk_scrolled_window_set_policy(
			widget->stat.combo.popup_scrolled_window,
			Q8TK_POLICY_AUTOMATIC, Q8TK_POLICY_ALWAYS);

		if (widget->stat.combo.width) {
			q8tk_misc_set_size(widget->stat.combo.popup_scrolled_window,
							   widget->stat.combo.width + 3, height);
		} else {
			q8tk_misc_set_size(widget->stat.combo.popup_scrolled_window,
							   widget->stat.combo.length + 3, height);
		}

		q8tk_widget_show(widget->stat.combo.popup_scrolled_window);
		q8tk_container_add(widget->stat.combo.popup_window,
						   widget->stat.combo.popup_scrolled_window);

	} else {
		/* ノーマル WINDOW の場合 */

		q8tk_container_add(widget->stat.combo.popup_window,
						   widget->stat.combo.popup_list);

	}


	q8tk_widget_show(widget->stat.combo.popup_window);
	q8tk_grab_add(widget->stat.combo.popup_window);
	q8tk_widget_set_focus(widget->stat.combo.popup_list);

	if (widget->stat.combo.width) {
		q8tk_misc_set_size(widget->stat.combo.popup_list,
						   widget->stat.combo.width, 0);
	}

	widget->stat.combo.popup_window->stat.window.set_position = TRUE;
	widget->stat.combo.popup_window->stat.window.x =
		widget->x - 1 - ((widget->stat.combo.popup_scrolled_window) ? 1 : 0);
	widget->stat.combo.popup_window->stat.window.y =
		y_pos; /* + Q8GR_TVRAM_USER_Y; */

	q8tk_signal_connect(widget->stat.combo.popup_window,
						"inactivate", combo_fake_callback, widget);


	/* ESC キーを押した時リストを消去するための、ダミーを生成 */

	widget->stat.combo.popup_fake = q8tk_button_new();
	q8tk_signal_connect(widget->stat.combo.popup_fake, "clicked",
						combo_fake_callback, widget);

	widget->stat.combo.popup_accel_group = q8tk_accel_group_new();
	q8tk_accel_group_attach(widget->stat.combo.popup_accel_group,
							widget->stat.combo.popup_window);
	q8tk_accel_group_add(widget->stat.combo.popup_accel_group, Q8TK_KEY_ESC,
						 widget->stat.combo.popup_fake, "clicked");
}

/* リターン・スペースを押したら、一覧表示ボタンマウスクリックと同じ処理をする
 * ↑↓キーを押した場合も同様 */
static void combo_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		combo_event_button_on(widget);
	}

#ifdef  OLD_FOCUS_CHANGE
	/* DO NOTHING */
#else /* 新たなフォーカス移動方式 */
	if (key == Q8TK_KEY_UP || key == Q8TK_KEY_DOWN) {
		combo_event_button_on(widget);
	}
#endif
}

/* エントリ部に入力があったとき、シグナル発生 */
static void combo_event_entry_activate(UNUSED_WIDGET, Q8tkWidget *parent)
{
	widget_signal_do(parent, "activate");
}
static void combo_event_entry_changed(UNUSED_WIDGET, Q8tkWidget *parent)
{
	widget_signal_do(parent, "changed");
}


Q8tkWidget *q8tk_combo_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_COMBO;
	w->sensitive = TRUE;

	w->key_up_used    = TRUE;
	w->key_down_used  = TRUE;

	w->stat.combo.entry  = q8tk_entry_new();
	w->stat.combo.list   = NULL;
	w->stat.combo.width  = 0;
	w->stat.combo.length = 0;

	w->stat.combo.entry->stat.entry.combo = w;

	q8tk_combo_set_editable(w, FALSE);
	q8tk_widget_show(w->stat.combo.entry);
	q8tk_signal_connect(w->stat.combo.entry, "activate",
						combo_event_entry_activate, w);
	q8tk_signal_connect(w->stat.combo.entry, "changed",
						combo_event_entry_changed, w);

	w->event_button_on = combo_event_button_on;
	w->event_key_on    = combo_event_key_on;

	return w;
}
void q8tk_combo_append_popdown_strings(Q8tkWidget *combo,
									   const char *entry_str,
									   const char *disp_str)
{
	int l0, l1, l2;
	Q8tkWidget *list_item;

	if (combo->stat.combo.list == NULL) {
		/* 念のため初期化 */
		combo->stat.combo.length   = 0;
		combo->stat.combo.nr_items = 0;
	}

	/* 文字列長をチェック */

	if (entry_str == NULL) {
		entry_str = "";
	}
	if (disp_str  == NULL) {
		disp_str  = entry_str;
	}

	l0 = combo->stat.combo.length;
	l1 = (disp_str) ? q8gr_strlen(q8tk_kanji_code, disp_str) : 0;
	l2 = q8gr_strlen(q8tk_kanji_code, entry_str)
		 + ((combo->stat.combo.entry->stat.entry.editable) ? +1 : 0);

	combo->stat.combo.length = Q8TKMAX(Q8TKMAX(l0, l1), l2);


	/* ラベル付き LIST ITEM を生成し、そのポインタをリストに保持 */

	list_item = q8tk_list_item_new_with_label(disp_str, 0);
	q8tk_widget_show(list_item);
	q8tk_list_item_set_string(list_item, entry_str);

	combo->stat.combo.list = q8_list_append(combo->stat.combo.list, list_item);
	combo->stat.combo.nr_items ++;


	/* コンボの入力部に、この文字列をセット */

	q8tk_entry_set_text(combo->stat.combo.entry, entry_str);

	if (combo->stat.combo.width) {
		q8tk_misc_set_size(combo->stat.combo.entry,
						   combo->stat.combo.width, 0);
	} else {
		q8tk_misc_set_size(combo->stat.combo.entry,
						   combo->stat.combo.length, 0);
	}
}

#if 0
void q8tk_combo_clear_popdown_strings(Q8tkWidget *combo,
									  int start, int end)
{
	/* 未作成 */
}
#endif

const char *q8tk_combo_get_text(Q8tkWidget *combo)
{
	return combo->stat.combo.entry->name;
}

void q8tk_combo_set_text(Q8tkWidget *combo, const char *text)
{
	q8tk_entry_set_text(combo->stat.combo.entry, text);
}

void q8tk_combo_set_editable(Q8tkWidget *combo, int editable)
{
	q8tk_entry_set_editable(combo->stat.combo.entry, editable);
}
