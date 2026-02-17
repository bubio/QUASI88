/***********************************************************************
 * タッチキーボード処理 (システム依存)
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "quasi88.h"
#include "fname.h"
#include "utility.h"
#include "graph.h"
#include "device.h"
#include "event.h"
#include "keyboard.h"

/************************************************************************/

#define	DEBUG_PRINTF	(0)
#define DBGPRINTF(x) if (DEBUG_PRINTF) printf x

static Uint32 key_window_id;
static SDL_Window *key_window;
static SDL_Renderer *key_renderer;
static int key_layout_no;
static int key_show;				/* タッチキーの表示有無 */
int sdl_in_touchkey_area = FALSE;	/* マウス位置がタッチキーエリアなら真 */

int key_width;
int key_height;
int key_nr_finger_clicks;			/* タッチの連続タップ回数 */
/* 指でタッチした時の、連続タップ回数を key_nr_finger_clicks にセットする
 *   指でタッチすると、 SDL_FINGERDOWN イベントが発生するが、
 *   その直前に SDL_MOUSEBUTTONDOWN イベントが発生するので、
 *   そこからクリック回数を取得して覚えておく */


/* タッチキーボード上に存在できるキーの最大数 */
#define	NR_KEYNO		(128)

/* タッチキー押下で、最大で以下の個数のキーを同時に押下したことにできる */
#define	NR_KEYCOMBO		(2)

/* タッチキーボードのロックキーの種類 */
enum {
	LOCK_TYPE_NONE,			/* クリックしてもロックできないキー */
	LOCK_TYPE_SINGLE,		/* シングルクリックでロックできるキー */
	LOCK_TYPE_DOUBLE,		/* ダブルクリックでロックできるキー */
};

/* タッチキーの押下状態。真で押下中 */
static char keydown_state[NR_KEYNO];
static char keydown_state_changed;

/* マウス・タッチ操作状態 */
static enum {
	DRAGGING_NONE,				/* マウス・タッチ操作なし */
	DRAGGING_MENU_TOUCH,		/* MENUをタッチで操作中 */
	DRAGGING_TOUCHKEY_MOUSE,	/* タッチキーをマウスで操作中 */
	DRAGGING_COMBINAITON,		/* EMU/MENUをマウス、タッチキーを指で操作中 */
} dragging_mode = DRAGGING_NONE;


/************************************************************************
 *
 ************************************************************************/
#include "touchkey-define.h"
#include "touchkey-layout.h"
#include "touchkey-image.h"



/* タッチキーボードの種類 */
#define	MAX_TOUCHKEY_LIST	(8)
static  int nr_touchkey_list;

/* タッチキーボードの諸元リスト */
static T_TOUCHKEY_LIST touchkey_list[MAX_TOUCHKEY_LIST];




/************************************************************************
 * タッチキーボード初期化/終了
 ************************************************************************/
static int config_read_touchkeyconf_file(const char *filename);
static int malloc_touchkey_spec(void);
static void free_touchkey_spec(void);
static int malloc_keytop_bmp(void);
static void free_keytop_bmp(void);
static int create_keytop_texture(int no, SDL_Renderer *renderer);
static void destroy_keytop_texture(int no);

int sdl2_touchkey_init(SDL_Renderer *renderer)
{
	/* 全てのタッチキーボードの諸元を変数に格納する */
	const char *touchconf_filename = filename_alloc_touchkey_cfgname();

	if (config_read_touchkeyconf_file(touchconf_filename) == FALSE) {
		if (malloc_touchkey_spec() == FALSE) {
			return FALSE;
		}
	}

	/* 全てのキートップ画像をメモリに保持 */
	if (malloc_keytop_bmp() == FALSE) {
		return FALSE;
	}

	/* 全てのキートップ画像を texture で保持 */
	if (create_keytop_texture(0, renderer) == FALSE) {
		return FALSE;
	}

	/* タッチキー用のウインドウ、レンダラ、テクスチャを生成 */
	if (separate_touchkey) {
		Uint32 flags;

		flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		if ((key_window = SDL_CreateWindow("QUASI88",
										   SDL_WINDOWPOS_UNDEFINED,
										   SDL_WINDOWPOS_UNDEFINED,
										   key_width, key_height,
										   flags)) == NULL) {
			return FALSE;
		}
		key_window_id = SDL_GetWindowID(key_window);

		flags = SDL_RENDERER_ACCELERATED;
		if ((key_renderer = SDL_CreateRenderer(key_window, -1,
											   flags)) == NULL) {

			SDL_DestroyWindow(key_window);
			key_window = NULL;
			return FALSE;
		}

		SDL_SetRenderDrawColor(key_renderer, 0, 0, 0, 255);

		sdl2_set_icon(key_window);

		SDL_RenderClear(key_renderer);
		SDL_RenderPresent(key_renderer);
		key_show = FALSE;


		if (create_keytop_texture(1, key_renderer) == FALSE) {
			return FALSE;
		}
	}

	return TRUE;
}

void sdl2_touchkey_exit(void)
{
	if (key_window) {
		SDL_DestroyWindow(key_window);
		key_window = NULL;

		key_window_id = 0;
	}
	if (key_renderer) {
		SDL_DestroyRenderer(key_renderer);
		key_renderer = NULL;
	}

	free_keytop_bmp();

	free_touchkey_spec();

	enable_touchkey = FALSE;
}


/************************************************************************
 * タッチキーボード画像の生成/破棄
 ************************************************************************/

/* テクスチャはウインドウ毎に生成しておかないといけないので、
   [0]:メインウインドウ用、[1]:タッチキーウインドウ用、の二つが必要 */
static SDL_Texture *keytop_texture[2][2][KT_END];


/* 全てのキートップ画像と全てのタッチキーボード背景画像を texture で保持する。
 * 成功時、真を返す */
static int create_keytop_texture(int no, SDL_Renderer *renderer)
{
	int i, j;
	SDL_Surface *surface;
	SDL_Texture *texture;

	for (i = 0; i < KT_END; i++) {
		for (j = 0; j < 2; j++) {
			/* j = 0 : OFF画像、 j = 1 : ON画像 */
			surface = create_surface_from_malloc_bmp(keytop_bmp[i][j],
													 keytop_image[i].width,
													 keytop_image[i].height);
			if (surface == NULL) {
				return FALSE;
			}

			texture = SDL_CreateTextureFromSurface(renderer, surface);
			if (texture == NULL) {
				fprintf(stderr, "Cant Create Texture: %s\n", SDL_GetError());
				return FALSE;
			}

			keytop_texture[no][j][i] = texture;

			SDL_FreeSurface(surface);
		}
	}
	return TRUE;
}

static void destroy_keytop_texture(int no)
{
}


/************************************************************************
 * タッチキーの画像や座標の情報
 ************************************************************************/

/* 全てのタッチキーボードの諸元を変数に格納する */
static int malloc_touchkey_spec(void)
{
	struct {
		const T_LAYOUT_BUILDIN *layout_buildin;
		int w, h;
		int scale_w, scale_h;
	} buildin_list[] = {
#ifdef USE_BUILDIN_NEWMODE
		{ buildin_newmodel, BUILDIN_NEWMODEL_W, BUILDIN_NEWMODEL_H, BUILDIN_NEWMODEL_SCALE_W, BUILDIN_NEWMODEL_SCALE_H, },
#endif
#ifdef USE_BUILDIN_OLDMODE
		{ buildin_oldmodel, BUILDIN_OLDMODEL_W, BUILDIN_OLDMODEL_H, BUILDIN_OLDMODEL_SCALE_W, BUILDIN_OLDMODEL_SCALE_H, },
#endif
		{ buildin_jiskey,   BUILDIN_JISKEY_W,   BUILDIN_JISKEY_H,   BUILDIN_JISKEY_SCALE_W,   BUILDIN_JISKEY_SCALE_H,   },
		{ buildin_tenkey,   BUILDIN_TENKEY_W,   BUILDIN_TENKEY_H,   BUILDIN_TENKEY_SCALE_W,   BUILDIN_TENKEY_SCALE_H,   },
		{ buildin_gamekey,  BUILDIN_GAMEKEY_W,  BUILDIN_GAMEKEY_H,  BUILDIN_GAMEKEY_SCALE_W,  BUILDIN_GAMEKEY_SCALE_H,  },
		{ NULL, },
	}, *ll;

	for (ll = &buildin_list[0]; ll->layout_buildin; ll++) {
		const T_LAYOUT_BUILDIN *l = ll->layout_buildin;
		int base_width = ll->w;
		int base_height = ll->h;
		int scale_w = ll->scale_w;
		int scale_h = ll->scale_h;

		int nr_item = 0;
		T_LAYOUT *layout = (T_LAYOUT *) calloc(sizeof(T_LAYOUT), NR_KEYNO);
		T_LAYOUT *p = layout;

		if (layout == NULL) {
			fprintf(stderr, "Out of memory ()\n");
			return FALSE;
		}

		for (; l->keytop_no != KEYTOP_END; l++) {
			int i;
			unsigned char keytop_no = l->keytop_no;
			unsigned char lock_type = l->lock_type;
			float cx = (float) ((l->cx >= 0) ? (l->cx) : (base_width + l->cx));
			float cy = (float) ((l->cy >= 0) ? (l->cy) : (base_height + l->cy));
			float w = keytop_spec[keytop_no].width;
			float h = keytop_spec[keytop_no].height;

			p->keytop_no = keytop_no;
			p->lock_type = lock_type;
			for (i = 0; i < NR_KEYCOMBO; i++) {
				p->code[i] = keytop_spec[keytop_no].code[i];
			}

			p->x0 = cx - (w / 2);
			p->y0 = cy - (h / 2);
			p->x1 = cx + (w / 2);
			p->y1 = cy + (h / 2);
			p->sx = w;
			p->sy = h;

			nr_item++;
			if (nr_item >= NR_KEYNO) {
				break;
			}

			p++;
		}

		touchkey_list[ nr_touchkey_list ].layout = layout;
		touchkey_list[ nr_touchkey_list ].sz_array = nr_item;
		touchkey_list[ nr_touchkey_list ].base_width = base_width;
		touchkey_list[ nr_touchkey_list ].base_height = base_height;
		touchkey_list[ nr_touchkey_list ].scale_w = scale_w;
		touchkey_list[ nr_touchkey_list ].scale_h = scale_h;
		touchkey_list[ nr_touchkey_list ].r = 0xbf;
		touchkey_list[ nr_touchkey_list ].g = 0xbf;
		touchkey_list[ nr_touchkey_list ].b = 0xbf;
		nr_touchkey_list ++;
	}		

	return TRUE;
}

