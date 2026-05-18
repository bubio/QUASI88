#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "quasi88.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "crtcdmac.h"
#include "memory.h"
#include "graph.h"

#include "basic.h"


#ifdef USE_LOCALE
#include <locale.h>
#include <langinfo.h>
#endif /* USE_LOCALE */

/*==============================================================
 * テキストスクリーン関連                       by peach
 *==============================================================*/
#define PUT_JIS_IN(fp)		fprintf(fp, "%c%c%c", 0x1b, 0x28, 0x49);
#define PUT_JIS_OUT(fp)		fprintf(fp, "%c%c%c", 0x1b, 0x28, 0x42);

enum {
	LANG_EUC,
	LANG_JIS,
	LANG_SJIS
};

static int lang = -1;

static void set_lang(void)
{
	/*char *p;*/

	if (lang >= 0) {
		return;
	}

#ifdef USE_LOCALE
	setlocale(LC_ALL, "");
	p = nl_langinfo(CODESET);
	if (strncmp(p, "JIS_", 4) == 0) {
		lang = LANG_JIS;
	} else if (strncmp(p, "SHIFT_JIS", 9) == 0) {
		lang = LANG_SJIS;
	} else { /* EUC_JP (default)*/
		lang = LANG_EUC;
	}
#else
	lang = LANG_EUC;
#endif
}

void print_hankaku(FILE *fp, Uchar *str, char ach)
{
	Uchar *ptr;

	/* 標準出力じゃないならそのまま */
	if (fp != stdout) {
		fprintf(fp, "%s", str);
		return;
	}

	ptr = str;
	set_lang();

	if (lang == LANG_JIS) {
		PUT_JIS_IN(fp);
	}
	while (*ptr != '\0') {
		if (*ptr == '\n') {
			if (lang == LANG_JIS) {
				PUT_JIS_OUT(fp);
				fputc('\n', fp);
				PUT_JIS_IN(fp);
			} else {
				fputc('\n', fp);
			}
		} else if (0xa1u <= *ptr && *ptr <= 0xdfu) {
			switch (lang) {
			case LANG_EUC:
				fputc(0x8eu, fp);
				fputc(*ptr, fp);
				break;
			case LANG_JIS:
				fputc(*ptr - 0x80u, fp);
				break;
			case LANG_SJIS:
				fputc(*ptr, fp);
				break;
			}
		} else if (isprint(*ptr)) {
			fputc(*ptr, fp);
		} else {
			/* 表示不能 */
			fputc(ach, fp);
			/*fprintf(fp, "0x%x", *ptr);*/
		}
		ptr++;
	}
	if (lang == LANG_JIS) {
		PUT_JIS_OUT(fp);
	}
}


#ifdef USE_MONITOR

/*----------------------------------------------------------------------
 * textscr
 *      テキスト画面をコンソールに表示
 *                              この機能は peach氏により実装されました
 *----------------------------------------------------------------------*/
void mon_help_textscr(int oneline)
{
	if (oneline) {
		printf(
			"print text screen"
			);
	} else {
		printf(
			"  textscr [<char>]\n"
			"    print text screen in the console screen\n"
			"    <char> ... alternative character to unprintable one.\n"
			"               [omit] ... use 'X'\n"
			);
	}
}

static void print_text_screen(void)
{
	/*FILE *fp;*/
	int i, j;
	int line;
	int width;
	int end;
	Uchar text_buf[82];			/* 80文字 + '\n' + '\0' */

	if (grph_ctrl & 0x20) {
		line = 25;
	} else {
		line = 20;
	}

	if (sys_ctrl & 0x01) {
		width = 80;
	} else {
		width = 40;
	}

	for (i = 0, end = 0; i < line; i++) {
		for (j = 0; j < width; j++) {
			if (width == 80) {
				text_buf[j] =
					text_attr_buf[ text_attr_flipflop ][i * 80 + j] >> 8;
			} else {
				text_buf[j] =
					text_attr_buf[ text_attr_flipflop ][i * 80 + j * 2] >> 8;
			}
			if (text_buf[j] == 0) {
				text_buf[j] = ' ';
			} else {
				end = j;
			}
		}
		/* 終端までの空白は入れない */
		text_buf[end + 1] = '\n';
		text_buf[end + 2] = '\0';
		print_hankaku(stdout, text_buf, alt_char);
	}
}

void mon_exec_textscr(void)
{
	/* <char> */
	if (mon_exist_argv()) {
		alt_char = argv.str[0];
		mon_shift();
	}

	print_text_screen();
}

/*--------------------------------------------------------------
 * loadbas <filename> [<type>]
 *      BASIC LIST を読み込む
 *--------------------------------------------------------------*/
