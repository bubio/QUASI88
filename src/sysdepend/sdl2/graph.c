/***********************************************************************
 * グラフィック処理 (システム依存)
 *
 *      詳細は、 graph.h 参照
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "quasi88.h"
#include "graph.h"
#include "device.h"

/************************************************************************/

#define DEBUG_PRINTF	(0)
#define DBGPRINTF(x)	if (DEBUG_PRINTF) printf x


/* 以下は static な変数。オプションで変更できるのでグローバルにしてある */

int use_software_rendering = FALSE;		/*SDL_RENDERER_SOFTWARE を使う*/
int use_desktop_fullscreen = TRUE;		/*SDL_WINDOW_FULLSCREEN_DESKTOPを使う*/


/* 以下は、 event.c などで使用する、 OSD なグローバル変数 */

int sdl_mouse_rel_move;					/* マウス相対移動量検知可能か */
int sdl_repeat_on;						/* キーリピートONか */
int sdl_req_mouse_show = TRUE;			/* マウス表示要求ありなら真 */




/************************************************************************/

static T_GRAPH_SPEC graph_spec;			/* 基本情報            */

static int graph_exist;					/* 真で、画面生成済み  */
static T_GRAPH_INFO graph_info;			/* その時の、画面情報  */

Uint32 sdl_window_id;
int    sdl_fullscreen;
int    sdl_width;
int    sdl_height;
int    sdl_buildin_touchkey;
T_GRAPH_RECT sdl_area_main;
T_GRAPH_RECT sdl_area_touch;
static int   sdl_area_overlapped;

int enable_touchkey = TRUE;				/* タッチキーボードの可否 */
int separate_touchkey = TRUE;			/* 別ウインドウタッチキーボード*/

int touchkey_fit = TRUE;		/* 全画面の幅いっぱいに広げる */
int touchkey_overlap = 0;		/* メイン部との重なり度合 0:なし .. 100:全て */
int touchkey_reduce = 0;		/* タッチキーの縮小度合   0:なし .. 90:90%減 */
int touchkey_opaque = 0xff;		/* 重なった時のアルファ値 0:透明 .. 255:なし */

static int calc_buildin_pos_in_fullscreen(
	int desktop_w, int desktop_h,
	int main_w, int main_h,
	int touchkey_w, int touchkey_h,
	int overlap,			/* 重なり度 / 0:全く重ならない 100:全て重なる */
	int reduce,				/* 縮小度   / 0:縮小しない      50:50% */
	int *logical_w, int *logical_h,
	T_GRAPH_RECT *main, T_GRAPH_RECT *touch);


/************************************************************************
 * SDL2の初期化
 * SDL2の終了
 ************************************************************************/

