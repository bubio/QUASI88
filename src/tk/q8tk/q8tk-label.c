/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"


/*---------------------------------------------------------------------------
 * ラベル (LABEL)
 *---------------------------------------------------------------------------
 * ・(表示用の)文字列を保持できる
 * ・さまざまなコンテナの子になる。
 * ・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_label_new(const char *label)
 *    文字列 label をもつラベルの生成。
 *
 * void q8tk_label_set(Q8tkWidget *w, const char *label)
 *    ラベルの文字列を label に変更する。
 *
 * void q8tk_label_set_reverse(Q8tkWidget *w, int reverse)
 *    ラベルの文字列の反転表示を設定する。 reverse が真なら反転する。
 *
 * void q8tk_label_set_color(Q8tkWidget *w,
 *                           int foreground, int background)
 *    ラベルの文字列の色を設定する。色は Q8GR_PALETTE_xxxx で指定する。
 *    この値が負なら、デフォルトの色に戻す。
 * --------------------------------------------------------------------------
 * 【LABEL】    子は持てない
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_label_new(const char *label)
{
	Q8tkWidget *w;

	if (label == NULL) {
		label = "";
	}

	w = malloc_widget();
	w->type = Q8TK_TYPE_LABEL;
	w->name = (char *)malloc(strlen(label) + 1);
	w->sensitive = TRUE;

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, label);
	w->code = q8tk_kanji_code;

	w->stat.label.foreground = Q8GR_PALETTE_FOREGROUND;
	w->stat.label.background = Q8GR_PALETTE_BACKGROUND;
	w->stat.label.reverse    = FALSE;
	w->stat.label.width      = 0;

	return w;
}

void q8tk_label_set(Q8tkWidget *w, const char *label)
{
	if (label == NULL) {
		label = "";
	}

	if (w->name) {
		free(w->name);
	}
	w->name = (char *)malloc(strlen(label) + 1);

	Q8tkAssert(w->name, "memory exhoused");

	strcpy(w->name, label);
	w->code = q8tk_kanji_code;

	set_construct_flag(TRUE);
}

void q8tk_label_set_reverse(Q8tkWidget *w, int reverse)
{
	if (w->stat.label.reverse != reverse) {
		w->stat.label.reverse = reverse;
		set_construct_flag(TRUE);
	}
}

void q8tk_label_set_color(Q8tkWidget *w,
						  int foreground, int background)
{
	if (foreground < 0) {
		foreground = Q8GR_PALETTE_FOREGROUND;
	}
	if (background < 0) {
		background = Q8GR_PALETTE_BACKGROUND;
	}

	if (w->stat.label.foreground != foreground) {
		w->stat.label.foreground = foreground;
		set_construct_flag(TRUE);
	}
	if (w->stat.label.background != background) {
		w->stat.label.background = background;
		set_construct_flag(TRUE);
	}
}


/*---------------------------------------------------------------------------
 * ビットマップ (BMP)
 *---------------------------------------------------------------------------
 *・ビットマップ画像を保持できる
 *・さまざまなコンテナの子になる。(と思う)
 *・シグナル … 無し
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_bitmap_new(int x_size, int y_size,
 *                             const unsigned char *bmp)
 *    画像 bmp をもつビットマップの生成。
 *    画像サイズは x_size * y_size だが、表示は文字サイズ単位となる。
 *
 * void q8tk_bitmap_set_color(Q8tkWidget *w,
 *                            int foreground, int background)
 *    ビットマップの色を設定する。
 * --------------------------------------------------------------------------
 * 【BMP】    子は持てない
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_bitmap_new(int x_size, int y_size,
							const unsigned char *bmp)
{
	Q8tkWidget *w;
	int x, y;
	int sx = (x_size + 7) / 8;
	int sy = (y_size + 15) / 16;
	const unsigned char *src = bmp;
	byte *dst;

	w = malloc_widget();
	w->type = Q8TK_TYPE_BMP;
	w->sensitive = TRUE;

	w->stat.bmp.foreground = Q8GR_PALETTE_FOREGROUND;
	w->stat.bmp.background = Q8GR_PALETTE_BACKGROUND;
	w->stat.bmp.x_size = x_size;
	w->stat.bmp.y_size = y_size;
	w->stat.bmp.width = sx;
	w->stat.bmp.height = sy;

	dst = (byte *) calloc(1, sx * sy * 16);
	Q8tkAssert(sx * sy * 16 < BITMAP_DATA_MAX_SIZE, "bmp size too big");
	Q8tkAssert(dst != NULL, "memory exhoused");

	w->stat.bmp.alloc = widget_bitmap_table_add(dst);
	w->stat.bmp.index = w->stat.bmp.alloc;
	Q8tkAssert(w->stat.bmp.alloc >= 0, "memory exhoused");


	if (bmp) {
		for (y = 0; y < sy * 16; y++) {
			for (x = 0; x < sx; x++) {
				if (y < y_size) {
					dst[((((y / 16) * sx) + x) * 16) + (y % 16) ] = *src++;
				}
			}
		}
	}

	return w;
}

void q8tk_bitmap_set_color(Q8tkWidget *w,
						   int foreground, int background)
{
	if (foreground < 0) {
		foreground = Q8GR_PALETTE_FOREGROUND;
	}
	if (background < 0) {
		background = Q8GR_PALETTE_BACKGROUND;
	}

	if (w->stat.bmp.foreground != foreground) {
		w->stat.bmp.foreground = foreground;
		set_construct_flag(TRUE);
	}
	if (w->stat.bmp.background != background) {
		w->stat.bmp.background = background;
		set_construct_flag(TRUE);
	}
}



/*---------------------------------------------------------------------------
 * 垂直セパレータ (VSEPARATOR)
 *---------------------------------------------------------------------------
 * ・子は持てない。
 * ・長さが、親ウィジットの大きさにより、動的に変わる。
 * ・シグナル … なし
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_vseparator_new(void)
 *    垂直セパレータの生成
 * --------------------------------------------------------------------------
 * 【VSEPARATOR】    子は持てない
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_vseparator_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_VSEPARATOR;
	w->sensitive = TRUE;

	w->stat.separator.style = Q8TK_SEPARATOR_NORMAL;

	return w;
}

void q8tk_vseparator_set_style(Q8tkWidget *widget, int style)
{
	widget->stat.separator.style = style;
}




/*---------------------------------------------------------------------------
 * 水平セパレータ (HSEPARATOR)
 *---------------------------------------------------------------------------
 * ・子は持てない。
 * ・長さが、親ウィジットの大きさにより、動的に変わる。
 * ・シグナル … なし
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_hseparator_new(void)
 *    水平セパレータの生成
 * --------------------------------------------------------------------------
 * 【VSEPARATOR】    子は持てない
 *
 *---------------------------------------------------------------------------*/
Q8tkWidget *q8tk_hseparator_new(void)
{
	Q8tkWidget *w;

	w = malloc_widget();
	w->type = Q8TK_TYPE_HSEPARATOR;
	w->sensitive = TRUE;

	w->stat.separator.style = Q8TK_SEPARATOR_NORMAL;

	return w;
}

void q8tk_hseparator_set_style(Q8tkWidget *widget, int style)
{
	widget->stat.separator.style = style;
}
