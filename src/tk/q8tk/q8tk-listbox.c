/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"
#include "screen.h"
#include "alarm.h"


/*---------------------------------------------------------------------------
 * リストボックス (LIST BOX)
 *---------------------------------------------------------------------------
 * ・垂直ボックスのようなウィジット。
 * ・複数の子をもてる。  但し、リストアイテム以外の子を持ってはいけない。
 * ・子を持つには、q8tk_container_add() を使用する。
 *   子は、q8tk_container_add() を呼び出した順番に下へ下へと並んでいく。
 * ・リストアイテムのいずれか一つを選択状態にすることができる。
 *   (選択なしにすることもできるが、複数を選択状態にすることはできない)
 * ・シグナル
 *   "selection_change"    リストアイテムをマウスクリックした時、
 *                         現在選択状態となっているリストアイテムが
 *                         変化したら発生する。
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_listbox_new(void)
 *    リストボックスの生成。
 *
 * void q8tk_listbox_clear_items(Q8tkWidget *wlist, int start, int end)
 *    子のリストアイテムを削除する。
 *    リストアイテムは、q8tk_container_add()を呼び出した順に 0 からの番号が
 *    振られるので、この番号で、削除するリストアイテムを指定する。
 *    end が start よりも小さい場合は、全てのリストアイテムが削除される。
 *    削除したリストアイテムは消滅するので後から再利用することはできない。
 *    後で利用したい場合はひとつずつ q8tk_container_remove() にて削除すべし。
 *
 * void q8tk_listbox_select_item(Q8tkWidget *wlist, int item)
 *    item の番号のリストアイテムを選択状態にする (シグナルが発生する)。
 *
 * void q8tk_listbox_select_child(Q8tkWidget *wlist, Q8tkWidget *child)
 *    child のリストアイテムを選択状態にする (シグナルが発生する)。
 *
 * void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
 *    リストボックスの表示サイズ width を指定する。 height は無効。
 *
 * void q8tk_listbox_set_placement(Q8tkWidget *widget,
 *                                 int top_pos, int left_pos)
 *    リストボックスがスクロールドウインドウの子である場合、リストボックスの
 *    子のリストアイテムのうち、アクティブなものがスクロールインするように
 *    表示位置が調整されるが、その調整位置を指定する。
 *        top_pos が正の場合は、アクティブなものがウインドウ上部に、
 *        負の場合は、アクティブなものがウインドウ下部に、表示される
 *        ように調整される。値の絶対値が大きくなるほど、ウインドウ中央
 *        寄りに表示される。
 *        left_pos が正の場合は、リストアイテムの先頭が、負の場合は、
 *        リストアイテムの末尾が表示されるように調整される。値の絶対値は
 *        関係ない。
 *        値が 0 の場合 (デフォルト) は、 -1 とみなされる。(つまり、
 *        ウインドウ下部にアイテムの末尾が表示されるように調整される)
 *
 * void q8tk_listbox_set_big(Q8tkWidget *widget, int big)
 *
 * --------------------------------------------------------------------------
 * 【LIST BOX】←→ [LIST ITEM] ←→ [LABEL]    リストアイテム (と、ラベル)
 *    ：               ↑↓                     を子に持つ
 *    ：       ←   [LIST ITEM] ←→ [LABEL]
 *    ：               ↑↓
 *    ：…->   ←   [LIST ITEM] ←→ [LABEL]
 * キー入力            ↑↓
 * にて        ←   [LIST ITEM] ←→ [LABEL]
 * いずれかを          ↑↓
 * 選択状態に
 * する
 *     ……… → 選択された LIST ITEM は マウスクリックされたことになる
 *
 *
 * リストボックスがスクロールドウインドウの子である場合、スクロール
 * ウインドウにフォーカスされると直ちにリストボックスにフォーカスが
 * 移る。これにより、リストボックスはキー入力を受けつけるようになる。
 *
 * キー入力にて、あるリストアイテムを選択した場合、
 * そのリストアイテムがマウスでクリックされたのと同じ動作をする。
 *
 * q8tk_listbox_select_item() 、q8tk_listbox_select_child() で
 * リストアイテムを選択した場合とは動作が違う？？？
 *
 *---------------------------------------------------------------------------*/