void mon_help_loadbas(int oneline)
{
	if (oneline) {
		printf(
			"load basic list"
			);
	} else {
		printf(
			"  loadbas <filename> [<type>]\n"
			"    load basic list\n"
			"    <filename> ... filename of basic list.\n"
			"    <type>     ... set type of basic list ASCII or BINARY\n"
			"                   ASCII  ... load as text list\n"
			"                   BINARY ... load as intermediate code\n"
			"                   [omit] ... select ASCII\n"
			);
	}
}

#if 1									/* experimental by peach */
void mon_exec_loadbas(void)
{
	char *filename;
	int size;
	int type = ARG_ASCII;
	FILE *fp;

	/* <filename> */
	if (! mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	/* <type> */
	if (mon_exist_argv()) {
		if (! mon_argv_is(ARGV_BASIC)) {
			error();
		}
		type = argv.val;
		mon_shift();
	}

	if (mon_exist_argv()) {
		error();
	}

	if ((fp = fopen(filename, "r"))) {
		if (type == ARG_ASCII) {
			size = basic_encode_list(fp);
		} else {
			size = basic_load_intermediate_code(fp);
		}
		fclose(fp);
		if (size > 0) {
			printf("Load [%s] (size %d)\n", filename, size);
		}
	} else {
		printf("file [%s] can't open\n", filename);
	}
}
#else
void mon_exec_loadbas(void)
{
	printf("sorry, not support\n");
}
#endif
/*--------------------------------------------------------------
 * savebas [<filename> [<type>]]
 *      BASIC LIST を出力
 *--------------------------------------------------------------*/
void mon_help_savebas(int oneline)
{
	if (oneline) {
		printf(
			"save basic list"
			);
	} else {
		printf(
			"  savebas [<filename> [<type>]]\n"
			"    print or save basic list\n"
			"    <filename> ... filename of basic list.\n"
			"    <type>     ... set type of basic list ASCII or BINARY\n"
			"                   ASCII  ... save as text list\n"
			"                   BINARY ... save as intermediate code\n"
			"                   [omit] ... select ASCII\n"
			);
	}
}

#if 1									/* experimental by peach */
void mon_exec_savebas(void)
{
	char *filename;
	int size;
	int type = ARG_ASCII;
	FILE *fp;

	/* <filename> */
	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		/* <type> */
		if (mon_exist_argv()) {
			if (! mon_argv_is(ARGV_BASIC)) {
				error();
			}
			type = argv.val;
			mon_shift();
		}
	} else {
		filename = NULL;
	}

	if (mon_exist_argv()) {
		error();
	}

	if (filename == NULL) {
		basic_decode_list(stdout);
	} else if ((fp = fopen(filename, "w"))) {
		if (type == ARG_ASCII) {
			size = basic_decode_list(fp);
		} else {
			size = basic_save_intermediate_code(fp);
		}
		fclose(fp);
		if (size > 0) {
			printf("Save [%s] (size %d)\n", filename, size);
		}
	} else {
		printf("file [%s] can't open\n", filename);
	}
}
#else
void mon_exec_savebas(void)
{
	printf("sorry, not support\n");
}
#endif






/*--------------------------------------------------------------
 * loadfont <filename> <format> <type>
 *      フォントファイルのロード
 *--------------------------------------------------------------*/
void mon_help_loadfont(int oneline)
{
	if (oneline) {
		printf(
			"load text-font file"
			);
	} else {
		printf(
			"  loadfont <filename> <format> <type>\n"
			"    load text-font file\n"
			"    <filename> ... specify text-font-file filename.\n"
			"    <format>   ... select format\n"
			"                   0 ... ROM-image\n"
			"                   1 ... bitmap format (horizontal)\n"
			"                   2 ... bitmap format (vertical)\n"
			"    <type>     ... select font kind\n"
			"                   0 ... PCG font\n"
			"                   1 ... Standard font\n"
			"                   2 ... 2nd font (usually hiragana-font)\n"
			);
	}
}

