/************************************************************************/
/*                                                                      */
/* 画面の表示                                                           */
/*                                                                      */
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "quasi88.h"
#include "debug.h"
#include "screen.h"
#include "screen-common.h"
#include "screen-func.h"
#include "graph.h"

#include "crtcdmac.h"
#include "pc88main.h"

#include "statusbar.h"
#include "toolbar.h"

#include "intr.h"
#include "q8tk.h"



static	int		do_skip_draw = FALSE;			/* 今回スキップするか?	*/
static	int		already_skip_draw = FALSE;		/* 前回スキップしたか?	*/

static	int		skip_counter = 0;		/* 連続何回スキップしたか		*/
static	int		skip_count_max = 15;	/* これ以上連続スキップしたら
												一旦、強制的に描画する	*/

static	int		frame_counter = 0;		/* フレームスキップ用のカウンタ */



static	int		blink_ctrl_cycle   = 1; /* カーソル表示用のカウンタ		*/
static	int		blink_ctrl_counter = 0; /*				〃				*/

static	int		drawn_count;			/* fps計算用の描画回数カウンタ	*/





/*----------------------------------------------------------------------
 * GVRAM/TVRAMを画像バッファに転送する関数の、リストを作成
 *      この関数は、 bpp ・ サイズ ・ エフェクト の変更時に呼び出す。
 *----------------------------------------------------------------------*/
static  int (*vram2screen_list[4][4][2])(T_RECTANGLE *updated_area);
static  void (*screen_buf_init_p)(void);

static  int (*menu2screen_p)(T_RECTANGLE *updated_area);
static  int (*tool2screen_p)(T_RECTANGLE *updated_area);
static  int (*stat2screen_p)(T_RECTANGLE *updated_area);

void set_vram2screen_list(void)
{
	typedef     int (*V2S_FUNC_TYPE)(T_RECTANGLE * updated_area);
	typedef     V2S_FUNC_TYPE   V2S_FUNC_LIST[4][4][2];
	V2S_FUNC_LIST *list = NULL;


	if (DEPTH <= 8) {           /* ----------------------------------------- */

#ifdef  SUPPORT_8BPP
		switch (now_screen_size) {
		case SCREEN_SIZE_FULL:
			if (use_interlace == 0) {
				if (update_info.write_only)   {
					list = &vram2screen_list_F_N__8_d;
				} else                     {
					list = &vram2screen_list_F_N__8;
				}
			} else if (use_interlace >  0) {
				list = &vram2screen_list_F_I__8;
			} else                         {
				list = &vram2screen_list_F_S__8;
			}
			menu2screen_p = menu2screen_F_N__8;
			tool2screen_p = tool2screen_F_N__8;
			stat2screen_p = stat2screen_F_N__8;
			break;

		case SCREEN_SIZE_HALF:
			if (now_half_interp) {
				list = &vram2screen_list_H_P__8;
				menu2screen_p = menu2screen_H_P__8;
				tool2screen_p = tool2screen_H_P__8;
				stat2screen_p = stat2screen_H_P__8;
			} else                 {
				list = &vram2screen_list_H_N__8;
				menu2screen_p = menu2screen_H_N__8;
				tool2screen_p = tool2screen_H_N__8;
				stat2screen_p = stat2screen_H_N__8;
			}
			break;

#ifdef  SUPPORT_DOUBLE
		case SCREEN_SIZE_DOUBLE:
			if (update_info.write_only) {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N__8_d;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I__8_d;
				} else                         {
					list = &vram2screen_list_D_S__8_d;
				}
			} else {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N__8;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I__8;
				} else                         {
					list = &vram2screen_list_D_S__8;
				}
			}
			menu2screen_p = menu2screen_D_N__8;
			tool2screen_p = tool2screen_D_N__8;
			stat2screen_p = stat2screen_D_N__8;
			break;
#endif
		}
		screen_buf_init_p  = screen_buf_init__8;
#else
		fprintf(stderr, "Error! This version is not support %dbpp !\n", DEPTH);
		exit(1);
