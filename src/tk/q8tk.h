#ifndef Q8TK_H_INCLUDED
#define Q8TK_H_INCLUDED

#include "quasi88.h"
#include "keyboard.h"

/*--------------------------------------------------------------
 * メニュー表示用の画面
 *--------------------------------------------------------------*/

#define	Q8GR_TVRAM_W		(80)
#define	Q8GR_TVRAM_H		(30)

#define	Q8GR_TVRAM_TOOL_X	(0)
#define	Q8GR_TVRAM_TOOL_Y	(0)
#define	Q8GR_TVRAM_TOOL_W	(80)
#define	Q8GR_TVRAM_TOOL_H	(4)

#define	Q8GR_TVRAM_USER_X	(0)
#define	Q8GR_TVRAM_USER_Y	(4)
#define	Q8GR_TVRAM_USER_W	(80)
#define	Q8GR_TVRAM_USER_H	(25)

#define	Q8GR_TVRAM_STAT_X	(0)
#define	Q8GR_TVRAM_STAT_Y	(29)
#define	Q8GR_TVRAM_STAT_W	(80)
#define	Q8GR_TVRAM_STAT_H	(1)


enum {					/* 構造体 T_Q8GR_TVRAM のメンバ font_type の値	*/
	FONT_UNUSED = 0,
	FONT_1_BYTE,
	FONT_ANK	= FONT_1_BYTE,	/* ASCII (Alphabet, Number, Kana etc)	*/
	FONT_QUART,					/* 1/4角文字 (ANK)						*/
	FONT_HALF,					/* 半角文字	 (英数字、片仮名、平仮名)	*/
	FONT_BMP,					/*										*/
	FONT_2_BYTE,
	FONT_KNJ1L	= FONT_2_BYTE,	/* 漢字 第一水準 (左半分)				*/
	FONT_KNJ1R,					/* 漢字 第一水準 (右半分)				*/
	FONT_KNJ2L,					/* 漢字 第二水準 (左半分)				*/
	FONT_KNJ2R,					/* 漢字 第二水準 (右半分)				*/
	FONT_KNJXL,					/* 漢字 ダミー   (左半分)				*/
	FONT_KNJXR,					/* 漢字 ダミー  (右半分)				*/

	/* 最大で16種類(4bit)まで拡張可能 */
};


typedef struct {
	Uint background:	4;		/* 背景パレットコード (0..15)			*/
	Uint foreground:	4;		/* 表示パレットコード (0..15)			*/
	Uint rsv:			1;
	Uint mouse:			1;		/* マウスポインタ       なし=0 あり=1	*/
	Uint reverse:		1;		/* 反転表示             通常=0 反転=1	*/
	Uint underline:		1;		/* アンダーライン       なし=0 あり=1	*/
	Uint font_type:		4;		/* フォントタイプ (下参照)				*/
	Uint addr:			16;		/* 漢字ROM アドレス						*/
} T_Q8GR_TVRAM;	/* 計32bit == unsigned int と決めつけ (;_;) */


extern	int				q8gr_tvram_flip;
extern	T_Q8GR_TVRAM	q8gr_tvram[2][ Q8GR_TVRAM_H ][ Q8GR_TVRAM_W ];


#define			BITMAP_DATA_MAX_OF_BIT	(9)
#define			BITMAP_NUMS_MAX_OF_BIT	(7)
#define			BITMAP_DATA_MAX_SIZE	(1 << BITMAP_DATA_MAX_OF_BIT)
#define			BITMAP_TABLE_SIZE		(1 << BITMAP_NUMS_MAX_OF_BIT)
extern	byte	*bitmap_table[ BITMAP_TABLE_SIZE ];

/*--------------------------------------------------------------
 * メニュー画面のパレットコード
 *--------------------------------------------------------------*/

#define Q8GR_PALETTE_BLACK			(0)
#define Q8GR_PALETTE_WHITE			(1)
#define Q8GR_PALETTE_RED			(2)
#define Q8GR_PALETTE_GREEN			(3)
#define Q8GR_PALETTE_PRIMARY_END	(4)

#define Q8GR_PALETTE_FOREGROUND		(0 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_BACKGROUND		(1 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_LIGHT			(2 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_SHADOW			(3 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_FONT_FG		(4 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_FONT_BG		(5 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_SCALE_SLD		(6 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_SCALE_BAR		(7 + Q8GR_PALETTE_PRIMARY_END)
#define Q8GR_PALETTE_ELEMENT_END	(8 + Q8GR_PALETTE_PRIMARY_END)

#define Q8GR_PALETTE_SIZE			(Q8GR_PALETTE_ELEMENT_END)


/*--------------------------------------------------------------
 * メニューで使用するキーコード (Q8TK 専用の特殊キー)
 *--------------------------------------------------------------*/