static void free_touchkey_spec(void)
{
	int i;
	for (i = 0; i < nr_touchkey_list; i++) {
		free(touchkey_list[i].layout);
		touchkey_list[i].layout = NULL;
	}
	nr_touchkey_list = 0;
}

/* (x,y) 座標に位置する仮想キー番号(0は無し)とキーコードを取得 */
static int get_touchkey_map(int x, int y, int width, int height,
							int code[NR_KEYCOMBO], int *lock_type)
{
	int i, j;
	const T_LAYOUT *layout = touchkey_list[key_layout_no].layout;
	int sz_array = touchkey_list[key_layout_no].sz_array;
	float fx = (float) x * touchkey_list[key_layout_no].base_width / width;
	float fy = (float) y * touchkey_list[key_layout_no].base_height / height;

	for (i = 0; i < sz_array; i++) {
		if ((layout[i].x0 <= fx) && (fx <= layout[i].x1) &&
			(layout[i].y0 <= fy) && (fy <= layout[i].y1)) {

			for (j = 0; j < NR_KEYCOMBO; j++) {
				code[j] = layout[i].code[j];
			}
			*lock_type = layout[i].lock_type;

			/* 仮想キー番号 は 1からの連番なので +1 する */
			return i + 1;
		}
	}

	/* 仮想キー無しなら 0 を返す */
	for (j = 0; j < NR_KEYCOMBO; j++) {
		code[j] = KEY88_INVALID;
	}
	*lock_type = LOCK_TYPE_NONE;
	return 0;
}


/************************************************************************
 * タッチキーボードの表示/非表示
 ************************************************************************/
static void get_layout_size(int layout_no, int *width, int *height);
static void touchkey_all_release(int all_keys);
static void modifier_stat_request(int request);

static int sdl2_touchkey_show(int layout_no)
{
	if (enable_touchkey == FALSE) {
		return FALSE;
	}

	memset(keydown_state, 0, sizeof(keydown_state));
	keydown_state_changed = TRUE;
	get_layout_size(layout_no, &key_width, &key_height);
	dragging_mode = DRAGGING_NONE;

	if ((separate_touchkey) && (!sdl_fullscreen)) {
		key_show = TRUE;
		SDL_HideWindow(key_window);
		SDL_SetWindowSize(key_window, key_width, key_height);
		SDL_ShowWindow(key_window);
	} else {
		sdl_buildin_touchkey = TRUE;
		quasi88_cfg_reset_window();
	}

	return TRUE;
}

void sdl2_touchkey_hide(int do_resize)
{
	if (enable_touchkey == FALSE) {
		return;
	}

	key_show = FALSE;
	sdl_buildin_touchkey = FALSE;

	if ((separate_touchkey) && (!sdl_fullscreen)) {
		SDL_HideWindow(key_window);
	} else {
		if (do_resize) {
			quasi88_cfg_reset_window();
		}
	}
}

void sdl2_touchkey(int request)
{
	if (enable_touchkey == FALSE) {
		return;
	}

	switch (request) {
	case TOUCHKEY_NOTIFY_REQ_ACTION:
	case TOUCHKEY_NOTIFY_REQ_PREV:
	case TOUCHKEY_NOTIFY_REQ_NEXT:

		touchkey_all_release(TRUE);

		if (key_show || sdl_buildin_touchkey) {
			/* タッチキー表示中ならば、閉じる。
			 * ただし、次に備えてレイアウトを次のに切り替え */
			int next;
			if (request == TOUCHKEY_NOTIFY_REQ_PREV) {
				next = key_layout_no - 1;
				if (next < 0) {
					next = nr_touchkey_list - 1;
				}
			} else {
				next = key_layout_no + 1;
				if (next >= nr_touchkey_list) {
					next = 0;
				}
			}
			key_layout_no = next;

			if (request == TOUCHKEY_NOTIFY_REQ_ACTION) {
				/* 一旦閉じる */
				sdl2_touchkey_hide(TRUE);
			} else {
				/* 次のレイアウトにてタッチキーを表示する */
				sdl2_touchkey_show(key_layout_no);
			}

		} else {
			/* 現在のレイアウトにてタッチキーを表示する */
			sdl2_touchkey_show(key_layout_no);
		}
		break;

	case TOUCHKEY_NOTIFY_REQ_SHIFT_OFF:
	case TOUCHKEY_NOTIFY_REQ_SHIFT_ON:
	case TOUCHKEY_NOTIFY_REQ_SHIFTL_OFF:
	case TOUCHKEY_NOTIFY_REQ_SHIFTL_ON:
	case TOUCHKEY_NOTIFY_REQ_SHIFTR_OFF:
	case TOUCHKEY_NOTIFY_REQ_SHIFTR_ON:
	case TOUCHKEY_NOTIFY_REQ_KANA_OFF:
	case TOUCHKEY_NOTIFY_REQ_KANA_ON:
	case TOUCHKEY_NOTIFY_REQ_CAPS_OFF:
	case TOUCHKEY_NOTIFY_REQ_CAPS_ON:
	case TOUCHKEY_NOTIFY_REQ_ROMAJI_OFF:
	case TOUCHKEY_NOTIFY_REQ_ROMAJI_ON:
	case TOUCHKEY_NOTIFY_REQ_NUM_OFF:
	case TOUCHKEY_NOTIFY_REQ_NUM_ON:
		if (key_show || sdl_buildin_touchkey) {
			modifier_stat_request(request);
		}
		break;

	default:
		break;
	}
}


/************************************************************************
 *
 ************************************************************************/
/* レイアウトの各種情報を、ワークにセットする */
static void get_layout_size(int layout_no, int *width, int *height)
{
	int base_width  = touchkey_list[layout_no].base_width;
	int base_height = touchkey_list[layout_no].base_height;
	int scale_w     = touchkey_list[layout_no].scale_w;
	int scale_h     = touchkey_list[layout_no].scale_h;

	*width  = base_width  * scale_w / 100;
	*height = base_height * scale_h / 100;
}


/************************************************************************
 *
 ************************************************************************/

/* マウス・タッチ操作中ワーク (どのキーを押下中かなどの情報を保持する) */
typedef struct {
	char         used;				/* このワーク使用中 */
	int          key_no;			/* 押下中の仮想キー番号 (1からの連番) */
	int          code[NR_KEYCOMBO];	/* 押下中のキーコード (最大NR_KEYCOMBO個)*/
	SDL_TouchID  touchId;			/* タッチの場合、TouchID */
	SDL_FingerID fingerId;			/* タッチの場合、FingerID */
} T_TOUCH_STATE;

/* マウス・タッチ あわせて NR_FINGER 個の情報を保持する */
#define	NR_FINGER		(11)

static T_TOUCH_STATE touch_state[NR_FINGER];

/* touch_state[0] はマウス操作時、 touch_state[1..] はタッチ操作時のワーク */
#define STATE_MOUSE		(0)


/* ロック中のキーを保持するワーク (touch_state[] は、マウス・タッチを
 * 解放すると押下情報が無くなってしまうので、 lock_state[] で保持する) */
typedef struct {
	char locked;
	int  code[NR_KEYCOMBO];
} T_LOCK_STATE;
static T_LOCK_STATE lock_state[NR_KEYNO];


/* キーコード code が押下状態となっている数を返す */
static int get_num_key_on(int code)
{
	int count = 0;
	int i, j;
	T_TOUCH_STATE *s;
	T_LOCK_STATE *l;

	for (s = &touch_state[0], i = 0; i < NR_FINGER; i++, s++) {
		if ((s->used) &&
			(s->key_no != 0)) {
			for (j = 0; j < NR_KEYCOMBO; j++) {
				if (s->code[j] == code) {
					count ++;
				}
			}
		}
	}
	for (l = &lock_state[0], i = 0; i < NR_KEYNO; i++, l++) {
		if (l->locked) {
			for (j = 0; j < NR_KEYCOMBO; j++) {
				if (l->code[j] == code) {
					count ++;
				}
			}
		}
	}
	return count;
}

/* マウス・タッチ操作中 (ワークを使用中) なら真を返す */
static int is_mouse_touch_used(void)
{
	int i;
	for (i = 0; i < NR_FINGER; i++) {
		if (touch_state[i].used) {
			return TRUE;
		}
	}
	return FALSE;
}


/************************************************************************
 * マウス・タッチの、オン/移動/オフ 各処理
 ************************************************************************/

static int get_symbol(int key88_no, int shift_on, int caps_on)
{
	int code = key88_no;
	if (shift_on) {
		if (BETWEEN(KEY88_a, code, KEY88_z) ||
			BETWEEN(KEY88_A, code, KEY88_Z)) {
			code ^= 0x20;
		} else if (BETWEEN(KEY88_1, code, KEY88_SEMICOLON)) {
			code -= 0x10;
		} else if (BETWEEN(KEY88_COMMA, code, KEY88_SLASH)) {
			code += 0x10;
		} else if (code == KEY88_AT) {
			code += 0x20;
		} else if (BETWEEN(KEY88_BRACKETLEFT, code, KEY88_CARET)) {
			code += 0x20;
		}
	}
	if (caps_on) {
		if (BETWEEN(KEY88_a, code, KEY88_z) ||
			BETWEEN(KEY88_A, code, KEY88_Z)) {
			code ^= 0x20;
		}
	}
	return code;
}

/* 装飾キーの状態 */
static unsigned short modifier_stat;
#define	MODIFIER_STAT_SHIFT				(0x0001)
#define	MODIFIER_STAT_SHIFTL			(0x0002)
#define	MODIFIER_STAT_SHIFTR			(0x0004)
#define	MODIFIER_STAT_KANA				(0x0008)
#define	MODIFIER_STAT_CAPS				(0x0010)
#define	MODIFIER_STAT_ROMAJI			(0x0020)
#define	MODIFIER_STAT_NUM				(0x0040)

#define	RESET_MODIFIER_STAT()			(modifier_stat = 0)
#define	SET_MODIFIER_STAT(bitpat)		(modifier_stat |=  (bitpat))
#define	CLR_MODIFIER_STAT(bitpat)		(modifier_stat &= ~(bitpat))

#define IS_MODIFIER_STAT_PRESSED_SHIFT_ANY()	\
	(modifier_stat & (MODIFIER_STAT_SHIFT  |	\
					  MODIFIER_STAT_SHIFTL |	\
					  MODIFIER_STAT_SHIFTR))

