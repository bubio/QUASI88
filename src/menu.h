#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

extern int menu_lang;							/* メニューの言語 */
extern int menu_readonly;						/* ディスク選択ダイアログは
												 * 初期状態は ReadOnly ? */
extern int menu_swapdrv;						/* ドライブの表示順序 */

enum {
	MENU_THEMA_CLASSIC,
	MENU_THEMA_LIGHT,
	MENU_THEMA_DARK,
};
extern int menu_thema;							/* メニューのテーマ */
void menu_set_thema(int thema);
int  menu_get_thema(void);


enum {
	FILENAME_CODING_AUTO = -1,
	FILENAME_CODING_ASCII,
	FILENAME_CODING_EUC,
	FILENAME_CODING_SJIS,
	FILENAME_CODING_UTF8,
	FILENAME_CODING_END
};
extern int filename_coding;						/* ファイル名の漢字コード */
extern int filename_synchronize;				/* ファイル名を同調させる */


/* メニューモード */

enum {
	MENU_MODE_FULLMENU,
	MENU_MODE_PAUSE,
	MENU_MODE_ASKRESET,
	MENU_MODE_ASKSPEEDUP,
	MENU_MODE_ASKDISKCHANGE,
	MENU_MODE_ASKQUIT,
	MENU_MODE
};
void menu_init(void);
void menu_main(void);
void menu_status(void);



/***********************************************************************
 * メニュー画面で表示する、システム固有のメッセージを取得する関数
 *
 * int  menu_about_osd_msg(int        req_japanese,
 *                         int        *result_code,
 *                         const char *message[])
 *
 *      メニュー画面の初期化時に呼び出される。
 *
 *      req_japanese … 真なら、日本語のメッセージ取得を、
 *                      偽なら、英語(ASCII)メッセージ取得を要求する。
 *
 *      result_code  … 日本語のメッセージの場合、漢字コードをセットする。
 *                      EUCの場合      Q8TK_KANJI_EUC
 *                      SJISの場合     Q8TK_KANJI_SJIS
 *                      UTF8の場合     Q8TK_KANJI_UTF8
 *                      指定無しの場合 -1
 *                      英語(ASCII)メッセージなら、不定でよい。
 *
 *      message      … メッセージ文字列へのポインタをセットする。
 *
 *      戻り値       … メッセージがある場合、真を返す。ないなら、偽を返す。
 *                      偽を返す場合、 code, message は不定でよい。
 ************************************************************************/
int menu_about_osd_msg(int req_japanese,
					   int *result_code,
					   const char *message[]);


/*----------------------------------------------------------------------
 * イベント処理の対処
 *----------------------------------------------------------------------*/
void q8tk_event_key_on(int code);
void q8tk_event_key_off(int code);
void q8tk_event_mouse_on(int code);
void q8tk_event_mouse_off(int code);
void q8tk_event_mouse_moved(int x, int y);
void q8tk_event_touch_start(void);
void q8tk_event_touch_stop(void);
void q8tk_event_quit(void);




#endif /* MENU_H_INCLUDED */