static void list_scroll_adjust(Q8tkWidget *widget);

#define	BUF_SIZE	sizeof(widget->stat.listbox.input)


static void cb_list_event_key_on(Q8tkWidget *widget, UNUSED_PARM)
{
	char *input = &widget->stat.listbox.input[0];
	memset(input, 0, BUF_SIZE);
}

/* 文字キーを押したら、その文字を先頭にもつリストアイテムがアクティブにする  */
/* ↑↓キーを押したら、上下のリストアイテムをアクティブにする                */
/* スペース・リターンを押したら、そのリストアイテムを選択する (シグナル発生) */
static void list_event_key_on(Q8tkWidget *widget, int key)
{
	Q8tkWidget *w = widget->stat.listbox.active;
	char *input = &widget->stat.listbox.input[0];

	if (w) {
		if (key <= 0xff &&
			isgraph(key)) {
			/* スペース以外の文字が入力された場合 */

			Q8tkWidget *active = w;
			size_t len = strlen(input);
			if (len < BUF_SIZE - 1) {
				if ((len == 1) &&
					(input[0] == key)) {
					/* 1文字目と2文字目が同じ場合に限り、2文字目を無視 */

				} else {
					/* 入力した文字を逐次保存 */
					input[len    ] = (char) key;
					input[len + 1] = '\0';
					len ++;
				}
			}

			/* リストアイテムの、子(ラベル)の文字列とキー入力した文字列が
			 * 一致するものを順次探していく。
			 * リストアイテムの末尾の次は先頭に戻り、すべてを対象に探す。
			 *
			 * 入力文字列が 1文字の場合は、アクティブなリストアイテムより
			 * 後ろのリストアイテムからチェックを開始するが、
			 * 入力文字列が 2文字以上の場合は、アクティブなリストアイテム
			 * からチェックを開始する */

			if (len == 1) {
				w = w->next;
				if (w == NULL) {
					w = widget->child;
				}
			}

			for (;;) {

				if (w->child) {
					if ((w->name        && strncmp(w->name,        input, len) == 0) ||
						(w->child->name && strncmp(w->child->name, input, len) == 0)) {
						/* 見つかったら それをアクティブに*/
						widget->stat.listbox.active = w;
						/* 表示位置を調節 */
						list_scroll_adjust(widget);

						alarm_remove(widget);
						alarm_add(widget, NULL,
								  (alarm_callback_func) cb_list_event_key_on,
								  900, FALSE);
						break;
					}
				}

				w = w->next;
				if (w == NULL) {
					w = widget->child;
				}

				if (w == active || w == NULL) {
					/* なければあきらめる */
					break;
				}
			}

		} else {
			/* スペースや制御文字が入力された場合 */

			int do_clear = TRUE;
			switch (key) {

			case Q8TK_KEY_RET:
			case Q8TK_KEY_SPACE:
				(*w->event_key_on)(w, key);
				break;

			case Q8TK_KEY_UP:
			case Q8TK_KEY_HOME:
				if (w->prev) {
					w = w->prev;
					if (key == Q8TK_KEY_HOME) {
						while (w->prev) {
							w = w->prev;
						}
					}
					/* 1個前/一番前をアクティブ */
					widget->stat.listbox.active = w;
					/* 表示位置を調節 */
					list_scroll_adjust(widget);
				}
				break;

			case Q8TK_KEY_DOWN:
			case Q8TK_KEY_END:
				if (w->next) {
					w = w->next;
					if (key == Q8TK_KEY_END) {
						while (w->next) {
							w = w->next;
						}
					}
					/* 1個後/一番後をアクティブ */
					widget->stat.listbox.active = w;
					/* 表示位置を調節 */
					list_scroll_adjust(widget);
				}
				break;

			case Q8TK_KEY_PAGE_UP:
			case Q8TK_KEY_PAGE_DOWN:
				if (widget->parent &&
					widget->parent->type == Q8TK_TYPE_SCROLLED_WINDOW &&
					widget->parent->stat.scrolled.vscrollbar) {

					int h = widget->parent->stat.scrolled.child_sy;
					if (widget->stat.listbox.big == FALSE) {
						h = h - 1;
					} else {
						h = h / 2 - 1;
					}
					if (key == Q8TK_KEY_PAGE_UP) {
						for (; h > 0; h--) {
							if (w->prev == NULL) {
								break;
							}
							w = w->prev;
						}
					} else {
						for (; h > 0; h--) {
							if (w->next == NULL) {
								break;
							}
							w = w->next;
						}
					}
					/* 1頁前/1頁後をアクティブ */
					widget->stat.listbox.active = w;
					/* 表示位置を調節 */
					list_scroll_adjust(widget);
				}
				break;

			case Q8TK_KEY_BS:
			case Q8TK_KEY_DEL:
			case Q8TK_KEY_TAB:
			case Q8TK_KEY_ESC:
			case Q8TK_KEY_RIGHT:
			case Q8TK_KEY_LEFT:
			case Q8TK_KEY_F1:
			case Q8TK_KEY_F2:
			case Q8TK_KEY_F3:
			case Q8TK_KEY_F4:
			case Q8TK_KEY_F5:
			case Q8TK_KEY_F6:
			case Q8TK_KEY_F7:
			case Q8TK_KEY_F8:
			case Q8TK_KEY_F9:
			case Q8TK_KEY_F10:
			case Q8TK_KEY_MENU:
			case Q8TK_BUTTON_U:
			case Q8TK_BUTTON_D:
				/* その他の制御文字は、処理なし */
				break;

			default:
				/* シフトなどのキーは、キー入力文字列を保持し続ける */
				do_clear = FALSE;
				break;
			}

			if (do_clear) {
				alarm_remove(widget);
				memset(input, 0, BUF_SIZE);
			}
		}
	}

}



