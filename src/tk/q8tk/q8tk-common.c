/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/***********************************************************************
 * デバッグ・エラー処理
 ************************************************************************/

void _Q8tkAssert(char *file, int line, char *exp, const char *s)
{
	fprintf(stderr, "Fatal Error | %s <%s:%d>\n", exp, file, line);
	fprintf(stderr, "message = %s\n", (s) ? s : "---");
	quasi88_exit(-1);
}

const char *debug_type(int type)
{
	switch (type) {
	case Q8TK_TYPE_WINDOW:				return "window  :";
	case Q8TK_TYPE_BUTTON:				return "button  :";
	case Q8TK_TYPE_TOGGLE_BUTTON:		return "t-button:";
	case Q8TK_TYPE_CHECK_BUTTON:		return "c-button:";
	case Q8TK_TYPE_RADIO_BUTTON:		return "r-button:";
	case Q8TK_TYPE_RADIOPUSH_BUTTON:	return "rbush-b :";
	case Q8TK_TYPE_SWITCH:				return "switch  :";
	case Q8TK_TYPE_FRAME:				return "frame   :";
	case Q8TK_TYPE_LABEL:				return "label   :";
	case Q8TK_TYPE_BMP:					return "bmp     :";
	case Q8TK_TYPE_ITEM_LABEL:			return "it-label:";
	case Q8TK_TYPE_ITEM_BMP:			return "it-bmp  :";
	case Q8TK_TYPE_NOTEBOOK:			return "notebook:";
	case Q8TK_TYPE_NOTEPAGE:			return "page    :";
	case Q8TK_TYPE_VBOX:				return "vbox    :";
	case Q8TK_TYPE_HBOX:				return "hbox    :";
	case Q8TK_TYPE_ITEMBAR:				return "itembar :";
	case Q8TK_TYPE_VSEPARATOR:			return "vsep    :";
	case Q8TK_TYPE_HSEPARATOR:			return "hsep    :";
	case Q8TK_TYPE_COMBO:				return "combo   :";
	case Q8TK_TYPE_LISTBOX:				return "listbox :";
	case Q8TK_TYPE_LIST_ITEM:			return "listitem:";
	case Q8TK_TYPE_ADJUSTMENT:			return "adjust  :";
	case Q8TK_TYPE_HSCALE:				return "hscale  :";
	case Q8TK_TYPE_VSCALE:				return "vscale  :";
	case Q8TK_TYPE_SCROLLED_WINDOW:		return "scrolled:";
	case Q8TK_TYPE_ENTRY:				return "entry   :";
	case Q8TK_TYPE_DIALOG:				return "dialog  :";
	case Q8TK_TYPE_FILE_SELECTION:		return "f-select:";
	}
	return "UNDEF TYPE:";
}



/***********************************************************************
 * ワーク
 ************************************************************************/

int				MAX_WIDGET;
Q8tkWidget		**widget_table;


int				MAX_LIST;
Q8List			**list_table;


int				window_occupy;

Q8tkWidget		*window_layer[ TOTAL_WINDOW_LAYER ];
int				window_layer_level;
Q8tkWidget		*focus_widget[ TOTAL_WINDOW_LAYER ];



t_q8tk_mouse	q8tk_mouse;


int				q8tk_kanji_code = Q8TK_KANJI_ANK;
int				q8tk_disp_cursor = FALSE;

int				q8tk_now_shift_on;

/*----------------------------------------------------------------------*/

int				q8tk_construct_flag;


Q8tkWidget		*q8tk_drag_widget;




/***********************************************************************
 * 動的ワークの確保／開放
 ************************************************************************/
/*--------------------------------------------------------------
 * Widget
 *--------------------------------------------------------------*/