#endif

	} else if (DEPTH <= 16) {   /* ----------------------------------------- */

#ifdef  SUPPORT_16BPP
		switch (now_screen_size) {
		case SCREEN_SIZE_FULL:
			if (use_interlace == 0) {
				if (update_info.write_only)   {
					list = &vram2screen_list_F_N_16_d;
				} else                     {
					list = &vram2screen_list_F_N_16;
				}
			} else if (use_interlace >  0) {
				list = &vram2screen_list_F_I_16;
			} else                         {
				list = &vram2screen_list_F_S_16;
			}
			menu2screen_p = menu2screen_F_N_16;
			tool2screen_p = tool2screen_F_N_16;
			stat2screen_p = stat2screen_F_N_16;
			break;

		case SCREEN_SIZE_HALF:
			if (now_half_interp) {
				list = &vram2screen_list_H_P_16;
				menu2screen_p = menu2screen_H_P_16;
				tool2screen_p = tool2screen_H_P_16;
				stat2screen_p = stat2screen_H_P_16;
			} else                 {
				list = &vram2screen_list_H_N_16;
				menu2screen_p = menu2screen_H_N_16;
				tool2screen_p = tool2screen_H_N_16;
				stat2screen_p = stat2screen_H_N_16;
			}
			break;

#ifdef  SUPPORT_DOUBLE
		case SCREEN_SIZE_DOUBLE:
			if (update_info.write_only) {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N_16_d;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I_16_d;
				} else                         {
					list = &vram2screen_list_D_S_16_d;
				}
			} else {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N_16;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I_16;
				} else                         {
					list = &vram2screen_list_D_S_16;
				}
			}
			menu2screen_p = menu2screen_D_N_16;
			tool2screen_p = tool2screen_D_N_16;
			stat2screen_p = stat2screen_D_N_16;
			break;
#endif
		}
		screen_buf_init_p  = screen_buf_init_16;
#else
		fprintf(stderr, "Error! This version is not support %dbpp !\n", DEPTH);
		exit(1);
#endif

	} else if (DEPTH <= 32) {   /* ----------------------------------------- */

#ifdef  SUPPORT_32BPP
		switch (now_screen_size) {
		case SCREEN_SIZE_FULL:
			if (use_interlace == 0) {
				if (update_info.write_only)   {
					list = &vram2screen_list_F_N_32_d;
				} else                     {
					list = &vram2screen_list_F_N_32;
				}
			} else if (use_interlace >  0) {
				list = &vram2screen_list_F_I_32;
			} else                         {
				list = &vram2screen_list_F_S_32;
			}
			menu2screen_p = menu2screen_F_N_32;
			tool2screen_p = tool2screen_F_N_32;
			stat2screen_p = stat2screen_F_N_32;
			break;

		case SCREEN_SIZE_HALF:
			if (now_half_interp) {
				list = &vram2screen_list_H_P_32;
				menu2screen_p = menu2screen_H_P_32;
				tool2screen_p = tool2screen_H_P_32;
				stat2screen_p = stat2screen_H_P_32;
			} else                 {
				list = &vram2screen_list_H_N_32;
				menu2screen_p = menu2screen_H_N_32;
				tool2screen_p = tool2screen_H_N_32;
				stat2screen_p = stat2screen_H_N_32;
			}
			break;

#ifdef  SUPPORT_DOUBLE
		case SCREEN_SIZE_DOUBLE:
			if (update_info.write_only) {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N_32_d;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I_32_d;
				} else                         {
					list = &vram2screen_list_D_S_32_d;
				}
			} else {
				if (use_interlace == 0) {
					list = &vram2screen_list_D_N_32;
				} else if (use_interlace >  0) {
					list = &vram2screen_list_D_I_32;
				} else                         {
					list = &vram2screen_list_D_S_32;
				}
			}
			menu2screen_p = menu2screen_D_N_32;
			tool2screen_p = tool2screen_D_N_32;
			stat2screen_p = stat2screen_D_N_32;
			break;
#endif
		}
		screen_buf_init_p  = screen_buf_init_32;
