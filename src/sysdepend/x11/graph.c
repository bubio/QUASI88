/***********************************************************************
 * グラフィック処理 (システム依存)
 *
 *      詳細は、 graph.h 参照
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#include "quasi88.h"
#include "graph.h"
#include "device.h"

#include "screen.h"


#ifdef MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

#ifdef  USE_VIDMODE
#include <sys/types.h>
#include <X11/extensions/xf86vmode.h>
#endif

/************************************************************************/

/* 以下は static な変数。オプションで変更できるのでグローバルにしてある */

int colormap_type = 0;				/* カラーマップ指定 0..2 */
int use_xsync = TRUE;				/* XSync を使用するかどうか */
#ifdef MITSHM
int use_SHM = TRUE;					/* MIT-SHM を使用するかどうか */
#endif


/* 以下は、 event.c などで使用する、 OSD なグローバル変数 */

Display *x11_display;
Window  x11_window;
Atom    x11_atom_kill_type;
Atom    x11_atom_kill_data;

int     x11_width;					/* 現在のウインドウの幅 */
int     x11_height;					/* 現在のウインドウの高 */

int     x11_mouse_rel_move;			/* マウス相対移動量検知させるか */

/************************************************************************/

static T_GRAPH_SPEC graph_spec;					/* 基本情報 */

static int          graph_exist;				/* 真で、画面生成済み */
static T_GRAPH_INFO graph_info;					/* その時の、画面情報 */


static int    x11_enable_fullscreen;			/* 真で、全画面可能 */

static Screen *x11_screen;
static GC     x11_gc;
static Visual *x11_visual;

static int    x11_depth;
static int    x11_byte_per_pixel;


/* 現在の属性 */
static int    x11_mouse_show   = TRUE;
static int    x11_grab         = FALSE;
static int    x11_keyrepeat_on = TRUE;
static int    x11_reset_grab   = FALSE;

/************************************************************************/

/* ウインドウの生成時・リサイズ時は、ウインドウサイズ変更不可を指示する */
static void set_wm_hints(int w, int h, int fullscreen);

#ifdef MITSHM
static XShmSegmentInfo SHMInfo;

/* MIT-SHM の失敗をトラップ */
static int private_handler(Display *display, XErrorEvent *E);
#endif

#ifdef USE_VIDMODE
enum {
	VIDMODE_ERR_NONE = 0,
	VIDMODE_ERR_AVAILABLE,
	VIDMODE_ERR_LOCAL_DISPLAY,
	VIDMODE_ERR_QUERY_VERSION,
	VIDMODE_ERR_QUERY_EXTENSION,
	VIDMODE_ERR_GET_ALL_MODE_LINES
};
static int vidmode_err = VIDMODE_ERR_AVAILABLE;

static XF86VidModeModeInfo **vid_mode = NULL;
static int vid_mode_count;
#endif


/************************************************************************
 * X11の初期化
 * X11の終了
 ************************************************************************/

void x11_init(void)
{
	x11_enable_fullscreen = FALSE;

	x11_display = XOpenDisplay(NULL);
	if (! x11_display) {
		return;
	}

#ifdef  USE_VIDMODE
	{
		int i, j;
		char *s;

		if (!(s = getenv("DISPLAY")) || (s[0] != ':')) {
			vidmode_err = VIDMODE_ERR_LOCAL_DISPLAY;

		} else if (!XF86VidModeQueryVersion(x11_display, &i, &j)) {
			vidmode_err = VIDMODE_ERR_QUERY_VERSION;

		} else if (!XF86VidModeQueryExtension(x11_display, &i, &j)) {
			vidmode_err = VIDMODE_ERR_QUERY_EXTENSION;

		} else if (!XF86VidModeGetAllModeLines(x11_display,
											   DefaultScreen(x11_display),
											   &vid_mode_count, &vid_mode)) {
			vidmode_err = VIDMODE_ERR_GET_ALL_MODE_LINES;

		} else {
			vidmode_err = VIDMODE_ERR_NONE;
			x11_enable_fullscreen = TRUE;
		}
	}
#endif
}

static  void    init_verbose(void)
{
	if (verbose_proc) {

		if (! x11_display) {
			printf("FAILED\n");
			return;
		} else               {
			printf("OK");
		}

#ifdef USE_VIDMODE
		printf("\n");
		printf("  VidMode : ");

		if (vidmode_err == VIDMODE_ERR_NONE) {
			printf("OK");

		} else if (vidmode_err == VIDMODE_ERR_LOCAL_DISPLAY) {
			printf("FAILED (Only works on a local display)");

		} else if (vidmode_err == VIDMODE_ERR_QUERY_VERSION) {
			printf("FAILED (XF86VidModeQueryVersion)");

		} else if (vidmode_err == VIDMODE_ERR_QUERY_EXTENSION) {
			printf("FAILED (XF86VidModeQueryExtension)");

		} else if (vidmode_err == VIDMODE_ERR_GET_ALL_MODE_LINES) {
			printf("FAILED (XF86VidModeGetAllModeLines)");

		} else {
			printf("FAILED (Not Support)");
		}

		if (vidmode_err == VIDMODE_ERR_NONE) {
			printf(", fullscreen available\n");
		} else {
			printf(", fullscreen not available\n");
		}
#else
		printf(" (fullscreen not supported)\n");
#endif
	}
}

/************************************************************************/

