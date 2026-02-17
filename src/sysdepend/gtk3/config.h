#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


/*----------------------------------------------------------------------
 * GTK3 バージョン固有の定義
 *----------------------------------------------------------------------*/

#include <gtk/gtk.h>


/* GTK3版 QUASI88 のための識別用 */

#ifndef QUASI88_GTK3
#define QUASI88_GTK3
#endif



/* エンディアンネスをチェック */

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
#undef  LSB_FIRST
#define LSB_FIRST
#else
#undef  LSB_FIRST
#endif



/* メニューのタイトル／バージョン表示にて追加で表示する言葉 (任意の文字列) */

#define Q_COMMENT		"GTK3 port"



/* 画面の bpp の定義。GTK3版は 32bpp のみをサポートする */

#undef  SUPPORT_8BPP
#undef  SUPPORT_16BPP
#define SUPPORT_32BPP


#endif /* CONFIG_H_INCLUDED */