#else
		fprintf(stderr, "Error! This version is not support %dbpp !\n", DEPTH);
		exit(1);
#endif

	}

	memcpy(vram2screen_list, list, sizeof(vram2screen_list));
}


void clear_all_screen(void)
{
	if (update_info.draw_start) {
		(update_info.draw_start)();    /* システム依存の描画前処理 */
	}

	(screen_buf_init_p)();                      /* 画面全クリア(ボーダー含) */

	if (update_info.draw_finish) {
		(update_info.draw_finish)();    /* システム依存の描画後処理 */
	}
}



/*----------------------------------------------------------------------
 * GVRAM/TVRAM を screen_buf に転送する
 *
 *      int method == V_DIF … screen_dirty_flag に基づき、差分だけを転送
 *                 == V_ALL … 画面すべてを転送
 *
 *      戻り値     == -1    … 転送なし (画面に変化なし)
 *                 != -1    … 上位から 8ビットずつに、x0, y0, x1, y1 の
 *                             4個の unsigned 値がセットされる。ここで、
 *                                  (x0 * 8, y0 * 2) - (x1 * 8, y1 * 2)
 *                             で表される範囲が、転送した領域となる。
 *
 *      予め、 set_vram2screen_list で関数リストを生成しておくこと
 *----------------------------------------------------------------------*/
static  int     vram2screen(int method, T_RECTANGLE *updated_area)
{
	int vram_mode, text_mode;

	if (sys_ctrl & SYS_CTRL_80) {               /* テキストの行・桁 */
		if (CRTC_SZ_LINES == 25) {
			text_mode = V_80x25;
		} else                     {
			text_mode = V_80x20;
		}
	} else {
		if (CRTC_SZ_LINES == 25) {
			text_mode = V_40x25;
		} else                     {
			text_mode = V_40x20;
		}
	}

	if (grph_ctrl & GRPH_CTRL_VDISP) {          /* VRAM 表示する */

		if (grph_ctrl & GRPH_CTRL_COLOR) {              /* カラー */
			vram_mode = V_COLOR;
		} else {
			if (grph_ctrl & GRPH_CTRL_200) {            /* 白黒 */
				vram_mode = V_MONO;
			} else {                                    /* 400ライン */
				vram_mode = V_HIRESO;
			}
		}

	} else {                                    /* VRAM 表示しない */

		vram_mode = V_UNDISP;
	}

#if 0
	{ /*現在どのモードで表示中? */
		static int vram_mode0 = -1, text_mode0 = -1;
		if (text_mode0 != text_mode) {
			if (text_mode == V_80x25) {
				printf("      80x25\n");
			} else if (text_mode == V_80x20) {
				printf("      80x20\n");
			} else if (text_mode == V_40x25) {
				printf("      40x25\n");
			} else if (text_mode == V_40x20) {
				printf("      40x20\n");
			}
			text_mode0 = text_mode;
		}
		if (vram_mode0 != vram_mode) {
			if (vram_mode == V_COLOR) {
				printf("COLOR\n");
			} else if (vram_mode == V_MONO) {
				printf("mono \n");
			} else if (vram_mode == V_HIRESO) {
				printf("H=400\n");
			} else if (vram_mode == V_UNDISP) {
				printf("-----\n");
			}
			vram_mode0 = vram_mode;
		}
	}
#endif

	return (vram2screen_list[ vram_mode ][ text_mode ][ method ])(updated_area);
}



/*----------------------------------------------------------------------
 * 画面表示     ボーダー(枠)領域、メイン領域、ステータス領域の全てを表示
 *----------------------------------------------------------------------*/
void put_image_all(void)
{
	T_GRAPH_RECT rect[1];

	rect[0].x = 0;
	rect[0].y = 0;
	rect[0].w = WIDTH;
	rect[0].h = HEIGHT;

	graph_update(1, &rect[0]);
}


/*----------------------------------------------------------------------
 * 画面表示     メイン領域の (x0,y0)-(x1,y1) と 指定されたステータス領域を表示
 *----------------------------------------------------------------------*/