#define Q8TK_KEY_BS				KEY88_BS
#define Q8TK_KEY_DEL			KEY88_DEL
#define Q8TK_KEY_TAB			KEY88_TAB
#define Q8TK_KEY_RET			KEY88_RETURN
#define Q8TK_KEY_ESC			KEY88_ESC
#define Q8TK_KEY_RIGHT			KEY88_RIGHT
#define Q8TK_KEY_LEFT			KEY88_LEFT
#define Q8TK_KEY_UP				KEY88_UP
#define Q8TK_KEY_DOWN			KEY88_DOWN
#define Q8TK_KEY_PAGE_UP		KEY88_ROLLDOWN
#define Q8TK_KEY_PAGE_DOWN		KEY88_ROLLUP
#define Q8TK_KEY_SPACE			KEY88_SPACE
#define Q8TK_KEY_SHIFT			KEY88_SHIFT
#define Q8TK_KEY_HOME			KEY88_HOME
#define Q8TK_KEY_END			KEY88_HELP

#define Q8TK_KEY_F1				KEY88_F1
#define Q8TK_KEY_F2				KEY88_F2
#define Q8TK_KEY_F3				KEY88_F3
#define Q8TK_KEY_F4				KEY88_F4
#define Q8TK_KEY_F5				KEY88_F5
#define Q8TK_KEY_F6				KEY88_F6
#define Q8TK_KEY_F7				KEY88_F7
#define Q8TK_KEY_F8				KEY88_F8
#define Q8TK_KEY_F9				KEY88_F9
#define Q8TK_KEY_F10			KEY88_F10
#define Q8TK_KEY_MENU			KEY88_SYS_MENU


#define Q8TK_BUTTON_L			KEY88_MOUSE_L
#define Q8TK_BUTTON_R			KEY88_MOUSE_R
#define Q8TK_BUTTON_U			KEY88_MOUSE_WUP
#define Q8TK_BUTTON_D			KEY88_MOUSE_WDN

#define Q8TK_BUTTON_OFF			(0)
#define Q8TK_BUTTON_ON			(1)


/*--------------------------------------------------------------
 * ウィジットの構造体
 *--------------------------------------------------------------*/

/*--------------------------------------
 * アジャストメント
 *--------------------------------------*/

enum {
	ADJUST_CTRL_NONE		= 0,
	ADJUST_CTRL_STEP_DEC	= 1,
	ADJUST_CTRL_STEP_INC	= 2,
	ADJUST_CTRL_PAGE_DEC	= 3,
	ADJUST_CTRL_PAGE_INC	= 4,
	ADJUST_CTRL_SLIDER		= 5,
	ADJUST_CTRL_BIT_MOUSE	= 0x00,
	ADJUST_CTRL_BIT_KEY		= 0x80,
	ADJUST_CTRL_BIT_MASK	= 0x7f
};

typedef	struct	_Q8Adjust		Q8Adjust;
struct	_Q8Adjust {
	int				value;				/* 現在値 */
	int				lower;				/* 最小値 */
	int				upper;				/* 最大値 */
	int				step_increment;		/* 増分(小) */
	int				page_increment;		/*     (大) */
	int				step_speed;			/* スライダー移動速度 */
	int				page_speed;			/* スライダー移動速度 */
	int				do_effect;			/* 真でスライダー移動演出あり */
	int				active;				/* 操作部位 ADJUST_CTRL_XXX */
	int				max_length;			/* バー長さ指定(矢印除)、0で自動 */
	int				x, y;				/* 表示時 座標 */
	int				length;				/*        バー長さ(矢印除) */
	int				pos;				/*        スライダー位置 0..length-1 */
	int				horizontal;			/*        真で水平配置 */
	int				arrow;				/*        真で矢印あり */
	float			scale;				/*        1コマあたりの変化量 */
	int				size;				/*        1コマのサイズ 1 or 2 */

	int				listbox_changed;	/* LISTBOX用 真で変更時例外処理あり */
};

/*--------------------------------------
 * ウィジット共通
 *--------------------------------------*/

typedef	struct	_Q8tkWidget		Q8tkWidget;
typedef	struct	_Q8List			Q8List;

typedef	void (*Q8tkSignalFunc)();

struct	_Q8tkWidget {

	int				occupy;			/* 真でメインメニュー用ウィジット */

	int				type;			/* ウィジットの種類 Q8TK_TYPE_XXX */
	int				attr;			/* コンテナ属性     Q8TK_ATTR_XXX */
	int				visible;		/* 真で表示する(可視) */
	int				sensitive;		/* 真で操作可能 */

	int				placement_x;	/* 配置 Q8TK_PLACEMENT_XXX */
	int				placement_y;

	int				x, y, sx, sy;	/* 表示座標、表示サイズ */

	Q8tkWidget		*parent;		/* ウィジット連結構造 */
	Q8tkWidget		*child;			/* (表示の時にこのリンクをたどる) */
	Q8tkWidget		*prev;
	Q8tkWidget		*next;

	char			key_up_used;	/* 真でカーソルキー操作予約済み */
	char			key_down_used;
	char			key_left_used;
	char			key_right_used;

	char			*name;			/* mallocされた領域のへポインタ
									 * ・ラベルの文字列
									 * ・フレームの文字列
									 * ・ノートページの文字列
									 * ・エントリの文字列
									 * ・リストアイテムの情報(文字列) */