#define IS_MODIFIER_STAT_PRESSED_KANA()			\
	(modifier_stat & (MODIFIER_STAT_KANA))

#define IS_MODIFIER_STAT_PRESSED_CAPS()			\
	(modifier_stat & (MODIFIER_STAT_CAPS))

#define IS_MODIFIER_STAT_PRESSED_ROMAJI()		\
	(modifier_stat & (MODIFIER_STAT_ROMAJI))

#define IS_MODIFIER_STAT_PRESSED_NUM()			\
	(modifier_stat & (MODIFIER_STAT_NUM))

static void modifier_stat_request(int request)
{
	static const struct {
		unsigned char request;
		unsigned char on;
		unsigned short modifier;
	} table[] = {
		{ TOUCHKEY_NOTIFY_REQ_SHIFT_OFF,  FALSE, MODIFIER_STAT_SHIFT,  },
		{ TOUCHKEY_NOTIFY_REQ_SHIFT_ON,   TRUE,  MODIFIER_STAT_SHIFT,  },
		{ TOUCHKEY_NOTIFY_REQ_SHIFTL_OFF, FALSE, MODIFIER_STAT_SHIFTL, },
		{ TOUCHKEY_NOTIFY_REQ_SHIFTL_ON,  TRUE,  MODIFIER_STAT_SHIFTL, },
		{ TOUCHKEY_NOTIFY_REQ_SHIFTR_OFF, FALSE, MODIFIER_STAT_SHIFTR, },
		{ TOUCHKEY_NOTIFY_REQ_SHIFTR_ON,  TRUE,  MODIFIER_STAT_SHIFTR, },
		{ TOUCHKEY_NOTIFY_REQ_KANA_OFF,   FALSE, MODIFIER_STAT_KANA,   },
		{ TOUCHKEY_NOTIFY_REQ_KANA_ON,    TRUE,  MODIFIER_STAT_KANA,   },
		{ TOUCHKEY_NOTIFY_REQ_CAPS_OFF,   FALSE, MODIFIER_STAT_CAPS,   },
		{ TOUCHKEY_NOTIFY_REQ_CAPS_ON,    TRUE,  MODIFIER_STAT_CAPS,   },
		{ TOUCHKEY_NOTIFY_REQ_ROMAJI_OFF, FALSE, MODIFIER_STAT_ROMAJI, },
		{ TOUCHKEY_NOTIFY_REQ_ROMAJI_ON,  TRUE,  MODIFIER_STAT_ROMAJI, },
		{ TOUCHKEY_NOTIFY_REQ_NUM_OFF,    FALSE, MODIFIER_STAT_NUM,    },
		{ TOUCHKEY_NOTIFY_REQ_NUM_ON,     TRUE,  MODIFIER_STAT_NUM,    },
	};

	size_t i;
	for (i = 0; i < COUNTOF(table); i++) {
		if (request == table[i].request) {
			if (table[i].on) {
				SET_MODIFIER_STAT(table[i].modifier);
			} else {
				CLR_MODIFIER_STAT(table[i].modifier);
			}
			break;
		}
	}
	keydown_state_changed = TRUE;
}

static void touch_key_pressed(int key88_no)
{
	switch (key88_no) {
	case KEY88_SHIFT:
		SET_MODIFIER_STAT(MODIFIER_STAT_SHIFT);
		break;
	case KEY88_SHIFTL:
		SET_MODIFIER_STAT(MODIFIER_STAT_SHIFTL);
		break;
	case KEY88_SHIFTR:
		SET_MODIFIER_STAT(MODIFIER_STAT_SHIFTR);
		break;
	case KEY88_CAPS:
		SET_MODIFIER_STAT(MODIFIER_STAT_CAPS);
		break;
	}

	if (!quasi88_is_exec()) {
		key88_no = get_symbol(key88_no,
							  IS_MODIFIER_STAT_PRESSED_SHIFT_ANY(),
							  IS_MODIFIER_STAT_PRESSED_CAPS());
	}

	quasi88_key_pressed(key88_no);
}

static void touch_key_released(int key88_no)
{
	switch (key88_no) {
	case KEY88_SHIFT:
		CLR_MODIFIER_STAT(MODIFIER_STAT_SHIFT);
		break;
	case KEY88_SHIFTL:
		CLR_MODIFIER_STAT(MODIFIER_STAT_SHIFTL);
		break;
	case KEY88_SHIFTR:
		CLR_MODIFIER_STAT(MODIFIER_STAT_SHIFTR);
		break;
	case KEY88_CAPS:
		CLR_MODIFIER_STAT(MODIFIER_STAT_CAPS);
		break;
	}

	if (!quasi88_is_exec()) {
		key88_no = get_symbol(key88_no,
							  IS_MODIFIER_STAT_PRESSED_SHIFT_ANY(),
							  IS_MODIFIER_STAT_PRESSED_CAPS());
	}

	quasi88_key_released(key88_no);
}

static void keydown_state_on(int key_no)
{
	keydown_state[key_no] = TRUE;
	keydown_state_changed = TRUE;
}

static void keydown_state_off(int key_no)
{
	keydown_state[key_no] = FALSE;
	keydown_state_changed = TRUE;
}


/* 仮想キー key_no (キーコードは code[]) を押下 or スライド選択した時の処理 */
static void action_off_on(T_TOUCH_STATE *s, int key_no, int code[], int lock)
{
	int i;

	if (lock) {
		/* ロックキーなら、キーは押下してないものとする */
		key_no = 0;
	}

	if (s->key_no == key_no) {
		/* マウス/指を移動したが、仮想キーは変わらず */
		return;

	} else {
		/* マウス/指を移動したことで、仮想キーが変わった */

		if (s->key_no != 0) {
			/* 変わる前に押下していた仮想キーがあったなら、オフ */

			keydown_state_off(s->key_no);

			for (i = 0; i < NR_KEYCOMBO; i++) {
				if (s->code[i] != KEY88_INVALID) {
					if (get_num_key_on(s->code[i]) == 1) {
						/* 最後の1個ならオフ */
						touch_key_released(s->code[i]);
					}
				}
			}
		}

		if (key_no != 0) {
			/* 変わった後に押下している仮想キーがあるなら、オン */

			keydown_state_on(key_no);

			for (i = 0; i < NR_KEYCOMBO; i++) {
				if (code[i] != KEY88_INVALID) {
					if (get_num_key_on(code[i]) == 0) {
						/* 最初の1個ならオン */
						touch_key_pressed(code[i]);
					}
				}
				s->code[i] = code[i];
			}
		}
		s->key_no = key_no;
	}
}

/* 仮想ロックキー key_no (キーコードは code[]) を押下した時の処理 */
static void action_lock(int key_no, int code[])
{
	int i;
	T_LOCK_STATE *s = &lock_state[key_no];

	if (s->locked) {

		keydown_state_off(key_no);

		for (i = 0; i < NR_KEYCOMBO; i++) {
			if (code[i] != KEY88_INVALID) {
				if (get_num_key_on(code[i]) == 1) {
					/* 最後の1個ならオフ */
					touch_key_released(code[i]);
				}
			}
		}
		s->locked = FALSE;

	} else {

		keydown_state_on(key_no);

		for (i = 0; i < NR_KEYCOMBO; i++) {
			if (code[i] != KEY88_INVALID) {
				if (get_num_key_on(code[i]) == 0) {
					touch_key_pressed(code[i]);
				}
			}
			s->code[i] = code[i];
		}
		s->locked = TRUE;
	}
}

/*
 * マウス ボタン押下
 */
void touchkey_mouse_down(int button_x, int button_y, int width, int height,
						 int clicks)
{
	T_TOUCH_STATE *s = &touch_state[0];

	if (s->used == FALSE) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };
		int lock = LOCK_TYPE_NONE;

		s->used = TRUE;
		s->touchId = 0;
		s->fingerId = 0;
		s->key_no = 0;

		/* (x,y) 座標に位置する仮想キー番号(0は無し)とキーコードを取得 */
		key_no = get_touchkey_map(button_x, button_y, width, height,
								  code, &lock);

		switch (lock) {
		case LOCK_TYPE_NONE:
			action_off_on(s, key_no, code, FALSE);
			break;

		case LOCK_TYPE_SINGLE:
			action_off_on(s, key_no, code, TRUE);
			action_lock(key_no, code);
			break;

		case LOCK_TYPE_DOUBLE:
			if (clicks != 2) {
				/* シングルクリックなら、 */
				if (lock_state[key_no].locked) {
					/* すでにロック状態なら、仮想キーなし扱い */
					action_off_on(s, 0, code, FALSE);
				} else {
					/* そうでないなら、通常キー扱い */
					action_off_on(s, key_no, code, FALSE);
				}
			} else {
				/* ダブルクリックなら、ロックキー扱い */
				action_off_on(s, key_no, code, TRUE);
				action_lock(key_no, code);
			}
		}
	}
}

/*
 * マウス ボタン押下したまま移動
 */
void touchkey_mouse_move(int motion_x, int motion_y, int width, int height)
{
	T_TOUCH_STATE *s = &touch_state[0];

	if (s->used) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };
		int lock = LOCK_TYPE_NONE;

		/* (x,y) 座標に位置する仮想キー番号(0は無し)とキーコードを取得 */
		key_no = get_touchkey_map(motion_x, motion_y, width, height,
								  code, &lock);

		switch (lock) {
		case LOCK_TYPE_NONE:
			action_off_on(s, key_no, code, FALSE);
			break;

		case LOCK_TYPE_SINGLE:
			action_off_on(s, key_no, code, TRUE);
			break;

		case LOCK_TYPE_DOUBLE:
			if (lock_state[key_no].locked) {
				/* すでにロック状態 */
				action_off_on(s, key_no, code, TRUE);
			} else {
				action_off_on(s, key_no, code, FALSE);
			}
			break;
		}
	}
}

/*
 * マウス ボタン解放
 */
void touchkey_mouse_up(void)
{
	T_TOUCH_STATE *s = &touch_state[0];

	if (s->used) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };

		action_off_on(s, key_no, code, FALSE);

		s->used = FALSE;
	}
}

/*
 * タッチ押下
 */
