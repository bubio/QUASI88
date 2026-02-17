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


/*---------------------------------------------------------------------------
 * エントリー (ENTRY)
 *---------------------------------------------------------------------------
 * ・文字の入力が可能。但し、文字は ASCIIのみ。表示・削除は漢字も可
 * ・子は持てない。
 * ・シグナル
 *   "activate"    リターンキー入力があった時に発生
 *   "changed"     文字入力、文字削除があった時に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget  *q8tk_entry_new(void)
 *     エントリを生成。入力可能な文字数は無制限。
 *
 * Q8tkWidget *q8tk_entry_new_with_max_length(int max)
 *    エントリを生成。入力可能な文字数は、max 文字まで。(max==0で無制限)
 *
 * void q8tk_entry_set_text(Q8tkWidget *entry, const char *text)
 *    エントリに、文字列 text を設定する。シグナルは発生しない。
 *    カーソルは非表示となり、表示位置は文字列先頭となる。
 *    ただし、ウィジットがアクティブかつ編集可能な場合、カーソルは文字列末端
 *    に移動し、カーソルが表示されるように表示位置が変化する。
 *
 * void q8tk_entry_set_position(Q8tkWidget *entry, int position)
 *    エントリ内部の文字列のカーソル位置を position に設定する。
 *    position == -1 の場合は、カーソルは表示されない。
 *    position が文字列末尾より後ろの場合、カーソルは文字列末尾に表示される。
 *    カーソル表示する場合、カーソルが表示されるように表示位置が変化する。
 *
 * void q8tk_entry_set_max_length(Q8tkWidget *entry, int max)
 *    エントリに入力可能な文字数を max に変更する。(max==0で無制限)
 *
 * void q8tk_entry_set_editable(Q8tkWidget *entry, int editable)
 *    エントリ領域の入力可・不可を設定する。 editable が 真なら入力可。
 *
 * void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
 *    エントリの表示サイズ width を指定する。 height は無効。
 *    カーソル表示する場合、カーソルが表示されるように表示位置が変化する。
 * --------------------------------------------------------------------------
 * 【ENTRY】    子は持てない
 *
 *---------------------------------------------------------------------------*/
/* strlen()の結果が strsize であるような文字列をセット可能なメモリを確保 */
static void q8tk_entry_malloc(Q8tkWidget *entry, int strsize)
{
#if 0
	/* strsize より 1バイト余分に確保 ('\0' の分を余分に) */
	int malloc_size = strsize + 1;
#else
	/* 1バイト単位でなく、512バイト単位で確保するようにしてみよう */
	int malloc_size = (((strsize + 1) / 512) + 1) * 512;
#endif

	if (entry->name == NULL ||
		entry->stat.entry.malloc_length < malloc_size) {
		if (entry->name == NULL) {
			entry->name = (char *)malloc(malloc_size);
		} else {
			entry->name = (char *)realloc(entry->name, malloc_size);
		}

		Q8tkAssert(entry->name, "memory exhoused");

		entry->stat.entry.malloc_length = malloc_size;
	}
}

/* エントリをマウスクリックしたら、カーソル移動 (編集可能時のみ) */
static void entry_event_button_on(Q8tkWidget *widget)
{
	int m_x = q8tk_mouse.x / 8;

	if (widget->stat.entry.editable == FALSE) {
		return;
	}

	if (widget->stat.entry.cursor_pos < 0) {
		/* カーソル非表示なら 文字列末尾にカーソル移動 */
		q8tk_entry_set_position(widget,
								q8gr_strlen(widget->code, widget->name));

	} else {
		/* カーソル表示ありなら マウス位置にカーソル移動 */
		q8tk_entry_set_position(widget,
								m_x - widget->x + widget->stat.entry.disp_pos);
	}
	set_construct_flag(TRUE);
}

