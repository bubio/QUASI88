/************************************************************************/
/*                                                                      */
/* QUASI88 メニュー用 Tool Kit                                          */
/*                              Graphic lib                             */
/*                                                                      */
/*      Q8TK の API ( q8tk_XXX() 関数 ) 内部で呼ばれる関数群            */
/*      q8gr_tvram[][]に然るべき値をセットする。                        */
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "memory.h"             /* has_kanji_rom        */

#include "q8tk.h"
#include "q8tk-glib.h"
#include "q8tk-common.h"

#include "utf16.c"

/*******************************************************
 * ワーク
 ********************************************************/
int				q8gr_tvram_flip;
T_Q8GR_TVRAM	q8gr_tvram[2][ Q8GR_TVRAM_H ][ Q8GR_TVRAM_W ];
byte			*bitmap_table[ BITMAP_TABLE_SIZE ];
static	int		menu_mouse_x;
static	int		menu_mouse_y;
int q8gr_is_solid;


/*******************************************************
 * プロトタイプ
 ********************************************************/
static void q8gr_putchar(int x, int y, int fg, int bg,
						 int reverse, int underline, int c);

static int q8gr_strings(int x, int y, int fg, int bg,
						int reverse, int underline,
						int cursor_pos,
						int code, const char *str, int start, int width);

INLINE void q8gr_putc(int x, int y, int fg, int bg, int c)
{
	q8gr_putchar(x, y, fg, bg, FALSE, FALSE,  c);
}

INLINE int q8gr_puts(int x, int y, int fg, int bg,
					 int code, const char *str)
{
	return q8gr_strings(x, y, fg, bg, FALSE, FALSE, -1, code, str, 0, 0);
}


/*******************************************************
 *
 ********************************************************/
static void tvram_set(int x, int y,
					  int background, int foreground,
					  int reverse, int underline,
					  int font_type, int addr)
{
	q8gr_tvram[q8gr_tvram_flip][y][x].background = background;
	q8gr_tvram[q8gr_tvram_flip][y][x].foreground = foreground;
	q8gr_tvram[q8gr_tvram_flip][y][x].reverse    = reverse;
	q8gr_tvram[q8gr_tvram_flip][y][x].underline  = underline;
	q8gr_tvram[q8gr_tvram_flip][y][x].font_type  = font_type;
	q8gr_tvram[q8gr_tvram_flip][y][x].addr       = addr;
}


/*******************************************************
 *
 ********************************************************/
void q8gr_init(void)
{
	memset(q8gr_tvram, 0, sizeof(q8gr_tvram));
	q8gr_tvram_flip = 0;

	q8gr_clear_screen();
}



/*******************************************************
 * q8gr_tvram[][]をクリア
 ********************************************************/
void q8gr_clear_screen(void)
{
	memset(&q8gr_tvram[ q8gr_tvram_flip ][0][0], 0, sizeof(q8gr_tvram[ q8gr_tvram_flip ][0][0]));

	menu_mouse_x = -1;
	menu_mouse_y = -1;

	q8gr_set_screen_mask(0, 0, Q8GR_TVRAM_W, Q8GR_TVRAM_H);
	q8gr_clear_focus_screen();

	q8gr_set_cursor_exist(FALSE);

	if (q8tk_disp_cursor) {
		q8gr_clear_area(Q8GR_TVRAM_USER_X, Q8GR_TVRAM_USER_Y,
						Q8GR_TVRAM_USER_W, Q8GR_TVRAM_USER_H);
	}
}

void q8gr_clear_area(int x, int y, int sx, int sy)
{
	int i, j;

	T_Q8GR_TVRAM (*p)[ Q8GR_TVRAM_W ] = &q8gr_tvram[ q8gr_tvram_flip ][0];

	for (j = y; j < y + sy; j++) {
		for (i = x; i < x + sx; i++) {
			p[j][i].background = Q8GR_PALETTE_BACKGROUND;
			p[j][i].foreground = Q8GR_PALETTE_FOREGROUND;
			p[j][i].mouse      = FALSE;
			p[j][i].reverse    = FALSE;
			p[j][i].underline  = FALSE;
			p[j][i].font_type  = FONT_ANK;
			p[j][i].addr       = 0;
		}
	}
}



/*******************************************************
 * スクリーンのマスキング
 ********************************************************/
static	int		screen_mask_x0, screen_mask_x1;
static	int		screen_mask_y0, screen_mask_y1;



#define CHECK_MASK_X_FOR(x)		if      ((x) < screen_mask_x0) continue; \
								else if ((x) >=screen_mask_x1) break
#define CHECK_MASK_Y_FOR(y)		if      ((y) < screen_mask_y0) continue; \
								else if ((y) >=screen_mask_y1) break
#define CHECK_MASK_X(x)			((x)<screen_mask_x0 || (x)>=screen_mask_x1)
#define CHECK_MASK_Y(x)			((y)<screen_mask_y0 || (y)>=screen_mask_y1)


void q8gr_set_screen_mask(int x, int y, int sx, int sy)
{
	screen_mask_x0 = x;
	screen_mask_y0 = y;
	screen_mask_x1 = x + sx;
	screen_mask_y1 = y + sy;
}
void q8gr_get_screen_mask(int *x, int *y, int *sx, int *sy)
{
	*x  = screen_mask_x0;
	*y  = screen_mask_y0;
	*sx = screen_mask_x1 - screen_mask_x0;
	*sy = screen_mask_y1 - screen_mask_y0;
}




/*******************************************************
 * フォーカス用のスクリーン情報
 ********************************************************/
Q8tkWidget		dummy_widget_window;			/* ダミーの未使用ウィジット */
static	void	*focus_screen[ Q8GR_TVRAM_H ][ Q8GR_TVRAM_W ];


/* 座標マップをクリア・・・マスキング範囲が対象 */
void q8gr_clear_focus_screen(void)
{
	int i, j;

	for (j = screen_mask_y0; j < screen_mask_y1; j++) {
		for (i = screen_mask_x0; i < screen_mask_x1; i++) {
			focus_screen[j][i] = NULL;
		}
	}
}

/* 座標マップにウィジットをセット・・・マスキング範囲が対象 */
void q8gr_set_focus_screen(int x, int y, int sx, int sy, void *p)
{
	int i, j;

	if (p) {
		for (j = y; j < y + sy; j++) {
			CHECK_MASK_Y_FOR(j);
			for (i = x; i < x + sx; i++) {
				CHECK_MASK_X_FOR(i);
				focus_screen[j][i] = p;
			}
		}
	}
}

/* 指定座標のウィジットを取得 ・・・ 全エリアが対象 */
void *q8gr_get_focus_screen(int x, int y)
{
	if (0 <= x && x < Q8GR_TVRAM_W &&
		0 <= y && y < Q8GR_TVRAM_H) {
		return focus_screen[y][x];
	} else {
		return NULL;
	}
}

