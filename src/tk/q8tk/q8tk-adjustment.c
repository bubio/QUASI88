/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "alarm.h"


/*---------------------------------------------------------------------------
 * アジャストメント (ADJUSTMENT)
 *---------------------------------------------------------------------------
 * ・表示できない
 * ・子は持てない。子になることもできない。
 * ・スケールやスクロールドウインドウを生成する際に、必要となる。
 *   単独では使用することはない。
 * ・レンジ(値の範囲)と増分(小さい増分、大きい増分の2種類) を持つが、
 *   いずれも整数に限る。
 * ・スケールを生成する場合は、前もってアジャストメントを生成しておくのが
 *   おくのが一般的。
 * ・スクロールドウインドウを生成する場合は、その時にアジャストメントを自動
 *   生成させるのが一般的。
 * ・シグナル
 *    "value_changed"    値が変わった時に発生
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_adjustment_new(int value, int lower, int upper,
 *                                 int step_increment, int page_increment)
 *    アジャストメントの生成。
 *    値の範囲は lower .. upper 、現在値は value 、
 *    大きい増分は step_increment 、小さい増分は page_increment 。
 *
 * void q8tk_adjustment_set_value(Q8tkWidget *adj, int value)
 *    アジャストメントの現在値を設定する。シグナルは発生しない。
 *
 * void q8tk_adjustment_clamp_page(Q8tkWidget *adj, int lower, int upper)
 *    値の範囲を lower .. upper に設定しなおす。この時、現在値が範囲外の場合
 *    最大値ないし最小値に設定される。シグナルは発生しない。
 *
 * void q8tk_adjustment_set_increment(Q8tkWidget *adj,
 *                                    int step_increment, int page_increment)
 *    大きい増分と、小さい増分を設定しなおす。
 *
 * void q8tk_adjustment_set_arrow(Q8tkWidget *adj, int arrow)
 *    arrow が真の場合、矢印を表示する。
 *    なお、矢印がクリックされた場合は ± step_increment 分、増減する。
 *    背景部分がクリックされた場合は ± page_increment 分、増減する。
 *
 * void q8tk_adjustment_set_length(Q8tkWidget *adj, int length)
 *
 * void q8tk_adjustment_set_speed(Q8tkWidget *adj,
 *                                int step_speed, int page_speed)
 *
 * void q8tk_adjustment_set_size(Q8tkWidget *adj, int size)
 *
 * --------------------------------------------------------------------------
 * 【ADJUSTMENT】    子は持てないし、子にもなれない
 *
 *---------------------------------------------------------------------------*/

/* マウス位置が、アジャストメントのどの部位にあるかを返す */
static int adjustment_get_controller(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	int zone0, zone1;
	int slider0, slider1, arrow_lo0, arrow_lo1, arrow_hi0, arrow_hi1;
	int m_x = q8tk_mouse.x / 8;
	int m_y = q8tk_mouse.y / 16;
	int adj_size = (adj->size);
	int bar_size = adj_size;
	int arrow_size = adj_size;
	int slider_size = adj_size;

	if (adj->horizontal) {

		zone0 = adj->y;
		zone1 = (zone0) + bar_size - 1;

		if (BETWEEN(zone0, m_y, zone1)) {

			slider0 = adj->x + adj->pos;
			slider1 = (slider0) + slider_size - 1;

			if (adj->arrow) {
				arrow_lo0 = adj->x;
				arrow_lo1 = (arrow_lo0) + arrow_size - 1;
				arrow_hi0 = adj->x + arrow_size + adj->length;
				arrow_hi1 = (arrow_hi0) + arrow_size - 1;

				if (BETWEEN(arrow_lo0, m_x, arrow_lo1)) {
					return ADJUST_CTRL_STEP_DEC;
				} else if (BETWEEN(arrow_hi0, m_x, arrow_hi1)) {
					return ADJUST_CTRL_STEP_INC;
				}

				slider0 += arrow_size;
				slider1 += arrow_size;
			}

			if (BETWEEN(slider0, m_x, slider1)) {
				return ADJUST_CTRL_SLIDER;
			} else if (m_x < slider0) {
				return ADJUST_CTRL_PAGE_DEC;
			} else {
				return ADJUST_CTRL_PAGE_INC;
			}

		} else {
			return ADJUST_CTRL_NONE;
		}

	} else {

		zone0 = adj->x;
		zone1 = (zone0) + bar_size - 1;

		if (BETWEEN(zone0, m_x, zone1)) {
			slider0 = adj->y + adj->pos;
			slider1 = (slider0) + slider_size - 1;

			if (adj->arrow) {
				arrow_lo0 = adj->y;
				arrow_lo1 = (arrow_lo0) + arrow_size - 1;
				arrow_hi0 = adj->y + arrow_size + adj->length;
				arrow_hi1 = (arrow_hi0) + arrow_size - 1;

				if (BETWEEN(arrow_lo0, m_y, arrow_lo1)) {
					return ADJUST_CTRL_STEP_DEC;
				} else if (BETWEEN(arrow_hi0, m_y, arrow_hi1)) {
					return ADJUST_CTRL_STEP_INC;
				}

				slider0 += arrow_size;
				slider1 += arrow_size;
			}

			if (BETWEEN(slider0, m_y, slider1)) {
				return ADJUST_CTRL_SLIDER;
			} else if (m_y < slider0) {
				return ADJUST_CTRL_PAGE_DEC;
			} else {
				return ADJUST_CTRL_PAGE_INC;
			}

		} else {
			return ADJUST_CTRL_NONE;
		}

	}
}