int sdl2_init(void)
{
	SDL_version libver;
	SDL_GetVersion(&libver);

	if (verbose_proc) {
		printf("Initializing SDL2 (%d.%d.%d) ... ",
			   libver.major, libver.minor, libver.patch);
		fflush(NULL);
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {

		if (verbose_proc) {
			printf("Failed\n");
		}
		fprintf(stderr, "SDL Error: %s\n", SDL_GetError());

		return FALSE;

	} else {

		if (verbose_proc) {
			printf("OK\n");
		}

#if DEBUG_PRINTF
		{
			int i, num;
			int j, max;

			/* 今のビデオドライバと、利用可能なビデオドライバの一覧 */
			printf("video driver = %s\n", SDL_GetCurrentVideoDriver());
			num = SDL_GetNumVideoDrivers();
			for (i = 0; i < num; i++) {
				printf("  %2d: %s\n", i, SDL_GetVideoDriver(i));
			}

			/* 今のオーディオドライバと、利用可能なオーディオドライバの一覧 */
			printf("audio driver = %s\n", SDL_GetCurrentAudioDriver());
			num = SDL_GetNumAudioDrivers();
			for (i = 0; i < num; i++) {
				printf("  %2d: %s\n", i, SDL_GetAudioDriver(i));
			}

			/* 使用可能なレンダリングドライバの一覧 */
			num = SDL_GetNumRenderDrivers();
			printf("rendering driver = %d driver(s)\n", num);
			for (i = 0; i < num; i++) {
				SDL_RendererInfo info;
				if (SDL_GetRenderDriverInfo(i, &info) == 0) {
					Uint32 f = info.flags;
					printf("  %2d: %-16s %c|%c|%c|%c\n", i, info.name,
						   (f & SDL_RENDERER_SOFTWARE) ? 'S' : ' ',
						   (f & SDL_RENDERER_ACCELERATED) ? 'H' : ' ',
						   (f & SDL_RENDERER_PRESENTVSYNC) ? 'V' : ' ',
						   (f & SDL_RENDERER_TARGETTEXTURE) ? 'T' : ' ');
				}
			}

			/* ディスプレイの数と、利用可能なディスプレイモードの一覧 */
			max = SDL_GetNumVideoDisplays();
			for (j = 0; j < max; j++) {
				SDL_DisplayMode mode;
				num = SDL_GetNumDisplayModes(j);
				printf("display[%d] = %d mode(s)\n", j, num);
				for (i = 0; i < num; i++) {
					if (SDL_GetDisplayMode(j, i, &mode) == 0) {
						Uint32 f = mode.format;
						printf("  %2d: %4d x %4d (%2dbpp %s)\n",
							   i, mode.w, mode.h,
							   SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName(f));
					}
				}
			}
		}
#endif
		return TRUE;
	}
}

/************************************************************************/

void sdl2_exit(void)
{
	SDL_Quit();
}


/************************************************************************
 * グラフィック処理の初期化
 * グラフィック処理の動作
 * グラフィック処理の終了
 ************************************************************************/

const T_GRAPH_SPEC *graph_init(void)
{
	int ful_w, ful_h;
	SDL_DisplayMode mode;

	if (verbose_proc) {
		printf("Initializing Graphic System ... OK");
	}

	if (use_desktop_fullscreen) {
		/* デスクトップフルスクリーンなら、最大サイズは無制限にする */
		ful_w = 10000;
		ful_h = 10000;

	} else {
		/* 0番目のディスプレイだけ確認。リストの0番目が一番幅の大きいサイズ */
		if (SDL_GetDisplayMode(0, 0, &mode) == 0) {
			ful_w = mode.w;
			ful_h = mode.h;
		} else {
			ful_w = 0;
			ful_h = 0;
		}
	}


	graph_spec.window_max_width      = 10000;
	graph_spec.window_max_height     = 10000;
	graph_spec.fullscreen_max_width  = ful_w;
	graph_spec.fullscreen_max_height = ful_h;
	graph_spec.touchkey_window        = enable_touchkey;
	graph_spec.touchkey_fullscreen    = enable_touchkey;
	graph_spec.touchkey_notify        = sdl2_touchkey;
	graph_spec.forbid_half           = FALSE;

	if (verbose_proc) {
		printf(" (FullscreenMax=%dx%d)\n", ful_w, ful_h);
	}

	return &graph_spec;
}

/************************************************************************/

/* T_GRAPH_INFO (画面サイズ、描画バッファのポインタ、使える色数など) を返す
   引数で指示されたどおりになるとは限らない */

static SDL_Window *sdl_window;
static SDL_Renderer *sdl_renderer;
static SDL_Texture *sdl_texture;
static SDL_Surface *sdl_offscreen;

const T_GRAPH_INFO *graph_setup(int width, int height,
								int fullscreen, double aspect)
{
	int req_width, req_height, req_fullscreen;
	int depth;
	int byte_per_pixel = 4;
	int area_set = FALSE;

	/* まだウインドウが無いならば生成する。この時はまだ全画面にはしない */
	/* (ここで生成したウインドウを、リサイズ時も全画面時も使いまわす) */
	if (graph_exist == FALSE) {
		Uint32 flags;

		sdl_fullscreen = FALSE;
		sdl_width = width;
		sdl_height = height;

		flags = 0;
		/* flags |= SDL_WINDOW_RESIZABLE; */

		if (verbose_proc) {
			printf("Screen init ... ");
		}
		if ((sdl_window = SDL_CreateWindow("QUASI88",
										   SDL_WINDOWPOS_UNDEFINED,
										   SDL_WINDOWPOS_UNDEFINED,
										   width, height, flags)) == NULL) {
			return NULL;
		}
		sdl_window_id = SDL_GetWindowID(sdl_window);

		flags = 0;
		if (use_software_rendering) {
			if (verbose_proc) {
				printf("(Using software rendering)");
			}
			flags = SDL_RENDERER_SOFTWARE;
		} else {
			flags = SDL_RENDERER_ACCELERATED;
		}
		if ((sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
											   flags)) == NULL) {
			return NULL;
		}

		SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);

		sdl2_set_icon(sdl_window);

#if SDL_VERSION_ATLEAST(2, 0, 4)
		/* (Windows) Alt+F4 で SDL_WINDOWEVENT_CLOSE イベントを生成しない */
		SDL_SetHint(SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4, "1");
#endif

		if (enable_touchkey) {
			if (sdl2_touchkey_init(sdl_renderer) == FALSE) {
				if (verbose_proc) {
					printf("Failed (touchkey_init)\n");
				}
				return NULL;
			}
		}

	} else {
		if (verbose_proc) {
			printf("Screen change ... ");
		}
		/* ウインドウとフルスクリーンの切り替えなら、タッチキーボード消す */
		if ((( sdl_fullscreen) && (!fullscreen)) ||
			((!sdl_fullscreen) && ( fullscreen))) {
			sdl2_touchkey_hide(FALSE);
		}
	}

	/* この時点でウインドウはすでにある */
	if (verbose_proc) {
		printf("Req %s:(%dx%d) => ",
			   (fullscreen) ? "full" : "win", width, height);
	}

	/* 要求された画面サイズ。タッチキー表示の場合はその分広げる */
	if (sdl_buildin_touchkey) {
		req_width  = MAX(width, key_width);
		req_height = height + key_height;
	} else {
		req_width  = width;
		req_height = height;
	}
	req_fullscreen = fullscreen;

	/* フルスクリーンが要求されたら、画面サイズを再計算し実行 */
	sdl_area_overlapped = FALSE;
	if (req_fullscreen) {
		Uint32 flags;
		int full_width, full_height;
		SDL_DisplayMode desktop;
		int display_index = SDL_GetWindowDisplayIndex(sdl_window);

		if (use_desktop_fullscreen) {
			/* デスクトップサイズを取得し、相応しい画面サイズを計算する */
			flags = SDL_WINDOW_FULLSCREEN_DESKTOP;

			if (SDL_GetDesktopDisplayMode(display_index, &desktop) == 0) {
				if (sdl_buildin_touchkey && touchkey_fit) {

					sdl_area_overlapped = 
						calc_buildin_pos_in_fullscreen(
							desktop.w, desktop.h,
							width, height,
							key_width, key_height,
							touchkey_overlap,
							touchkey_reduce,
							&full_width, &full_height,
							&sdl_area_main, &sdl_area_touch);
					area_set = TRUE;

				} else {
					double desktop_aspect = ((double) desktop.w / desktop.h);
					double request_aspect = ((double) req_width / req_height);

					if (aspect >= 0.05) {
						desktop_aspect = aspect;
					}

					if (desktop_aspect <= request_aspect) {
						full_width = req_width;
						full_height = (int)(req_width / desktop_aspect);
						full_height = (full_height + 1) & ~1;	/* 2の倍数 */
					} else {
						full_width = (int)(req_height * desktop_aspect);
						full_width = (full_width + 7) & ~7;		/* 8 の倍数 */
						full_height = req_height;
					}
				}

				byte_per_pixel = SDL_BYTESPERPIXEL(desktop.format);

				if (verbose_proc) {
					printf("Try (%dx%d),desktop %dx%d => ",
						   full_width, full_height, desktop.w, desktop.h);
				}
			} else {
				req_fullscreen = FALSE;
			}

		} else {
			/* 最も画面サイズに近いディスプレイモードを取得する */
			SDL_DisplayMode target, closest;

			flags = SDL_WINDOW_FULLSCREEN;

			target.w = req_width;
			target.h = req_height;
			target.format = 0;
			target.refresh_rate = 0;
			target.driverdata = 0;
			if (SDL_GetClosestDisplayMode(display_index, &target, &closest)) {

				full_width = closest.w;
				full_height = closest.h;
				byte_per_pixel = SDL_BYTESPERPIXEL(closest.format);

				if (verbose_proc) {
					printf("Try (%dx%d) => ", full_width, full_height);
				}
			} else {
				req_fullscreen = FALSE;
			}
		}

		/* フルスクリーン可能なら、切り替え・リサイズ */
		if (req_fullscreen) {
			SDL_SetWindowSize(sdl_window, full_width, full_height);

			if (SDL_SetWindowFullscreen(sdl_window, flags) == 0) {

				sdl_fullscreen = TRUE;
				sdl_width = full_width;
				sdl_height = full_height;

			} else {
				req_fullscreen = FALSE;
			}
		}
	}

	/* ウインドウに切り替え・リサイズ (フルスクリーンに失敗した時もこっち) */
	if (req_fullscreen == FALSE) {
		SDL_DisplayMode desktop;
		int display_index = SDL_GetWindowDisplayIndex(sdl_window);
		if (SDL_GetDesktopDisplayMode(display_index, &desktop) == 0) {
			byte_per_pixel = SDL_BYTESPERPIXEL(desktop.format);
		}

		/* 現在、フルスクリーンなら一旦ウインドウに戻す */
		if (sdl_fullscreen) {
			SDL_SetWindowFullscreen(sdl_window, 0);
			SDL_SetWindowPosition(sdl_window,
								  SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED);
			sdl_fullscreen = FALSE;
		}

		SDL_SetWindowSize(sdl_window, req_width, req_height);

		sdl_fullscreen = FALSE;
		sdl_width = req_width;
		sdl_height = req_height;
	}

	/* 各エリア情報をセット。(セット済みならスキップ) */
	if (area_set == FALSE) {
		if (sdl_buildin_touchkey) {
			sdl_area_main.x = (sdl_width - width) / 2;
			sdl_area_main.y = 0;
			sdl_area_main.w = width;
			sdl_area_main.h = height;
			sdl_area_touch.x = (sdl_width - key_width) / 2;
			sdl_area_touch.y = sdl_height - key_height;
			sdl_area_touch.w = key_width;
			sdl_area_touch.h = key_height;
		} else {
			sdl_area_main.x = 0;
			sdl_area_main.y = 0;
			sdl_area_main.w = sdl_width;
			sdl_area_main.h = sdl_height;
			sdl_area_touch.x = 0;
			sdl_area_touch.y = 0;
			sdl_area_touch.w = 0;
			sdl_area_touch.h = 0;
		}
	}


	/* ウインドウの変更が終わったので、色深度を決定する */

