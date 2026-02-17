/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "alarm.h"


static void itembar_size_update(Q8tkWidget *bar);

/*---------------------------------------------------------------------------
 * アイテムバー用ラベル (ITEM_LABEL)
 *---------------------------------------------------------------------------
 *・アイテムバー用の文字列を保持できる
 *・アイテムバーの子になる。
 *・表示・非表示は操作できない。親にしたアイテムバーに依存する。
 *  (最大文字列長を 0 にセットすると、実質非表示になる)
 *・シグナル … 無し
 *---------------------------------------------------------------------------
 * Q8tkWidget *q8tk_item_label_new(int width)
 *    最大文字列長 width のエリアをもつアイテムバー用ラベル表示エリアの生成。
 *    (生成時点で表示される文字列は、空白)
 *    文字コードはこの時点で固定される。
 *
 * void q8tk_item_label_set(Q8tkWidget *item, const char *label)
 *    文字列を表示エリアにセットする。label が最大文字列長に満たない場合は、
 *    空白で埋められる。
 *
 * void q8tk_item_label_set_cyclic(Q8tkWidget *item,
 *                                 const char *(*get_label)(void), int timeout)
 *    get_label() の戻り値文字列を表示エリアにセットする。
 *    文字列が最大文字列長に満たない場合は、空白で埋められる。
 *    timeout [ms] 経過すると、再度 get_label() を呼び出し、戻り値文字列を
 *    表示エリアにセットする。この動作は、q8tk_item_label_set が呼び出され
 *    るまで繰り返される。
 *
 * void q8tk_item_label_show_message(Q8tkWidget *item,
 *                                   const char *message, int timeout)
 *    一時的に、文字列を表示エリアにセットする。label が最大文字列長に満た
 *    ない場合は、空白で埋められる。
 *    timeout [ms] 経過すると、もとの文字列の表示に戻る。
 *
 * void q8tk_item_label_hide_message(Q8tkWidget *item)
 *    show_message で一時的にセットした文字列を、もとの文字列の表示に戻す。
 *    (timeout [ms] 経過した時と同じ状態にする)
 *
 * void q8tk_item_label_set_width(Q8tkWidget *item, int width)
 *    最大文字列長を width に変更する。この時、表示文字列は空白になる。
 * --------------------------------------------------------------------------
 * 【ITEM_LABEL】
 *         ｜
 *         └──【ITEM_LABEL】
 *
 * q8tk_item_label_show_message() を実現するためのウィジットを内部で生成する
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_item_label_new(int width)
{
	int i;
	Q8tkWidget *w[2];
	int malloc_size = width * 2 + 1;
	char *save = NULL;

	for (i = 0; i < 2; i++) {
		w[i] = malloc_widget();
		w[i]->type = Q8TK_TYPE_ITEM_LABEL;
		w[i]->name = (char *)malloc(malloc_size);
		w[i]->sensitive = TRUE;

		Q8tkAssert(w[i]->name, "memory exhoused");

		memset(w[i]->name, 0, malloc_size);

		w[i]->code = q8tk_kanji_code;

		w[i]->stat.label.foreground = Q8GR_PALETTE_FOREGROUND;
		w[i]->stat.label.background = Q8GR_PALETTE_BACKGROUND;
		w[i]->stat.label.reverse    = FALSE;

		w[i]->stat.label.width = width;
		w[i]->stat.label.name_size = malloc_size;
	}

	w[0]->stat.label.is_overlay = FALSE;
	w[0]->stat.label.overlay = w[1];
	w[1]->stat.label.overlay = NULL;

	return w[0];
}


static void item_label_set(Q8tkWidget *item, const char *label)
{
	size_t l;

	if (label == NULL) {
		label = "";
	}

	l = strlen(label) + 1;
	if (l > item->stat.label.name_size) {
		size_t sz = l * 2 + 1;
		item->name = (char *)realloc(item->name, sz);
		item->stat.label.name_size = sz;

		Q8tkAssert(item->name, "memory exhoused");
	}
	strcpy(item->name, label);
}

void q8tk_item_label_set(Q8tkWidget *item, const char *label)
{
	Q8tkWidget *overlay = item->stat.label.overlay;

	item_label_set(item, label);

	if (item->stat.label.is_overlay) {
		item->stat.label.is_overlay = FALSE;

		alarm_remove(overlay);
		overlay->visible = FALSE;
	}

	alarm_remove(item);
	item->visible = (item->stat.label.width > 0) ? TRUE : FALSE;

	set_construct_flag(TRUE);
}


static void cb_item_label_set_cyclic(Q8tkWidget *item, void *parm)
{
	q8tk_item_label_set_cyclic(item, parm, item->stat.label.timeout);
}

void q8tk_item_label_set_cyclic(Q8tkWidget *item,
								const char *(*get_label)(void),
								int timeout)
{
	if ((get_label) && (timeout > 0)) {
		item_label_set(item, (get_label)());

		/* タイムアウトの監視を開始 */
		alarm_add(item, (void *) get_label,
				  (alarm_callback_func) cb_item_label_set_cyclic,
				  timeout, FALSE);
		item->stat.label.timeout = timeout;

	} else {
		item_label_set(item, "");

		alarm_remove(item);
	}

	if (item->stat.label.is_overlay == FALSE) {
		set_construct_flag(TRUE);
	}
}