	int				code;			/* name の文字コード Q8TK_KANJI_XXX */

	int				with_label;		/* 真でラベル自動生成 XXX_new_with_label */


	union {							/* ウィジット別ワーク */

		struct {						/* ウインドウ */
			int			no_frame;			/* 真で枠なし */
			int			shadow_type;		/* 枠のタイプ Q8TK_SHADOW_XXX */
			int			set_position;		/* 真で表示位置を固定する */
			int			x, y;				/* その時の表示位置 */
			int			type;				/* 種類 Q8TK_WINDOW_XXX */
			Q8tkWidget	*work;				/* 関連するウィジット */
			Q8tkWidget	*accel;				/* アクセラレータキー */
		} window;

		struct {						/* フレーム */
			int			shadow_type;		/* 枠のタイプ Q8TK_SHADOW_XXX */
		} frame;

		struct {						/* ラベル */
			int			foreground;			/* 前景色 Q8GR_PALETTE_XXX */
			int			background;			/* 背景色 Q8GR_PALETTE_XXX */
			int			reverse;			/* 真で反転表示 */
			int			width;				/* LIST ITEM の 子LABEL用
											 *      サイズ計算時の最大文字幅
											 * ITEM_LABEL用 表示固定文字幅 */
			size_t		name_size;			/* ITEM_LABEL用 mallocしたサイズ */
			int			is_overlay;			/* ITEM_LABEL用 真で一時表示中 */
			Q8tkWidget	*overlay;			/* ITEM_LABEL用 そのウィジット */
			int			timeout;			/* ITEM_LABEL用 表示時間/周期[ms]*/
			const char *(*get_label)(void);	/* ITEM_LABEL用 表示文字列取得 */
		} label;

		struct {						/* ビットマップ */
			int			foreground;			/* 前景色 Q8GR_PALETTE_XXX */
			int			background;			/* 背景色 Q8GR_PALETTE_XXX */
			int			x_size;				/* 画像幅 pixel単位 */
			int			y_size;				/* 画像高 pixel単位 */
			int			width;				/* 表示幅 文字単位 */
			int			height;				/* 表示高 文字単位 */
			int			index;				/* bitmap_table[] のインデックス */
			int			alloc;				/* 〃                            */
		} bmp;

		struct {						/* 各種ボタン */
			int			active;				/* 押下状態 Q8TK_BUTTON_OFF/ON */
			Q8List		*list;				/* ラジオボタンのリスト */
			int			from_bitmap;		/* 真で画像ボタン */
			int			timer;				/* 演出用タイマー */
			Q8tkWidget	*release;			/* 画像ボタン用 解放画像 */
			Q8tkWidget	*press;				/* 画像ボタン用 押下画像 */
			Q8tkWidget	*inactive;			/* 画像ボタン用 無効画像 */
		} button;

		struct {						/* ノートブック・ノートページ */
			Q8tkWidget	*page;				/* 現在選択中の PAGE */
			struct notebook_draw {			/* 描画時のワーク */
				int		drawing;				/* 真でNOTEBOOK表示 */
				int		x, y;					/* NOTEBOOKの表示座標 */
				int		x0, x1;					/* NOTEPAGEの表示座標 */
				int		selected;				/* 真で表示(選択)中NOTEPAGE */
			} draw;
			int			lost_focus;			/* 真でNOTEBOOKフォーカスをクリア*/
		} notebook;

		struct {						/* コンボ */
			Q8tkWidget	*entry;					/* 配下にあるエントリ */
			Q8List		*list;					/* LIST ITEM のリスト */
			int			nr_items;				/* LIST ITEM の数 */
			int			length;					/* LIST ITEM 最大文字長 */
			int			width;					/* 表示バイト数 */
			Q8tkWidget	*popup_window;			/* POPUPのウインドウ */
			Q8tkWidget	*popup_scrolled_window; /* POPUPの    〃     */
			Q8tkWidget	*popup_list;			/* POPUPのリスト */
			Q8tkWidget	*popup_accel_group;		/* POPUPのESCキー */
			Q8tkWidget	*popup_fake;			/* POPUPのダミーキー */
		} combo;

		struct {						/* リストボックス */
			Q8tkWidget	*selected;			/* 選択済の LIST ITEM */
			Q8tkWidget	*active;			/* 操作中の LIST ITEM */
			Q8tkWidget	*insight;			/* 画面表示対象の LIST ITEM */
			int			width;				/* 表示幅 */
			int			scrollin_top;		/* 表示補正する行数 */
			int			scrollin_left;		/* 表示補正する桁数 */
			char		input[16];			/* 選択入力文字のバッファ */
			int			big;				/* 巨大サイズ表示する場合、真 */
		} listbox;		/* SELECTION TYPE は BROWSE のみ */

		struct {						/* リストアイテム */
			int			fsel_type;			/* ファイルセレクション用情報 */
			int			dir;				/* ドラッグ時のカーソル移動方向 */
			int			cnt;				/* ドラッグ時の処理回数カウンタ */
			int			slide_y;			/* スライド処理時のy座標 */
			int			slide_stat;			/* スライド処理 開始:1- 不可:0 */
		} listitem;

