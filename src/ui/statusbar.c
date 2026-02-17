/************************************************************************/
/*                                                                      */
/* ステータス部の表示 (FDD表示、ほかメッセージ表示)                     */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "q8tk.h"
#include "statusbar.h"
#include "menu.h"				/* menu_lang    */

#include "pc88main.h"           /* boot_basic   */
#include "keyboard.h"           /* key_scan     */
#include "screen.h"

#include "drive.h"              /* get_drive_ready()    */



/*---------------------------------------------------------------------------*/

int status_imagename = TRUE;					/* イメージ名表示有無 */

/*---------------------------------------------------------------------------*/

static byte statusbar_bmp_capslock[ 2 * 16 ] = {
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x10, 0xc2,		/* 0001 0000 1100 0010 */
	0x11, 0x22,		/* 0001 0001 0010 0010 */
	0x11, 0x22,		/* 0001 0001 0010 0010 */
	0x12, 0x12,		/* 0001 0010 0001 0010 */
	0x12, 0x12,		/* 0001 0010 0001 0010 */
	0x13, 0xf2,		/* 0001 0011 1111 0010 */
	0x14, 0x0a,		/* 0001 0100 0000 1010 */
	0x14, 0x0a,		/* 0001 0100 0000 1010 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
};
static byte statusbar_bmp_kanalock[ 2 * 16 ] = {
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x11, 0x02,		/* 0001 0001 0000 0010 */
	0x17, 0xf2,		/* 0001 0111 1111 0010 */
	0x11, 0x12,		/* 0001 0001 0001 0010 */
	0x11, 0x12,		/* 0001 0001 0001 0010 */
	0x11, 0x12,		/* 0001 0001 0001 0010 */
	0x12, 0x12,		/* 0001 0010 0001 0010 */
	0x12, 0x12,		/* 0001 0010 0001 0010 */
	0x14, 0x22,		/* 0001 0100 0010 0010 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
};
static byte statusbar_bmp_romajilock[ 2 * 16 ] = {
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x17, 0xf2,		/* 0001 0111 1111 0010 */
	0x14, 0x12,		/* 0001 0100 0001 0010 */
	0x14, 0x12,		/* 0001 0100 0001 0010 */
	0x14, 0x12,		/* 0001 0100 0001 0010 */
	0x14, 0x12,		/* 0001 0100 0001 0010 */
	0x17, 0xf2,		/* 0001 0111 1111 0010 */
	0x14, 0x12,		/* 0001 0100 0001 0010 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x1f, 0xfc,		/* 0000 1111 1111 1100 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
};
static byte statusbar_bmp_numlock[ 2 * 16 ] = {
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0xc2,		/* 0001 0000 1100 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0x42,		/* 0001 0000 0100 0010 */
	0x10, 0xe2,		/* 0001 0000 1110 0010 */
	0x10, 0x02,		/* 0001 0000 0000 0010 */
	0x0f, 0xfc,		/* 0000 1111 1111 1100 */
	0x00, 0x00,		/* 0000 0000 0000 0000 */
};
static byte statusbar_bmp_tape_off[ 3 * 16 ] = {
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x30, 0x00, 0x0c,	/* 0011 0000 0000 0000 0000 1100 */
	0x28, 0x00, 0x14,	/* 0010 1000 0000 0000 0001 0100 */
	0x24, 0x00, 0x24,	/* 0010 0100 0000 0000 0010 0100 */
	0x23, 0xff, 0xc4,	/* 0010 0011 1111 1111 1100 0100 */
	0x20, 0x00, 0x04,	/* 0010 0000 0000 0000 0000 0100 */
	0x20, 0x81, 0x04,	/* 0010 0000 1000 0001 0000 0100 */
	0x21, 0xc3, 0x84,	/* 0010 0001 1100 0011 1000 0100 */
	0x20, 0x81, 0x04,	/* 0010 0000 1000 0001 0000 0100 */
	0x20, 0x00, 0x04,	/* 0010 0000 0000 0000 0000 0100 */
	0x20, 0x00, 0x04,	/* 0010 0000 0000 0000 0000 0100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
};
static byte statusbar_bmp_tape_on[ 3 * 16 ] = {
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x7f, 0xff, 0xfe,	/* 0111 1111 1111 1111 1111 1110 */
	0x40, 0x00, 0x02,	/* 0100 0000 0000 0000 0000 0010 */
	0x4f, 0xff, 0xf2,	/* 0100 1111 1111 1111 1111 0010 */
	0x57, 0xff, 0xea,	/* 0101 0111 1111 1111 1110 1010 */
	0x5b, 0xff, 0xda,	/* 0101 1011 1111 1111 1101 1010 */
	0x5c, 0x00, 0x3a,	/* 0101 1100 0000 0000 0011 1010 */
	0x5f, 0xff, 0xfa,	/* 0101 1111 1111 1111 1111 1010 */
	0x5f, 0x7e, 0xfa,	/* 0101 1111 0111 1110 1111 1010 */
	0x5e, 0x3c, 0x7a,	/* 0101 1110 0011 1100 0111 1010 */
	0x5f, 0x7e, 0xfa,	/* 0101 1111 0111 1110 1111 1010 */
	0x5f, 0xff, 0xfa,	/* 0101 1111 1111 1111 1111 1010 */
	0x5f, 0xff, 0xfa,	/* 0101 1111 1111 1111 1111 1010 */
	0x40, 0x00, 0x02,	/* 0100 0000 0000 0000 0000 0010 */
	0x7f, 0xff, 0xfe,	/* 0111 1111 1111 1111 1111 1110 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
};
static byte statusbar_bmp_disk_off[ 3 * 16 ] = {
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x2a, 0xaa, 0xa8,	/* 0010 1010 1010 1010 1010 1000 */
	0x15, 0x55, 0x54,	/* 0001 0101 0101 0101 0101 0100 */
	0x2a, 0xaa, 0xa8,	/* 0010 1010 1010 1010 1010 1000 */
	0x15, 0x55, 0x54,	/* 0001 0101 0101 0101 0101 0100 */
	0x2a, 0xaa, 0xa8,	/* 0010 1010 1010 1010 1010 1000 */
	0x15, 0x55, 0x54,	/* 0001 0101 0101 0101 0101 0100 */
	0x2a, 0xaa, 0xa8,	/* 0010 1010 1010 1010 1010 1000 */
	0x15, 0x55, 0x54,	/* 0001 0101 0101 0101 0101 0100 */
	0x2a, 0xaa, 0xa8,	/* 0010 1010 1010 1010 1010 1000 */
	0x15, 0x55, 0x54,	/* 0001 0101 0101 0101 0101 0100 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
};
static byte statusbar_bmp_disk_on[ 3 * 16 ] = {
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x3f, 0xff, 0xfc,	/* 0011 1111 1111 1111 1111 1100 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
	0x00, 0x00, 0x00,	/* 0000 0000 0000 0000 0000 0000 */
};

