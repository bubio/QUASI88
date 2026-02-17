/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "alarm.h"




/********************************************************/
/* ウィジットの作成                                     */
/********************************************************/

/***********************************************************************
 * 表示関係
 ************************************************************************/
/*---------------------------------------------------------------------------
 * 可視化、不可視化
 *---------------------------------------------------------------------------
 * ・ウィジットの表示・非表示
 * --------------------------------------------------------------------------
 * void q8tk_widget_show(Q8tkWidget *widget)
 *    ウィジットを表示する。
 *
 * void q8tk_widget_hide(Q8tkWidget *widget)
 *    ウィジットを非表示にする。子も全て非表示になる。
 *---------------------------------------------------------------------------*/
void q8tk_widget_show(Q8tkWidget *widget)
{
	widget->visible = TRUE;
	widget_map(widget);
}
void q8tk_widget_hide(Q8tkWidget *widget)
{
	widget->visible = FALSE;
	widget_map(widget);
}


/*---------------------------------------------------------------------------
 * 操作可否
 *---------------------------------------------------------------------------
 * ・ウィジットの操作可否
 * --------------------------------------------------------------------------
 * void q8tk_widget_set_sensitive(Q8tkWidget *widget, int sensitive)
 *    ウィジットを操作可能／操作不能にする。
 *    ---------------------------------------------------------------------
 *    操作可否は、子に伝わる。
 *    操作不能なウィジットは、通常とは違う色(白系の色)の表示となる。
 *    もともと操作できないウィジット(ラベルなど)に対しても、指定できる。
 *    ---------------------------------------------------------------------
 *    VSEPARATOR / HSEPARATOR は、操作不能にしても色は変化しない。
 *    (変化させたほうがいいかも?)
 *
 *    NOTEPAGE は、内部でコンテナ(NOTEPAGE)を自動生成する仕組みなので、
 *    操作可否を設定できない。 (無理やりさせたければ、
 *    q8tk_widget_set_sensitive(widget->parent, FALSE) でできるが…)
 *
 *    ADJUSTMENT は操作可否を設定できるが、スケールなどのほうで設定する
 *    だろうから、直接指定することはないだろう。
 *---------------------------------------------------------------------------*/
void q8tk_widget_set_sensitive(Q8tkWidget *widget, int sensitive)
{
	widget->sensitive = sensitive;
	widget_map(widget);
}


/***********************************************************************
 * ウィジットの消去
 ************************************************************************/
/*---------------------------------------------------------------------------
 * ウィジット消去
 *---------------------------------------------------------------------------
 * ・ウィジットを消去する
 *   ダイアログ           … q8tk_dialog_new にて生成したウィジット全て消去。
 *                           vbox や action_area に載せたウィジットも全て
 *                           消去される。
 *
 *   ファイルセレクション … 内部で生成した全てのウィジットを消去。
 *                           ファイル選択途中に生成したウィジットも全て消去
 *
 *   その他               … 内部で自動生成したウィジットもあわせて消去。
 *                           q8tk_xxx_new_with_label で生成したラベルや、
 *                           q8tk_scrolled_window_new で自動生成した
 *                           アジャストメントなど
 * --------------------------------------------------------------------------
 * void q8tk_widget_destroy(Q8tkWidget *widget)
 *    new / new_with_label で生成したウィジット widget を消去する。
 *    以降、引数の widget は使用禁止
 *    widget がウインドウの場合、予め q8tk_grab_remove しておくこと
 *    ダイアログの場合、内部で生成した全てのウィジットを消去する。
 *        vbox や action_area に載せたウィジットは消去しない。
 *        ウインドウに関連づけたアクセラレータは消去しない。
 *    ファイルセレクションの場合、内部で生成した全てのウィジットを消去する。
 *        ウインドウに関連づけたアクセラレータは消去しない。
 *    ウインドウの場合、
 *        ウインドウに関連づけたアクセラレータは消去しない。
 *    その他の場合、内部で生成した全てのウィジットを消去する。
 *        new_with_label で生成したラベルや、
 *        q8tk_scrolled_window_new で自動生成したアジャストメントなど。
 *        関数の呼び出し前に、コンテナから remove しておくこと
 *
 * void q8tk_widget_destroy_all_related(Q8tkWidget *widget)
 *    ウィジット widget と、その子・仲間ウィジットを再帰的に全て消去する。
 *    以降、消去したウィジット・子・仲間のウィジットは全て使用禁止
 *    widget がウインドウの場合、予め q8tk_grab_remove しておくこと
 *    ダイアログの場合、内部で生成した全てのウィジットだけでなく、
 *        vbox や action_area に載せたウィジットや、
 *        ウインドウに関連づけたアクセラレータも消去する。
 *    ファイルセレクションの場合、内部で生成した全てのウィジットだけでなく、
 *        ウインドウに関連づけたアクセラレータも消去する。
 *    ウインドウの場合、
 *        ウインドウに関連づけたアクセラレータも消去する。
 *    その他の場合、内部で生成した全てのウィジットを消去する。
 *
 *    widget にはウインドウを指定することを想定している。想定外のウィジットが
 *    削除されるかもしれないので、慎重に使用すること。
 *---------------------------------------------------------------------------*/