#if 0 /* 現在未使用 */
int q8gr_scan_focus_screen(void *p)
{
	int i, j;

	for (j = 0; j < Q8GR_TVRAM_H; j++) {
		for (i = 0; i < Q8GR_TVRAM_W; i++) {
			if (focus_screen[j][i] == p) {
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif





/*----------------------------------------------------------------------
 * (スクロールド)ウインドウ・(トグル)ボタン・フレーム・オプションメニュー
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ  ┌──────────┓↑     左上から光があたっているような
 *     │                    ┃｜     立体感をだす。
 *     │                    ┃sy
 *     │                    ┃｜
 *     └━━━━━━━━━━┛↓
 *      ←────sx────→
 *----------------------------------------------------------------------*/
static void draw_normal_wall(int x, int y, int sx, int sy, int shadow_type, int edge_type)
{
	static const int edge[2][4] = {
		{ Q8GR_G_7, Q8GR_G_9, Q8GR_G_1, Q8GR_G_3, },
		{ Q8GR_C_7, Q8GR_C_9, Q8GR_C_1, Q8GR_C_3, },
	};		
	int i, j, c, fg;
	int light, shadow;

	switch (shadow_type) {

	case Q8TK_SHADOW_NONE:						/* 枠線なし */
#if 0 /* 見えなくするため、背景と同じ色で描画 */
		light  = Q8GR_PALETTE_BACKGROUND;
		shadow = Q8GR_PALETTE_BACKGROUND;
		break;
#else /* いっそのこと、描画しない */
		return;
#endif

	case Q8TK_SHADOW_IN:						/* 全体がへこんでいる */
		if (q8gr_is_solid == FALSE) {
			light  = Q8GR_PALETTE_LIGHT;
			shadow = Q8GR_PALETTE_LIGHT;
		} else {
			light  = Q8GR_PALETTE_SHADOW;
			shadow = Q8GR_PALETTE_LIGHT;
		}
		break;

	case Q8TK_SHADOW_OUT:						/* 全体が盛り上がっている */
		if (q8gr_is_solid == FALSE) {
			light  = Q8GR_PALETTE_SHADOW;
			shadow = Q8GR_PALETTE_SHADOW;
		} else {
			light  = Q8GR_PALETTE_LIGHT;
			shadow = Q8GR_PALETTE_SHADOW;
		}
		break;

	case Q8TK_SHADOW_ETCHED_IN:					/* 枠だけへこんでいる */
		light  = Q8GR_PALETTE_LIGHT;
		shadow = Q8GR_PALETTE_LIGHT;
		break;

	case Q8TK_SHADOW_ETCHED_OUT:				/* 枠だけ盛り上がっている */
	default:
		light  = Q8GR_PALETTE_SHADOW;
		shadow = Q8GR_PALETTE_SHADOW;
		break;
	}

	for (j = y; j < y + sy; j++) {
		CHECK_MASK_Y_FOR(j);
		if (j == y || j == y + sy - 1) {
			for (i = x; i < x + sx; i++) {
				CHECK_MASK_X_FOR(i);

				if (i == x && j == y) {                           /* 左上 ┌ */
					c  = edge[edge_type][0];
					fg = light;
				} else if (i == x + sx - 1 && j == y) {           /* 右上 ┐ */
					c  = edge[edge_type][1];
					fg = shadow;
				} else if (i == x && j == y + sy - 1) {           /* 左下 └ */
					c  = edge[edge_type][2];
					fg = light;
				} else if (i == x + sx - 1 && j == y + sy - 1) {  /* 右下 ┘ */
					c  = edge[edge_type][3];
					fg = shadow;
				} else {
					c  = Q8GR_G__;
					if (j == y) {
						fg = light;                               /* 上   ─ */
					} else {
						fg = shadow;                              /* 下   ─ */
					}
				}

				q8gr_putc(i, j, fg, Q8GR_PALETTE_BACKGROUND, c);
			}
		} else {
			for (i = x; i < x + sx; i++) {
				CHECK_MASK_X_FOR(i);

				if (i == x || i == x + sx - 1) {
					c  = Q8GR_G_I;
					if (i == x) {
						fg = light;                               /* 左   │ */
					} else {
						fg = shadow;                              /* 右   │ */
					}
				} else {                                          /* 間      */
					c  = (Uint)' ';
					fg = Q8GR_PALETTE_FOREGROUND;
				}

				q8gr_putc(i, j, fg, Q8GR_PALETTE_BACKGROUND, c);
			}
		}
	}
}
static void draw_normal_box(int x, int y, int sx, int sy, int shadow_type)
{
	draw_normal_wall(x, y, sx, sy, shadow_type, 0);
}
static void draw_normal_ellipse(int x, int y, int sx, int sy, int shadow_type)
{
	draw_normal_wall(x, y, sx, sy, shadow_type, 1);
}


void q8gr_draw_window(int x, int y, int sx, int sy, int shadow_type,
					  void *p)
{
	draw_normal_box(x, y, sx, sy, shadow_type);

	if (p) {
		q8gr_set_focus_screen(x, y, sx, sy, p);
	}
}

void q8gr_draw_button(int x, int y, int sx, int sy, int condition, void *p)
{
	int shadow_type;
	if (condition == Q8TK_BUTTON_OFF) {
		shadow_type = Q8TK_SHADOW_OUT;
	} else {
		shadow_type = Q8TK_SHADOW_IN;
	}

	draw_normal_box(x, y, sx, sy, shadow_type);

	q8gr_set_focus_screen(x, y, sx, sy, p);
}

void q8gr_draw_button_space(int x, int y, int sx, int sy, void *p)
{
	q8gr_set_focus_screen(x, y, sx, sy, p);
}

void q8gr_draw_frame(int x, int y, int sx, int sy, int shadow_type,
					 int code, const char *str, void *p)
{
	int fg = (p) ? Q8GR_PALETTE_FOREGROUND : Q8GR_PALETTE_WHITE;
	int bg =       Q8GR_PALETTE_BACKGROUND;

	if (q8gr_is_solid == FALSE) {
		if (shadow_type == Q8TK_SHADOW_OUT) {
			shadow_type = Q8TK_SHADOW_IN;
		}
		if (shadow_type == Q8TK_SHADOW_ETCHED_OUT) {
			shadow_type = Q8TK_SHADOW_ETCHED_IN;
		}
	}

	draw_normal_box(x, y, sx, sy, shadow_type);

	q8gr_puts(x + 1, y, fg, bg, code, str);
}

void q8gr_draw_scrolled_window(int x, int y, int sx, int sy,
							   int shadow_type, void *p)
{
	draw_normal_box(x, y, sx, sy, shadow_type);

#if 0
	q8gr_set_focus_screen(x, y, sx, sy, p);
	if (sx >= 2  &&  sy >= 2) {
		q8gr_set_focus_screen(x + 1, y + 1, sx - 2, sy - 2, p);
	}
#endif
}

void q8gr_draw_switch(int x, int y, int condition, int timer, void *p)
{
	int sx = 11;
	int sy = 3;
	int dx = 0;

	draw_normal_ellipse(x, y, sx, sy, Q8TK_SHADOW_ETCHED_IN);
	q8gr_puts(x + 1, y + 1,
			  Q8GR_PALETTE_WHITE,
			  Q8GR_PALETTE_FONT_FG,
			  Q8TK_KANJI_ANK, "    ");

	if (timer == 0) {
		if (condition == Q8TK_BUTTON_OFF) {
			draw_normal_ellipse(x, y,
								(sx + 1) / 2, sy, Q8TK_SHADOW_ETCHED_OUT);
		} else {
			draw_normal_ellipse(x + 5, y,
								(sx + 1) / 2, sy, Q8TK_SHADOW_ETCHED_OUT);
		}
	} else {
		if (condition == Q8TK_BUTTON_OFF) {
			dx = timer / 25;
			draw_normal_ellipse(x + dx, y,
								(sx + 1) / 2, sy, Q8TK_SHADOW_ETCHED_OUT);
		} else {
			dx = timer / 25;
			draw_normal_ellipse(x + 5 - dx, y,
								(sx + 1) / 2, sy, Q8TK_SHADOW_ETCHED_OUT);
		}
	}


	q8gr_set_focus_screen(x, y, sx, sy, p);
}



/*----------------------------------------------------------------------
 * チェックボタン
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ     □  文字列の部分
 *      ←─→
 *      ボタン
 *----------------------------------------------------------------------*/
void q8gr_draw_check_button(int x, int y, int condition, void *p)
{
	int fg_edge  = (p) ? Q8GR_PALETTE_FOREGROUND : Q8GR_PALETTE_WHITE;
	int bg_edge  =       Q8GR_PALETTE_BACKGROUND;
	int fg_check = (p) ? Q8GR_PALETTE_FONT_FG    : Q8GR_PALETTE_WHITE;
	int bg_check = (p) ? Q8GR_PALETTE_FONT_BG    : Q8GR_PALETTE_SCALE_BAR;

	q8gr_putc(x, y, fg_edge, bg_edge, Q8GR_L_RIGHT);

#if 1 /* チェック部を■で表現 */
	q8gr_putc(x + 1, y, fg_check, bg_check,
			  ((condition == Q8TK_BUTTON_OFF) ? Q8GR_B_UL : Q8GR_B_BOX));
#else /* チェック部を×で表現 */
	q8gr_putchar(x + 1, y, fg_check, bg_check, FALSE, TRUE,
				 ((condition == Q8TK_BUTTON_OFF) ? Q8GR_B_UL : Q8GR_B_X));
#endif

	q8gr_putc(x + 2, y, fg_edge, bg_edge, Q8GR_L_LEFT);



	if (p) {
#if 0 /* チェックボタン部のみ、クリックに反応する */

		q8gr_set_focus_screen(x + 1, y, 1, 1, p);

#else /* ラベルを子に持つ場合、そのラベルをクリックしても反応する */

		if (((Q8tkWidget *)p)->child &&
			((Q8tkWidget *)p)->child->type == Q8TK_TYPE_LABEL &&
			((Q8tkWidget *)p)->child->visible &&
			((Q8tkWidget *)p)->child->sensitive &&
			((Q8tkWidget *)p)->child->name) {

			int len = q8gr_strlen(((Q8tkWidget *)p)->child->code,
								  ((Q8tkWidget *)p)->child->name);

			q8gr_set_focus_screen(x + 1, y, 1 + 1 + len, 1, p);
		} else {
			q8gr_set_focus_screen(x + 1, y, 1, 1, p);
		}
#endif
	}
}


/*----------------------------------------------------------------------
 * ラジオボタン
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ     ○  文字列の部分
 *      ←─→
 *      ボタン
 *----------------------------------------------------------------------*/
void q8gr_draw_radio_button(int x, int y, int condition, void *p)
{
	int fg = (p) ? Q8GR_PALETTE_FOREGROUND : Q8GR_PALETTE_WHITE;
	int bg =       Q8GR_PALETTE_BACKGROUND;

	q8gr_putc(x, y, fg, bg, ' ');

	q8gr_putc(x + 1, y, fg, bg,
			  ((condition == Q8TK_BUTTON_OFF) ? Q8GR_B_OFF : Q8GR_B_ON));

	q8gr_putc(x + 2, y, fg, bg, ' ');

	if (p) {
#if 0 /* ラジオボタン部のみ、クリックに反応する */

		q8gr_set_focus_screen(x + 1, y, 1, 1, p);

#else /* ラベルを子に持つ場合、そのラベルをクリックしても反応する */

		if (((Q8tkWidget *)p)->child &&
			((Q8tkWidget *)p)->child->type == Q8TK_TYPE_LABEL &&
			((Q8tkWidget *)p)->child->visible &&
			((Q8tkWidget *)p)->child->sensitive &&
			((Q8tkWidget *)p)->child->name) {

			int len = q8gr_strlen(((Q8tkWidget *)p)->child->code,
								  ((Q8tkWidget *)p)->child->name);

			q8gr_set_focus_screen(x + 1, y, 1 + 1 + len, 1, p);
		} else {
			q8gr_set_focus_screen(x + 1, y, 1, 1, p);
		}
#endif
	}
}

void q8gr_draw_radio_push_button(int x, int y, int sx, int sy, int condition,
								 void *p)
{
	if (q8gr_is_solid == FALSE) {
		condition = (condition) ? 0 : 1;
	}

	q8gr_draw_button(x, y, sx, sy, condition, p);
}



/*----------------------------------------------------------------------
 * ノートブック
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ  ┌──┓━━┓━━┓    ↑
 *     │タブ┃タブ│タブ│    ｜
 *     │    └──┴──┴─┓｜
 *     │                    ┃sy
 *     │                    ┃｜
 *     │                    ┃｜
 *     └━━━━━━━━━━┛↓
 *      ←────sx────→
 *----------------------------------------------------------------------*/
void q8gr_draw_notebook(int x, int y, int sx, int sy,
						Q8tkWidget *notebook, void *p)
{
	struct notebook_draw *draw = &notebook->stat.notebook.draw;

	if (p) {
		draw->drawing = TRUE;
		draw->x  = x;
		draw->y  = y;
		draw->x0 = x;
		draw->x1 = x + sx - 1;
		draw->selected = FALSE;
	} else {
		draw->drawing = FALSE;
	}

	/* 上部のタグ部分以外の、ウインドウを描く */
	q8gr_draw_button(x, y + 2, sx, sy - 2, Q8TK_BUTTON_OFF, NULL);
}

void q8gr_draw_notepage(int code, const char *tag,
						int select_flag, int active_flag,
						Q8tkWidget *notebook, void *p)
{
	int i, len;
	int fg = (p) ? Q8GR_PALETTE_FOREGROUND : Q8GR_PALETTE_WHITE;
	int bg =       Q8GR_PALETTE_BACKGROUND;
	int light  = Q8GR_PALETTE_LIGHT;
	int shadow = Q8GR_PALETTE_SHADOW;
	int focus_x;
	struct notebook_draw *draw = &notebook->stat.notebook.draw;


	if (draw->drawing == FALSE) {
		return;
	}

	if (select_flag) {
		/* 選択中ページのタグは、フォーカスなし */
		p = NULL;
	}

	focus_x = draw->x;

	len = q8gr_strlen(code, tag);

	if (select_flag) {
		/* 選択したノートページのタグ部分を表示 */

		q8gr_strings(draw->x + 1, draw->y + 1, fg, bg,
					 FALSE, active_flag, -1, code, tag, 0, 0);

		if (q8gr_is_solid == FALSE) {
			light = shadow;
		}

		q8gr_putc(draw->x, draw->y,   light, bg, Q8GR_C_7);
		q8gr_putc(draw->x, draw->y + 1, light, bg, Q8GR_G_I);
		if (draw->x == draw->x0) {
			q8gr_putc(draw->x, draw->y + 2, light, bg, Q8GR_G_I);
		} else {
			q8gr_putc(draw->x, draw->y + 2, light, bg, Q8GR_G_3);
		}

		draw->x ++;
		for (i = 0; i < len; i++, draw->x++) {
			q8gr_putc(draw->x, draw->y,   light, bg, Q8GR_G__);
			q8gr_putc(draw->x, draw->y + 2, light, bg, ' ');
		}

		q8gr_putc(draw->x, draw->y,   shadow, bg, Q8GR_C_9);
		q8gr_putc(draw->x, draw->y + 1, shadow, bg, Q8GR_G_I);
		if (draw->x == draw->x1) {
			q8gr_putc(draw->x, draw->y + 2, shadow, bg, Q8GR_G_I);
		} else {
			q8gr_putc(draw->x, draw->y + 2, light,  bg, Q8GR_G_1);
		}
		draw->x ++;

		draw->selected = TRUE;

	} else if (draw->selected == FALSE) {
		/* 選択したノートページより左のページの、タグ部分を表示 */

		q8gr_strings(draw->x + 1, draw->y + 1, fg, bg,
					 FALSE, active_flag, -1, code, tag, 0, 0);

		if (q8gr_is_solid == FALSE) {
			shadow = light;
		}

		q8gr_putc(draw->x, draw->y,   shadow, bg, Q8GR_C_7);
		q8gr_putc(draw->x, draw->y + 1, shadow, bg, Q8GR_G_I);
		if (q8gr_is_solid) {
			if (draw->x == draw->x0) {
				q8gr_putc(draw->x, draw->y + 2, light, bg, Q8GR_G_4);
			} else {
				q8gr_putc(draw->x, draw->y + 2, light, bg, Q8GR_G_2);
			}
		}

		draw->x ++;
		for (i = 0; i < len; i++, draw->x++) {
			q8gr_putc(draw->x, draw->y, shadow, bg, Q8GR_G__);
		}

	} else {
		/* 選択したノートページより右のページの、タグ部分を表示 */

		q8gr_strings(draw->x, draw->y + 1, fg, bg,
					 FALSE, active_flag, -1, code, tag, 0, 0);

		if (q8gr_is_solid == FALSE) {
			shadow = light;
		}

		for (i = 0; i < len; i++, draw->x++) {
			q8gr_putc(draw->x, draw->y, shadow, bg, Q8GR_G__);
		}

		q8gr_putc(draw->x, draw->y,   shadow, bg, Q8GR_C_9);
		q8gr_putc(draw->x, draw->y + 1, light,  bg, Q8GR_G_I);
		if (q8gr_is_solid) {
			if (draw->x == draw->x1) {
				q8gr_putc(draw->x, draw->y + 2, shadow, bg, Q8GR_G_6);
			} else {
				q8gr_putc(draw->x, draw->y + 2, light,  bg, Q8GR_G_2);
			}
		}
		draw->x ++;

	}

	q8gr_set_focus_screen(focus_x, draw->y, draw->x - focus_x, 2, p);
}


/*----------------------------------------------------------------------
 * 垂直・水平区切り線
 *----------------------------------------------------------------------
 *      ｘ             ｘ
 * ｙ   ┃↑        ｙ ━━━━━━━━━━
 *      ┃｜           ←───width ──→
 *      ┃height
 *      ┃｜
 *      ┃↓
 *----------------------------------------------------------------------*/
void q8gr_draw_vseparator(int x, int y, int height, int style)
{
	int j;
	static const int charactor[] = { Q8GR_G_I, '|', Q8GR_G_I, };
	if (style >= COUNTOF(charactor)) {
		style = 0;
	}

	if (! CHECK_MASK_X(x)) {
		for (j = y; j < y + height; j++) {
			CHECK_MASK_Y_FOR(j);
			q8gr_putc(x, j, Q8GR_PALETTE_FOREGROUND, Q8GR_PALETTE_BACKGROUND,
					  charactor[style]);
		}
	}
}

void q8gr_draw_hseparator(int x, int y, int width, int style)
{
	int i;
	static const int charactor[] = { Q8GR_G__, '-', Q8GR_G_W, };
	if (style >= COUNTOF(charactor)) {
		style = 0;
	}

	if (! CHECK_MASK_Y(y)) {
		for (i = x; i < x + width; i++) {
			CHECK_MASK_X_FOR(i);
			q8gr_putc(i, y, Q8GR_PALETTE_FOREGROUND, Q8GR_PALETTE_BACKGROUND,
					  charactor[style]);
		}
	}
}

/*----------------------------------------------------------------------
 * エントリ
 *----------------------------------------------------------------------
 *        ｘ
 *        ←─width →
 * y      文字列の部分
 *
 *----------------------------------------------------------------------*/
void q8gr_draw_entry(int x, int y, int width, int code, const char *text,
					 int disp_pos, int cursor_pos, void *p)
{
	int fg = (p) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_BACKGROUND;
	int bg = (p) ? Q8GR_PALETTE_FONT_BG : Q8GR_PALETTE_SCALE_BAR;

	q8gr_strings(x, y, fg, bg,
				 FALSE, FALSE,
				 cursor_pos, code, text, disp_pos, width);

	/* 編集可能な場合のみ反応する */
	if (p && (((Q8tkWidget *)p)->stat.entry.editable)) {
		q8gr_set_focus_screen(x, y, width, 1, p);
	}
}


/*----------------------------------------------------------------------
 * コンボ
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ   文字列の部分  ▼              矢印だけを表示する。
 *      ←─width →←─→            文字列は entry として別途表示する
 *                   矢印
 *----------------------------------------------------------------------*/
void q8gr_draw_combo(int x, int y, int width, int active, void *p)
{
#if 0 /* 矢印部を制御文字(↓の反転)で表現 */
	int fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_FOREGROUND;
	int bg =            Q8GR_PALETTE_BACKGROUND;

	if (p == NULL) {
		fg = Q8GR_PALETTE_SCALE_BAR;
	}

	q8gr_putchar(x + width,  y, fg, bg, TRUE, FALSE, ' ');
	q8gr_putchar(x + width + 1, y, fg, bg, TRUE, FALSE, Q8GR_A_D);
	q8gr_putchar(x + width + 2, y, fg, bg, TRUE, FALSE, ' ');

	q8gr_set_focus_screen(x + width, y, 3, 1, p);


#else /* 矢印部をグラフィック文字2文字(▼)で表現 */
	int fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_FOREGROUND;
	int bg =            Q8GR_PALETTE_BACKGROUND;

	if (p == NULL) {
		fg = Q8GR_PALETTE_SCALE_BAR;
	}

	q8gr_putc(x + width,  y, fg, bg, 0xe6);
	q8gr_putc(x + width + 1, y, fg, bg, 0xe7);

	q8gr_set_focus_screen(x + width, y, 2, 1, p);
#endif


	/* エントリ部が編集不可の場合、エントリ部をクリックしても反応する */
	if (p &&
		(((Q8tkWidget *)p)->stat.combo.entry->stat.entry.editable == FALSE)) {
		q8gr_set_focus_screen(x, y, width, 1, p);
	}
}


/*----------------------------------------------------------------------
 * リストアイテム
 *----------------------------------------------------------------------
 *      ｘ
 * ｙ   文字列の部分
 *      ←─width →
 *----------------------------------------------------------------------*/
static void draw_list_item_frame(int x, int y, int fg, int bg, int sx, int type)
{
	/* リストアイテムに枠をつける場合の描画処理
	 *
	 *	0	┌─┐	1	┌─┐	2	├─┤	3	├─┤	
	 *		│	│		│	│		│	│		│	│	
	 *		└─┘					└─┘				
	 */
	int sy = (type & 1) ? 2 : 3;
	int i, j, c;

	for (j = y; j < y + sy; j++) {
		CHECK_MASK_Y_FOR(j);
		for (i = x; i < x + sx; i++) {
			CHECK_MASK_X_FOR(i);
			if (j == y) {

				if (i == x) {
					c  = (type & 2) ? Q8GR_G_4: Q8GR_G_7;
				} else if (i == x + sx - 1) {
					c  = (type & 2) ? Q8GR_G_6: Q8GR_G_9;
				} else {
					c  = Q8GR_G__;
				}

			} else if (j == y + 1) {

				if (i == x || i == x + sx - 1) {
					c  = Q8GR_G_I;
				} else {
					c  = (Uint)' ';
				}

			} else {

				if (i == x) {
					c  = Q8GR_G_1;
				} else if (i == x + sx - 1) {
					c  = Q8GR_G_3;
				} else {
					c  = Q8GR_G__;
				}
			}
			q8gr_putc(i, j, fg, bg, c);
		}
	}
}


void q8gr_draw_list_item(int x, int y, int width, int active,
						 int reverse, int underline,
						 int code, const char *text, int big_type, void *p)
{
	int fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_SHADOW;
	int bg = (p)      ? Q8GR_PALETTE_FONT_BG : Q8GR_PALETTE_BACKGROUND;

	if (p == NULL) {
		fg = Q8GR_PALETTE_WHITE;
	}

	if (big_type == 0) {
		q8gr_strings(x, y, fg, bg,
					 reverse, underline,
					 -1, code, text, 0, width);

		q8gr_set_focus_screen(x, y, width, 1, p);
	} else {

		draw_list_item_frame(x, y, fg, bg, width, big_type - 1);

		q8gr_strings(x + 1, y + 1, fg, bg,
					 reverse, underline,
					 -1, code, text, 0, width - 2);

		q8gr_set_focus_screen(x, y, width, 2, p);
	}
}


/*----------------------------------------------------------------------
 * 水平スケール・垂直スケール
 *----------------------------------------------------------------------
 *      ｘ             ｘ
 * ｙ   ↑↑        ｙ □□□□■□□□□→
 *      □｜           ←───length──→
 *      ■height
 *      □｜
 *      ↓↓
 *----------------------------------------------------------------------*/
#define	HIGHLIGHT	(1)
static void draw_adjustment(int x, int y, int active,
							Q8Adjust *adj, int adj_size, void *p)
{
	/* volatile は不要なのだが、諸事情により付けておく */
	static const volatile unsigned char s2x2[] = { Q8GR_S0, Q8GR_S1, Q8GR_S2, Q8GR_S3, };
	static const volatile unsigned char l2x2[] = { Q8GR_L0, Q8GR_L1, Q8GR_L2, Q8GR_L3, };
	static const volatile unsigned char r2x2[] = { Q8GR_R0, Q8GR_R1, Q8GR_R2, Q8GR_R3, };
	static const volatile unsigned char u2x2[] = { Q8GR_U0, Q8GR_U1, Q8GR_U2, Q8GR_U3, };
	static const volatile unsigned char d2x2[] = { Q8GR_D0, Q8GR_D1, Q8GR_D2, Q8GR_D3, };
	int i, j, rev, bg, c;
	int fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_FOREGROUND;
	int slider0, slider1;
	int slider_size = adj_size;
	int arrow_size = adj_size;
	int bar_size = adj_size;

	slider0 = adj->pos;
	slider1 = adj->pos + slider_size - 1;

	adj->x = x;
	adj->y = y;

	if (adj->horizontal) {
		/* HORIZONTAL */

		if (p) {
			if (adj->arrow) {
				if (HIGHLIGHT && (adj->active == ADJUST_CTRL_STEP_DEC)) {
					rev = TRUE;
				} else {
					rev = FALSE;
				}
				bg = Q8GR_PALETTE_SCALE_SLD;
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = l2x2[ j * 2 + i ];
						} else {
							c = Q8GR_A_L;
						}
						q8gr_putchar(x, y + j, fg, bg, rev, FALSE, c);
					}
					x++;
				}
			}
			for (i = 0; i < adj->length; i++) {
				if (i < adj->pos) {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_PAGE_DEC)) {
						bg = Q8GR_PALETTE_SCALE_SLD;
					} else {
						bg = Q8GR_PALETTE_SCALE_BAR;
					}
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x, y + j, fg, bg, ' ');
					}

				} else if (BETWEEN(slider0, i, slider1)) {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_SLIDER)) {
						rev = TRUE;
					} else {
						rev = FALSE;
					}
					bg = Q8GR_PALETTE_SCALE_SLD;
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = s2x2[ j * 2 + (i - slider0) ];
						} else {
							c = Q8GR_B_B;
						}
						q8gr_putchar(x, y + j, fg, bg, rev, FALSE, c);
					}

				} else {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_PAGE_INC)) {
						bg = Q8GR_PALETTE_SCALE_SLD;
					} else {
						bg = Q8GR_PALETTE_SCALE_BAR;
					}
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x, y + j, fg, bg, ' ');
					}
				}
				x++;
			}
			if (adj->arrow) {
				if (HIGHLIGHT && (adj->active == ADJUST_CTRL_STEP_INC)) {
					rev = TRUE;
				} else {
					rev = FALSE;
				}
				bg = Q8GR_PALETTE_SCALE_SLD;
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = r2x2[ j * 2 + i ];
						} else {
							c = Q8GR_A_R;
						}
						q8gr_putchar(x, y + j, fg, bg, rev, FALSE, c);
					}
					x++;
				}
			}
		} else {
			if (adj->arrow) {
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x, y + j, fg, Q8GR_PALETTE_SCALE_SLD, ' ');
					}
					x++;
				}
			}
			for (i = 0; i < adj->length; i++) {
				for (j = 0; j < bar_size; j++) {
					q8gr_putc(x, y + j, fg, Q8GR_PALETTE_SCALE_BAR, ' ');
				}
				x++;
			}
			if (adj->arrow) {
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x, y + j, fg, Q8GR_PALETTE_SCALE_SLD, ' ');
					}
					x++;
				}
			}
		}
		q8gr_set_focus_screen(adj->x, adj->y,
							  adj->length + (adj->arrow ? arrow_size * 2 : 0),
							  bar_size,
							  p);

	} else {
		/* VIRTICAL */

		if (p) {
			if (adj->arrow) {
				if (HIGHLIGHT && (adj->active == ADJUST_CTRL_STEP_DEC)) {
					rev = TRUE;
				} else {
					rev = FALSE;
				}
				bg = Q8GR_PALETTE_SCALE_SLD;
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = u2x2[ i * 2 + j ];
						} else {
							c = Q8GR_A_U;
						}
						q8gr_putchar(x + j, y, fg, bg, rev, FALSE, c);
					}
					y++;
				}
			}
			for (i = 0; i < adj->length; i++) {
				if (i < adj->pos) {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_PAGE_DEC)) {
						bg = Q8GR_PALETTE_SCALE_SLD;
					} else {
						bg = Q8GR_PALETTE_SCALE_BAR;
					}
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x + j, y, fg, bg, ' ');
					}

				} else if (BETWEEN(slider0, i, slider1)) {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_SLIDER)) {
						rev = TRUE;
					} else {
						rev = FALSE;
					}
					bg = Q8GR_PALETTE_SCALE_SLD;
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = s2x2[ (i - slider0) * 2 + j ];
						} else {
							c = Q8GR_B_B;
						}
						q8gr_putchar(x + j, y, fg, bg, rev, FALSE, c);
					}

				} else {
					if (HIGHLIGHT && (adj->active == ADJUST_CTRL_PAGE_INC)) {
						bg = Q8GR_PALETTE_SCALE_SLD;
					} else {
						bg = Q8GR_PALETTE_SCALE_BAR;
					}
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x + j, y, fg, bg, ' ');
					}
				}
				y++;
			}
			if (adj->arrow) {
				if (HIGHLIGHT && (adj->active == ADJUST_CTRL_STEP_INC)) {
					rev = TRUE;
				} else {
					rev = FALSE;
				}
				bg = Q8GR_PALETTE_SCALE_SLD;
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						if (adj_size == 2) {
							c = d2x2[ i * 2 + j ];
						} else {
							c = Q8GR_A_D;
						}
						q8gr_putchar(x + j, y, fg, bg, rev, FALSE, c);
					}
					y++;
				}
			}
		} else {
			if (adj->arrow) {
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x + j, y, fg, Q8GR_PALETTE_SCALE_SLD, ' ');
					}
					y++;
				}
			}
			for (i = 0; i < adj->length; i++) {
				for (j = 0; j < bar_size; j++) {
					q8gr_putc(x + j, y, fg, Q8GR_PALETTE_SCALE_BAR, ' ');
				}
				y++;
			}
			if (adj->arrow) {
				for (i = 0; i < arrow_size; i++) {
					for (j = 0; j < bar_size; j++) {
						q8gr_putc(x + j, y, fg, Q8GR_PALETTE_SCALE_SLD, ' ');
					}
					y++;
				}
			}
		}
		q8gr_set_focus_screen(adj->x, adj->y,
							  bar_size,
							  adj->length + (adj->arrow ? arrow_size * 2 : 0),
							  p);
	}

}



