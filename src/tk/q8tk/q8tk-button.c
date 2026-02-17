/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"
#include "alarm.h"


/*---------------------------------------------------------------------------
 * ボタン (BUTTON)
 *---------------------------------------------------------------------------
 * ・マウスなどでボタンを押している間は引っ込んで、マウスを離すと戻る。
 * ・子を一つ持てる。 (但し、ラベル以外の子を持った場合の動作は未保証)
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・シグナル
 *   "clicked"    ボタンが押された時に発生
 *  -------------------------------------------------------------------------
 * Q8tkWidget *q8tk_button_new(void)
 *    ボタンの生成。
 *
 * Q8tkWidget  *q8tk_button_new_with_label(const char *label)
 *    文字列 label が描かれたボタンの生成。
 *    内部的には、ボタンとラベルを作り、親子にする。
 *
 * Q8tkWidget *q8tk_button_new_from_bitmap(Q8tkWidget *bmp_release,
 *                                         Q8tkWidget *bmp_press,
 *                                         Q8tkWidget *bmp_inactive)
 *    ビットマップボタンの生成。
 *    ラベルの代わりにビットマップが描かれたボタンの生成するのではなく、
 *    指定したビットマップ画像を、ボタンとして生成する。
 *    解放・押下・無効状態の画像のビットマップウィジットを指定するが、
 *    画像サイズは同じでないといけない。同じウィジットを指定してもよい。
 *    内部的には、ダミーのビットマップウィジット作り、親子にする。
 *    実際に表示する時に、指定したウィジットから画像データを取り出して
 *    ダミーのビットマップウィジットに反映する (ウィジットの表示状態は
 *    関係しない)。そのため、指定したウィジットは削除してはいけない。
 *
 * void q8tk_button_set_bitmap(Q8tkWidget *button,
 *                             Q8tkWidget *bmp_release,
 *                             Q8tkWidget *bmp_press,
 *                             Q8tkWidget *bmp_inactive)
 *    ビットマップボタンのビットマップ画像を変更する。
 * --------------------------------------------------------------------------
 * 【BUTTON】←→ [LABEL]    ラベルを子に持つ
 *
 *---------------------------------------------------------------------------*/
#define VISUAL_EFFECT
#ifdef VISUAL_EFFECT
static void cb_button_event_key_on(Q8tkWidget *widget, UNUSED_PARM)
{
	widget->stat.button.active = Q8TK_BUTTON_OFF;
	widget_signal_do(widget, "clicked");
	set_construct_flag(TRUE);
}
#endif
/* リターン・スペースを押したら、シグナル発生 */
static void button_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {

#ifndef	VISUAL_EFFECT /* このままだと、凹んだ状態を描画する暇がない */
		widget_signal_do(widget, "clicked");

#else                 /* 凹んだ状態を描画させたいので、イベントを遅らせる */
		widget->stat.button.active = Q8TK_BUTTON_ON;
		set_construct_flag(TRUE);
		alarm_remove(widget);
		alarm_add(widget, NULL,
				  (alarm_callback_func) cb_button_event_key_on,
				  50, TRUE);
#endif
	}
}

/* ボタンをマウスクリックしたら、ボタンを凹ませて、ドラッグ開始とする */
static void button_event_button_on(Q8tkWidget *widget)
{
	set_drag_widget(widget);

	widget->stat.button.active = Q8TK_BUTTON_ON;
	set_construct_flag(TRUE);
}

/* ドラッグ中は、マウスの位置に応じて、ボタンを凸凹させる */
static void button_event_dragging(Q8tkWidget *widget)
{
	if (q8gr_get_focus_screen(q8tk_mouse.x / 8, q8tk_mouse.y / 16) == widget) {
		if (widget->stat.button.active == Q8TK_BUTTON_OFF) {
			widget->stat.button.active = Q8TK_BUTTON_ON;
			set_construct_flag(TRUE);
		}
	} else {
		if (widget->stat.button.active == Q8TK_BUTTON_ON) {
			widget->stat.button.active = Q8TK_BUTTON_OFF;
			set_construct_flag(TRUE);
		}
	}
}