void touchkey_finger_down(SDL_TouchID touchId, SDL_FingerID fingerId,
						  int finger_x, int finger_y, int width, int height,
						  int clicks)
{
	int i;
	T_TOUCH_STATE *s;
	int empty = 0;

	for (i = 1, s = &touch_state[i]; i < NR_FINGER; i++, s++) {
		if ((empty == 0) &&
			(s->used == FALSE)) {
			empty = i;
		}
		if (s->touchId == touchId &&
			s->fingerId == fingerId) {
			/* すでに使用中? */
			return;
		}
	}

	if (empty) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };
		int lock = LOCK_TYPE_NONE;

		s = &touch_state[empty];

		s->used = TRUE;
		s->touchId = touchId;
		s->fingerId = fingerId;
		s->key_no = 0;

		/* (x,y) 座標に位置する仮想キー番号(0は無し)とキーコードを取得 */
		key_no = get_touchkey_map(finger_x, finger_y, width, height,
								  code, &lock);

		switch (lock) {
		case LOCK_TYPE_NONE:
			action_off_on(s, key_no, code, FALSE);
			break;

		case LOCK_TYPE_SINGLE:
			action_off_on(s, key_no, code, TRUE);
			action_lock(key_no, code);
			break;

		case LOCK_TYPE_DOUBLE:
			if (clicks != 2) {
				/* シングルクリックなら、 */
				if (lock_state[key_no].locked) {
					/* すでにロック状態なら、仮想キーなし扱い */
					action_off_on(s, 0, code, FALSE);
				} else {
					/* そうでないなら、通常キー扱い */
					action_off_on(s, key_no, code, FALSE);
				}
			} else {
				/* ダブルクリックなら、ロックキー扱い */
				action_off_on(s, key_no, code, TRUE);
				action_lock(key_no, code);
			}
		}
	}
}

/*
 * タッチしたまま移動
 */
void touchkey_finger_move(SDL_TouchID touchId, SDL_FingerID fingerId,
						  int finger_x, int finger_y, int width, int height)
{
	int i, found = FALSE;
	T_TOUCH_STATE *s;

	for (i = 1, s = &touch_state[i]; i < NR_FINGER; i++, s++) {
		if ((s->used) &&
			(s->touchId == touchId) &&
			(s->fingerId == fingerId)) {
			found = TRUE;
			break;
		}
	}

	if (found) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };
		int lock = LOCK_TYPE_NONE;

		/* (x,y) 座標に位置する仮想キー番号(0は無し)とキーコードを取得 */
		key_no = get_touchkey_map(finger_x, finger_y, width, height,
								  code, &lock);

		switch (lock) {
		case LOCK_TYPE_NONE:
			action_off_on(s, key_no, code, FALSE);
			break;

		case LOCK_TYPE_SINGLE:
			action_off_on(s, key_no, code, TRUE);
			break;

		case LOCK_TYPE_DOUBLE:
			if (lock_state[key_no].locked) {
				/* すでにロック状態 */
				action_off_on(s, key_no, code, TRUE);
			} else {
				action_off_on(s, key_no, code, FALSE);
			}
			break;
		}
	}
}

/*
 * タッチしたまま解放
 */
void touchkey_finger_up(SDL_TouchID touchId, SDL_FingerID fingerId)
{
	int i, found = FALSE;
	T_TOUCH_STATE *s;

	for (i = 1, s = &touch_state[i]; i < NR_FINGER; i++, s++) {
		if ((s->used) &&
			(s->touchId == touchId) &&
			(s->fingerId == fingerId)) {
			found = TRUE;
			break;
		}
	}

	if (found) {
		int key_no = 0;
		int code[NR_KEYCOMBO] = { KEY88_INVALID, };

		action_off_on(s, key_no, code, FALSE);

		s->used = FALSE;
	}
}


/*
 * すべてを解放する
 */
static void touchkey_all_release(int all_keys)
{
	int i, j;
	T_TOUCH_STATE *s;
	T_LOCK_STATE *l;

	key_nr_finger_clicks = 0;

	for (s = &touch_state[0], i = 0; i < NR_FINGER; i++, s++) {
		if ((s->used) &&
			(s->key_no != 0)) {

			keydown_state_off(s->key_no);

			for (j = 0; j < NR_KEYCOMBO; j++) {
				if (s->code[j] != KEY88_INVALID) {
					if (get_num_key_on(s->code[j]) == 1) {
						touch_key_released(s->code[j]);
					}
				}
			}
		}
		memset(s, 0, sizeof(*s));
	}

	if (all_keys == FALSE) {
		/* ロックキーは解放しない場合、ここで終了 */
		return;
	}

	for (l = &lock_state[0], i = 0; i < NR_KEYNO; i++, l++) {
		if (l->locked) {

			keydown_state_off(i);

			for (j = 0; j < NR_KEYCOMBO; j++) {
				if (l->code[j] != KEY88_INVALID) {
					if (get_num_key_on(l->code[j]) == 1) {
						touch_key_released(l->code[j]);
					}
				}
			}
		}
		memset(l, 0, sizeof(*l));
	}
}


/************************************************************************
 * マウス・タッチの、オン/移動/オフイベント処理
 ************************************************************************/

static int is_touch_area(int x, int y)
{
	/* タッチキー表示中ならば、クリックした座標がタッチキーエリアかどうか判定 */
	if (BETWEEN(sdl_area_touch.x, x, (sdl_area_touch.x + sdl_area_touch.w)) &&
		BETWEEN(sdl_area_touch.y, y, (sdl_area_touch.y + sdl_area_touch.h))) {
		return TRUE;
	}
	return FALSE;
}


/*
 * ウインドウ内に、メインエリアとタッチキーエリアの両方がある場合、
 * どちらのエリアをマウスでまたは指タッチで操作開始したかによって、
 * その後のマウス・指タッチの処理が変わる。
 *
 * MENU     … MENUエリアを指タッチで操作開始したら、この状態になる。
 *             指を離すまでマウス操作は無視。タッチキーエリアは操作不可
 * TOUCHKEY … タッチキーエリアをマウスで操作開始したら、この状態になる。
 *             指タッチもマウス操作もタッチキーエリアで処理
 * COMBI    … タッチキーエリアを指操作で開始したら、もしくは
 *             EMU/MENUエリアをマウスで操作開始したら、この状態になる。
 *             指タッチはタッチキーエリア、マウス操作はEMU/MENUエリアで処理
 *
 *                             |                SDLイベント               |
 *  dragging|                  |BUTTONDOWN/  |MOUSEMOTION/ |BUTTONUP/     |
 *   mode   |                  |FINGERDOWN   |FINGERMOTION |FINGERUP      |
 *  --------+-----------------++-------------+-------------+--------------+
 *  NONE    |マウス| M | EMU  || emu/menu()  | emu/menu()  | emu/menu()   |
 *          |      |   | MENU || => COMBI    |             |              |
 *          |      +----------++-------------+             |              |
 *          |      | T        || touchkey()  |             |              |
 *          |      |          || => TOUCHKEY |             |              |
 *          +-----------------++-------------+-------------+--------------+
 *          |偽    | M | EMU  || 無視        | 無視        | 無視         |
 *          |      |   +------++-------------+-------------+--------------+
 *          |      |   | MENU || emu/menu()  | emu/menu()  | 無視         |
 *          |      |   |      || => MENU     |             |              |
 *          |      +----------++-------------+-------------+--------------+
 *          |      | T        || 無視 ＊     | 無視        | 無視         |
 *          +-----------------++-------------+-------------+--------------+
 *          |指    | M | EMU  || 無視 †     | 無視 †     | 無視 †      |
 *          |      |   +------++-------------+-------------+--------------+
 *          |      |   | MENU || 無視        | 無視        | 無視         |
 *          |      +----------++-------------+-------------+--------------+
 *          |      | T        || touchkey()  | 無視        | 無視         |
 *          |      |          || => COMBI    |             |              |
 *  --------+-----------------++-------------+-------------+--------------+
 *  MENU    |マウス           || 無視        | 無視        | 無視         |
 *          +-----------------++-------------+-------------+--------------+
 *          |偽               || emu/mouse() | emu/mouse() | emu/mouse()  |
 *          |                 ||             |             | => NONE 条件 |
 *          +-----------------++-------------+-------------+--------------+
 *          |指               || 無視        | 無視        | 無視         |
 *  --------+-----------------++-------------+-------------+--------------+
 *  TOUCHKEY|マウス           || touchkey()  | touchkey()  | touchkey()   |
 *          |                 ||             |             | => NONE 条件 |
 *          +-----------------++-------------+-------------+--------------+
 *          |偽               || 無視 ＊     | 無視        | 無視         |
 *          +-----------------++-------------+-------------+--------------+
 *          |指               || touchkey()  | touchkey()  | touchkey()   |
 *          |                 ||             |             | => NONE 条件 |
 *  --------+-----------------++-------------+-------------+--------------+
 *  COMBI   |マウス           || emu/menu()  | emu/menu()  | emu/menu()   |
 *          |                 ||             |             | => NONE 条件 |
 *          +-----------------++-------------+-------------+--------------+
 *          |偽               || 無視 ＊     | 無視        | 無視         |
 *          +-----------------++-------------+-------------+--------------+
 *          |指               || touchkey()  | touchkey()  | touchkey()   |
 *          |                 ||             |             | => NONE 条件 |
 *  --------+-----------------++-------------+-------------+--------------+
 * ▼マウス … マウスイベント(マウス操作) / 偽 … マウスイベント(指操作)
 *   指     … フィンガーイベント
 * ▼M   … メインエリアでイベント発生 / T    … タッチキーエリアでイベント発生
 * ▼EMU … メインエリアは EMU 状態    / MENU … メインエリアは MENU 状態
 * ●emu/menu() … EMU/MENU向け処理    / touchkey() … タッチキー向け処理
 * ●=> … dragging_mode の変更
 *         NONE になる条件は、全ての指とマウスボタンがOFFであること
 * ＊ … 処理としては無視だが、連続タップのカウント値の保存が必要
 *       このカウント値は、直後の指フィンガーイベントで使用する
 * † … 処理としては無視だが、呼び出し元で区別できるようにする (X_X);
 */


/* 本体ウインドウ (タッチキー表示の場合も含む) で、MOUSE/FINGERイベントを
 * 受けた時に、次の動作 (M_F_EVENT_xxx) を返す。(処理は呼び出し元で行う) */