		Q8Adjust		adj;			/* アジャストメント */

		struct {						/* 水平・垂直スケール */
			Q8tkWidget	*adj;				/* アジャストメント */
			int			draw_value;			/* 真で数値を表示する */
			int			value_pos;			/* 表示位置 Q8TK_POS_XXX */
		} scale;

		struct {						/* スクロールドウインドウ */
			int			no_frame;			/* 真で枠なし */
			Q8tkWidget	*hadj;				/* 水平アジャストメント */
			Q8tkWidget	*vadj;				/* 垂直アジャストメント */
			int			hpolicy;			/* 水平バー属性 Q8TK_POLICY_XXX */
			int			vpolicy;			/* 垂直バー属性 Q8TK_POLICY_XXX */
			int			width;				/* 表示幅 */
			int			height;				/* 表示高 */
			int			hscrollbar;			/* 真で水平スクロールバーを表示 */
			int			vscrollbar;			/* 真で垂直スクロールバーを表示 */
			int			child_x0, child_y0;	/* 子ウィジット表示位置 */
			int			child_sx, child_sy;	/* 子ウィジット表示サイズ */
			int			vadj_value;			/* LISTBOX用 垂直ADJの現在値 */
		} scrolled;

		struct {						/* エントリー */
			int			max_length;			/* 入力可能上限 0で無限 */
			int			malloc_length;		/* mallocしたサイズ */
			int			cursor_pos;			/* カーソルバイト位置 */
			int			disp_pos;			/* 表示開始バイト位置 */
			int			width;				/* 表示幅 */
			int			editable;			/* 真でエントリ編集可能 */
			Q8tkWidget	*combo;				/* 上位のコンボボックス */
		} entry;

		struct {						/* アクセラレーターキー */
			Q8tkWidget	*widget;			/* キーに関連づけるウィジット */
			int			key;				/* キー Q8TK_KEY_XXX */
		} accel;

		struct {						/* ダイアログ */
			Q8tkWidget	*content_area;		/* 文言エリア VBOX */
			Q8tkWidget	*action_area;		/* ボタンエリア HBOX */
			Q8tkWidget	*frame;				/* 枠 */
			Q8tkWidget	*box;				/* 見栄え用のエリア */
			Q8tkWidget	*separator;			/* 二つのエリアのセパレータ */
		} dialog;

		struct {						/* ファイルセレクション */
			char		*pathname;			/* パス名バッファ */
			char		*filename;			/* ファイル名バッファ */
			Q8tkWidget	*scrolled_window;	/* ↓のスクロールドウインドウ */
			Q8tkWidget	*file_list;			/* ファイル名のリストボックス */
			Q8tkWidget	*selection_entry;	/* ファイル名入力のエントリ */
			Q8tkWidget	*ok_button;			/* OKのボタン */
			Q8tkWidget	*cancel_button;		/* Cancelのボタン */
			Q8tkWidget	*eject_button;		/* Ejectのボタン (簡易版のみ) */
			Q8tkWidget	*dir_name;			/* ディレクトリ名のラベル */
			Q8tkWidget	*nr_files;			/* ファイル数のラベル */
			Q8tkWidget	*ro_button;			/* ReadOnlyのチェックボタン */
			Q8tkWidget	*drv_button[2];		/* Drive N:のチェックボタン */
			int			selection_changed;	/* 真で別のLIST ITEMを選択した */
			int			width;				/* 表示幅 */
		} fselect;

		struct {						/* アイテムバー */
			int			used_size;			/* バーの表示に必要な幅 */
			int			max_width;			/* バーの最大表示幅 */
			int			max_height;			/* バーの最大表示高 */
		} itembar;

		struct {						/* セパレータ */
			int			style;				/* 種類 Q8TK_SEPARATOR_XXX */
		} separator;

	} stat;

	/* イベント処理関数 */

	void (*event_button_on)(Q8tkWidget *);
	void (*event_key_on)(Q8tkWidget *, int);
	void (*event_dragging)(Q8tkWidget *);
	void (*event_drag_off)(Q8tkWidget *);

	/* イベント処理ユーザ関数 */

	void (*user_event_0)(Q8tkWidget *, void *);
	void *user_event_0_parm;
	void (*user_event_1)(Q8tkWidget *, void *);
	void *user_event_1_parm;

};

