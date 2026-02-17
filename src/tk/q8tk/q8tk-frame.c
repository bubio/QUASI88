/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/*---------------------------------------------------------------------------
 * フレーム (FRAME)
 *---------------------------------------------------------------------------
 * ・子をひとつもてる。
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・(見出しの)文字列を保持できる。
 * ・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget  *q8tk_frame_new(const char *label)
 *    文字列 label を見出しにもつフレームの生成。
 * void q8tk_frame_set_shadow_type(Q8tkWidget *frame, int shadow_type)
 *    フレームの種類を設定できる。
 *        Q8TK_SHADOW_NONE          枠線のないフレーム
 *        Q8TK_SHADOW_IN            全体がへこんだフレーム
 *        Q8TK_SHADOW_OUT           全体が盛り上がったフレーム
 *        Q8TK_SHADOW_ETCHED_IN     枠がへこんだフレーム
 *        Q8TK_SHADOW_ETCHED_OUT    枠が盛り上がったフレーム
 * --------------------------------------------------------------------------
 * 【FRAME】 ←→ [xxxx]    いろんな子が持てる
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_frame_new(const char *label)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_FRAME;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->stat.frame.shadow_type = Q8TK_SHADOW_OUT;

	w->name = (char *)malloc(strlen(label) + 1);

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, label);
	w->code = q8tk_kanji_code;

	return w;
}

void q8tk_frame_set_shadow_type(Q8tkWidget *frame, int shadow_type)
{
	frame->stat.frame.shadow_type = shadow_type;
}




/*---------------------------------------------------------------------------
 * 水平ボックス (HBOX)
 *---------------------------------------------------------------------------
 * ・複数の子をもてる。
 * ・子を持つには、q8tk_box_pack_start() / _end ()を使用する。
 * ・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_hbox_new(void)
 *    水平ボックスの生成。
 * --------------------------------------------------------------------------
 * 【HBOX】←→ [xxxx]    いろんな子が持てる
 *               ↑↓
 *         ←   [xxxx]
 *               ↑↓
 *         ←   [xxxx]
 *               ↑↓
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_hbox_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_HBOX;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	return w;
}




/*---------------------------------------------------------------------------
 * 垂直ボックス (VBOX)
 *---------------------------------------------------------------------------
 * ・複数の子をもてる。
 * ・子を持つには、q8tk_box_pack_start() / _end ()を使用する。
 * ・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_vbox_new(void)
 *    垂直ボックスの生成。
 * --------------------------------------------------------------------------
 * 【VBOX】←→ [xxxx]    いろんな子が持てる
 *               ↑↓
 *         ←   [xxxx]
 *               ↑↓
 *         ←   [xxxx]
 *               ↑↓
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_vbox_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_VBOX;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	return w;
}
