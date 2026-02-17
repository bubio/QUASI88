/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"


/************************************************************************/
/* コンテナ関係                                                         */
/************************************************************************/
/*---------------------------------------------------------------------------
 * コンテナ操作
 *---------------------------------------------------------------------------
 * ・コンテナウィジットとウィジットを親子にする
 * --------------------------------------------------------------------------
 * void q8tk_container_add(Q8tkWidget *container, Q8tkWidget *widget)
 *    コンテナウィジット container と ウィジット widget を親子にする。
 *
 * void q8tk_box_pack_start(Q8tkWidget *box, Q8tkWidget *widget)
 *    水平・垂直ボックス box の後方に、ウィジット widget を詰める。
 *
 * void q8tk_box_pack_end(Q8tkWidget *box, Q8tkWidget *widget)
 *    水平・垂直ボックス box の前方に、ウィジット widget を詰める。
 *
 * void q8tk_container_remove(Q8tkWidget *container, Q8tkWidget *widget)
 *    コンテナウィジット container から ウィジット widget を切り離す。
 *---------------------------------------------------------------------------*/
void q8tk_container_add(Q8tkWidget *container, Q8tkWidget *widget)
{
	Q8tkAssert(container->attr & Q8TK_ATTR_CONTAINER, NULL);
	if (container->attr & Q8TK_ATTR_LABEL_CONTAINER) {
		Q8tkAssert(widget->type == Q8TK_TYPE_LABEL, NULL);
	}

	switch (container->type) {
	case Q8TK_TYPE_LISTBOX:
		/* LIST BOX 例外処理 */
		/* 最初の LIST ITEM の場合は、それを選択状態にする   */
		if (container->child == NULL) {
			container->stat.listbox.selected = widget;
			container->stat.listbox.active   = widget;
			container->stat.listbox.insight  = widget;
		}
		/* 処理はVBOXと同じ */
		q8tk_box_pack_start(container, widget);
		return;

	default:
		/* 通常の処理 */
		container->child = widget;
		widget->parent = container;
		widget->prev   = NULL;
		widget->next   = NULL;
		break;
	}

	if (widget->visible) {
		widget_map(widget);
	}
}

void q8tk_box_pack_start(Q8tkWidget *box, Q8tkWidget *widget)
{
	Q8tkAssert(box->attr & Q8TK_ATTR_CONTAINER, NULL);

	if (box->child == NULL) {
		box->child   = widget;
		widget->prev = NULL;
		widget->next = NULL;
	} else {
		Q8tkWidget *c = box->child;
		while (c->next) {
			c = c->next;
		}
		c->next      = widget;
		widget->prev = c;
		widget->next = NULL;
	}
	widget->parent = box;

	if (widget->visible) {
		widget_map(widget);
	}
}

void q8tk_box_pack_end(Q8tkWidget *box, Q8tkWidget *widget)
{
	Q8tkAssert(box->attr & Q8TK_ATTR_CONTAINER, NULL);

	if (box->child == NULL) {
		widget->prev = NULL;
		widget->next = NULL;
	} else {
		Q8tkWidget *c = box->child;
		Q8tkAssert(c->prev == NULL, NULL);
		c->prev      = widget;
		widget->next = c;
		widget->prev = NULL;
	}
	box->child     = widget;
	widget->parent = box;

	if (widget->visible) {
		widget_map(widget);
	}
}

void q8tk_container_remove(Q8tkWidget *container, Q8tkWidget *widget)
{
	Q8tkAssert(container->attr & Q8TK_ATTR_CONTAINER, NULL);
	Q8tkAssert(widget->parent == container, NULL);

	if (widget->prev == NULL) {
		/* 自分が親の直下の時 */

		Q8tkWidget *n = widget->next;
		if (n) {
			n->prev = NULL;
		}
		container->child = n;

	} else {
		/* そうじゃない時 */

		Q8tkWidget *p = widget->prev;
		Q8tkWidget *n = widget->next;
		if (n) {
			n->prev = p;
		}
		p->next = n;
	}

	widget->parent = NULL;
	widget->prev = NULL;
	widget->next = NULL;

	switch (container->type) {

	case Q8TK_TYPE_LISTBOX:
		/* LIST BOX 例外処理 */
		if (container->stat.listbox.selected == widget) {
			container->stat.listbox.selected = NULL;
			container->stat.listbox.active   = NULL;
			container->stat.listbox.insight  = NULL;
		}
		break;

	}


	if (container->visible) {
		widget_map(container);
	}
}