static  void    put_image(T_UPDATE_RECTANGLE area[3])
{
	int i;
	int x0, y0, x1, y1;
	int n = 0;
	T_GRAPH_RECT rect[3];

	for (i = 0; i < 3; i++) {
		if (area[i].exist) {
			if (now_screen_size == SCREEN_SIZE_FULL) {
				x0 = area[i].rectangle.x0;
				y0 = area[i].rectangle.y0;
				x1 = area[i].rectangle.x1;
				y1 = area[i].rectangle.y1;

			} else if (now_screen_size == SCREEN_SIZE_HALF) {
				x0 = area[i].rectangle.x0 / 2;
				y0 = area[i].rectangle.y0 / 2;
				x1 = area[i].rectangle.x1 / 2;
				y1 = area[i].rectangle.y1 / 2;

			} else { /* now_screen_size == SCREEN_SIZE_DOUBLE */
				x0 = area[i].rectangle.x0 * 2;
				y0 = area[i].rectangle.y0 * 2;
				x1 = area[i].rectangle.x1 * 2;
				y1 = area[i].rectangle.y1 * 2;
			}

			rect[n].x = area[i].offset_x + x0;
			rect[n].y = area[i].offset_y + y0;
			rect[n].w = x1 - x0;
			rect[n].h = y1 - y0;
			n ++;
		}
	}

	graph_update(n, &rect[0]);
}








/***********************************************************************
 * イメージ転送 (表示)
 *
 *      この関数は、表示タイミング (約 1/60秒毎) に呼び出される。
 ************************************************************************/