void x11_exit(void)
{
	if (x11_display) {
		XAutoRepeatOn(x11_display);		/* オートリピート設定をもとに戻す */

#ifdef  USE_VIDMODE
		if (vid_mode) {
			XFree(vid_mode);
		}
#endif

		XSync(x11_display, True);
		XCloseDisplay(x11_display);

		x11_display = NULL;
	}
}


/************************************************************************
 * グラフィック処理の初期化
 * グラフィック処理の動作
 * グラフィック処理の終了
 ************************************************************************/

/* マウス非表示を実現するため、透明マウスカーソルを用意しよう。
   グラフィック初期化時にカーソルを生成、終了時に破棄する。*/
static void create_invisible_mouse(void);
static void destroy_invisible_mouse(void);


const T_GRAPH_SPEC *graph_init(void)
{
	int win_w, win_h;
	int ful_w, ful_h;

	int i;

	if (verbose_proc) {
		printf("Initializing Graphic System (X11) ... ");
		init_verbose();
	}

	if (! x11_display) {
		return NULL;
	}


	x11_screen = DefaultScreenOfDisplay(x11_display);
	x11_gc     = DefaultGCOfScreen(x11_screen);
	x11_visual = DefaultVisualOfScreen(x11_screen);


	/* 色深度と、ピクセルあたりのバイト数をチェック */
	{
		int count;
		XPixmapFormatValues *pixmap;

		pixmap = XListPixmapFormats(x11_display, &count);
		if (pixmap == NULL) {
			if (verbose_proc) {
				printf("  X11 error (Out of memory ?)\n");
			}
			return NULL;
		}
		for (i = 0; i < count; i++) {
			if (pixmap[i].depth == DefaultDepthOfScreen(x11_screen)) {
				x11_depth = pixmap[i].depth;
				if (x11_depth <=  8 && pixmap[i].bits_per_pixel ==  8) {
					x11_byte_per_pixel = 1;
				} else if (x11_depth <= 16 && pixmap[i].bits_per_pixel == 16) {
					x11_byte_per_pixel = 2;
				} else if (x11_depth <= 32 && pixmap[i].bits_per_pixel == 32) {
					x11_byte_per_pixel = 4;
				} else {			/* 上記以外のフォーマットは面倒なので NG */
					x11_byte_per_pixel = 0;
				}
				break;
			}
		}
		XFree(pixmap);
	}

	{							/* 非対応の depth なら弾く */
		const char *s = NULL;
		switch (x11_byte_per_pixel) {
		case 0:
			s = "this bpp is not supported";
			break;
#ifndef SUPPORT_8BPP
		case 1:
			s = "8bpp is not supported";
			break;
#endif
#ifndef SUPPORT_16BPP
		case 2:
			s = "16bpp is not supported";
			break;
#endif
#ifndef SUPPORT_32BPP
		case 4:
			s = "32bpp is not supported";
			break;
#endif
		}
		if (s) {
			if (verbose_proc) {
				printf("  %s\n", s);
			}
			return NULL;
		}
	}

	if (x11_depth < 8) {
		if (verbose_proc) {
			printf("  < 8bpp is not supported\n");
		}
		return NULL;
	}


	/* 利用可能なウインドウのサイズを調べておく */

	win_w = 10000;		/* ウインドウは制約なし。適当に大きな値をセット */
	win_h = 10000;

#ifdef USE_VIDMODE		/* 全画面モードは、一覧から幅の最も大きなのをセット */
	if (x11_enable_fullscreen) {
		ful_w = 0;
		ful_h = 0;
		for (i = 0; i < vid_mode_count; i++) {
			if (ful_w < vid_mode[i]->hdisplay) {
				ful_w = vid_mode[i]->hdisplay;
				ful_h = vid_mode[i]->vdisplay;
			}
			if (verbose_proc)
				printf("  VidMode %3d: %dx%d %d\n",
					   i, vid_mode[i]->hdisplay, vid_mode[i]->vdisplay,
					   vid_mode[i]->privsize);
		}
	} else
#endif
	{
		ful_w = 0;
		ful_h = 0;
	}

	graph_spec.window_max_width      = win_w;
	graph_spec.window_max_height     = win_h;
	graph_spec.fullscreen_max_width  = ful_w;
	graph_spec.fullscreen_max_height = ful_h;
	graph_spec.touchkey_window        = FALSE;
	graph_spec.touchkey_fullscreen    = FALSE;
	graph_spec.touchkey_notify        = NULL;
	graph_spec.forbid_half           = FALSE;

	if (verbose_proc)
		printf("  INFO: %dbpp(%dbyte), Maxsize=win(%d,%d),full(%d,%d)\n",
			   x11_depth, x11_byte_per_pixel,
			   win_w, win_h, ful_w, ful_h);


	/* マウス非表示のための、透明マウスカーソルを生成 */
	create_invisible_mouse();

	/* Drag & Drop 初期化 */
	xdnd_initialize();

	return &graph_spec;
}

/************************************************************************/

/* ウインドウの生成時／リサイズ時／破棄時 の、 実際の処理をする関数 */

static int create_window(int *width, int *height,
						 void **ret_buffer, int *ret_nr_color,
						 int *fullscreen, double aspect);
static int resize_window(int *width, int *height, void *old_buffer,
						 void **ret_buffer, int *ret_nr_color);
static void destroy_window(void *old_buffer, int fullscreen);



