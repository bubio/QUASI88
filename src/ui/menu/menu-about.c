/*===========================================================================
 *
 * メインページ バージョン情報
 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "menu.h"

#include "snddrv.h"
#include "screen.h"
#include "screen-common.h"

#include "q8tk.h"

#include "menu-common.h"

#include "menu-about.h"
#include "menu-about-message.h"


/* ロゴの実体はこれ。 192x48ドット == 24文字x3行 */

#include "menu-about-logo.c"

#define LOGO_HEIGHT		(BMP_LOGO_H / 16)


/*----------------------------------------------------------------------*/
/* テーマ */
static int get_about_thema(void)
{
	return menu_get_thema();
}
static void cb_about_thema(UNUSED_WIDGET, void *p)
{
	int val = P2INT(p);
	if (menu_get_thema() != val) {
		menu_set_thema(val);
		menu_palette(quasi88_cfg_can_interp());
	}
}


static Q8tkWidget *menu_about_thema(void)
{
	Q8tkWidget *hbox;

	hbox = PACK_HBOX(NULL);
	{
		PACK_RADIO_BUTTONS(hbox,
						   data_about_thema, COUNTOF(data_about_thema),
						   get_about_thema(), cb_about_thema);
	}

	return hbox;
}

/*======================================================================*/

Q8tkWidget *menu_about(void)
{
	int i;
	Q8tkWidget *vx, *hx, *vbox, *swin, *hbox, *w;

	vx = PACK_VBOX(NULL);
	{
		/* 上半分にロゴ表示 */
		hx = PACK_HBOX(vx);
		{
			/* インデント */
			PACK_LABEL(hx, " ");

			if (strcmp(Q_TITLE, "QUASI88") == 0) {
				w = q8tk_bitmap_new(BMP_LOGO_W, BMP_LOGO_H, bmp_logo_left);
				q8tk_widget_show(w);
				q8tk_box_pack_start(hx, w);

				w = q8tk_bitmap_new(BMP_LOGO_W, BMP_LOGO_H, bmp_logo_middle);
				q8tk_widget_show(w);
				q8tk_box_pack_start(hx, w);

				w = q8tk_bitmap_new(BMP_LOGO_W, BMP_LOGO_H, bmp_logo_right);
				q8tk_widget_show(w);
				q8tk_box_pack_start(hx, w);
			} else {
				PACK_LABEL(hx, Q_TITLE);
			}

			vbox = PACK_VBOX(hx);
			{
				i = LOGO_HEIGHT;

				PACK_LABEL(vbox, "  " Q_COPYRIGHT);
				i--;

				for (; i > 1; i--) {
					PACK_LABEL(vbox, "");
				}

				PACK_LABEL(vbox, "  ver. " Q_VERSION  "  <" Q_COMMENT ">");
			}
		}

		/* 下半分は情報表示 */
		swin = q8tk_scrolled_window_new(NULL, NULL);
		{
			hbox = PACK_HBOX(NULL);
			{
				vbox = PACK_VBOX(hbox);
				{
					{
						/* サウンドに関する情報表示 */
						const char *(*s) =
							(menu_lang == 0) ? data_about_en : data_about_jp;

						for (i = 0; s[i]; i++) {
							if (strcmp(s[i], "@MAMEVER") == 0) {
								PACK_LABEL(vbox, xmame_version_mame());
							} else if (strcmp(s[i], "@FMGENVER") == 0) {
								PACK_LABEL(vbox, xmame_version_fmgen());
							} else {
								PACK_LABEL(vbox, s[i]);
							}
						}
					}

					{
						/* システム依存部に関する情報表示 */
						int new_code, save_code = 0;
						const char *msg;
						char buf[256];
						int i;

						if (menu_about_osd_msg(menu_lang, &new_code, &msg)) {

							if (menu_lang == MENU_JAPAN && new_code >= 0) {
								save_code = q8tk_set_kanjicode(new_code);
							}

							i = 0;
							for (;;) {
								if (i == 255 || *msg == '\n' || *msg == '\0') {
									buf[i] = '\0';
									PACK_LABEL(vbox, buf);
									i = 0;
									if (*msg == '\n') {
										msg++;
									}
									if (*msg == '\0') {
										break;
									}
								} else {
									buf[i] = *msg++;
									i++;
								}
							}

							if (menu_lang == MENU_JAPAN && new_code >= 0) {
								q8tk_set_kanjicode(save_code);
							}
						}
					}
				}
			}
			q8tk_container_add(swin, hbox);
		}

		q8tk_scrolled_window_set_policy(swin,
										Q8TK_POLICY_AUTOMATIC,
										Q8TK_POLICY_AUTOMATIC);
		q8tk_misc_set_size(swin, 78, 18 - LOGO_HEIGHT - 1);
		q8tk_widget_show(swin);
		q8tk_box_pack_start(vx, swin);

		/* 最下部にテーマ切り替えを表示 */
		hbox = PACK_HBOX(vx);
		{
			const t_menulabel *l = data_about;
			PACK_LABEL(hbox, GET_LABEL(l, DATA_ABOUT_THEMA));

			q8tk_box_pack_start(hbox, menu_about_thema());
		}
	}

	return vx;
}

/*
  TODO
  menu_about_osd_msg() の引数 *result_code は不要。応答してくる文字コードは
  ファイルの文字コードに限定でいいはず
*/