/* ドラッグ終了で、ボタンが凹んでたら、シグナル発生 */
static void button_event_drag_off(Q8tkWidget *widget)
{
	if (widget->stat.button.active == Q8TK_BUTTON_ON) {

#ifndef	VISUAL_EFFECT /* マウス連打での、凹凸の連続表示がちょっと見苦しい */
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		widget_signal_do(widget, "clicked");
		set_construct_flag(TRUE);

#else                 /* イベントを遅らせることで連続表示しないようにする */
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		set_construct_flag(TRUE);
		alarm_remove(widget);
		alarm_add(widget, NULL,
				  (alarm_callback_func) cb_button_event_key_on,
				  50, TRUE);
#endif
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_button_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_BUTTON;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->event_button_on = button_event_button_on;
	w->event_key_on    = button_event_key_on;
	w->event_dragging  = button_event_dragging;
	w->event_drag_off  = button_event_drag_off;

	return w;
}

Q8tkWidget *q8tk_button_new_with_label(const char *label)
{
	Q8tkWidget *b = q8tk_button_new();
	Q8tkWidget *l = q8tk_label_new(label);

	q8tk_widget_show(l);
	q8tk_container_add(b, l);

	b->with_label = TRUE;

	return b;
}

static void button_new_from_bitmap(Q8tkWidget *button,
								   Q8tkWidget *bmp_release,
								   Q8tkWidget *bmp_press,
								   Q8tkWidget *bmp_inactive);
Q8tkWidget *q8tk_button_new_from_bitmap(Q8tkWidget *bmp_release,
										Q8tkWidget *bmp_press,
										Q8tkWidget *bmp_inactive)
{
	Q8tkWidget *b = q8tk_button_new();

	button_new_from_bitmap(b,
						   bmp_release,
						   bmp_press,
						   bmp_inactive);

	return b;
}
static void button_new_from_bitmap(Q8tkWidget *button,
								   Q8tkWidget *bmp_release,
								   Q8tkWidget *bmp_press,
								   Q8tkWidget *bmp_inactive)
{
	Q8tkWidget *p;

	Q8tkAssert(bmp_release->type == Q8TK_TYPE_BMP, NULL);
	Q8tkAssert(bmp_press->type == Q8TK_TYPE_BMP, NULL);
	Q8tkAssert(bmp_inactive->type == Q8TK_TYPE_BMP, NULL);
	Q8tkAssert(bmp_release->stat.bmp.width == bmp_press->stat.bmp.width, NULL);
	Q8tkAssert(bmp_release->stat.bmp.width == bmp_inactive->stat.bmp.width, NULL);

	/* 同じ画像サイズの、空のビットマップウィジット作成してそれを子にする
	 * (ボタン押下に応じて、画像データを子ウィジットに転送する) */
	p = q8tk_bitmap_new(bmp_release->stat.bmp.x_size,
						bmp_release->stat.bmp.y_size, NULL);

	q8tk_widget_show(p);
	q8tk_container_add(button, p);

	button->stat.button.from_bitmap = TRUE;
	button->stat.button.release = bmp_release;
	button->stat.button.press = bmp_press;
	button->stat.button.inactive = bmp_inactive;
}

void q8tk_button_set_bitmap(Q8tkWidget *button,
							Q8tkWidget *bmp_release,
							Q8tkWidget *bmp_press,
							Q8tkWidget *bmp_inactive)
{
	Q8tkAssert((button->stat.button.from_bitmap), NULL);

	button->stat.button.release = bmp_release;
	button->stat.button.press = bmp_press;
	button->stat.button.inactive = bmp_inactive;

	set_construct_flag(TRUE);
}




/*---------------------------------------------------------------------------
 * トグルボタン (TOGGLE BUTTON)
 * トグルスイッチ (SWITCH)
 *---------------------------------------------------------------------------
 * ・マウスなどでボタンを押すと引っ込む。もう一度ボタンを押すと戻る。
 * ・子を一つ持てる。 (但し、ラベル以外の子を持った場合の動作は未保証)
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・シグナル
 *   "clicked"    ボタンを押して引っ込んだ時に発生
 *   "toggled"    ボタンの状態が変化した時に発生
 *  -------------------------------------------------------------------------
 * Q8tkWidget *q8tk_toggle_button_new(void)
 *    トグルボタンの生成。
 *
 * Q8tkWidget *q8tk_toggle_button_new_with_label(const char *label)
 *    文字列 label が描かれたグルボタンの生成。
 *    内部的には、トグルボタンとラベルを作り、親子にする。
 *
 * Q8tkWidget *q8tk_toggle_button_new_from_bitmap(Q8tkWidget *bmp_release,
 *                                                Q8tkWidget *bmp_press,
 *                                                Q8tkWidget *bmp_inactive)
 *    ビットマップトグルボタンの生成。
 *    ラベルの代わりにビットマップが描かれたボタンの生成するのではなく、
 *    指定したビットマップ画像を、ボタンとして生成する。
 *    解放・押下・無効状態の画像のビットマップウィジットを指定するが、
 *    画像サイズは同じでないといけない。同じウィジットを指定してもよい。
 *    内部的には、ダミーのビットマップウィジット作り、親子にする。
 *    実際に表示する時に、指定したウィジットから画像データを取り出して
 *    ダミーのビットマップウィジットに反映する (ウィジットの表示状態は
 *    関係しない)。そのため、指定したウィジットは削除してはいけない。
 *
 * void q8tk_toggle_button_set_state(Q8tkWidget *widget, int status)
 *    ボタンの状態を変更する。
 *    status が真ならボタンを押した状態にする (シグナルが発生する)。
 *    status が偽ならなにもしない。
 *
 * void q8tk_toggle_button_set_bitmap(Q8tkWidget *button,
 *                                    Q8tkWidget *bmp_release,
 *                                    Q8tkWidget *bmp_press,
 *                                    Q8tkWidget *bmp_inactive)
 *    ビットマップトグルボタンのビットマップ画像を変更する。
 *
 * Q8tkWidget *q8tk_switch_new(void)
 *    トグルスイッチの生成。
 *    外見は異なるが、機能はトグルボタンである。
 *    トグルボタンと違って、子(ラベルや画像)は持てない。
 * --------------------------------------------------------------------------
 * 【TOGGLE BUTTON】←→ [LABEL]    ラベルを子に持つ
 *
 *---------------------------------------------------------------------------*/
/* ボタンをマウスクリックしたら、シグナル発生 */
static void toggle_button_event_button_on(Q8tkWidget *widget)
{
	if (widget->stat.button.active == Q8TK_BUTTON_ON) {
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		widget_signal_do(widget, "toggled");
	} else {
		widget->stat.button.active = Q8TK_BUTTON_ON;
		widget_signal_do(widget, "clicked");
		widget_signal_do(widget, "toggled");
	}
	set_construct_flag(TRUE);
}

/* リターン・スペースを押したら、マウスクリックと同じ処理をする */
static void toggle_button_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		toggle_button_event_button_on(widget);
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_toggle_button_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_TOGGLE_BUTTON;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->event_button_on = toggle_button_event_button_on;
	w->event_key_on    = toggle_button_event_key_on;

	return w;
}