const T_GRAPH_INFO *graph_setup(int width, int height,
								int fullscreen, double aspect)
{
	int nr_color = 0;
	void *buf = NULL;
	int success;

	/* 全画面不可なら、全画面要求は無視 */
	if ((x11_enable_fullscreen == FALSE) && (fullscreen)) {
		fullscreen = FALSE;
	}


	/* ウインドウ → ウインドウの場合、ウインドウを使いまわす。
	   それ以外は、一旦ウインドウを破棄する */
	if (graph_exist) {
		if (verbose_proc) {
			printf("Re-Initializing Graphic System (X11)\n");
		}

		if ((graph_info.fullscreen) || (fullscreen)) {
			destroy_window(graph_info.buffer, graph_info.fullscreen);
			graph_exist = FALSE;
		}
	}


	if (graph_exist == FALSE) {
		success = create_window(&width, &height, &buf, &nr_color,
								&fullscreen, aspect);
	} else {
		success = resize_window(&width, &height,
								graph_info.buffer, &buf, &nr_color);
	}

	if (success == FALSE) {
		graph_exist = FALSE;
		return NULL;
	}


	graph_exist = TRUE;

	if (fullscreen) {
		graph_info.fullscreen = TRUE;
	} else {
		graph_info.fullscreen = FALSE;
	}
	graph_info.width          = width;
	graph_info.height         = height;
	graph_info.byte_per_pixel = x11_byte_per_pixel;
	graph_info.byte_per_line  = width * x11_byte_per_pixel;
	graph_info.buffer         = buf;
	graph_info.nr_color       = nr_color;
	graph_info.write_only     = FALSE;
	graph_info.broken_mouse   = FALSE;
	graph_info.draw_start     = NULL;
	graph_info.draw_finish    = NULL;
	graph_info.dont_frameskip = NULL;

	if (fullscreen == FALSE) {
		/* ウインドウのタイトルバーを復元 */
		graph_set_window_title(NULL);
	}

	x11_width  = width;
	x11_height = height;

#if 0   /* debug */
	printf("@ fullscreen      %d\n",    graph_info.fullscreen);
	printf("@ width           %d\n",    graph_info.width);
	printf("@ height          %d\n",    graph_info.height);
	printf("@ byte_per_pixel  %d\n",    graph_info.byte_per_pixel);
	printf("@ byte_per_line   %d\n",    graph_info.byte_per_line);
	printf("@ buffer          %p\n",    graph_info.buffer);
	printf("@ nr_color        %d\n",    graph_info.nr_color);
	printf("@ write_only      %d\n",    graph_info.write_only);
	printf("@ broken_mouse    %d\n",    graph_info.broken_mouse);
	printf("@ dont_frameskip  %p\n",    graph_info.dont_frameskip);
#endif

	return &graph_info;
}

/************************************************************************/

void graph_exit(void)
{
	if (graph_exist) {

		destroy_window(graph_info.buffer, graph_info.fullscreen);

		graph_exist = FALSE;
	}

	/* 透明マウスカーソルを破棄 */
	destroy_invisible_mouse();
}

/*======================================================================*/

static Cursor x11_cursor_id;
static Pixmap x11_cursor_pix;

static void create_invisible_mouse(void)
{
	char data[1] = { 0x00 };
	XColor color;

	x11_cursor_pix = XCreateBitmapFromData(x11_display,
										   DefaultRootWindow(x11_display),
										   data, 8, 1);
	color.pixel    = BlackPixelOfScreen(x11_screen);
	x11_cursor_id  = XCreatePixmapCursor(x11_display,
										 x11_cursor_pix, x11_cursor_pix,
										 &color, &color, 0, 0);
}

static void destroy_invisible_mouse(void)
{
	if (x11_mouse_show == FALSE) {
		XUndefineCursor(x11_display, DefaultRootWindow(x11_display));
	}
	XFreePixmap(x11_display, x11_cursor_pix);
}

/*======================================================================*/

#ifdef MITSHM
/* MIT-SHM の失敗をトラップ */
static int private_handler(Display *display, XErrorEvent *E)
{
	char str[256];

	if (E->error_code == BadAccess ||
		E->error_code == BadAlloc) {
		use_SHM = FALSE;
		return 0;
	}

	XGetErrorText(display, E->error_code, str, 256);
	fprintf(stderr, "X Error (%s)\n", str);
	fprintf(stderr, " Error Code   %d\n", E->error_code);
	fprintf(stderr, " Request Code %d\n", E->request_code);
	fprintf(stderr, " Minor code   %d\n", E->minor_code);

	exit(-1);

	return 1;
}
#endif

/*======================================================================*/