static void list_scroll_adjust(Q8tkWidget *widget)
{
	int i;
	Q8tkWidget *c;

	widget->stat.listbox.insight = widget->stat.listbox.active;

	if (widget->parent &&
		widget->parent->type == Q8TK_TYPE_SCROLLED_WINDOW) {

		c = widget->child;
		for (i = 0; c ; i++) {
			if (c == widget->stat.listbox.insight) {
				break;
			}
			c = c->next;
		}

		if (widget->stat.listbox.big == FALSE) {
			if (i < widget->parent->stat.scrolled.child_y0) {
				Q8TK_ADJUSTMENT(widget->parent->stat.scrolled.vadj)->value = i;
			} else if (i >= widget->parent->stat.scrolled.child_y0
					   + widget->parent->stat.scrolled.child_sy) {
				Q8TK_ADJUSTMENT(widget->parent->stat.scrolled.vadj)->value
					= i - widget->parent->stat.scrolled.child_sy + 1;
			}
		} else {
			if (i * 2 < widget->parent->stat.scrolled.child_y0) {
				Q8TK_ADJUSTMENT(widget->parent->stat.scrolled.vadj)->value = i * 2;
			} else if (i * 2 + 1 >= widget->parent->stat.scrolled.child_y0
					   + widget->parent->stat.scrolled.child_sy) {
				Q8TK_ADJUSTMENT(widget->parent->stat.scrolled.vadj)->value
					= i * 2 - widget->parent->stat.scrolled.child_sy + 1 + 2;
			}
		}
	}
	set_construct_flag(TRUE);
}





Q8tkWidget *q8tk_listbox_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_LISTBOX;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->key_up_used    = TRUE;
	w->key_down_used  = TRUE;

	w->event_key_on = list_event_key_on;

	w->stat.listbox.selected = NULL;
	w->stat.listbox.active   = NULL;
	w->stat.listbox.insight  = NULL;
	w->stat.listbox.big      = FALSE;

	return w;
}

void q8tk_listbox_clear_items(Q8tkWidget *wlist, int start, int end)
{
	int i, count;
	Q8tkWidget *wk, *c = wlist->child;

	if (c == NULL) {
		return;
	}
	if (start < 0) {
		return;
	}

	if (end < start) {
		count = -1;
	} else {
		count = end - start + 1;
	}

	for (i = 0; i < start; i++) {
		/* 子の LIST ITEM を start個目まで順にたどる */
		if ((c = c->next) == NULL) {
			return;
		}
	}
	/* そこから end個目までを切り離す */
	while (count) {
		/* LIST ITEM と LABEL も一気に削除する  */
		wk = c->next;
		q8tk_container_remove(wlist, c);
		q8tk_widget_destroy(c);
		if ((c = wk) == NULL) {
			break;
		}
		if (count > 0) {
			count --;
		}
	}

	set_construct_flag(TRUE);
}