Q8tkWidget *q8tk_toggle_button_new_with_label(const char *label)
{
	Q8tkWidget *b = q8tk_toggle_button_new();
	Q8tkWidget *l = q8tk_label_new(label);

	q8tk_widget_show(l);
	q8tk_container_add(b, l);

	b->with_label = TRUE;

	return b;
}

Q8tkWidget *q8tk_toggle_button_new_from_bitmap(Q8tkWidget *bmp_release,
											   Q8tkWidget *bmp_press,
											   Q8tkWidget *bmp_inactive)
{
	Q8tkWidget *b = q8tk_toggle_button_new();

	button_new_from_bitmap(b,
						   bmp_release,
						   bmp_press,
						   bmp_inactive);

	return b;
}

void q8tk_toggle_button_set_state(Q8tkWidget *widget, int status)
{
	if (status) {
		if (widget->event_key_on) {
			(*widget->event_key_on)(widget, Q8TK_KEY_SPACE);
		}
	} else {
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		set_construct_flag(TRUE);
	}
}

void q8tk_toggle_button_set_bitmap(Q8tkWidget *button,
								   Q8tkWidget *bmp_release,
								   Q8tkWidget *bmp_press,
								   Q8tkWidget *bmp_inactive)
{
	Q8tkAssert((button->stat.button.from_bitmap), NULL);

	button->stat.button.release = bmp_release;
	button->stat.button.press = bmp_press;
	button->stat.button.inactive = bmp_inactive;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* ボタンをマウスクリックしたら、シグナル発生 */
static void cb_switch_button_event_button_on(Q8tkWidget *widget, UNUSED_PARM)
{
	alarm_remove(widget);

	widget->stat.button.timer += 25;
	if (widget->stat.button.timer < 100) {
		alarm_add(widget, NULL,
				  (alarm_callback_func) cb_switch_button_event_button_on,
				  50, TRUE);
		set_construct_flag(TRUE);
		return;
	}

	widget->stat.button.timer = 0;

	if (widget->stat.button.active == Q8TK_BUTTON_ON) {
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		widget_signal_do(widget, "toggled");
	} else {
		widget->stat.button.active = Q8TK_BUTTON_ON;
		widget_signal_do(widget, "clicked");
		widget_signal_do(widget, "toggled");
	}
	set_construct_flag(TRUE);
}

static void switch_button_event_button_on(Q8tkWidget *widget)
{
	cb_switch_button_event_button_on(widget, NULL);
}

/* リターン・スペースを押したら、マウスクリックと同じ処理をする */
static void switch_button_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		switch_button_event_button_on(widget);
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_switch_new(void)
{
	Q8tkWidget *w = q8tk_toggle_button_new();

	w->type = Q8TK_TYPE_SWITCH;
	w->attr = 0;

	w->event_button_on = switch_button_event_button_on;
	w->event_key_on    = switch_button_event_key_on;

	w->stat.button.timer = 0;

	return w;
}




/*---------------------------------------------------------------------------
 * チェックボタン (CHECK BUTTON)
 *---------------------------------------------------------------------------
 * ・ボタンを押すと、チェックボックスが塗りつぶされる。
 *   もう一度ボタンを押すと戻る。
 * ・子を一つ持てる。 (但し、ラベル以外の子を持った場合の動作は未保証)
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・シグナル
 *   "clicked"    ボタンが押された時に発生
 *   "toggled"    ボタンの状態が変化した時に発生
 *                現在OFF状態の場合は、 "toggled" が発生
 *                現在 ON状態の場合は、 "clicked" → "toggled" の順に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_check_button_new(void)
 *    チェックボタンの生成。
 *
 * Q8tkWidget *q8tk_check_button_new_with_label(const char *label)
 *    文字列 label が描かれたチェックボタンの生成。
 *    内部的には、チェックボタンとラベルを作り、親子にする。
 * --------------------------------------------------------------------------
 * 見た目は違うが、機能的にはトグルボタンと同じである。
 * トグルボタンと同様、q8tk_toggle_button_set_state() により、
 * ボタンを押した状態にすることができる (シグナルが発生する)。
 * --------------------------------------------------------------------------
 * 【CHECK BUTTON】←→ [LABEL]    ラベルを子に持つ
 *
 *---------------------------------------------------------------------------*/
static void check_button_event_do(Q8tkWidget *widget)
{
	if (widget->stat.button.active == Q8TK_BUTTON_ON) {
		widget->stat.button.active = Q8TK_BUTTON_OFF;
		widget_signal_do(widget, "toggled");
	} else {
		widget->stat.button.active = Q8TK_BUTTON_ON;
		widget_signal_do(widget, "clicked");
		widget_signal_do(widget, "toggled");
	}
	set_construct_flag(TRUE);
}

/* リターン・スペースを押したら、シグナル発生 */
static void check_button_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		check_button_event_do(widget);
	}
}