Q8tkWidget *malloc_widget(void)
{
	int i;
	Q8tkWidget  **t, *w;
	int add;

	t = NULL;
	for (i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i] == NULL) {
			t = &widget_table[i];
			break;
		}
	}

	if (t == NULL) {
		add = 1024;
		if (MAX_WIDGET == 0) {
			widget_table = malloc((MAX_WIDGET + add) * sizeof(Q8tkWidget *));
		} else {
			widget_table = realloc(widget_table,
								   (MAX_WIDGET + add) * sizeof(Q8tkWidget *));
		}
		if (widget_table) {
			for (i = 0; i < add; i++) {
				widget_table[ MAX_WIDGET + i ] = NULL;
			}
			t = &widget_table[ MAX_WIDGET ];
			MAX_WIDGET += add;
		}
	}

	if (t) {
		w = (Q8tkWidget *)calloc(1, sizeof(Q8tkWidget));
		if (w) {
			w->occupy = window_occupy;
			*t = w;
			return w;
		} else {
			Q8tkAssert(FALSE, "memory exhoused");
		}
	}

	Q8tkAssert(FALSE, "work 'widget' exhoused");
	return NULL;
}
void free_widget(Q8tkWidget *w)
{
	int i;
	for (i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i] == w) {
			free(w);
			widget_table[i] = NULL;
			return;
		}
	}
	Q8tkAssert(FALSE, "pointer is not malloced widget");
}
/*--------------------------------------------------------------
 * List
 *--------------------------------------------------------------*/
Q8List *malloc_list(void)
{
	int i;
	Q8List **t, *l;
	int add;

	t = NULL;
	for (i = 0; i < MAX_LIST; i++) {
		if (list_table[i] == NULL) {
			t = &list_table[i];
			break;
		}
	}

	if (t == NULL) {
		add = 512;
		if (MAX_LIST == 0) {
			list_table = malloc((MAX_LIST + add) * sizeof(Q8List *));
		} else {
			list_table = realloc(list_table,
								 (MAX_LIST + add) * sizeof(Q8List *));
		}
		if (list_table) {
			for (i = 0; i < add; i++) {
				list_table[ MAX_LIST + i ] = NULL;
			}
			t = &list_table[ MAX_LIST ];
			MAX_LIST += add;
		}
	}

	if (t) {
		l = (Q8List *)calloc(1, sizeof(Q8List));
		if (l) {
			l->occupy = window_occupy;
			*t = l;
			return l;
		} else {
			Q8tkAssert(FALSE, "memory exhoused");
		}
	}

	Q8tkAssert(FALSE, "work 'list' exhoused");
	return NULL;
}
void free_list(Q8List *l)
{
	int i;
	for (i = 0; i < MAX_LIST; i++) {
		if (list_table[i] == l) {
			free(l);
			list_table[i] = NULL;
			return;
		}
	}
	Q8tkAssert(FALSE, "pointer is not malloced list");
}



#if 0
/***********************************************************************
 * Q8TK 専用 文字列処理
 *
 * q8_strncpy(s, ct, n)
 *    文字列 ct を 文字列 s に コピーする。
 *    s の文字列終端は、必ず '\0' となり、s の長さは n-1 文字
 *    以下に収まる。
 * q8_strncat(s, ct, n)
 *    文字列 ct を 文字列 s に 付加する。
 *    s の文字列終端は、必ず '\0' となり、s の長さは n-1 文字
 *    以下に収まる。
 ************************************************************************/
void q8_strncpy(char *s, const char *ct, size_t n)
{
	strncpy(s, ct, n - 1);
	s[ n - 1 ] = '\0';
}
void q8_strncat(char *s, const char *ct, size_t n)
{
	if (n > strlen(s) + 1) {
		strncat(s, ct, n - strlen(s) - 1);
	}
}
#endif


/***********************************************************************
 * リスト処理
 ************************************************************************/
/*--------------------------------------------------------------
 * リストの末尾にあたらしい要素を追加 (またはリストを新規作成)
 *    戻り値は、リストの先頭
 *--------------------------------------------------------------*/
Q8List *q8_list_append(Q8List *list, void *data)
{
	Q8List *new_list;

	new_list  = malloc_list();

	if (list) {
		/* 既存のリストにつなげる場合 末尾とリンクする */
		list = q8_list_last(list);
		list->next     = new_list;
		new_list->prev = list;
		new_list->next = NULL;
	} else {
		/* 新規のリストの場合 */
		new_list->prev = NULL;
		new_list->next = NULL;
	}
	new_list->data   = data;

	/* 処理後のリスト先頭を返す */
	return q8_list_first(new_list);
}
/*--------------------------------------------------------------
 * リストの途中にあたらしい要素を追加 (position==0で先頭)
 *    戻り値は、リストの先頭
 *--------------------------------------------------------------*/
#if 0
Q8List *q8_list_insert(Q8List *list, void *data, int position)
{
	/* 未作成 */
	return NULL;
}
#endif
/*--------------------------------------------------------------
 * リストの要素をひとつだけ削除
 *--------------------------------------------------------------*/