/* LIST BOX に繋がった LIST の、 name が s に一致するものを探す */
Q8tkWidget *q8tk_listbox_search_items(Q8tkWidget *wlist, const char *s)
{
	Q8tkWidget *c = wlist->child;

	for (;;) {
		if (c == NULL) {
			return NULL;
		}

		if (c->name &&
			strcmp(c->name, s) == 0) {
			return c;
		}

		c = c->next;
	}
}

void q8tk_listbox_select_item(Q8tkWidget *wlist, int item)
{
	int i;
	Q8tkWidget *c = wlist->child;

	if (c == NULL) {
		return;
	}

	if (item < 0) {
		c = NULL;
	} else {
		for (i = 0; i < item; i++) {
			if ((c = c->next) == NULL) {
				break;
			}
		}
	}

	q8tk_listbox_select_child(wlist, c);
}

void q8tk_listbox_select_child(Q8tkWidget *wlist, Q8tkWidget *child)
{
	if (wlist->stat.listbox.selected != child) {
		wlist->stat.listbox.selected = child;
		wlist->stat.listbox.active   = child;
		widget_signal_do(wlist, "selection_changed");
		set_construct_flag(TRUE);
	}
	if (child) {
		widget_signal_do(child, "select");
		widget_scrollin_register(child);
	}
}

void q8tk_listbox_set_placement(Q8tkWidget *widget,
								int top_pos, int left_pos)
{
	widget->stat.listbox.scrollin_top  = top_pos;
	widget->stat.listbox.scrollin_left = left_pos;
}

void q8tk_listbox_set_big(Q8tkWidget *widget, int big)
{
	if (widget->stat.listbox.big != big) {
		widget->stat.listbox.big = big;
		set_construct_flag(TRUE);
	}
}


/*
 * これはちょっと特殊処理。
 *    LIST BOX が SCROLLED WINDOW の子の場合で、SCROLLED WINDOW のスクロール
 *    バー(縦方向) が動かされたとき、この関数が呼ばれる。
 *    ここでは、SCROLLED WINDOW の表示範囲に応じて、LIST BOX の active
 *    ウィジットを変更している。
 */
extern void list_event_window_scrolled(Q8tkWidget *swin, int sy)
{
	Q8tkWidget *widget = swin->child;   /* == LISTBOX */

	Q8tkWidget *c = widget->child;
	int        nth = 0;

	if (c == NULL) {
		return;
	}

	while (c) {
		if (c == widget->stat.listbox.insight) {
			break;
		}
		nth ++;
		if (widget->stat.listbox.big) {
			nth += (c->next) ? 1 : 2;
		}
		c = c->next;
	}
	nth = nth - swin->stat.scrolled.vadj->stat.adj.value;

	if (0 <= nth && nth < sy) {
		/* Ok, No Adjust */
	} else {

		if (nth < 0) {
			nth = swin->stat.scrolled.vadj->stat.adj.value;
		} else {
			nth = swin->stat.scrolled.vadj->stat.adj.value + sy - 1;
		}

		c = widget->child;
		while (nth--) {
			if ((c = c->next) == NULL) {
				return;
			}
		}
		widget->stat.listbox.insight   = c;
		set_construct_flag(TRUE);

	}
}