/* ウインドウマネージャにサイズ変更不可を指示する */
static void set_wm_hints(int w, int h, int fullscreen)
{
	static int icon_exist = FALSE;

	XSetWindowAttributes attributes;
	XSizeHints Hints;
	XWMHints WMHints;

	if (fullscreen) {
#if 0
		/* ウインドウマネージャの設定リクエストをリダイレクトする */
		attributes.override_redirect = True;
		XChangeWindowAttributes(x11_display, x11_window,
								CWOverrideRedirect, &attributes);

		/* ウインドウマネージャの介入が一切なくなる様子。
		   自力でキーボードやマウスをグラブしないと、入力もできない */
#else

		/* ウインドウの装飾をなくす */
#define MWM_HINTS_DECORATIONS   (1L << 1)
		typedef struct {
			unsigned long flags;
			unsigned long functions;
			unsigned long decorations;
			long inputMode;
			unsigned long status;
		} MotifWmHints;

		Atom mwmatom;
		MotifWmHints mwmhints;
		mwmhints.flags = MWM_HINTS_DECORATIONS;
		mwmhints.decorations = 0;
		mwmatom = XInternAtom(x11_display, "_MOTIF_WM_HINTS", 0);

		XChangeProperty(x11_display, x11_window,
						mwmatom, mwmatom, 32,
						PropModeReplace, (unsigned char *)&mwmhints,
						sizeof(MotifWmHints) / 4);
		/* 表示位置と、画面サイズの変更不可を指定する */
		Hints.x      = 0;
		Hints.y      = 0;
		Hints.flags  = PMinSize | PMaxSize | USPosition;
		Hints.min_width  = Hints.max_width  = w;
		Hints.min_height = Hints.max_height = h;
		XSetWMNormalHints(x11_display, x11_window, &Hints);

#endif

		icon_exist = FALSE;
	} else {
		/* リサイズ時 (ステータス ON/OFF切替時) に、一瞬画面が消えるのを防ぐ */
		attributes.bit_gravity = NorthWestGravity;
		XChangeWindowAttributes(x11_display, x11_window,
								CWBitGravity, &attributes);

		/* 画面サイズの変更不可を指定する */
		Hints.flags      = PMinSize | PMaxSize;
		Hints.min_width  = Hints.max_width  = w;
		Hints.min_height = Hints.max_height = h;
		XSetWMNormalHints(x11_display, x11_window, &Hints);

		/* アイコン */
		if (icon_exist == FALSE) {
			int i;
			unsigned long icon[2 + (16 * 16) + 2 + (32 * 32) + 2 + (48 * 48)];
			Atom net_wm_icon = XInternAtom(x11_display, "_NET_WM_ICON", False);
			Atom cardinal = XInternAtom(x11_display, "CARDINAL", False);

			i = 0;
			icon[i++] = 16;
			icon[i++] = 16;
			memcpy(&icon[i], iconimage_get_16x16(), 16 * 16 * sizeof(long));
			i += 16 * 16;
			icon[i++] = 32;
			icon[i++] = 32;
			memcpy(&icon[i], iconimage_get_32x32(), 32 * 32 * sizeof(long));
			i += 32 * 32;
#if 1
			icon[i++] = 48;
			icon[i++] = 48;
			memcpy(&icon[i], iconimage_get_48x48(), 48 * 48 * sizeof(long));
			i += 48 * 48;
#endif

			XChangeProperty(x11_display, x11_window, net_wm_icon, cardinal, 32,
							PropModeReplace, (const unsigned char *) icon, i);

			icon_exist = TRUE;
		}
	}

	/* 入力フォーカスモデルの指定。意味不明 … */
	WMHints.input = True;
	WMHints.flags = InputHint;
	XSetWMHints(x11_display, x11_window, &WMHints);

	/* 強制終了、中断に備えて、アトムを設定 */
	x11_atom_kill_type = XInternAtom(x11_display, "WM_PROTOCOLS", False);
	x11_atom_kill_data = XInternAtom(x11_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x11_display, x11_window, &x11_atom_kill_data, 1);
}


/*======================================================================*/

/* ウインドウの生成時／リサイズ時／破棄時 に、
   それぞれ カラーマップを確保／再初期化／解放 という処理が必要 */
static int  create_colormap(void);
static int  reuse_colormap(void);
static void destroy_colormap(void);

/* ウインドウの生成時／リサイズ時／破棄時 に、イメージの確保・解放が必要 */
static void *create_image(int width, int height);
static void destroy_image(void *buf);

#ifdef  USE_VIDMODE
/* 全画面時に、もっとも最適なモードラインを探す */
static int search_mode(int w, int h, double aspect);
#endif


