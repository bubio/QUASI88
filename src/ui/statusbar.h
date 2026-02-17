#ifndef STATUSBAR_H_INCLUDED
#define STATUSBAR_H_INCLUDED


extern int status_imagename;				/* イメージ名表示有無 */


#define STATUS_INFO			(3  * 1000)		/* 標準の表示時間 約3秒 */
#define STATUS_WARN			(10 * 1000)		/* 警告の表示時間 約10秒 */


void statusbar_init(void);
void statusbar_update(void);
void statusbar_message(int timeout, const char *message_ascii, const char *message_japanese);
void statusbar_message_cyclic(int timeout, const char *(*get_message)(void));
void statusbar_image_name(void);

void statusbar_set_imagename_show(int show);
int  statusbar_get_imagename_show(void);

#endif /* STATUSBAR_H_INCLUDED */
