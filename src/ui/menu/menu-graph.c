/*===========================================================================
 *
 * メインページ 画面
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "menu.h"

#include "screen.h"
#include "screen-common.h"
#include "pc88main.h"
#include "memory.h"

#include "q8tk.h"

#include "menu-common.h"

#include "menu-graph.h"
#include "menu-graph-message.h"


/*----------------------------------------------------------------------*/
/* フレームレート */
static int get_graph_frate(void)
{
	return quasi88_cfg_now_frameskip_rate();
}
static void cb_graph_frate(Q8tkWidget *widget, void *label)
{
	int i;
	const t_menudata *p = data_graph_frate;
	const char *combo_str = q8tk_combo_get_text(widget);
	char str[32];

	for (i = 0; i < COUNTOF(data_graph_frate); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			sprintf(str, " fps (-frameskip %d)", p->val);
			q8tk_label_set((Q8tkWidget *)label, str);

			quasi88_cfg_set_frameskip_rate(p->val);
			return;
		}
	}
}
/* thanks floi ! */
static int get_graph_autoskip(void)
{
	return use_auto_skip;
}
static void cb_graph_autoskip(Q8tkWidget *widget, UNUSED_PARM)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;
	use_auto_skip = key;
}


static Q8tkWidget *menu_graph_frate(void)
{
	Q8tkWidget *vbox, *hbox, *combo, *label;
	char wk[32];

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			label = q8tk_label_new(" fps");
			{
				sprintf(wk, "%6.3f", 60.0f / get_graph_frate());
				combo = PACK_COMBO(hbox,
								   data_graph_frate, COUNTOF(data_graph_frate),
								   get_graph_frate(), wk, 6,
								   cb_graph_frate, label,
								   NULL, NULL);
			}
			{
				q8tk_box_pack_start(hbox, label);
				q8tk_widget_show(label);
				cb_graph_frate(combo, (void *)label);
			}
		}

		/* 空行 */
		PACK_LABEL(vbox, "");

		/* thanks floi ! */
		PACK_CHECK_BUTTON(vbox,
						  GET_LABEL(data_graph_autoskip, 0),
						  get_graph_autoskip(),
						  cb_graph_autoskip, NULL);
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* 画面サイズ */
static int get_graph_resize(void)
{
	return quasi88_cfg_now_size();
}
static void cb_graph_resize(UNUSED_WIDGET, void *p)
{
	int new_size = P2INT(p);

	quasi88_cfg_set_size(new_size);
}
static int get_graph_fullscreen(void)
{
	return use_fullscreen;
}
static void cb_graph_fullscreen(Q8tkWidget *widget, UNUSED_PARM)
{
	int on = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	if (quasi88_cfg_can_fullscreen()) {
		quasi88_cfg_set_fullscreen(on);
	}
}