#if   defined(SUPPORT_16BPP) && defined(SUPPORT_32BPP)
	if (byte_per_pixel == 4) {
		depth = 32;					/* 24 だと SDL_CreateRGBSurface で NG? */
	} else if (byte_per_pixel == 2) {
		depth = 16;
	} else {
		depth = 32;
		byte_per_pixel = 4;
	}
#elif defined(SUPPORT_16BPP)
	depth = 16;
	byte_per_pixel = 2;
#elif defined(SUPPORT_32BPP)
	depth = 32;
	byte_per_pixel = 4;
#endif

	if (verbose_proc) {
		printf("Res %s:(%dx%d),%dbpp\n", (sdl_fullscreen) ? "full" : "win",
			   sdl_width, sdl_height, depth);
	}


	/* レンダラー、テクスチャー、サーフェスを再設定 */

	if (sdl_fullscreen == 0) {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	} else {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	}
	SDL_RenderSetLogicalSize(sdl_renderer, sdl_width, sdl_height);


	if (sdl_offscreen) {
		SDL_FreeSurface(sdl_offscreen);
	}
	/*                               depth が 24 だとうまくいかない?  ↓ */
	if ((sdl_offscreen =
		 SDL_CreateRGBSurface(0, sdl_area_main.w, sdl_area_main.h, depth,
							  0, 0, 0, 0)) == NULL) {
		return NULL;
	}


	if (sdl_texture) {
		SDL_DestroyTexture(sdl_texture);
	}
	/*      ピクセルフォーマットは surface にあわせる ↓ */
	if ((sdl_texture =
		 SDL_CreateTexture(sdl_renderer,
						   sdl_offscreen->format->format,
						   SDL_TEXTUREACCESS_STREAMING,
						   sdl_area_main.w, sdl_area_main.h)) == NULL) {
		return NULL;
	}

	/* 暗くなれ */
	/* SDL_SetTextureColorMod(sdl_texture, 128, 128, 128); */


	SDL_RenderClear(sdl_renderer);
	/* SDL_RenderPresent(sdl_renderer); */

