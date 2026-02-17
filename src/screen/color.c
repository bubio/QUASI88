/************************************************************************/
/*                                                                      */
/* 画面の表示                                                           */
/*                                                                      */
/************************************************************************/

#include "quasi88.h"
#include "screen.h"
#include "screen-common.h"
#include "menu-screen.h"
#include "menu.h"
#include "graph.h"

#include "pc88main.h"

#include "q8tk.h"



Ulong color_pixel[16];							/* 色コード				*/
Ulong color_half_pixel[16][16];					/* 色補完時の色コード	*/
Ulong menu_pixel[12];							/* 色コード				*/
Ulong menu_half_pixel[12][12];					/* 色補完時の色コード	*/
Ulong black_pixel;								/* 黒の色コード			*/


static int added_emu_color;
static unsigned long added_emu_pixel[120 + 16];
static int added_menu_color;
static unsigned long added_menu_pixel[66 + 12];

/*----------------------------------------------------------------------
 * 全ての色を解放する
 *----------------------------------------------------------------------*/
void remove_palette(void)
{
	if (added_emu_color) {
		graph_remove_color(added_emu_color, added_emu_pixel);
		added_emu_color = 0;
	}
	if (added_menu_color) {
		graph_remove_color(added_menu_color, added_menu_pixel);
		added_menu_color = 0;
	}
}

/*----------------------------------------------------------------------
 * メニュー用の色設定
 *----------------------------------------------------------------------*/
static void get_menu_palette(PC88_PALETTE_T pal[12])
{
	static const int preset_pal[3][Q8GR_PALETTE_SIZE] = {
		{	/* Classic Thema */
			0x000000,						/* Q8GR_PALETTE_BLACK */
			0xffffff,						/* Q8GR_PALETTE_WHITE */
			0xff0000,						/* Q8GR_PALETTE_RED */
			0x00ff00,						/* Q8GR_PALETTE_GREEN */
			MENU_COLOR_CLASSIC_FOREGROUND,	/* Q8GR_PALETTE_FOREGROUND */
			MENU_COLOR_CLASSIC_BACKGROUND,	/* Q8GR_PALETTE_BACKGROUND */
			MENU_COLOR_CLASSIC_LIGHT,		/* Q8GR_PALETTE_LIGHT */
			MENU_COLOR_CLASSIC_SHADOW,		/* Q8GR_PALETTE_SHADOW */
			MENU_COLOR_CLASSIC_FONT_FG,		/* Q8GR_PALETTE_FONT_FG */
			MENU_COLOR_CLASSIC_FONT_BG,		/* Q8GR_PALETTE_FONT_BG */
			MENU_COLOR_CLASSIC_SCALE_SLD,	/* Q8GR_PALETTE_SCALE_SLD */
			MENU_COLOR_CLASSIC_SCALE_BAR,	/* Q8GR_PALETTE_SCALE_BAR */
		},
		{	/* Light Thema */
			0x000000,						/* Q8GR_PALETTE_BLACK */
			0xffffff,						/* Q8GR_PALETTE_WHITE */
			0xff0000,						/* Q8GR_PALETTE_RED */
			0x00ff00,						/* Q8GR_PALETTE_GREEN */
			MENU_COLOR_LIGHT_FOREGROUND,	/* Q8GR_PALETTE_FOREGROUND */
			MENU_COLOR_LIGHT_BACKGROUND,	/* Q8GR_PALETTE_BACKGROUND */
			MENU_COLOR_LIGHT_LIGHT,			/* Q8GR_PALETTE_LIGHT */
			MENU_COLOR_LIGHT_SHADOW,		/* Q8GR_PALETTE_SHADOW */
			MENU_COLOR_LIGHT_FONT_FG,		/* Q8GR_PALETTE_FONT_FG */
			MENU_COLOR_LIGHT_FONT_BG,		/* Q8GR_PALETTE_FONT_BG */
			MENU_COLOR_LIGHT_SCALE_SLD,		/* Q8GR_PALETTE_SCALE_SLD */
			MENU_COLOR_LIGHT_SCALE_BAR,		/* Q8GR_PALETTE_SCALE_BAR */
		},
		{	/* Dark Thema */
			0x000000,						/* Q8GR_PALETTE_BLACK */
			0xffffff,						/* Q8GR_PALETTE_WHITE */
			0xff0000,						/* Q8GR_PALETTE_RED */
			0x00ff00,						/* Q8GR_PALETTE_GREEN */
			MENU_COLOR_DARK_FOREGROUND,		/* Q8GR_PALETTE_FOREGROUND */
			MENU_COLOR_DARK_BACKGROUND,		/* Q8GR_PALETTE_BACKGROUND */
			MENU_COLOR_DARK_LIGHT,			/* Q8GR_PALETTE_LIGHT */
			MENU_COLOR_DARK_SHADOW,			/* Q8GR_PALETTE_SHADOW */
			MENU_COLOR_DARK_FONT_FG,		/* Q8GR_PALETTE_FONT_FG */
			MENU_COLOR_DARK_FONT_BG,		/* Q8GR_PALETTE_FONT_BG */
			MENU_COLOR_DARK_SCALE_SLD,		/* Q8GR_PALETTE_SCALE_SLD */
			MENU_COLOR_DARK_SCALE_BAR,		/* Q8GR_PALETTE_SCALE_BAR */
		},
	};

	int i;
	const int *menu_pal;

	menu_pal = preset_pal[ menu_thema ];

	for (i = 0; i < Q8GR_PALETTE_SIZE; i++) {
		pal[ i ].red   = (menu_pal[i] >> 16) & 0xff;
		pal[ i ].green = (menu_pal[i] >>  8) & 0xff;
		pal[ i ].blue  = (menu_pal[i] >>  0) & 0xff;
	}
}