int get_mouse_finger_event_in_main_window(const SDL_Event *E)
{
	int m_f_event = M_F_EVENT_IGNORE;
	int in_touchkey_area = FALSE;
	enum {
		MOUSE,		/* マウス (MOUSEイベント) */
		TOUCH,		/* 偽     (MOUSEイベント) */
		FINGER,		/* 指     (FINGERイベント) */
	} kind;

	/* 6種類のイベントを、3種類の kind に分類する */
	switch (E->type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONUP:
		if (E->button.which != SDL_TOUCH_MOUSEID) {
			kind = MOUSE;
		} else {
			kind = TOUCH;
		}
		break;
	case SDL_FINGERDOWN:
	case SDL_FINGERMOTION:
	case SDL_FINGERUP:
		kind = FINGER;
		break;
	default:
		return m_f_event;
	}

	/* SDL_FINGERDOWN イベント時は、直前の連続タップ回数を使用する */
	if (E->type == SDL_FINGERDOWN) {
		/* DO NOTHING */;
	} else {
		key_nr_finger_clicks = 0;
	}

	/* 「タッチキーをマウスで操作中」「EMU/MENUをマウス、タッチキーを指で
	 * 操作中」の場合、 マウスボタン・指タッチが全てOFFならば、状態遷移する。
	 * この処理は SDL_MOUSEBUTTONUP/SDL_FINGERUP イベント発生時に行うべきだが、
	 * その時点ではまだ、全てOFFの判定ができなかったため、ここで処理する */
	if ((dragging_mode == DRAGGING_TOUCHKEY_MOUSE) ||
		(dragging_mode == DRAGGING_COMBINAITON)) {
		if (is_mouse_touch_used() == FALSE) {
			dragging_mode = DRAGGING_NONE;
		}
	}
/*
printf("{%d} %d =>",is_mouse_touch_used(),dragging_mode);
*/

	sdl_in_touchkey_area = FALSE;

	switch (dragging_mode) {

	case DRAGGING_NONE:
		switch (E->type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_FINGERDOWN:
			/* マウスボタンON or 指タッチON イベント */

			if (E->type == SDL_MOUSEBUTTONDOWN) {
				in_touchkey_area = is_touch_area(E->button.x, E->button.y);
			} else {
				in_touchkey_area =
					is_touch_area((int) (E->tfinger.x * sdl_width),
								  (int) (E->tfinger.y * sdl_height));
			}

			switch (kind) {
			case MOUSE:
				if (in_touchkey_area == FALSE) {
					m_f_event = M_F_EVENT_EMU_MENU;
					dragging_mode = DRAGGING_COMBINAITON;
				} else {
					m_f_event = M_F_EVENT_TOUCHKEY;
					dragging_mode = DRAGGING_TOUCHKEY_MOUSE;
				}
				break;
			case TOUCH:
				if (in_touchkey_area == FALSE) {
					if (quasi88_is_exec()) {
						m_f_event = M_F_EVENT_IGNORE;
					} else {
						m_f_event = M_F_EVENT_EMU_MENU;
						dragging_mode = DRAGGING_MENU_TOUCH;
					}
				} else {
					m_f_event = M_F_EVENT_IGNORE;
				}
				key_nr_finger_clicks = E->button.clicks;
				break;
			case FINGER:
				if (in_touchkey_area == FALSE) {
					if (quasi88_is_exec()) {
						m_f_event = M_F_EVENT_IGNORE2;
					} else {
						m_f_event = M_F_EVENT_IGNORE;
					}
				} else {
					m_f_event = M_F_EVENT_TOUCHKEY;
					dragging_mode = DRAGGING_COMBINAITON;
				}
				break;
			}
			break;

		case SDL_MOUSEMOTION:
		case SDL_FINGERMOTION:
			/* マウス移動 or 指タッチ移動 */
			if (E->type == SDL_MOUSEMOTION) {
				/* 強引だが、タッチキーエリアなのにマウス非表示なら表示する */
				sdl_in_touchkey_area = is_touch_area(E->button.x, E->button.y);
				if (sdl_req_mouse_show == FALSE) {
					if (sdl_in_touchkey_area) {
						if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE) {
							SDL_ShowCursor(SDL_ENABLE);
						}
					} else {
						if (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) {
							SDL_ShowCursor(SDL_DISABLE);
						}
					}
				}
			}

			switch (kind) {
			case MOUSE:
				m_f_event = M_F_EVENT_EMU_MENU;
				break;
			case TOUCH:
				if (quasi88_is_exec()) {
					m_f_event = M_F_EVENT_IGNORE;
				} else {
					m_f_event = M_F_EVENT_EMU_MENU;
				}
				break;
			case FINGER:
				if (quasi88_is_exec()) {
					m_f_event = M_F_EVENT_IGNORE2;
				} else {
					m_f_event = M_F_EVENT_IGNORE;
				}
				break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
		case SDL_FINGERUP:
			/* マウスボタンOFF or 指タッチOFF */
			switch (kind) {
			case MOUSE:
				m_f_event = M_F_EVENT_EMU_MENU;
				break;
			case TOUCH:
				m_f_event = M_F_EVENT_IGNORE;
				break;
			case FINGER:
				if (quasi88_is_exec()) {
					m_f_event = M_F_EVENT_IGNORE2;
				} else {
					m_f_event = M_F_EVENT_IGNORE;
				}
				break;
			}
			break;
		}
		break;

	case DRAGGING_MENU_TOUCH:
		switch (kind) {
		case TOUCH:
			m_f_event = M_F_EVENT_EMU_MENU;
			/* マウスボタン・指タッチが全てOFFなら DRAGGING_NONE に遷移 */
			if ((E->type == SDL_MOUSEBUTTONUP) &&
				(SDL_GetMouseState(NULL, NULL) == 0)) {
				dragging_mode = DRAGGING_NONE;
			}
			break;
		case MOUSE:
		case FINGER:
			m_f_event = M_F_EVENT_IGNORE;
			break;
		}
		break;

	case DRAGGING_TOUCHKEY_MOUSE:
		switch (kind) {
		case MOUSE:
		case FINGER:
			m_f_event = M_F_EVENT_TOUCHKEY;
			/* マウスボタン・指タッチが全てOFFなら DRAGGING_NONE に遷移だが、
			 * この時点ではまだ OFF 判定できないので、次イベント時に持ち越し */
#if 0
			if ((E->type == SDL_MOUSEBUTTONUP) ||
				(E->type == SDL_FINGERUP)) {
				if (is_mouse_touch_used() == FALSE) {
					dragging_mode = DRAGGING_NONE;
				}
			}
#endif
			break;
		case TOUCH:
			m_f_event = M_F_EVENT_IGNORE;
			if (E->type == SDL_MOUSEBUTTONDOWN) {
				key_nr_finger_clicks = E->button.clicks;
			}
			break;
		}
		break;

	case DRAGGING_COMBINAITON:
		switch (kind) {
		case MOUSE:
			m_f_event = M_F_EVENT_EMU_MENU;
			/* マウスボタン・指タッチが全てOFFなら DRAGGING_NONE に遷移だが、
			 * この時点ではまだ OFF 判定できないので、次イベント時に持ち越し */
			break;
		case TOUCH:
			m_f_event = M_F_EVENT_IGNORE;
			if (E->type == SDL_MOUSEBUTTONDOWN) {
				key_nr_finger_clicks = E->button.clicks;
			}
			break;
		case FINGER:
			m_f_event = M_F_EVENT_TOUCHKEY;
			/* マウスボタン・指タッチが全てOFFなら DRAGGING_NONE に遷移だが、
			 * この時点ではまだ OFF 判定できないので、次イベント時に持ち越し */
			break;
		}
		break;
	}
/*
	printf("%-20s %-7s => %d [%d] {%d} ;%-18s\n",
		   ((E->type==SDL_MOUSEMOTION) ? "SDL_MOUSEMOTION"
			: ((E->type==SDL_MOUSEBUTTONDOWN) ? "SDL_MOUSEBUTTONDOWN"
			   : ((E->type==SDL_MOUSEBUTTONUP) ? "SDL_MOUSEBUTTONUP"
				  : ((E->type==SDL_FINGERMOTION) ? "SDL_FINGERMOTION"
					 : ((E->type==SDL_FINGERDOWN) ? "SDL_FINGERDOWN"
						: ((E->type==SDL_FINGERUP) ? "SDL_FINGERUP" : "")))))),
		   ((kind==MOUSE) ? "MOUSE"
			: ((kind==TOUCH) ? "TOUCH"
			   : ((kind==FINGER) ? "FINGER" : ""))),
		   dragging_mode,key_nr_finger_clicks,is_mouse_touch_used(),
		   ((m_f_event==M_F_EVENT_IGNORE) ? "M_F_EVENT_IGNORE"
			: ((m_f_event==M_F_EVENT_IGNORE2) ? "M_F_EVENT_IGNORE2"
			   : ((m_f_event==M_F_EVENT_EMU_MENU) ? "M_F_EVENT_EMU_MENU"
				  : ((m_f_event==M_F_EVENT_TOUCHKEY) ? "M_F_EVENT_TOUCHKEY" : "")))));
*/
	return m_f_event;
}

/* タッチキーウインドウで、MOUSE/FINGERイベントを
 * 受けた時に、次の動作 (M_F_EVENT_xxx) を返す。(処理は呼び出し元で行う) */