enum {							/* (Q8tkWidget*)->type		*/
	Q8TK_TYPE_WINDOW,			/* ウインドウ				*/
	Q8TK_TYPE_BUTTON,			/* ボタン					*/
	Q8TK_TYPE_TOGGLE_BUTTON,	/* トグルボタン				*/
	Q8TK_TYPE_CHECK_BUTTON,		/* チェックボタン			*/
	Q8TK_TYPE_RADIO_BUTTON,		/* ラジオボタン				*/
	Q8TK_TYPE_RADIOPUSH_BUTTON,	/* ラジオボタン(プッシュ型)	*/
	Q8TK_TYPE_SWITCH,			/* スイッチ					*/
	Q8TK_TYPE_FRAME,			/* フレーム					*/
	Q8TK_TYPE_LABEL,			/* ラベル					*/
	Q8TK_TYPE_BMP,				/*							*/
	Q8TK_TYPE_ITEM_LABEL,		/* ラベル					*/
	Q8TK_TYPE_ITEM_BMP,			/*							*/
	Q8TK_TYPE_NOTEBOOK,			/* ノートブック				*/
	Q8TK_TYPE_NOTEPAGE,			/* ノートブックのページ		*/
	Q8TK_TYPE_VBOX,				/* 縦ボックス				*/
	Q8TK_TYPE_HBOX,				/* 横ボックス				*/
	Q8TK_TYPE_ITEMBAR,			/*							*/
	Q8TK_TYPE_VSEPARATOR,		/* 縦区切り線				*/
	Q8TK_TYPE_HSEPARATOR,		/* 横区切り線				*/
	Q8TK_TYPE_COMBO,			/* コンボボックス			*/
	Q8TK_TYPE_LISTBOX,			/* リスト					*/
	Q8TK_TYPE_LIST_ITEM,		/* リストアイテム			*/
	Q8TK_TYPE_ADJUSTMENT,		/* アジャストメント			*/
	Q8TK_TYPE_HSCALE,			/* 横スケール				*/
	Q8TK_TYPE_VSCALE,			/* 縦スケール				*/
	Q8TK_TYPE_SCROLLED_WINDOW,	/* スクロールウインドウ		*/
	Q8TK_TYPE_ENTRY,			/* エントリー				*/

	Q8TK_TYPE_ACCEL_GROUP,		/* アクセラレータキー		*/
	Q8TK_TYPE_ACCEL_KEY,		/* 〃						*/

	Q8TK_TYPE_DIALOG,			/* ダイアログ				*/
	Q8TK_TYPE_FILE_SELECTION,	/* ファイルセレクション		*/

	Q8TK_TYPE_END
};
enum {										/* (Q8tkWidget*)->attr	*/
	Q8TK_ATTR_CONTAINER		  = (1 << 0),	/* コンテナ				*/
	Q8TK_ATTR_LABEL_CONTAINER = (1 << 1),	/* コンテナ(LABEL専用)	*/
	Q8TK_ATTR_MENU_CONTAINER  = (1 << 2),	/* コンテナ(MENU専用)	*/
	Q8TK_ATTR_END
};
enum {							/* (Q8tkWidget*)->placement_x	*/
	Q8TK_PLACEMENT_X_LEFT,
	Q8TK_PLACEMENT_X_CENTER,
	Q8TK_PLACEMENT_X_RIGHT,
	Q8TK_PLACEMENT_X_END
};
enum {							/* (Q8tkWidget*)->placement_y	*/
	Q8TK_PLACEMENT_Y_TOP,
	Q8TK_PLACEMENT_Y_CENTER,
	Q8TK_PLACEMENT_Y_BOTTOM,
	Q8TK_PLACEMENT_Y_END
};


enum {							/* window_new() の引数	*/
	Q8TK_WINDOW_TOPLEVEL,
	Q8TK_WINDOW_DIALOG,
	Q8TK_WINDOW_POPUP,
	Q8TK_WINDOW_TOOL,
	Q8TK_WINDOW_STAT,
	Q8TK_WINDOW_END
};
enum {							/* フレームのタイプ */
	Q8TK_SHADOW_NONE,
	Q8TK_SHADOW_IN,
	Q8TK_SHADOW_OUT,
	Q8TK_SHADOW_ETCHED_IN,
	Q8TK_SHADOW_ETCHED_OUT,
	Q8TK_SHADOW_END
};
enum {							/* セパレータのタイプ */
	Q8TK_SEPARATOR_NORMAL,
	Q8TK_SEPARATOR_WEAK,
	Q8TK_SEPARATOR_STRONG,
	Q8TK_SEPARATOR_END
};
enum {							/* スクロールウインドウの属性 */
	Q8TK_POLICY_ALWAYS,
	Q8TK_POLICY_AUTOMATIC,
	Q8TK_POLICY_NEVER,
	Q8TK_POLICY_END
};


enum {							/* 汎用位置指定 */
	Q8TK_POS_LEFT,
	Q8TK_POS_RIGHT,
	Q8TK_POS_TOP,
	Q8TK_POS_BOTTOM,
	Q8TK_POS_END
};


enum {							/* 表示可能な漢字コード */
	Q8TK_KANJI_ANK,
	Q8TK_KANJI_EUC,
	Q8TK_KANJI_SJIS,
	Q8TK_KANJI_UTF8,
	Q8TK_KANJI_UTF16LE_DUMMY,	/* 内部使用のみ */
	Q8TK_KANJI_UTF16BE_DUMMY,	/* 内部使用のみ*/
	Q8TK_KANJI_END
};