void q8gr_draw_hscale(int x, int y, Q8Adjust *adj, int active,
					  int draw_value, int value_pos,
					  int adj_size,
					  void *p)
{
	int arrow_size = adj_size;

	if (draw_value) {
		int  fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_FOREGROUND;
		int  bg =            Q8GR_PALETTE_BACKGROUND;
		int  vx, vy;
		char valstr[8];
		int  len = adj->length + (adj->arrow ? (arrow_size * 2) : 0);

		if (p == NULL) {
			fg = Q8GR_PALETTE_WHITE;
		}

		if (adj->value < -99) {
			strcpy(valstr, "-**");
		} else if (adj->value > 999) {
			strcpy(valstr, "***");
		} else {
			sprintf(valstr, "%3d", adj->value);
		}

		switch (value_pos) {

		case Q8TK_POS_LEFT:
			vx = x;
			vy = y;
			x += 4;
			break;

		case Q8TK_POS_RIGHT:
			vx = x + len + 1;
			vy = y;
			break;

		case Q8TK_POS_TOP:
			vx = x + ((adj->pos + 3 > len) ? (len - 3) : adj->pos);
			vy = y;
			y += 1;
			break;

		case Q8TK_POS_BOTTOM:
		default:
			vx = x + ((adj->pos + 3 > len) ? (len - 3) : adj->pos);
			vy = y + 1;
			break;
		}

		q8gr_puts(vx, vy, fg, bg, Q8TK_KANJI_ANK, valstr);
	}

	draw_adjustment(x, y, active, adj, adj_size, p);
}