#if 0
	{
		int w, h;
		SDL_Rect rect;
		SDL_GetRendererOutputSize(sdl_renderer, &w, &h);
		printf("render output size %dx%d\n", w, h);

		SDL_RenderGetLogicalSize(sdl_renderer, &w, &h);
		printf("render logical size %dx%d\n", w, h);

		SDL_RenderGetViewport(sdl_renderer, &rect);
		printf("render viewport %dx%d+%d+%d\n", rect.w, rect.h, rect.x,
			   rect.y);
	}
#endif

	/* graph_info に諸言をセットする */

	graph_info.fullscreen       = sdl_fullscreen;
	graph_info.width            = sdl_offscreen->w;
	graph_info.height           = sdl_offscreen->h;
	graph_info.byte_per_pixel   = byte_per_pixel;
	graph_info.byte_per_line    = sdl_offscreen->pitch;
	graph_info.buffer           = sdl_offscreen->pixels;
	graph_info.nr_color         = 255;
	graph_info.write_only       = TRUE;
	graph_info.broken_mouse     = FALSE;
	graph_info.draw_start       = NULL;
	graph_info.draw_finish      = NULL;
	graph_info.dont_frameskip   =
		(sdl_buildin_touchkey) ? sdl2_touchkey_need_rendering : NULL;

	graph_exist = TRUE;

	return &graph_info;
}