int get_mouse_finger_event_in_touchkey_window(const SDL_Event *E)
{
	int m_f_event = M_F_EVENT_IGNORE;

	switch (E->type) {

	case SDL_MOUSEMOTION:
		if (E->motion.windowID == key_window_id) {
			key_nr_finger_clicks = 0;
			if (E->motion.which == SDL_TOUCH_MOUSEID) {
				break;
			}
			if (E->motion.state & SDL_BUTTON_LMASK) {
				m_f_event = M_F_EVENT_TOUCHKEY;
			}
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if (E->button.windowID == key_window_id) {
			key_nr_finger_clicks = 0;
			if (E->button.which == SDL_TOUCH_MOUSEID) {
				if (E->type == SDL_MOUSEBUTTONDOWN) {
					key_nr_finger_clicks = E->button.clicks;
				}
				break;
			}
			if (E->button.button == SDL_BUTTON_LEFT) {
				m_f_event = M_F_EVENT_TOUCHKEY;
			}
		}
		break;

	case SDL_FINGERMOTION:
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
		if (E->tfinger.windowID == key_window_id) {
			if (E->type != SDL_FINGERDOWN) {
				key_nr_finger_clicks = 0;
			}
			m_f_event = M_F_EVENT_TOUCHKEY;
		}
		break;
	}

	return m_f_event;
}

/* タッチキーウインドウで、WINDOWイベントを受けた時に、処理を実行する */
void action_window_event_in_touchkey_window(const SDL_Event *E)
{
	if ((E->type == SDL_WINDOWEVENT) &&
		(E->window.windowID == key_window_id)) {

		keydown_state_changed = TRUE;

		switch (E->window.event) {
		case SDL_WINDOWEVENT_SHOWN:
		case SDL_WINDOWEVENT_HIDDEN:
		case SDL_WINDOWEVENT_EXPOSED:
		case SDL_WINDOWEVENT_MOVED:
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
			/* 全てオフにする */
			touchkey_all_release(FALSE);
			break;

		case SDL_WINDOWEVENT_ENTER:
			SDL_ShowCursor(SDL_ENABLE);
			break;

		case SDL_WINDOWEVENT_RESIZED:
			/* この次に SDL_WINDOWEVENT_SIZE_CHANGED が発生する */
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			DBGPRINTF(("Change %d x %d\n", E->window.data1, E->window.data2));

			/* 全てオフにする */
			touchkey_all_release(FALSE);

			key_width = E->window.data1;
			key_height = E->window.data2;
			break;

		case SDL_WINDOWEVENT_CLOSE:
			DBGPRINTF(("Close\n"));

			/* 全てオフにする */
			touchkey_all_release(TRUE);

			sdl2_touchkey_hide(FALSE);
			break;
		}
	}
}


int touchkey_config_action(const SDL_Event *E)
{
	int i;
	SDL_Keymod st;

	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {

			switch (E->type) {
			case SDL_MOUSEWHEEL:
				st = SDL_GetModState();
				if (st & KMOD_CTRL) {
					touchkey_cfg_overlap((E->wheel.y > 0) ? +10 : -10);
					return TRUE;
				} else if (st & KMOD_ALT) {
					touchkey_cfg_recude((E->wheel.y > 0) ? -10 : +10);
					return TRUE;
				} else if (st & KMOD_SHIFT) {
					touchkey_cfg_fit((E->wheel.y > 0) ? TRUE : FALSE);
					return TRUE;
				} else {
					touchkey_cfg_opaque((E->wheel.y > 0) ? +8 : -8);
					return TRUE;
				}
				break;

			case SDL_MULTIGESTURE:
				/*printf("%f (%f,%f) %f\n",E->mgesture.dDist,E->mgesture.x,E->mgesture.y,E->mgesture.dTheta);*/
				if (E->mgesture.dDist > 0.002f) {
					i = +10;
				} else if (E->mgesture.dDist < -0.002f) {
					i = -10;
				} else {
					break;
				}
				if (is_touch_area((int) (E->mgesture.x * sdl_width),
								  (int) (E->mgesture.y * sdl_height))
					== FALSE) {
					if (E->mgesture.x > 0.6f) {
						/* メインエリア右側をピンチ */
						touchkey_cfg_overlap(i);
					} else if (E->mgesture.x < 0.4f) {
						/* メインエリア左側をピンチ */
						touchkey_cfg_recude(i);
					} else {
						/* メインエリア中央をピンチ */
						touchkey_cfg_opaque(i * 8 / 10);
					}
					return TRUE;
				}
				break;

			case SDL_FINGERDOWN:
				if (dragging_mode == DRAGGING_NONE) {
					if (key_nr_finger_clicks == 3) {
						int x = (int) (E->tfinger.x * sdl_width);
						int y = (int) (E->tfinger.y * sdl_height);
						if ((is_touch_area(x, y) == FALSE) &&
							(get_area_of_coord(x - sdl_area_main.x,
											   y - sdl_area_main.y)
							 != AREA_TOOLBAR)) {
							/* メインエリア(ツールバー以外)をトリプルタップ */
							touchkey_cfg_fit((touchkey_fit) ? FALSE : TRUE);
						}
					}
				}
				break;
			}
		}
	}		
	return FALSE;
}


/************************************************************************
 * タッチキーボード表示処理
 ************************************************************************/

int sdl2_touchkey_need_rendering(void)
{
	if ((enable_touchkey) &&
		(sdl_buildin_touchkey) &&
		(keydown_state_changed)) {

		return TRUE;
	} else {
		return FALSE;
	}
}


void sdl2_touchkey_rendering(int no, SDL_Renderer *renderer, SDL_Rect *dstrect,
							 int with_keybase, Uint8 alpha)
{
	int mode = 0;
	int i;
	SDL_Rect rect;
	SDL_Texture *texture;
	const T_LAYOUT *layout = touchkey_list[key_layout_no].layout;
	int sz_array = touchkey_list[key_layout_no].sz_array;

	if (IS_MODIFIER_STAT_PRESSED_NUM()) {
		mode = KEYTOP_STAT_NUM;
	} else {
		if (IS_MODIFIER_STAT_PRESSED_ROMAJI()) {
			mode |= KEYTOP_STAT_ROMAJI;
		} else if (IS_MODIFIER_STAT_PRESSED_KANA()) {
			mode |= KEYTOP_STAT_KANA;
		}
		if (IS_MODIFIER_STAT_PRESSED_SHIFT_ANY()) {
			mode |= 1;
		}
	}

	if (with_keybase) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
		SDL_SetRenderDrawColor(renderer,
							   touchkey_list[key_layout_no].r,
							   touchkey_list[key_layout_no].g,
							   touchkey_list[key_layout_no].b,
							   255);
		SDL_RenderFillRect(renderer, dstrect);
	}

	for (i = 0; i < sz_array; i++) {
		/* keydown_state[]の添え字は仮想キー番号なので +1 して参照する */
		int keytop_no = layout[i].keytop_no;
		int keytop_img_no = keytop_spec[keytop_no].keytop[mode];

		if (keydown_state[i + 1]) {
			/* キートップON画像を表示 */
			texture = keytop_texture[no][1][ keytop_img_no ];

		} else {
			/* キートップOFF画像を表示 */
			texture = keytop_texture[no][0][ keytop_img_no ];
		}

		{
			int base_width = touchkey_list[key_layout_no].base_width;
			int base_height = touchkey_list[key_layout_no].base_height;
			rect.x = (int) ((layout[i].x0 * dstrect->w) / base_width);
			rect.y = (int) ((layout[i].y0 * dstrect->h) / base_height);
			rect.w = (int) ((layout[i].sx * dstrect->w) / base_width);
			rect.h = (int) ((layout[i].sy * dstrect->h) / base_height);

			rect.x += dstrect->x;
			rect.y += dstrect->y;

			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, alpha);

			SDL_RenderCopy(renderer, texture, NULL, &rect);
		}
	}

	keydown_state_changed = FALSE;
}

void sdl2_touchkey_draw(void)
{
	SDL_Rect rect;

	if (enable_touchkey == FALSE) {
		return;
	}

	if ((separate_touchkey) &&
		(key_show) &&
		(keydown_state_changed)) {

		SDL_RenderClear(key_renderer);

		rect.x = 0;
		rect.y = 0;
		rect.w = key_width;
		rect.h = key_height;
		sdl2_touchkey_rendering(1, key_renderer, &rect, TRUE, 255);

		SDL_RenderPresent(key_renderer);
	}
}

void touchkey_cfg_overlap(int delta)
{
	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {
			if (touchkey_fit) {
				int next = touchkey_overlap + delta;
				if (next < 0) {
					next = 0;
				} else if (next > 100) {
					next = 100;
				}
				if (touchkey_overlap != next) {
					touchkey_overlap = next;
					quasi88_cfg_reset_window();
				}
			}
		}
	}
}

void touchkey_cfg_recude(int delta)
{
	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {
			if (touchkey_fit) {
				int next = touchkey_reduce + delta;
				if (next < 0) {
					next = 0;
				} else if (next > 50) {
					next = 50;
				}
				if (touchkey_reduce != next) {
					touchkey_reduce = next;
					quasi88_cfg_reset_window();
				}
			}
		}
	}
}

void touchkey_cfg_fit(int do_fit)
{
	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {
			if (touchkey_fit != do_fit) {
				touchkey_fit = do_fit;
				quasi88_cfg_reset_window();
			}
		}
	}
}

void touchkey_cfg_orientation(Uint32 display)
{
	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {
			SDL_Window *w = SDL_GetWindowFromID(sdl_window_id);
			if (SDL_GetWindowDisplayIndex(w) == display) {
				quasi88_cfg_reset_window();
			}
		}
	}
}

void touchkey_cfg_opaque(int delta)
{
	if (enable_touchkey) {
		if ((sdl_fullscreen) && (sdl_buildin_touchkey)) {
			if (touchkey_fit) {
				int next = touchkey_opaque + delta;
				if (next < 0) {
					next = 0;
				} else if (next > 255) {
					next = 255;
				}
				if (touchkey_opaque != next) {
					touchkey_opaque = next;
					keydown_state_changed = TRUE;
				}
			}
		}
	}
}










/****************************************************************************
 *
 *      ユーティリティ
 *
 *****************************************************************************/

/* タッチキートップの文字列を int 値に変換するテーブル */