void q8gr_draw_vscale(int x, int y, Q8Adjust *adj, int active,
					  int draw_value, int value_pos,
					  int adj_size,
					  void *p)
{
	int arrow_size = adj_size;

	if (draw_value) {
		int  fg = (active) ? Q8GR_PALETTE_FONT_FG : Q8GR_PALETTE_FOREGROUND;
		int  bg =            Q8GR_PALETTE_BACKGROUND;
		int  vx, vy;
		char valstr[8];

		if (p == NULL) {
			fg = Q8GR_PALETTE_WHITE;
		}

		if (adj->value < -99) {
			strcpy(valstr, "-**");
		} else if (adj->value > 999) {
			strcpy(valstr, "***");
		} else {
			sprintf(valstr, "%3d", adj->value);
		}

		switch (value_pos) {

		case Q8TK_POS_LEFT:
			vx = x;
			vy = y + adj->pos + (adj->arrow ? (arrow_size) : 0);
			x += 4;
			break;

		case Q8TK_POS_RIGHT:
			vx = x + 1;
			vy = y + adj->pos + (adj->arrow ? (arrow_size) : 0);
			break;

		case Q8TK_POS_TOP:
			vx = x;
			vy = y;
			x += 1;
			y += 1;
			break;

		case Q8TK_POS_BOTTOM:
		default:
			vx = x;
			vy = y + adj->length + (adj->arrow ? (arrow_size * 2) : 0);
			x += 1;
			break;
		}

		q8gr_puts(vx, vy, fg, bg, Q8TK_KANJI_ANK, valstr);
	}

	draw_adjustment(x, y, active, adj, adj_size, p);
}





