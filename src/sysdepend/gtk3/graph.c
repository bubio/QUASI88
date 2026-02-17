/***********************************************************************
 * グラフィック処理 (システム依存)
 *
 *      詳細は、 graph.h 参照
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "quasi88.h"
#include "graph.h"
#include "event.h"
#include "device.h"


static T_GRAPH_SPEC graph_spec;					/* 基本情報 */

static int          graph_exist;				/* 真で、画面生成済み */
static T_GRAPH_INFO graph_info;					/* その時の、画面情報 */


/************************************************************************
 * グラフィック処理の初期化
 * グラフィック処理の動作
 * グラフィック処理の終了
 ************************************************************************/

const T_GRAPH_SPEC *graph_init(void)
{
	if (verbose_proc) {
		printf("Initializing Graphic System ... ");
	}

	graph_spec.window_max_width      = 640;
	graph_spec.window_max_height     = 480;
	graph_spec.fullscreen_max_width  = 0;
	graph_spec.fullscreen_max_height = 0;
	graph_spec.touchkey_window        = FALSE;
	graph_spec.touchkey_fullscreen    = FALSE;
	graph_spec.touchkey_notify        = NULL;
	graph_spec.forbid_half           = FALSE;

	if (verbose_proc) {
		printf("OK\n");
	}

	return &graph_spec;
}

/************************************************************************/
static GtkWidget *main_window;
static GtkWidget *menu_bar;

static GtkWidget *event_box;

static GtkImage *image;
static GdkPixbuf *pixbuf;
#define BYTES_PER_PIXEL		(3)

static void free_pixels(guchar *pixels, gpointer data);
static int create_image(int width, int height);

const T_GRAPH_INFO *graph_setup(int width, int height,
								int fullscreen, double aspect)
{
	GtkWidget *vbox;

	/* fullscreen, aspect は未使用 */

	if (image) {
		gtk_widget_destroy(GTK_WIDGET(image));
		image = NULL;
		g_object_unref(G_OBJECT(pixbuf));
		pixbuf = NULL;
	}

	if (graph_exist == FALSE) {

		/* ウインドウを生成する */
		main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		{
			gtksys_set_signal_frame(main_window);
		}

		/* メニューバーを生成する */
		{
			create_menubar(main_window, &menu_bar);
			gtk_widget_show(menu_bar);

			quasi88_set_memu_callback(menubar_controll);
		}

		/* イベントボックスを生成する */
		event_box = gtk_event_box_new();
		{
			gtksys_set_signal_view(event_box);
		}
		gtk_widget_show(event_box);


		/* メニューバーとイベントボックスをパックして表示 */
		vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_show(vbox);
		gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(vbox), event_box, FALSE, TRUE, 0);

		gtk_container_add(GTK_CONTAINER(main_window), vbox);
		gtk_widget_show(main_window);


		graph_exist = TRUE;
	}

	/* 描画領域を生成する */
	if (image == NULL) {
		guchar *pixels = calloc(1, width * height * BYTES_PER_PIXEL);
		pixbuf = gdk_pixbuf_new_from_data(pixels, GDK_COLORSPACE_RGB, 0,
										  8, width, height,
										  width * BYTES_PER_PIXEL,
										  free_pixels, NULL);
		gdk_pixbuf_fill(pixbuf, 0x00000000);

		image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));

		gtk_widget_show(GTK_WIDGET(image));

		/* 描画領域を、イベントボックスに乗せる */
		gtk_container_add(GTK_CONTAINER(event_box), GTK_WIDGET(image));
	}


	/* 描画バッファを生成する */
	if (create_image(width, height)) {

		GdkGeometry hints;
		hints.min_width = width;
		hints.max_width = width;
		hints.min_height = height;
		hints.max_height = height;
		gtk_window_set_geometry_hints(GTK_WINDOW(main_window), NULL, &hints,
									  (GdkWindowHints)(GDK_HINT_MIN_SIZE |
													   GDK_HINT_MAX_SIZE));
		gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);

		return &graph_info;
	}

	return NULL;
}

static void free_pixels(guchar *pixels, gpointer data)
{
	free(pixels);
}

static unsigned int *buffer = NULL;

static int create_image(int width, int height)
{
	if (buffer) {
		free(buffer);
		buffer = NULL;
	}

	buffer = malloc(width * height * sizeof(unsigned int));

	if (buffer == NULL) {
		return FALSE;
	}

	memset(buffer, 0, width * height * sizeof(unsigned int));

	graph_info.byte_per_pixel   = sizeof(unsigned int);
	graph_info.byte_per_line    = width * sizeof(unsigned int);
	graph_info.buffer           = buffer;

	graph_info.fullscreen       = FALSE;
	graph_info.width            = width;
	graph_info.height           = height;
	graph_info.nr_color         = 256;
	graph_info.write_only       = FALSE;
	graph_info.broken_mouse     = FALSE;
	graph_info.draw_start       = NULL;
	graph_info.draw_finish      = NULL;
	graph_info.dont_frameskip   = NULL;

	return TRUE;
}


/************************************************************************/

void graph_exit(void)
{
	if (buffer) {
		free(buffer);
	}
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
		pixel[i] = ((((unsigned long) color[i].red)   << 16) |
					(((unsigned long) color[i].green) <<  8) |
					(((unsigned long) color[i].blue)));
	}
}

/************************************************************************/

void graph_remove_color(int nr_pixel, unsigned long pixel[])
{
	/* DO NOTHING */
}

/************************************************************************
 * グラフィックの更新
 ************************************************************************/

void graph_update(int nr_rect, T_GRAPH_RECT rect[])
{
	int i, h, w;
	int width = graph_info.width;
	int height = graph_info.height;
	/*GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);*/
	guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
	unsigned int *src;
	guchar *dst;

#if 1 /* 差分描画 */
	for (i = 0; i < nr_rect; i++) {
		for (h = 0; h < rect[i].h; h++) {
			src = (buffer
				   + (rect[i].y + h) * width
				   + (rect[i].x));
			dst = (pixels
				   + (rect[i].y + h) * width * BYTES_PER_PIXEL
				   + (rect[i].x) * BYTES_PER_PIXEL);
			for (w = 0; w < rect[i].w; w++) {
				unsigned int rgb = *src++;
				dst[0] = (rgb >> 16) & 0xff;
				dst[1] = (rgb >> 8) & 0xff;
				dst[2] = (rgb) & 0xff;
				dst += 3;
			}
		}
	}

#else /* 全部描画 */
	src = buffer;
	dst = pixels;
	for (h = 0; h < height; h++) {
		for (w = 0; w < width; w++) {
			unsigned int rgb = *src++;
			dst[0] = (rgb >> 16) & 0xff;
			dst[1] = (rgb >> 8) & 0xff;
			dst[2] = (rgb) & 0xff;
			dst += 3;
		}
	}
#endif

	gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
}


/************************************************************************
 * タイトルの設定
 * 属性の設定
 ************************************************************************/

void graph_set_window_title(const char *title)
{
	if (main_window) {
		gtk_window_set_title(GTK_WINDOW(main_window), title);
	}
}

/************************************************************************/

void graph_set_attribute(int mouse_show, int grab, int keyrepeat_on,
						 int *result_show, int *result_grab)
{
	*result_show = TRUE;
	*result_grab = FALSE;
}