/* UTF16を無理やり表示させるために、内部で使うダミーの文字コード
 *
 * ここで定義した文字コードから128文字分を使う
 * (U+E000 - U+F8FF は私用領域なので、このエリアに割り当てる。
 * ちなみにこのエリアはいろんなメーカーが好き勝手に使っているらしい。
 * それらの文字と衝突するかもだけど、気にしないことにしよう…) */
#define Q8TK_UTF16_DUMMYCODE		(0xf000)


/*--------------------------------------------------------------
 * リスト構造
 *--------------------------------------------------------------*/
struct _Q8List {
	int			occupy;
	void		*data;
	Q8List		*prev;
	Q8List		*next;
};

Q8List	*q8_list_append(Q8List *list, void *data);
Q8List	*q8_list_insert(Q8List *list, void *data, int position);
void	q8_list_remove(Q8List *list, void *data);
void	q8_list_free(Q8List *list);
Q8List	*q8_list_first(Q8List *list);
Q8List	*q8_list_last(Q8List *list);
Q8List	*q8_list_find(Q8List *list, void *data);



/*--------------------------------------------------------------
 * APIなど
 *--------------------------------------------------------------*/

#define Q8TKMAX(a, b)			((a)>(b)?(a):(b))


int		q8tk_set_kanjicode(int code);
void	q8tk_set_cursor(int enable);

void	q8tk_init(void);
void	q8tk_exit(void);
int		q8tk_one_frame(void);
void	q8tk_main_start(void);
void	q8tk_main_stop(void);
void	q8tk_main_quit(void);



void	q8tk_grab_add(Q8tkWidget *widget);
void	q8tk_grab_remove(Q8tkWidget *widget);

Q8tkWidget		*q8tk_window_new(int window_type);
void			q8tk_window_set_shadow_type(Q8tkWidget *widget,
											int shadow_type);


/* TOGGLE/CHECK/RADIO BUTTON の active を見るには、必ず下のマクロを通す */
/* 例）                                                                 */
/*     Q8tkWidget *toggle = q8tk_tobble_button_new();                   */
/*     if (Q8TK_TOBBLE_BUTTON(toggle)->active) {                        */
/*        :                                                             */
/*        :                                                             */
/*     }                                                                */

#define Q8TK_TOGGLE_BUTTON(w)	(&((w)->stat.button))

Q8tkWidget		*q8tk_button_new(void);
Q8tkWidget		*q8tk_button_new_with_label(const char *label);
Q8tkWidget		*q8tk_button_new_from_bitmap(Q8tkWidget *bmp_release,
											 Q8tkWidget *bmp_press,
											 Q8tkWidget *bmp_inactive);
void			q8tk_button_set_bitmap(Q8tkWidget *button,
									   Q8tkWidget *bmp_release,
									   Q8tkWidget *bmp_press,
									   Q8tkWidget *bmp_inactive);

Q8tkWidget		*q8tk_toggle_button_new(void);
Q8tkWidget		*q8tk_toggle_button_new_with_label(const char *label);
Q8tkWidget		*q8tk_toggle_button_new_from_bitmap(Q8tkWidget *bmp_release,
													Q8tkWidget *bmp_press,
													Q8tkWidget *bmp_inactive);
void			q8tk_toggle_button_set_state(Q8tkWidget *widget, int status);
void			q8tk_toggle_button_set_bitmap(Q8tkWidget *button,
											  Q8tkWidget *bmp_release,
											  Q8tkWidget *bmp_press,
											  Q8tkWidget *bmp_inactive);

Q8tkWidget		*q8tk_switch_new(void);

Q8tkWidget		*q8tk_check_button_new(void);
Q8tkWidget		*q8tk_check_button_new_with_label(const char *label);

Q8tkWidget		*q8tk_radio_button_new(Q8tkWidget *group);
Q8tkWidget		*q8tk_radio_button_new_with_label(Q8tkWidget *group,
												  const char *label);
Q8List			*q8tk_radio_button_get_list(Q8tkWidget *group);

Q8tkWidget		*q8tk_radio_push_button_new(Q8tkWidget *group);
Q8tkWidget		*q8tk_radio_push_button_new_with_label(Q8tkWidget *group,
												  const char *label);

Q8tkWidget		*q8tk_combo_new(void);
void			q8tk_combo_append_popdown_strings(Q8tkWidget *combo,
												  const char *entry_str,
												  const char *disp_str);
const	char	*q8tk_combo_get_text(Q8tkWidget *combo);
void			q8tk_combo_set_text(Q8tkWidget *combo, const char *text);
void			q8tk_combo_set_editable(Q8tkWidget *combo, int editable);

Q8tkWidget		*q8tk_listbox_new(void);
void			q8tk_listbox_clear_items(Q8tkWidget *wlist,
										 int start, int end);
Q8tkWidget		*q8tk_listbox_search_items(Q8tkWidget *wlist, const char *s);
void			q8tk_listbox_select_item(Q8tkWidget *wlist, int item);
void			q8tk_listbox_select_child(Q8tkWidget *wlist,
										  Q8tkWidget *child);
void			q8tk_listbox_set_placement(Q8tkWidget *widget,
										   int top_pos, int left_pos);
void			q8tk_listbox_set_big(Q8tkWidget *widget, int big);