/***********************************************************************
 * ワーク q8gr_tvram[][] を実際に操作する関数
 *      なお、表示バイトとは、半角1バイト、全角2バイトと数える
 ************************************************************************/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * ASCIIコード、JIS漢字コードを、内蔵ROMアドレスに変換
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void kanji2addr(int code, unsigned int *addr, unsigned int *type)
{
	if (has_kanji_rom == FALSE) {
		/* 漢字ROMない場合 */

		if (code < 0x100) {
			/* 半角文字 */
			*addr = (code << 3);
			*type = FONT_ANK;
		} else {
			/* 全角文字 */
			*addr = 0;
			*type = FONT_KNJXL;
		}

	} else {
		/* 漢字ROMある場合 */

		if (code < 0x100) {
			/* 半角文字 */
			if ((0x20 <= code && code <= 0x7f) ||
				(0xa0 <= code && code <= 0xdf)) {
				/* ASCII or カタカナ */

				*addr = (code << 3);
				*type = FONT_HALF;

			} else {
				/* コントロール文字,グラフィック文字 */
				*addr = (code << 2) | 0x0800;
				*type = FONT_QUART;
			}

		} else if ((0x2121 <= code && code <= 0x217e) ||
				   (0x2221 <= code && code <= 0x222e) ||
				   (0x2330 <= code && code <= 0x2339) ||
				   (0x2341 <= code && code <= 0x235a) ||
				   (0x2361 <= code && code <= 0x237a) ||
				   (0x2421 <= code && code <= 0x2473) ||
				   (0x2521 <= code && code <= 0x2576) ||
				   (0x2621 <= code && code <= 0x2638) ||
				   (0x2641 <= code && code <= 0x2658) ||
				   (0x2721 <= code && code <= 0x2741) ||
				   (0x2751 <= code && code <= 0x2771)) {
			/* 非漢字 */
			*addr = (((code & 0x0060) << 7) | ((code & 0x0700) << 1)
					 | ((code & 0x001f) << 4));
			*type = FONT_KNJ1L;

		} else if (0x3021 <= code && code <= 0x4f53) {
			/* 第一水準漢字 */
			*addr = (((code & 0x0060) << 9) | ((code & 0x1f00) << 1)
					 | ((code & 0x001f) << 4));
			*type = FONT_KNJ1L;

		} else if (0x5021 <= code && code <= 0x6f7e) {
			/* 第二水準漢字 その1 */
			*addr = (((code & 0x0060) << 9) | ((code & 0x0f00) << 1)
					 | ((code & 0x001f) << 4) | (code & 0x2000));
			*type = FONT_KNJ2L;

		} else if (0x7021 <= code && code <= 0x737e) {
			/* 第二水準漢字 その2 */
			*addr = (((code & 0x0060) << 7) | ((code & 0x0700) << 1)
					 | ((code & 0x001f) << 4));
			*type = FONT_KNJ2L;

		} else {
			*addr = 0;
			*type = FONT_KNJXL;
		}
	}
}