/*---------------------------------------------------------------------------*/

/* 現在の状態と、それを表すウィジットを覚えておく */
static struct {

	struct {
		char	disk[2];				/* 0:OFF 1:READY 2:BUSY */
		char	tape;					/* 0:OFF 1:EXIST 2:READ 3:WRITE */
		char	capskey;				/* 0:OFF 1:ON */
		char	kanakey;				/* 0:OFF 1:KANA 2:ROMAJI */
		char	numkey;					/* 0:OFF 1:ON */
	} stat;

	Q8tkWidget *message;				/* メッセージエリア   33桁 */
	Q8tkWidget *image_name[2];			/* ドライブイメージ名 16桁 x 2*/
	Q8tkWidget *led_disk[2];			/* ドライブランプ      3桁 x 2*/
	Q8tkWidget *led_tape;				/* テープアイコン      3桁 */
	Q8tkWidget *led_capskey;			/* CAPSインジケータ    2桁 */
	Q8tkWidget *led_kanakey;			/* かなインジケータ    2桁 */
	Q8tkWidget *led_numkey;				/* 数字インジケータ    2桁 */

	Q8tkWidget *bmp_capslock;			/* BMP CAPSインジケータ */
	Q8tkWidget *bmp_kanalock;			/* BMP かなインジケータ */
	Q8tkWidget *bmp_romajilock;			/* BMP ローマ字インジケータ */
	Q8tkWidget *bmp_numlock;			/* BMP 数字インジケータ */
	Q8tkWidget *bmp_tape_off;			/* BMP テープ停止アイコン */
	Q8tkWidget *bmp_tape_on;			/* BMP テープ動作アイコン */
	Q8tkWidget *bmp_disk_off;			/* BMP ドライブランプ消灯 */
	Q8tkWidget *bmp_disk_on;   			/* BMP ドライブランプ点灯 */
} statusbar_info;