/*---------------------------------------------------------------------------
 * リストアイテム (LIST ITEM)
 *---------------------------------------------------------------------------
 * ・リストボックスの子になれる。
 * ・子を一つ持てる。 (但し、ラベル以外の子を持った場合の動作は未保証)
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・内部に文字列を保持できる。
 * ・シグナル
 *   "select"    クリックした時 (すでに選択された状態でも発生)
 *               親のリストボックスが選択状態としているのが自身で無い
 *               場合、先に親に "selection_changed" イベントを発生させる
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_list_item_new(void)
 *    リストアイテムの生成。
 *
 * Q8tkWidget *q8tk_list_item_new_with_label(const char *label, int max_width)
 *    文字列 label が描かれたリストアイテムの生成。
 *    内部的には、リストアイテムとラベルを作り、親子にする。
 *    ラベルの文字列幅は最大でも max_width とみなされるため、LISTBOX にて指定
 *    したサイズよりも小さくしておけば、LISTBOX のサイズ内で表示される。ただし
 *    この場合は、ラベルの文字列は途中までしか表示されないことがある。
 *    max_width == 0 とした場合は、ラベルの文字列はすべて表示される。そのため
 *    LISTBOX は、指定したサイズよりも大きなサイズで表示されることがある。
 *
 * void q8tk_list_item_set_string(Q8tkWidget *w, const char *str)
 *    内部に文字列 str を保持する。
 * --------------------------------------------------------------------------
 * [LIST BOX] ←→ 【LIST ITEM】←→ [LABEL]    ラベルを子に持つ
 *                                               親は、リストボックス
 *    ↑               ↑
 *    ：               ：
 * "selection_change"  ：
 *    ：             "select"
 *    ：………………   ：
 * 先に親に            ：
 * 伝播する        ボタンで選択
 *
 *---------------------------------------------------------------------------*/
#define VISUAL_EFFECT

/* リストアイテムをマウスクリックしたら、アクティブにしてドラッグ開始とする */
/* リストボックスの親がスクロールドウインドウでタッチ操作ならばスワイプ風に */
static void list_item_event_button_on(Q8tkWidget *widget)
{
	set_drag_widget(widget);

	widget->stat.listitem.dir = 0;
	widget->stat.listitem.slide_stat = 0;

	if (widget->parent) {
		q8tk_widget_set_focus(widget->parent);

		if (widget->parent->stat.listbox.active != widget) {
			widget->parent->stat.listbox.active  = widget;
			set_construct_flag(TRUE);
		}

		/* スワイプっぽい動作の準備 */
		Q8tkAssert(widget->parent->type == Q8TK_TYPE_LISTBOX, NULL);
		{
			Q8tkWidget *lbox = widget->parent;
			if ((lbox->parent) &&
				(lbox->parent->type == Q8TK_TYPE_SCROLLED_WINDOW) &&
				(q8tk_mouse.is_touch)) {

				widget->stat.listitem.slide_y = q8tk_mouse.y / 16;
				widget->stat.listitem.slide_stat = 1;
			}
		}
	}
}

/* ドラッグ中は、同じリストボックスであれば、アクティブなリストを切り替える */
/* リストボックス外にドラッグした時にタイマーによるリピート処理を行うCB関数 */
static void cb_list_item_event_dragging_mouse_repeat(Q8tkWidget *widget,
													 UNUSED_PARM)
{
	int timeout;
	Q8tkWidget *lbox = widget->parent;

	if (widget->stat.listitem.dir != 0) {
		if (widget->stat.listitem.dir < 0) {
			list_event_key_on(lbox, Q8TK_KEY_UP);
		} else {
			list_event_key_on(lbox, Q8TK_KEY_DOWN);
		}

		/* 連続呼び出し(ドラッグ保持) なら、タイマーの期間を短くする */
		widget->stat.listitem.cnt++;
		if (widget->stat.listitem.cnt > 40) {
			timeout = 10;
		} else if (widget->stat.listitem.cnt > 10) {
			timeout = 40;
		} else {
			timeout = 100;
		}
		alarm_add(widget, NULL,
				  (alarm_callback_func) cb_list_item_event_dragging_mouse_repeat,
				  timeout, TRUE);
	}
}