/* ボタンをマウスクリックしたら、ドラッグ開始とする */
static void check_button_event_button_on(Q8tkWidget *widget)
{
	set_drag_widget(widget);
	set_construct_flag(TRUE);
}

/* ドラッグ終了で、シグナル発生 (ドラッグ中は、なにもしない) */
static void check_button_event_drag_off(Q8tkWidget *widget)
{
	if (q8gr_get_focus_screen(q8tk_mouse.x / 8, q8tk_mouse.y / 16) == widget) {
		check_button_event_do(widget);
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_check_button_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_CHECK_BUTTON;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->event_button_on = check_button_event_button_on;
	w->event_key_on    = check_button_event_key_on;
	w->event_drag_off  = check_button_event_drag_off;

	return w;
}

Q8tkWidget *q8tk_check_button_new_with_label(const char *label)
{
	Q8tkWidget *b = q8tk_check_button_new();
	Q8tkWidget *l = q8tk_label_new(label);

	q8tk_widget_show(l);
	q8tk_container_add(b, l);

	b->with_label = TRUE;

	return b;
}




/*---------------------------------------------------------------------------
 * ラジオボタン (RADIO BUTTON)
 * ラジオプッシュボタン  (RADIOPUSH BUTTON)
 *---------------------------------------------------------------------------
 * ・いくつかのラジオボタン同士でグルーピングできる。
 * ・ボタンを押すとチェックされるが、同じグルーピングされた
 *   他のラジオボタンは、チェックが外れる。
 * ・子を一つ持てる。 (但し、ラベル以外の子を持った場合の動作は未保証)
 * ・子を持つには、q8tk_container_add() を使用する。
 * ・シグナル
 *   "clicked"    ボタンが押された時に発生
 *   "toggled"    ボタンの状態が変化した時に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget  *q8tk_radio_button_new(Q8tkWidget *group)
 *    ラジオボタンの生成。
 *    同じグループとするラジオボタンを引数で与える。(最初のボタンなら NULL)
 *    最初のボタンがチェックされる。次以降のボタンはチェックが外れる。
 *    以下は、3個のラジオボタンを生成する例
 *        button[0] = q8tk_radio_button_new(NULL);
 *        button[1] = q8tk_radio_button_new(button[0]);
 *        button[2] = q8tk_radio_button_new(button[1]);
 *
 * Q8tkWidget *q8tk_radio_button_new_with_label(Q8List *list,
 *                                              const char *label)
 *    文字列 label が描かれたラジオボタンの生成。
 *    内部的には、ラジオボタンとラベルを作り、親子にする。
 *
 * Q8tkWidget *q8tk_radio_push_button_new(Q8tkWidget *group)
 * Q8tkWidget *q8tk_radio_push_button_new_with_label(Q8List *list,
 *                                                   const char *label)
 *    ラジオプッシュボタンの生成。
 *    外見がプッシュボタンなだけで、機能はラジオボタンである。
 * --------------------------------------------------------------------------
 * グルーピングされているが、機能的にはトグルボタンと同じである。
 * トグルボタンと同様、q8tk_toggle_button_set_state() により、
 * ボタンを押した状態にすることができる。なお、この時グルーピングされた
 * すべてのラジオボタンに "toggled" シグナルが発生する。
 * --------------------------------------------------------------------------
 * 【RADIO BUTTON】←→ [LABEL]            ラベルを子に持つ
 *         ↑                              LIST 先頭を内部で保持
 *         └──────────  LIST
 *                                 ↑｜
 * 【RADIO BUTTON】←→ [LABEL]    ｜｜
 *         ↑                      ｜↓
 *         └──────────  LIST
 *
 * 各ラジオボタンは独立したウィジットであるが、リスト情報によりグループを
 * なしている。各ラジオボタンは、このリストの先頭を内部で保持している。
 * ボタンを押すとグループを生成している全ウィジットにシグナルが発生する。
 *
 *---------------------------------------------------------------------------*/
static void radio_button_event_do(Q8tkWidget *widget)
{
	Q8List *list;

	/* 自分自身は clicked シグナルを発生 */
	widget_signal_do(widget, "clicked");
	if (widget->stat.button.active == Q8TK_BUTTON_ON) {
		return;
	}

	/* リストをたどって自分以外のボタンをオフにし、 toggled シグナル発生 */
	list = widget->stat.button.list;
	while (list) {
		if (list->data != widget) {
			((Q8tkWidget *)(list->data))->stat.button.active = Q8TK_BUTTON_OFF;
			widget_signal_do((Q8tkWidget *)(list->data), "toggled");
		}
		list = list->next;
	}
	/* 自身もボタン状態変化なら toggled シグナル発生 */
	widget->stat.button.active = Q8TK_BUTTON_ON;

	widget_signal_do(widget, "toggled");

	set_construct_flag(TRUE);
}

/* リターン・スペースを押したら、シグナル発生 */
/* ↑←/↓→キーを押したら、前/後のラジオボタンに、フォーカス移動 */
static void radio_button_event_key_on(Q8tkWidget *widget, int key)
{
	if (key == Q8TK_KEY_RET || key == Q8TK_KEY_SPACE) {
		radio_button_event_do(widget);
	}

#ifdef  OLD_FOCUS_CHANGE
	/* DO NOTHING */
#else /* 新たなフォーカス移動方式 */
	if (key == Q8TK_KEY_LEFT  ||
		key == Q8TK_KEY_UP    ||
		key == Q8TK_KEY_RIGHT ||
		key == Q8TK_KEY_DOWN) {

#if 0
		/* OFF状態でフォーカスありの場合、カーソルキーでボタン押下とみなす */
		if (widget->stat.button.active == FALSE) {
			radio_button_event_do(widget);
		} else
#endif
		{
			Q8List *list = q8_list_find(widget->stat.button.list, widget);

			if (list) {
				if (key == Q8TK_KEY_LEFT ||
					key == Q8TK_KEY_UP) {
					if (list->prev == NULL) {
						list = q8_list_last(list);
					} else {
						list = list->prev;
					}
				} else {
					if (list->next == NULL) {
						list = q8_list_first(list);
					} else {
						list = list->next;
					}
				}
				if (list) {
					q8tk_widget_set_focus((Q8tkWidget *)(list->data));
#if 0
					/* ↑←/↓→キーを押したら、
					 * 前/後のラジオボタンを押下とみなす */
					radio_button_event_do((Q8tkWidget *)(list->data));
#endif
				}
			}
		}
	}
#endif
}

/* ボタンをマウスクリックしたら、ドラッグ開始とする */
static void radio_button_event_button_on(Q8tkWidget *widget)
{
	set_drag_widget(widget);
	set_construct_flag(TRUE);
}

/* ドラッグ終了で、シグナル発生 (ドラッグ中は、なにもしない) */
static void radio_button_event_drag_off(Q8tkWidget *widget)
{
	if (q8gr_get_focus_screen(q8tk_mouse.x / 8, q8tk_mouse.y / 16) == widget) {
		radio_button_event_do(widget);
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_radio_button_new(Q8tkWidget *group)
{
	Q8tkWidget *w;
	Q8List *list;

	w = malloc_widget();
	w->type = Q8TK_TYPE_RADIO_BUTTON;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->key_up_used    = TRUE;
	w->key_down_used  = TRUE;
	w->key_left_used  = TRUE;
	w->key_right_used = TRUE;

	w->event_button_on = radio_button_event_button_on;
	w->event_key_on    = radio_button_event_key_on;
	w->event_drag_off  = radio_button_event_drag_off;

	if (group == NULL) {
		/* 最初の1個目 */
		list = NULL;
		w->stat.button.active = TRUE;
	} else {
		/* 2個目以降   */
		list = group->stat.button.list;
		w->stat.button.active = FALSE;
	}
	w->stat.button.list = q8_list_append(list, w);

	return w;
}

Q8tkWidget *q8tk_radio_button_new_with_label(Q8tkWidget *group,
											 const char *label)
{
	Q8tkWidget *b = q8tk_radio_button_new(group);
	Q8tkWidget *l = q8tk_label_new(label);

	q8tk_widget_show(l);
	q8tk_container_add(b, l);

	b->with_label = TRUE;

	return b;
}

Q8List *q8tk_radio_button_get_list(Q8tkWidget *group)
{
	return group->stat.button.list;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Q8tkWidget *q8tk_radio_push_button_new(Q8tkWidget *group)
{
	Q8tkWidget *w = q8tk_radio_button_new(group);

	w->type = Q8TK_TYPE_RADIOPUSH_BUTTON;

	return w;
}

Q8tkWidget *q8tk_radio_push_button_new_with_label(Q8tkWidget *group,
												  const char *label)
{
	Q8tkWidget *b = q8tk_radio_button_new_with_label(group, label);

	b->type = Q8TK_TYPE_RADIOPUSH_BUTTON;

	return b;
}
