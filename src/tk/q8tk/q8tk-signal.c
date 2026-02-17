/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/***********************************************************************
 * シグナル関係
 ************************************************************************/
/*---------------------------------------------------------------------------
 * シグナルの設定・削除
 *---------------------------------------------------------------------------
 * ・ウィジットのシグナルに、コールバック関数を設定する。
 *   各ウィジットごとに発生するシグナルが決まっているので、そのシグナルが
 *   発生した時に呼び出される関数を設定する。
 * ・ここで設定する関数内部で自分自身のウィジットを削除するようなことは
 *   してはいけない。内部で誤動作する可能性がある。
 *    (ただし、ボタンウィジットに限り、可能とする。なので、ダイアログや
 *     ファイルセレクションのボタンにて、自身を削除するのは大丈夫なはず)
 *
 * --------------------------------------------------------------------------
 * int q8tk_signal_connect(Q8tkWidget *widget, const char *name,
 *                         Q8tkSignalFunc func, void *func_data)
 *    widget に name で示されるシグナルに対するコールバック関数 func を
 *    設定する。シグナル発生時には、関数 func が 引数 widget, func_data を
 *    伴って呼び出される。
 *    なお、戻り値は常に 0 になる。
 *
 * void q8tk_signal_handlers_destroy(Q8tkWidget *widget)
 *    widget に設定された全てのシグナルのコールバック関数を無効にする。
 *
 *---------------------------------------------------------------------------*/
void q8tk_signal_handlers_destroy(Q8tkWidget *widget)
{
	widget->user_event_0 = NULL;
	widget->user_event_1 = NULL;
}

int q8tk_signal_connect(Q8tkWidget *widget, const char *name,
						Q8tkSignalFunc func, void *func_data)
{
	switch (widget->type) {

	case Q8TK_TYPE_WINDOW:
		if (strcmp(name, "inactivate") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_BUTTON:
		if (strcmp(name, "clicked") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_TOGGLE_BUTTON:
	case Q8TK_TYPE_CHECK_BUTTON:
	case Q8TK_TYPE_RADIO_BUTTON:
	case Q8TK_TYPE_RADIOPUSH_BUTTON:
	case Q8TK_TYPE_SWITCH:
		if (strcmp(name, "clicked") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		} else if (strcmp(name, "toggled") == 0) {
			widget->user_event_1      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_1_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_NOTEBOOK:
		if (strcmp(name, "switch_page") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_COMBO:
		if (strcmp(name, "activate") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		} else if (strcmp(name, "changed") == 0) {
			widget->user_event_1      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_1_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_LISTBOX:
		if (strcmp(name, "selection_changed") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_LIST_ITEM:
		if (strcmp(name, "select") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_ADJUSTMENT:
		if (strcmp(name, "value_changed") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		}
		break;

	case Q8TK_TYPE_ENTRY:
		if (strcmp(name, "activate") == 0) {
			widget->user_event_0      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_0_parm = func_data;
			return 0;
		} else if (strcmp(name, "changed") == 0) {
			widget->user_event_1      = (void (*)(Q8tkWidget *, void *))func;
			widget->user_event_1_parm = func_data;
			return 0;
		}
		break;

	}

	fprintf(stderr,
			"Undefined signal %s '%s'\n", debug_type(widget->type), name);
	Q8tkAssert(FALSE, NULL);
	return 0;
}