void q8tk_widget_destroy(Q8tkWidget *widget)
{
	widget_destroy_core(widget, 1);
}

void q8tk_widget_destroy_all_related(Q8tkWidget *widget)
{
	if (widget->next) {
		q8tk_widget_destroy_all_related(widget->next);
	}
	if (widget->child) {
		q8tk_widget_destroy_all_related(widget->child);
		widget->child = NULL;
	}

	widget_destroy_core(widget, 2);
}


/*
 * level は、削除の再帰度合い
 * 0 : 自ウィジットのみ削除する
 * 1 : 内部で自動生成したウィジットだけは削除する
 * 2 : 子や仲間のウィジットも再帰的に削除する
 */
void widget_destroy_core(Q8tkWidget *widget, int level)
{
	/* ウィジットが、内部で自動生成したウィジットを削除する */
	switch (widget->type) {

	case Q8TK_TYPE_WINDOW:
		if (level >= 1) {
			Q8tkWidget *work = widget->stat.window.work;
			if (work) {
				switch (work->type) {

				case Q8TK_TYPE_DIALOG:
					if (level == 1) {
						/* 自動生成したウィジットのみ削除 */
						q8tk_widget_destroy(Q8TK_DIALOG(widget)->action_area);
						q8tk_widget_destroy(Q8TK_DIALOG(widget)->content_area);
						if (Q8TK_DIALOG(widget)->separator) {
							q8tk_widget_destroy(Q8TK_DIALOG(widget)->separator);
						}
						if (Q8TK_DIALOG(widget)->frame) {
							q8tk_widget_destroy(Q8TK_DIALOG(widget)->frame);
						}
						q8tk_widget_destroy(Q8TK_DIALOG(widget)->box);
						q8tk_widget_destroy(work);
					} else {
						/* 子や仲間ウィジットは再帰的に削除済み */
						q8tk_widget_destroy(work);
					}
					break;

				case Q8TK_TYPE_FILE_SELECTION:
					q8tk_widget_destroy_all_related(widget->child);
					q8tk_widget_destroy(work);
					break;
				}
			}
		}
		if (level >= 2) {
			if (widget->stat.window.accel) {
				q8tk_widget_destroy(widget->stat.window.accel);
			}
		}
		break;

	case Q8TK_TYPE_FILE_SELECTION:
		free(widget->stat.fselect.pathname);
		free(widget->stat.fselect.filename);
		break;

	case Q8TK_TYPE_ITEM_LABEL:
		if (widget->stat.label.overlay) {
			q8tk_widget_destroy(widget->stat.label.overlay);
		}
		break;

	case Q8TK_TYPE_BMP:
	case Q8TK_TYPE_ITEM_BMP:
		widget_bitmap_table_remove(widget->stat.bmp.alloc);
		break;

	case Q8TK_TYPE_RADIO_BUTTON:
	case Q8TK_TYPE_RADIOPUSH_BUTTON:
		if (level >= 1) {
			q8_list_remove(widget->stat.button.list, widget);
		}
		break;

	case Q8TK_TYPE_COMBO:
		if (level >= 1) {
			Q8List *l = widget->stat.combo.list;
			while (l) {
				q8tk_widget_destroy((Q8tkWidget *)(l->data));
				l = l->next;
			}
			q8_list_free(widget->stat.combo.list);
			q8tk_widget_destroy(widget->stat.combo.entry);
		}
		break;

	case Q8TK_TYPE_ACCEL_GROUP:
		if (level >= 1) {
			if (widget->child) {
				q8tk_widget_destroy_all_related(widget->child);
			}
		}
		break;

	case Q8TK_TYPE_SCROLLED_WINDOW:
		if (level >= 1) {
			if (widget->with_label) {
				widget->with_label = FALSE;
				q8tk_widget_destroy(widget->stat.scrolled.hadj);
				q8tk_widget_destroy(widget->stat.scrolled.vadj);
			}
		}
		break;

	case Q8TK_TYPE_HSCALE:
		if (level >= 1) {
			if (widget->with_label) {
				widget->with_label = FALSE;
				q8tk_widget_destroy(widget->stat.scale.adj);
			}
		}
		break;

	case Q8TK_TYPE_VSCALE:
		if (level >= 1) {
			if (widget->with_label) {
				widget->with_label = FALSE;
				q8tk_widget_destroy(widget->stat.scale.adj);
			}
		}
		break;

	default:
		break;
	}

	/* XXX_new_with_label() にて生成された LABEL を削除する */
	if (level >= 1) {
		if (widget->with_label &&
			widget->child      &&
			widget->child->type == Q8TK_TYPE_LABEL) {
			widget->with_label = FALSE;
			q8tk_widget_destroy(widget->child);
		}
	}

	if (widget->name) {
		free(widget->name);
		widget->name = NULL;
	}

	alarm_remove(widget);

	if (get_drag_widget() == widget) {
		set_drag_widget(NULL);
	}

	free_widget(widget);
}