void menu_palette(int half_interp)
{
	PC88_PALETTE_T color[78];
	int i, j, num;
	PC88_PALETTE_T syspal[12];

	/* EMU の色を一旦すべて削除 */
	if (added_emu_color) {
		graph_remove_color(added_emu_color, added_emu_pixel);
		added_emu_color = 0;
	}


	/* これより、 MENU の色を設定する */
	get_menu_palette(syspal);

	if (added_menu_color) {
		graph_remove_color(added_menu_color, added_menu_pixel);
		added_menu_color = 0;
	}

	/* メニュー用カラー 12色分を設定 */
	for (i = 0; i < 12; i++) {
		color[i].red   = syspal[i].red;
		color[i].green = syspal[i].green;
		color[i].blue  = syspal[i].blue;
	}
	num = 12;

	/* 色補完用カラー 66色分を設定 */
	if (half_interp) {
		for (i = 0; i < 12; i++) {
			for (j = i + 1; j < 12; j++) {
				color[num].red   = (color[i].red  >> 1) + (color[j].red  >> 1);
				color[num].green = (color[i].green >> 1) + (color[j].green >> 1);
				color[num].blue  = (color[i].blue >> 1) + (color[j].blue >> 1);
				num++;
			}
		}
	}

	/* システムに色確保を要求 */
	graph_add_color(color, num, added_menu_pixel);
	added_menu_color = num;

	/* メニュー用の色ピクセル値を取得 */
	for (i = 0; i < 12; i++) {
		menu_pixel[i] = added_menu_pixel[i];
	}

	/* 色補完用のピクセル値を取得 */
	if (half_interp) {
		for (i = 0; i < 12; i++) {
			menu_half_pixel[i][i] = menu_pixel[i];
		}
		num = 12;
		for (i = 0; i < 12; i++) {
			for (j = i + 1; j < 12; j++) {
				menu_half_pixel[i][j] = added_menu_pixel[num];
				menu_half_pixel[j][i] = menu_half_pixel[i][j];
				num++;
			}
		}
	}

	/* 黒ピクセル値を覚えておく */
	black_pixel = menu_pixel[ Q8GR_PALETTE_BLACK ];

	set_screen_dirty_frame();			/* 全領域 更新 */
	frameskip_counter_reset();			/* 次回描画 */
}


/*----------------------------------------------------------------------
 * パレット設定
 *----------------------------------------------------------------------*/
