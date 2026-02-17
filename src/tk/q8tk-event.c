/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"
#include "alarm.h"


/***********************************************************************
 * イベントのチェック
 ************************************************************************/
/*
 * DRAGGING
 */
static void widget_dragging(Q8tkWidget *focus)
{
	if (focus && focus->event_dragging) {
		(*focus->event_dragging)(focus);
	}
}
/*
 * DRAG OFF
 */
static void widget_drag_off(Q8tkWidget *focus)
{
	if (focus && focus->event_drag_off) {
		(*focus->event_drag_off)(focus);
	}
}

/*
 * MOUSE BUTTON ON
 */
static void widget_button_on(Q8tkWidget *focus)
{
	if (focus == NULL) {
		/* ウィジット以外をクリックしたら、その親ウインドウにイベント通知 */
		focus = window_layer[ window_layer_level ];
	}
	if (focus && focus->event_button_on) {
		(*focus->event_button_on)(focus);
	}
}
/*
 * KEY ON
 */
static void widget_key_on(Q8tkWidget *focus, int key)
{
	if (focus && focus->event_key_on) {
		(*focus->event_key_on)(focus, key);
	}
}



/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/
void q8tk_event_key_on(int code)
{
	Q8tkWidget *w;
	int change = FALSE;
	int back   = FALSE;

	if (alarm_is_lock()) {
		return;
	}


	if (code == Q8TK_KEY_SHIFT) {
		q8tk_now_shift_on = TRUE;
	}

	if (! get_drag_widget()) {
		/* ドラッグ中じゃない */

		w = get_focus_widget();

		switch (code) {
		case Q8TK_KEY_TAB:
			change = TRUE;
			if (q8tk_now_shift_on) {
				back = TRUE;
			} else {
				back = FALSE;
			}
			break;

#ifdef OLD_FOCUS_CHANGE
			/* DO NOTHING */
#else /* 新たなフォーカス移動方式 */
		case Q8TK_KEY_UP:
			if (w->key_up_used == FALSE) {
				change = TRUE;
				back   = TRUE;
			}
			break;
		case Q8TK_KEY_DOWN:
			if (w->key_down_used == FALSE) {
				change = TRUE;
				back   = FALSE;
			}
			break;
		case Q8TK_KEY_LEFT:
			if (w->key_left_used == FALSE) {
				change = TRUE;
				back   = TRUE;
			}
			break;
		case Q8TK_KEY_RIGHT:
			if (w->key_right_used == FALSE) {
				change = TRUE;
				back   = FALSE;
			}
			break;
#endif
		}

		if (change) {
			/* [TAB][↑][↓][←][→] で、前後のウィジットにフォーカスを移す */

			w = widget_focus_list_get_next(w, back);
			q8tk_widget_set_focus(w);

			/* フォーカスを移動したウィジットが、
			 * スクロールドウインドウ内でかつ、スクロールアウト領域なら、
			 * 強制的にスクロールインさせたいので、ここで補正を予約 */
			if (w &&
				w->type != Q8TK_TYPE_ADJUSTMENT &&
				w->type != Q8TK_TYPE_LISTBOX &&
				w->type != Q8TK_TYPE_DIALOG &&
				w->type != Q8TK_TYPE_FILE_SELECTION) {
				widget_scrollin_register(w);
			}

		} else {
			/* 他のキー */

			/* アクセラレーターキーが、設定されていれば処理する */
			if (window_layer_level >= MIN_WINDOW_LAYER) {
				w = window_layer[ window_layer_level ];
				Q8tkAssert(w->type == Q8TK_TYPE_WINDOW, NULL);

				if (w->stat.window.accel) {
					w = (w->stat.window.accel)->child;
					while (w) {
						if (w->stat.accel.key == code) {
							widget_signal_do(w->stat.accel.widget, w->name);
							break;
						}
						w = w->next;
					}
				}
			}

			/* アクセラレーターキーが有効なら、標準の処理はしないほうがいい? */
			widget_key_on(get_focus_widget(), code);
		}
	}
}

void q8tk_event_key_off(int code)
{
	if (code == Q8TK_KEY_SHIFT) {
		q8tk_now_shift_on = FALSE;
	}
}

void q8tk_event_mouse_on(int code)
{
	if (get_construct_flag()) {
		/* ウインドウ破棄直後のクリック回避 */
		return;
	}
	if (alarm_is_lock()) {
		return;
	}


	if (code == Q8TK_BUTTON_L) {
		/* 左ボタン ON */

		if (! get_drag_widget()) {
			/* ドラッグ中じゃない */

			void *focus = q8gr_get_focus_screen(q8tk_mouse.x / 8,
												q8tk_mouse.y / 16);
			if (focus == Q8GR_WIDGET_NONE) {
				widget_button_on(NULL);
			} else if (focus == Q8GR_WIDGET_WINDOW) {
				/* DO NOTHING */ ;
			} else {
				q8tk_widget_set_focus((Q8tkWidget *)focus);
				widget_button_on((Q8tkWidget *)focus);
			}
		}

	} else if (code == Q8TK_BUTTON_U) {
		/* ホイール UP */

		widget_key_on(get_focus_widget(), Q8TK_KEY_PAGE_UP);

	} else if (code == Q8TK_BUTTON_D) {
		/* ホイール DOWN */

		widget_key_on(get_focus_widget(), Q8TK_KEY_PAGE_DOWN);
	}
}

void q8tk_event_mouse_off(int code)
{
	q8tk_mouse.is_touch = FALSE;

	if (code == Q8TK_BUTTON_L) {
		if (get_drag_widget()) {
			/* 左ボタン OFF かつ 只今 ドラッグ中 */
			widget_drag_off(get_drag_widget());
			set_drag_widget(NULL);
		}
	}
}

void q8tk_event_mouse_moved(int x, int y)
{
	int block_moved;

	q8tk_mouse.x_old = q8tk_mouse.x;
	q8tk_mouse.y_old = q8tk_mouse.y;
	q8tk_mouse.x     = x;
	q8tk_mouse.y     = y;

	if (q8tk_mouse.x / 8  != q8tk_mouse.x_old / 8  ||
		q8tk_mouse.y / 16 != q8tk_mouse.y_old / 16) {
		/* マウス 8dot以上 移動 */
		block_moved = TRUE;

		if (q8tk_disp_cursor) {
			set_construct_flag(TRUE);
		}
	} else {
		block_moved = FALSE;
	}

	if (get_drag_widget()) {
		if (block_moved) {
			/* 只今 ドラッグ中 かつ マウス 8dot以上 移動 */
			widget_dragging(get_drag_widget());
		}
	}
}

void q8tk_event_touch_start(void)
{
	q8tk_mouse.is_touch = TRUE;
}

void q8tk_event_touch_stop(void)
{
	q8tk_mouse.is_touch = FALSE;
}

void q8tk_event_quit(void)
{
	q8tk_main_quit();
}