static int get_letter(int code, const char **str)
{
	const unsigned char *p = (const unsigned char *)(*str);
	unsigned int h, c = 0;

	switch (code) {
	case Q8TK_KANJI_EUC:
		while ((h = *p ++)) {
			if (h == 0x8e) {
				/* 半角カナ */
				if ((c = *p++) == '\0') {
					break;
				}
				/* 0xa0..0xdf以外の値でも気にしない */
				;

			} else if (h == 0x8f) {
				/* 補助漢字 */
				if ((c = *p++) == '\0') {
					break;
				}
				if ((c = *p++) == '\0') {
					break;
				}
				/* 0x21,0x21..0x7e,0x7e以外の値でも気にしない */
				c = 0xffff;

			} else if (0xa1 <= h && h <= 0xfe) {
				/* 漢字 */
				if ((c = *p++) == '\0') {
					break;
				}

				if (0xa1 <= c && c <= 0xfe) {
					/* EUC → JIS */
					c = ((h & 0x7f) << 8) | (c & 0x7f);
				} else {
					/* 範囲外 */
					c = 0xffff;
				}

			} else {
				/* ASCII */
				c = h;
			}
			*str = (const char *)p;
			return c;
		}
		break;

	case Q8TK_KANJI_SJIS:
		while ((h = *p ++)) {
			if ((0x81 <= h && h <= 0x9f) ||
				(0xe0 <= h && h <= 0xef)) {
				/* 漢字 */
				if ((c = *p++) == '\0') {
					break;
				}

				if ((0x40 <= c && c <= 0x7e) ||
					(0x80 <= c && c <= 0xfc)) {
					/* SJIS → JIS */
					c = (h << 8) | c;
					if (0xe000 <= c) {
						c -= 0x4000;
					}
					c = ((((c & 0xff00) - 0x8100) << 1) | (c & 0x00ff)) & 0xffff;
					if ((c & 0x00ff) >= 0x80) {
						c -= 1;
					}
					if ((c & 0x00ff) >= 0x9e) {
						c += 0x100 - 0x9e;
					} else {
						c -= 0x40;
					}
					c += 0x2121;
				} else {
					/* 範囲外 */
					c = 0xffff;
				}

			} else if (0xf0 <= h && h <= 0xfc) {
				/* 漢字(予備) */
				if ((c = *p++) == '\0') {
					break;
				}
				/* 範囲外でも気にしない */
				c = 0xffff;

			} else {
				/* ANK */
				c = h;
			}
			*str = (const char *)p;
			return c;
		}
		break;

	case Q8TK_KANJI_UTF8:
		while ((h = *p ++)) {
			if (h < 0x80) {
				/* ASCII    0xxxxxxx */
				c = h;
			} else {
				if (h < 0xc0) {
					/* 継続     10xxxxxx */
					continue;

				} else if (h < 0xe0) {
					/* 2byte    110xxxxx */
					c = (h & 0x1f) << 6;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f);

				} else if (h < 0xf0) {
					/* 3byte    1110xxxx */
					c = (h & 0x0f) << 12;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f) << 6;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f);

				} else if (h < 0xf8) {
					/* 4byte    11110xxx */
					c = (h & 0x07) << 18;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f) << 12;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f) << 6;
					if ((h = *p++) == '\0') {
						break;
					}
					c |= (h & 0x3f);

				} else {
					/* 5..6byte          */
					continue;
				}

				/* Unicode → JIS */
				if (0xff61 <= c && c <= 0xff9f) {
					c = c - 0xff61 + 0xa1;
				} else {
					int i;
					int found = FALSE;
					for (i = 0; jis2utf16[i][0]; i++) {
						if (jis2utf16[i][1] == c) {
							found = TRUE;
							break;
						}
					}
					if (found) {
						c = jis2utf16[i][0];
					} else {
						c = 0xffff;
					}
				}
			}
			*str = (const char *)p;
			return c;
		}
		break;

	case Q8TK_KANJI_UTF16LE_DUMMY:
	case Q8TK_KANJI_UTF16BE_DUMMY:
		while ((h = *p++)) {
			if ((c = *p++) == '\0') {
				break;
			}
			if (code == Q8TK_KANJI_UTF16LE_DUMMY) {
				c = (c << 8) | h;
			} else {
				c = (h << 8) | c;
			}
			if (c < 0x80) {
				/* ASCII */
				;
			} else if (0xd800 <= c && c <= 0xdb00) {
				/* サロゲート */
				if ((c = *p++) == '\0') {
					break;
				}
				if ((c = *p++) == '\0') {
					break;
				}
				/* 0xdc00..0xdfff以外の値でも気にしない */
				c = 0xffff;

			} else if (0xdc00 <= c && c <= 0xdfff) {
				/* サロゲート */
				/* 範囲外 */
				c = 0xffff;

			} else if ((Q8TK_UTF16_DUMMYCODE <= c) &&
					   (c <= Q8TK_UTF16_DUMMYCODE + 0x7f)) {
				/* ダミーコード */
				/* U+0001 - U+007F の文字は内部で変換しているので戻す */
				c = c - Q8TK_UTF16_DUMMYCODE;

			} else {
				/* Unicode → JIS */
				if (0xff61 <= c && c <= 0xff9f) {
					c = c - 0xff61 + 0xa1;
				} else {
					int i;
					int found = FALSE;
					for (i = 0; jis2utf16[i][0]; i++) {
						if (jis2utf16[i][1] == c) {
							found = TRUE;
							break;
						}
					}
					if (found) {
						c = jis2utf16[i][0];
					} else {
						c = 0xffff;
					}
				}
			}
			*str = (const char *) p;
			return c;
		}
		break;

	default:
		/* case Q8TK_KANJI_ANK: */
		while ((c = *p ++)) {
			*str = (const char *)p;
			return c;
		}
		break;

	}
	return 0;
}




/*----------------------------------------------------------------------
 * 汎用文字列表示 (EUC/SJIS/UTF8/ANK)
 *
 * x, y, bg, fg       … 表示座標 および、前景色・背景色
 * reverse, underline … 真なら反転 および アンダーライン
 * cursor_pos         … カーソル表示バイト位置 (-1なら無し)
 * code, str          … 文字コード、文字列
 * start              … 表示開始バイト位置 (0..)
 * width              … 表示バイト数。 width==0 なら全て表示する
 *                       文字列が表示バイト数に満たない場合は空白を表示
 * 戻り値             … 実際に表示した文字バイト数
 *                       (width!=0 なら、widthと同じはず)
 *----------------------------------------------------------------------*/