/* ←→↑↓キーを押したら、カーソル移動           (編集可能時のみ) */
/* 文字キーを押したら、文字列編集し、シグナル発生 (編集可能時のみ) */
/* リターンを押したら、シグナル発生               (編集可能時のみ) */
static void entry_event_key_on(Q8tkWidget *widget, int key)
{
	if (widget->stat.entry.editable == FALSE) {
		return;
	}
	if (widget->stat.entry.cursor_pos < 0) {
		return;
	}

	switch (key) {
	case Q8TK_KEY_LEFT:
		if (widget->stat.entry.cursor_pos) {
			q8tk_entry_set_position(widget, widget->stat.entry.cursor_pos - 1);
		}
		break;

	case Q8TK_KEY_RIGHT: {
		int w = 1;
		if (q8gr_strchk(widget->code, widget->name,
						widget->stat.entry.cursor_pos) == 1) {
			w = 2;
		}
		q8tk_entry_set_position(widget, widget->stat.entry.cursor_pos + w);
	}
	break;

	case Q8TK_KEY_UP:
	case Q8TK_KEY_DOWN:
#ifdef  OLD_FOCUS_CHANGE
		/* DO NOTHING */
#else /* 新たなフォーカス移動方式 */
		if (widget->stat.entry.combo) {
			Q8List *l = widget->stat.entry.combo->stat.combo.list;
			/* コンボの配下にある場合、↑↓でコンボのリストを手繰る */
			while (l) {
				if (strcmp(widget->name,
						   ((Q8tkWidget *)(l->data))->child->name) == 0) {
					if (key == Q8TK_KEY_UP) {
						l = l->prev;
					} else {
						l = l->next;
					}
					if (l) {
						q8tk_entry_set_text(widget,
											((Q8tkWidget *)(l->data))->child->name);
						widget_signal_do(widget, "activate");
						return;
					} else {
						break;
					}
				}
				l = l->next;
			}
			/* 手繰る対象がなければ、リストの先頭(末尾)をセット */
			l = widget->stat.entry.combo->stat.combo.list;
			if (l) {
				if (key == Q8TK_KEY_UP) {
					l = q8_list_first(l);
				} else {
					l = q8_list_last(l);
				}
				if (l) {
					q8tk_entry_set_text(widget,
										((Q8tkWidget *)(l->data))->child->name);
					widget_signal_do(widget, "activate");
					return;
				}
			}
		}
#endif
		if (key == Q8TK_KEY_UP) {
			q8tk_entry_set_position(widget, 0);
		} else {
			q8tk_entry_set_position(widget,
									q8gr_strlen(widget->code, widget->name));
		}
		break;

	case Q8TK_KEY_PAGE_UP:
		q8tk_entry_set_position(widget, 0);
		break;

	case Q8TK_KEY_PAGE_DOWN:
		q8tk_entry_set_position(widget,
								q8gr_strlen(widget->code, widget->name));
		break;

	case Q8TK_KEY_RET:
		widget_signal_do(widget, "activate");
		break;

	case Q8TK_KEY_BS:
		if (widget->stat.entry.cursor_pos > 0) {
			int w = q8gr_strdel(widget->code, widget->name,
								widget->stat.entry.cursor_pos - 1);
			/* カーソルを1文字分、左に移動 */
			q8tk_entry_set_position(widget, widget->stat.entry.cursor_pos - w);
			widget_signal_do(widget, "changed");
		}
		break;

	case Q8TK_KEY_DEL:
		if (widget->stat.entry.cursor_pos < (int)q8gr_strlen(widget->code,
															 widget->name)) {
			q8gr_strdel(widget->code, widget->name,
						widget->stat.entry.cursor_pos);
			/* カーソルの移動は不要 */
			set_construct_flag(TRUE);
			widget_signal_do(widget, "changed");
		}
		break;

	default:
		if (key <= 0xff && isprint(key)) {
			int len = (int)strlen(widget->name) + 1; /* 1バイト文字を追加 */
			if (widget->stat.entry.max_length &&
				widget->stat.entry.max_length < len) {
				/* 文字列上限サイズを超えたら無視   */
				;
			} else {
				/* メモリが足りないなら確保 */
				q8tk_entry_malloc(widget, len);

				if (q8gr_stradd(widget->code, widget->name,
								widget->stat.entry.cursor_pos, key)) {
					/* カーソルを1文字分、右に移動 */
					q8tk_entry_set_position(widget,
											widget->stat.entry.cursor_pos + 1);
					widget_signal_do(widget, "changed");
				}
			}
		}
		break;
	}
}