Q8tkWidget		*q8tk_list_item_new(void);
Q8tkWidget		*q8tk_list_item_new_with_label(const char *label, int max_width);
void			q8tk_list_item_set_string(Q8tkWidget *w, const char *str);

Q8tkWidget		*q8tk_label_new(const char *label);
void			q8tk_label_set(Q8tkWidget *w, const char *label);
void			q8tk_label_set_reverse(Q8tkWidget *w, int reverse);
void			q8tk_label_set_color(Q8tkWidget *w,
									 int foreground, int background);

Q8tkWidget		*q8tk_bitmap_new(int x_size, int y_size,
								 const unsigned char *bmp);
void			q8tk_bitmap_set_color(Q8tkWidget *w,
									  int foreground, int background);

Q8tkWidget		*q8tk_item_label_new(int width);
void			q8tk_item_label_set(Q8tkWidget *item, const char *label);
void			q8tk_item_label_set_cyclic(Q8tkWidget *item,
										   const char *(*get_label)(void),
										   int timeout);
void			q8tk_item_label_show_message(Q8tkWidget *item,
											 const char *message, int timeout);
void			q8tk_item_label_hide_message(Q8tkWidget *item);
void			q8tk_item_label_set_width(Q8tkWidget *item, int width);

Q8tkWidget		*q8tk_item_bitmap_new(int x_size, int y_size);
void			q8tk_item_bitmap_set_bitmap(Q8tkWidget *item,
											Q8tkWidget *bitmap);
void			q8tk_item_bitmap_clear(Q8tkWidget *item);
void			q8tk_item_bitmap_set_color(Q8tkWidget *item,
										   int foreground, int background);

Q8tkWidget		*q8tk_frame_new(const char *label);
void			q8tk_frame_set_shadow_type(Q8tkWidget *frame, int shadow_type);

Q8tkWidget		*q8tk_hbox_new(void);

Q8tkWidget		*q8tk_vbox_new(void);

Q8tkWidget		*q8tk_itembar_new(int width, int height);
void			q8tk_itembar_add(Q8tkWidget *bar, Q8tkWidget *item);
Q8tkWidget		*q8tk_statusbar_new(void);
void			q8tk_statusbar_add(Q8tkWidget *bar, Q8tkWidget *item);
Q8tkWidget		*q8tk_toolbar_new(void);
void			q8tk_toolbar_add(Q8tkWidget *bar, Q8tkWidget *item);

Q8tkWidget		*q8tk_notebook_new(void);
void			q8tk_notebook_append(Q8tkWidget *notebook,
									 Q8tkWidget *widget, const char *label);
int				q8tk_notebook_current_page(Q8tkWidget *notebook);
void			q8tk_notebook_set_page(Q8tkWidget *notebook, int page_num);
void			q8tk_notebook_next_page(Q8tkWidget *notebook);
void			q8tk_notebook_prev_page(Q8tkWidget *notebook);
void			q8tk_notebook_hook_focus_lost(Q8tkWidget *notebook,
											  int focus_lost);

Q8tkWidget		*q8tk_vseparator_new(void);
void			q8tk_vseparator_set_style(Q8tkWidget *widget, int style);

Q8tkWidget		*q8tk_hseparator_new(void);
void			q8tk_hseparator_set_style(Q8tkWidget *widget, int style);


/* ADJUSTMENT の value などを見るには、必ず下のマクロを通す             */
/* 例）                                                                 */
/*     Q8tkWidget *adj = q8tk_adjustment_new();                         */
/*     val = Q8TK_ADJUSTMENT(adj)->value;                               */

#define Q8TK_ADJUSTMENT(w)		(&((w)->stat.adj))
Q8tkWidget		*q8tk_adjustment_new(int value, int lower, int upper,
									 int step_increment, int page_increment);
void			q8tk_adjustment_clamp_page(Q8tkWidget *adj,
										   int lower, int upper);
void			q8tk_adjustment_set_value(Q8tkWidget *adj, int value);
void			q8tk_adjustment_set_arrow(Q8tkWidget *adj, int arrow);
void			q8tk_adjustment_set_length(Q8tkWidget *adj, int length);
void			q8tk_adjustment_set_increment(Q8tkWidget *adj,
											  int step_increment,
											  int page_increment);
void			q8tk_adjustment_set_speed(Q8tkWidget *adj,
										  int step_speed, int page_speed);
void			q8tk_adjustment_set_size(Q8tkWidget *adj, int size);

Q8tkWidget		*q8tk_hscale_new(Q8tkWidget *adjustment);
Q8tkWidget		*q8tk_vscale_new(Q8tkWidget *adjustment);
void			q8tk_scale_set_value_pos(Q8tkWidget *scale, int pos);
void			q8tk_scale_set_draw_value(Q8tkWidget *scale, int draw_value);

Q8tkWidget		*q8tk_scrolled_window_new(Q8tkWidget *hadjustment,
										  Q8tkWidget *vadjustment);
void			q8tk_scrolled_window_set_policy(Q8tkWidget *scrolledw,
												int hscrollbar_policy,
												int vscrollbar_policy);