/* ドラッグ中は、同じリストボックスであれば、アクティブなリストを切り替える */
static void list_item_event_dragging_mouse(Q8tkWidget *widget)
{
	Q8tkWidget *lbox = widget->parent;
	Q8tkWidget *w = q8gr_get_focus_screen(q8tk_mouse.x / 8, q8tk_mouse.y / 16);

	if (w &&
		w->type == Q8TK_TYPE_LIST_ITEM) {
		if (w->parent == lbox) {
			if (w->parent->stat.listbox.active != w) {
				w->parent->stat.listbox.active  = w;
				set_construct_flag(TRUE);
			}
		}
		widget->stat.listitem.dir = 0;
		alarm_remove(widget);

	} else {
		if (lbox &&
			lbox->type == Q8TK_TYPE_LISTBOX &&
			lbox->parent &&
			lbox->parent->type == Q8TK_TYPE_SCROLLED_WINDOW) {
			int dir, mouse_y = q8tk_mouse.y / 16;
			if (mouse_y <= lbox->parent->y) {
				/* リストボックスより上部なら、上方向にリストをリピート選択 */
				dir = -1;
			} else if ((lbox->parent->y + lbox->parent->sy - 1) <= mouse_y) {
				/* リストボックスより下部なら、上方向にリストをリピート選択 */
				dir = +1;
			} else {
				dir = 0;
			}
			if (SGN(widget->stat.listitem.dir) == SGN(dir)) {
				/* ドラッグ先が同じエリアなら処理なし (タイマー動作は保持) */
				/* DO NOTHING */
			} else {
				/* ドラッグ先が別エリアなら処理開始 (タイマー動作もする) */
				widget->stat.listitem.dir = dir;
				widget->stat.listitem.cnt = 0;
				alarm_remove(widget);
				if (dir == 0) {
					/* DO NOTHING */
				} else {
					if (dir < 0) {
						list_event_key_on(lbox, Q8TK_KEY_UP);
					} else {
						list_event_key_on(lbox, Q8TK_KEY_DOWN);
					}
					alarm_add(widget, NULL,
							  (alarm_callback_func) cb_list_item_event_dragging_mouse_repeat,
							  100, TRUE);
				}
			}
		}
	}
}

/* ドラッグ中は、スクロールドウインドウであれば、スクロールさせる */
static void list_item_event_dragging_touch(Q8tkWidget *widget)
{
	Q8tkWidget *lbox = widget->parent;
	/* Q8tkAssert(lbox->parent->type, Q8TK_TYPE_SCROLLED_WINDOW); */
	Q8tkWidget *vadj = lbox->parent->stat.scrolled.vadj;

	if (vadj &&
		vadj->event_key_on) {

		int mouse_y = q8tk_mouse.y / 16;
		int dy = widget->stat.listitem.slide_y - mouse_y;
		int i, loop = ABS(dy), sgn = SGN(dy);

		for (i = 0; i < loop; i++) {
			if (sgn > 0) {
				vadj->event_key_on(vadj, Q8TK_KEY_DOWN);
			} else {
				vadj->event_key_on(vadj, Q8TK_KEY_UP);
			}
		}

		widget->stat.listitem.slide_y = mouse_y;
		if (loop > 0) {
			/* 一度でもスクロールしたら、 slide_stat = 2 */
			widget->stat.listitem.slide_stat = 2;
		}
	}
}

/* ドラッグ中は、同じリストボックスであれば、アクティブなリストを切り替える */
/* ドラッグ中は、スクロールドウインドウであれば、スクロールさせる */
static void list_item_event_dragging(Q8tkWidget *widget)
{
	if (widget->stat.listitem.slide_stat == 0) {
		/* 基本動作としてのドラッグ操作中 */
		list_item_event_dragging_mouse(widget);

	} else {
		/* リストボックスの親がスクロールドウインドウでタッチ操作中 */
		list_item_event_dragging_touch(widget);
	}
}


/* シグナルを発生させる関数。遅延動作のためコールバック関数にする */
static void cb_list_item_event_drag_off(Q8tkWidget *widget,
										 void *selection_changed_widget)
{
	Q8tkWidget *parent = selection_changed_widget;
	if (parent) {
		widget_signal_do(parent, "selection_changed");
	}
	widget_signal_do(widget, "select");
	set_construct_flag(TRUE);
}

/* ドラッグ終了で、同じリストボックス内であれば、シグナル発生 */
static void list_item_event_drag_off_mouse(Q8tkWidget *widget)
{
	Q8tkWidget *selection_changed_widget = NULL;
	Q8tkWidget *w = q8gr_get_focus_screen(q8tk_mouse.x / 8, q8tk_mouse.y / 16);

	if (w) {
		if (w->type == Q8TK_TYPE_LIST_ITEM) {
			if (w->parent == widget->parent) {

				if (w->parent->stat.listbox.selected != w) {
					w->parent->stat.listbox.selected = w;
					w->parent->stat.listbox.active   = w;

					selection_changed_widget = widget->parent;
				}
			}
#ifndef	VISUAL_EFFECT /* このままだと、選択した状態を描画する暇がない */
			cb_list_item_event_drag_off(w, selection_changed_widget);
#else                 /* 選択した状態を描画させたいのでイベントを遅らせる */
			set_construct_flag(TRUE);
			alarm_add(w, selection_changed_widget,
					  (alarm_callback_func) cb_list_item_event_drag_off,
					  150, TRUE);
#endif
		}
	}
}