static const T_SYMBOL_TABLE keytopval_list[] = {
	{ "KEYTOP_STOP"			,	KEYTOP_STOP			, },
	{ "KEYTOP_COPY"			,	KEYTOP_COPY			, },
	{ "KEYTOP_F1"			,	KEYTOP_F1			, },
	{ "KEYTOP_F2"			,	KEYTOP_F2			, },
	{ "KEYTOP_F3"			,	KEYTOP_F3			, },
	{ "KEYTOP_F4"			,	KEYTOP_F4			, },
	{ "KEYTOP_F5"			,	KEYTOP_F5			, },
	{ "KEYTOP_F6"			,	KEYTOP_F6			, },
	{ "KEYTOP_F7"			,	KEYTOP_F7			, },
	{ "KEYTOP_F8"			,	KEYTOP_F8			, },
	{ "KEYTOP_F9"			,	KEYTOP_F9			, },
	{ "KEYTOP_F10"			,	KEYTOP_F10			, },
	{ "KEYTOP_ROLLUP"		,	KEYTOP_ROLLUP		, },
	{ "KEYTOP_ROLLDOWN"		,	KEYTOP_ROLLDOWN		, },
	{ "KEYTOP_ESC"			,	KEYTOP_ESC			, },
	{ "KEYTOP_1"			,	KEYTOP_1			, },
	{ "KEYTOP_2"			,	KEYTOP_2			, },
	{ "KEYTOP_3"			,	KEYTOP_3			, },
	{ "KEYTOP_4"			,	KEYTOP_4			, },
	{ "KEYTOP_5"			,	KEYTOP_5			, },
	{ "KEYTOP_6"			,	KEYTOP_6			, },
	{ "KEYTOP_7"			,	KEYTOP_7			, },
	{ "KEYTOP_8"			,	KEYTOP_8			, },
	{ "KEYTOP_9"			,	KEYTOP_9			, },
	{ "KEYTOP_0"			,	KEYTOP_0			, },
	{ "KEYTOP_MINUS"		,	KEYTOP_MINUS		, },
	{ "KEYTOP_CARET"		,	KEYTOP_CARET		, },
	{ "KEYTOP_YEN"			,	KEYTOP_YEN			, },
	{ "KEYTOP_BS"			,	KEYTOP_BS			, },
	{ "KEYTOP_INS"			,	KEYTOP_INS			, },
	{ "KEYTOP_DEL"			,	KEYTOP_DEL			, },
	{ "KEYTOP_HOME"			,	KEYTOP_HOME			, },
	{ "KEYTOP_HELP"			,	KEYTOP_HELP			, },
	{ "KEYTOP_KP_SUB"		,	KEYTOP_KP_SUB		, },
	{ "KEYTOP_KP_DIVIDE"	,	KEYTOP_KP_DIVIDE	, },
	{ "KEYTOP_TAB"			,	KEYTOP_TAB			, },
	{ "KEYTOP_Q"			,	KEYTOP_Q			, },
	{ "KEYTOP_W"			,	KEYTOP_W			, },
	{ "KEYTOP_E"			,	KEYTOP_E			, },
	{ "KEYTOP_R"			,	KEYTOP_R			, },
	{ "KEYTOP_T"			,	KEYTOP_T			, },
	{ "KEYTOP_Y"			,	KEYTOP_Y			, },
	{ "KEYTOP_U"			,	KEYTOP_U			, },
	{ "KEYTOP_I"			,	KEYTOP_I			, },
	{ "KEYTOP_O"			,	KEYTOP_O			, },
	{ "KEYTOP_P"			,	KEYTOP_P			, },
	{ "KEYTOP_AT"			,	KEYTOP_AT			, },
	{ "KEYTOP_BRACKETLEFT"	,	KEYTOP_BRACKETLEFT	, },
	{ "KEYTOP_RETURNL"		,	KEYTOP_RETURNL		, },
	{ "KEYTOP_KP_7"			,	KEYTOP_KP_7			, },
	{ "KEYTOP_KP_8"			,	KEYTOP_KP_8			, },
	{ "KEYTOP_KP_9"			,	KEYTOP_KP_9			, },
	{ "KEYTOP_KP_MULTIPLY"	,	KEYTOP_KP_MULTIPLY	, },
	{ "KEYTOP_CTRL"			,	KEYTOP_CTRL			, },
	{ "KEYTOP_CAPS"			,	KEYTOP_CAPS			, },
	{ "KEYTOP_A"			,	KEYTOP_A			, },
	{ "KEYTOP_S"			,	KEYTOP_S			, },
	{ "KEYTOP_D"			,	KEYTOP_D			, },
	{ "KEYTOP_F"			,	KEYTOP_F			, },
	{ "KEYTOP_G"			,	KEYTOP_G			, },
	{ "KEYTOP_H"			,	KEYTOP_H			, },
	{ "KEYTOP_J"			,	KEYTOP_J			, },
	{ "KEYTOP_K"			,	KEYTOP_K			, },
	{ "KEYTOP_L"			,	KEYTOP_L			, },
	{ "KEYTOP_SEMICOLON"	,	KEYTOP_SEMICOLON	, },
	{ "KEYTOP_COLON"		,	KEYTOP_COLON		, },
	{ "KEYTOP_BRACKETRIGHT"	,	KEYTOP_BRACKETRIGHT	, },
	{ "KEYTOP_UP"			,	KEYTOP_UP			, },
	{ "KEYTOP_KP_4"			,	KEYTOP_KP_4			, },
	{ "KEYTOP_KP_5"			,	KEYTOP_KP_5			, },
	{ "KEYTOP_KP_6"			,	KEYTOP_KP_6			, },
	{ "KEYTOP_KP_ADD"		,	KEYTOP_KP_ADD		, },
	{ "KEYTOP_SHIFTL"		,	KEYTOP_SHIFTL		, },
	{ "KEYTOP_Z"			,	KEYTOP_Z			, },
	{ "KEYTOP_X"			,	KEYTOP_X			, },
	{ "KEYTOP_C"			,	KEYTOP_C			, },
	{ "KEYTOP_V"			,	KEYTOP_V			, },
	{ "KEYTOP_B"			,	KEYTOP_B			, },
	{ "KEYTOP_N"			,	KEYTOP_N			, },
	{ "KEYTOP_M"			,	KEYTOP_M			, },
	{ "KEYTOP_COMMA"		,	KEYTOP_COMMA		, },
	{ "KEYTOP_PERIOD"		,	KEYTOP_PERIOD		, },
	{ "KEYTOP_SLASH"		,	KEYTOP_SLASH		, },
	{ "KEYTOP_UNDERSCORE"	,	KEYTOP_UNDERSCORE	, },
	{ "KEYTOP_SHIFTR"		,	KEYTOP_SHIFTR		, },
	{ "KEYTOP_LEFT"			,	KEYTOP_LEFT			, },
	{ "KEYTOP_DOWN"			,	KEYTOP_DOWN			, },
	{ "KEYTOP_RIGHT"		,	KEYTOP_RIGHT		, },
	{ "KEYTOP_KP_1"			,	KEYTOP_KP_1			, },
	{ "KEYTOP_KP_2"			,	KEYTOP_KP_2			, },
	{ "KEYTOP_KP_3"			,	KEYTOP_KP_3			, },
	{ "KEYTOP_KP_EQUAL"		,	KEYTOP_KP_EQUAL		, },
	{ "KEYTOP_KANA"			,	KEYTOP_KANA			, },
	{ "KEYTOP_GRAPH"		,	KEYTOP_GRAPH		, },
	{ "KEYTOP_KETTEI"		,	KEYTOP_KETTEI		, },
	{ "KEYTOP_SPACE"		,	KEYTOP_SPACE		, },
	{ "KEYTOP_HENKAN"		,	KEYTOP_HENKAN		, },
	{ "KEYTOP_PC"			,	KEYTOP_PC			, },
	{ "KEYTOP_ZENKAKU"		,	KEYTOP_ZENKAKU		, },
	{ "KEYTOP_KP_0"			,	KEYTOP_KP_0			, },
	{ "KEYTOP_KP_COMMA"		,	KEYTOP_KP_COMMA		, },
	{ "KEYTOP_KP_PERIOD"	,	KEYTOP_KP_PERIOD	, },
	{ "KEYTOP_RETURNR"		,	KEYTOP_RETURNR		, },
	{ "KEYTOP_F1_OLD"		,	KEYTOP_F1_OLD		, },
	{ "KEYTOP_F2_OLD"		,	KEYTOP_F2_OLD		, },
	{ "KEYTOP_F3_OLD"		,	KEYTOP_F3_OLD		, },
	{ "KEYTOP_F4_OLD"		,	KEYTOP_F4_OLD		, },
	{ "KEYTOP_F5_OLD"		,	KEYTOP_F5_OLD		, },
	{ "KEYTOP_SPACE_OLD"	,	KEYTOP_SPACE_OLD	, },
	{ "KEYTOP_SHIFTL_OLD"	,	KEYTOP_SHIFTL_OLD	, },
	{ "KEYTOP_SHIFTR_OLD"	,	KEYTOP_SHIFTR_OLD	, },
	{ "KEYTOP_RETURNL_OLD"	,	KEYTOP_RETURNL_OLD	, },
	{ "KEYTOP_RETURNR_OLD"	,	KEYTOP_RETURNR_OLD	, },
	{ "KEYTOP_INSDEL"		,	KEYTOP_INSDEL		, },
	{ "KEYTOP_2_4"			,	KEYTOP_2_4			, },
	{ "KEYTOP_2_6"			,	KEYTOP_2_6			, },
	{ "KEYTOP_8_4"			,	KEYTOP_8_4			, },
	{ "KEYTOP_8_6"			,	KEYTOP_8_6			, },
	{ "KEYTOP_ESC_PAD"		,	KEYTOP_ESC_PAD		, },
	{ "KEYTOP_SHIFT_PAD"	,	KEYTOP_SHIFT_PAD	, },
	{ "KEYTOP_SPACE_PAD"	,	KEYTOP_SPACE_PAD	, },
	{ "KEYTOP_RETURN_PAD"	,	KEYTOP_RETURN_PAD	, },
	{ "KEYTOP_Z_PAD"		,	KEYTOP_Z_PAD		, },
	{ "KEYTOP_X_PAD"		,	KEYTOP_X_PAD		, },
	{ "KEYTOP_SHIFT_MINI"	,	KEYTOP_SHIFT_MINI	, },
	{ "KEYTOP_SHIFTL_MINI"	,	KEYTOP_SHIFTL_MINI	, },
	{ "KEYTOP_SHIFTR_MINI"	,	KEYTOP_SHIFTR_MINI	, },
	{ "KEYTOP_ROMAJI"		,	KEYTOP_ROMAJI		, },
	{ "KEYTOP_NUM"			,	KEYTOP_NUM			, },
	{ "KEYTOP_F1_MINI"		,	KEYTOP_F1_MINI		, },
	{ "KEYTOP_F2_MINI"		,	KEYTOP_F2_MINI		, },
	{ "KEYTOP_F3_MINI"		,	KEYTOP_F3_MINI		, },
	{ "KEYTOP_F4_MINI"		,	KEYTOP_F4_MINI		, },
	{ "KEYTOP_F5_MINI"		,	KEYTOP_F5_MINI		, },
	{ "KEYTOP_F6_MINI"		,	KEYTOP_F6_MINI		, },
	{ "KEYTOP_F7_MINI"		,	KEYTOP_F7_MINI		, },
	{ "KEYTOP_F8_MINI"		,	KEYTOP_F8_MINI		, },
	{ "KEYTOP_F9_MINI"		,	KEYTOP_F9_MINI		, },
	{ "KEYTOP_F10_MINI"		,	KEYTOP_F10_MINI		, },
	{ "KEYTOP_ESC_MINI"		,	KEYTOP_ESC_MINI		, },
	{ "KEYTOP_TAB_MINI"		,	KEYTOP_TAB_MINI		, },
	{ "KEYTOP_BS_MINI"		,	KEYTOP_BS_MINI		, },
	{ "KEYTOP_INSDEL_MINI"	,	KEYTOP_INSDEL_MINI	, },
	{ "KEYTOP_SPACE_MINI"	,	KEYTOP_SPACE_MINI	, },
	{ "KEYTOP_KETTEI_MINI"	,	KEYTOP_KETTEI_MINI	, },
	{ "KEYTOP_HENKAN_MINI"	,	KEYTOP_HENKAN_MINI	, },
	{ "KEYTOP_RETURN_MINI"	,	KEYTOP_RETURN_MINI	, },
	{ "KEYTOP_RETURNL_MINI"	,	KEYTOP_RETURNL_MINI	, },
	{ "KEYTOP_RETURNR_MINI"	,	KEYTOP_RETURNR_MINI	, },
	{ "KEYTOP_F6_FUNC"		,	KEYTOP_F6_FUNC		, },
	{ "KEYTOP_F7_FUNC"		,	KEYTOP_F7_FUNC		, },
	{ "KEYTOP_F8_FUNC"		,	KEYTOP_F8_FUNC		, },
	{ "KEYTOP_F9_FUNC"		,	KEYTOP_F9_FUNC		, },
	{ "KEYTOP_F10_FUNC"		,	KEYTOP_F10_FUNC		, },
	{ "KEYTOP_PREV"			,	KEYTOP_PREV			, },
	{ "KEYTOP_NEXT"			,	KEYTOP_NEXT			, },
};

