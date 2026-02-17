/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"


/***********************************************************************
 * モーダルの設定
 ************************************************************************/
/*--------------------------------------------------------------
 * モーダル設定
 *--------------------------------------------------------------*/
void q8tk_grab_add(Q8tkWidget *widget)
{
	int i;
	Q8tkAssert(widget->type == Q8TK_TYPE_WINDOW, "grab add not window");

	switch (widget->stat.window.type) {

	case Q8TK_WINDOW_TOOL:
		if (window_layer[TOOL_LAYER_INDEX] == NULL) {
			window_layer[TOOL_LAYER_INDEX] = widget;
			set_construct_flag(TRUE);
			return;
		}
		break;

	case Q8TK_WINDOW_STAT:
		if (window_layer[STAT_LAYER_INDEX] == NULL) {
			window_layer[STAT_LAYER_INDEX] = widget;
			set_construct_flag(TRUE);
			return;
		}
		break;

	default:
		for (i = MIN_WINDOW_LAYER; i < MAX_WINDOW_LAYER; i++) {
			if (window_layer[i] == NULL) {
				window_layer[i] = widget;
				window_layer_level = i;
				set_construct_flag(TRUE);
				return;
			}
		}
	}
	Q8tkAssert(FALSE, "overflow window level");
}
/*--------------------------------------------------------------
 * モーダル解除
 *--------------------------------------------------------------*/
void q8tk_grab_remove(Q8tkWidget *widget)
{
	int i;

	if (window_layer[TOOL_LAYER_INDEX] == widget) {
		window_layer[TOOL_LAYER_INDEX] = NULL;
		set_construct_flag(TRUE);
		return;
	}
	if (window_layer[STAT_LAYER_INDEX] == widget) {
		window_layer[STAT_LAYER_INDEX] = NULL;
		set_construct_flag(TRUE);
		return;
	}

	for (i = MIN_WINDOW_LAYER; i < MAX_WINDOW_LAYER; i++) {
		if (window_layer[i] == widget) {
			break;
		}
	}
	Q8tkAssert(i < MAX_WINDOW_LAYER, "grab remove not widget");
	for (; i < MAX_WINDOW_LAYER - 1; i++) {
		window_layer[i] = window_layer[i + 1];
		focus_widget[i] = focus_widget[i + 1];
	}
	window_layer[i] = NULL;
	focus_widget[i] = NULL;
	window_layer_level --;
	set_construct_flag(TRUE);
}