void    screen_update(void)
{
	int skip = FALSE;
	int all_area  = FALSE;      			/* 全エリア転送フラグ */
	T_UPDATE_RECTANGLE user[2], area[3];	/* 転送有無と転送エリア */
	PC88_PALETTE_T syspal[16];
	int is_exec = (quasi88_is_exec()) ? TRUE : FALSE;
	int flip_menu_screen = FALSE;
	int dont_frameskip = FALSE;

	memset(&user, 0, sizeof(user));
	memset(&area, 0, sizeof(area));


	if (is_exec) {
		profiler_lapse(PROF_LAPSE_BLIT);
	}


	/* メイン領域は、描画をスキップする場合があるので、以下で判定 */
	/* (メニューなどは、常時 frame_counter==0 なので、毎回描画)   */

	if ((frame_counter % frameskip_rate) == 0) { /* 描画の時が来た。       */
		/* 以下のいずれかなら描画 */
		if (no_wait ||                             /* ウェイトなし設定時     */
			use_auto_skip == FALSE ||              /* 自動スキップなし設定時 */
			do_skip_draw  == FALSE) {              /* 今回スキップ対象でない */

			skip = FALSE;

		} else {                                 /* 以外はスキップ */

			skip = TRUE;

			/* 描画タイミングなのにスキップした場合は、そのことを覚えておく */
			already_skip_draw = TRUE;
		}

		/* カーソル点滅のワーク更新 */
		if (is_exec) {
			if (--blink_ctrl_counter == 0) {
				blink_ctrl_counter = blink_ctrl_cycle;
				blink_counter ++;
			}
		}
	} else {
		skip = TRUE;
	}


	/* メイン領域を描画する (スキップしない) 場合の処理 */

	if (skip == FALSE) {

		/* 色転送 */

		if (screen_dirty_palette) {             /* 色をシステムに転送 */
			if (is_exec) {
				screen_get_emu_palette(syspal);
				emu_palette(syspal, now_half_interp);
			}
		}

		/* 必要に応じて、フラグをセット */

		if (screen_dirty_frame) {
			set_screen_dirty_all();
			set_screen_dirty_tool_full();
			set_screen_dirty_stat_full();
			all_area = TRUE;
		}

		if (screen_dirty_palette) {
			set_screen_dirty_all();
			screen_dirty_palette = FALSE;
		}

		/* フラグに応じて、描画 */

		if (update_info.draw_start) {
			(update_info.draw_start)();    /* システム依存の描画前処理 */
		}

		if (screen_dirty_frame) {
			(screen_buf_init_p)();              /* 画面全クリア(ボーダー含) */
			screen_dirty_frame = FALSE;
			/* ボーダー部は黒固定。色変更可とするなら、先に色転送が必要… */
		}

		/* VRAM転送 */

		if (quasi88_is_fullmenu()) {
			/* メニューでは、EMU画面の処理はスキップ */
			/* DO NOTHING */
		} else {
			/* EMU および ツールメニューでは EMU画面を描画 */

			/* VRAM更新フラグ screen_dirty_flag の例外処理                   */
			/*          VRAM非表示の場合、更新フラグは意味無いのでクリアする */
			/*          400ラインの場合、更新フラグを画面下半分にも拡張する  */

			if (screen_dirty_all == FALSE) {
				if (!(grph_ctrl & GRPH_CTRL_VDISP)) {
					/* 非表示 */
					memset(screen_dirty_flag, 0, sizeof(screen_dirty_flag) / 2);
				}
				if (!(grph_ctrl & (GRPH_CTRL_COLOR | GRPH_CTRL_200))) {
					/* 400ライン */
					memcpy(&screen_dirty_flag[80 * 200], screen_dirty_flag, 80 * 200);
				}
			}

			crtc_make_text_attr();      /* TVRAM の 属性一覧作成          */
			/* VRAM/TEXT → screen_buf 転送   */
			user[0].exist = vram2screen(screen_dirty_all ? V_ALL : V_DIF, &user[0].rectangle);

			text_attr_flipflop ^= 1;
			memset(screen_dirty_flag, 0, sizeof(screen_dirty_flag));
		}

		/* メニュー、ツールメニューでは MENU 画面を描画 */
		if (quasi88_is_menu_mode()) {

			if (screen_dirty_all ||
				screen_dirty_menu) {

				if (screen_dirty_all) {
					/* 裏画面をクリア。これで差分＝全部になる */
					memset(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_USER_Y][0],
						   0,
						   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_USER_W * Q8GR_TVRAM_USER_H);
				}

				user[1].exist = (menu2screen_p)(&user[1].rectangle);

				/* 表画面と裏画面を一致させておかないとおかしくなる… */
				memcpy(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_USER_Y][0],
					   &q8gr_tvram[q8gr_tvram_flip    ][Q8GR_TVRAM_USER_Y][0],
					   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_USER_W * Q8GR_TVRAM_USER_H);

				flip_menu_screen = TRUE;
				screen_dirty_menu = 0;
			}

		}
		screen_dirty_all = FALSE;

		/* EMU と MENU 両方描画した時は、転送エリアは重複する範囲 */
		if (user[0].exist && user[1].exist) {
			area[0].exist = TRUE;
			area[0].rectangle.x0 = MIN(user[0].rectangle.x0, user[1].rectangle.x0);
			area[0].rectangle.x1 = MAX(user[0].rectangle.x1, user[1].rectangle.x1);
			area[0].rectangle.y0 = MIN(user[0].rectangle.y0, user[1].rectangle.y0);
			area[0].rectangle.y1 = MAX(user[0].rectangle.y1, user[1].rectangle.y1);
		} else if (user[0].exist) {
			area[0] = user[0];
		} else if (user[1].exist) {
			area[0] = user[1];
		}

		if (update_info.draw_finish) {
			(update_info.draw_finish)();    /* システム依存の描画後処理 */
		}
	}


	/* ステータスエリアの処理 (ステータスは、表示する限りスキップしない) */

	if (update_info.draw_start) {
		(update_info.draw_start)();    /* システム依存の描画前処理 */
	}

	if (screen_dirty_tool_full ||
		screen_dirty_tool_part) {

		if (screen_dirty_tool_full) {
			/* 裏画面をクリア。これで差分＝全部になる */
			memset(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_TOOL_Y][0],
				   0,
				   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_TOOL_W * Q8GR_TVRAM_TOOL_H);
		}

		if (show_toolbar) {
			area[1].exist = (tool2screen_p)(&area[1].rectangle);
			if (screen_dirty_tool_full) {
				area[1].exist = TRUE;
				area[1].rectangle.x0 = 0;
				area[1].rectangle.y0 = 0;
				area[1].rectangle.x1 = 640;
				area[1].rectangle.y1 = 64;
			}
		}

		/* 表画面と裏画面を一致させておかないとおかしくなる… */
		memcpy(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_TOOL_Y][0],
			   &q8gr_tvram[q8gr_tvram_flip    ][Q8GR_TVRAM_TOOL_Y][0],
			   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_TOOL_W * Q8GR_TVRAM_TOOL_H);

		flip_menu_screen = TRUE;
		screen_dirty_tool_full = FALSE;
		screen_dirty_tool_part = FALSE;
	}

	if (screen_dirty_stat_full ||
		screen_dirty_stat_part) {

		if (screen_dirty_stat_full) {
			/* 裏画面をクリア。これで差分＝全部になる */
			memset(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_STAT_Y][0],
				   0,
				   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_STAT_W * Q8GR_TVRAM_STAT_H);
		}

		if (show_statusbar) {
			area[2].exist = (stat2screen_p)(&area[2].rectangle);
			if (screen_dirty_stat_full) {
				area[2].exist = TRUE;
				area[2].rectangle.x0 = 0;
				area[2].rectangle.y0 = 0;
				area[2].rectangle.x1 = 640;
				area[2].rectangle.y1 = 16;
			}
		}

		/* 表画面と裏画面を一致させておかないとおかしくなる… */
		memcpy(&q8gr_tvram[q8gr_tvram_flip ^ 1][Q8GR_TVRAM_STAT_Y][0],
			   &q8gr_tvram[q8gr_tvram_flip    ][Q8GR_TVRAM_STAT_Y][0],
			   sizeof(T_Q8GR_TVRAM) * Q8GR_TVRAM_STAT_W * Q8GR_TVRAM_STAT_H);

		flip_menu_screen = TRUE;
		screen_dirty_stat_full = FALSE;
		screen_dirty_stat_part = FALSE;
	}

	if (flip_menu_screen) {
		q8gr_tvram_flip ^= 1;
	}

	if (update_info.draw_finish) {
		(update_info.draw_finish)();    /* システム依存の描画後処理 */
	}

	if (is_exec) {
		profiler_video_output(((frame_counter % frameskip_rate) == 0),
							  skip, (all_area || area[0].exist));
	}


	if (is_exec) {
		++ frame_counter;
	} else {    /* menu,pause,monitor */
		frame_counter = 0;
	}

	if (is_exec) {
		profiler_lapse(PROF_LAPSE_VIDEO);
	}

	if (update_info.dont_frameskip) {
		dont_frameskip = update_info.dont_frameskip();
	}

	if (all_area) {

		put_image_all();
		drawn_count ++;

	} else {
		if (area[0].exist ||
			area[1].exist ||
			area[2].exist ||
			(dont_frameskip)) {

			/* rect は、 (x0 << 24) | (y0 << 15) | (x1 << 8) | (y1) で、
			 * (x0, y0) - (x1, y1) が更新する領域となる。
			 *  0 <= x <= 80, 0 <= y <= 200 となるので、 x * 8, y * 2 する。
			 * 全て 0始点なので、オフセットをつけないといけない
			 */

			if (area[0].exist) {
				area[0].offset_x = screen_user_dx;
				area[0].offset_y = screen_user_dy;
			} else {
				area[0].exist = FALSE;
			}

			if (area[1].exist) {
				area[1].offset_x = screen_tool_dx;
				area[1].offset_y = screen_tool_dy;
			} else {
				area[1].exist = FALSE;
			}

			if (area[2].exist) {
				area[2].offset_x = screen_stat_dx;
				area[2].offset_y = screen_stat_dy;
			} else {
				area[2].exist = FALSE;
			}

			put_image(area);
			drawn_count ++;
		}
	}
}