static void cb_item_label_show_message(UNUSED_WIDGET, void *item)
{
	q8tk_item_label_hide_message((Q8tkWidget *) item);
}

void q8tk_item_label_show_message(Q8tkWidget *item,
								  const char *message, int timeout)
{
	Q8tkWidget *overlay = item->stat.label.overlay;

	if (item->stat.label.is_overlay == FALSE) {
		item->stat.label.is_overlay = TRUE;
		item->visible = FALSE;

	} else {
		/* 現在メッセージ表示中なら、タイムアウトの監視を停止 */
		alarm_remove(overlay);
	}

	/* 実際に表示する文字列をセット */
	item_label_set(overlay, message);

	overlay->visible = (overlay->stat.label.width > 0) ? TRUE : FALSE;

	/* タイムアウトの監視を開始 */
	alarm_add(overlay, item,
			  (alarm_callback_func) cb_item_label_show_message,
			  timeout, FALSE);

	set_construct_flag(TRUE);
}

void q8tk_item_label_hide_message(Q8tkWidget *item)
{
	Q8tkWidget *overlay = item->stat.label.overlay;

	if (item->stat.label.is_overlay) {
		item->stat.label.is_overlay = FALSE;

		alarm_remove(overlay);
		overlay->visible = FALSE;

		item->visible = (item->stat.label.width > 0) ? TRUE : FALSE;

		set_construct_flag(TRUE);
	}
}

void q8tk_item_label_set_width(Q8tkWidget *item, int width)
{
	Q8tkWidget *bar = item->parent;

	q8tk_item_label_set(item, "");

	item->stat.label.width = width;
	item->stat.label.overlay->stat.label.width = width;

	item->visible = (width > 0) ? TRUE : FALSE;

	if (bar &&
		bar->type == Q8TK_TYPE_ITEMBAR) {
		itembar_size_update(bar);
		set_construct_flag(TRUE);
	}
}