/************************************************************************/

void graph_exit(void)
{
	sdl2_touchkey_exit();

	if (sdl_window) {
		SDL_SetWindowGrab(sdl_window, SDL_FALSE);
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


/************************************************************************
 *
 ************************************************************************/
#define ICON_SIZE	48

void sdl2_set_icon(SDL_Window *window)
{
	const unsigned long *icon_pixel;

#if defined(ICON_SIZE)
#if   (ICON_SIZE == 48)
		icon_pixel = iconimage_get_48x48();
#elif (ICON_SIZE == 32)
		icon_pixel = iconimage_get_32x32();
#elif (ICON_SIZE == 16)
		icon_pixel = iconimage_get_16x16();
#else
#error
#endif
		if (icon_pixel) {
			SDL_Surface *icon_surface;
			Uint32 icon[ ICON_SIZE * ICON_SIZE ];
			int i;

			for (i = 0; i < ICON_SIZE * ICON_SIZE; i++) {
				icon[i] = icon_pixel[i];
			}
			icon_surface =
				SDL_CreateRGBSurfaceFrom(icon, ICON_SIZE, ICON_SIZE,
										 32,
										 ICON_SIZE * sizeof(Uint32),
										 0x00ff0000, 0x0000ff00, 0x000000ff,
										 0xff000000);
			// SDL_SetWindowIcon(window, icon_surface);
			SDL_FreeSurface(icon_surface);
		}
#endif

}

/************************************************************************
 * 色の確保
 * 色の解放
 ************************************************************************/

void graph_add_color(const PC88_PALETTE_T color[],
					 int nr_color, unsigned long pixel[])
{
	int i;
	for (i = 0; i < nr_color; i++) {
		pixel[i] = SDL_MapRGB(sdl_offscreen->format,
							  color[i].red, color[i].green, color[i].blue);
	}
}

/************************************************************************/

void graph_remove_color(int nr_pixel, unsigned long pixel[])
{
	/* 色に関しては何も管理しないので、ここでもなにもしない */
}

/************************************************************************
 * グラフィックの更新
 ************************************************************************/

void graph_update(int nr_rect, T_GRAPH_RECT rect[])
{
	SDL_Rect drect;
	int i;

	if (nr_rect > 16) {
		fprintf(stderr, "SDL: Maybe Update Failied...\n");
		nr_rect = 16;
	}

	/* オフスクリーンの更新されたエリアをテクスチャにコピー */
#if 0
	for (i = 0; i < nr_rect; i++) {
//		printf("%dx%d+%d+%d, ", rect[i].w, rect[i].h, rect[i].x, rect[i].y);

		drect.x = rect[i].x;
		drect.y = rect[i].y;
		drect.w = rect[i].w;
		drect.h = rect[i].h;

		if (SDL_UpdateTexture(sdl_texture, &drect,
							  ((char *) sdl_offscreen->pixels)
							  + (drect.x * graph_info.byte_per_pixel)
							  + (drect.y * sdl_offscreen->pitch),
							  sdl_offscreen->pitch) < 0) {
			fprintf(stderr, "SDL: Unsuccessful SDL_UpdateTexture\n");
		}
	}
#elif 0
	if (SDL_UpdateTexture
		(sdl_texture, NULL, sdl_offscreen->pixels, sdl_offscreen->pitch) < 0) {
		fprintf(stderr, "SDL: Unsuccessful SDL_UpdateTexture\n");
	}
#else
	for (i = 0; i < nr_rect; i++) {
		unsigned char *pixels;
		int pitch;
		int j;

		drect.x = rect[i].x;
		drect.y = rect[i].y;
		drect.w = rect[i].w;
		drect.h = rect[i].h;

		SDL_LockTexture(sdl_texture, &drect, (void **) &pixels, &pitch);

		for (j = 0; j < drect.h; j++) {
			unsigned char *dst = pixels + (j * pitch);
			unsigned char *src = ((unsigned char *) sdl_offscreen->pixels)
								 + (drect.x * graph_info.byte_per_pixel)
								 + ((drect.y + j) * sdl_offscreen->pitch);
			int size = drect.w * graph_info.byte_per_pixel;
			memcpy(dst, src, size);
		}

		SDL_UnlockTexture(sdl_texture);
	}
#endif

	if (sdl_buildin_touchkey) {
		/* タッチキーも表示する場合は、まず画面のテクスチャをコピー */
		SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
		SDL_RenderClear(sdl_renderer);
		SDL_SetTextureBlendMode(sdl_texture, SDL_BLENDMODE_NONE);
		drect.x = sdl_area_main.x;
		drect.y = sdl_area_main.y;
		drect.w = sdl_area_main.w;
		drect.h = sdl_area_main.h;
		SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &drect);

		/* そのあとで、タッチキーのテクスチャをコピー */
		drect.x = sdl_area_touch.x;
		drect.y = sdl_area_touch.y;
		drect.w = sdl_area_touch.w;
		drect.h = sdl_area_touch.h;
		if (touchkey_fit && sdl_fullscreen) {
			sdl2_touchkey_rendering(0, sdl_renderer, &drect, FALSE,
									touchkey_opaque);
		} else {
			sdl2_touchkey_rendering(0, sdl_renderer, &drect, TRUE, 0xff);
		}

	} else {
		/* 画面のテクスチャをレンダラにコピー */
		SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
	}

	SDL_RenderPresent(sdl_renderer);
}


/************************************************************************
 * タイトルの設定
 * 属性の設定
 ************************************************************************/

void graph_set_window_title(const char *title)
{
	SDL_SetWindowTitle(sdl_window, title);
}

/************************************************************************/

void graph_set_attribute(int mouse_show, int grab, int keyrepeat_on,
						 int *result_show, int *result_grab)
{
	if (mouse_show) {
		sdl_req_mouse_show = TRUE;
		SDL_ShowCursor(SDL_ENABLE);
		*result_show = TRUE;
	} else {
		sdl_req_mouse_show = FALSE;
		if (sdl_in_touchkey_area == FALSE) {
			SDL_ShowCursor(SDL_DISABLE);
		}
		*result_show = FALSE;
	}

	if (grab) {
		SDL_SetWindowGrab(sdl_window, SDL_TRUE);
		*result_grab = TRUE;
	} else {
		SDL_SetWindowGrab(sdl_window, SDL_FALSE);
		*result_grab = FALSE;
	}

	if (keyrepeat_on) {
		sdl_repeat_on = TRUE;
	} else {
		sdl_repeat_on = FALSE;
	}


	if ((mouse_show == FALSE) && grab) {
		if (sdl_buildin_touchkey) {
			SDL_Rect rect;
			rect.x = sdl_area_main.x;
			rect.y = sdl_area_main.y;
			rect.w = sdl_area_main.w;
			rect.h = sdl_area_main.h;
			SDL_SetWindowMouseRect(sdl_window, &rect);
		}
		SDL_SetRelativeMouseMode(SDL_TRUE);
		sdl_mouse_rel_move = TRUE;
	} else {
		SDL_SetWindowMouseRect(sdl_window, NULL);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		sdl_mouse_rel_move = FALSE;
	}
}



/* 全画面表示(フルスクリーン)におけるメインエリア、タッチキーエリアの計算
 * 全画面では下部の幅一体にタッチキー、上部にメインを表示する。
 *
 *                   X                  ・メイン画面のサイズ A * B
 *    +------------------------------+      基準値は 640 * 480
 *    |       +-------------+     p  |      計算にはこの値を使用する
 *    |       |      A = 640|        |
 *    |       |             |        |  ・タッチキーのサイズ  H * W
 *    |       |B = 480      |        |          アスペクト比  q = H / W
 *    |       |             |        |      サイズは例えば 920 * 260 だが、
 *  Y |       +-------------+        |    画面下部いっぱいに拡大するので、
 *    |+----------------------------+|      サイズの絶対量ではなくアスペクト比
 *    ||                          q ||      3.538 (= 920 / 260) を使用する
 *    ||H                           ||
 *    ||             W              ||  ・全画面の理論サイズ  X * Y
 *    |+----------------------------+|      アスペクト比  p = X / Y
 *    +------------------------------+      計算にはアスペクト比のみを使用
 *
 * ◆メインとタッチキーを重ならないように隙間なく並べる場合
 *     全画面サイズは、幅 W 、 高さ H + B で、このアスペクト比が p なので、
 *         W : H + B =  X : Y
 *        qH : H + B = pY : Y
 *        pY *(H + B)= qH * Y
 *        pHY + pBY  = qHY
 *        pH  + pB   = qH
 *              pB   = (q - p)H
 *     ∴
 *        H = pB / (q - p)      メイン    :位置 (X/2 - A/2, 0) サイズ A * B
 *        W = H * q             タッチキー:位置 (0, B)         サイズ W * H
 *        Y = H + B
 *        X = W
 * 
 * ◆メインとタッチキーを重なるように並べる場合
 *    +--------------------------+
 *    |       +---------+        |  メイン画面のサイズ A * B のうち、
 *    |    B' |         |        |  重なっていないサイズを B' とする。
 *  Y |+------------------------+|
 *    ||      |         |       ||
 *    ||      +---------+       ||
 *    |+------------------------+|
 *    +--------------------------+
 *     計算式は先ほどと同じで、 B → B' に置き換わる。
 *        H = pB' / (q - p)
 *        Y = H + B'
 *
 *     重ねる場合、全画面サイズがメインエリアよりも小さくなる時がある。
 *     つまり、「計算結果の高さ > メイン画面の高さ」でないといけない。
 *        Y = pB' / (q - p) + B' > B
 *            pB' / (q - p)      > B - B'
 *            pB'                > (B - B') * (q - p)
 *            pB' + (q - p) * B' > B * (q - p)
 *                   qB'         > B * (q - p)
 *                    B'         > B * (q - p) / q    ・・・条件その1
 *     また、「計算結果の幅 > メイン画面の幅」でないといけない。
 *        X = W = pB' / (q - p) * q > A
 *               pqB' / (q - p)     > A
 *               pqB'               > A * (q - p)
 *                 B'            > A * (q - p) / pq   ・・・条件その2
 *     ∴
 *        H = pB' / (q - p)     メイン    :位置 (X/2 - A/2, 0) サイズ A * B
 *        W = H * q             タッチキー:位置 (0, B')        サイズ W * H
 *        Y = H + B'
 *        X = W
 *        ただし、    B' > B * (q - p) / q   かつ
 *                    B' > A * (q - p) / pq
 *
 * ◆全画面サイズが縦にとても長い場合 (アスペクト比 p がとても小さい)
 *     この場合、メインとタッチキーを重ならないように隙間なく並べると、
 *     上下に隙間ができてしまう。この時の p を計算する。
 *     「計算結果の幅 > メイン画面の幅」でないといけないので、
 *        W = H * q = pB / (q - p) * q >= A
 *                                 pqB >= A (q - p)
 *                            pqB + pA >= qA
 *                           p(qB + A) >= qA
 *                           p         >= qA / (qB + A)
 *     全画面サイズのアスペクト比 p が上記を満たさない場合、
 *     全画面サイズの幅 X = メイン画面の幅 A = タッチキーの幅 W となる。
 *     ∴
 *        W = A                 メイン    :位置 (X/2 - A/2, 0) サイズ A * B
 *        H = W / q             タッチキー:位置 (0, Y - H)     サイズ W * H
 *        X = W
 *        Y = X / p
 *
 * ◆全画面サイズが横にとても長い場合 (アスペクト比 p がとても大きい)
 *     この場合、タッチキーの幅を全画面の幅に合わせると、タッチキーの高さが
 *     全画面からはみ出てしまう。つまり、 p が以下の時、
 *        q - p < 0
 *     タッチキーの高さ H = 全画面サイズの高さ X = メイン画面の高さ B となる。
 *     ∴
 *        H = B                 メイン    :位置 (X/2 - A/2, 0) サイズ A * B
 *        W = H * q             タッチキー:位置 (X/2 - W/2, 0) サイズ W * H
 *        Y = B
 *        X = Y * p
 *
 */

static int calc_buildin_pos_in_fullscreen(
	int desktop_w, int desktop_h,
	int main_w, int main_h,
	int touchkey_w, int touchkey_h,
	int overlap,			/* 重なり度 / 0:全く重ならない 100:全て重なる */
	int reduce,				/* 縮小度   / 0:縮小しない      50:50% */
	int *logical_w, int *logical_h,
	T_GRAPH_RECT *main, T_GRAPH_RECT *touch)
{
	int is_overlapped = FALSE;
	int X = desktop_w;
	int Y = desktop_h;
	int A = main_w;
	int B = main_h;
	int W = touchkey_w;
	int H = touchkey_h;

	double p = (double) X / Y;
	double q = (double) W / H;
	double min_p;

	if (overlap < 0) {
		overlap = 0;
	} else if (overlap > 100) {
		overlap = 100;
	}
	if (reduce < 0) {
		reduce = 0;
	} else if (reduce > 90) {
		reduce = 90;
	}

	if (q > p) {
		/* タッチキーを横方向に画面一杯に拡大しても、画面内に入る */

		if (reduce > 0) {
			W = touchkey_w * 100 / (100 - reduce);
			q = (double) W / H;
		}

		min_p = q * A / (q * B + A);
		if (p > min_p) {
			/* 画面に幅があるので、メイン画面とタッチ画面は隙間なく入る */

			if (overlap == 0) {
				/* メイン画面とタッチキーはオーバーラップさせない */

				H = (int) (p * B / (q - p));
				W = (int) (H * q);
				Y = H + B;
				X = W;

				main->x = X / 2 - A / 2;
				main->y = 0;
				touch->x = 0;
				touch->y = B;

			} else {
				/* メイン画面とタッチキーをオーバーラップさせる */

				double Bdash_min1 = B * (q - p) / q;
				double Bdash_min2 = A * (q - p) / (p * q);
				double Bdash_min  = MAX(Bdash_min1, Bdash_min2);
				double Bdash;
				/* overlap が 0 .. 100       に合わせて、
				   Bdash を   B .. Bdash_min にする */
				Bdash = (Bdash_min - B) / 100.0 * overlap + B;

				H = (int) (p * Bdash / (q - p));
				W = (int) (H * q);
				Y = (int) (H + Bdash);
				X = W;
    
				main->x = X / 2 - A / 2;
				main->y = 0;
				touch->x = 0;
				touch->y = (int) Bdash;

				is_overlapped = TRUE;
			}

		} else {
			/* 画面に幅がないので、メイン画面とタッチ画面の間に余白ができる */

			W = A;
			H = (int) (W / q);
			X = W;
			Y = (int) (X / p);

			main->x = X / 2 - A / 2;
			main->y = 0;
			touch->x = 0;
			touch->y = Y - H;

			if (overlap > 0) {
				main->y = (Y - B - H) / 2 * overlap / 100;
				touch->y = (B - Y + H) / 2 * overlap / 100 + (Y - H);

				is_overlapped = TRUE;
			}
		}

		touch->w = W;
		touch->h = H;

		if (reduce > 0) {
			touch->x = (W - W * (100 - reduce) / 100) / 2;
			touch->w = W * (100 - reduce) / 100;
		}

	} else {
		/* タッチキーを横方向に画面一杯に拡大したら、画面内に入らない */

		H = B;
		W = (int) (H * q);
		Y = B;
		X = (int) (Y * p);
    
		if (reduce > 0) {
			H = H * (100 - reduce) / 100;
			W = W * (100 - reduce) / 100;
		}

		main->x = X / 2 - A / 2;
		main->y = 0;
		touch->x = X / 2 - W / 2;
		touch->y = Y - H;

		touch->w = W;
		touch->h = H;
	}

	*logical_w = X;
	*logical_h = Y;

	main->w = main_w;
	main->h = main_h;


	return is_overlapped;
}