/* ステータスバーの情報の並び順 */
static struct {
	Q8tkWidget	**p_widget;
	char		type;					/* 0:BMP 1:UTF8 2:SJIS */
	char		width;
	char		height;
} statusbar_layout[] = {
	{ &statusbar_info.message,			1,	33,	0,	},
	{ &statusbar_info.led_capskey,		0,	16,	16,	},
	{ &statusbar_info.led_kanakey,		0,	16,	16,	},
	{ &statusbar_info.led_numkey,		0,	16,	16,	},
	{ &statusbar_info.led_tape,			0,	24,	16,	},
	{ &statusbar_info.led_disk[1],		0,	24,	16,	},
	{ &statusbar_info.image_name[1],	2,	16,	0,	},
	{ &statusbar_info.led_disk[0],		0,	24,	16,	},
	{ &statusbar_info.image_name[0],	2,	16,	0,	},
};

/* ビットマップのウィジットを覚えておく */
static struct {
	Q8tkWidget	**p_widget;
	int			width;
	int			height;
	byte		*bmp;
} statusbar_bmp[] = {
	{ &statusbar_info.bmp_capslock,		16,	16,	statusbar_bmp_capslock,		},
	{ &statusbar_info.bmp_kanalock,		16,	16,	statusbar_bmp_kanalock,		},
	{ &statusbar_info.bmp_romajilock,	16,	16,	statusbar_bmp_romajilock,	},
	{ &statusbar_info.bmp_numlock,		16,	16,	statusbar_bmp_numlock,		},
	{ &statusbar_info.bmp_tape_off,		24,	16,	statusbar_bmp_tape_off,		},
	{ &statusbar_info.bmp_tape_on,		24,	16,	statusbar_bmp_tape_on,		},
	{ &statusbar_info.bmp_disk_off,		24,	16,	statusbar_bmp_disk_off,		},
	{ &statusbar_info.bmp_disk_on,		24,	16,	statusbar_bmp_disk_on,		},
};



/***************************************************************************
 *
 ****************************************************************************/
void statusbar_set_imagename_show(int show)
{
	status_imagename = show;

	if (status_imagename) {
		q8tk_item_label_set_width(statusbar_info.message, 33);
		q8tk_item_label_set_width(statusbar_info.image_name[1],	16);
		q8tk_item_label_set_width(statusbar_info.image_name[0],	16);
	} else {
		q8tk_item_label_set_width(statusbar_info.message, 33 + 16 + 16);
		q8tk_item_label_set_width(statusbar_info.image_name[1],	16 - 16);
		q8tk_item_label_set_width(statusbar_info.image_name[0],	16 - 16);
	}

	quasi88_set_status();
	statusbar_image_name();
}
int statusbar_get_imagename_show(void)
{
	return status_imagename;
}



/***************************************************************************
 * ステータスバー関係の初期化
 ****************************************************************************/
void statusbar_init(void)
{
	int i, code;
	Q8tkWidget *win, *bar;

	memset(&statusbar_info, 0, sizeof(statusbar_info));


	win = q8tk_window_new(Q8TK_WINDOW_STAT);

	bar = q8tk_statusbar_new();
	q8tk_widget_show(bar);
	q8tk_container_add(win, bar);

	for (i = 0; i < COUNTOF(statusbar_layout); i++) {
		Q8tkWidget *w;
		switch (statusbar_layout[i].type) {
		case 0:
			w = q8tk_item_bitmap_new(statusbar_layout[i].width,
									 statusbar_layout[i].height);
			break;
		case 1:
			w = q8tk_item_label_new(statusbar_layout[i].width);
			break;
		case 2:
			code = q8tk_set_kanjicode(Q8TK_KANJI_SJIS);
			w = q8tk_item_label_new(statusbar_layout[i].width);
			q8tk_set_kanjicode(code);
			break;
		}
		q8tk_statusbar_add(bar, w);
		*statusbar_layout[i].p_widget = w;
	}

	for (i = 0; i < COUNTOF(statusbar_bmp); i++) {
		*statusbar_bmp[i].p_widget = q8tk_bitmap_new(statusbar_bmp[i].width,
													 statusbar_bmp[i].height,
													 statusbar_bmp[i].bmp);
	}

	statusbar_set_imagename_show(status_imagename);

	q8tk_grab_add(win);
	q8tk_widget_show(win);

	statusbar_update();

	set_screen_dirty_stat_full();
}

/***************************************************************************
 * ステータスバーのアイコン類更新
 * VSYNC毎に呼び出す
 ****************************************************************************/
