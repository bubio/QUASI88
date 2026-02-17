#ifndef Q8TK_COMMON_H_INCLUDED
#define Q8TK_COMMON_H_INCLUDED

/*#define OLD_FOCUS_CHANGE*/


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



/***********************************************************************
 * デバッグ・エラー処理
 ************************************************************************/

#define Q8tkAssert(e,s)													\
	((e) ? (void)0 : _Q8tkAssert(__FILE__, __LINE__, #e, s))

extern void _Q8tkAssert(char *file, int line, char *exp, const char *s);
extern const char *debug_type(int type);


/***********************************************************************
 * ワーク
 ************************************************************************/

extern	int				MAX_WIDGET;
extern	Q8tkWidget		**widget_table;
extern	int				MAX_LIST;
extern	Q8List			**list_table;


/* -------------------------------------------------------------------- */


extern int window_occupy;

/* レイヤ番号 0 は未使用 (ダミー) */
#define MIN_WINDOW_LAYER		(1)		/* USER のレイヤ番号 (これ以上) */
#define MAX_WINDOW_LAYER		(6)		/* USER のレイヤ番号 (これ未満) */
#define TOOL_LAYER_INDEX		(6)		/* TOOL のレイヤ番号 */
#define STAT_LAYER_INDEX		(7)		/* STAT のレイヤ番号 */
#define TOTAL_WINDOW_LAYER		(8)		/* USER/TOOL/STAT 全レイヤ数 */
extern	Q8tkWidget				*window_layer[ TOTAL_WINDOW_LAYER ];
extern	int						window_layer_level;
extern	Q8tkWidget				*focus_widget[ TOTAL_WINDOW_LAYER ];
#define get_focus_widget()		(focus_widget[ window_layer_level ])


extern void widget_bitmap_table_init(void);
extern void widget_bitmap_table_term(void);
extern int  widget_bitmap_table_add(byte *addr);
extern void widget_bitmap_table_remove(int index);



typedef struct {
	int x, y;
	int x_old, y_old;
	int is_touch;
} t_q8tk_mouse;
extern	t_q8tk_mouse q8tk_mouse;


extern	int		q8tk_kanji_code;

extern	int		q8tk_disp_cursor;
extern	int		q8tk_now_shift_on;


extern	int						q8tk_construct_flag;
#define set_construct_flag(f)	q8tk_construct_flag = (f)
#define get_construct_flag()	q8tk_construct_flag


extern	Q8tkWidget				*q8tk_drag_widget;
#define set_drag_widget(w)		q8tk_drag_widget = (w)
#define get_drag_widget()		q8tk_drag_widget




extern void widget_signal_do(Q8tkWidget *widget, const char *name);


extern void widget_map(Q8tkWidget *widget);
extern void widget_construct(void);

extern	void	widget_scrollin_init(void);
extern	void	widget_scrollin_register(Q8tkWidget *widget);
extern	void	widget_scrollin_adjust_reset(void);
extern	void	widget_scrollin_drawn(Q8tkWidget *w);
extern	int		widget_scrollin_adjust(void);

extern	void		widget_focus_list_init(void);
extern	void		widget_focus_list_reset(void);
extern	void		widget_focus_list_append(Q8tkWidget *widget);
extern	Q8tkWidget *widget_focus_list_get_top(void);
extern	Q8tkWidget *widget_focus_list_get_next(Q8tkWidget *widget, int back);

extern	void	widget_destroy_core(Q8tkWidget *widget, int level);


/***********************************************************************
 * 動的ワークの確保／開放
* ***********************************************************************/
extern	Q8tkWidget	*malloc_widget(void);
extern	void		free_widget(Q8tkWidget *w);
extern	Q8List		*malloc_list(void);
extern	void		free_list(Q8List *l);

#if 0
extern	void	q8_strncpy(char *s, const char *ct, size_t n);
extern	void	q8_strncat(char *s, const char *ct, size_t n);
#endif


#endif /* Q8TK_COMMON_H_INCLUDED */