void    screen_update_immidiate(void)
{
	set_screen_dirty_frame();           /* 全領域 更新 */
	frameskip_counter_reset();          /* 次回描画 */

	screen_update();                    /* 描画処理 */
}

int     quasi88_info_draw_count(void)
{
	return drawn_count;
}













/***********************************************************************
 * テキスト点滅 (カーソルおよび、文字属性の点滅) 処理のワーク設定
 *      CRTC の frameskip_rate, blink_cycle が変更されるたびに呼び出す
 ************************************************************************/
void    frameskip_blink_reset(void)
{
	int wk;

	wk = blink_cycle / frameskip_rate;

	if (wk == 0 ||
		!(blink_cycle - wk * frameskip_rate < (wk + 1) * frameskip_rate - blink_cycle)) {
		wk++;
	}

	blink_ctrl_cycle = wk;
	blink_ctrl_counter = blink_ctrl_cycle;
}



/***********************************************************************
 * フレームカウンタ初期化
 *      次フレームは、必ず表示される。(スキップされない)
 ************************************************************************/
void    frameskip_counter_reset(void)
{
	frame_counter = 0;
	do_skip_draw = FALSE;
	already_skip_draw = FALSE;
}



/***********************************************************************
 * 自動フレームスキップ処理             ( by floi, thanks ! )
 ************************************************************************/