typedef union {
	struct {
		int is_repeat	: 2;
		int speed		: 15;
		int diff		: 15;
	} d;
	int i;
} t_effect_parm;
static void cb_adj_effect(Q8tkWidget *widget, void *parm);
static void cb_adj_repeat(Q8tkWidget *widget, void *parm);

static int is_mouse_button_on(Q8Adjust *adj)
{
	if ((adj->active == ADJUST_CTRL_NONE) ||
		((adj->active & ADJUST_CTRL_BIT_KEY) == ADJUST_CTRL_BIT_KEY)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/* アジャストメントの矢印部・背景部のマウスクリック時動作 */
static void adjustment_do(Q8tkWidget *widget, int is_repeat)
{
	Q8Adjust *adj = &widget->stat.adj;
	int mouse_on = is_mouse_button_on(adj);
	t_effect_parm parm;
	int value, speed, diff;

	switch (adj->active & ADJUST_CTRL_BIT_MASK) {
	case ADJUST_CTRL_STEP_INC:
		value = adj->value + adj->step_increment;
		speed = (adj->step_speed > 0) ? +adj->step_speed : +adj->step_increment;
		break;
	case ADJUST_CTRL_STEP_DEC:
		value = adj->value - adj->step_increment;
		speed = (adj->step_speed > 0) ? -adj->step_speed : -adj->step_increment;
		break;
	case ADJUST_CTRL_PAGE_INC:
		value = adj->value + adj->page_increment;
		speed = (adj->page_speed > 0) ? +adj->page_speed : +adj->page_increment;
		break;
	case ADJUST_CTRL_PAGE_DEC:
		value = adj->value - adj->page_increment;
		speed = (adj->page_speed > 0) ? -adj->page_speed : -adj->page_increment;
		break;
	default:
		return;
	}

	if (value > adj->upper) {
		value = adj->upper;
	}
	if (value < adj->lower) {
		value = adj->lower;
	}

	diff = value - adj->value;

	adj->do_effect = TRUE;

	if (mouse_on) {
		set_drag_widget(widget);
	}

	parm.d.is_repeat = is_repeat;
	parm.d.speed = speed;
	parm.d.diff = diff;
	cb_adj_effect(widget, INT2P(parm.i));
}

/* アジャストメント値変更時の演出処理 (タイマーで少しずつ値を変化させる) */
#define VISUAL_EFFECT
static void cb_adj_effect(Q8tkWidget *widget, void *arg_parm)
{
	Q8Adjust *adj = &widget->stat.adj;
	int mouse_on = is_mouse_button_on(adj);
	t_effect_parm parm;
	int value_old, speed, diff;

	parm.i = P2INT(arg_parm);
	speed = parm.d.speed;
	diff = parm.d.diff;
	value_old = adj->value;

#ifdef VISUAL_EFFECT
	if (ABS(diff) < ABS(speed)) {
		adj->value += diff;
		diff = 0;
	} else {
		adj->value += speed;
		diff -= speed;
	}
#else
	adj->value += diff;
	diff = 0;
#endif
	parm.d.diff = diff;

	if (adj->value != value_old) {
		widget_signal_do(widget, "value_changed");
	}

	if (diff == 0) {
		adj->do_effect = FALSE;

		if (mouse_on) {
			int timeout = (parm.d.is_repeat) ? 50 : 500;
			alarm_remove(widget);
			alarm_add(widget, NULL,
					  (alarm_callback_func) cb_adj_repeat,
					  timeout, FALSE);
		}

		if (adj->value != value_old) {
			/* アジャストメント操作で、
			 * LISTBOXが操作された場合の例外処理フラグON */
			adj->listbox_changed = TRUE;
		}

	} else {
		/* 演出中はイベントは無視したいが、マウスのオフは検知せねば */
		int is_event_lock = TRUE;
		if (mouse_on) {
			is_event_lock = FALSE;
		}
		alarm_remove(widget);
		alarm_add(widget, INT2P(parm.i),
				  (alarm_callback_func) cb_adj_effect,
				  1, is_event_lock);
	}

	set_construct_flag(TRUE);
}

/* 矢印部・背景部をクリックしたままの時の、リピート処理 (タイマーで制御) */
static void cb_adj_repeat(Q8tkWidget *widget, UNUSED_PARM)
{
	Q8Adjust *adj = &widget->stat.adj;

	if (adj->active != ADJUST_CTRL_NONE) {
		int mouse_act = adjustment_get_controller(widget);

		if (adj->active == mouse_act) {
			switch (adj->active) {
			case ADJUST_CTRL_STEP_INC:
			case ADJUST_CTRL_STEP_DEC:
			case ADJUST_CTRL_PAGE_INC:
			case ADJUST_CTRL_PAGE_DEC:
				adjustment_do(widget, TRUE);
				break;
			}
		}
	}
}

/* 矢印部・背景部のクリックを離した時の動作 */
static void adjustment_drag_off(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	adj->active = ADJUST_CTRL_NONE;
	set_drag_widget(NULL);
	if (adj->do_effect) {
		/* EMPTY */
	} else {
		alarm_remove(widget);
		set_construct_flag(TRUE);
	}
}

/* アジャストメントの矢印部・背景部をドラッグした時の動作 */
static void adjustment_dragging_another(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	int mouse_act = adjustment_get_controller(widget);

	if (adj->active == mouse_act) {
		switch (adj->active) {
		case ADJUST_CTRL_STEP_INC:
		case ADJUST_CTRL_STEP_DEC:
		case ADJUST_CTRL_PAGE_INC:
		case ADJUST_CTRL_PAGE_DEC:
			adjustment_do(widget, FALSE);
			break;
		}
	}
}

/* アジャストメントのスライダー部をドラッグした時の動作 */
static void adjustment_dragging_slider(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	int m_x = q8tk_mouse.x / 8;
	int m_y = q8tk_mouse.y / 16;
	int adj_x = adj->x;
	int adj_y = adj->y;
	int slider = -1;
	int adj_size = (adj->size);
	int arrow_size = adj_size;

	if (adj->upper <= adj->lower) {
		return;
	}

	if (adj->horizontal) {
		if (adj->arrow) {
			adj_x += arrow_size;
		}
		if (adj_x <= m_x && m_x < adj_x + adj->length) {
			slider = m_x - adj_x;
		}
	} else {
		if (adj->arrow) {
			adj_y += arrow_size;
		}
		if (adj_y <= m_y && m_y < adj_y + adj->length) {
			slider = m_y - adj_y;
		}
	}

	if (slider >= 0) {
		float val0 = (slider - 1) * adj->scale + adj->lower;
		float val1 = (slider) * adj->scale + adj->lower;
		int   val;

		if (slider <= 0) {
			val = adj->lower;
		} else if (slider >= adj->length - 1) {
			val = adj->upper;
		} else {
			float base = (float)(adj->upper - adj->lower) / (adj->length - 1);
			int   val2 = (int)((int)(val1 / base) * base);
			if (val0 < val2 && val2 <= val1) {
				val =         val2;
			} else {
				val = (int)((val0 + val1) / 2);
			}
		}

		if (adj->value != val) {
			adj->value = val;
			widget_signal_do(widget, "value_changed");
			/* アジャストメント操作で、
			 * LISTBOXが操作された場合の例外処理フラグON */
			adj->listbox_changed = TRUE;
		}
		set_construct_flag(TRUE);
	}
}


/* アジャストメントの上にてマウスクリックしたら、値調整しシグナル発生 */
static void adjustment_event_button_on(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	if (adj->do_effect == FALSE) {
		int act = adjustment_get_controller(widget);

		switch (act) {

		case ADJUST_CTRL_STEP_INC:
		case ADJUST_CTRL_STEP_DEC:
		case ADJUST_CTRL_PAGE_INC:
		case ADJUST_CTRL_PAGE_DEC:
			adj->active = act;
			adjustment_do(widget, FALSE);
			break;

		case ADJUST_CTRL_SLIDER:
			adj->active = act;
			set_drag_widget(widget);
			break;

		default:
			/* EMPTY */
			break;
		}
	}
}

/* アジャストメントの部品をドラッグしたら、値調整しシグナル発生 */
static void adjustment_event_dragging(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;

	if (adj->active == ADJUST_CTRL_SLIDER) {
		adjustment_dragging_slider(widget);
	} else {
		adjustment_dragging_another(widget);
	}
}

/* アジャストメントの部品のドラッグを離したら、処理終了 */
static void adjustment_event_drag_off(Q8tkWidget *widget)
{
	Q8Adjust *adj = &widget->stat.adj;
	adj->active = ADJUST_CTRL_NONE;
	adjustment_drag_off(widget);
}

/* カーソルキーなどを押したら、マウスクリックと同じ処理をする */
static void adjustment_event_key_on(Q8tkWidget *widget, int key)
{
	Q8Adjust *adj = &widget->stat.adj;
	if (adj->do_effect == FALSE) {

		if (adj->horizontal) {
			/* HORIZONTAL ADJUSTMENT */
			if (key == Q8TK_KEY_LEFT) {
				adj->active = ADJUST_CTRL_STEP_DEC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);

			} else if (key == Q8TK_KEY_RIGHT) {
				adj->active = ADJUST_CTRL_STEP_INC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);
			}

		} else {
			/* VIRTICAL ADJUSTMENT */
			if (key == Q8TK_KEY_UP) {
				adj->active = ADJUST_CTRL_STEP_DEC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);

			} else if (key == Q8TK_KEY_DOWN) {
				adj->active = ADJUST_CTRL_STEP_INC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);

			} else if (key == Q8TK_KEY_PAGE_UP) {
				adj->active = ADJUST_CTRL_PAGE_DEC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);

			} else if (key == Q8TK_KEY_PAGE_DOWN) {
				adj->active = ADJUST_CTRL_PAGE_INC | ADJUST_CTRL_BIT_KEY;
				adjustment_do(widget, FALSE);
			}
		}
	}
}