void			q8tk_scrolled_window_set_frame(Q8tkWidget *w, int with_frame);
void			q8tk_scrolled_window_set_adj_size(Q8tkWidget *w, int size);

Q8tkWidget		*q8tk_entry_new(void);
Q8tkWidget		*q8tk_entry_new_with_max_length(int max);
const	char	*q8tk_entry_get_text(Q8tkWidget *entry);
void			q8tk_entry_set_text(Q8tkWidget *entry, const char *text);
void			q8tk_entry_set_position(Q8tkWidget *entry, int position);
void			q8tk_entry_set_max_length(Q8tkWidget *entry, int max);
void			q8tk_entry_set_editable(Q8tkWidget *entry, int editable);

Q8tkWidget		*q8tk_accel_group_new(void);
void			q8tk_accel_group_attach(Q8tkWidget *accel_group,
										Q8tkWidget *window);
void			q8tk_accel_group_detach(Q8tkWidget *accel_group,
										Q8tkWidget *window);
void			q8tk_accel_group_add(Q8tkWidget *accel_group, int accel_key,
									 Q8tkWidget *widget, const char *signal);


/* DIALOG の content_area, action_area を見るには、必ず下のマクロを通す */
/* 例）                                                                 */
/*     Q8tkWidget *dialog = q8tk_dialog_new();                          */
/*     q8tk_box_pack_start(Q8TK_DIALOG(dialog)->content_area, button);  */

#define Q8TK_DIALOG(w)			(&((w)->stat.window.work->stat.dialog))
Q8tkWidget		*q8tk_dialog_new(void);
void			q8tk_dialog_set_style(Q8tkWidget *widget,
									  int frame_style, int separator_style);


/* FILE SELECTION の ok_button などを見るには、必ず下のマクロを通す     */
/* 例）                                                                 */
/*     Q8tkWidget *fselect = q8tk_file_selection_new("LOAD", FALSE);    */
/*     q8tk_signal_connect(Q8TK_FILE_SELECTION(fselect)->ok_button,     */
/*                          func, fselect);                             */

#define Q8TK_FILE_SELECTION(w)	(&((w)->stat.window.work->stat.fselect))


/* FILE_SELECTION で扱えるパス込みのファイル名バイト数 (NUL含む)        */

#define Q8TK_MAX_FILENAME		(QUASI88_MAX_FILENAME)
/* #define		Q8TK_MAX_FILENAME		(OSD_MAX_FILENAME) */


/* FILE_SELECTION が通常版か簡易版かを判定するマクロ                    */
#define Q8TK_IS_NORMAL_FILE_SELECTION(w)	\
		(((w)->stat.window.work->stat.fselect.eject_button) == NULL)
#define Q8TK_IS_SIMPLE_FILE_SELECTION(w)	\
		(((w)->stat.window.work->stat.fselect.eject_button) != NULL)


Q8tkWidget		*q8tk_file_selection_new(const char *title, int select_ro);
Q8tkWidget		*q8tk_file_selection_simple_new(void);
const	char	*q8tk_file_selection_get_filename(Q8tkWidget *fselect);
void			q8tk_file_selection_set_filename(Q8tkWidget *fselect,
												 const char *filename);
int				q8tk_file_selection_get_readonly(Q8tkWidget *fselect);
int				q8tk_file_selection_simple_get_drive(Q8tkWidget *fselect);

int				q8tk_utility_view(const char *filename);




void	q8tk_misc_set_placement(Q8tkWidget *widget,
								int placement_x, int placement_y);
void	q8tk_misc_set_size(Q8tkWidget *widget, int width, int height);

void	q8tk_misc_redraw(void);


void	q8tk_container_add(Q8tkWidget *container, Q8tkWidget *widget);
void	q8tk_box_pack_start(Q8tkWidget *box, Q8tkWidget *widget);
void	q8tk_box_pack_end(Q8tkWidget *box, Q8tkWidget *widget);
void	q8tk_container_remove(Q8tkWidget *container, Q8tkWidget *widget);

void	q8tk_widget_show(Q8tkWidget *widget);
void	q8tk_widget_hide(Q8tkWidget *widget);

void	q8tk_widget_set_sensitive(Q8tkWidget *widget, int sensitive);

void	q8tk_widget_destroy(Q8tkWidget *widget);
void	q8tk_widget_destroy_all_related(Q8tkWidget *widget);

void	q8tk_widget_set_focus(Q8tkWidget *widget);


/* ↓ 返り値は、無効 (必ず 0) */
int		q8tk_signal_connect(Q8tkWidget *widget, const char *name,
							Q8tkSignalFunc func, void *func_data);
void	q8tk_signal_handlers_destroy(Q8tkWidget *widget);


void q8tk_timer_add(Q8tkWidget *widget,
					void (*callback)(Q8tkWidget *widget, void *parm),
					void *parm, int timer_ms);
void q8tk_timer_remove(Q8tkWidget *widget);

void q8tk_set_solid(int is_solid);



#endif /* Q8TK_H_INCLUDED */
