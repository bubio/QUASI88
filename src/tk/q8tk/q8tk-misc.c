/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"


/***********************************************************************
 * 漢字コード
 * Q8TK は文字列を char型で扱っているので、UTF16 には対応していない。
 *
 * FILE VIEWER で UTF16 のテキストファイルを表示させたかったので、
 * 文字コードを内部で置換するという強引な方法をやってみたのだが、
 * この方法は ENTRY や FILE SELECTION などでは使えない。
 * UTF16 に対応するには、Q8TK をワイド文字が扱えるよう改造するしかない ?
 ************************************************************************/
int q8tk_set_kanjicode(int code)
{
	int ret = q8tk_kanji_code;

	if ((Q8TK_KANJI_ANK <= code) && (code < Q8TK_KANJI_END)) {
		q8tk_kanji_code = code;
	} else {
		q8tk_kanji_code = Q8TK_KANJI_ANK;
	}

	return ret;
}
/***********************************************************************
 * カーソル表示の有無
 ************************************************************************/
void q8tk_set_cursor(int enable)
{
	int new_disp_cursor = (enable) ? TRUE : FALSE;

	if (q8tk_disp_cursor != new_disp_cursor) {
		q8tk_disp_cursor = new_disp_cursor;
		set_construct_flag(TRUE);
	}
}





/***********************************************************************
 * 雑多な処理
 ************************************************************************/

/*---------------------------------------------------------------------------
 * 表示位置の変更
 *---------------------------------------------------------------------------
 * ・HBOX、VBOX直下の子のウィジットのみ有効。他は無効 (無視)
 * --------------------------------------------------------------------------
 * void q8tk_misc_set_placement(Q8tkWidget *widget,
 *                              int placement_x, int placement_y)
 *---------------------------------------------------------------------------*/
void q8tk_misc_set_placement(Q8tkWidget *widget,
							 int placement_x, int placement_y)
{
	widget->placement_x = placement_x;
	widget->placement_y = placement_y;
}

/*---------------------------------------------------------------------------
 * 表示サイズの変更
 *---------------------------------------------------------------------------
 * ・以下のウィジットのみ有効。他は無効 (無視)
 *   COMBO           … 文字列部分の表示幅
 *   LIST            … 文字列の表示幅
 *   SCROLLED WINDOW … ウインドウの幅、高さ
 *   ENTRY           … 文字列の表示幅
 * --------------------------------------------------------------------------
 * void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
 *---------------------------------------------------------------------------*/
void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
{
	switch (widget->type) {

	case Q8TK_TYPE_COMBO:
		if (width > 0) {
			widget->stat.combo.width = width;
			q8tk_misc_set_size(widget->stat.combo.entry,
							   widget->stat.combo.width, 0);
		} else {
			widget->stat.combo.width = 0;
			q8tk_misc_set_size(widget->stat.combo.entry,
							   widget->stat.combo.length, 0);
		}
		break;

	case Q8TK_TYPE_LISTBOX:
		if (width > 0) {
			widget->stat.listbox.width = width;
		} else {
			widget->stat.listbox.width = 0;
		}
		break;

	case Q8TK_TYPE_SCROLLED_WINDOW:
		widget->stat.scrolled.width  = width;
		widget->stat.scrolled.height = height;

		if (widget->with_label) {
			q8tk_adjustment_set_increment(widget->stat.scrolled.hadj,
										  1, MAX(10, width - 3));
			q8tk_adjustment_set_increment(widget->stat.scrolled.vadj,
										  1, MAX(10, height - 3));
			q8tk_adjustment_set_speed(widget->stat.scrolled.hadj, 0, 3);
			q8tk_adjustment_set_speed(widget->stat.scrolled.vadj, 0, 3);
		}
		break;

	case Q8TK_TYPE_ENTRY:
		widget->stat.entry.width = width;
		q8tk_entry_set_position(widget, widget->stat.entry.cursor_pos);
		break;

	default:
		fprintf(stderr, "Cant resize widget=%s\n", debug_type(widget->type));
		Q8tkAssert(FALSE, NULL);
		return;
	}
	set_construct_flag(TRUE);
}