static int q8gr_strings(int x, int y, int fg, int bg,
						int reverse, int underline,
						int cursor_pos,
						int code, const char *str, int start, int width)
{
	const char *p = str;
	unsigned int c, type, addr, rev;
	int w, pos = 0;
	int count = 0;


	/* width バイト分、表示 (width==0なら全て表示) */

	while (*p) {

		c = get_letter(code, &p);
		if (c == '\0') {
			break;
		}


		/* 描画開始チェック */

		if (c < 0x100) {
			/* 1バイト文字 */
			w = 1;
		} else {
			/* 2バイト文字 */
			w = 2;
		}

		if (count == 0) {
			/* 未描画 */

			if (pos < start && pos + w <= start) {
				/* 描画領域未達 */
				pos += w;
				continue;

			} else if (pos < start) {
				/* 描画領域干渉 */
				/* ダミー文字を 1バイト描画  */
				c = 0x8e;
				w = 1;

			} else {
				/* 描画領域到達 */
				;
			}
		}

#if 0 /* select_start .. select_end を反転させる (文字列の範囲指定) */
		if (0 <= select_start) {
			if (pos == select_start) {
				reverse = !(reverse);
			}
			if (0 <= select_end) {
				if (pos == select_end) {
					reverse = !(reverse);
				}
			}
		}
#endif

		rev = reverse;
		/* カーソル位置 */
		if (0 <= cursor_pos) {
			if (pos == cursor_pos) {
				q8gr_set_cursor_exist(TRUE);
				if (q8gr_get_cursor_blink()) {
					rev = !(rev);
				}
			}
		}

		pos += w;


		/* 文字コードをフォント種別／アドレスに変換 */

		kanji2addr(c, &addr, &type);



		/* q8gr_tvram[][] にセット */

		if (! CHECK_MASK_Y(y)) {
			if (! CHECK_MASK_X(x)) {
				tvram_set(x, y, bg, fg, rev, underline, type, addr);
			}
			if (type >= FONT_2_BYTE) {
				x++;
				count ++;
				if (width && width <= count) {
					break;
				}
				if (! CHECK_MASK_X(x)) {
					tvram_set(x, y, bg, fg, rev, underline, type + 1, addr);
				}
			}
		}
		x++;
		count ++;
		if (width && width <= count) {
			break;
		}
	}


	/* width 指定時、 余った部分はスペースを表示 */

	if (width) {
		for (; count < width;) {

#if 0 /* select_start .. select_end を反転させる (文字列の範囲指定) */
			if (0 <= select_start) {
				if (pos == select_start) {
					reverse = !(reverse);
				}
				if (0 <= select_end) {
					if (pos == select_end) {
						reverse = !(reverse);
					}
				}
			}
#endif

			rev = reverse;
			/* カーソル位置 */
			if (0 <= cursor_pos) {
				if (pos == cursor_pos) {
					q8gr_set_cursor_exist(TRUE);
					if (q8gr_get_cursor_blink()) {
						rev = !(rev);
					}
				}
			}

			q8gr_putchar(x, y, fg, bg, rev, underline, ' ');

			pos ++;
			x ++;
			count ++;
		}
	}
	return count;
}

/*----------------------------------------------------------------------
 * ANK文字 putc
 *
 * x, y, bg, fg      … 表示座標 および、前景色・背景色
 * reverse, unsigned … 真なら反転 および アンダーライン
 * c                 … 文字コード (0x00..0xff)
 *----------------------------------------------------------------------*/
static void q8gr_putchar(int x, int y, int fg, int bg,
						 int reverse, int underline, int c)
{
	if (! CHECK_MASK_X(x) && ! CHECK_MASK_Y(y)) {
		tvram_set(x, y, bg, fg, reverse, underline,
				  FONT_ANK, (c & 0xff) << 3);
	}
}


/*----------------------------------------------------------------------
 * 文字列の表示バイト長を返す

 * code, str … 文字コード, 文字列,
 *----------------------------------------------------------------------*/
int q8gr_strlen(int code, const char *str)
{
	const char *p = str;
	unsigned int h;
	int count = 0;

	while (*p) {

		h = get_letter(code, &p);
		if (h == '\0') {
			break;
		}

		if (h < 0x100) {
			count ++;
		} else {
			count += 2;
		}
	}
	return count;
}


/*----------------------------------------------------------------------
 * 文字列の 表示バイト pos が、文字のどの部分にあたるかを返す
 *
 * code, str, pos … 文字コード, 文字列, チェックする表示バイト位置
 * 戻り値         … 0 = 1バイト文字
 *                   1 = 2バイト文字の前半
 *                   2 = 2バイト文字の後半
 *----------------------------------------------------------------------*/
int q8gr_strchk(int code, const char *str, int pos)
{
	const char *p = str;
	unsigned int h;
	int count = 0;
	int type = 0;

	while (*p) {

		h = get_letter(code, &p);
		if (h == '\0') {
			break;
		}

		if (h < 0x100) {
			type = 0;
		} else {
			type = 1;
		}


		if (pos == count) {
			return type;
		}
		if (type == 1) {
			count ++;
			if (pos == count) {
				return 2;
			}
		}
		count ++;
	}
	return 0;
}


/*--------------------------------------------------------------
 * エントリ用文字列削除
 *
 * code    … 文字コード
 * str     … 文字列。このワークを直接書き換える
 * del_pos … 削除する表示バイト位置。以降の文字列は前に詰める。
 * 戻り値  … 削除したバイト数(半角=1、全角=2、削除なし=0)
 *--------------------------------------------------------------*/
int q8gr_strdel(int code, char *str, int del_pos)
{
	char *p = str;
	char *q = p;
	int count = 0;
	unsigned int h;


	if (del_pos < 0) {
		return 0;
	}

	/* 表示バイト位置が、全角文字の途中なら、全角文字の先頭とする */
	if (q8gr_strchk(code, str, del_pos) == 2) {
		del_pos --;
	}


	while (*p) {

		h = get_letter(code, (const char **)&p);
		if (h == '\0') {
			return 0;
		}

		if (count == del_pos) {
			memmove(q, p, strlen(p) + 1);
			if (h < 0x100) {
				return 1;
			} else {
				return 2;
			}
		}

		if (h < 0x100) {
			count ++;
		} else {
			count += 2;
		}

		q = p;
	}
	return 0;
}



/*--------------------------------------------------------------
 * エントリ用文字列挿入
 *
 * code    … 文字コード
 * str     … 文字列。このワークを直接書き換える (サイズは十分にあること)
 * add_pos … 挿入する表示バイト位置。以降の文字列は後ろにずらす。
 * add_chr … 挿入する文字 (現時点では、ASCIIのみ対応)
 * 戻り値  … 文字を挿入したら、真。しなかったら、偽
 *--------------------------------------------------------------*/
int q8gr_stradd(int code, char *str, int add_pos, int add_chr)
{
	char *p = str;
	int count = 0;
	unsigned int h;


	if (add_pos < 0) {
		return FALSE;
	}

	/* 表示バイト位置が、全角文字の途中なら、全角文字の先頭とする */
	if (q8gr_strchk(code, str, add_pos) == 2) {
		add_pos --;
	}


	for (;;) {
		if (count == add_pos) {
			memmove(p + 1, p, strlen(p) + 1);
			*p = (char)add_chr;
			return TRUE;
		}

		if (*p == '\0') {
			break;
		}

		h = get_letter(code, (const char **)&p);
		if (h == '\0') {
			break;
		}

		if (h < 0x100) {
			count ++;
		} else {
			count += 2;
		}
	}
	return FALSE;
}



/*--------------------------------------------------------------
 * 文字列を size バイト分、コピー
 *
 * code … 文字コード
 * dst  … コピー先文字列
 * src  … コピー元文字列
 * size … コピーするサイズ
 *         多バイト文字の途中でサイズに達した場合、
 *         その文字はコピーされない。
 * ※ 途中で指定サイズに達した場合は末端は \0 にならない。
 *    また、指定サイズに満たないエリアは \0 を埋める。 (strncpy と同様)
 *--------------------------------------------------------------*/
void q8gr_strncpy(int code, char *dst, const char *src, int size)
{
	const char *p = src;
	const char *q = p;
	int len = (int) strlen(src);
	int esize;

	if (size < 0) {
		return;
	}

	/* 十分なサイズがあれば、まるごとコピー */
	if (size >= len) {
		strncpy(dst, src, size);
		return;
	}

	/* 十分なサイズがなければ、多バイト文字の区切れ目を探しだす */
	for (;;) {

		if (get_letter(code, &p) == '\0') {
			/* \0 が返る時は、 p は壊れているので、文字列の末尾とみなす */
			p = &src[ len ];
		}

		/* p が size を超えれば、サイズオーバー。他バイト文字の途中で中断 */
		if ((p - src) > size) {
			break;
		}

		q = p;

		/* size に一致しても、サイズオーバー。他バイト文字の切れ目で中断 */
		if ((p - src) == size) {
			break;
		}
	}


	/* 有効サイズ */
	esize = (int)(q - src);

	/* 有効サイズ分コピー */
	strncpy(dst, src, esize);
	if (esize < size) {
		/* 有効サイズ以降は 0 埋め */
		memset(&dst[esize], 0, size - esize);
	}
}