/* ドラッグ終了で、スクロールをしなかったならば、シグナル発生 */
static void list_item_event_drag_off_touch(Q8tkWidget *widget)
{
	Q8tkWidget *selection_changed_widget = NULL;

	if (widget->stat.listitem.slide_stat == 1) {
		if (widget->parent) {
			q8tk_widget_set_focus(widget->parent);

			if (widget->parent->stat.listbox.selected != widget) {
				widget->parent->stat.listbox.selected = widget;
				widget->parent->stat.listbox.active   = widget;

				selection_changed_widget = widget->parent;

			}
			list_scroll_adjust(widget->parent);
		}
#ifndef	VISUAL_EFFECT /* このままだと、選択した状態を描画する暇がない */
		cb_list_item_event_drag_off(widget, selection_changed_widget);
#else                 /* 選択した状態を描画させたいのでイベントを遅らせる */
		alarm_add(widget, selection_changed_widget,
				  (alarm_callback_func) cb_list_item_event_drag_off,
				  150, TRUE);
#endif
	}
}

/* ドラッグ終了で、同じリストボックス内であれば、シグナル発生 */
/* ドラッグ終了で、スクロールをしなかったならば、シグナル発生 */
static void list_item_event_drag_off(Q8tkWidget *widget)
{
	alarm_remove(widget);

	if (widget->stat.listitem.slide_stat == 0) {
		/* 基本動作としてのドラッグ操作中 */
		list_item_event_drag_off_mouse(widget);

	} else {
		/* リストボックスの親がスクロールドウインドウでタッチ操作中 */
		list_item_event_drag_off_touch(widget);
	}
}

/* スペース・リターンを押したら、シグナル発生 */
static void list_item_event_key_on(Q8tkWidget *widget, int key)
{
	Q8tkWidget *selection_changed_widget = NULL;

	set_drag_widget(NULL);
	widget->stat.listitem.dir = 0;
	alarm_remove(widget);

	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		if (widget->parent) {
			q8tk_widget_set_focus(widget->parent);

			if (widget->parent->stat.listbox.selected != widget) {
				widget->parent->stat.listbox.selected = widget;
				widget->parent->stat.listbox.active   = widget;

				selection_changed_widget = widget->parent;

			}
			list_scroll_adjust(widget->parent);
		}
#ifndef	VISUAL_EFFECT /* このままだと、選択した状態を描画する暇がない */
		cb_list_item_event_drag_off(widget, selection_changed_widget);
#else                 /* 選択した状態を描画させたいのでイベントを遅らせる */
		alarm_add(widget, selection_changed_widget,
				  (alarm_callback_func) cb_list_item_event_drag_off,
				  150, TRUE);
#endif
	}
}


Q8tkWidget *q8tk_list_item_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_LIST_ITEM;
	w->attr = Q8TK_ATTR_CONTAINER | Q8TK_ATTR_LABEL_CONTAINER;
	w->sensitive = TRUE;

	w->event_button_on = list_item_event_button_on;
	w->event_key_on    = list_item_event_key_on;
	w->event_dragging  = list_item_event_dragging;
	w->event_drag_off  = list_item_event_drag_off;

	return w;
}

Q8tkWidget *q8tk_list_item_new_with_label(const char *label, int max_width)
{
	Q8tkWidget *i = q8tk_list_item_new();
	Q8tkWidget *l = q8tk_label_new(label);

	if (max_width > 0) {
		l->stat.label.width = max_width;
	}

	q8tk_widget_show(l);
	q8tk_container_add(i, l);

	i->with_label = TRUE;

	return i;
}

void q8tk_list_item_set_string(Q8tkWidget *w, const char *str)
{
	if (w->name) {
		free(w->name);
	}
	w->name = (char *)malloc(strlen(str) + 1);

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, str);
	w->code = q8tk_kanji_code;
}