void statusbar_update(void)
{
	int i, stat;

	for (i = 0; i < 2; i++) {
		if (get_drive_ready(i)) {
			stat = 1;
		} else {
			stat = 2;
		}
		if (statusbar_info.stat.disk[i] != stat) {
			statusbar_info.stat.disk[i] = stat;
			switch (stat) {
			case 0:
				q8tk_item_bitmap_clear(statusbar_info.led_disk[i]);
				break;
			case 1:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_disk[i],
											statusbar_info.bmp_disk_off);
				break;
			case 2:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_disk[i],
											statusbar_info.bmp_disk_on);
				q8tk_item_bitmap_set_color(statusbar_info.led_disk[i],
										   Q8GR_PALETTE_RED, -1);
				break;
			}
		}
	}

	{
		if (tape_writing()) {
			stat = 3;
		} else if (tape_reading()) {
			stat = 2;
		} else if (tape_exist()) {
			stat = 1;
		} else {
			stat = 0;
		}
		if (statusbar_info.stat.tape != stat) {
			statusbar_info.stat.tape = stat;
			switch (stat) {
			case 0:
				q8tk_item_bitmap_clear(statusbar_info.led_tape);
				break;
			case 1:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_tape,
											statusbar_info.bmp_tape_off);
				break;
			case 2:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_tape,
											statusbar_info.bmp_tape_on);
				break;
			case 3:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_tape,
											statusbar_info.bmp_tape_on);
				q8tk_item_bitmap_set_color(statusbar_info.led_tape,
										   Q8GR_PALETTE_RED, -1);
				break;
			}
		}
	}

	{
		if ((key_scan[0x0a] & 0x80) == 0) {
			stat = 1;
		} else {
			stat = 0;
		}
		if (statusbar_info.stat.capskey != stat) {
			statusbar_info.stat.capskey = stat;
			if (stat == 0) {
				q8tk_item_bitmap_clear(statusbar_info.led_capskey);
			} else {
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_capskey,
											statusbar_info.bmp_capslock);
			}
		}
	}

	{
		if ((key_scan[0x08] & 0x20) == 0) {
			if (romaji_input_mode) {
				stat = 2;
			} else {
				stat = 1;
			}
		} else {
			stat = 0;
		}
		if (statusbar_info.stat.kanakey != stat) {
			statusbar_info.stat.kanakey = stat;
			switch (stat) {
			case 0:
				q8tk_item_bitmap_clear(statusbar_info.led_kanakey);
				break;
			case 1:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_kanakey,
											statusbar_info.bmp_kanalock);
				break;
			case 2:
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_kanakey,
											statusbar_info.bmp_romajilock);
				break;
			}
		}
	}

	{
		if (numlock_emu) {
			stat = 1;
		} else {
			stat = 0;
		}
		if (statusbar_info.stat.numkey != stat) {
			statusbar_info.stat.numkey = stat;
			if (stat == 0) {
				q8tk_item_bitmap_clear(statusbar_info.led_numkey);
			} else {
				q8tk_item_bitmap_set_bitmap(statusbar_info.led_numkey,
											statusbar_info.bmp_numlock);
			}
		}
	}
}

/***************************************************************************
 * ステータスバーにメッセージを表示する
 * ステータスバーに周期メッセージを表示する
 ****************************************************************************/
static int now_cyclic = FALSE;
void statusbar_message(int timeout, const char *message_ascii, const char *message_japanese)
{
	const char *message;
	if ((menu_lang == MENU_JAPAN) && (message_japanese)) {
		message = message_japanese;
	} else {
		message = message_ascii;
	}

	if (timeout == 0) {
		q8tk_item_label_set(statusbar_info.message, message);
		now_cyclic = FALSE;
	} else {
		if (now_cyclic == FALSE) {
			q8tk_item_label_show_message(statusbar_info.message,
										 message, timeout);
		}
	}
}

void statusbar_message_cyclic(int timeout, const char *(*get_message)(void))
{
	if (timeout == 0) {
		q8tk_item_label_set(statusbar_info.message, (get_message)());
		now_cyclic = FALSE;
	} else {
		q8tk_item_label_set_cyclic(statusbar_info.message,
								   get_message, timeout);
		now_cyclic = TRUE;
	}
}

/***************************************************************************
 * ステータスバーにディスクイメージ名を表示する
 ****************************************************************************/
void statusbar_image_name(void)
{
	int i;

	for (i = 0; i < 2; i++) {
		const char *s = "";

		if (status_imagename) {
			if (disk_image_exist(i) &&
				drive_check_empty(i) == FALSE) {

				s = drive[i].image[ disk_image_selected(i) ].name;
				q8tk_item_bitmap_set_color(statusbar_info.image_name[i],
										   Q8GR_PALETTE_FONT_FG,
										   Q8GR_PALETTE_FONT_BG);
			} else {
				q8tk_item_bitmap_set_color(statusbar_info.image_name[i],
										   -1, -1);
			}

		} else {
			q8tk_item_bitmap_set_color(statusbar_info.image_name[i],
									   -1, -1);
		}

		q8tk_item_label_set(statusbar_info.image_name[i], s);
	}
}