void    frameskip_check(int on_time)
{
	if (use_auto_skip) {

		if (on_time) {                  /* 時間内に処理できた */

			skip_counter = 0;
			do_skip_draw = FALSE;           /* 次回描画とする */

			if (already_skip_draw) {        /* 前回描画時、スキップしてたら */
				frameskip_counter_reset();      /* 次のVSYNCで強制描画 */
			}

		} else {                        /* 時間内に処理できていない */

			do_skip_draw = TRUE;            /* 次回描画スキップ */

			skip_counter++;                 /* 但し、スキップしすぎなら */
			if (skip_counter >= skip_count_max) {
				frameskip_counter_reset();      /* 次のVSYNCで強制描画 */
			}
		}
	}
}



/***********************************************************************
 * フレームスキップレートの取得・設定
 ************************************************************************/
int     quasi88_cfg_now_frameskip_rate(void)
{
	return frameskip_rate;
}
void    quasi88_cfg_set_frameskip_rate(int rate)
{
	char str[32];

	if (rate <= 0) {
		rate = 1;
	}

	if (rate != frameskip_rate) {
		frameskip_rate = rate;

		frameskip_blink_reset();
		frameskip_counter_reset();

		sprintf(str, "FRAME RATE = %2d/sec", 60 / rate);
		statusbar_message(STATUS_INFO, str, 0);
		/* 変更した後は、しばらく画面にフレームレートを表示させる */

		submenu_controll(CTRL_CHG_FRAMERATE);
	}
}
























/* デバッグ用の関数 */
void attr_misc(int line)
{
	int i;

	text_attr_flipflop ^= 1;
	for (i = 0; i < 80; i++) {
		printf("%02X[%02X] ",
			   text_attr_buf[text_attr_flipflop][line * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][line * 80 + i] & 0xff);
	}
	return;
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][9 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][9 * 80 + i] & 0xff);
	}
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][10 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][10 * 80 + i] & 0xff);
	}
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][11 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][11 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][12 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][12 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][13 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][13 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[text_attr_flipflop][14 * 80 + i] >> 8,
			   text_attr_buf[text_attr_flipflop][14 * 80 + i] & 0xff);
	}
#if 0
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[0][15 * 80 + i] >> 8,
			   text_attr_buf[0][15 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[0][16 * 80 + i] >> 8,
			   text_attr_buf[0][16 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[0][17 * 80 + i] >> 8,
			   text_attr_buf[0][17 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[0][18 * 80 + i] >> 8,
			   text_attr_buf[0][18 * 80 + i] & 0xff);
	}
	printf("\n");
	for (i = 0; i < 80; i++) {
		printf("%c[%02X] ",
			   text_attr_buf[0][19 * 80 + i] >> 8,
			   text_attr_buf[0][19 * 80 + i] & 0xff);
	}
	printf("\n");
#endif
}