Q8tkWidget *q8tk_adjustment_new(int value, int lower, int upper,
								int step_increment, int page_increment)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_ADJUSTMENT;
	w->sensitive = TRUE;

	w->event_button_on = adjustment_event_button_on;
	w->event_dragging  = adjustment_event_dragging;
	w->event_drag_off  = adjustment_event_drag_off;
	w->event_key_on    = adjustment_event_key_on;

	w->stat.adj.value = value;
	w->stat.adj.lower = lower;
	w->stat.adj.upper = upper;
	w->stat.adj.step_increment = step_increment;
	w->stat.adj.page_increment = page_increment;
	w->stat.adj.step_speed = 0;
	w->stat.adj.page_speed = 0;

	w->stat.adj.size = 1;

	return w;
}

void q8tk_adjustment_set_value(Q8tkWidget *adj, int value)
{
	if (value < adj->stat.adj.lower) {
		value = adj->stat.adj.lower;
	}
	if (value > adj->stat.adj.upper) {
		value = adj->stat.adj.upper;
	}

	if (adj->stat.adj.value != value) {
		adj->stat.adj.value = value;
		/* このアジャストメント操作で、LISTBOXを操作することは無いハズ */
		/* adj->stat.adj.listbox_changed = TRUE; */

		/* シグナルは発生させない */
		/* widget_signal_do(adj, "value_changed"); */

		set_construct_flag(TRUE);
	}
}