void mon_exec_loadfont(void)
{
	char *filename = NULL;
	int format = 0;
	int type   = 0;

	/* <filename> */
	if (! mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	/* <format> */
	if (! mon_exist_argv() ||
		! mon_argv_is(ARGV_INT)) {
		error();
	}
	format = argv.val;
	if (! BETWEEN(0, format, 2)) {
		error();
	}
	mon_shift();

	/* <type> */
	if (! mon_exist_argv() ||
		! mon_argv_is(ARGV_INT)) {
		error();
	}
	type = argv.val;
	if (! BETWEEN(0, type, 2)) {
		error();
	}
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	{
		static const int rev[] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
		};
		int c, i, j, k, l, n;
		int result;
		FILE *fp;
		bit8 *font;

		if (type == 1) {
			font = font_mem;
		} else if (type == 2) {
			font = font_mem2;
		} else {
			font = font_pcg;
		}

		fp = fopen(filename, (format == 0) ? "rb" : "r");

		if (fp) {

			if (format == 0) {

				if (fread(font, sizeof(byte), 8 * 256, fp) != 8 * 256) {
					printf("file [%s] read error\n", filename);
				}

			} else {
				/* #define font_width 128 */
				result = fscanf(fp, "#define %*s %d\n", &c);
				if (result != 1 || c != 128) {
					goto ERR;
				}
				/* #define font_height 128 */
				result = fscanf(fp, "#define %*s %d\n", &c);
				if (result != 1 || c != 128) {
					goto ERR;
				}
				/* static unsigned char font_bits[] = { */
				result = fscanf(fp, "%*[^]]%*[^=]%*[^{]{");
				if (result != 0) {
					goto ERR;
				}

				l = 0;
				for (k = 0; k < 16; k++) {
					for (j = 0; j < 8; j++) {
						for (i = 0; i < 16; i++) {

							result = fscanf(fp, "%i", &c); /* 0xnn */
							if (result != 1) {
								goto ERR;
							}

							c = ((rev[ c & 0xf ]) << 4) | (rev[ c >> 4 ]);
							if (format == 1) {
								n = (k * 16 + i) * 8 + j;
							} else {
								n = (i * 16 + k) * 8 + j;
							}
							font[ n ] = c & 0xff;

							if (l < 8 * 256) {
								result = fscanf(fp, "%*[^1234567890]"); /* , */
								if (result != 0) {
									goto ERR;
								}
							}
							l++;
						}
					}
				}
			}

			fclose(fp);
			screen_update_immidiate();

		} else {
			printf("file [%s] can't open\n", filename);
		}

		return;

	ERR:
		printf("file [%s] read error\n", filename);
		fclose(fp);
		screen_update_immidiate();
	}

	return;
}



/*--------------------------------------------------------------
 * savefont <filename> <format> <type>
 *      フォントファイルのセーブ
 *--------------------------------------------------------------*/
void mon_help_savefont(int oneline)
{
	if (oneline) {
		printf(
			"save text-font file"
			);
	} else {
		printf(
			"  savefont <filename> <format> <type>\n"
			"    save text-font file\n"
			"    <filename> ... specify text-font-file filename.\n"
			"    <format>   ... select format\n"
			"                   0 ... ROM-image\n"
			"                   1 ... bitmap format (horizontal)\n"
			"                   2 ... bitmap format (vertical)\n"
			"    <type>     ... select font kind\n"
			"                   0 ... PCG font\n"
			"                   1 ... Standard font\n"
			"                   2 ... 2nd font (usually hiragana-font)\n"
			);
	}
}

void mon_exec_savefont(void)
{
	char *filename = NULL;
	int format = 0;
	int type   = 0;

	/* <filename> */
	if (! mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	/* <format> */
	if (! mon_exist_argv() ||
		! mon_argv_is(ARGV_INT)) {
		error();
	}
	format = argv.val;
	if (! BETWEEN(0, format, 2)) {
		error();
	}
	mon_shift();

	/* <type> */
	if (! mon_exist_argv() ||
		! mon_argv_is(ARGV_INT)) {
		error();
	}
	type = argv.val;
	if (! BETWEEN(0, type, 2)) error();
	mon_shift();

	if (mon_exist_argv()) error();


	{
		static const int rev[] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
		};
		int c, i, j, k, l, n;
		FILE *fp;
		bit8 *font;

		if (type == 1) {
			font = font_mem;
		} else if (type == 2) {
			font = font_mem2;
		} else {
			font = font_pcg;
		}

		fp = fopen(filename, (format == 0) ? "wb" : "w");

		if (fp) {

			if (format == 0) {

				if (fwrite(font, sizeof(byte), 8 * 256, fp) != 8 * 256) {
					printf("file [%s] write error\n", filename);
				}

			} else {

				fprintf(fp, "#define font_width 128\n");
				fprintf(fp, "#define font_height 128\n");
				fprintf(fp, "static unsigned char font_bits[] = {\n");

				l = 0;
				for (k = 0; k < 16; k++) {
					for (j = 0; j < 8; j++) {
						for (i = 0; i < 16; i++) {

							if (format == 1) {
								n = (k * 16 + i) * 8 + j;
							} else {
								n = (i * 16 + k) * 8 + j;
							}
							c = (font[ n ]) & 0xff;
							c = ((rev[ c & 0xf ]) << 4) | (rev[ c >> 4 ]);

							if ((l % 12) == 0) {
								fprintf(fp, "  ");
							}
							fprintf(fp, " 0x%02x", c & 0xff);
							if (l == 8 * 256 - 1) {
								fprintf(fp, "};\n");
							} else if ((l % 12) == 11) {
								fprintf(fp, ",\n");
							} else {
								fprintf(fp, ",");
							}
							l++;
						}
					}
				}

			}

			fclose(fp);

		} else {
			printf("file [%s] can't open\n", filename);
		}
	}

	return;
}

#endif /* USE_MONITOR */