/***********************************************************************
 * 与えられた文字列を、タッチキートップの番号に変換するQUASI88 キーコードに変換する
 ************************************************************************/

static int touchkey_str2keytopnum(const char *str)
{
	int i;

	/* 定義文字列に合致するのを探す */
	for (i = 0; i < COUNTOF(keytopval_list); i++) {
		if (strcmp(keytopval_list[i].name, str) == 0) {
			return keytopval_list[i].val;
		}
	}
	for (i = 0; i < COUNTOF(keytopval_list); i++) {
		if (my_strcmp(keytopval_list[i].name, str) == 0) {
			return keytopval_list[i].val;
		}
	}

	return -1;
}


/****************************************************************************
 * キー設定ファイルを読み込んで、設定を行う。
 *****************************************************************************/
#include <file-op.h>

/* キー設定ファイル1行あたりの最大文字数 */
#define MAX_KEYFILE_LINE		(256)

#define MAX_LAYOUT_WIDTH		(1280)
#define MAX_LAYOUT_HEIGHT		(800)

static int config_read_touchkeyconf_file(const char *filename)
{
	int i;
	OSD_FILE *fp = NULL;
	T_TOUCHKEY_LIST *touchkey_list_p = NULL;
	int  line_cnt = 0;
	char line[ MAX_KEYFILE_LINE ];
	char buff[ MAX_KEYFILE_LINE ];
	int base_width = -1;
	int base_height = -1;
	int offset_x = 0;
	int offset_y = 0;

	/* キー設定ファイルを開く */
	if (filename) {
		fp = osd_fopen(FTYPE_CFG, filename, "r");
		if (verbose_proc) {
			if (fp) {
				printf("(\"%s\" read and initialize ..", filename);
			} else {
				printf("can't open touchkey configuration file \"%s\"\n",
					   filename);
			}
		}
	}
	if (fp == NULL) {
		/* キー設定ファイルが開けなかったら偽を返す */
		return FALSE;
	}

	/* キー設定ファイルを1行ずつ解析 */
	while (osd_fgets(line, MAX_KEYFILE_LINE, fp)) {
		long l;
		char *conv_end;
		int effective = FALSE;
		int nr_token = 0;
		char *token[8];

		line_cnt ++;

		/* 行の内容をトークンに分解。各トークンは、token[] にセット */
		for (i = 0; i < COUNTOF(token); i++) {
			token[i] = NULL;
		}
		{
			char *str = line;
			char *b = &buff[0];
			for (nr_token = 0; nr_token < COUNTOF(token); nr_token++) {
				str = my_strtok(b, str);
				if (str) {
					token[nr_token] = b;
					b += strlen(b) + 1;
				} else {
					break;
				}
			}
		}

		/* トークンがなければ次の行へ */
		if (nr_token == 0) {
			continue;
		}

		if (token[0][0] == '[') {
			/* 1個目のトークンが「識別タグ」行の場合の処理 */
			T_LAYOUT *layout = NULL;

			base_width = -1;
			base_height = -1;
			offset_x = 0;
			offset_y = 0;

			if ((nr_token >= 2) &&
				((my_strcmp(token[1], "TRUE") == 0) ||
				 (my_strcmp(token[1], "1") == 0))) {

				if ((nr_touchkey_list < MAX_TOUCHKEY_LIST) &&
					(nr_token >= 4)) {

					l = strtoul(token[2], &conv_end, 0);
					if ((*conv_end == '\0') &&
						(l <= MAX_LAYOUT_WIDTH)) {
						base_width = (int) l;
					}

					l = strtoul(token[3], &conv_end, 0);
					if ((*conv_end == '\0') &&
						(l <= MAX_LAYOUT_HEIGHT)) {
						base_height = (int) l;
					}

					if (nr_token >= 5) {
						l = strtol(token[4], &conv_end, 0);
						if ((*conv_end == '\0') &&
							(ABS(l) < MAX_LAYOUT_WIDTH / 2)) {
							offset_x = l;
						} else {
							offset_x = MAX_LAYOUT_WIDTH; /* 範囲外 */
						}
					}

					if (nr_token >= 6) {
						l = strtol(token[5], &conv_end, 0);
						if ((*conv_end == '\0') &&
							(ABS(l) < MAX_LAYOUT_HEIGHT / 2)) {
							offset_y = l;
						} else {
							offset_y = MAX_LAYOUT_HEIGHT; /* 範囲外 */
						}
					}

					layout = (T_LAYOUT *) calloc(sizeof(T_LAYOUT), NR_KEYNO);
				}

				if (nr_touchkey_list >= MAX_TOUCHKEY_LIST) {
					fprintf(stderr,
							"\nwarning: too many tags in line %d (ignored)", line_cnt);
				} else if (nr_token < 4 ||
						   base_width < 0 ||
						   base_height < 0 ||
						   offset_x >= MAX_LAYOUT_WIDTH ||
						   offset_y >= MAX_LAYOUT_HEIGHT) {
					fprintf(stderr,
							"\nwarning: error in line %d (ignored)\n", line_cnt);
				} else if (layout == NULL) {
					fprintf(stderr,
							"\nwarning: out of memory in line %d (ignored)\n", line_cnt);
				} else {
					effective = TRUE;
				}
			}

			if (effective) {
				/* 有効な識別タグだった */
				touchkey_list_p = &touchkey_list[ nr_touchkey_list ];

				touchkey_list_p->layout = layout;
				touchkey_list_p->sz_array = 0;
				touchkey_list_p->base_width = base_width;
				touchkey_list_p->base_height = base_height;
				touchkey_list_p->scale_w = 100;
				touchkey_list_p->scale_h = 100;
				touchkey_list_p->r = 0xbf;
				touchkey_list_p->g = 0xbf;
				touchkey_list_p->b = 0xbf;

				nr_touchkey_list ++;

			} else {
				/* 無効な識別タグだった */
				touchkey_list_p = NULL;
			}

		} else {
			/* 1個目のトークンが「識別タグ」行でない場合の処理 */

			if (touchkey_list_p) {
				int keytop_no = -1;
				int lock_type = -1;
				float cx = -1.0f;
				float cy = -1.0f;

				if ((nr_token == 4) &&
					(touchkey_list_p->sz_array < NR_KEYNO - 1)) {

					keytop_no = touchkey_str2keytopnum(token[0]);

					if (((my_strcmp(token[1], "NO") == 0) ||
						 (my_strcmp(token[1], "0") == 0))) {
						lock_type = LOCK_TYPE_NONE;

					} else if (((my_strcmp(token[1], "SINGLE") == 0) ||
								(my_strcmp(token[1], "1") == 0))) {
						lock_type = LOCK_TYPE_SINGLE;

					} else if (((my_strcmp(token[1], "DOUBLE") == 0) ||
								(my_strcmp(token[1], "2") == 0))) {
						lock_type = LOCK_TYPE_DOUBLE;
					}

					l = strtol(token[2], &conv_end, 0);
					if (*conv_end == '\0') {
						if (l < 0) {
							l = base_width + l;
						}
						l = l + offset_x;
						if (BETWEEN(0, l, base_width)) {
							cx = (float) l;
						}
					}

					l = strtol(token[3], &conv_end, 0);
					if (*conv_end == '\0') {
						if (l < 0) {
							l = base_height + l;
						}
						l = l + offset_y;
						if (BETWEEN(0, l, base_height)) {
							cy = (float) l;
						}
					}
				}

				if (touchkey_list_p->sz_array >= NR_KEYNO - 1) {
					fprintf(stderr,
							"\nwarning: too many keytop in line %d (ignored)\n", line_cnt);
				} else if ((nr_token != 4) ||
						   (keytop_no < 0) ||
						   (lock_type < 0) ||
						   (cx < 0) ||
						   (cy < 0)) {
					fprintf(stderr,
							"\nwarning: error in line %d (ignored)\n", line_cnt);
				} else {
					effective = TRUE;
				}

				if (effective) {
					float w = keytop_spec[keytop_no].width;
					float h = keytop_spec[keytop_no].height;
					T_LAYOUT *p
						= &touchkey_list_p->layout[touchkey_list_p->sz_array];

					p->keytop_no = (unsigned char) keytop_no;
					p->lock_type = (unsigned char) lock_type;
					for (i = 0; i < NR_KEYCOMBO; i++) {
						p->code[i] = keytop_spec[keytop_no].code[i];
					}

					p->x0 = cx - (w / 2);
					p->y0 = cy - (h / 2);
					p->x1 = cx + (w / 2);
					p->y1 = cy + (h / 2);
					p->sx = w;
					p->sy = h;

					touchkey_list_p->sz_array ++;
				}
			}
		}
	}
	osd_fclose(fp);

	if (verbose_proc) {
		printf(".. OK (found %d layout(s)))\n", nr_touchkey_list);
	}

	if (nr_touchkey_list == 0) {
		/* レイアウトが一つもなければ偽を返す */
		return FALSE;
	} else {
		/* レイアウトが一つでもあれば真を返す */
		return TRUE;
	}
}