void q8tk_adjustment_clamp_page(Q8tkWidget *adj,
								int lower, int upper)
{
	if (upper < lower) {
		upper = lower;
	}

	adj->stat.adj.lower = lower;
	adj->stat.adj.upper = upper;
	if (adj->stat.adj.value < adj->stat.adj.lower) {
		adj->stat.adj.value = adj->stat.adj.lower;
	}
	if (adj->stat.adj.value > adj->stat.adj.upper) {
		adj->stat.adj.value = adj->stat.adj.upper;
	}

	/* シグナルは発生させない */

	set_construct_flag(TRUE);
}

void q8tk_adjustment_set_arrow(Q8tkWidget *adj, int arrow)
{
	if (adj->stat.adj.arrow != arrow) {
		adj->stat.adj.arrow = arrow;
		set_construct_flag(TRUE);
	}
}

void q8tk_adjustment_set_length(Q8tkWidget *adj, int length)
{
	if (adj->stat.adj.max_length != length) {
		adj->stat.adj.max_length = length;
		set_construct_flag(TRUE);
	}
}

void q8tk_adjustment_set_increment(Q8tkWidget *adj,
								   int step_increment,
								   int page_increment)
{
	adj->stat.adj.step_increment = step_increment;
	adj->stat.adj.page_increment = page_increment;
}

