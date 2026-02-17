/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"

#include "file-op.h"


/*---------------------------------------------------------------------------
 * FILE VIEWER
 *---------------------------------------------------------------------------
 * ・いきおいで作った、ファイルをちょっとだけみるユーティリティ
 * ・テキストファイルの判断は、拡張子のみ
 *   SJIS か EUC か UTF8 かを曖昧に判断。とりあえず UTF16 も。
 * ・そのうちにちゃんとしたものを作るとしよう………
 *   というか、出来がよくなる気配がなければ消そう………
 * ・ちなみにウィジットではなく、いきなりファイル表示する。
 *---------------------------------------------------------------------------*/
#define VIEW_MAX_LINE	(1000)					/* 表示する最大の行数 */
#define VIEW_MAX_COLUMN (120)					/* 処理する最大のバイト数/行 */
static	Q8tkWidget		*view_widget_win;
static	Q8tkWidget		*view_widget_vbox;
static	Q8tkWidget		*view_widget_swin;
static	Q8tkWidget		*view_widget_board;
static	Q8tkWidget		*view_widget_quit;
static	Q8tkWidget		*view_widget_accel;
static	Q8tkWidget		*view_string[ VIEW_MAX_LINE ];
static	int				view_string_cnt;

static void q8tk_utility_view_end(UNUSED_WIDGET, UNUSED_PARM);
int q8tk_utility_view(const char *filename)
{
	int save_code, file_code, file_text;
	OSD_FILE *fp;
	Q8tkWidget *w, *swin, *x, *b, *z;

	if (filename == NULL || *filename == '\0' ||
		osd_file_stat(filename) != FILE_STAT_FILE) {
		return 0;
	}

	fp = osd_fopen(FTYPE_READ, filename, "rb");

	if (fp == NULL) {
		return 0;
	}

	/* 拡張子が .txt .md ならテキストファイル。以外はバイナリファイルとする */
	{
		int l = (int) strlen(filename);
		if (l >= 4 && ((filename[l - 4] == '.') &&
					   (filename[l - 3] == 'T' || filename[l - 3] == 't') &&
					   (filename[l - 2] == 'X' || filename[l - 2] == 'x') &&
					   (filename[l - 1] == 'T' || filename[l - 1] == 't'))) {
			file_text = TRUE;
		} else if (l >= 3 &&
				   ((filename[l - 3] == '.') &&
					(filename[l - 2] == 'M' || filename[l - 2] == 'm') &&
					(filename[l - 1] == 'D' || filename[l - 1] == 'd'))) {
			file_text = TRUE;
		} else {
			file_text = FALSE;
		}
	}

	if (file_text == FALSE) {
		/* バイナリファイル */

		file_code = Q8TK_KANJI_ANK;

	} else {
		/* テキストファイル */

		/* ファイル先頭 1KB を適当に見て漢字コードを判定。ほんとに適当 */
		int  sz;
		char buf[1024];

		sz = (int)osd_fread(buf, sizeof(char), sizeof(buf), fp);
		if (osd_fseek(fp, 0, SEEK_SET) == -1) {
			osd_fclose(fp);
			return 0;
		}
		file_code = q8gr_strcode(buf, sz);
	}



	/* メインとなるウインドウ */
	{
		w = q8tk_window_new(Q8TK_WINDOW_DIALOG);
		view_widget_accel = q8tk_accel_group_new();
		q8tk_accel_group_attach(view_widget_accel, w);
	}
	/* それにボックスを乗せる */
	{
		x = q8tk_vbox_new();
		q8tk_container_add(w, x);
		q8tk_widget_show(x);
		/* ボックスには */
		{
			/* SCRLウインドウと */
			swin  = q8tk_scrolled_window_new(NULL, NULL);
			q8tk_widget_show(swin);
			q8tk_scrolled_window_set_policy(swin, Q8TK_POLICY_AUTOMATIC,
											Q8TK_POLICY_AUTOMATIC);
			q8tk_misc_set_size(swin, 78, 20);
			q8tk_box_pack_start(x, swin);
		}
		{
			/* 終了ボタンを配置 */
			b = q8tk_button_new_with_label(" O K ");
			q8tk_box_pack_start(x, b);
			q8tk_widget_show(b);

			q8tk_misc_set_placement(b, Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
			q8tk_signal_connect(b, "clicked", q8tk_utility_view_end, NULL);

			q8tk_accel_group_add(view_widget_accel, Q8TK_KEY_ESC,
								 b, "clicked");
		}
	}

	{
		/* SCRLウインドウに VBOXを作って */
		z = q8tk_vbox_new();
		q8tk_container_add(swin, z);
		q8tk_widget_show(z);

		save_code = q8tk_set_kanjicode(file_code);

		/* ファイル内容をラベルとして配置 */
		{
			int c;
			int line;
			int end_of_file;
			char src[ VIEW_MAX_COLUMN + 2 ];

			int leading_CR;
			int charmode16;
			int t1, t2; /* TABの置き換え文字 */

			if (file_text) {
				leading_CR = FALSE;

				switch (file_code) {
				case Q8TK_KANJI_UTF16LE_DUMMY:
					charmode16 = 1;
					t1 = ' ';
					t2 = (Q8TK_UTF16_DUMMYCODE >> 8);
					break;
				case Q8TK_KANJI_UTF16BE_DUMMY:
					charmode16 = 2;
					t1 = (Q8TK_UTF16_DUMMYCODE >> 8);
					t2 = ' ';
					break;
				default:
					charmode16 = 0;
					break;
				}
			}

			line = 0;
			end_of_file = FALSE;

			do {

				if (file_text) {
					/* テキストファイル */

					int d1, d2;
					int column = 0;
					int end_of_line = FALSE;

					memset(src, 0, sizeof(src));

					do {
						/* 1文字リード */
						if (charmode16 == 0) {
							c = osd_fgetc(fp);
						} else {
							/* UTF16の場合、2バイトで1文字になる。これをその
							 * まま char型で扱うと、 U+0001 - U+007F の文字は
							 * 0x00 が出現するので NUL と区別できない。そこで
							 * 内部では私用領域のコードに置き換えておく。強引
							 * だがファイルビューア用なので、まあいいだろう */

							d1 = osd_fgetc(fp);
							d2 = osd_fgetc(fp);
							if (d1 == EOF || d2 == EOF) {
								c = EOF;
							} else {
								if (charmode16 == 1) {
									c = (d2 << 8) | d1;
									if (c <= 0x7f) {
										d1 = (c + Q8TK_UTF16_DUMMYCODE) & 0xff;
										d2 = (c + Q8TK_UTF16_DUMMYCODE) >> 8;
									}
								} else {
									c = (d1 << 8) | d2;
									if (c <= 0x7f) {
										d1 = (c + Q8TK_UTF16_DUMMYCODE) >> 8;
										d2 = (c + Q8TK_UTF16_DUMMYCODE) & 0xff;
									}
								}
							}
						}

						/* 直前の文字が \r で今回が \n ならスキップする */
						if (leading_CR) {
							leading_CR = FALSE;
							if (c == '\n') {
								continue;
							}
						}

						switch (c) {
						case EOF:
							end_of_line = TRUE;
							end_of_file = TRUE;
							break;

						case '\r':
							/* \r\n と続いた場合に備えて覚えておく */
							leading_CR = TRUE;
							/* FALLTHROUGH */
						case '\n':
							end_of_line = TRUE;
							break;

						case '\t':
							/* タブは複数個の空白に置換 */
							if (column < VIEW_MAX_COLUMN) {
								int len = q8gr_strlen(file_code, src);
								int pad = ((len + 8) / 8) * 8 - len;

								while (pad && (column < VIEW_MAX_COLUMN)) {
									if (charmode16 == 0) {
										src[ column++ ] = ' ';
									} else {
										src[ column++ ] = t1;
										src[ column++ ] = t2;
									}
									pad--;
								}
							}
							break;

						default:
							if (column < VIEW_MAX_COLUMN) {
								if (charmode16 == 0) {
									src[ column++ ] = c;
								} else {
									src[ column++ ] = d1;
									src[ column++ ] = d2;
								}
							}
							break;
						}
						/* 行末まで繰り返す */
					} while (end_of_line == FALSE);

				} else {
					/* バイナリファイル */

					int i, cnt;
					unsigned char hex[16];
					char fmt[128];

					/* 16文字リード */
					for (cnt = 0; cnt < sizeof(hex); cnt++) {
						c = osd_fgetc(fp);
						if (c == EOF) {
							break;
						}
						hex[cnt] = c;
					}

					if (cnt == 0) {
						strcpy(src, "[EOF]");
						end_of_file = TRUE;
					} else {
						sprintf(src, "%04X : ", line * 0x10);
						for (i = 0; i < sizeof(hex); i++) {
							if (i < cnt) {
								sprintf(fmt, "%02X ", hex[i]);
							} else {
								sprintf(fmt, "   ");
							}
							strcat(src, fmt);
						}
						strcat(src, "   ");
						for (i = 0; i < sizeof(hex); i++) {
							if (i < cnt && hex[i] != 0) {
								sprintf(fmt, "%c", hex[i]);
							} else {
								sprintf(fmt, " ");
							}
							strcat(src, fmt);
						}
					}
				}

				/* 80行を超えたら固定メッセージを出す (それ以上は表示しない) */
				if (line == COUNTOF(view_string) - 1) {
					strcpy(src, "........To be continued.");
				}

				/* 1行の内容をラベルとして配置 */
				view_string[ line ] = q8tk_label_new(src);
				q8tk_widget_show(view_string[ line ]);
				q8tk_box_pack_start(z, view_string[ line ]);

				line ++;
				/* ファイル終端 or 80行まで繰り返す */
			} while ((end_of_file == FALSE) && (line < COUNTOF(view_string)));

			view_string_cnt = line;
		}
		q8tk_set_kanjicode(save_code);
	}

	osd_fclose(fp);


	q8tk_widget_show(w);
	q8tk_grab_add(w);

	q8tk_widget_set_focus(b);


	/* ダイアログを閉じたときに備えてウィジットを覚えておきます */
	view_widget_win   = w;
	view_widget_vbox  = x;
	view_widget_swin  = swin;
	view_widget_board = z;
	view_widget_quit  = b;

	return 1;
}

static void q8tk_utility_view_end(UNUSED_WIDGET, UNUSED_PARM)
{
	int i;
	for (i = 0; i < view_string_cnt; i++) {
		q8tk_widget_destroy(view_string[ i ]);
	}

	q8tk_widget_destroy(view_widget_quit);
	q8tk_widget_destroy(view_widget_board);
	q8tk_widget_destroy(view_widget_swin);
	q8tk_widget_destroy(view_widget_vbox);

	q8tk_grab_remove(view_widget_win);
	q8tk_widget_destroy(view_widget_win);
	q8tk_widget_destroy(view_widget_accel);
}