void q8_list_remove(Q8List *list, void *data)
{
	list = q8_list_first(list);

	while (list) {
		/* リスト先頭から順にたどって data の一致するものを探す */

		if (list->data == data) {
			/* みつかればリンクをつなぎ変えて自身は削除 */

			if (list->prev) {
				list->prev->next = list->next;
			}
			if (list->next) {
				list->next->prev = list->prev;
			}
			free_list(list);
			break;
		}

		list = list->next;
	}
}
/*--------------------------------------------------------------
 * リストの消去
 *--------------------------------------------------------------*/
void q8_list_free(Q8List *list)
{
	Q8List *l;

	/* リストの先頭から */
	list = q8_list_first(list);

	/* たどりながら解放 */
	while (list) {
		l = list->next;
		free_list(list);
		list = l;
	}
}
/*--------------------------------------------------------------
 * リスト先頭へ
 *--------------------------------------------------------------*/
Q8List *q8_list_first(Q8List *list)
{
	if (list) {
		/* リストを前にたどる */
		while (list->prev) {
			list = list->prev;
		}
		/* たどれなくなったら それが先頭 */
	}
	return list;
}
/*--------------------------------------------------------------
 * リスト末尾へ
 *--------------------------------------------------------------*/
Q8List *q8_list_last(Q8List *list)
{
	if (list) {
		/* リストを後にたどる */
		while (list->next) {
			list = list->next;
		}
		/* たどれなくなったらそれが末尾 */
	}
	return list;
}
/*--------------------------------------------------------------
 * リスト検索
 *--------------------------------------------------------------*/
Q8List *q8_list_find(Q8List *list, void *data)
{
	/* リストの先頭から */
	list = q8_list_first(list);

	/* たどりながら 一致するものを探す */
	while (list) {
		if (list->data == data) {
			break;
		}
		list = list->next;
	}
	return list;
}


/***********************************************************************
 * シグナル関係
 ************************************************************************/
/*
 * 任意のウィジットに、任意のシグナルを送る
 */