static int create_window(int *width, int *height,
						 void **ret_buffer, int *ret_nr_color,
						 int *fullscreen, double aspect)
{
	int req_width = (*width);
	int req_height = (*height);

	if (verbose_proc) {
		printf("  Opening window ... ");
	}

#ifdef  USE_VIDMODE
	if ((*fullscreen)) {
		/* 要求に近いモードラインを探す */
		int fit = search_mode((*width), (*height), aspect);

		if ((0 <= fit) && (fit < vid_mode_count)) {
			/* 見つかったらそのモードラインに切り替える */

			/* 実際の画面サイズをセットする */
			(*width) = vid_mode[fit]->hdisplay;
			(*height) = vid_mode[fit]->vdisplay;

			/* モードラインを切り替え */
			if (XF86VidModeSwitchToMode(x11_display,
										DefaultScreen(x11_display),
										vid_mode[fit])) {

				XF86VidModeSetViewPort(x11_display, DefaultScreen(x11_display),
									   0, 0);
			} else {
				if (verbose_proc) {
					printf("Change fullscreen failed, try window ... ");
				}
				(*fullscreen) = FALSE;
			}
		} else {
			// みつからなかった
			if (verbose_proc) {
				printf("Not found fullscreen, try window ... ");
			}
			(*fullscreen) = FALSE;
		}
	}
#else
	(*fullscreen) = FALSE;
#endif

	x11_window = XCreateSimpleWindow(x11_display,
									 RootWindowOfScreen(x11_screen),
									 0, 0,
									 (*width), (*height),
									 0,
									 WhitePixelOfScreen(x11_screen),
									 BlackPixelOfScreen(x11_screen));
	if (verbose_proc)
		printf("%s (%dx%d => %dx%d)\n", (x11_window ? "OK" : "FAILED"),
			   req_width, req_height, (*width), (*height));

	if (x11_window) {
		/* ウインドウマネージャーへ特性(サイズ変更不可)を指示する */
		set_wm_hints((*width), (*height), (*fullscreen));

		/* イベントの設定 */
		XSelectInput(x11_display, x11_window,
					 FocusChangeMask | ExposureMask |
					 KeyPressMask | KeyReleaseMask |
					 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
		/* カラーを確保する */
		(*ret_nr_color) = create_colormap();

		/* スクリーンバッファ と image を確保 */
		(*ret_buffer) = create_image((*width), (*height));

	}

	if ((!x11_window) || ((*ret_nr_color) < 16) || ((*ret_buffer) == NULL)) {
		if (x11_window) {
			/* カラーマップ破棄 */
			destroy_colormap();
			/* イメージ破棄 */
			if (ret_buffer) {
				destroy_image(ret_buffer);
			}
			/* ウインドウ破棄 */
			XDestroyWindow(x11_display, x11_window);
		}
#ifdef  USE_VIDMODE
		/* モードラインを元に戻す */
		if ((*fullscreen)) {
			XF86VidModeSwitchToMode(x11_display, DefaultScreen(x11_display),
									vid_mode[0]);
		}
#endif
		return FALSE;
	}

	/* 最前面に表示 */
	XMapRaised(x11_display, x11_window);

	if ((*fullscreen)) {
		/* キーボード・マウスをグラブする */
		x11_get_focus = TRUE;
		x11_set_attribute_focus_in();
	} else {
		/* Drag & Drop 受け付け開始 */
		xdnd_start();
	}

	return TRUE;
}


static int resize_window(int *width, int *height, void *old_buffer,
						 void **ret_buffer, int *ret_nr_color)
{
	Window child;
	int x, y;

	if (verbose_proc) {
		printf("  Resizing window ... ");
	}

	/* ウインドウマネージャーへ新たなサイズを指示する */
	set_wm_hints((*width), (*height), FALSE);

	/* リサイズしてウインドウが画面外に出てしまったらイヤなので、その場合は
	   ウインドウを画面内に移動させようと思う。が環境によっては XGetGeometry()
	   を使ってもちゃんと座標が取得できないし、 XMoveWindow() を使っても、
	   ウインドウの枠とかを考慮せずに移動する場合がある。ウインドウマネージャー
	   が関わっているからだと思うのだが、どうするのが正しいんでしょう ? */
#if 1
	/* とりあえずルートウインドウからの相対位置を求めて、原点が上か左の画面外
	   だったら移動させる。仮想ウインドウマネージャーでも大丈夫だろう */

	XTranslateCoordinates(x11_display, x11_window,
						  DefaultRootWindow(x11_display), 0, 0,
						  &x, &y, &child);
	if (x < 0 || y < 0) {
		if (x < 0) {
			x = 0;
		}
		if (y < 0) {
			y = 0;
		}
		XMoveResizeWindow(x11_display, x11_window, x, y, (*width), (*height));
	} else
#endif
	{
		XResizeWindow(x11_display, x11_window, (*width), (*height));
	}

	if (verbose_proc)
		printf("%s (%dx%d)\n", (x11_window ? "OK" : "FAILED"),
			   (*width), (*height));

	/* image を破棄する */
	if (old_buffer) {
		destroy_image(old_buffer);
	}

	/* カラーマップ状態の再設定 */
	(*ret_nr_color) = reuse_colormap();

	/* スクリーンバッファ と image を確保 */
	(*ret_buffer) = create_image((*width), (*height));

	return (((*ret_nr_color) >= 16) && (*ret_buffer)) ? TRUE : FALSE;
}


static void destroy_window(void *old_buffer, int fullscreen)
{
	if (verbose_proc) {
		printf("  Closing Window\n");
	}

	/* カラーマップ破棄 */
	destroy_colormap();

	/* イメージ破棄 */
	if (old_buffer) {
		destroy_image(old_buffer);
	}

	/* ウインドウ破棄 */
	XDestroyWindow(x11_display, x11_window);

	/* グラブ状態破棄 */
	if (x11_grab) {
		XUngrabKeyboard(x11_display, CurrentTime);
		XUngrabPointer(x11_display, CurrentTime);
		x11_grab = FALSE;
	}

#ifdef  USE_VIDMODE
	/* モードラインを元に戻す */
	if (fullscreen) {
		XF86VidModeSwitchToMode(x11_display, DefaultScreen(x11_display),
								vid_mode[0]);
	}
#endif

	XSync(x11_display, True);           /* 全イベント破棄 */
}



#ifdef USE_VIDMODE

#define FABS(a)			(((a) >= 0.0) ? (a) : -(a))

static int search_mode(int w, int h, double aspect)
{
	int i;
	int fit = -1;
	int fit_w = 0, fit_h = 0;
	double fit_a = 0.0;

	for (i = 0; i < vid_mode_count; i++) {
		/* 画面サイズに収まっていること */
		if (w <= vid_mode[i]->hdisplay &&
			h <= vid_mode[i]->vdisplay) {

			int tmp_w = vid_mode[i]->hdisplay;
			int tmp_h = vid_mode[i]->vdisplay;
			double tmp_a = FABS(((double)tmp_w / tmp_h) - aspect);

			/* 最初に見つかったものをまずはチョイス */
			if (fit == -1) {
				fit = i;
				fit_w = tmp_w;
				fit_h = tmp_h;
				fit_a = tmp_a;

			} else {
				/* 次からは、前回のと比べて、よりフィットすればチョイス */

				/* 横長モニター、ないし、アスペクト未指定の場合 */
				if (aspect >= 1.0 || aspect < 0.01) {

					/* 縦の差の少ないほう、またはアスペクト比の近いほう */
					if (((tmp_h - h) < (fit_h - h)) ||
						((tmp_h == fit_h) && (tmp_a < fit_a))) {
						fit = i;
						fit_w = tmp_w;
						fit_h = tmp_h;
						fit_a = tmp_a;
					}

				} else {		/* 縦長モニターの場合 (使ったことないけど) */

					/* 横の差の少ないほう、またはアスペクト比の近いほう */
					if (((tmp_w - w) < (fit_w - w)) ||
						((tmp_w == fit_w) && (tmp_a < fit_a))) {
						fit = i;
						fit_w = tmp_w;
						fit_h = tmp_h;
						fit_a = tmp_a;
					}
				}
			}
		}
	}
	/* 該当するのが全くない場合は、 -1 が返る */
	return fit;
}

#endif


/*======================================================================*/

static Colormap x11_colormap;					/* 使用中のカラーマップID */
static int      x11_cmap_type;					/* 現在のカラーマップ処理 */

/* 使用中の色の数を自力で管理するので、そのための変数を用意 */
static unsigned long color_cell[256];			/* ピクセル値の配列     */
static int           nr_color_cell_used;		/* 配列の使用済み位置   */
static int           sz_color_cell;				/* 配列の最大数         */


static int create_colormap(void)
{
	int i, j, max;
	unsigned long plane[1];		/* dummy */
	unsigned long pixel[256];

	if (verbose_proc) {
		printf("  Colormap: ");
	}

	x11_colormap = DefaultColormapOfScreen(x11_screen);

	sz_color_cell = 0;

	switch (colormap_type) {
	case 0:								/* 共有カラーセルを使用 */
		if (verbose_proc) {
			printf("shared ... ");
		}
		for (i = 0; i < 3; i++) {
			if (i == 0) {
				max = 256;				/* 最初は、256色確保     */
			} else if (i == 1) {
				max = 214;				/* だめなら214色、       */
			} else {
				max = 32;				/* さらには 32色、と試す */
			}

			if (XAllocColorCells(x11_display,
								 DefaultColormapOfScreen(x11_screen),
								 False, plane, 0, pixel, max)) {
				/* 色確保成功 */
				nr_color_cell_used = 0;
				sz_color_cell      = max;
				for (j = 0; j < sz_color_cell; j++) {
					color_cell[j] = pixel[j];
				}
				break;
			}
		}
		if (sz_color_cell > 0) {
			if (verbose_proc) {
				printf("OK (%d colors)\n", sz_color_cell);
			}
			x11_cmap_type = 0;
			return sz_color_cell;
		}
		if (verbose_proc) {
			printf("FAILED, ");
		}
		/* FALLTHROUGH */

	case 1:								/* プライベートカラーマップを確保 */
		if (x11_visual->class == PseudoColor) {
			if (verbose_proc) {
				printf("private ... ");
			}

			x11_colormap = XCreateColormap(x11_display, x11_window,
										   x11_visual, AllocAll);

			XSetWindowColormap(x11_display, x11_window, x11_colormap);

			/* 本当は bpp に依存した値のはずだが・・・ */
			nr_color_cell_used = 0;
			sz_color_cell      = 256;
			for (j = 0; j < sz_color_cell; j++) {
				color_cell[j] = j;
			}

			if (verbose_proc) {
				printf("OK\n");
			}
			x11_cmap_type = 1;
			return sz_color_cell;
		}
		/* FALLTHROUGH */

	case 2:								/* 色は必要時に動的に確保 */
		if (verbose_proc) {
			printf("no color allocated\n");
		}

		nr_color_cell_used = 0;
		sz_color_cell      = 256;

		x11_cmap_type = 2;
		return sz_color_cell;
	}

	return 0;
}


static int reuse_colormap(void)
{
	switch (x11_cmap_type) {
	case 0:
		/* 引き続き、同じ共有カラーセルを使用 */
		break;

	case 1:
		/* 引き続き、同じカラーマップを使用 */
		break;

	case 2:
		XFreeColors(x11_display, x11_colormap,
					color_cell, nr_color_cell_used, 0);
		break;
	}

	nr_color_cell_used = 0;

	return sz_color_cell;
}


static void destroy_colormap(void)
{
	switch (x11_cmap_type) {
	case 0:
		XFreeColors(x11_display, DefaultColormapOfScreen(x11_screen),
					color_cell, sz_color_cell, 0);
		break;

	case 1:
		XFreeColormap(x11_display, x11_colormap);
		break;

	case 2:
		XFreeColors(x11_display, x11_colormap,
					color_cell, nr_color_cell_used, 0);
		break;
	}

	sz_color_cell = 0;
}


/*======================================================================*/

static XImage *image;


static void *create_image(int width, int height)
{
	void *buf = NULL;

#ifdef MITSHM
	if (use_SHM) {						/* MIS-SHM が実装されてるかを判定 */
		int tmp;
		if (! XQueryExtension(x11_display, "MIT-SHM", &tmp, &tmp, &tmp)) {
			if (verbose_proc) {
				printf("  X-Server not support MIT-SHM\n");
			}
			use_SHM = FALSE;
		}
	}

	if (use_SHM) {

		if (verbose_proc) printf("  Using shared memory (MIT-SHM):\n"
									 "    CreateImage ... ");
		image = XShmCreateImage(x11_display, x11_visual, x11_depth,
								ZPixmap, NULL, &SHMInfo,
								width, height);

		if (image) {

			if (verbose_proc) {
				printf("GetInfo ... ");
			}
			SHMInfo.shmid = shmget(IPC_PRIVATE,
								   image->bytes_per_line * image->height,
								   IPC_CREAT | 0777);
			if (SHMInfo.shmid < 0) {
				use_SHM = FALSE;
			}

			XSetErrorHandler(private_handler);	/* エラーハンドラを横取り */
			/* (XShmAttach()異常検出) */
			if (use_SHM) {

				if (verbose_proc) {
					printf("Allocate ... ");
				}
				image->data =
				SHMInfo.shmaddr = (char *)shmat(SHMInfo.shmid, 0, 0);
				if (image->data == NULL) {
					use_SHM = FALSE;
				}

				if (use_SHM) {
					if (verbose_proc) {
						printf("Attach ... ");
					}
					SHMInfo.readOnly = False;

					if (! XShmAttach(x11_display, &SHMInfo)) {
						use_SHM = FALSE;
					}

					XSync(x11_display, False);
					/* sleep(2); */
				}
			}

			if (SHMInfo.shmid >= 0) {
				shmctl(SHMInfo.shmid, IPC_RMID, 0);
			}


			if (use_SHM) {								/* すべて成功 */
				buf = image->data;
				if (verbose_proc) {
					printf("OK\n");
				}
			} else {									/* どっかで失敗 */
				if (verbose_proc) {
					printf("FAILED(can't use shared memory)\n");
				}
				if (SHMInfo.shmaddr) {
					shmdt(SHMInfo.shmaddr);
				}
				XDestroyImage(image);
				image = NULL;
			}

			XSetErrorHandler(None);				/* エラーハンドラを戻す */

		} else {
			if (verbose_proc) {
				printf("FAILED(can't use shared memory)\n");
			}
			use_SHM = FALSE;
		}
	}

	if (use_SHM == FALSE)
#endif
	{
		/* スクリーンバッファを確保 */

		if (verbose_proc) {
			printf("  Screen buffer: Memory allocate ... ");
		}
		buf = malloc(width * height * x11_byte_per_pixel);
		if (verbose_proc) {
			if (buf == NULL) {
				printf("FAILED\n");
			}
		}

		if (buf) {
			/* スクリーンバッファをイメージに割り当て */

			if (verbose_proc) {
				printf("CreateImage ... ");
			}
			image = XCreateImage(x11_display, x11_visual, x11_depth,
								 ZPixmap, 0, buf,
								 width, height, 8, 0);
			if (verbose_proc) {
				printf("%s\n", (image ? "OK" : "FAILED"));
			}
			if (image == NULL) {
				free(buf);
				buf = NULL;
			}
		}
	}

	return buf;
}


static void destroy_image(void *buf)
{

#ifdef MITSHM
	if (use_SHM) {
		XShmDetach(x11_display, &SHMInfo);
		if (SHMInfo.shmaddr) {
			shmdt(SHMInfo.shmaddr);
		}
		/*if (SHMInfo.shmid >= 0) shmctl(SHMInfo.shmid,IPC_RMID,0);*/
	}
#endif

	if (image) {
		XDestroyImage(image);
		image = NULL;
	}

#if 0			/* buf はもう不要なので、ここで free しようとしたが、
				   image のほうでまだ使用中らしく、free するとコケてしまう。
				   というか、 XDestroyImage してるのに………。
				   XSync をしてもだめな様子。じゃあ、いつ free するの ?? */
#ifdef MITSHM
	if (use_SHM == FALSE)
#endif
	{
		free(buf);
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
	XColor xcolor[256];

	/* debug */
	if (nr_color_cell_used + nr_color > sz_color_cell) {
		/* 追加すべき色が多すぎ */
		printf("color add err? %d %d\n", nr_color, nr_color_cell_used);
		return;
	}


	for (i = 0; i < nr_color; i++) {
		xcolor[i].red   = (unsigned short)color[i].red   << 8;
		xcolor[i].green = (unsigned short)color[i].green << 8;
		xcolor[i].blue  = (unsigned short)color[i].blue  << 8;
		xcolor[i].flags = DoRed | DoGreen | DoBlue;
	}


	switch (x11_cmap_type) {
	case 0:
	case 1:
		for (i = 0; i < nr_color; i++) {
			pixel[i] =
				xcolor[i].pixel = color_cell[ nr_color_cell_used ];
			nr_color_cell_used ++;
		}

		XStoreColors(x11_display, x11_colormap, xcolor, nr_color);
		break;

	case 2:
		for (i = 0; i < nr_color; i++) {
			if (XAllocColor(x11_display, x11_colormap, &xcolor[i])) {
				/* 成功 */;		/* DO NOTHING */
			} else {
				/* 失敗したら、黒色を確保。これは失敗しないだろう… */
				xcolor[i].red = xcolor[i].green = xcolor[i].blue = 0;
				XAllocColor(x11_display, x11_colormap, &xcolor[i]);
			}
			pixel[i] =
				color_cell[ nr_color_cell_used ] = xcolor[i].pixel;
			nr_color_cell_used ++;
		}
	}
}

/************************************************************************/

void graph_remove_color(int nr_pixel, unsigned long pixel[])
{
	/* debug */
	if (nr_pixel > nr_color_cell_used) {
		/* 削除すべき色が多すぎ */
		printf("color remove err? %d %d\n", nr_pixel, nr_color_cell_used);
	} else {
		if (memcmp(&color_cell[ nr_color_cell_used - nr_pixel ], pixel,
				   sizeof(unsigned long) * nr_pixel) != 0) {
			/* 削除すべき色が、直前に追加した色と違う */
			printf("color remove unmatch???\n");
		}
	}


	switch (x11_cmap_type) {
	case 0:
	case 1:
		nr_color_cell_used -= nr_pixel;
		break;
	case 2:
		nr_color_cell_used -= nr_pixel;
		XFreeColors(x11_display, x11_colormap, pixel, nr_pixel, 0);
		break;
	}
}


/************************************************************************
 * グラフィックの更新
 ************************************************************************/

void graph_update(int nr_rect, T_GRAPH_RECT rect[])
{
	int i;

	for (i = 0; i < nr_rect; i++) {
#ifdef MITSHM
		if (use_SHM) {
			XShmPutImage(x11_display, x11_window, x11_gc, image,
						 rect[i].x, rect[i].y,
						 rect[i].x, rect[i].y,
						 rect[i].w, rect[i].h, False);
		} else
#endif
		{
			XPutImage(x11_display, x11_window, x11_gc, image,
					  rect[i].x, rect[i].y,
					  rect[i].x, rect[i].y,
					  rect[i].w, rect[i].h);
		}
	}

	if (use_xsync) {
		XSync(x11_display, False);
	} else {
		XFlush(x11_display);
	}
}


/************************************************************************
 * タイトルの設定
 * 属性の設定
 ************************************************************************/

void graph_set_window_title(const char *title)
{
	static char saved_title[128];

	if (title) {
		saved_title[0] = '\0';
		strncat(saved_title, title, sizeof(saved_title) - 1);
	}

	if (graph_exist) {
		XStoreName(x11_display, x11_window, saved_title);
	}
}

/************************************************************************/

void graph_set_attribute(int mouse_show, int grab, int keyrepeat_on,
						 int *result_show, int *result_grab)
{
	x11_mouse_show   = mouse_show;
	x11_grab         = grab;
	x11_keyrepeat_on = keyrepeat_on;

	x11_reset_grab = TRUE;
	if (x11_get_focus) {
		x11_set_attribute_focus_in();
	}

	*result_show = x11_mouse_show;
	*result_grab = x11_grab;
}


/***********************************************************************
 *
 * X11 独自関数
 *
 ************************************************************************/

/* フォーカスを失った時、マウス表示、キーリピートありにする */
void x11_set_attribute_focus_out(void)
{
	XUndefineCursor(x11_display, x11_window);
	XAutoRepeatOn(x11_display);
}

/* フォーカスを得た時、、マウス・グラブ・キーリピートを、設定通りに戻す */
void x11_set_attribute_focus_in(void)
{
	int fullscreen = (graph_info.fullscreen) ? TRUE : FALSE;

	if (x11_mouse_show) {
		XUndefineCursor(x11_display, x11_window);
	} else {
		XDefineCursor(x11_display, x11_window, x11_cursor_id);
	}

	if (x11_reset_grab) {
		x11_reset_grab = FALSE;

		if (x11_grab || fullscreen) {
			/* グラブ指示あり、または フルスクリーン時はグラブする */
			XGrabKeyboard(x11_display, x11_window, True, GrabModeAsync,
						  GrabModeAsync, CurrentTime);
			XGrabPointer(x11_display, x11_window, True,
						 PointerMotionMask | ButtonPressMask |
						 ButtonReleaseMask,
						 GrabModeAsync, GrabModeAsync,
						 x11_window, None, CurrentTime);
		} else {
			XUngrabKeyboard(x11_display, CurrentTime);
			XUngrabPointer(x11_display, CurrentTime);
		}
	}

	if (x11_keyrepeat_on) {
		XAutoRepeatOn(x11_display);
	} else {
		XAutoRepeatOff(x11_display);
	}


	/* マウス移動によるイベント発生時の処理方法を設定 */

//	if (fullscreen) {
//		x11_mouse_rel_move = 1;
//	} else
	if (x11_grab &&
		x11_mouse_show == FALSE) {
		x11_mouse_rel_move = -1;
	} else {
		x11_mouse_rel_move = 0;
	}

	/* マウス移動時のイベントについて (event.c)

	   DGA の場合、マウス移動量が通知される   → 1: 相対移動
	   X11 の場合、マウス絶対位置が通知される → 0: 絶対移動

	   ウインドウグラブ時は、マウスがウインドウの端にたどり着くと
	   それ以上動けなくなる。
	   そこで、マウスを常にウインドウの中央にジャンプさせることで、
	   無限にマウスを動かすことができるかのようにする。
	   この時のマウスの移動処理だが、計算により相対量として扱う。
	   また、マウスを非表示にしておかないと、無様な様子が見えるので
	   この機能はマウスなし時のみとする       → -1: むりやり相対移動
	*/
}