/*--------------------------------------------------------------
 * 文字コードをなんとなく判断する
 *
 * buffer … 文字をセットしたバッファ
 * size   … バッファのサイズ
 * 戻り値 … 文字コード
 * ※ buffer にあるデータを、size バイト分チェックする。
 * ※ buffer には、 \0 を含むすべての文字がセット可能
 *--------------------------------------------------------------
 *
 * ASCII
 *              | 1バイト目
 *    ----------+-----------------
 *    制御      | 0x00..0x1F, 0x7F
 *    文字      | 0x00..0x7E
 *
 *    ※ ESC(0x1B) があれば JIS だと思うが、今回は無視
 *
 * EUC-JP
 *              | 1バイト目  | 2バイト目  | 3バイト目
 *    ----------+------------+------------+-----------
 *    半角カナ  | 0x8E       | 0xA1..0xDF |
 *    漢字      | 0xA1..0xFE | 0xA1..0xFE |
 *    補助漢字  | 0x8F       | 0xA1..0xFE | 0xA1..0xFE
 *
 *    ※補助漢字は、今回は無視
 *
 * SJIS
 *              | 1バイト目              | 2バイト目
 *    ----------+------------------------+-----------------------
 *    半角カナ  | 0xA1..0xDF             |
 *    漢字      | 0x81..0x9F、0xE0..0xFC | 0x40..0x7E、0x80..0xFC
 *
 *    ※ 漢字の 1バイト目 0xF0..0xFC は予備領域
 *       (..0xEFで、1面の文字は全て収まる)
 *
 * UTF-8
 *              | 1バイト目  | 2バイト目  | 3バイト目  | 4バイト目
 *    ----------+------------+------------+------------+-----------
 *    ..U+000F  | 0x00..0x7F |            |            |
 *    ..U+07FF  | 0xC2..0xDF | 0x80..0xBF |            |
 *    ..U+FFFF  | 0xE0..0xEF | 0x80..0xBF | 0x80..0xBF |
 *    ..U+1FFFFF| 0xF0..0xF7 | 0x80..0xBF | 0x80..0xBF | 0x80..0xBF
 *
 *    ※ ファイル先頭に EF BB BF (BOM) が付加される場合もある
 *       0xC0..0xC1は、セキュリティ上、使用禁止らしい
 *       0xF4..0xFDは、使われなくなったらしい (5..6バイト長は廃止)
 *       0xFE..0xFFは、使用しない
 *
 * UTF-16
 *              | 1バイト目  | 2バイト目
 *    ----------+------------+------------
 *    UTF-16LE  | 0xFF       | 0xFE
 *    UTF-16BE  | 0xFE       | 0xFF
 *
 *    ※ ファイル先頭に BOM (FEFF) が付加される場合、
 *       このバイト並びでエンディアンネスが決まる
 *       BOM が付加されない場合もある
 *--------------------------------------------------------------*/
int q8gr_strcode(const char *buffer, int size)
{
	const unsigned char *p;
	unsigned char c;
	int sz;

	int found_not_ascii = FALSE;


	/* UTF-8 は値に制約があるので、規定外はすぐにわかる。また、
	 * 日本語の場合は 3バイト長で、EUC や SJIS と値が重なる範囲が狭い。
	 * よって、最初にチェックしても誤認識しにくいだろう、と勝手に想像 */


	/* ASCII か、 UTF-8 か、それ以外かを判別
	 * ・0x00..0x7f しか出現しない → ASCII
	 * ・UTF-8 の規則に従っている  → UTF-8 (多分)
	 * ・いずれでもない            → それ以外 */

	p = (const unsigned char *)buffer;
	sz = size;

	while (sz) {
		c = *p;
		if (c <= 0x7f) {
			/* ASCII */
			p++;
			sz--;
			continue;

		} else {
			/* UTF8 ? */

			found_not_ascii = TRUE;
			/* 継続     10xxxxxx */
			if (c < 0xc0) {
				/* NG */
				break;

			} else {
				int len;
				if (c < 0xe0) {
					/* 2byte    110xxxxx */
					len = 2;
				} else if (c < 0xf0) {
					/* 3byte    1110xxxx */
					len = 3;
				} else if (c < 0xf8) {
					/* 4byte    11110xxx */
					len = 4;
				} else {
					/* NG */
					break;
				}

				p++;
				sz--;
				len--;
				for (; len && sz ; len--, sz--) {
					c = *p++;
					/* 継続     10xxxxxx */
					if ((c & 0xc0) == 0x80) {
						/* OK */
					} else {
						/* NG */
						break;
					}
				}
			}
		}
	}
	/* ASCII、ないし UTF-8 の場合は、 sz == 0 となる */
	if (sz == 0) {
		if (found_not_ascii) {
			return Q8TK_KANJI_UTF8;
		} else {
			return Q8TK_KANJI_ANK;
		}
	}
	/* sz > 0 なら、 UTF-16 か EUC か SJIS */


	/* UTF-16 か、それ以外かを判別
	 * ・先頭が 0xff 0xfe → UTF-16LE (多分)
	 * ・先頭が 0xfe 0xff → UTF-16BE (多分)
	 * ・いずれでもない   → それ以外 */

	p = (const unsigned char *) buffer;
	sz = size;

	if ((sz >= 2) && ((sz % 2) == 0)) {
		if (p[0] == 0xff && p[1] == 0xfe) {
			return Q8TK_KANJI_UTF16LE_DUMMY;
		}
		if (p[0] == 0xfe && p[1] == 0xff) {
			return Q8TK_KANJI_UTF16BE_DUMMY;
		}
	}

	/* EUC か SJIS かを判別
	 * ・EUC 半角 の範囲から外れている → SJIS
	 * ・0xfd、0xfe が含まれる         → EUC
	 * ・EUC 漢字の範囲内のみ          → EUC  (多分)
	 * ・それ以外                      → SJIS か その他 */

	p = (const unsigned char *)buffer;
	sz = size;

	while (sz) {
		c = *p++;
		if (c <= 0x7f) {
			/* ASCII */
			sz --;
			continue;

		} else {
			if (c == 0x8e) {
				/* EUC半角? */
				sz --;
				if (sz == 0) {
					break;
				}
				c = *p++;
				if (0xa1 <= c && c <= 0xdf) {
					/* yes, でもSJISかも */
					sz --;
					continue;
				} else {
					/* EUCじゃない */
					break;
				}

			} else if (0xa1 <= c && c <= 0xfc) {
				/* EUC全角? */
				sz --;
				if (sz == 0) {
					break;
				}
				c = *p++;
				if (0xa1 <= c && c <= 0xfc) {
					/* yes, でもSJISかも */
					sz --;
					continue;
				} else if (0xfd <= c && c <= 0xfe) {
					/* EUC確定 */
					sz = 0;
					break;
				} else {
					/* EUCじゃない */
					break;
				}
			} else if (0xfd <= c && c <= 0xfe) {
				/* EUC確定 */
				sz = 0;
				break;
			} else {
				/* EUCじゃない */
				break;
			}
		}
	}
	/* EUC の場合は、 sz == 0 となる */
	if (sz == 0) {
		return Q8TK_KANJI_EUC;
	} else {
		return Q8TK_KANJI_SJIS;
	}

	/* この処理だと、文字コードが不明な場合は SJIS になる。ま、いいか */
}





void q8gr_draw_mouse(int x, int y)
{
	if (0 <= menu_mouse_x) {
		q8gr_tvram[q8gr_tvram_flip][menu_mouse_y][menu_mouse_x].mouse = FALSE;
	}


	if (0 <= x && x < Q8GR_TVRAM_W &&
		0 <= y && y < Q8GR_TVRAM_H) {

		q8gr_tvram[q8gr_tvram_flip][y][x].mouse = TRUE;
		menu_mouse_x = x;
		menu_mouse_y = y;

	} else {
		menu_mouse_x = -1;
		menu_mouse_y = -1;
	}

}








int q8gr_draw_label(int x, int y, int fg, int bg,
					int reverse, int underline, int code, const char *str,
					int width, void *p)
{
	if (p == NULL) {
		fg = Q8GR_PALETTE_FONT_BG;
	}

	return q8gr_strings(x, y, fg, bg, reverse, underline, -1, code, str,
						0, width);
}


/*----------------------------------------------------------------------
 * ビットマップ
 *----------------------------------------------------------------------
 *     ｘ
 * ｙ  □□□□□□□□□□□□↑
 *     □□□□□□□□□□□□｜
 *     □□□□□□□□□□□□sy
 *     □□□□□□□□□□□□｜
 *     □□□□□□□□□□□□↓
 *      ←────sx────→
 *
 *----------------------------------------------------------------------*/
void q8gr_draw_bitmap(int x, int y, int fg, int bg,
					  int sx, int sy, int addr)
{
	int i, j;
	int a = addr;

	for (j = 0; j < sy; j ++) {
		for (i = 0; i < sx; i ++) {

			if (! CHECK_MASK_Y(y + j)) {
				if (! CHECK_MASK_X(x + i)) {
					tvram_set(x + i, y + j, bg, fg, FALSE, FALSE,
							  FONT_BMP, a);
				}
			}

			a += 16;
		}
	}
}