static Q8tkWidget *menu_graph_resize(void)
{
	Q8tkWidget *vbox;
	int i = COUNTOF(data_graph_resize);
	int j = quasi88_cfg_max_size() - quasi88_cfg_min_size() + 1;

	vbox = PACK_VBOX(NULL);
	{
		PACK_RADIO_BUTTONS(PACK_VBOX(vbox),
						   &data_graph_resize[quasi88_cfg_min_size()],
						   MIN(i, j),
						   get_graph_resize(), cb_graph_resize);

		if (quasi88_cfg_can_fullscreen()) {

			/* 空行 */
			PACK_LABEL(vbox, "");

			PACK_CHECK_BUTTON(vbox,
							  GET_LABEL(data_graph_fullscreen, 0),
							  get_graph_fullscreen(),
							  cb_graph_fullscreen, NULL);
		}
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* 雑多な設定 (モニタ周波数、デジタルモニタ、縮小補間、インターレース) */
static int get_graph_misc(int type)
{
	switch (type) {
	case DATA_GRAPH_MISC_15K:
		return (monitor_15k == 0x02) ? TRUE : FALSE;

	case DATA_GRAPH_MISC_DIGITAL:
		return (monitor_analog == FALSE) ? TRUE : FALSE;

	case DATA_GRAPH_MISC_NOINTERP:
		return (quasi88_cfg_now_interp() == FALSE) ? TRUE : FALSE;
	}
	return FALSE;
}
static void cb_graph_misc(Q8tkWidget *widget, void *p)
{
	int key = (Q8TK_TOGGLE_BUTTON(widget)->active) ? TRUE : FALSE;

	switch (P2INT(p)) {
	case DATA_GRAPH_MISC_15K:
		monitor_15k = (key) ? 0x02 : 0x00;
		return;

	case DATA_GRAPH_MISC_DIGITAL:
		monitor_analog = (key) ? FALSE : TRUE;
		return;

	case DATA_GRAPH_MISC_NOINTERP:
		if (quasi88_cfg_can_interp()) {
			quasi88_cfg_set_interp((key) ? FALSE : TRUE);
		}
		return;
	}
}
static int get_graph_misc2(void)
{
	return quasi88_cfg_now_interlace();
}
static void cb_graph_misc2(UNUSED_WIDGET, void *p)
{
	quasi88_cfg_set_interlace(P2INT(p));
}


static Q8tkWidget *menu_graph_misc(void)
{
	Q8tkWidget *vbox;
	const t_menudata *p = data_graph_misc;
	int i = COUNTOF(data_graph_misc);

	if (quasi88_cfg_can_interp() == FALSE) {
		i --;
	}

	vbox = PACK_VBOX(NULL);
	{
		PACK_CHECK_BUTTONS(vbox,
						   p, i,
						   get_graph_misc, cb_graph_misc);

		PACK_LABEL(vbox, "");

		PACK_RADIO_BUTTONS(vbox,
						   data_graph_misc2, COUNTOF(data_graph_misc2),
						   get_graph_misc2(), cb_graph_misc2);
	}

	return vbox;
}

/*----------------------------------------------------------------------*/
/* PCG-8100、フォント */
static Q8tkWidget *graph_font_widget;
static int get_graph_pcg(void)
{
	return use_pcg;
}
static void cb_graph_pcg(Q8tkWidget *widget, void *p)
{
	if (widget) {
		use_pcg = P2INT(p);
		memory_set_font();
	}

	if (use_pcg) {
		q8tk_widget_set_sensitive(graph_font_widget, FALSE);
	} else         {
		q8tk_widget_set_sensitive(graph_font_widget, TRUE);
	}
}

static int get_graph_font(void)
{
	return font_type;
}
static void cb_graph_font(UNUSED_WIDGET, void *p)
{
	font_type = P2INT(p);
	memory_set_font();
}

static Q8tkWidget *menu_graph_pcg(void)
{
	Q8tkWidget *vbox, *b;
	const t_menulabel *l = data_graph;
	t_menudata data_graph_font[3];


	/* フォント選択ウィジット生成 (PCG有無により、insensitive になる) */
	{
		data_graph_font[0] = data_graph_font1[(font_loaded & 1) ? 1 : 0];
		data_graph_font[1] = data_graph_font2[(font_loaded & 2) ? 1 : 0];
		data_graph_font[2] = data_graph_font3[(font_loaded & 4) ? 1 : 0];

		b = PACK_VBOX(NULL);
		{
			PACK_RADIO_BUTTONS(b,
							   data_graph_font, COUNTOF(data_graph_font),
							   get_graph_font(), cb_graph_font);
		}
		graph_font_widget = PACK_FRAME(NULL,
									   GET_LABEL(l, DATA_GRAPH_FONT), b);
	}

	/* PCG有無ウィジットと、フォント選択ウィジットを並べる */
	vbox = PACK_VBOX(NULL);
	{
		{
			b = PACK_HBOX(NULL);
			{
				PACK_RADIO_BUTTONS(b,
								   data_graph_pcg, COUNTOF(data_graph_pcg),
								   get_graph_pcg(), cb_graph_pcg);
			}
			PACK_FRAME(vbox, GET_LABEL(l, DATA_GRAPH_PCG), b);
		}

		q8tk_box_pack_start(vbox, graph_font_widget);
	}

	return vbox;
}

/*======================================================================*/

Q8tkWidget *menu_graph(void)
{
	Q8tkWidget *vbox, *hbox;
	const t_menulabel *l = data_graph;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(vbox);
		{
			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_GRAPH_FRATE), menu_graph_frate());

			PACK_FRAME(hbox,
					   GET_LABEL(l, DATA_GRAPH_RESIZE), menu_graph_resize());
		}

		hbox = PACK_HBOX(vbox);
		{
			q8tk_box_pack_start(hbox, menu_graph_misc());

			PACK_LABEL(hbox, " ");

			q8tk_box_pack_start(hbox, menu_graph_pcg());
		}
	}

	return vbox;
}