void emu_palette(PC88_PALETTE_T syspal[], int half_interp)
{
	PC88_PALETTE_T color[120 + 16];
	int i, j, num;

	if (added_emu_color) {
		graph_remove_color(added_emu_color, added_emu_pixel);
		added_emu_color = 0;
	}

	/* 88のパレット16色分を設定 */
	for (i = 0; i < 16; i++) {
		color[i].red   = syspal[i].red;
		color[i].green = syspal[i].green;
		color[i].blue  = syspal[i].blue;
	}
	num = 16;

	/* 色補完用カラー 120色分を設定 */
	if (half_interp) {
		for (i = 0; i < 16; i++) {
			for (j = i + 1; j < 16; j++) {
				color[num].red   = (color[i].red  >> 1) + (color[j].red  >> 1);
				color[num].green = (color[i].green >> 1) + (color[j].green >> 1);
				color[num].blue  = (color[i].blue >> 1) + (color[j].blue >> 1);
				num++;
			}
		}
	}

	/* システムに色確保を要求 */
	graph_add_color(color, num, added_emu_pixel);
	added_emu_color = num;

	/* 88のピクセル値を取得 */
	for (i = 0; i < 16; i++) {
		color_pixel[i] = added_emu_pixel[i];
	}

	/* 色補完用のピクセル値を取得 */
	if (half_interp) {
		for (i = 0; i < 16; i++) {
			color_half_pixel[i][i] = color_pixel[i];
		}
		num = 16;
		for (i = 0; i < 16; i++) {
			for (j = i + 1; j < 16; j++) {
				color_half_pixel[i][j] = added_emu_pixel[num];
				color_half_pixel[j][i] = color_half_pixel[i][j];
				num++;
			}
		}
	}
}


/***********************************************************************
 * 描画の際に使用する、実際のパレット情報を引数 syspal にセットする
 ************************************************************************/
void    screen_get_emu_palette(PC88_PALETTE_T pal[16])
{
	int i;

	/* VRAM の カラーパレット設定   pal[0]..[7] */

	if (grph_ctrl & GRPH_CTRL_COLOR) {			/* VRAM カラー */

		if (monitor_analog) {
			for (i = 0; i < 8; i++) {
				pal[i].red   = vram_palette[i].red   * 73 / 2;
				pal[i].green = vram_palette[i].green * 73 / 2;
				pal[i].blue  = vram_palette[i].blue  * 73 / 2;
			}
		} else {
			for (i = 0; i < 8; i++) {
				pal[i].red   = vram_palette[i].red   ? 0xff : 0;
				pal[i].green = vram_palette[i].green ? 0xff : 0;
				pal[i].blue  = vram_palette[i].blue  ? 0xff : 0;
			}
		}

	} else {									/* VRAM 白黒 */

		if (monitor_analog) {
			pal[0].red   = vram_bg_palette.red   * 73 / 2;
			pal[0].green = vram_bg_palette.green * 73 / 2;
			pal[0].blue  = vram_bg_palette.blue  * 73 / 2;
		} else {
			pal[0].red   = vram_bg_palette.red   ? 0xff : 0;
			pal[0].green = vram_bg_palette.green ? 0xff : 0;
			pal[0].blue  = vram_bg_palette.blue  ? 0xff : 0;
		}
		for (i = 1; i < 8; i++) {
			pal[i].red   = 0;
			pal[i].green = 0;
			pal[i].blue  = 0;
		}

	}


	/* TEXT の カラーパレット設定   pal[8]..[15] */

	if (grph_ctrl & GRPH_CTRL_COLOR) {          /* VRAM カラー */

		/* TEXT 白黒の場合は 黒=[8],白=[15] を使うので問題なし  */
		for (i = 8; i < 16; i++) {
			pal[i].red   = (i & 0x02) ? 0xff : 0;
			pal[i].green = (i & 0x04) ? 0xff : 0;
			pal[i].blue  = (i & 0x01) ? 0xff : 0;
		}

	} else {									/* VRAM 白黒   */

		if (misc_ctrl & MISC_CTRL_ANALOG) {			/* アナログパレット時 */

			if (monitor_analog) {
				for (i = 8; i < 16; i++) {
					pal[i].red   = vram_palette[i & 0x7].red   * 73 / 2;
					pal[i].green = vram_palette[i & 0x7].green * 73 / 2;
					pal[i].blue  = vram_palette[i & 0x7].blue  * 73 / 2;
				}
			} else {
				for (i = 8; i < 16; i++) {
					pal[i].red   = vram_palette[i & 0x7].red   ? 0xff : 0;
					pal[i].green = vram_palette[i & 0x7].green ? 0xff : 0;
					pal[i].blue  = vram_palette[i & 0x7].blue  ? 0xff : 0;
				}
			}

		} else {									/* デジタルパレット時 */
			for (i = 8; i < 16; i++) {
				pal[i].red   = (i & 0x02) ? 0xff : 0;
				pal[i].green = (i & 0x04) ? 0xff : 0;
				pal[i].blue  = (i & 0x01) ? 0xff : 0;
			}
		}

	}
}
