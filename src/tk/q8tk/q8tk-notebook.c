/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/*---------------------------------------------------------------------------
 * ノートブック (NOTE BOOK / NOTE PAGE)
 *---------------------------------------------------------------------------
 * ・複数のノートページをもてる。
 * ・各ノートページはコンテナであり、子を一つ持てる。
 * ・q8tk_notebook_append() で子を持つが、その度に内部でノートページを生成し
 *   これが子を持つことになる。
 * ・NOTE PAGE は、(見出しの)文字列を保持できる。
 * ・シグナル
 *   "switch_page"    別のページに切り替わった時に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_notebook_new(void)
 *    ノートボックの生成
 *
 * void q8tk_notebook_append(Q8tkWidget *notebook,
 *                           Q8tkWidget *widget, const char *label)
 *    ノートブック noteboot  に新たなページを作成する。
 *    そのページに widget を追加する。見出しの文字列は label。
 *    内部的には、ノートページを作り、親子にしたうえでさらに、
 *    ノートページと wiget を親子にする、
 *
 * int q8tk_notebook_current_page(Q8tkWidget *notebook)
 *    現在のページ番号を取得する。
 *    なお、ノートブックにページを追加した順に、0 からの番号が振られる。
 *    全くページを追加していない場合は -1 。
 *
 * void q8tk_notebook_set_page(Q8tkWidget *notebook, int page_num)
 *    ノートブックのページを指定する(シグナルが発生する)。
 *
 * void q8tk_notebook_next_page(Q8tkWidget *notebook)
 *    ノートブックのページを次頁にする(シグナルが発生する)。
 *
 * void q8tk_notebook_prev_page(Q8tkWidget *notebook)
 *    ノートブックのページを前頁にする(シグナルが発生する)。
 * --------------------------------------------------------------------------
 * 【NOTE BOOK】←→【NOTE PAGE】←→ [xxxx]    ノートページを子に持ち
 *                       ↑↓                   さらにノートページは
 *              ←  【NOTE PAGE】←→ [xxxx]    いろんな子が持てる
 *                       ↑↓
 *              ←  【NOTE PAGE】←→ [xxxx]
 *                       ↑↓
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_notebook_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_NOTEBOOK;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->stat.notebook.page = NULL;

	return w;
}

/* ノートページのタグをマウスクリックしたら、シグナル発生 */
static void notepage_event_button_on(Q8tkWidget *widget)
{
	if ((widget->parent)->stat.notebook.page != widget) {
		(widget->parent)->stat.notebook.page = widget;
		widget_signal_do(widget->parent, "switch_page");

		/* ノートページ切替直後は、フォーカス(アンダーライン)表示しない */
		/*    ・・・なんとなく、表示がカッコ悪い気がするので・・・ */
		if ((widget->parent)->stat.notebook.lost_focus) {
			q8tk_widget_set_focus(NULL);
		}

		set_construct_flag(TRUE);
	}
}

/* リターン・スペースを押したら、マウスクリックと同じ処理をする */
/* ←→キーを押したら、前後のページでの、マウスクリックと同じ処理をする */
static void notepage_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		notepage_event_button_on(widget);
	}

#ifdef  OLD_FOCUS_CHANGE
	/* DO NOTHING */
#else /* 新たなフォーカス移動方式 */
	{
		Q8tkWidget *w = NULL;

		if (key == Q8TK_KEY_LEFT) {
			if (widget->prev == NULL) {
				w = widget->next;
				if (w) {
					while (w->next) {
						w = w->next;
					}
				}
			} else {
				w = widget->prev;
			}
		}
		if (key == Q8TK_KEY_RIGHT) {
			if (widget->next == NULL) {
				w = widget->prev;
				if (w) {
					while (w->prev) {
						w = w->prev;
					}
				}
			} else {
				w = widget->next;
			}
		}

		if (w) {
			q8tk_widget_set_focus(w);
			notepage_event_button_on(w);
		}
	}
#endif
}

void q8tk_notebook_append(Q8tkWidget *notebook,
						  Q8tkWidget *widget, const char *label)
{
	Q8tkWidget *w;

	Q8tkAssert(notebook->type == Q8TK_TYPE_NOTEBOOK, NULL);

	w = malloc_widget();
	w->type = Q8TK_TYPE_NOTEPAGE;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->key_left_used  = TRUE;
	w->key_right_used = TRUE;

	w->name = (char *)malloc(strlen(label) + 1);

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, label);
	w->code = q8tk_kanji_code;

	if (notebook->child == NULL) {
		/* 最初の NOTE PAGE の場合は それを選択状態にする    */
		notebook->stat.notebook.page = w;
	}
	/* コンテナ処理は BOXと同じ */
	q8tk_box_pack_start(notebook, w);

	q8tk_container_add(w, widget);
	q8tk_widget_show(w);

	w->event_button_on = notepage_event_button_on;
	w->event_key_on    = notepage_event_key_on;
}

int q8tk_notebook_current_page(Q8tkWidget *notebook)
{
	Q8tkWidget *child = notebook->child;
	int        i = 0;

	while (child) {
		if (child == notebook->stat.notebook.page) {
			return i;
		}
		child = child->next;
		i++;
	}
	return -1;
}

void q8tk_notebook_set_page(Q8tkWidget *notebook, int page_num)
{
	Q8tkWidget *child = notebook->child;
	int        i = 0;

	while (child) {
		if (i == page_num) {
			if (notebook->stat.notebook.page != child) {
				q8tk_widget_set_focus(child);
				notepage_event_button_on(child);
			}
			break;
		} else {
			child = child->next;
			i++;
		}
	}
}

void q8tk_notebook_next_page(Q8tkWidget *notebook)
{
	if (notebook->child) {
		Q8tkWidget *page = notebook->stat.notebook.page;
		if (page && page->next) {
			q8tk_widget_set_focus(page->next);
			notepage_event_button_on(page->next);
		}
	}
}

void q8tk_notebook_prev_page(Q8tkWidget *notebook)
{
	if (notebook->child) {
		Q8tkWidget *page = notebook->stat.notebook.page;
		if (page && page->prev) {
			q8tk_widget_set_focus(page->prev);
			notepage_event_button_on(page->prev);
		}
	}
}

void q8tk_notebook_hook_focus_lost(Q8tkWidget *notebook,
								   int focus_lost)
{
	notebook->stat.notebook.lost_focus = focus_lost;
}