Q8tkWidget *q8tk_entry_new(void)
{
	return q8tk_entry_new_with_max_length(0);
}

Q8tkWidget *q8tk_entry_new_with_max_length(int max)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_ENTRY;
	w->sensitive = TRUE;

	w->key_up_used    = TRUE;
	w->key_down_used  = TRUE;
	w->key_left_used  = TRUE;
	w->key_right_used = TRUE;

	q8tk_entry_malloc(w, max);

	w->name[0] = '\0';
	w->code = q8tk_kanji_code;

	w->stat.entry.max_length    = max;
	/*  w->stat.entry.malloc_length = q8tk_entry_malloc()にて設定済み */
	w->stat.entry.disp_pos      = 0;
	w->stat.entry.cursor_pos    = 0;
	w->stat.entry.width         = 8;
	w->stat.entry.editable      = TRUE;
	w->stat.entry.combo         = NULL;

	w->event_button_on = entry_event_button_on;
	w->event_key_on    = entry_event_key_on;

	return w;
}

const char *q8tk_entry_get_text(Q8tkWidget *entry)
{
	return entry->name;
}

void q8tk_entry_set_text(Q8tkWidget *entry, const char *text)
{
	int len = (int) strlen(text);

	if (entry->stat.entry.max_length == 0) {
		/* 上限なし */

		/* メモリが足りないなら、確保 */
		q8tk_entry_malloc(entry, len);
		strcpy(entry->name, text);

	} else {
		/* 上限あり */

		q8gr_strncpy(entry->code,
					 entry->name, text, entry->stat.entry.max_length - 1);
		entry->name[ entry->stat.entry.max_length - 1 ] = '\0';

	}

	/* 文字列先頭から表示 */
	entry->stat.entry.disp_pos = 0;

	/* カーソル非表示 */
	q8tk_entry_set_position(entry, -1);
}

void q8tk_entry_set_position(Q8tkWidget *entry, int position)
{
	int disp_pos;
	int tail = q8gr_strlen(entry->code, entry->name);

	if (position < 0) {
		entry->stat.entry.cursor_pos = -1;
		set_construct_flag(TRUE);
		return;
	}

	if (tail < position) {
		position = tail;
	}
	if (q8gr_strchk(entry->code, entry->name, position) == 2) {
		position -= 1;
	}

	if (position < entry->stat.entry.disp_pos) {
		/* 表示範囲より前にカーソルセット */

		entry->stat.entry.disp_pos   = position;
		entry->stat.entry.cursor_pos = position;

	} else if (entry->stat.entry.disp_pos + entry->stat.entry.width
			   <= position) {
		/* 表示範囲より後ろにカーソルセット */
		disp_pos = position - entry->stat.entry.width + 1;
		if (q8gr_strchk(entry->code, entry->name, disp_pos) == 2) {
			disp_pos += 1;
		}
		entry->stat.entry.disp_pos   = disp_pos;
		entry->stat.entry.cursor_pos = position;

	} else if ((entry->stat.entry.disp_pos + entry->stat.entry.width - 1
				== position) &&
			   q8gr_strchk(entry->code, entry->name, position) == 1) {
		/* 表示範囲の末尾 (全角文字の2バイト目)にカーソルセット */
		disp_pos = entry->stat.entry.disp_pos + 1;
		if (q8gr_strchk(entry->code, entry->name, disp_pos) == 2) {
			disp_pos += 1;
		}
		entry->stat.entry.disp_pos   = disp_pos;
		entry->stat.entry.cursor_pos = position;

	} else {
		/* それ以外 (表示範囲内にカーソルセット) */
		entry->stat.entry.cursor_pos = position;

	}
	q8gr_set_cursor_blink();
	set_construct_flag(TRUE);
}

void q8tk_entry_set_max_length(Q8tkWidget *entry, int max)
{
	/* メモリが足りないなら、確保 */
	q8tk_entry_malloc(entry, max);

	entry->stat.entry.max_length = max;
}

void q8tk_entry_set_editable(Q8tkWidget *entry, int editable)
{
	if (entry->stat.entry.editable != editable) {
		entry->stat.entry.editable = editable;
		set_construct_flag(TRUE);
	}
}