void q8tk_adjustment_set_speed(Q8tkWidget *adj,
							   int step_speed, int page_speed)
{
	if (step_speed < 0) {
		adj->stat.adj.step_speed = 0;
	} else {
		adj->stat.adj.step_speed = step_speed;
	}

	if (page_speed < 0) {
		adj->stat.adj.page_speed = 0;
	} else {
		adj->stat.adj.page_speed = page_speed;
	}
}

void q8tk_adjustment_set_size(Q8tkWidget *adj, int size)
{
	int new_size = (size >= 2) ? 2 : 1;

	if (adj->stat.adj.size != new_size) {
		adj->stat.adj.size = new_size;
		set_construct_flag(TRUE);
	}
}



/*---------------------------------------------------------------------------
 * 水平スケール (HSCALE)
 *---------------------------------------------------------------------------
 * ・new()時に、引数でアジャストメントを指定する。
 *   スケールのレンジ(範囲)や増分は、このアジャストメントに依存する。
 * ・new()時の引数が NULL の場合は、自動的にアジャストメントが生成されるが、
 *   この時のレンジは 0..10 、増分は 1 と 2 に固定である。(変更可能)
 * ・子は持てない
 * ・シグナル … なし。
 *   ただし、アジャストメントはシグナルを受ける。
 *   アジャストメントを自動生成した場合、どうやってシグナルをセットする？
 *  -------------------------------------------------------------------------
 * Q8tkWidget *q8tk_hscale_new(Q8tkWidget *adjustment)
 *    水平スケールを生成。
 *    予め作成済みのアジャストメント adjustment を指定する。
 *    NULL なら内部で自動生成される。
 *
 * void q8tk_scale_set_draw_value(Q8tkWidget *scale, int draw_value)
 *    draw_value が真なら、現在値をスライダーの横に表示する。
 *
 * void q8tk_scale_set_value_pos(Q8tkWidget *scale, int pos)
 *     現在値を表示する位置を決める。
 *     Q8TK_POS_LEFT   スライダーの左に表示
 *     Q8TK_POS_RIGHT  スライダーの右に表示
 *     Q8TK_POS_TOP    スライダーの上に表示
 *     Q8TK_POS_BOTTOM スライダーの下に表示
 * --------------------------------------------------------------------------
 * 【HSCALE】                        子は持てない
 *      └──── 【ADJUSTMENT】
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_hscale_new(Q8tkWidget *adjustment)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_HSCALE;
	w->sensitive = TRUE;

	if (adjustment) {
		w->stat.scale.adj = adjustment;
	} else {
		w->stat.scale.adj = q8tk_adjustment_new(0, 0, 10, 1, 2);
		w->with_label = TRUE;
	}

	w->stat.scale.adj->stat.adj.horizontal = TRUE;
	w->stat.scale.adj->parent = w;

	w->stat.scale.adj->key_up_used    = FALSE;
	w->stat.scale.adj->key_down_used  = FALSE;
	w->stat.scale.adj->key_left_used  = TRUE;
	w->stat.scale.adj->key_right_used = TRUE;

	return w;
}




/*---------------------------------------------------------------------------
 * 垂直スケール (HSCALE)
 *---------------------------------------------------------------------------
 * ・new()時に、引数でアジャストメントを指定する。
 *   スケールのレンジ(範囲)や増分は、このアジャストメントに依存する。
 * ・new()時の引数が NULL の場合は、自動的にアジャストメントが生成されるが、
 *   この時のレンジは 0..10 、増分は 1 と 2 に固定である。(変更可能)
 * ・子は持てない
 * ・シグナル … なし。
 *   ただし、アジャストメントはシグナルを受ける。
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_hscale_new(Q8tkWidget *adjustment)
 *    垂直スケールを生成。
 *    予め作成済みのアジャストメント adjustment を指定する。
 *    NULL なら内部で自動生成される。
 *
 * void q8tk_scale_set_draw_value(Q8tkWidget *scale, int draw_value)
 *    水平スケールと同じ。
 *
 * void q8tk_scale_set_value_pos(Q8tkWidget *scale, int pos)
 *    水平スケールと同じ。
 * --------------------------------------------------------------------------
 * 【VSCALE】                        子は持てない
 *      └──── 【ADJUSTMENT】
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_vscale_new(Q8tkWidget *adjustment)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_VSCALE;
	w->sensitive = TRUE;

	w->key_up_used    = TRUE;
	w->key_down_used  = TRUE;

	if (adjustment) {
		w->stat.scale.adj = adjustment;
	} else {
		w->stat.scale.adj = q8tk_adjustment_new(0, 0, 10, 1, 2);
		w->with_label = TRUE;
	}

	w->stat.scale.adj->stat.adj.horizontal = FALSE;
	w->stat.scale.adj->parent = w;

	w->stat.scale.adj->key_up_used    = TRUE;
	w->stat.scale.adj->key_down_used  = TRUE;
	w->stat.scale.adj->key_left_used  = FALSE;
	w->stat.scale.adj->key_right_used = FALSE;

	return w;
}

void q8tk_scale_set_value_pos(Q8tkWidget *scale, int pos)
{
	scale->stat.scale.value_pos = pos;
	set_construct_flag(TRUE);
}

void q8tk_scale_set_draw_value(Q8tkWidget *scale, int draw_value)
{
	scale->stat.scale.draw_value = draw_value;
	set_construct_flag(TRUE);
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * 表示時のサイズ計算 (widget_size()内から呼ばれる)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
extern void adjustment_size(Q8Adjust *adj, int *sx, int *sy)
{
	int adj_size = (adj->size);
	int bar_size = adj_size;
	int arrow_size = adj_size;
	int slider_size = adj_size;
	int range = (adj->upper - adj->lower + 1);

	/* バー長さ length を設定
	 * スライダーが「前中後」の3コマ分は動いてほしいので、最小長さは
	 * スライダーサイズ + 2 にしよう */
	if (adj->max_length < (slider_size + 2) ) {
		/* 指定が小さすぎなので、画面サイズ80%以内で適当なサイズにする */
		int i, limit;
		if (adj->horizontal) {
			limit = (int)(Q8GR_TVRAM_USER_W * 0.8);
		} else {
			limit = (int)(Q8GR_TVRAM_USER_H * 0.8);
		}
		for (i = 1; ; i++) {
			if (range / i < limit) {
				break;
			}
		}
		adj->length = range / i;
	} else {
		adj->length = adj->max_length;
	}

	/* 1コマあたりの変化量 */
	adj->scale = (float)(range - 1) / (adj->length - slider_size - 1);

	/* スライダーの位置  */
	if (adj->value == adj->lower) {
		adj->pos = 0;
	} else if (adj->value == adj->upper) {
		adj->pos = adj->length - slider_size;
	} else {
		adj->pos = (int)((adj->value - adj->lower) / adj->scale) + 1;
		if (adj->pos >= adj->length - slider_size) {
			adj->pos = adj->length - slider_size - 1;
		}
	}

	/* バー長さに矢印部の分を加えた値を返す */
	if (adj->horizontal) {
		*sx = adj->length + ((adj->arrow) ? arrow_size * 2: 0);
		*sy = bar_size;
	} else {
		*sx = bar_size;
		*sy = adj->length + ((adj->arrow) ? arrow_size * 2: 0);
	}
}