void widget_signal_do(Q8tkWidget *widget, const char *name)
{
	switch (widget->type) {

	case Q8TK_TYPE_WINDOW:
		if (strcmp(name, "inactivate") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_BUTTON:
		if (strcmp(name, "clicked") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_TOGGLE_BUTTON:
	case Q8TK_TYPE_CHECK_BUTTON:
	case Q8TK_TYPE_RADIO_BUTTON:
	case Q8TK_TYPE_RADIOPUSH_BUTTON:
	case Q8TK_TYPE_SWITCH:
		if (strcmp(name, "clicked") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		} else if (strcmp(name, "toggled") == 0) {
			if (widget->user_event_1) {
				(*widget->user_event_1)(widget, widget->user_event_1_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_NOTEBOOK:
		if (strcmp(name, "switch_page") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_COMBO:
		if (strcmp(name, "activate") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		} else if (strcmp(name, "changed") == 0) {
			if (widget->user_event_1) {
				(*widget->user_event_1)(widget, widget->user_event_1_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_LISTBOX:
		if (strcmp(name, "selection_changed") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_LIST_ITEM:
		if (strcmp(name, "select") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_ADJUSTMENT:
		if (strcmp(name, "value_changed") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		}
		break;

	case Q8TK_TYPE_ENTRY:
		if (strcmp(name, "activate") == 0) {
			if (widget->user_event_0) {
				(*widget->user_event_0)(widget, widget->user_event_0_parm);
			}
			return;
		} else if (strcmp(name, "changed") == 0) {
			if (widget->user_event_1) {
				(*widget->user_event_1)(widget, widget->user_event_1_parm);
			}
			return;
		}
		break;

	}

	fprintf(stderr, "BAD signal %s '%s'\n", debug_type(widget->type), name);
	Q8tkAssert(FALSE, NULL);
	return;
}


/*--------------------------------------------------------------
 * ウィジットの表示設定
 *    親ウィジットをつぎつぎチェックし、
 *    ・全ての親ウィジットが可視状態
 *    ・先祖ウィジットが WINDOW
 *    の場合、その WINDOW 以下を再計算して表示する
 *    ように、フラグをたてる。
 *    実際の再計算、表示は、q8tk_main() で行なう。
 *--------------------------------------------------------------*/
void widget_map(Q8tkWidget *widget)
{
	Q8tkWidget *ancestor, *parent;
	int size_calc = TRUE;

	if (widget->visible) {
		ancestor = widget;
		parent   = widget->parent;
		while (parent) {
			if (parent->visible) {
				ancestor = parent;
				parent   = parent->parent;
			} else {
				size_calc = FALSE;
				break;
			}
		}
		if (size_calc && ancestor->type == Q8TK_TYPE_WINDOW) {
			set_construct_flag(TRUE);
		}
	}
}


/*--------------------------------------------------------------
 * ウィジットが、スクロールドウインドウに含まれる場合、
 * スクロール位置によっては、表示されないことがある。
 *
 * 予め、このウィジットを以下の関数で登録しておけば、
 *
 *    widget_scrollin_register(this_widget);
 *
 * 画面表示の際に、そのウィジットが表示されるように、スクロール
 * 位置を調整するようにする。
 *
 * スクロールドウインドウにあるウィジットが表示されたかどうかは、
 * 実際に表示するまでわからないので、実際に表示しながらワークを
 * 設定し、ワークの内容次第で、再表示ということになる。
 *
 * つまり、以下のような感じになる。
 *
 *    widget_scrollin_adjust_reset();   … 処理の初期化
 *    widget_draw();                    … 描画
 *
 *    if (widget_scrollin_adjust()) {   … 調整したら真
 *        widget_draw();                … 調整したので、再描画
 *    }
 *
 * なお、処理を行なうのは、最初に見つかった 親の SCROLLED WINDOW のみ
 * に対してだけなので、SCROLLED WINDOW の入れ子の場合はどうなるか
 * わからない。
 *--------------------------------------------------------------*/

/* 調整可能なウィジットの数。これだけあれば大丈夫だろう */
#define MAX_WIDGET_SCROLLIN		(8)
static struct {
	int         drawn;
	Q8tkWidget  *widget;
} widget_scrollin[ MAX_WIDGET_SCROLLIN ];

/* 全ワークの初期化 (一度だけ行う) */
void widget_scrollin_init(void)
{
	int i;
	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
		widget_scrollin[i].drawn  = FALSE;
		widget_scrollin[i].widget = NULL;
	}
}
/* ウィジットの登録 */
void widget_scrollin_register(Q8tkWidget *w)
{
	int i;
	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
		if (widget_scrollin[i].widget == w) {
			return;
		}
	}
	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
		if (widget_scrollin[i].widget == NULL) {
			widget_scrollin[i].drawn  = FALSE;
			widget_scrollin[i].widget = w;
			return;
		}
	}
}
/* 初期化：ウィジットの描画前に呼び出す */
void widget_scrollin_adjust_reset(void)
{
	int i;
	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
		widget_scrollin[i].drawn = FALSE;
	}
}
/* 設定：各ウィジットの描画毎に呼び出す */
void widget_scrollin_drawn(Q8tkWidget *w)
{
	int i;
	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
		if (widget_scrollin[i].widget == w) {
			widget_scrollin[i].drawn = TRUE;
			return;
		}
	}
}
/* 調整：全ウィジットの描画後に呼び出す */
int widget_scrollin_adjust(void)
{
	int i, result = 0;

	for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {

		/*printf("%d %d %s\n",i,widget_scrollin[i].drawn,(widget_scrollin[i].widget)?debug_type(widget_scrollin[i].widget->type):"NULL");*/

		if (widget_scrollin[i].widget) {

			if (widget_scrollin[i].drawn) {

				Q8tkWidget *widget = widget_scrollin[i].widget;
				Q8tkWidget *p = widget->parent;
				/*int	x1 = widget->x;*/
				int		x1 = widget->x + widget->sx - 1;
				int		y1 = widget->y + widget->sy - 1;

				/*
				 SCROLLED WINDOW 内に乗せるウィジット全体を、左上を原点とした、
				 相対位置 real_y

				 y1 = widget->y + widget->sy -1;
				 p->y + 1 - p->stat.scrolled.child_y0 + [ real_y ] = y1;
				*/

				while (p) {
					if (p->type == Q8TK_TYPE_SCROLLED_WINDOW) {

						int d, topleft;

						/* d > 0 なら、左端表示 */
						/* d < 0 なら、右端表示 */
						if (widget->type == Q8TK_TYPE_LIST_ITEM &&
							widget->parent->type == Q8TK_TYPE_LISTBOX) {
							d = widget->parent->stat.listbox.scrollin_left;
						} else {
							d = 0;
						}
						/* d == 0 は、デフォルト (-1 とする) */
						if (d == 0) {
							d = -1;
						}

						if (d > 0) {
							topleft = TRUE;
							d -= 1;
						} else       {
							topleft = FALSE;
							d += 1;
						}

						if (topleft) {
							/* 左端が表示されるように位置補正 */
							/* d = 0, 1, 2, 3... */
							/* 補正しなければ左端が表示されるので、このまま */

						} else {
							/* 右端が表示されるように位置補正 */
							/* d = 0, -1, -2, -3... */
							if (p->x + 1 <= x1 &&
								x1 < p->x + 1 + p->stat.scrolled.child_sx) {
								/* Ok, Expose */
							} else {
								Q8TK_ADJUSTMENT(p->stat.scrolled.hadj)->value
									= x1 - (p->x + 1 - p->stat.scrolled.child_x0)
									  - p->stat.scrolled.child_sx + 1;
								result = 1;
							}
						}
						/* ↑ d の値によって補正量を変えたいが、また今度に */


						/* d > 0 なら、上端表示。+1の都度、その1文字下を表示 */
						/* d < 0 なら、下端表示。-1の都度、その1文字上を表示 */
						if (widget->type == Q8TK_TYPE_LIST_ITEM &&
							widget->parent->type == Q8TK_TYPE_LISTBOX) {
							d = widget->parent->stat.listbox.scrollin_top;
						} else {
							d = 0;
						}
						/* d == 0 は、デフォルト (-1 とする) */
						if (d == 0) {
							d = -1;
						}

						if (d > 0) {
							topleft = TRUE;
							d -= 1;
						} else       {
							topleft = FALSE;
							d += 1;
						}

						if (topleft) {
							/* 上端が表示されるように位置補正 */
							/* d = 0, 1, 2, 3... */
							if (p->y + 1 == (y1 - d)) {
								/* Ok */
							} else {
								Q8TK_ADJUSTMENT(p->stat.scrolled.vadj)->value =
									(y1 - d) - (p->y + 1 - p->stat.scrolled.child_y0);
								result = 1;
							}

						} else {
							/* 下端が表示されるように位置補正 */
							/* d = 0, -1, -2, -3... */
							if (p->y + 1 <= (y1 - d) &&
								(y1 - d) < p->y + 1 + p->stat.scrolled.child_sy) {
								/* Ok, Expose */
							} else {
								Q8TK_ADJUSTMENT(p->stat.scrolled.vadj)->value =
									(y1 - d) - (p->y + 1 - p->stat.scrolled.child_y0)
									- p->stat.scrolled.child_sy + 1;
								result = 1;
							}
						}

						/*
						printf("  %s %d %d %d %d  %d %d %d %d\n",debug_type(p->type),
						       p->x, p->y, p->sx, p->sy,
						       p->stat.scrolled.child_x0, p->stat.scrolled.child_y0,
						       p->stat.scrolled.child_sx, p->stat.scrolled.child_sy);
						printf("  %s %d %d %d %d\n",debug_type(widget->type),
						       widget->x, widget->y, widget->sx, widget->sy);
						printf("  %d\n",y1);
						if (result) printf("ADJ %d,%d\n",
						                   Q8TK_ADJUSTMENT(p->stat.scrolled.vadj)->value,
						                   Q8TK_ADJUSTMENT(p->stat.scrolled.hadj)->value);
						*/


						break;
					}
					p = p->parent;
				}
				widget_scrollin[i].drawn  = FALSE;
				widget_scrollin[i].widget = NULL;
			}
		}
	}

	return result;
}


/*--------------------------------------------------------------
 * ウィジットのフォーカス移動のリスト作成
 *
 * TABキーや、カーソルキーによる、フォーカスの移動を実現するために、
 * 対象となるウィジットのリストを生成する。
 *
 *    widget_focus_list_reset()  … リストの初期化
 *    widget_draw()              … 描画。内部でリストを生成する
 *
 * widget_draw() の内部では、フォーカス移動の対象としたいウィジットの
 * 描画の際に、『widget_focus_list_append()』 関数を呼ぶ。
 *
 * TABキーなどを押されたときに、次にフォーカスの移るウィジットを取得するには
 * 『widget_focus_list_get_next()』関数を呼ぶ。
 *--------------------------------------------------------------*/
/* BACKTAB 対応  thanks! floi */

static	Q8List			*widget_focus_list;

/* 全ワークの初期化 (一度だけ行う) */
void widget_focus_list_init(void)
{
	widget_focus_list = NULL;
}
/* 初期化：ウィジットの描画前に呼び出す */
void widget_focus_list_reset(void)
{
	q8_list_free(widget_focus_list);
	widget_focus_list = NULL;
}
/* 設定：フォーカスリストに登録するウィジットなら、呼び出す */
void widget_focus_list_append(Q8tkWidget *widget)
{
	if (widget) {
		widget_focus_list = q8_list_append(widget_focus_list, widget);
	}
}
/* 取得：フォーカスリストの先頭ウィジットを取得 */
Q8tkWidget *widget_focus_list_get_top(void)
{
	if (widget_focus_list) {
		return (Q8tkWidget *)(widget_focus_list->data);
	} else {
		return NULL;
	}
}
/* 取得：フォーカスリストの次(前)のウィジットを取得 */
Q8tkWidget *widget_focus_list_get_next(Q8tkWidget *widget, int back)
{
	Q8List *list;

	if (widget == NULL) {
		return widget_focus_list_get_top();
	}

#if defined(OLD_FOCUS_CHANGE)
	list = q8_list_find(widget_focus_list, widget);

#else   /* 新たなフォーカス移動方式 */
	switch (widget->type) {
	case Q8TK_TYPE_RADIO_BUTTON:
	case Q8TK_TYPE_RADIOPUSH_BUTTON:
		;{
			/* オンの RADIO BUTTON を探して、その前後へ */
			Q8List *l = widget->stat.button.list;
			while (l) {
				widget = (Q8tkWidget *)(l->data);
				if (widget->stat.button.active == Q8TK_BUTTON_ON) {
					break;
				}
				l = l->next;
			}
		}
		list = q8_list_find(widget_focus_list, widget);
		break;

	case Q8TK_TYPE_NOTEPAGE:
		/* 今のところ、フォーカスのあるNOTEPAGEは常に選択状態に
		 * なるので、常にTAB付けされる。つまり、ここにはこない。 */
		widget = (widget->parent)->stat.notebook.page;
		list = q8_list_find(widget_focus_list, widget);
		break;

	case Q8TK_TYPE_COMBO:
		if (widget->stat.combo.entry->stat.entry.editable == FALSE) {
			/* ENTRY が編集不可の場合は、COMBO 自身の前後へ */
			list = q8_list_find(widget_focus_list, widget);
		} else {
			/* ENTRY が編集可の場合は */
			if (back == FALSE) {
				/* COMBO の次は、ENTRY の次に同じ */
				list = q8_list_find(widget_focus_list,
									widget->stat.combo.entry);
			} else {
				/* COMBO の前は、ENTRY そのものにしてしまおう */
				return widget->stat.combo.entry;
			}
		}
		break;

	default:
		list = q8_list_find(widget_focus_list, widget);
		break;
	}
#endif

	if (list) {
		if (back == FALSE) {
			if (list->next) {
				return (Q8tkWidget *)((list->next)->data);
			} else {
				return (Q8tkWidget *)((q8_list_first(list))->data);
			}
		} else {
			if (list->prev) {
				return (Q8tkWidget *)((list->prev)->data);
			} else {
				return (Q8tkWidget *)((q8_list_last(list))->data);
			}
		}
	}
	return NULL;
}



/*--------------------------------------------------------------
 * ビットマップ
 *
 * ビットマップウィジット用画像管理
 * (表示方法の兼ね合いで、最大で BITMAP_TABLE_SIZE 個しか
 * ビットマップ画像を管理できない。BMPウィジットの最大生成数も同様。
 * 足りなくなるようなら、根本的に考え直さねば・・・)
 *
 *--------------------------------------------------------------*/

void widget_bitmap_table_init(void)
{
	int i;
	for (i = 0; i < BITMAP_TABLE_SIZE ; i++) {
		bitmap_table[i] = NULL;
	}
}

void widget_bitmap_table_term(void)
{
	int i;
	for (i = 0; i < BITMAP_TABLE_SIZE ; i++) {
		if (bitmap_table[i]) {
			free(bitmap_table[i]);
			bitmap_table[i] = NULL;
		}
	}
}

int widget_bitmap_table_add(byte *addr)
{
	int i;
	for (i = 0; i < BITMAP_TABLE_SIZE ; i++) {
		if (bitmap_table[i] == NULL) {
			bitmap_table[i] = addr;
			return i;
		}
	}
	return -1;
}

void widget_bitmap_table_remove(int index)
{
	if (bitmap_table[index]) {
		free(bitmap_table[index]);
		bitmap_table[index] = NULL;
	}
}
