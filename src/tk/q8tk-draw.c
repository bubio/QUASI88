/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"

#include "q8tk-listbox.h"
#include "q8tk-adjustment.h"

/***********************************************************************
 *
 *
 *
 *
 *
 *
 ************************************************************************/

/*--------------------------------------------------------------
 * 子ウィジットのサイズを元に、自分自身のサイズを計算する。
 * 再帰的に、全ての子ウィジェットも同時に計算する。
 *
 *    *widget … 一番親のウィジット
 *    *max_sx … 全ての子ウィジットのなかで最大サイズ x
 *    *max_sy … 全ての子ウィジットのなかで最大サイズ y
 *    *sum_sx … 子の仲間ウィジットのサイズ総和 x
 *    *sum_sy … 子の仲間ウィジットのサイズ総和 y
 *--------------------------------------------------------------*/
static  void    widget_resize(Q8tkWidget *widget, int max_sx, int max_sy);
static  void    widget_size(Q8tkWidget *widget,
							int *max_sx, int *max_sy,
							int *sum_sx, int *sum_sy)
{
	int n_msx, n_msy, n_ssx, n_ssy;

	/*  printf("%d \n", widget->type); fflush(stdout);*/

	/* 自分自身の仲間 (next) が存在すれば、再帰的に計算 */
	if (widget->next) {
		widget_size(widget->next, &n_msx, &n_msy, &n_ssx, &n_ssy);
	} else {
		n_msx = n_msy = n_ssx = n_ssy = 0;
	}


	if (widget->visible) {

		int c_msx, c_msy, c_ssx, c_ssy;

		/* 子ウィジットのサイズ計算(再帰) */
		if (widget->child) {
			widget_size(widget->child, &c_msx, &c_msy, &c_ssx, &c_ssy);
		} else {
			c_msx = c_msy = c_ssx = c_ssy = 0;
		}

		/* 子ウィジットを元に、自身のサイズ計算 */
		switch (widget->type) {

		case Q8TK_TYPE_WINDOW:
			if (widget->stat.window.no_frame) {
				widget->sx = c_msx;
				widget->sy = c_msy;
			} else {
				widget->sx = c_msx + 2;
				widget->sy = c_msy + 2;
			}
			break;

		case Q8TK_TYPE_BUTTON:
		case Q8TK_TYPE_TOGGLE_BUTTON:
		case Q8TK_TYPE_RADIOPUSH_BUTTON:
			if (widget->stat.button.from_bitmap == FALSE) {
				widget->sx = c_msx + 2;
				widget->sy = c_msy + 2;
			} else {
				widget->sx = c_msx;
				widget->sy = c_msy;
			}
			break;

		case Q8TK_TYPE_CHECK_BUTTON:
		case Q8TK_TYPE_RADIO_BUTTON:
			widget->sx = c_msx + 3;
			widget->sy = Q8TKMAX(c_msy, 1);
			break;

		case Q8TK_TYPE_SWITCH:
			widget->sx = 11;
			widget->sy = 3;
			break;

		case Q8TK_TYPE_FRAME:
			if (widget->name) {
				widget->sx = Q8TKMAX(c_msx,
									 q8gr_strlen(widget->code, widget->name))
							 + 2;
			} else {
				widget->sx = c_msx + 2;
			}
			widget->sy = c_msy + 2;
			break;

		case Q8TK_TYPE_LABEL:
			if (widget->name) {
				widget->sx = q8gr_strlen(widget->code, widget->name);
				/* LIST ITEM の子の場合、最大文字幅を偽る */
				if ((widget->stat.label.width > 0) &&
					(widget->sx > widget->stat.label.width)) {
					widget->sx = widget->stat.label.width;
				}
			} else {
				widget->sx = 0;
			}
			widget->sy = 1;
			break;

		case Q8TK_TYPE_ITEM_LABEL:
			widget->sx = widget->stat.label.width;
			widget->sy = 1;
			break;

		case Q8TK_TYPE_BMP:
		case Q8TK_TYPE_ITEM_BMP:
			widget->sx = widget->stat.bmp.width;
			widget->sy = widget->stat.bmp.height;
			break;

		case Q8TK_TYPE_NOTEBOOK:
			{
				int     len = 0;
				Q8tkWidget *n = widget->child;
				while (n) {
					if (n ->name) {
						len += q8gr_strlen(n->code, n->name);
					}
					len += 1;
					n = n->next;
				}
				len += 1;
				widget->sx = Q8TKMAX(c_msx + 2, len);
			}
			widget->sy = c_msy + 4;
			break;

		case Q8TK_TYPE_NOTEPAGE:
			widget->sx = c_msx;
			widget->sy = c_msy;
			break;

		case Q8TK_TYPE_VBOX:
			widget->sx = c_msx;
			widget->sy = c_ssy;
			break;

		case Q8TK_TYPE_HBOX:
			widget->sx = c_ssx;
			widget->sy = c_msy;
			break;

		case Q8TK_TYPE_ITEMBAR:
			widget->sx = widget->stat.itembar.max_width;
			widget->sy = widget->stat.itembar.max_height;
			break;

		case Q8TK_TYPE_VSEPARATOR:
			widget->sx = 1;
			widget->sy = 1;
			break;

		case Q8TK_TYPE_HSEPARATOR:
			widget->sx = 1;
			widget->sy = 1;
			break;

		case Q8TK_TYPE_COMBO:
			if (widget->stat.combo.width) {
				widget->sx = widget->stat.combo.width;
			} else {
				if (widget->stat.combo.length) {
					widget->sx = widget->stat.combo.length;
				} else {
					widget->sx = 8;
				}
			}
			widget->sx += 3;
			widget->sy = 1;
			break;

		case Q8TK_TYPE_LISTBOX:
			widget->sx = Q8TKMAX(c_msx, widget->stat.listbox.width);
			widget->sy = c_ssy;
			break;

		case Q8TK_TYPE_LIST_ITEM:
			widget->sx = c_msx;
			widget->sy = c_msy;
			if (((widget->parent)->type == Q8TK_TYPE_LISTBOX) &&
				(widget->parent)->stat.listbox.big) {
				widget->sx += 2;
				widget->sy += ((widget->next) ? 1 : 2);
			}
			break;

		case Q8TK_TYPE_ADJUSTMENT:
			Q8tkAssert(FALSE, NULL);
			break;

		case Q8TK_TYPE_HSCALE:
		case Q8TK_TYPE_VSCALE:
			if (widget->stat.scale.adj) {
				int sx, sy;
				adjustment_size(&widget->stat.scale.adj->stat.adj, &sx, &sy);

				if (widget->stat.scale.draw_value) {
					if (widget->stat.scale.value_pos == Q8TK_POS_LEFT ||
						widget->stat.scale.value_pos == Q8TK_POS_RIGHT) {
						widget->sx = sx + 4;
						widget->sy = Q8TKMAX(sy, 1);
					} else {
						/* Q8TK_POS_UP || Q8TK_POS_BOTTOM */
						widget->sx = Q8TKMAX(sx, 3);
						widget->sy = sy + 1;
					}
				} else {
					widget->sx = sx;
					widget->sy = sy;
				}
			} else {
				widget->sx = 0;
				widget->sy = 0;
			}
			break;

		case Q8TK_TYPE_SCROLLED_WINDOW:
			if (widget->child) {

				int win_w, win_h, bar_w, bar_h, tmp;
				int frame_size
					= (widget->stat.scrolled.no_frame == FALSE) ? 2 : 0;
				int adj_size = (widget->stat.scrolled.vadj->stat.adj.size);
				int arrow_size = adj_size;
				int bar_size = adj_size;

				tmp = 0;
				switch (widget->stat.scrolled.hpolicy) {
				case Q8TK_POLICY_NEVER:
					tmp |= 0x00;
					break;
				case Q8TK_POLICY_ALWAYS:
					tmp |= 0x01;
					break;
				default: /* AUTOMATIC */
					if (c_msx <= widget->stat.scrolled.width - frame_size) {
						tmp |= 0x00;
					} else {
						tmp |= 0x01;
					}
				}
				switch (widget->stat.scrolled.vpolicy) {
				case Q8TK_POLICY_NEVER:
					tmp |= 0x00;
					break;
				case Q8TK_POLICY_ALWAYS:
					tmp |= 0x10;
					break;
				default: /* AUTOMATIC */
					if (c_msy <= widget->stat.scrolled.height - frame_size) {
						tmp |= 0x00;
					} else {
						tmp |= 0x10;
					}
				}

				if (tmp == 0x00) {
					widget->stat.scrolled.hscrollbar = FALSE;
					widget->stat.scrolled.vscrollbar = FALSE;
				} else if (tmp == 0x01) {
					widget->stat.scrolled.hscrollbar = TRUE;
					widget->stat.scrolled.vscrollbar = FALSE;
				} else if (tmp == 0x10) {
					widget->stat.scrolled.hscrollbar = FALSE;
					widget->stat.scrolled.vscrollbar = TRUE;
				} else {
					widget->stat.scrolled.hscrollbar = TRUE;
					widget->stat.scrolled.vscrollbar = TRUE;
				}

				if (widget->stat.scrolled.vscrollbar) {
					win_w = widget->stat.scrolled.width - frame_size - bar_size;
					bar_w = widget->stat.scrolled.width - arrow_size * 2 - bar_size;
				} else {
					win_w = widget->stat.scrolled.width - frame_size;
					bar_w = widget->stat.scrolled.width - arrow_size * 2;
				}
				if (widget->stat.scrolled.hscrollbar) {
					win_h = widget->stat.scrolled.height - frame_size - bar_size;
					bar_h = widget->stat.scrolled.height - arrow_size * 2 - bar_size;
				} else {
					win_h = widget->stat.scrolled.height - frame_size;
					bar_h = widget->stat.scrolled.height - arrow_size * 2;
				}

				q8tk_adjustment_set_length(widget->stat.scrolled.hadj, bar_w);
				q8tk_adjustment_clamp_page(widget->stat.scrolled.hadj,
										   0, c_msx - win_w);
				adjustment_size(&widget->stat.scrolled.hadj->stat.adj,
								&tmp, &tmp);

				q8tk_adjustment_set_length(widget->stat.scrolled.vadj, bar_h);
				q8tk_adjustment_clamp_page(widget->stat.scrolled.vadj,
										   0, c_msy - win_h);
				adjustment_size(&widget->stat.scrolled.vadj->stat.adj,
								&tmp, &tmp);

				if (widget->child->type == Q8TK_TYPE_LISTBOX &&
					(widget->stat.scrolled.vadj->stat.adj.listbox_changed) &&
					(widget->stat.scrolled.vadj_value
					 != widget->stat.scrolled.vadj->stat.adj.value)) {
					list_event_window_scrolled(widget, win_h);
				}
			}
			widget->stat.scrolled.vadj_value
				=  widget->stat.scrolled.vadj->stat.adj.value;
			widget->stat.scrolled.vadj->stat.adj.listbox_changed = FALSE;
			widget->sx = widget->stat.scrolled.width;
			widget->sy = widget->stat.scrolled.height;
			break;

		case Q8TK_TYPE_ENTRY:
			widget->sx = widget->stat.entry.width;
			widget->sy = 1;
			break;


		case Q8TK_TYPE_DIALOG:
			Q8tkAssert(FALSE, NULL);
			break;
		case Q8TK_TYPE_FILE_SELECTION:
			Q8tkAssert(FALSE, NULL);
			break;

		default:
			Q8tkAssert(FALSE, "Undefined type");
		}

	} else {
		widget->sx = 0;
		widget->sy = 0;
	}


	/* サイズ情報更新 */
	*max_sx = Q8TKMAX(widget->sx, n_msx);
	*max_sy = Q8TKMAX(widget->sy, n_msy);
	*sum_sx = widget->sx + n_ssx;
	*sum_sy = widget->sy + n_ssy;

	/* 子ウィジットにセパレータが含まれる場合は、サイズ調整 */
	widget_resize(widget, widget->sx, widget->sy);

	/* リストボックスなどの場合、子ウィジットのサイズを調整 */
	if (widget->type == Q8TK_TYPE_LISTBOX) {
		Q8tkWidget *child = widget->child;
		while (child) {
			Q8tkAssert(child->type == Q8TK_TYPE_LIST_ITEM, NULL);
			child->sx = widget->sx;
			if (child->next) {
				child = child->next;
			} else {
				break;
			}
		}
	}


	/*printf("%s (%02d,%02d) max{ %02d,%02d } sum{ %02d,%02d }\n",debug_type(widget->type),widget->sx,widget->sy,*max_sx,*max_sy,*sum_sx,*sum_sy);fflush(stdout);*/
}