/*---------------------------------------------------------------------------
 * アイテムバー用ビットマップ (ITEM_BMP)
 *---------------------------------------------------------------------------
 *・アイテムバー用のビットマップ画像を保持できる
 *・アイテムバーの子になる。
 *・表示・非表示は操作できない。親にしたアイテムバーに依存する。
 *・シグナル … 無し
 *---------------------------------------------------------------------------
 * Q8tkWidget *q8tk_item_bitmap_new(int x_size, int y_size)
 *    画像サイズ x_size * y_size のエリアをもつアイテムバー用ビットマップ
 *    表示エリアの生成。(生成時点で表示される画像は、空白)
 *
 * void q8tk_item_bitmap_set_bitmap(Q8tkWidget *item, Q8tkWidget *bitmap)
 *    ビットマップ画像を表示エリアにセットする。
 *    画像および色は、この時に bitmap から取り出し、表示エリアに反映する。
 *    (bitmap ウィジットの表示状態には影響を受けない)
 *     bitmap は q8tk_item_bitmap_clear() を呼び出すまでは削除してはならない。
 *
 * void q8tk_item_bitmap_clear(Q8tkWidget *item)
 *    ビットマップ表示エリアをクリアする。(空白画像となる)
 *    以降 q8tk_item_bitmap_set_bitmap() で指定した bitmap は削除してもよい。
 *
 * void q8tk_item_bitmap_set_color(Q8tkWidget *item,
 *                                 int foreground, int background);
 *    q8tk_item_bitmap_set_bitmap() を呼び出した後で、ビットマップの色を
 *    設定する。(bitmap ウィジットの色へは影響を与えない)
 * --------------------------------------------------------------------------
 * 【ITEM_BMP】    子は持てない
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_item_bitmap_new(int x_size, int y_size)
{
	Q8tkWidget *w;
	int sx = (x_size + 7) / 8;
	int sy = (y_size + 15) / 16;
	byte *dst;

	w = malloc_widget();
	w->type = Q8TK_TYPE_ITEM_BMP;
	w->sensitive = TRUE;

	w->stat.bmp.foreground = Q8GR_PALETTE_FOREGROUND;
	w->stat.bmp.background = Q8GR_PALETTE_BACKGROUND;
	w->stat.bmp.x_size = x_size;
	w->stat.bmp.y_size = y_size;
	w->stat.bmp.width = sx;
	w->stat.bmp.height = sy;

	dst = (byte *) calloc(1, sx * sy * 16);
	Q8tkAssert(sx * sy * 16 < BITMAP_DATA_MAX_SIZE, "bmp size too big");
	Q8tkAssert(dst != NULL, "memory exhoused");

	w->stat.bmp.alloc = widget_bitmap_table_add(dst);
	w->stat.bmp.index = w->stat.bmp.alloc;
	Q8tkAssert(w->stat.bmp.alloc >= 0, "memory exhoused");

	return w;
}

void q8tk_item_bitmap_set_bitmap(Q8tkWidget *item, Q8tkWidget *bitmap)
{
	Q8tkAssert((item->stat.bmp.width == bitmap->stat.bmp.width) &&
			   (item->stat.bmp.height == bitmap->stat.bmp.height),
			   NULL);

	item->stat.bmp.foreground = bitmap->stat.bmp.foreground;
	item->stat.bmp.background = bitmap->stat.bmp.background;
	item->stat.bmp.index = bitmap->stat.bmp.alloc;

	set_construct_flag(TRUE);
}

void q8tk_item_bitmap_clear(Q8tkWidget *item)
{
	item->stat.bmp.index = item->stat.bmp.alloc;

	set_construct_flag(TRUE);
}

void q8tk_item_bitmap_set_color(Q8tkWidget *item,
								int foreground, int background)
{
	q8tk_bitmap_set_color(item, foreground, background);
}




/*---------------------------------------------------------------------------
 * アイテムバー (ITEMBAR)
 *---------------------------------------------------------------------------
 * ・幅、高さが固定サイズの、水平ボックス
 * ・複数の子をもてる。ただし、以下のウィジットのみ
 *   ITEM_LABEL / ITEM_BMP / VSEPARATOR / BUTTON(BMP) / TOGGLE_BUTTON(BMP)
 * ・子ウィジットの高さは、アイテムバーと同じでなくてはならない
 *   (ITEM_LABEL / VSEPARATOR は除く)
 * ・子ウィジットがアイテムバーの幅からはみ出す場合、その子ウィジットは
 *   表示されない。
 * ・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_itembar_new(int width, int height)
 *    アイテムバーの生成。
 *
 * void q8tk_itembar_add(Q8tkWidget *bar, Q8tkWidget *item)
 *    アイテムバーの後方に、アイテムウィジット item を詰める。
 * --------------------------------------------------------------------------
 * 【ITEMBAR】 ←→ [xxxx]    いろんな子が持てる
 *                   ↑↓
 *             ←   [xxxx]
 *                   ↑↓
 *             ←   [xxxx]
 *                   ↑↓
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_itembar_new(int width, int height)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_ITEMBAR;
	/* w->attr = Q8TK_ATTR_CONTAINER; 実質コンテナだが、どうする? */
	w->sensitive = TRUE;
	w->stat.itembar.used_size = 0;
	w->stat.itembar.max_width = width;
	w->stat.itembar.max_height = height;

	return w;
}