/*---------------------------------------------------------------------------
 * スクロールドウインドウ (SCROLLED WINDOW)
 *---------------------------------------------------------------------------
 * ・new()時に、引数でアジャストメントを指定するが、スケールのレンジ(範囲)は
 *   このスクロールドウインドウの子の大きさによって、動的に変化する。
 *    (増分は引き継がれる)
 * ・new()時の引数が NULL の場合は、自動的にアジャストメントが生成される。
 *   この時の増分は 1 と 10 である。
 *   特に理由がなければ、NULL による自動生成の方が簡単で便利である。
 *   なお、引数の片方だけを NULL にするとバグりそうな予感がする。
 *   なので、共に NULL または 共にアジャストメントを設定、とすること。
 * ・子を一つ持てる。
 *   (但し、子や孫がスクロールウインドウを持つような場合の動作は未保証)
 * ・シグナル … なし
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_scrolled_window_new(Q8tkWidget *hadjustment,
 *                                      Q8tkWidget *vadjustment)
 *    スクロールドウインドウの生成。
 *    予め作成済みのアジャストメント adjustment を指定する。
 *    NULL なら内部で自動生成される。
 *    ただし hadjustment/vadjustment ともに指定するか、ともにNULLのこと)
 *
 * void q8tk_scrolled_window_set_policy(Q8tkWidget *scrolledw,
 *                                      int hscrollbar_policy,
 *                                      int vscrollbar_policy)
 *    スクロールバーの表示方法を縦・横個別に設定する。
 *    Q8TK_POLICY_ALWAYS       常に表示する
 *    Q8TK_POLICY_AUTOMATIC    表示サイズが大きい場合のみ表示する
 *    Q8TK_POLICY_NEVER        表示しない
 *
 * void q8tk_misc_set_size(Q8tkWidget *widget, int width, int height)
 *    ウインドウのサイズ width 、 height を指定する。
 *
 * void q8tk_scrolled_window_set_frame(Q8tkWidget *w, int with_frame)
 *
 * void q8tk_scrolled_window_set_adj_size(Q8tkWidget *w, int size)
 *
 * --------------------------------------------------------------------------
 * 【SCROLLED WINDOW】←→ [xxxx]    いろんな子が持てる
 *         ｜｜
 *         ｜└─  【ADJUSTMENT】
 *         └──  【ADJUSTMENT】
 *
 *
 *
 * 【SCROLLED WINDOW】←→ [LISTBOX]
 *
 * スクロールドウインドウの子がリストボックスの時、
 * スクロールドウインドウをクリックすると、リストボックスに
 * フォーカスが設定される。つまり、キー入力は全てリストボックスに伝わる。
 *
 *---------------------------------------------------------------------------*/