/*
 * セパレータなど、親の大きさに依存するウィジットのサイズを再計算する
 */
static void widget_resize(Q8tkWidget *widget, int max_sx, int max_sy)
{
	if (widget->type == Q8TK_TYPE_WINDOW   ||
		widget->type == Q8TK_TYPE_NOTEPAGE ||
		widget->type == Q8TK_TYPE_VBOX     ||
		widget->type == Q8TK_TYPE_HBOX     ||
		widget->type == Q8TK_TYPE_ITEMBAR) {

		Q8tkWidget *child = widget->child;

		if (widget->type == Q8TK_TYPE_WINDOW &&
			! widget->stat.window.no_frame) {
			max_sx -= 2;
			max_sy -= 2;
		}
		if (child) {
			widget_resize(child, max_sx, max_sy);
		}

		while (child) {
			switch (child->type) {

			case Q8TK_TYPE_HSEPARATOR:
				if (widget->type != Q8TK_TYPE_HBOX) {
					if (child->sx < max_sx) {
						child->sx = max_sx;
					}
				}
				break;

			case Q8TK_TYPE_VSEPARATOR:
				if (widget->type != Q8TK_TYPE_VBOX) {
					if (child->sy < max_sy) {
						child->sy = max_sy;
					}
				}
				break;

			case Q8TK_TYPE_VBOX:
				if (widget->type == Q8TK_TYPE_VBOX) {
					if (child->sx < max_sx) {
						child->sx = max_sx;
					}
				}
				break;

			case Q8TK_TYPE_HBOX:
			case Q8TK_TYPE_ITEMBAR:
				if (widget->type == Q8TK_TYPE_HBOX ||
					widget->type == Q8TK_TYPE_ITEMBAR) {
					if (child->sy < max_sy) {
						child->sy = max_sy;
					}
				}
				break;
			}

			if (child->next) {
				child = child->next;
			} else {
				break;
			}
		}
	}
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*--------------------------------------------------------------
 * 自分自身の表示位置をもとに、描画し、
 *  再帰的に、全ての子ウィジェットも描画する。
 *--------------------------------------------------------------*/

static void widget_draw(Q8tkWidget *widget,
						int parent_focus, int parent_sensitive)
{
	int         x = widget->x;
	int         y = widget->y;
	Q8tkWidget  *child = widget->child;
	int         focus, sensitive;
	int         next_focus;
	Q8tkWidget  *sense_widget;

	/* 親がフォーカスありか、自身がフォーカスありで、フォーカスあり状態とする*/
	focus = (parent_focus || (widget == get_focus_widget())) ? TRUE : FALSE;

	/* 親が操作可能で、自身も操作可能なら、操作可能とする */
	sensitive = (parent_sensitive && widget->sensitive) ? TRUE : FALSE;
	sense_widget = (sensitive) ? widget : NULL;

	/* 仲間にフォーカスを伝える場合、真。通常は伝えない */
	next_focus = FALSE;


	widget_scrollin_drawn(widget);


	/* HBOX、VBOX の子の場合に限り、
	 * 配置を若干変更できる。
	 * FRAME もついでにヨシとしよう */

	if (widget->parent) {
		switch ((widget->parent)->type) {

		case Q8TK_TYPE_FRAME:
			if (widget->placement_x == Q8TK_PLACEMENT_X_CENTER) {
				x += ((widget->parent)->sx - 2 - widget->sx) / 2;
			} else if (widget->placement_x == Q8TK_PLACEMENT_X_RIGHT) {
				x += ((widget->parent)->sx - 2 - widget->sx);
			}
			widget->x = x;
			break;

		case Q8TK_TYPE_VBOX:
			if (widget->placement_x == Q8TK_PLACEMENT_X_CENTER) {
				x += ((widget->parent)->sx - widget->sx) / 2;
			} else if (widget->placement_x == Q8TK_PLACEMENT_X_RIGHT) {
				x += ((widget->parent)->sx - widget->sx);
			}
			widget->x = x;
			break;

		case Q8TK_TYPE_HBOX:
		case Q8TK_TYPE_ITEMBAR:
			if (widget->placement_y == Q8TK_PLACEMENT_Y_CENTER) {
				y += ((widget->parent)->sy - widget->sy) / 2;
			} else if (widget->placement_y == Q8TK_PLACEMENT_Y_BOTTOM) {
				y += ((widget->parent)->sy - widget->sy);
			}
			widget->y = y;
			break;
		}
	}


	/* 自分自身の typeをもとに枠などを書く。
	 * 子がいれば、x,y を求る。
	 * 子の仲間(next)の、x,y も求める。
	 * 直下の子に対してのみ再帰的に処理。 */

	/*printf("%s (%d,%d) %d %d\n",debug_type(widget->type),widget->sx,widget->sy,widget->x,widget->y);fflush(stdout);*/

	if (widget->visible) {

		switch (widget->type) {

		case Q8TK_TYPE_WINDOW:
			if (widget->stat.window.no_frame) {
				q8gr_clear_area(x, y, widget->sx, widget->sy);
			} else {
				q8gr_draw_window(x, y, widget->sx, widget->sy,
								 widget->stat.window.shadow_type,
								 (widget->stat.window.type
								  == Q8TK_WINDOW_POPUP)
								 ? Q8GR_WIDGET_WINDOW : NULL);
				/* q8gr_clear_area(x + 1, y + 1, sx - 2, sy - 2) ??? */
			}
			if (child) {
				child->x = x  + (widget->stat.window.no_frame ? 0 : 1);
				child->y = y  + (widget->stat.window.no_frame ? 0 : 1);
				widget_draw(child, FALSE, sensitive);
			}
			break;

		case Q8TK_TYPE_BUTTON:
		case Q8TK_TYPE_TOGGLE_BUTTON:
			if (widget->stat.button.from_bitmap == FALSE) {
				widget_focus_list_append(sense_widget);
				q8gr_draw_button(x, y, widget->sx, widget->sy,
								 widget->stat.button.active, sense_widget);
				if (child) {
					child->x = x + 1;
					child->y = y + 1;
					widget_draw(child, focus, sensitive);
				}
			} else {
				q8gr_draw_button_space(x, y, widget->sx, widget->sy,
									   sense_widget);
				if (sensitive) {
					if (widget->stat.button.active) {
						child->stat.bmp = widget->stat.button.press->stat.bmp;
					} else {
						child->stat.bmp = widget->stat.button.release->stat.bmp;
					}
				} else {
					child->stat.bmp = widget->stat.button.inactive->stat.bmp;
				}
				child->x = x;
				child->y = y;
				widget_draw(child, focus, sensitive);
			}
			break;

		case Q8TK_TYPE_CHECK_BUTTON:
			widget_focus_list_append(sense_widget);
			q8gr_draw_check_button(x, y, widget->stat.button.active,
								   sense_widget);
			if (child) {
				child->x = x + 3;
				child->y = y;
				widget_draw(child, focus, sensitive);
			}
			break;

		case Q8TK_TYPE_RADIO_BUTTON:
		case Q8TK_TYPE_RADIOPUSH_BUTTON:
#if defined(OLD_FOCUS_CHANGE)
			widget_focus_list_append(sense_widget);
#else /* 新たなフォーカス移動方式 */
			if (widget->stat.button.active == Q8TK_BUTTON_ON) {
				widget_focus_list_append(sense_widget);
			}
#endif
			if (widget->type == Q8TK_TYPE_RADIO_BUTTON) {
				q8gr_draw_radio_button(x, y, widget->stat.button.active,
									   sense_widget);
				if (child) {
					child->x = x + 3;
					child->y = y;
					widget_draw(child, focus, sensitive);
				}
			} else {
				q8gr_draw_radio_push_button(x, y, widget->sx, widget->sy,
											widget->stat.button.active,
											sense_widget);
				if (child) {
					child->x = x + 1;
					child->y = y + 1;
					widget_draw(child, focus, sensitive);
				}
			}
			break;

		case Q8TK_TYPE_SWITCH:
			q8gr_draw_switch(x, y, widget->stat.button.active,
							 widget->stat.button.timer, sense_widget);
			break;

		case Q8TK_TYPE_FRAME:
			q8gr_draw_frame(x, y, widget->sx, widget->sy,
							widget->stat.frame.shadow_type,
							widget->code, widget->name,
							sense_widget);
			if (child) {
				child->x = x + 1;
				child->y = y + 1;
				widget_draw(child, FALSE, sensitive);
			}
			break;

		case Q8TK_TYPE_LABEL:
		case Q8TK_TYPE_ITEM_LABEL:
			q8gr_draw_label(x, y,
							widget->stat.label.foreground,
							widget->stat.label.background,
							widget->stat.label.reverse,
							focus,
							(widget->name) ? (widget->code) : Q8TK_KANJI_ANK,
							(widget->name) ? (widget->name) : "",
							((widget->type == Q8TK_TYPE_LABEL)
							 ? 0 : widget->stat.label.width),
							sense_widget);
			break;

		case Q8TK_TYPE_BMP:
		case Q8TK_TYPE_ITEM_BMP:
			q8gr_draw_bitmap(x, y,
							 widget->stat.bmp.foreground,
							 widget->stat.bmp.background,
							 widget->stat.bmp.width,
							 widget->stat.bmp.height,
							 widget->stat.bmp.index << BITMAP_DATA_MAX_OF_BIT);
			break;

		case Q8TK_TYPE_NOTEBOOK:
			q8gr_draw_notebook(x, y, widget->sx, widget->sy,
							   widget, sense_widget);
			if (child) {
				child->x = x;
				child->y = y;
				widget_draw(child, FALSE, sensitive);
			}
			break;

		case Q8TK_TYPE_NOTEPAGE:
			{
				int select_flag =
					(widget == (widget->parent)->stat.notebook.page);

				q8gr_draw_notepage((widget->name) ? widget->code
								   : Q8TK_KANJI_ANK,
								   (widget->name) ? widget->name : "",
								   select_flag,
								   /*(select_flag) ? FALSE :*/ focus,
								   widget->parent,
								   sense_widget);
				if (child && select_flag) {
					child->x = ((widget->parent)->x) + 1;
					child->y = ((widget->parent)->y) + 3;
					widget_focus_list_append(widget);
					widget_draw(child, FALSE, sensitive);
				}
#if defined(OLD_FOCUS_CHANGE)
				else {
					widget_focus_list_append(sense_widget);
				}
#endif
			}
			break;

		case Q8TK_TYPE_VBOX:
			if (child) {
				child->x = x;
				x += 0;
				child->y = y;
				y += child->sy;
				while (child->next) {
					child = child->next;
					child->x = x;
					x += 0;
					child->y = y;
					y += child->sy;
				}
				child = widget->child;
				widget_draw(child, FALSE, sensitive);
			}
			break;

		case Q8TK_TYPE_HBOX:
		case Q8TK_TYPE_ITEMBAR:
			if (child) {
				child->x = x;
				x += child->sx;
				child->y = y;
				y += 0;
				while (child->next) {
					child = child->next;
					child->x = x;
					x += child->sx;
					child->y = y;
					y += 0;
				}
				child = widget->child;
				widget_draw(child, FALSE, sensitive);
			}
			break;

		case Q8TK_TYPE_VSEPARATOR:
			q8gr_draw_vseparator(x, y, widget->sy,
								 widget->stat.separator.style);
			break;

		case Q8TK_TYPE_HSEPARATOR:
			q8gr_draw_hseparator(x, y, widget->sx,
								 widget->stat.separator.style);
			break;

		case Q8TK_TYPE_COMBO:
			widget->stat.combo.entry->x  = x;
			widget->stat.combo.entry->y  = y;
			widget->stat.combo.entry->sx = widget->sx - 3;
			widget_draw(widget->stat.combo.entry, FALSE, sensitive);

#if defined(OLD_FOCUS_CHANGE)
			widget_focus_list_append(sense_widget);
#else /* 新たなフォーカス移動方式 */
			if (widget->stat.combo.entry->stat.entry.editable == FALSE) {
				widget_focus_list_append(sense_widget);
			}
#endif
			q8gr_draw_combo(x, y, widget->sx - 3, focus, sense_widget);
			break;

		case Q8TK_TYPE_LISTBOX:
			widget_focus_list_append(sense_widget);
			if (child) {
				child->x = x;
				x += 0;
				child->y = y;
				y += child->sy;
				while (child->next) {
					child = child->next;
					child->x = x;
					x += 0;
					child->y = y;
					y += child->sy;
				}
				child = widget->child;
				widget_draw(child, focus, sensitive);
			}
			break;

		case Q8TK_TYPE_LIST_ITEM:
			if (child) {
				int big_type;
				int rev   = (widget->parent->stat.listbox.selected == widget)
							? TRUE : FALSE;
				int under = (widget->parent->stat.listbox.active == widget)
							? TRUE : FALSE;
				if (rev && under) {
					under = FALSE;
				}

				if (((widget->parent)->type == Q8TK_TYPE_LISTBOX) &&
					(widget->parent)->stat.listbox.big) {
					big_type  = (widget->next) ? 0x01 : 0x00;
					big_type += (widget->prev) ? 0x02 : 0x00;
					big_type ++;
				} else {
					big_type = 0;
				}

				q8gr_draw_list_item(x, y, widget->sx, focus, rev,
									under, child->code, child->name,
									big_type,
									sense_widget);
			}
			next_focus = focus;
			break;

		case Q8TK_TYPE_ADJUSTMENT:
			Q8tkAssert(FALSE, NULL);
			break;

		case Q8TK_TYPE_HSCALE:
			if (widget->stat.scale.adj) {
				int a_focus = (focus ||
							   (widget->stat.scale.adj == get_focus_widget()))
							  ? TRUE : FALSE;
				Q8tkWidget *a_widget = ((sensitive) ? widget->stat.scale.adj
										: NULL);
				int adj_size = (widget->stat.scale.adj->stat.adj.size);
				widget_focus_list_append(a_widget);
				widget->stat.scale.adj->x = x;
				widget->stat.scale.adj->y = y;
				q8gr_draw_hscale(x, y,
								 &(widget->stat.scale.adj->stat.adj),
								 a_focus,
								 widget->stat.scale.draw_value,
								 widget->stat.scale.value_pos,
								 adj_size,
								 a_widget);
			}
			break;

		case Q8TK_TYPE_VSCALE:
			if (widget->stat.scale.adj) {
				int a_focus = (focus ||
							   (widget->stat.scale.adj == get_focus_widget()))
							  ? TRUE : FALSE;
				Q8tkWidget *a_widget = ((sensitive) ? widget->stat.scale.adj
										: NULL);
				int adj_size = (widget->stat.scale.adj->stat.adj.size);
				widget_focus_list_append(a_widget);
				widget->stat.scale.adj->x = x;
				widget->stat.scale.adj->y = y;
				q8gr_draw_vscale(x, y,
								 &(widget->stat.scale.adj->stat.adj),
								 a_focus,
								 widget->stat.scale.draw_value,
								 widget->stat.scale.value_pos,
								 adj_size,
								 a_widget);
			}
			break;

		case Q8TK_TYPE_SCROLLED_WINDOW:
			if (child) {
				int a_focus;
				Q8tkWidget *a_widget;

				int sx = widget->sx;
				int sy = widget->sy;

				int adj_size = (widget->stat.scrolled.vadj->stat.adj.size);
				int bar_size = adj_size;

				if (widget->stat.scrolled.vscrollbar) {
					sx -= bar_size;
				}
				if (widget->stat.scrolled.hscrollbar) {
					sy -= bar_size;
				}

				{
					int mask_x, mask_y, mask_sx, mask_sy;
					q8gr_get_screen_mask(&mask_x, &mask_y, &mask_sx, &mask_sy);

					if (widget->stat.scrolled.no_frame) {
						q8gr_clear_area(x, y, sx, sy);
						q8gr_set_screen_mask(x, y, sx, sy);
						widget->stat.scrolled.child_x0
							= widget->stat.scrolled.hadj->stat.adj.value;
						widget->stat.scrolled.child_y0
							= widget->stat.scrolled.vadj->stat.adj.value;
						widget->stat.scrolled.child_sx = sx;
						widget->stat.scrolled.child_sy = sy;

						child->x = x - widget->stat.scrolled.child_x0;
						child->y = y - widget->stat.scrolled.child_y0;

					} else {
						q8gr_draw_scrolled_window(x, y, sx, sy,
												  Q8TK_SHADOW_ETCHED_OUT,
												  sense_widget);
						q8gr_set_screen_mask(x + 1, y + 1, sx - 2, sy - 2);
						widget->stat.scrolled.child_x0
							= widget->stat.scrolled.hadj->stat.adj.value;
						widget->stat.scrolled.child_y0
							= widget->stat.scrolled.vadj->stat.adj.value;
						widget->stat.scrolled.child_sx = sx - 2;
						widget->stat.scrolled.child_sy = sy - 2;

						child->x = x - widget->stat.scrolled.child_x0 + 1;
						child->y = y - widget->stat.scrolled.child_y0 + 1;
					}

					widget_draw(child, FALSE, sensitive);

					q8gr_set_screen_mask(mask_x, mask_y, mask_sx, mask_sy);
				}

				if (widget->stat.scrolled.vscrollbar) {
					a_focus = (focus ||
							   (widget->stat.scrolled.vadj == get_focus_widget()))
							  ? TRUE : FALSE;
					a_widget = ((sensitive) ? widget->stat.scrolled.vadj
								: NULL);
					/* フォーカスリストには入れないほうがいいのでは… */
					widget_focus_list_append(a_widget);
					q8gr_draw_vscale(x + sx, y,
									 &(widget->stat.scrolled.vadj->stat.adj),
									 a_focus,
									 FALSE, 0,
									 adj_size,
									 a_widget);
				}
				if (widget->stat.scrolled.hscrollbar) {
					a_focus = (focus ||
							   (widget->stat.scrolled.hadj == get_focus_widget()))
							  ? TRUE : FALSE;
					a_widget = ((sensitive) ? widget->stat.scrolled.hadj
								: NULL);
					/* フォーカスリストには入れないほうがいいのでは… */
					widget_focus_list_append(a_widget);
					q8gr_draw_hscale(x, y + sy,
									 &(widget->stat.scrolled.hadj->stat.adj),
									 a_focus,
									 FALSE, 0,
									 adj_size,
									 a_widget);
				}
			} else {
				if (widget->stat.scrolled.no_frame) {
					q8gr_clear_area(x, y, widget->sx, widget->sy);
				} else {
					q8gr_draw_window(x, y, widget->sx, widget->sy,
									 Q8TK_SHADOW_ETCHED_OUT,
									 NULL);
				}
			}
			break;

		case Q8TK_TYPE_ENTRY:
			if (widget->stat.entry.editable) {
				widget_focus_list_append(sense_widget);
			}
			if (focus &&
				widget->stat.entry.editable &&
				widget->stat.entry.cursor_pos < 0) {
				q8tk_entry_set_position(widget,
										q8gr_strlen(widget->code, widget->name));
			}

			q8gr_draw_entry(x, y, widget->sx, widget->code, widget->name,
							widget->stat.entry.disp_pos,
							((focus && widget->stat.entry.editable)
							 ? widget->stat.entry.cursor_pos : -1),
							sense_widget);
			break;

		case Q8TK_TYPE_DIALOG:
			Q8tkAssert(FALSE, NULL);
			break;

		case Q8TK_TYPE_FILE_SELECTION:
			Q8tkAssert(FALSE, NULL);
			break;

		default:
			Q8tkAssert(FALSE, "Undefined type");
		}
	}


	/* 自分自身の仲間 (next) が存在すれば、再帰的に処理 */

	if (widget->next) {
		widget = widget->next;
		widget_draw(widget, next_focus, parent_sensitive);
	}
}














/*--------------------------------------------------------------
 * スクリーン画面を作成。
 * q8tk_grab_add() で設定された WINDOW をトップとして、
 * 全ての子ウィジェットの大きさ、位置を計算し、
 * q8gr_tvram[][]に、表示内容を設定する。
 * 同時に、TAB キーを押された時の、フォーカスの変更の
 * 手順も決めておく。
 *--------------------------------------------------------------*/

void    widget_construct(void)
{
	int         i, j, tmp;
	int         win_x, win_y, win_w, win_h;
	Q8tkWidget  *widget;

	q8gr_clear_screen();

	for (i = 0; i < TOTAL_WINDOW_LAYER; i++) {

		widget = window_layer[i];

		if (widget) {
			Q8tkAssert(i >= MIN_WINDOW_LAYER, NULL);
			Q8tkAssert(widget->type == Q8TK_TYPE_WINDOW, NULL);

			if (i == TOOL_LAYER_INDEX) {
				win_x = Q8GR_TVRAM_TOOL_X;
				win_y = Q8GR_TVRAM_TOOL_Y;
				win_w = Q8GR_TVRAM_TOOL_W;
				win_h = Q8GR_TVRAM_TOOL_H;
			} else if (i == STAT_LAYER_INDEX) {
				win_x = Q8GR_TVRAM_STAT_X;
				win_y = Q8GR_TVRAM_STAT_Y;
				win_w = Q8GR_TVRAM_STAT_W;
				win_h = Q8GR_TVRAM_STAT_H;
			} else {
				win_x = Q8GR_TVRAM_USER_X;
				win_y = Q8GR_TVRAM_USER_Y;
				win_w = Q8GR_TVRAM_USER_W;
				win_h = Q8GR_TVRAM_USER_H;
			}
			q8gr_set_screen_mask(win_x, win_y, win_w, win_h);


			for (j = 0; j < 2; j++) {

				q8gr_clear_focus_screen();

				widget_size(widget, &tmp, &tmp, &tmp, &tmp);

				if (widget->stat.window.set_position == FALSE) {
					widget->x = win_x + (win_w - widget->sx) / 2;
					widget->y = win_y + (win_h - widget->sy) / 2;
				} else {
					widget->x = widget->stat.window.x + win_x;
					widget->y = widget->stat.window.y + win_y;
				}

				widget_scrollin_adjust_reset();

				if (i < MAX_WINDOW_LAYER) {
					widget_focus_list_reset();
				}
				widget_draw(widget, FALSE, widget->sensitive);

				if (widget_scrollin_adjust()) {
					/* Redraw! */
				} else {
					break;
				}

			}
		}
	}


	if (BETWEEN(MIN_WINDOW_LAYER, window_layer_level, MAX_WINDOW_LAYER - 1)) {
		if (get_focus_widget() == NULL) {
			q8tk_widget_set_focus(widget_focus_list_get_top());
		}
	}

#if 0
	if (get_drag_widget()) {
		/* none */
	} else {
		Q8tkWidget *w;
		int exist;

		w = q8tk_tab_top_widget;

		if (w) {

			/* TAB TOP を NOTEPAGE 以外に設定 */
			do {
				if (w->type != Q8TK_TYPE_NOTEPAGE) {
					break;
				}
				w = w->tab_next;
			} while (w != q8tk_tab_top_widget);
			q8tk_tab_top_widget = w;

			/* event_widget が実在するかチェック */
			exist = FALSE;
			do {
				if (w == get_event_widget()) {
					exist = TRUE;
					break;
				}
				w = w->tab_next;
			} while (w != q8tk_tab_top_widget);
			if (! exist) {
				/* 実在しなければ NULL にしておく */
				set_event_widget(NULL);
			}
		}
	}
#endif

	if (q8tk_disp_cursor) {
		q8gr_draw_mouse(q8tk_mouse.x / 8, q8tk_mouse.y / 16);
	}


#if 0
	{
		int i, j;
		const char *c;
		void *w;
		for (j = 0; j < Q8GR_TVRAM_H; j++) {
			for (i = 0; i < Q8GR_TVRAM_W; i++) {
				w = q8gr_get_focus_screen(i, j);
				if (w == Q8GR_WIDGET_NONE) {
					c = " ";
				} else if (w == Q8GR_WIDGET_WINDOW) {
					c = "*";
				} else {
					c = debug_type(((Q8tkWidget *)w)->type);
				}
				printf("%c", *c);
			}
			printf("\n");
		}
		printf("\n");
		fflush(0);
	}
#endif

#if 0
	{
		int i;
		for (i = 0; i < MAX_WIDGET_SCROLLIN; i++) {
			Q8tkWidget *w = widget_scrollin[i].widget;
			printf("%d [%s] %p\n", widget_scrollin[i].drawn, (w) ? debug_type(w->type) : "", w);
		}
		printf("--\n");
		fflush(0);
	}
#endif

#if 0
	for (tmp = 0, i = 0; i < MAX_WIDGET; i++) {
		if (widget_table[i]) {
			tmp++;
		}
	}
	printf("[TOTAL WIDGET] %d/%d : ", tmp, MAX_WIDGET);
	for (tmp = 0, i = 0; i < MAX_LIST; i++) {
		if (list_table[i]) {
			tmp++;
		}
	}
	printf("[TOTAL LIST] %d/%d\n", tmp, MAX_LIST);
#endif
}
