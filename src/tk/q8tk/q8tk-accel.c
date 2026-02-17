/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/*---------------------------------------------------------------------------
 * アクセラレータグループ (ACCEL GROUP)
 * アクセラレーターキー   (ACCEL KEY)
 *---------------------------------------------------------------------------
 * ・子は持てないし、表示もできない。
 * ・内部的には、アクセラレータグループが親、アクセラレータキーが子の
 *   形態をとる
 * ・アクセラレータグループをウインドウに関連づけることで、アクセラレータ
 *   キーをそのウインドウにて有効にすることができる。
 * --------------------------------------------------------------------------
 * Q8tkWidget  *q8tk_accel_group_new(void)
 *    アクセラレータグループの生成。
 *
 * void q8tk_accel_group_attach(Q8tkWidget *accel_group,
 *                              Q8tkWidget *window)
 *    アクセラレータグループを、ウインドウ window に関連づける。
 *
 * void q8tk_accel_group_detach(Q8tkWidget *accel_group,
 *                              Q8tkWidget *window)
 *    ウインドウ window に関連づけたアクセラレータグループを切り離す。
 *
 * void q8tk_accel_group_add(Q8tkWidget *accel_group, int accel_key,
 *                          Q8tkWidget *widget, const char *signal)
 *    アクセラレータキーを設定する。
 *    アクセラレータグループ accel_group に、キー accel_key を設定する。
 *    このキーが押下されたら、 ウィジット widget に シグナル signal を送る。
 * --------------------------------------------------------------------------
 * [WINDOW]
 *    ｜
 *    └──【ACCEL GROUP】←→【ACCEL KEY】
 *                                 ↑↓
 *                         ←  【ACCEL KEY】
 *                                 ↑↓
 *                         ←  【ACCEL KEY】
 *                                 ↑↓
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_accel_group_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_ACCEL_GROUP;
	w->sensitive = TRUE;

	return w;
}

void q8tk_accel_group_attach(Q8tkWidget *accel_group,
							 Q8tkWidget *window)
{
	Q8tkAssert(window->type == Q8TK_TYPE_WINDOW, NULL);
	window->stat.window.accel = accel_group;
}

void q8tk_accel_group_detach(Q8tkWidget *accel_group,
							 Q8tkWidget *window)
{
	Q8tkAssert(window->type == Q8TK_TYPE_WINDOW, NULL);

	if (window->stat.window.accel == accel_group) {
		window->stat.window.accel = NULL;
	}
}

void q8tk_accel_group_add(Q8tkWidget *accel_group, int accel_key,
						  Q8tkWidget *widget, const char *signal)
{
	Q8tkWidget *w;
	w = malloc_widget();

	if (accel_group->child) {
		Q8tkWidget *c = accel_group->child;
		while (c->next) {
			c = c->next;
		}
		c->next = w;
		w->prev = c;
		w->next = NULL;
	} else {
		accel_group->child = w;
		w->prev = NULL;
		w->next = NULL;
	}
	w->parent = accel_group;

	w->type = Q8TK_TYPE_ACCEL_KEY;
	w->sensitive = TRUE;
	w->name = (char *)malloc(strlen(signal) + 1);

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, signal);
	w->code = Q8TK_KANJI_ANK;

	w->stat.accel.key    = accel_key;
	w->stat.accel.widget = widget;
}