/* ウインドウ内をマウスクリックした時
 * 直下の子がリストボックスの場合に限って、リストボックスをアクティブにする */
static void scrolled_window_event_button_on(Q8tkWidget *widget)
{
	if (widget->child && widget->child->type == Q8TK_TYPE_LISTBOX) {
		q8tk_widget_set_focus(widget->child);
	}
}
Q8tkWidget *q8tk_scrolled_window_new(Q8tkWidget *hadjustment,
									 Q8tkWidget *vadjustment)
{
	Q8tkWidget *w;

	Q8tkAssert(((hadjustment == NULL) && (vadjustment == NULL)) ||
			   ((hadjustment) && (vadjustment)), NULL);

	w = malloc_widget();
	w->type = Q8TK_TYPE_SCROLLED_WINDOW;
	w->attr = Q8TK_ATTR_CONTAINER;
	w->sensitive = TRUE;

	w->stat.scrolled.no_frame = FALSE;

	w->stat.scrolled.width  = 10;
	w->stat.scrolled.height = 10;
	w->stat.scrolled.hpolicy = Q8TK_POLICY_ALWAYS;
	w->stat.scrolled.vpolicy = Q8TK_POLICY_ALWAYS;
	w->stat.scrolled.hscrollbar = TRUE;
	w->stat.scrolled.vscrollbar = TRUE;

	if (hadjustment) {
		w->stat.scrolled.hadj = hadjustment;
	} else {
		w->stat.scrolled.hadj = q8tk_adjustment_new(0, 0, 7, 1, 10);
		w->with_label = TRUE;
	}
	q8tk_adjustment_set_arrow(w->stat.scrolled.hadj, TRUE);
	q8tk_adjustment_set_length(w->stat.scrolled.hadj, 7);
	w->stat.scrolled.hadj->stat.adj.horizontal = TRUE;
	w->stat.scrolled.hadj->parent = w;
	w->stat.scrolled.hadj->key_up_used    = FALSE;
	w->stat.scrolled.hadj->key_down_used  = FALSE;
	w->stat.scrolled.hadj->key_left_used  = TRUE;
	w->stat.scrolled.hadj->key_right_used = TRUE;

	if (vadjustment) {
		w->stat.scrolled.vadj = vadjustment;
	} else {
		w->stat.scrolled.vadj = q8tk_adjustment_new(0, 0, 7, 1, 10);
		w->with_label = TRUE;
	}
	q8tk_adjustment_set_arrow(w->stat.scrolled.vadj, TRUE);
	q8tk_adjustment_set_length(w->stat.scrolled.vadj, 7);
	w->stat.scrolled.vadj->stat.adj.horizontal = FALSE;
	w->stat.scrolled.vadj->parent = w;
	w->stat.scrolled.vadj->key_up_used    = TRUE;
	w->stat.scrolled.vadj->key_down_used  = TRUE;
	w->stat.scrolled.vadj->key_left_used  = FALSE;
	w->stat.scrolled.vadj->key_right_used = FALSE;

	w->event_button_on = scrolled_window_event_button_on;

	return w;
}
void q8tk_scrolled_window_set_policy(Q8tkWidget *scrolledw,
									 int hscrollbar_policy,
									 int vscrollbar_policy)
{
	scrolledw->stat.scrolled.hpolicy = hscrollbar_policy;
	scrolledw->stat.scrolled.vpolicy = vscrollbar_policy;

	if (scrolledw->stat.scrolled.hpolicy == Q8TK_POLICY_ALWAYS) {
		scrolledw->stat.scrolled.hscrollbar = TRUE;
	} else if (scrolledw->stat.scrolled.hpolicy == Q8TK_POLICY_NEVER)  {
		scrolledw->stat.scrolled.hscrollbar = FALSE;
	}

	if (scrolledw->stat.scrolled.vpolicy == Q8TK_POLICY_ALWAYS) {
		scrolledw->stat.scrolled.vscrollbar = TRUE;
	} else if (scrolledw->stat.scrolled.vpolicy == Q8TK_POLICY_NEVER)  {
		scrolledw->stat.scrolled.vscrollbar = FALSE;
	}

	set_construct_flag(TRUE);
}
#if 0
void q8tk_scrolled_window_set_width_height(Q8tkWidget *w,
										   int width, int height)
{
	w->stat.scrolled.width  = width;
	w->stat.scrolled.height = height;

	set_construct_flag(TRUE);
}
#endif
void q8tk_scrolled_window_set_frame(Q8tkWidget *w, int with_frame)
{
	int no_frame = (with_frame) ? FALSE : TRUE;

	if (w->stat.scrolled.no_frame != no_frame) {
		w->stat.scrolled.no_frame = no_frame;
		set_construct_flag(TRUE);
	}
}
void q8tk_scrolled_window_set_adj_size(Q8tkWidget *w, int size)
{
	q8tk_adjustment_set_size(w->stat.scrolled.hadj, size);
	q8tk_adjustment_set_size(w->stat.scrolled.vadj, size);
}