/***********************************************************************
 * 特定のウィジットにフォーカスを当てる。
 *
 * 通常 フォーカスは、直前にボタンやキー入力がなされたウィジット
 * になり、TAB により切替え可能だが、この関数で特定のウィジット
 * にフォーカスを設定することができる。
 * ただし、そのウィジットの一番先祖の WINDOW が、q8tk_grab_add()の
 * 処理をなされたあとでなければ無効である。
 ************************************************************************/
void q8tk_widget_set_focus(Q8tkWidget *widget)
{
	if (BETWEEN(MIN_WINDOW_LAYER, window_layer_level, MAX_WINDOW_LAYER - 1)) {

		Q8tkWidget *lost_focus_widget = focus_widget[ window_layer_level ];
		if (lost_focus_widget &&
			lost_focus_widget != widget) {
			/* フォーカスを失ったウィジットの処理があればここに書く */

			switch (lost_focus_widget->type) {
			case Q8TK_TYPE_LISTBOX:
				lost_focus_widget->stat.listbox.active = 
					lost_focus_widget->stat.listbox.selected;
				break;
			default:
				break;
			}
		}

		focus_widget[ window_layer_level ] = widget;
		set_construct_flag(TRUE);
	}
}


/***********************************************************************
 * ウィジットにタイマーを設定する
 *
 * 内部処理で使うために、やっつけで作ったので、
 * タイマーはシステム全体で MAX_ALARM 個しか設定できないとか、
 * タイマーの精度が適当とか、いろいろ問題がある。
 ************************************************************************/
void q8tk_timer_add(Q8tkWidget *widget,
					void (*callback)(Q8tkWidget *widget, void *parm),
					void *parm, int timer_ms)
{
	alarm_add(widget, parm, (alarm_callback_func) callback, timer_ms, FALSE);
}

void q8tk_timer_remove(Q8tkWidget *widget)
{
	alarm_remove(widget);
}