static void itembar_size_update(Q8tkWidget *bar)
{
	int used_size = 0;
	Q8tkWidget *c = bar->child;

	while (c) {
		int size = 0;

		switch (c->type) {
		case Q8TK_TYPE_ITEM_LABEL:
			if (c->stat.label.overlay) {
				size = c->stat.label.width;
			}
			break;
		case Q8TK_TYPE_ITEM_BMP:
			size = c->stat.bmp.width;
			break;
		case Q8TK_TYPE_BUTTON:
		case Q8TK_TYPE_TOGGLE_BUTTON:
			size = (c->stat.button.release)->stat.bmp.width;
			break;
		case Q8TK_TYPE_VSEPARATOR:
			size = 1;
			break;
		default:
			break;
		}

		if (size) {
			if (used_size + size <= bar->stat.itembar.max_width) {
				/* アイテムバーに入るサイズなので、表示する */
				c->visible = TRUE;
				used_size += size;

				if (bar->visible) {
					widget_map(bar);
				}

			} else {
				/* アイテムバーに入りきらないサイズなので、隠す */
				c->visible = FALSE;
			}
		}

		c = c->next;
	}

	bar->stat.itembar.used_size = used_size;
}

void q8tk_itembar_add(Q8tkWidget *bar, Q8tkWidget *item)
{
	int size = 0;

	Q8tkAssert(bar->type == Q8TK_TYPE_ITEMBAR, NULL);

	if (bar->child == NULL) {
		bar->child = item;
		item->prev = NULL;
		item->next = NULL;
	} else {
		Q8tkWidget *c = bar->child;
		while (c->next) {
			c = c->next;
		}
		c->next    = item;
		item->prev = c;
		item->next = NULL;
	}
	item->parent = bar;

	if (item->type == Q8TK_TYPE_ITEM_LABEL) {
		Q8tkWidget *overlay = item->stat.label.overlay;
		Q8tkWidget *c = bar->child;
		while (c->next) {
			c = c->next;
		}
		c->next    = overlay;
		overlay->prev = c;
		overlay->next = NULL;
		overlay->parent = bar;
	}

	switch (item->type) {
	case Q8TK_TYPE_ITEM_LABEL:
		q8tk_misc_set_placement(item,
								Q8TK_PLACEMENT_X_LEFT,
								Q8TK_PLACEMENT_Y_CENTER);
		break;
	case Q8TK_TYPE_ITEM_BMP:
		Q8tkAssert(item->stat.bmp.height <= bar->stat.itembar.max_height, NULL);
		break;
	case Q8TK_TYPE_BUTTON:
	case Q8TK_TYPE_TOGGLE_BUTTON:
		Q8tkAssert(item->stat.button.from_bitmap, NULL);
		Q8tkAssert((item->stat.button.release)->stat.bmp.height <= bar->stat.itembar.max_height, NULL);
		break;
	case Q8TK_TYPE_VSEPARATOR:
		break;
	default:
		Q8tkAssert("q8tk_itembar_add", NULL);
		break;
	}

	itembar_size_update(bar);
}



/*---------------------------------------------------------------------------
 * ステータスバー
 *---------------------------------------------------------------------------
 * ・幅が 80文字分、高さが 1文字分のアイテムバー
 *---------------------------------------------------------------------------*/
#define	STATUSBAR_WIDTH		(80)
#define	STATUSBAR_HEIGHT	(1)

Q8tkWidget *q8tk_statusbar_new(void)
{
	return q8tk_itembar_new(STATUSBAR_WIDTH, STATUSBAR_HEIGHT);
}

void q8tk_statusbar_add(Q8tkWidget *bar, Q8tkWidget *item)
{
	q8tk_itembar_add(bar, item);
}



/*---------------------------------------------------------------------------
 * ツールバー
 *---------------------------------------------------------------------------
 *・幅が 80文字分、、高さが 4文字分のアイテムバー
 *---------------------------------------------------------------------------*/
#define	TOOLBAR_WIDTH		(80)
#define	TOOLBAR_HEIGHT		(4)

Q8tkWidget *q8tk_toolbar_new(void)
{
	return q8tk_itembar_new(TOOLBAR_WIDTH, TOOLBAR_HEIGHT);
}
void q8tk_toolbar_add(Q8tkWidget *bar, Q8tkWidget *item)
{
	q8tk_itembar_add(bar, item);
}
