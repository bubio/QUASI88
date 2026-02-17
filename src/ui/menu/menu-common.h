#ifndef MENU_LOCAL_H_INCLUDED
#define MENU_LOCAL_H_INCLUDED

#ifdef HAVE_INTPTR_T
#include <stdint.h>
#define INT2P(val)		(void *)(intptr_t)(val)
#define P2INT(p)		(int)(intptr_t)(p)
#else
#define INT2P(val)		(void *)(val)
#define P2INT(p)		(int)(p)
#endif


/* コールバック関数の引数 (Q8tkWidget*, void*) が未使用の場合、
 * ワーニングが出て鬱陶しいので、 gcc に頼んで許してもらう。 */
#if defined(__GNUC__)
#define UNUSED_WIDGET	__attribute__((__unused__)) Q8tkWidget *dummy_0
#define UNUSED_PARM		__attribute__((__unused__)) void *dummy_1
#else
#define UNUSED_WIDGET	Q8tkWidget *dummy_0
#define UNUSED_PARM		void *dummy_1
#endif


typedef struct {
	char		*str[2];		/* [0]…ANK文字列  [1]…日本語文字列	*/
	int			val;			/* int値 汎用							*/
} t_menudata;

typedef struct {
	char		*str[2];		/* [0]…ANK文字列  [1]…日本語文字列	*/
} t_menulabel;


/* t_menulabel の index 番目の文字列を取得するマクロ -------------------*/

#define GET_LABEL(l, index)		(l[index].str[menu_lang])


/* キー配列変更コンボボックス用 のウィジット生成用データ */

typedef struct {
	char	*str;				/* キートップの文字 or パディング用空白	*/
	int		code;				/* キーコード       or  0				*/
} t_keymap;

#define SZ_KEYMAP_ASSIGN		(107)
extern const t_keymap keymap_assign[SZ_KEYMAP_ASSIGN];


/*--------------------------------------------------------------
 * ファイル操作エラーダイアログ
 *--------------------------------------------------------------*/

enum {
	ERR_NO,
	ERR_CANT_OPEN,
	ERR_READ_ONLY,
	ERR_MAYBE_BROKEN,
	ERR_SEEK,
	ERR_WRITE,
	ERR_OVERFLOW,
	ERR_UNEXPECTED
};
extern void start_file_error_dialog(int drv, int result);


extern Q8tkWidget *PACK_FRAME(Q8tkWidget *box,
							  const char *label, Q8tkWidget *widget);
extern Q8tkWidget *PACK_HBOX(Q8tkWidget *box);
extern Q8tkWidget *PACK_VBOX(Q8tkWidget *box);
extern Q8tkWidget *PACK_LABEL(Q8tkWidget *box, const char *label);
extern Q8tkWidget *PACK_VSEP(Q8tkWidget *box);
extern Q8tkWidget *PACK_HSEP(Q8tkWidget *box);
extern Q8tkWidget *PACK_BUTTON(Q8tkWidget *box,
							   const char *label,
							   Q8tkSignalFunc callback, void *parm);
extern Q8tkWidget *PACK_CHECK_BUTTON(Q8tkWidget *box,
									 const char *label, int on,
									 Q8tkSignalFunc callback, void *parm);
extern Q8tkWidget *PACK_RADIO_BUTTON(Q8tkWidget *box,
									 Q8tkWidget *button,
									 const char *label, int on,
									 Q8tkSignalFunc callback, void *parm);
extern Q8tkWidget *PACK_COMBO(Q8tkWidget *box,
							  const t_menudata *p, int count,
							  int initval, const char *initstr, int width,
							  Q8tkSignalFunc act_callback, void *act_parm,
							  Q8tkSignalFunc chg_callback, void *chg_parm);
extern Q8tkWidget *PACK_ENTRY(Q8tkWidget *box,
							  int length, int width, const char *text,
							  Q8tkSignalFunc act_callback, void *act_parm,
							  Q8tkSignalFunc chg_callback, void *chg_parm);
extern void PACK_CHECK_BUTTONS(Q8tkWidget *box,
							   const t_menudata *p, int count,
							   int (*f_initval)(int),
							   Q8tkSignalFunc callback);
extern Q8List *PACK_RADIO_BUTTONS(Q8tkWidget *box,
								  const t_menudata *p, int count,
								  int initval, Q8tkSignalFunc callback);
typedef struct {
	char	*str[2];
	int		val;
	int		min;
	int		max;
	int		step;
	int		page;
} t_volume;
extern Q8tkWidget *PACK_HSCALE(Q8tkWidget *box,
							   const t_volume *p,
							   int initval,
							   Q8tkSignalFunc callback, void *parm);
extern Q8tkWidget *MAKE_KEY_COMBO(Q8tkWidget *box,
								  const t_menudata *p,
								  int (*f_initval)(int),
								  Q8tkSignalFunc callback);
extern Q8tkWidget *PACK_KEY_ASSIGN(Q8tkWidget *box,
								   const t_menudata *p, int count,
								   int (*f_initval)(int),
								   Q8tkSignalFunc callback);
extern void START_FILE_SELECTION(const char *label,
								 int select_ro,
								 const char *filename,
								 void (*ok_button)(void),
								 char *get_filename,
								 int  sz_get_filename,
								 int  *get_ro);
extern void dialog_create(void);
extern void dialog_set_title(const char *label);
extern void dialog_set_check_button(const char *label, int on,
									Q8tkSignalFunc callback, void *parm);
extern void dialog_set_separator(void);
extern void dialog_set_button(const char *label,
							  Q8tkSignalFunc callback, void *parm);
extern void dialog_set_entry(const char *text, int max_length,
							 Q8tkSignalFunc callback, void *parm);
extern const char *dialog_get_entry(void);
extern void dialog_accel_key(int key);
extern void dialog_start(void);
extern void dialog_destroy(void);


#endif /* MENU_LOCAL_H_INCLUDED */
