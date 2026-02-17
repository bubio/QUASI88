/***********************************************************************
 * イベント処理 (システム依存)
 *
 *  詳細は、 event.h 参照
 ************************************************************************/

#include <SDL.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <string.h>

#include "quasi88.h"
#include "utility.h"
#include "device.h"

#include "event.h"
#include "keyboard.h"
#include <stdio.h>


#define KEY_PRINTF		(0)
#define IME_PRINTF		(0)
#define JOY_PRINTF		(0)
#define DND_PRINTF		(0)

#define KEYPRINTF(x)	if (KEY_PRINTF) printf x
#define IMEPRINTF(x)	if (IME_PRINTF) printf x
#define JOYPRINTF(x)	if (JOY_PRINTF) printf x
#define DNDPRINTF(x)	if (DND_PRINTF) printf x

/************************************************************************/

int use_menukey = FALSE;				/* 左Win/左Cmdキーでメニューへ遷移  */

int keyboard_type = 1;					/* キーボードの種類                 */
char *file_keyboard = NULL;				/* キー設定ファイル名               */

int use_joydevice = TRUE;				/* ジョイスティックデバイスを開く?  */

int use_ime = FALSE;					/* メニューでIMEを使えるようにする  */
static int now_ime = FALSE;
static int now_ime_input = FALSE;

static int now_drop = FALSE;
static int now_finger_mode = FALSE;
static SDL_FingerID finger_id;
static int          finger_x;
static int          finger_y;


#define AXIS_U			0x01
#define AXIS_D			0x02
#define AXIS_L			0x04
#define AXIS_R			0x08

typedef struct {

	SDL_Joystick *dev;				/* ジョイスティックの識別子(NULLなら無効)*/
	SDL_JoystickID id;				/* ジョイスティックのインスタンスID      */
	int axis;						/* ジョイスティックの方向ボタン押下状態  */

} T_JOY_INFO;

static T_JOY_INFO joy_info[KEY88_PAD_MAX];



static const char *debug_table(int scancode);
/*==========================================================================
 * キー配列について
 *
 *  ・一般キー(文字キー) は、PC-8801 の物理的配列にできるだけあわせよう
 *      jp106キーボードはそのままの配置を使う。
 *      us101キーボードはjp106の配列に読み替えて、足りないキーは置き換える。
 *          = → ^ 、 [ → @ 、 ] → [ 、 \ → ] 、 ' → :
 *          ` → \ 、 右CTRL → _
 *  ・特殊キー(機能キー) は、PC-8801 の特殊キーを入力できるようにしよう
 *      キー刻印にあわせる
 *          BackSpace → BS 、 Home → HOME CLR など
 *      雰囲気であわせる
 *          End → HELP 、 PrintScreen → COPY など
 *      Altに割り当てる
 *          左Alt → GRAPH 、 右Alt → カナ (ロックできないけど)
 *      jp106キーボードの言語入力キーは、PC-8801の日本語入力キーに割り当てる
 *          無変換 → 決定、 変換 → 変換、 アプリキー → 全角
 *      Winキーは、メニューモードの切替に割り当てる
 *      PC-8801のPCキーは割り当て無しで…
 *  ・テンキーは、そのままの使おう (, などの無いキーはあきらめる)
 *  ・メニューモードでは、キー刻印どおりのほうがいいかもしれない
 *
 * SDL2のキー入力について
 *      SCANCODE は SDL_NUM_SCANCODES (511) 未満の値だが、
 *      KEYSYM (KEYCODE) は int の範囲内の値をとる
 *      なので、 SCANCODE から PC-8801キーへの変換テーブルを使うことにする。
 *      なお、SCANCODE と KEYSYM  は、1対1に対応しているので、
 *      KEYSYM は一切使わないことにする。(使わなくても、なんとかなるはず…)
 *
 *      全角／半角キー、CapsLockキー、ひらがなカタカナキーは、
 *      環境によってはおかしな動作をするので、使わない。
 *      NumLock も使わないでおこう。
 *
 *===========================================================================*/

/*----------------------------------------------------------------------
 * SDL の scancode を QUASI88 の キーコードに変換するテーブル
 *
 *  スキャンコード SDL_SCANCODE_xxx が押されたら、
 *  scancode2key88[ SDL_SCANCODE_xxx ][0] が押されたとする。
 *  (シフトキー併用なら scancode2key88[ SDL_SCANCODE_xxx ][1])
 *
 *  scancode2key88[][] には、 KEY88_xxx をセットしておく。
 *  初期値は scancode2key88_default[][] と同じ
 *----------------------------------------------------------------------*/
static int scancode2key88[SDL_NUM_SCANCODES][2];



/*----------------------------------------------------------------------
 * キーボードレイアウト変更時の キーコード変換情報
 *
 *  binding[].code (SDL_SCANCODE_xxx) が押されたら、
 *  binding[].new_key88 (KEY88_xxx) が押されたことにする。
 *
 *  キーボードレイアウト変更時は、この情報にしたがって、
 *  scancode2key88[] を書き換える。
 *  変更できるキーの個数は、64個まで (これだけあればいいだろう)
 *  ** 現在、未使用 **
 *----------------------------------------------------------------------*/
typedef struct {
	int type;							/* KEYCODE_INVALID / SYM / SCAN */
	int code;							/* スキャンコード */
	int new_key88[2];					/* 変更時の QUASI88キーコード */
	int org_key88[2];					/* 復元時の QUASI88キーコード */
} T_BINDING;

static T_BINDING binding[64];



/*----------------------------------------------------------------------
 * SDL_SCANCODE_xxx → KEY88_xxx 変換テーブル (デフォルト)
 *
 *  SDL2 は、 us101 キーボードを前提にしたスキャンコード名称を定義しているが、
 *  ここでは jp106 キーボードにあわせた変換テーブルを用意する
 *----------------------------------------------------------------------*/

static const int scancode2key88_default[SDL_NUM_SCANCODES][2] = {

	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	KEY88_a,			KEY88_A,			},	/*	SDL_SCANCODE_A = 4,						*/
	{	KEY88_b,			KEY88_B,			},	/*	SDL_SCANCODE_B = 5,						*/
	{	KEY88_c,			KEY88_C,			},	/*	SDL_SCANCODE_C = 6,						*/
	{	KEY88_d,			KEY88_D,			},	/*	SDL_SCANCODE_D = 7,						*/
	{	KEY88_e,			KEY88_E,			},	/*	SDL_SCANCODE_E = 8,						*/
	{	KEY88_f,			KEY88_F,			},	/*	SDL_SCANCODE_F = 9,						*/
	{	KEY88_g,			KEY88_G,			},	/*	SDL_SCANCODE_G = 10,					*/
	{	KEY88_h,			KEY88_H,			},	/*	SDL_SCANCODE_H = 11,					*/
	{	KEY88_i,			KEY88_I,			},	/*	SDL_SCANCODE_I = 12,					*/
	{	KEY88_j,			KEY88_J,			},	/*	SDL_SCANCODE_J = 13,					*/
	{	KEY88_k,			KEY88_K,			},	/*	SDL_SCANCODE_K = 14,					*/
	{	KEY88_l,			KEY88_L,			},	/*	SDL_SCANCODE_L = 15,					*/
	{	KEY88_m,			KEY88_M,			},	/*	SDL_SCANCODE_M = 16,					*/
	{	KEY88_n,			KEY88_N,			},	/*	SDL_SCANCODE_N = 17,					*/
	{	KEY88_o,			KEY88_O,			},	/*	SDL_SCANCODE_O = 18,					*/
	{	KEY88_p,			KEY88_P,			},	/*	SDL_SCANCODE_P = 19,					*/
	{	KEY88_q,			KEY88_Q,			},	/*	SDL_SCANCODE_Q = 20,					*/
	{	KEY88_r,			KEY88_R,			},	/*	SDL_SCANCODE_R = 21,					*/
	{	KEY88_s,			KEY88_S,			},	/*	SDL_SCANCODE_S = 22,					*/
	{	KEY88_t,			KEY88_T,			},	/*	SDL_SCANCODE_T = 23,					*/
	{	KEY88_u,			KEY88_U,			},	/*	SDL_SCANCODE_U = 24,					*/
	{	KEY88_v,			KEY88_V,			},	/*	SDL_SCANCODE_V = 25,					*/
	{	KEY88_w,			KEY88_W,			},	/*	SDL_SCANCODE_W = 26,					*/
	{	KEY88_x,			KEY88_X,			},	/*	SDL_SCANCODE_X = 27,					*/
	{	KEY88_y,			KEY88_Y,			},	/*	SDL_SCANCODE_Y = 28,					*/
	{	KEY88_z,			KEY88_Z,			},	/*	SDL_SCANCODE_Z = 29,					*/
	{	KEY88_1,			KEY88_EXCLAM,		},	/*	SDL_SCANCODE_1 = 30,					*/
	{	KEY88_2,			KEY88_QUOTEDBL,		},	/*	SDL_SCANCODE_2 = 31,					*/
	{	KEY88_3,			KEY88_NUMBERSIGN,	},	/*	SDL_SCANCODE_3 = 32,					*/
	{	KEY88_4,			KEY88_DOLLAR,		},	/*	SDL_SCANCODE_4 = 33,					*/
	{	KEY88_5,			KEY88_PERCENT,		},	/*	SDL_SCANCODE_5 = 34,					*/
	{	KEY88_6,			KEY88_AMPERSAND,	},	/*	SDL_SCANCODE_6 = 35,					*/
	{	KEY88_7,			KEY88_APOSTROPHE,	},	/*	SDL_SCANCODE_7 = 36,					*/
	{	KEY88_8,			KEY88_PARENLEFT,	},	/*	SDL_SCANCODE_8 = 37,					*/
	{	KEY88_9,			KEY88_PARENRIGHT,	},	/*	SDL_SCANCODE_9 = 38,					*/
	{	KEY88_0,			0,					},	/*	SDL_SCANCODE_0 = 39,					*/
	{	KEY88_RETURNL,		0,					},	/*	SDL_SCANCODE_RETURN = 40,				*/
	{	KEY88_ESC,			0,					},	/*	SDL_SCANCODE_ESCAPE = 41,				*/
	{	KEY88_INS_DEL,		0,					},	/*	SDL_SCANCODE_BACKSPACE = 42,			*/
	{	KEY88_TAB,			0,					},	/*	SDL_SCANCODE_TAB = 43,					*/
	{	KEY88_SPACE,		0,					},	/*	SDL_SCANCODE_SPACE = 44,				*/
	{	KEY88_MINUS,		KEY88_EQUAL,		},	/*	SDL_SCANCODE_MINUS = 45,				*/
	{	KEY88_CARET,		KEY88_TILDE,		},	/*	SDL_SCANCODE_EQUALS = 46,				*/
	{	KEY88_AT,			KEY88_BACKQUOTE,	},	/*	SDL_SCANCODE_LEFTBRACKET = 47,			*/
	{	KEY88_BRACKETLEFT,	KEY88_BRACELEFT,	},	/*	SDL_SCANCODE_RIGHTBRACKET = 48,			*/
	{	KEY88_BRACKETRIGHT,	KEY88_BRACERIGHT,	},	/*	SDL_SCANCODE_BACKSLASH = 49,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_NONUSHASH = 50,			*/
	{	KEY88_SEMICOLON,	KEY88_PLUS,			},	/*	SDL_SCANCODE_SEMICOLON = 51,			*/
	{	KEY88_COLON,		KEY88_ASTERISK,		},	/*	SDL_SCANCODE_APOSTROPHE = 52,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_GRAVE = 53,				*/	/* 半角全角 */
	{	KEY88_COMMA,		KEY88_LESS,			},	/*	SDL_SCANCODE_COMMA = 54,				*/
	{	KEY88_PERIOD,		KEY88_GREATER,		},	/*	SDL_SCANCODE_PERIOD = 55,				*/
	{	KEY88_SLASH,		KEY88_QUESTION,		},	/*	SDL_SCANCODE_SLASH = 56,				*/
	{	KEY88_CAPS,			0,					},	/*	SDL_SCANCODE_CAPSLOCK = 57,				*/
	{	KEY88_F1,			0,					},	/*	SDL_SCANCODE_F1 = 58,					*/
	{	KEY88_F2,			0,					},	/*	SDL_SCANCODE_F2 = 59,					*/
	{	KEY88_F3,			0,					},	/*	SDL_SCANCODE_F3 = 60,					*/
	{	KEY88_F4,			0,					},	/*	SDL_SCANCODE_F4 = 61,					*/
	{	KEY88_F5,			0,					},	/*	SDL_SCANCODE_F5 = 62,					*/
	{	KEY88_F6,			0,					},	/*	SDL_SCANCODE_F6 = 63,					*/
	{	KEY88_F7,			0,					},	/*	SDL_SCANCODE_F7 = 64,					*/
	{	KEY88_F8,			0,					},	/*	SDL_SCANCODE_F8 = 65,					*/
	{	KEY88_F9,			0,					},	/*	SDL_SCANCODE_F9 = 66,					*/
	{	KEY88_F10,			0,					},	/*	SDL_SCANCODE_F10 = 67,					*/
	{	KEY88_SYS_STATUS,	0,					},	/*	SDL_SCANCODE_F11 = 68,					*/
	{	KEY88_SYS_MENU,		0,					},	/*	SDL_SCANCODE_F12 = 69,					*/
	{	KEY88_COPY,			0,					},	/*	SDL_SCANCODE_PRINTSCREEN = 70,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_SCROLLLOCK = 71,			*/
	{	KEY88_STOP,			0,					},	/*	SDL_SCANCODE_PAUSE = 72,				*/
	{	KEY88_INS,			0,					},	/*	SDL_SCANCODE_INSERT = 73,				*/
	{	KEY88_HOME,			0,					},	/*	SDL_SCANCODE_HOME = 74,					*/
	{	KEY88_ROLLDOWN,		0,					},	/*	SDL_SCANCODE_PAGEUP = 75,				*/
	{	KEY88_DEL,			0,					},	/*	SDL_SCANCODE_DELETE = 76,				*/
	{	KEY88_HELP,			0,					},	/*	SDL_SCANCODE_END = 77,					*/
	{	KEY88_ROLLUP,		0,					},	/*	SDL_SCANCODE_PAGEDOWN = 78,				*/
	{	KEY88_RIGHT,		0,					},	/*	SDL_SCANCODE_RIGHT = 79,				*/
	{	KEY88_LEFT,			0,					},	/*	SDL_SCANCODE_LEFT = 80,					*/
	{	KEY88_DOWN,			0,					},	/*	SDL_SCANCODE_DOWN = 81,					*/
	{	KEY88_UP,			0,					},	/*	SDL_SCANCODE_UP = 82,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_NUMLOCKCLEAR = 83,			*/
	{	KEY88_KP_DIVIDE,	0,					},	/*	SDL_SCANCODE_KP_DIVIDE = 84,			*/
	{	KEY88_KP_MULTIPLY,	0,					},	/*	SDL_SCANCODE_KP_MULTIPLY = 85,			*/
	{	KEY88_KP_SUB,		0,					},	/*	SDL_SCANCODE_KP_MINUS = 86,				*/
	{	KEY88_KP_ADD,		0,					},	/*	SDL_SCANCODE_KP_PLUS = 87,				*/
	{	KEY88_RETURNR,		0,					},	/*	SDL_SCANCODE_KP_ENTER = 88,				*/
	{	KEY88_KP_1,			0,					},	/*	SDL_SCANCODE_KP_1 = 89,					*/
	{	KEY88_KP_2,			0,					},	/*	SDL_SCANCODE_KP_2 = 90,					*/
	{	KEY88_KP_3,			0,					},	/*	SDL_SCANCODE_KP_3 = 91,					*/
	{	KEY88_KP_4,			0,					},	/*	SDL_SCANCODE_KP_4 = 92,					*/
	{	KEY88_KP_5,			0,					},	/*	SDL_SCANCODE_KP_5 = 93,					*/
	{	KEY88_KP_6,			0,					},	/*	SDL_SCANCODE_KP_6 = 94,					*/
	{	KEY88_KP_7,			0,					},	/*	SDL_SCANCODE_KP_7 = 95,					*/
	{	KEY88_KP_8,			0,					},	/*	SDL_SCANCODE_KP_8 = 96,					*/
	{	KEY88_KP_9,			0,					},	/*	SDL_SCANCODE_KP_9 = 97,					*/
	{	KEY88_KP_0,			0,					},	/*	SDL_SCANCODE_KP_0 = 98,					*/
	{	KEY88_KP_PERIOD,	0,					},	/*	SDL_SCANCODE_KP_PERIOD = 99,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_NONUSBACKSLASH = 100,		*/	/* ロ */
	{	0,					0,					},	/*	SDL_SCANCODE_APPLICATION = 101,			*/	/* App */
	{	0,					0,					},	/*	SDL_SCANCODE_POWER = 102,				*/
	{	KEY88_KP_EQUAL,		0,					},	/*	SDL_SCANCODE_KP_EQUALS = 103,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_F13 = 104,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F14 = 105,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F15 = 106,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F16 = 107,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F17 = 108,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F18 = 109,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F19 = 110,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F20 = 111,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F21 = 112,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F22 = 113,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F23 = 114,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_F24 = 115,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_EXECUTE = 116,				*/
	{	KEY88_HELP,			0,					},	/*	SDL_SCANCODE_HELP = 117,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_MENU = 118,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_SELECT = 119,				*/
	{	KEY88_STOP,			0,					},	/*	SDL_SCANCODE_STOP = 120,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_AGAIN = 121,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_UNDO = 122,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_CUT = 123,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_COPY = 124,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_PASTE = 125,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_FIND = 126,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_MUTE = 127,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_VOLUMEUP = 128,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_VOLUMEDOWN = 129,			*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	KEY88_KP_COMMA,		0,					},	/*	SDL_SCANCODE_KP_COMMA = 133,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_EQUALSAS400 = 134,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL1 = 135,		*/	/* ロ */
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL2 = 136,		*/	/* ひらカタ */
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL3 = 137,		*/	/* ￥ */
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL4 = 138,		*/	/* 変換 */
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL5 = 139,		*/	/* 無変換 */
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL6 = 140,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL7 = 141,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL8 = 142,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_INTERNATIONAL9 = 143,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG1 = 144,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG2 = 145,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG3 = 146,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG4 = 147,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG5 = 148,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG6 = 149,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG7 = 150,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG8 = 151,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_LANG9 = 152,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_ALTERASE = 153,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_SYSREQ = 154,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_CANCEL = 155,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_CLEAR = 156,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_PRIOR = 157,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_RETURN2 = 158,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_SEPARATOR = 159,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_OUT = 160,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_OPER = 161,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_CLEARAGAIN = 162,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_CRSEL = 163,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_EXSEL = 164,				*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_00 = 176,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_000 = 177,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_THOUSANDSSEPARATOR = 178,	*/
	{	0,					0,					},	/*	SDL_SCANCODE_DECIMALSEPARATOR = 179,	*/
	{	0,					0,					},	/*	SDL_SCANCODE_CURRENCYUNIT = 180,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_CURRENCYSUBUNIT = 181,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_LEFTPAREN = 182,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_RIGHTPAREN = 183,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_LEFTBRACE = 184,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_RIGHTBRACE = 185,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_TAB = 186,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_BACKSPACE = 187,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_A = 188,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_B = 189,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_C = 190,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_D = 191,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_E = 192,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_F = 193,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_XOR = 194,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_POWER = 195,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_PERCENT = 196,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_LESS = 197,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_GREATER = 198,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_AMPERSAND = 199,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_DBLAMPERSAND = 200,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_VERTICALBAR = 201,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_DBLVERTICALBAR = 202,	*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_COLON = 203,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_HASH = 204,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_SPACE = 205,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_AT = 206,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_EXCLAM = 207,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMSTORE = 208,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMRECALL = 209,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMCLEAR = 210,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMADD = 211,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMSUBTRACT = 212,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMMULTIPLY = 213,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_MEMDIVIDE = 214,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_PLUSMINUS = 215,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_CLEAR = 216,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_CLEARENTRY = 217,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_BINARY = 218,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_OCTAL = 219,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_DECIMAL = 220,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_KP_HEXADECIMAL = 221,		*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	KEY88_CTRL,			0,					},	/*	SDL_SCANCODE_LCTRL = 224,				*/
	{	KEY88_SHIFTL,		0,					},	/*	SDL_SCANCODE_LSHIFT = 225,				*/
	{	KEY88_GRAPH,		0,					},	/*	SDL_SCANCODE_LALT = 226,				*/	/* 左Alt/Opt */
	{	0,					0,					},	/*	SDL_SCANCODE_LGUI = 227,				*/	/* 左Win/Cmd */
	{	KEY88_CTRL,			0,					},	/*	SDL_SCANCODE_RCTRL = 228,				*/
	{	KEY88_SHIFTR,		0,					},	/*	SDL_SCANCODE_RSHIFT = 229,				*/
	{	KEY88_KANA,			0,					},	/*	SDL_SCANCODE_RALT = 230,				*/	/* 右Alt/Opt */
	{	0,					0,					},	/*	SDL_SCANCODE_RGUI = 231,				*/	/* 右Win/Cmd */
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*											*/
	{	0,					0,					},	/*	SDL_SCANCODE_MODE = 257,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_AUDIONEXT = 258,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AUDIOPREV = 259,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AUDIOSTOP = 260,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AUDIOPLAY = 261,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AUDIOMUTE = 262,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_MEDIASELECT = 263,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_WWW = 264,					*/
	{	0,					0,					},	/*	SDL_SCANCODE_MAIL = 265,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_CALCULATOR = 266,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_COMPUTER = 267,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_SEARCH = 268,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_HOME = 269,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_BACK = 270,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_FORWARD = 271,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_STOP = 272,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_REFRESH = 273,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_AC_BOOKMARKS = 274,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_BRIGHTNESSDOWN = 275,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_BRIGHTNESSUP = 276,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_DISPLAYSWITCH = 277,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KBDILLUMTOGGLE = 278,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KBDILLUMDOWN = 279,		*/
	{	0,					0,					},	/*	SDL_SCANCODE_KBDILLUMUP = 280,			*/
	{	0,					0,					},	/*	SDL_SCANCODE_EJECT = 281,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_SLEEP = 282,				*/
	{	0,					0,					},	/*	SDL_SCANCODE_APP1 = 283,				*/
	{	0,					0,					},	/*  SDL_SCANCODE_APP2 = 284,				*/
};

/*----------------------------------------------------------------------
 * scancode2key88[] の初期値は、 scancode2key88_default[] と同じだが、
 * キーボードの種類に応じて一部を変更することにする。
 * 以下は、その変更の情報。
 *----------------------------------------------------------------------*/
typedef struct {
	int scancode;						/* SCANCODE                    */
	int key88[2];						/* 変更する QUASI88キーコード  */
} T_REMAPPING;



static const T_REMAPPING remapping_win_106[] = {

	{	SDL_SCANCODE_GRAVE,				{	0,					0,			},	},	/* 半角全角 (不安定) */
	{	SDL_SCANCODE_SCROLLLOCK,		{	KEY88_STOP,			0,			},	},	/* ScrLock	*/
	{	SDL_SCANCODE_NONUSBACKSLASH,	{	KEY88_UNDERSCORE,	0,			},	},	/* ロ		*/
	{	SDL_SCANCODE_INTERNATIONAL2,	{	0,					0,			},	},	/* ひらカタ (不安定) */
	{	SDL_SCANCODE_INTERNATIONAL3,	{	KEY88_YEN,			KEY88_BAR,	},	},	/* ￥		*/
	{	SDL_SCANCODE_INTERNATIONAL4,	{	KEY88_HENKAN,		0,			},	},	/* 変換		*/
	{	SDL_SCANCODE_INTERNATIONAL5,	{	KEY88_KETTEI,		0,			},	},	/* 無変換	*/
	{	SDL_SCANCODE_APPLICATION,		{	KEY88_ZENKAKU,		0,			},	},	/* App		*/
};

static const T_REMAPPING remapping_win_101[] = {

	{	SDL_SCANCODE_GRAVE,				{	KEY88_YEN,			KEY88_BAR,	},	},	/* `		*/
	{	SDL_SCANCODE_SCROLLLOCK,		{	KEY88_STOP,			0,			},	},	/* ScrLock	*/
	{	SDL_SCANCODE_RCTRL,				{	KEY88_UNDERSCORE,	0,			},	},	/* 右Ctrl	*/
};



static const T_REMAPPING remapping_unix_106[] = {

	{	SDL_SCANCODE_GRAVE,				{	0,					0,			},	},	/* 半角全角 */
	{	SDL_SCANCODE_SCROLLLOCK,		{	KEY88_STOP,			0,			},	},	/* ScrLock	*/
	{	SDL_SCANCODE_INTERNATIONAL1,	{	KEY88_UNDERSCORE,	0,			},	},	/* ロ		*/
	{	SDL_SCANCODE_INTERNATIONAL2,	{	0,					0,			},	},	/* ひらカタ */
	{	SDL_SCANCODE_INTERNATIONAL3,	{	KEY88_YEN,			KEY88_BAR,	},	},	/* ￥		*/
	{	SDL_SCANCODE_INTERNATIONAL4,	{	KEY88_HENKAN,		0,			},	},	/* 変換		*/
	{	SDL_SCANCODE_INTERNATIONAL5,	{	KEY88_KETTEI,		0,			},	},	/* 無変換	*/
	{	SDL_SCANCODE_APPLICATION,		{	KEY88_ZENKAKU,		0,			},	},	/* App		*/
};

static const T_REMAPPING remapping_unix_101[] = {

	{	SDL_SCANCODE_GRAVE,				{	KEY88_YEN,			KEY88_BAR,	},	},	/* `		*/
	{	SDL_SCANCODE_SCROLLLOCK,		{	KEY88_STOP,			0,			},	},	/* ScrLock	*/
	{	SDL_SCANCODE_RCTRL,				{	KEY88_UNDERSCORE,	0,			},	},	/* 右Ctrl	*/
};



static const T_REMAPPING remapping_menukey[] = {

	{	SDL_SCANCODE_LGUI,				{	KEY88_SYS_MENU,		0,			},	},	/* 左Win/Cmd */
};



/*----------------------------------------------------------------------
 * キーボードレイアウト変更時の キーコード変換情報 (デフォルト)
 *----------------------------------------------------------------------*/

static const T_BINDING binding_default[] = {

/*  以下は、例   ↓このスキャンコード押下は、↓このキーコードとする */
/*	{ KEYCODE_SCAN,	SDL_SCANCODE_1,			{ KEY88_KP_1,		 },	{ 0, },	},*/

	{ KEYCODE_INVALID, 0,					{ 0,				 },	{ 0, },	},
};



/******************************************************************************
 * イベントハンドリング
 *
 *  1/60毎に呼び出される。
 *****************************************************************************/
static int joystick_init(void);
static void joystick_exit(void);
static int analyze_keyconf_file(void);

/*
 * これは 起動時に1回だけ呼ばれる
 */
void event_init(void)
{
	const T_REMAPPING *map;
	int mapsize;
	int i;

	/* ジョイスティック初期化 */

	if (use_joydevice) {
		if (verbose_proc) {
			printf("Initializing Joystick System ... ");
		}
		i = joystick_init();
		if (verbose_proc) {
			if (i >= 0) {
				printf("OK (found %d joystick(s))\n", i);
			} else {
				printf("FAILED\n");
			}
		}
	}


	/* キーマッピング初期化 */

	memset(scancode2key88, 0, sizeof(scancode2key88));
	memset(binding, 0, sizeof(binding));



	switch (keyboard_type) {

	case 0:									/* デフォルトキーボード */
		if (analyze_keyconf_file()) {
			;
		} else {
			memcpy(scancode2key88,
				   scancode2key88_default, sizeof(scancode2key88_default));
			memcpy(binding, binding_default, sizeof(binding_default));
		}
		break;


	case 1:									/* jp106キーボード */
	case 2:									/* us101キーボード ? */
		memcpy(scancode2key88,
			   scancode2key88_default, sizeof(scancode2key88_default));
		memcpy(binding, binding_default, sizeof(binding_default));


		/* 106/101キーボード独自のキー設定 */
		if (strncmp(SDL_GetPlatform(), "Windows", 7) == 0) {

			if (keyboard_type == 1) {
				map = remapping_win_106;
				mapsize = COUNTOF(remapping_win_106);
			} else {
				map = remapping_win_101;
				mapsize = COUNTOF(remapping_win_101);
			}

		} else {

			if (keyboard_type == 1) {
				map = remapping_unix_106;
				mapsize = COUNTOF(remapping_unix_106);
			} else {
				map = remapping_unix_101;
				mapsize = COUNTOF(remapping_unix_101);
			}
		}


		for (i = 0; i < mapsize; i++) {
			scancode2key88[map->scancode][0] = map->key88[0];
			scancode2key88[map->scancode][1] = map->key88[1];
			map++;
		}


		/* メニューキーを設定する */
		if (use_menukey) {
			map = remapping_menukey;
			mapsize = COUNTOF(remapping_menukey);

			for (i = 0; i < mapsize; i++) {
				scancode2key88[map->scancode][0] = map->key88[0];
				scancode2key88[map->scancode][1] = map->key88[1];
				map++;
			}
		}

		break;
	}


	/* キーボードレイアウト変更時のキー差し替えの準備 */

	for (i = 0; i < COUNTOF(binding); i++) {

		if (binding[i].type == KEYCODE_SYM) {

			/* skip! */

		} else if (binding[i].type == KEYCODE_SCAN) {

			binding[i].org_key88[0] = scancode2key88[binding[i].code][0];
			binding[i].org_key88[1] = scancode2key88[binding[i].code][1];

		} else {
			break;
		}
	}


	/*
	 * SDL_StopTextInput() により、IMEによる入力は禁止される。
	 *    1 キーを押下すると、SDL_KEYDOWN イベントが発生する。
	 *    SDL_TEXTINPUT イベントは発生しない。
	 *    押下されたのが 1 なのか ! なのかは自力で判断する必要がある。
	 *
	 * SDL_StartTextInput() を使えば IMEによる入力が可能になる (初期状態)
	 *    1 キーを押下すると、SDL_KEYDOWN イベントが発生する。
	 *    つづけて、SDL_TEXTINPUT イベントが発生する。
	 *    SDL_TEXTINPUT イベントでは、 "1" か "!" が通知される。
	 *
	 *    IMEを有効にした場合は、
	 *    1 キーを押下すると、SDL_KEYDOWN イベントが発生するが、
	 *    ここではまだ、SDL_TEXTINPUT イベントは発生しない。
	 *    入力を確定すると、SDL_TEXTINPUT イベントが発生する。
	 *    SDL_TEXTINPUT イベントでは、 UTF8 文字列が通知される
	 *
	 * SDL_StartTextInput() 状態で、 IME がオフならば、
	 * SDL_TEXTINPUT イベントで "1" か "!" が区別できるのだが、
	 * IME がオンならば、わけのわからんUTF8文字列がやってくる。
	 * (ASCIIの場合もあるのだが…)
	 * そもそも 全角/半角キーが IME に奪われてしまう。
	 * スキャンコードを拾って、修飾キーとの組み合わせを見て、
	 * 1 と ! や、 a と A を区別するしかないか。
	 * us101キーでも シフト+2 は " にするしかないけど。
	 */
	SDL_StopTextInput();

#if SDL_VERSION_ATLEAST(2, 0, 4)
	/* SDL_TEXTEDITINGイベントを送らず、IME内部で確定処理する */
	/* マニュアルにはこう書いてあるが Windows では効果がない? */
	SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");
#endif
}



/*
 * 約 1/60 毎に呼ばれる
 */
void event_update(void)
{
	SDL_Event E;
	int key88, x, y, i;
	Uint32 windowID, which;
	int act, w, h;


	SDL_PumpEvents();							/* イベントを汲み上げる */

	while (SDL_PeepEvents(&E, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {

		switch (E.type) {

		/*
		 * キーボード
		 */
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (E.key.windowID != sdl_window_id) {
				break;
			}

			KEYPRINTF(("%s%c scan=%3d <%-24s> [sym=%3d](%c) %d\n",
					   (E.type == SDL_KEYDOWN) ? "ON" : "  ",
					   (E.key.repeat != 0) ? '*' : ' ',
					   E.key.keysym.scancode,
					   /*SDL_GetScancodeName(E.key.keysym.scancode), */
					   debug_table(E.key.keysym.scancode),
					   E.key.keysym.sym,
					   /*SDL_GetKeyName(E.key.keysym.sym), */
					   (0x20 <= E.key.keysym.sym &&
						E.key.keysym.sym <= 0x7e) ? E.key.keysym.sym : ' ',
					   E.key.timestamp));

			if ((sdl_repeat_on == FALSE) &&
				(E.key.repeat)) {
				/* キーリピートオフ指定時は、キーリピートのイベントを無視 */
				break;
			}

			key88 = scancode2key88[E.key.keysym.scancode][0];

			if (quasi88_is_exec()) {
				/* 実行中はそのまま */

			} else {
				/* メニューでは文字に変換する */

				if (now_ime) {
					/* IME入力可の場合、制御キーのみを処理する */
					if ((0x20 <= E.key.keysym.sym) &&
						(E.key.keysym.sym <= 0x7e)) {
						/* sym が文字ならここでは無視 (SDL_TEXTINPUT を待つ) */
						break;

					} else if (memcmp
							   (SDL_GetScancodeName(E.key.keysym.scancode),
								"Keypad", 6) == 0) {
						/* scancode の名称が Keypad の時も、無視 */
						break;

					} else {
						/* 文字以外(制御キー)はそのまま */
						;
					}

				} else {
					/* IME入力不可の場合、シフト押下で文字を切り替え */
					if (scancode2key88[E.key.keysym.scancode][1]) {
						int isShift = 0;

						if ((KEY88_a <= key88) && (key88 <= KEY88_z)) {
							if (E.key.keysym.mod & KMOD_CAPS) {
								isShift ^= 1;
							}
						}
						if (E.key.keysym.mod & KMOD_SHIFT) {
							isShift ^= 1;
						}
						key88 = scancode2key88[E.key.keysym.scancode][isShift];
					}
				}
			}
			quasi88_key(key88, (E.type == SDL_KEYDOWN));
			break;

		case SDL_TEXTEDITING:
			if (E.edit.windowID != sdl_window_id) {
				break;
			}

			IMEPRINTF(("edit    \"%s\" (st=%d ln=%d)\n",
					   E.edit.text, E.edit.start, E.edit.length));

			/* IME入力中の文字は全て無視し、IME入力中であることを覚えておく */
			if (E.edit.text[0] != '\0') {
				now_ime_input = TRUE;
			} else {
				now_ime_input = FALSE;
			}
			break;

		case SDL_TEXTINPUT:
			if (E.edit.windowID != sdl_window_id) {
				break;
			}

			IMEPRINTF(("UNICODE \"%s\"\n", E.text.text));

			if (now_ime_input) {
				/* IMEで入力した文字はすべて破棄 */
			} else {
				/* IMEを使わずに入力したなら、ASCII文字に限り使う */
				if ((0x20 <= E.text.text[0]) && (E.text.text[0] <= 0x7e)) {
					key88 = E.text.text[0];
					quasi88_key(key88, SDL_KEYDOWN);
				}
			}
			now_ime_input = FALSE;
			break;

		/*
		 * マウス
		 */
		case SDL_MOUSEWHEEL:
			if (E.wheel.windowID != sdl_window_id) {
				break;
			}

			if (touchkey_config_action(&E)) {
				break;
			}

			/* マウス移動イベントも同時に処理する必要があるなら、
			 * quasi88_mouse_moved_abs/rel 関数をここで呼び出しておく */

			if (E.wheel.y > 0) {
				key88 = KEY88_MOUSE_WUP;
			} else if (E.wheel.y < 0) {
				key88 = KEY88_MOUSE_WDN;
			} else {
				key88 = 0;
			}
			if (key88) {
				quasi88_mouse(key88, 1);
				/* オフイベントがないけど、問題はなさげ */
			}
			break;

		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (E.type == SDL_MOUSEMOTION) {
				windowID = E.motion.windowID;
				which = E.motion.which;
				x = E.motion.x;
				y = E.motion.y;
			} else {
				windowID = E.button.windowID;
				which = E.button.which;
				x = E.button.x;
				y = E.button.y;
			}
			act = M_F_EVENT_IGNORE;

			if (windowID == sdl_window_id) {
				if (sdl_buildin_touchkey == FALSE) {
					/* 画面にメインエリアのみがある場合 */
					if (quasi88_is_exec()) {
						if (which == SDL_TOUCH_MOUSEID) {
							/* EMU画面での指タッチ操作は無視 */
							break;
						}
					}
					act = M_F_EVENT_EMU_MENU;
				} else {
					/* 画面にメインエリアとタッチキーエリアがある場合 */
					act = get_mouse_finger_event_in_main_window(&E);
					if (act == M_F_EVENT_TOUCHKEY) {
						x = x - sdl_area_touch.x;
						y = y - sdl_area_touch.y;
						w = sdl_area_touch.w;
						h = sdl_area_touch.h;
					}
				}
			} else {
				/* 画面はタッチキー専用の場合 */
				act = get_mouse_finger_event_in_touchkey_window(&E);
				if (act == M_F_EVENT_TOUCHKEY) {
					/* x = x; */
					/* y = y; */
					w = key_width;
					h = key_height;
				}
			}

			if (act == M_F_EVENT_EMU_MENU) {
				switch (E.type) {
				case SDL_MOUSEMOTION:
					if (sdl_mouse_rel_move) {
						/* マウスがウインドウの端に届いても、相対移動量を検出可 */
						x = E.motion.xrel;
						y = E.motion.yrel;
						quasi88_mouse_moved_rel(x, y);
					} else {
						x = E.motion.x;
						y = E.motion.y;
						if (sdl_buildin_touchkey) {
							x -= sdl_area_main.x;
							y -= sdl_area_main.y;
						}
						quasi88_mouse_moved_abs(x, y);
					}
					break;
				default:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					switch (E.button.button) {
					case SDL_BUTTON_LEFT:
						key88 = KEY88_MOUSE_L;
						break;
					case SDL_BUTTON_MIDDLE:
						key88 = KEY88_MOUSE_M;
						break;
					case SDL_BUTTON_RIGHT:
						key88 = KEY88_MOUSE_R;
						break;
					default:
						key88 = 0;
						break;
					}
					if (key88) {
						if (which == SDL_TOUCH_MOUSEID) {
							if (E.type == SDL_MOUSEBUTTONDOWN) {
								quasi88_touch_as_mouse(TRUE);
							} else {
								quasi88_touch_as_mouse(FALSE);
							}
						}
						quasi88_mouse(key88, (E.type == SDL_MOUSEBUTTONDOWN));
					}
					break;
				}
			}
			if (act == M_F_EVENT_TOUCHKEY) {
				switch (E.type) {
				case SDL_MOUSEMOTION:
					touchkey_mouse_move(x, y, w, h);
					break;
				case SDL_MOUSEBUTTONDOWN:
					touchkey_mouse_down(x, y, w, h, E.button.clicks);
					break;
				case SDL_MOUSEBUTTONUP:
					touchkey_mouse_up();
					break;
				}
			}
			break;

		/*
		 * フィンガー
		 */
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			act = M_F_EVENT_IGNORE;

			if (E.tfinger.windowID == sdl_window_id) {
				if (sdl_buildin_touchkey == FALSE) {
					/* 画面にメインエリアのみがある場合 */
					if (quasi88_is_exec() ||
						(now_finger_mode)) {
						/* EMU で指タッチした時点から離すまで、例外処理 */
						act = M_F_EVENT_IGNORE2;
					}
				} else {
					/* 画面にメインエリアとタッチキーエリアがある場合 */
					act = get_mouse_finger_event_in_main_window(&E);
					if (act == M_F_EVENT_TOUCHKEY) {
						x = (int) (E.tfinger.x * sdl_width  - sdl_area_touch.x);
						y = (int) (E.tfinger.y * sdl_height - sdl_area_touch.y);
						w = sdl_area_touch.w;
						h = sdl_area_touch.h;
					}
				}
				/* メインエリア EMU で指タッチした時点から離すまでの例外処理。
				 * (ツールバーの操作をするため・・・) (X_X); */
				if (act == M_F_EVENT_IGNORE2) {
					if (E.type == SDL_FINGERDOWN) {
						if (quasi88_is_exec()) {
							if (now_finger_mode == FALSE) {
								now_finger_mode = TRUE;
								finger_id = E.tfinger.fingerId;
								finger_x = (int)(E.tfinger.x * sdl_width);
								finger_y = (int)(E.tfinger.y * sdl_height);
								if (sdl_buildin_touchkey) {
									finger_x -= sdl_area_main.x;
									finger_y -= sdl_area_main.y;
								}
							}
						}
					}
					if (E.type == SDL_FINGERUP) {
						if (now_finger_mode &&
							finger_id == E.tfinger.fingerId) {
							now_finger_mode = FALSE;
							if (quasi88_is_exec()) {
								x = (int)(E.tfinger.x * sdl_width);
								y = (int)(E.tfinger.y * sdl_height);
								if (sdl_buildin_touchkey) {
									x -= sdl_area_main.x;
									y -= sdl_area_main.y;
								}
								if ((ABS(finger_x - x) < 48) &&
									(ABS(finger_y - y) < 48)) {
									/* 48ドット以内なら指がずれても無視 */
									quasi88_touch(x, y);
								}
							}
						}
					}
				} else {
					now_finger_mode = FALSE;
				}
			} else {
				/* 画面はタッチキー専用の場合 */
				act = get_mouse_finger_event_in_touchkey_window(&E);
				if (act == M_F_EVENT_TOUCHKEY) {
					x = (int) (E.tfinger.x * key_width);
					y = (int) (E.tfinger.y * key_height);
					w = key_width;
					h = key_height;
				}
			}
			if (act == M_F_EVENT_TOUCHKEY) {
				switch (E.type) {
				case SDL_FINGERMOTION:
					touchkey_finger_move(E.tfinger.touchId, E.tfinger.fingerId,
										 x, y, w, h);
					break;
				case SDL_FINGERDOWN:
					touchkey_finger_down(E.tfinger.touchId, E.tfinger.fingerId,
										 x, y, w, h, key_nr_finger_clicks);
					break;
				case SDL_FINGERUP:
					touchkey_finger_up(E.tfinger.touchId, E.tfinger.fingerId);
					break;
				}
			}

			touchkey_config_action(&E);
			break;

		case SDL_MULTIGESTURE:
			touchkey_config_action(&E);
			break;

		/*
		 * ジョイスティック
		 */
		case SDL_JOYAXISMOTION:
			for (i = 0; i < COUNTOF(joy_info); i++) {
				if (joy_info[i].dev && joy_info[i].id == E.jaxis.which) {

					T_JOY_INFO *joy = &joy_info[i];
					int offset = i * KEY88_PAD_OFFSET;
					int chg;
					int now = joy->axis;

					if (E.jaxis.axis == 0) {			/* 左右方向 */
						now &= ~(AXIS_L | AXIS_R);
						if (E.jaxis.value < -0x4000) {
							now |= AXIS_L;
						} else if (E.jaxis.value > 0x4000) {
							now |= AXIS_R;
						}
					} else if (E.jaxis.axis == 1) {		/* 上下方向 */
						now &= ~(AXIS_U | AXIS_D);
						if (E.jaxis.value < -0x4000) {
							now |= AXIS_U;
						} else if (E.jaxis.value > 0x4000) {
							now |= AXIS_D;
						}
					} else {
						break;
					}

					chg = joy->axis ^ now;
					if (chg & AXIS_L) {
						quasi88_pad(KEY88_PAD1_LEFT + offset, (now & AXIS_L));
					}
					if (chg & AXIS_R) {
						quasi88_pad(KEY88_PAD1_RIGHT + offset, (now & AXIS_R));
					}
					if (chg & AXIS_U) {
						quasi88_pad(KEY88_PAD1_UP + offset, (now & AXIS_U));
					}
					if (chg & AXIS_D) {
						quasi88_pad(KEY88_PAD1_DOWN + offset, (now & AXIS_D));
					}

					joy->axis = now;

					JOYPRINTF(("JOY axis [%d] id=%d, axis=%d, value=%6d "
							   "(%c%c%c%c)\n",
							   i, E.jaxis.which, E.jaxis.axis, E.jaxis.value,
							   (now & AXIS_U) ? 'U' : ' ',
							   (now & AXIS_D) ? 'D' : ' ',
							   (now & AXIS_L) ? 'L' : ' ',
							   (now & AXIS_R) ? 'R' : ' '));
					break;
				}
			}
			break;

		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			for (i = 0; i < COUNTOF(joy_info); i++) {
				if (joy_info[i].dev && joy_info[i].id == E.jbutton.which) {

					int offset = i * KEY88_PAD_OFFSET;

					if (E.jbutton.button < KEY88_PAD_BUTTON_MAX) {

						key88 = KEY88_PAD1_A + E.jbutton.button + offset;
						quasi88_pad(key88, (E.jbutton.state == SDL_PRESSED));
					}

					JOYPRINTF(("JOY %s [%d] id=%d button=%d state=%d\n",
							   (E.type == SDL_JOYBUTTONUP) ? "up  " : "down",
							   i, E.jbutton.which,
							   E.jbutton.button, E.jbutton.state));
					break;
				}
			}
			break;

		case SDL_JOYDEVICEADDED:
			JOYPRINTF(("JOY add  no=%d ", E.jdevice.which));

			for (i = 0; i < COUNTOF(joy_info); i++) {
				if (joy_info[i].dev == NULL) {
					SDL_Joystick *dev = SDL_JoystickOpen(E.jdevice.which);

					if (dev) {
						joy_info[i].dev = dev;
						joy_info[i].id = SDL_JoystickInstanceID(dev);
						joy_info[i].axis = 0;

						JOYPRINTF(("... [%d] id=%d, %daxis, %dbutton", i,
								   SDL_JoystickInstanceID(dev),
								   SDL_JoystickNumAxes(dev),
								   SDL_JoystickNumButtons(dev)));
					}
					break;
				}
			}
			JOYPRINTF(("\n"));
			break;

		case SDL_JOYDEVICEREMOVED:
			JOYPRINTF(("JOY del  id=%d ", E.jdevice.which));

			for (i = 0; i < COUNTOF(joy_info); i++) {
				if (joy_info[i].dev && joy_info[i].id == E.jdevice.which) {

					SDL_JoystickClose(joy_info[i].dev);
					joy_info[i].dev = NULL;
					joy_info[i].id = 0;
					joy_info[i].axis = 0;

					JOYPRINTF(("... [%d]", i));
					break;
				}
			}
			JOYPRINTF(("\n"));
			break;

		/*
		 * ドラッグ&ドロップ
		 */
		case SDL_DROPTEXT:
			if (E.drop.windowID != sdl_window_id) {
				break;
			}
			DNDPRINTF(("DROPTEXT \"%s\"\n", E.drop.file));
			if (E.drop.file) {
				SDL_free(E.drop.file);
			}
			break;
		case SDL_DROPBEGIN:
			if (E.drop.windowID != sdl_window_id) {
				break;
			}
			DNDPRINTF(("DROPBEGIN\n"));
			now_drop = TRUE;
			break;
		case SDL_DROPFILE:
			if (E.drop.windowID != sdl_window_id) {
				break;
			}
			DNDPRINTF(("DROPFILE:\"%s\"\n", E.drop.file));
			if (now_drop) {
				/* 複数のドラッグ&ドロップでも、最初のファイルのみを処理 */
#ifdef _WIN32
				/* ファイル名はUTF-8のようなので、SJISに変換 */
				int lenUTF16; wchar_t *bufUTF16;
				int lenSJIS; char *bufSJIS;

				/* UTF8 -> Unicode */
				lenUTF16 = MultiByteToWideChar(CP_UTF8, 0, E.drop.file, -1, NULL, 0);
				bufUTF16 = malloc(sizeof(wchar_t) * lenUTF16);
				MultiByteToWideChar(CP_UTF8, 0, E.drop.file, -1, bufUTF16, lenUTF16);

				/* Unicode -> SJIS */
				lenSJIS = WideCharToMultiByte(CP_ACP, 0, bufUTF16, -1, NULL, 0, NULL, NULL);
				bufSJIS = malloc(sizeof(char) * lenSJIS);
				WideCharToMultiByte(CP_ACP, 0, bufUTF16, lenUTF16 + 1, bufSJIS, lenSJIS, NULL, NULL);

				DNDPRINTF(("      =>:\"%s\"\n", bufSJIS));

				quasi88_drag_and_drop(bufSJIS);

				free(bufUTF16);
				free(bufSJIS);
#else
				quasi88_drag_and_drop(E.drop.file);
#endif
				now_drop = FALSE;
			}
			if (E.drop.file) {
				SDL_free(E.drop.file);
			}
			break;
		case SDL_DROPCOMPLETE:
			if (E.drop.windowID != sdl_window_id) {
				break;
			}
			DNDPRINTF(("DROPCOMPLATE\n"));
			now_drop = FALSE;
			break;

		/*
		 * その他
		 */
		case SDL_DISPLAYEVENT:
			if (E.display.event == SDL_DISPLAYEVENT_ORIENTATION) {
				touchkey_cfg_orientation(E.display.display);
			}
			break;

		case SDL_WINDOWEVENT:
			if (E.window.windowID != sdl_window_id) {
				action_window_event_in_touchkey_window(&E);
				break;
			}
			switch (E.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				break;

			case SDL_WINDOWEVENT_RESIZED:
			/* ユーザがサイズを変えたら、最初にこのイベント */
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				/* 次にこのイベント、となる。 */
				/* SDLのAPIで変えた場合は、このイベントのみ */
				break;

			case SDL_WINDOWEVENT_EXPOSED:
				quasi88_expose();
				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				quasi88_focus_in();
				break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				quasi88_focus_out();
				break;

			case SDL_WINDOWEVENT_CLOSE:
				if (verbose_proc) {
					printf("Window Closed !\n");
				}
				sdl2_touchkey_exit();
				quasi88_quit();
				break;
			}
			break;

		case SDL_QUIT:
			quasi88_quit();
			break;
		}
	}
	sdl2_touchkey_draw();
}

/*
 * これは 終了時に1回だけ呼ばれる
 */
void event_exit(void)
{
	joystick_exit();
}




/***********************************************************************
 * 現在のマウス座標取得関数
 *
 ************************************************************************/

void event_get_mouse_pos(int *x, int *y)
{
	int win_x, win_y;

	SDL_PumpEvents();
	SDL_GetMouseState(&win_x, &win_y);

	*x = win_x;
	*y = win_y;
}


/******************************************************************************
 * キーボードレイアウト変更／復元
 *		** 現在、未使用 **
 *****************************************************************************/

static void layout_setup(int enable)
{
	int i;

	for (i = 0; i < COUNTOF(binding); i++) {

		if (binding[i].type == KEYCODE_SYM) {

			/* skip! */

		} else if (binding[i].type == KEYCODE_SCAN) {

			if (enable) {
				scancode2key88[binding[i].code][0] = binding[i].new_key88[0];
				scancode2key88[binding[i].code][1] = binding[i].new_key88[1];
			} else {
				scancode2key88[binding[i].code][0] = binding[i].org_key88[0];
				scancode2key88[binding[i].code][1] = binding[i].org_key88[1];
			}

		} else {
			break;
		}
	}
}

int event_keylayout_change(void)
{
	layout_setup(TRUE);
	return TRUE;
}

void event_keylayout_revert(void)
{
	layout_setup(FALSE);
}



/******************************************************************************
 * エミュレート／メニュー／ポーズ／モニターモード の 開始時の処理
 *
 *****************************************************************************/

void event_switch(void)
{
	/* 既存のイベントをすべて破棄 */
	/* なんてことは、しない ? */

	if (quasi88_is_exec()) {

		SDL_StopTextInput();
		now_ime = FALSE;

	} else {
		if (use_ime) {
			SDL_StartTextInput();
			now_ime = TRUE;
			now_ime_input = FALSE;
		} else {
			SDL_StopTextInput();
			now_ime = FALSE;
		}
	}
}



/******************************************************************************
 * ジョイスティック
 *
 *****************************************************************************/

static int joystick_init(void)
{
	int i, num;

	/* ワーク初期化 */
	for (i = 0; i < COUNTOF(joy_info); i++) {
		joy_info[i].dev = NULL;
		joy_info[i].id = 0;
		joy_info[i].axis = 0;
	}


	/* ジョイスティックサブシステム初期化 */
	if (!SDL_WasInit(SDL_INIT_JOYSTICK)) {
		if (SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
			return -1;
		}
	}


	/* ジョイスティックの数を調べて、最大2個のデバイスをオープン */

	/* ジョイスティックイベント処理を無効にする */
	/* これがないと、デバイスを開いた後で  SDL_JOYDEVICEADDED が発生する? */
	SDL_JoystickEventState(SDL_DISABLE);

	num = SDL_NumJoysticks();

	for (i = 0; (i < num) && (i < COUNTOF(joy_info)); i++) {
		SDL_Joystick *dev = SDL_JoystickOpen(i);

		if (dev) {
			joy_info[i].dev = dev;
			joy_info[i].id = SDL_JoystickInstanceID(dev);
			joy_info[i].axis = 0;
			JOYPRINTF(("\nJOY init no=%d ... [%d] id=%d, %daxis, %dbutton\n",
					   i, i, SDL_JoystickInstanceID(dev),
					   SDL_JoystickNumAxes(dev), SDL_JoystickNumButtons(dev)));
		}
	}

	/* ジョイスティックイベント処理を有効にする */
	SDL_JoystickEventState(SDL_ENABLE);

	/* ジョイスティックの数を返す */
	return i;
}



static void joystick_exit(void)
{
	int i;

	for (i = 0; i < COUNTOF(joy_info); i++) {
		if (joy_info[i].dev) {
			SDL_JoystickClose(joy_info[i].dev);
			joy_info[i].dev = NULL;
			joy_info[i].id = 0;
			joy_info[i].axis = 0;
		}
	}
}



int event_get_joystick_num(void)
{
	int i, cnt;

	cnt = 0;
	for (i = 0; i < COUNTOF(joy_info); i++) {
		if (joy_info[i].dev) {
			cnt++;
		}
	}

	return cnt;
}






/****************************************************************************
 * キー設定ファイルを読み込んで、設定する。
 *      設定ファイルが無ければ偽、あれば処理して真を返す
 *****************************************************************************/

/* SDL_SCANCODE_xxx を int 値に変換するテーブル */

static const T_SYMBOL_TABLE sdlscancode_list[] = {

	{	"SDL_SCANCODE_A",					SDL_SCANCODE_A,					},	/*	 = 4,	*/
	{	"SDL_SCANCODE_B",					SDL_SCANCODE_B,					},	/*	 = 5,	*/
	{	"SDL_SCANCODE_C",					SDL_SCANCODE_C,					},	/*	 = 6,	*/
	{	"SDL_SCANCODE_D",					SDL_SCANCODE_D,					},	/*	 = 7,	*/
	{	"SDL_SCANCODE_E",					SDL_SCANCODE_E,					},	/*	 = 8,	*/
	{	"SDL_SCANCODE_F",					SDL_SCANCODE_F,					},	/*	 = 9,	*/
	{	"SDL_SCANCODE_G",					SDL_SCANCODE_G,					},	/*	 = 10,	*/
	{	"SDL_SCANCODE_H",					SDL_SCANCODE_H,					},	/*	 = 11,	*/
	{	"SDL_SCANCODE_I",					SDL_SCANCODE_I,					},	/*	 = 12,	*/
	{	"SDL_SCANCODE_J",					SDL_SCANCODE_J,					},	/*	 = 13,	*/
	{	"SDL_SCANCODE_K",					SDL_SCANCODE_K,					},	/*	 = 14,	*/
	{	"SDL_SCANCODE_L",					SDL_SCANCODE_L,					},	/*	 = 15,	*/
	{	"SDL_SCANCODE_M",					SDL_SCANCODE_M,					},	/*	 = 16,	*/
	{	"SDL_SCANCODE_N",					SDL_SCANCODE_N,					},	/*	 = 17,	*/
	{	"SDL_SCANCODE_O",					SDL_SCANCODE_O,					},	/*	 = 18,	*/
	{	"SDL_SCANCODE_P",					SDL_SCANCODE_P,					},	/*	 = 19,	*/
	{	"SDL_SCANCODE_Q",					SDL_SCANCODE_Q,					},	/*	 = 20,	*/
	{	"SDL_SCANCODE_R",					SDL_SCANCODE_R,					},	/*	 = 21,	*/
	{	"SDL_SCANCODE_S",					SDL_SCANCODE_S,					},	/*	 = 22,	*/
	{	"SDL_SCANCODE_T",					SDL_SCANCODE_T,					},	/*	 = 23,	*/
	{	"SDL_SCANCODE_U",					SDL_SCANCODE_U,					},	/*	 = 24,	*/
	{	"SDL_SCANCODE_V",					SDL_SCANCODE_V,					},	/*	 = 25,	*/
	{	"SDL_SCANCODE_W",					SDL_SCANCODE_W,					},	/*	 = 26,	*/
	{	"SDL_SCANCODE_X",					SDL_SCANCODE_X,					},	/*	 = 27,	*/
	{	"SDL_SCANCODE_Y",					SDL_SCANCODE_Y,					},	/*	 = 28,	*/
	{	"SDL_SCANCODE_Z",					SDL_SCANCODE_Z,					},	/*	 = 29,	*/
	{	"SDL_SCANCODE_1",					SDL_SCANCODE_1,					},	/*	 = 30,	*/
	{	"SDL_SCANCODE_2",					SDL_SCANCODE_2,					},	/*	 = 31,	*/
	{	"SDL_SCANCODE_3",					SDL_SCANCODE_3,					},	/*	 = 32,	*/
	{	"SDL_SCANCODE_4",					SDL_SCANCODE_4,					},	/*	 = 33,	*/
	{	"SDL_SCANCODE_5",					SDL_SCANCODE_5,					},	/*	 = 34,	*/
	{	"SDL_SCANCODE_6",					SDL_SCANCODE_6,					},	/*	 = 35,	*/
	{	"SDL_SCANCODE_7",					SDL_SCANCODE_7,					},	/*	 = 36,	*/
	{	"SDL_SCANCODE_8",					SDL_SCANCODE_8,					},	/*	 = 37,	*/
	{	"SDL_SCANCODE_9",					SDL_SCANCODE_9,					},	/*	 = 38,	*/
	{	"SDL_SCANCODE_0",					SDL_SCANCODE_0,					},	/*	 = 39,	*/
	{	"SDL_SCANCODE_RETURN",				SDL_SCANCODE_RETURN,			},	/*	 = 40,	*/
	{	"SDL_SCANCODE_ESCAPE",				SDL_SCANCODE_ESCAPE,			},	/*	 = 41,	*/
	{	"SDL_SCANCODE_BACKSPACE",			SDL_SCANCODE_BACKSPACE,			},	/*	 = 42,	*/
	{	"SDL_SCANCODE_TAB",					SDL_SCANCODE_TAB,				},	/*	 = 43,	*/
	{	"SDL_SCANCODE_SPACE",				SDL_SCANCODE_SPACE,				},	/*	 = 44,	*/
	{	"SDL_SCANCODE_MINUS",				SDL_SCANCODE_MINUS,				},	/*	 = 45,	*/
	{	"SDL_SCANCODE_EQUALS",				SDL_SCANCODE_EQUALS,			},	/*	 = 46,	*/
	{	"SDL_SCANCODE_LEFTBRACKET",			SDL_SCANCODE_LEFTBRACKET,		},	/*	 = 47,	*/
	{	"SDL_SCANCODE_RIGHTBRACKET",		SDL_SCANCODE_RIGHTBRACKET,		},	/*	 = 48,	*/
	{	"SDL_SCANCODE_BACKSLASH",			SDL_SCANCODE_BACKSLASH,			},	/*	 = 49,	*/
	{	"SDL_SCANCODE_NONUSHASH",			SDL_SCANCODE_NONUSHASH,			},	/*	 = 50,	*/
	{	"SDL_SCANCODE_SEMICOLON",			SDL_SCANCODE_SEMICOLON,			},	/*	 = 51,	*/
	{	"SDL_SCANCODE_APOSTROPHE",			SDL_SCANCODE_APOSTROPHE,		},	/*	 = 52,	*/
	{	"SDL_SCANCODE_GRAVE",				SDL_SCANCODE_GRAVE,				},	/*	 = 53,	*/
	{	"SDL_SCANCODE_COMMA",				SDL_SCANCODE_COMMA,				},	/*	 = 54,	*/
	{	"SDL_SCANCODE_PERIOD",				SDL_SCANCODE_PERIOD,			},	/*	 = 55,	*/
	{	"SDL_SCANCODE_SLASH",				SDL_SCANCODE_SLASH,				},	/*	 = 56,	*/
	{	"SDL_SCANCODE_CAPSLOCK",			SDL_SCANCODE_CAPSLOCK,			},	/*	 = 57,	*/
	{	"SDL_SCANCODE_F1",					SDL_SCANCODE_F1,				},	/*	 = 58,	*/
	{	"SDL_SCANCODE_F2",					SDL_SCANCODE_F2,				},	/*	 = 59,	*/
	{	"SDL_SCANCODE_F3",					SDL_SCANCODE_F3,				},	/*	 = 60,	*/
	{	"SDL_SCANCODE_F4",					SDL_SCANCODE_F4,				},	/*	 = 61,	*/
	{	"SDL_SCANCODE_F5",					SDL_SCANCODE_F5,				},	/*	 = 62,	*/
	{	"SDL_SCANCODE_F6",					SDL_SCANCODE_F6,				},	/*	 = 63,	*/
	{	"SDL_SCANCODE_F7",					SDL_SCANCODE_F7,				},	/*	 = 64,	*/
	{	"SDL_SCANCODE_F8",					SDL_SCANCODE_F8,				},	/*	 = 65,	*/
	{	"SDL_SCANCODE_F9",					SDL_SCANCODE_F9,				},	/*	 = 66,	*/
	{	"SDL_SCANCODE_F10",					SDL_SCANCODE_F10,				},	/*	 = 67,	*/
	{	"SDL_SCANCODE_F11",					SDL_SCANCODE_F11,				},	/*	 = 68,	*/
	{	"SDL_SCANCODE_F12",					SDL_SCANCODE_F12,				},	/*	 = 69,	*/
	{	"SDL_SCANCODE_PRINTSCREEN",			SDL_SCANCODE_PRINTSCREEN,		},	/*	 = 70,	*/
	{	"SDL_SCANCODE_SCROLLLOCK",			SDL_SCANCODE_SCROLLLOCK,		},	/*	 = 71,	*/
	{	"SDL_SCANCODE_PAUSE",				SDL_SCANCODE_PAUSE,				},	/*	 = 72,	*/
	{	"SDL_SCANCODE_INSERT",				SDL_SCANCODE_INSERT,			},	/*	 = 73,	*/
	{	"SDL_SCANCODE_HOME",				SDL_SCANCODE_HOME,				},	/*	 = 74,	*/
	{	"SDL_SCANCODE_PAGEUP",				SDL_SCANCODE_PAGEUP,			},	/*	 = 75,	*/
	{	"SDL_SCANCODE_DELETE",				SDL_SCANCODE_DELETE,			},	/*	 = 76,	*/
	{	"SDL_SCANCODE_END",					SDL_SCANCODE_END,				},	/*	 = 77,	*/
	{	"SDL_SCANCODE_PAGEDOWN",			SDL_SCANCODE_PAGEDOWN,			},	/*	 = 78,	*/
	{	"SDL_SCANCODE_RIGHT",				SDL_SCANCODE_RIGHT,				},	/*	 = 79,	*/
	{	"SDL_SCANCODE_LEFT",				SDL_SCANCODE_LEFT,				},	/*	 = 80,	*/
	{	"SDL_SCANCODE_DOWN",				SDL_SCANCODE_DOWN,				},	/*	 = 81,	*/
	{	"SDL_SCANCODE_UP",					SDL_SCANCODE_UP,				},	/*	 = 82,	*/
	{	"SDL_SCANCODE_NUMLOCKCLEAR",		SDL_SCANCODE_NUMLOCKCLEAR,		},	/*	 = 83,	*/
	{	"SDL_SCANCODE_KP_DIVIDE",			SDL_SCANCODE_KP_DIVIDE,			},	/*	 = 84,	*/
	{	"SDL_SCANCODE_KP_MULTIPLY",			SDL_SCANCODE_KP_MULTIPLY,		},	/*	 = 85,	*/
	{	"SDL_SCANCODE_KP_MINUS",			SDL_SCANCODE_KP_MINUS,			},	/*	 = 86,	*/
	{	"SDL_SCANCODE_KP_PLUS",				SDL_SCANCODE_KP_PLUS,			},	/*	 = 87,	*/
	{	"SDL_SCANCODE_KP_ENTER",			SDL_SCANCODE_KP_ENTER,			},	/*	 = 88,	*/
	{	"SDL_SCANCODE_KP_1",				SDL_SCANCODE_KP_1,				},	/*	 = 89,	*/
	{	"SDL_SCANCODE_KP_2",				SDL_SCANCODE_KP_2,				},	/*	 = 90,	*/
	{	"SDL_SCANCODE_KP_3",				SDL_SCANCODE_KP_3,				},	/*	 = 91,	*/
	{	"SDL_SCANCODE_KP_4",				SDL_SCANCODE_KP_4,				},	/*	 = 92,	*/
	{	"SDL_SCANCODE_KP_5",				SDL_SCANCODE_KP_5,				},	/*	 = 93,	*/
	{	"SDL_SCANCODE_KP_6",				SDL_SCANCODE_KP_6,				},	/*	 = 94,	*/
	{	"SDL_SCANCODE_KP_7",				SDL_SCANCODE_KP_7,				},	/*	 = 95,	*/
	{	"SDL_SCANCODE_KP_8",				SDL_SCANCODE_KP_8,				},	/*	 = 96,	*/
	{	"SDL_SCANCODE_KP_9",				SDL_SCANCODE_KP_9,				},	/*	 = 97,	*/
	{	"SDL_SCANCODE_KP_0",				SDL_SCANCODE_KP_0,				},	/*	 = 98,	*/
	{	"SDL_SCANCODE_KP_PERIOD",			SDL_SCANCODE_KP_PERIOD,			},	/*	 = 99,	*/
	{	"SDL_SCANCODE_NONUSBACKSLASH",		SDL_SCANCODE_NONUSBACKSLASH,	},	/*	 = 100,	*/
	{	"SDL_SCANCODE_APPLICATION",			SDL_SCANCODE_APPLICATION,		},	/*	 = 101,	*/
	{	"SDL_SCANCODE_POWER",				SDL_SCANCODE_POWER,				},	/*	 = 102,	*/
	{	"SDL_SCANCODE_KP_EQUALS",			SDL_SCANCODE_KP_EQUALS,			},	/*	 = 103,	*/
	{	"SDL_SCANCODE_F13",					SDL_SCANCODE_F13,				},	/*	 = 104,	*/
	{	"SDL_SCANCODE_F14",					SDL_SCANCODE_F14,				},	/*	 = 105,	*/
	{	"SDL_SCANCODE_F15",					SDL_SCANCODE_F15,				},	/*	 = 106,	*/
	{	"SDL_SCANCODE_F16",					SDL_SCANCODE_F16,				},	/*	 = 107,	*/
	{	"SDL_SCANCODE_F17",					SDL_SCANCODE_F17,				},	/*	 = 108,	*/
	{	"SDL_SCANCODE_F18",					SDL_SCANCODE_F18,				},	/*	 = 109,	*/
	{	"SDL_SCANCODE_F19",					SDL_SCANCODE_F19,				},	/*	 = 110,	*/
	{	"SDL_SCANCODE_F20",					SDL_SCANCODE_F20,				},	/*	 = 111,	*/
	{	"SDL_SCANCODE_F21",					SDL_SCANCODE_F21,				},	/*	 = 112,	*/
	{	"SDL_SCANCODE_F22",					SDL_SCANCODE_F22,				},	/*	 = 113,	*/
	{	"SDL_SCANCODE_F23",					SDL_SCANCODE_F23,				},	/*	 = 114,	*/
	{	"SDL_SCANCODE_F24",					SDL_SCANCODE_F24,				},	/*	 = 115,	*/
	{	"SDL_SCANCODE_EXECUTE",				SDL_SCANCODE_EXECUTE,			},	/*	 = 116,	*/
	{	"SDL_SCANCODE_HELP",				SDL_SCANCODE_HELP,				},	/*	 = 117,	*/
	{	"SDL_SCANCODE_MENU",				SDL_SCANCODE_MENU,				},	/*	 = 118,	*/
	{	"SDL_SCANCODE_SELECT",				SDL_SCANCODE_SELECT,			},	/*	 = 119,	*/
	{	"SDL_SCANCODE_STOP",				SDL_SCANCODE_STOP,				},	/*	 = 120,	*/
	{	"SDL_SCANCODE_AGAIN",				SDL_SCANCODE_AGAIN,				},	/*	 = 121,	*/
	{	"SDL_SCANCODE_UNDO",				SDL_SCANCODE_UNDO,				},	/*	 = 122,	*/
	{	"SDL_SCANCODE_CUT",					SDL_SCANCODE_CUT,				},	/*	 = 123,	*/
	{	"SDL_SCANCODE_COPY",				SDL_SCANCODE_COPY,				},	/*	 = 124,	*/
	{	"SDL_SCANCODE_PASTE",				SDL_SCANCODE_PASTE,				},	/*	 = 125,	*/
	{	"SDL_SCANCODE_FIND",				SDL_SCANCODE_FIND,				},	/*	 = 126,	*/
	{	"SDL_SCANCODE_MUTE",				SDL_SCANCODE_MUTE,				},	/*	 = 127,	*/
	{	"SDL_SCANCODE_VOLUMEUP",			SDL_SCANCODE_VOLUMEUP,			},	/*	 = 128,	*/
	{	"SDL_SCANCODE_VOLUMEDOWN",			SDL_SCANCODE_VOLUMEDOWN,		},	/*	 = 129,	*/
	{	"SDL_SCANCODE_KP_COMMA",			SDL_SCANCODE_KP_COMMA,			},	/*	 = 133,	*/
	{	"SDL_SCANCODE_KP_EQUALSAS400",		SDL_SCANCODE_KP_EQUALSAS400,	},	/*	 = 134,	*/
	{	"SDL_SCANCODE_INTERNATIONAL1",		SDL_SCANCODE_INTERNATIONAL1,	},	/*	 = 135,	*/
	{	"SDL_SCANCODE_INTERNATIONAL2",		SDL_SCANCODE_INTERNATIONAL2,	},	/*	 = 136,	*/
	{	"SDL_SCANCODE_INTERNATIONAL3",		SDL_SCANCODE_INTERNATIONAL3,	},	/*	 = 137,	*/
	{	"SDL_SCANCODE_INTERNATIONAL4",		SDL_SCANCODE_INTERNATIONAL4,	},	/*	 = 138,	*/
	{	"SDL_SCANCODE_INTERNATIONAL5",		SDL_SCANCODE_INTERNATIONAL5,	},	/*	 = 139,	*/
	{	"SDL_SCANCODE_INTERNATIONAL6",		SDL_SCANCODE_INTERNATIONAL6,	},	/*	 = 140,	*/
	{	"SDL_SCANCODE_INTERNATIONAL7",		SDL_SCANCODE_INTERNATIONAL7,	},	/*	 = 141,	*/
	{	"SDL_SCANCODE_INTERNATIONAL8",		SDL_SCANCODE_INTERNATIONAL8,	},	/*	 = 142,	*/
	{	"SDL_SCANCODE_INTERNATIONAL9",		SDL_SCANCODE_INTERNATIONAL9,	},	/*	 = 143,	*/
	{	"SDL_SCANCODE_LANG1",				SDL_SCANCODE_LANG1,				},	/*	 = 144,	*/
	{	"SDL_SCANCODE_LANG2",				SDL_SCANCODE_LANG2,				},	/*	 = 145,	*/
	{	"SDL_SCANCODE_LANG3",				SDL_SCANCODE_LANG3,				},	/*	 = 146,	*/
	{	"SDL_SCANCODE_LANG4",				SDL_SCANCODE_LANG4,				},	/*	 = 147,	*/
	{	"SDL_SCANCODE_LANG5",				SDL_SCANCODE_LANG5,				},	/*	 = 148,	*/
	{	"SDL_SCANCODE_LANG6",				SDL_SCANCODE_LANG6,				},	/*	 = 149,	*/
	{	"SDL_SCANCODE_LANG7",				SDL_SCANCODE_LANG7,				},	/*	 = 150,	*/
	{	"SDL_SCANCODE_LANG8",				SDL_SCANCODE_LANG8,				},	/*	 = 151,	*/
	{	"SDL_SCANCODE_LANG9",				SDL_SCANCODE_LANG9,				},	/*	 = 152,	*/
	{	"SDL_SCANCODE_ALTERASE",			SDL_SCANCODE_ALTERASE,			},	/*	 = 153,	*/
	{	"SDL_SCANCODE_SYSREQ",				SDL_SCANCODE_SYSREQ,			},	/*	 = 154,	*/
	{	"SDL_SCANCODE_CANCEL",				SDL_SCANCODE_CANCEL,			},	/*	 = 155,	*/
	{	"SDL_SCANCODE_CLEAR",				SDL_SCANCODE_CLEAR,				},	/*	 = 156,	*/
	{	"SDL_SCANCODE_PRIOR",				SDL_SCANCODE_PRIOR,				},	/*	 = 157,	*/
	{	"SDL_SCANCODE_RETURN2",				SDL_SCANCODE_RETURN2,			},	/*	 = 158,	*/
	{	"SDL_SCANCODE_SEPARATOR",			SDL_SCANCODE_SEPARATOR,			},	/*	 = 159,	*/
	{	"SDL_SCANCODE_OUT",					SDL_SCANCODE_OUT,				},	/*	 = 160,	*/
	{	"SDL_SCANCODE_OPER",				SDL_SCANCODE_OPER,				},	/*	 = 161,	*/
	{	"SDL_SCANCODE_CLEARAGAIN",			SDL_SCANCODE_CLEARAGAIN,		},	/*	 = 162,	*/
	{	"SDL_SCANCODE_CRSEL",				SDL_SCANCODE_CRSEL,				},	/*	 = 163,	*/
	{	"SDL_SCANCODE_EXSEL",				SDL_SCANCODE_EXSEL,				},	/*	 = 164,	*/
	{	"SDL_SCANCODE_KP_00",				SDL_SCANCODE_KP_00,				},	/*	 = 176,	*/
	{	"SDL_SCANCODE_KP_000",				SDL_SCANCODE_KP_000,			},	/*	 = 177,	*/
	{	"SDL_SCANCODE_THOUSANDSSEPARATOR",	SDL_SCANCODE_THOUSANDSSEPARATOR,},	/*	 = 178,	*/
	{	"SDL_SCANCODE_DECIMALSEPARATOR",	SDL_SCANCODE_DECIMALSEPARATOR,	},	/*	 = 179,	*/
	{	"SDL_SCANCODE_CURRENCYUNIT",		SDL_SCANCODE_CURRENCYUNIT,		},	/*	 = 180,	*/
	{	"SDL_SCANCODE_CURRENCYSUBUNIT",		SDL_SCANCODE_CURRENCYSUBUNIT,	},	/*	 = 181,	*/
	{	"SDL_SCANCODE_KP_LEFTPAREN",		SDL_SCANCODE_KP_LEFTPAREN,		},	/*	 = 182,	*/
	{	"SDL_SCANCODE_KP_RIGHTPAREN",		SDL_SCANCODE_KP_RIGHTPAREN,		},	/*	 = 183,	*/
	{	"SDL_SCANCODE_KP_LEFTBRACE",		SDL_SCANCODE_KP_LEFTBRACE,		},	/*	 = 184,	*/
	{	"SDL_SCANCODE_KP_RIGHTBRACE",		SDL_SCANCODE_KP_RIGHTBRACE,		},	/*	 = 185,	*/
	{	"SDL_SCANCODE_KP_TAB",				SDL_SCANCODE_KP_TAB,			},	/*	 = 186,	*/
	{	"SDL_SCANCODE_KP_BACKSPACE",		SDL_SCANCODE_KP_BACKSPACE,		},	/*	 = 187,	*/
	{	"SDL_SCANCODE_KP_A",				SDL_SCANCODE_KP_A,				},	/*	 = 188,	*/
	{	"SDL_SCANCODE_KP_B",				SDL_SCANCODE_KP_B,				},	/*	 = 189,	*/
	{	"SDL_SCANCODE_KP_C",				SDL_SCANCODE_KP_C,				},	/*	 = 190,	*/
	{	"SDL_SCANCODE_KP_D",				SDL_SCANCODE_KP_D,				},	/*	 = 191,	*/
	{	"SDL_SCANCODE_KP_E",				SDL_SCANCODE_KP_E,				},	/*	 = 192,	*/
	{	"SDL_SCANCODE_KP_F",				SDL_SCANCODE_KP_F,				},	/*	 = 193,	*/
	{	"SDL_SCANCODE_KP_XOR",				SDL_SCANCODE_KP_XOR,			},	/*	 = 194,	*/
	{	"SDL_SCANCODE_KP_POWER",			SDL_SCANCODE_KP_POWER,			},	/*	 = 195,	*/
	{	"SDL_SCANCODE_KP_PERCENT",			SDL_SCANCODE_KP_PERCENT,		},	/*	 = 196,	*/
	{	"SDL_SCANCODE_KP_LESS",				SDL_SCANCODE_KP_LESS,			},	/*	 = 197,	*/
	{	"SDL_SCANCODE_KP_GREATER",			SDL_SCANCODE_KP_GREATER,		},	/*	 = 198,	*/
	{	"SDL_SCANCODE_KP_AMPERSAND",		SDL_SCANCODE_KP_AMPERSAND,		},	/*	 = 199,	*/
	{	"SDL_SCANCODE_KP_DBLAMPERSAND",		SDL_SCANCODE_KP_DBLAMPERSAND,	},	/*	 = 200,	*/
	{	"SDL_SCANCODE_KP_VERTICALBAR",		SDL_SCANCODE_KP_VERTICALBAR,	},	/*	 = 201,	*/
	{	"SDL_SCANCODE_KP_DBLVERTICALBAR",	SDL_SCANCODE_KP_DBLVERTICALBAR,	},	/*	 = 202,	*/
	{	"SDL_SCANCODE_KP_COLON",			SDL_SCANCODE_KP_COLON,			},	/*	 = 203,	*/
	{	"SDL_SCANCODE_KP_HASH",				SDL_SCANCODE_KP_HASH,			},	/*	 = 204,	*/
	{	"SDL_SCANCODE_KP_SPACE",			SDL_SCANCODE_KP_SPACE,			},	/*	 = 205,	*/
	{	"SDL_SCANCODE_KP_AT",				SDL_SCANCODE_KP_AT,				},	/*	 = 206,	*/
	{	"SDL_SCANCODE_KP_EXCLAM",			SDL_SCANCODE_KP_EXCLAM,			},	/*	 = 207,	*/
	{	"SDL_SCANCODE_KP_MEMSTORE",			SDL_SCANCODE_KP_MEMSTORE,		},	/*	 = 208,	*/
	{	"SDL_SCANCODE_KP_MEMRECALL",		SDL_SCANCODE_KP_MEMRECALL,		},	/*	 = 209,	*/
	{	"SDL_SCANCODE_KP_MEMCLEAR",			SDL_SCANCODE_KP_MEMCLEAR,		},	/*	 = 210,	*/
	{	"SDL_SCANCODE_KP_MEMADD",			SDL_SCANCODE_KP_MEMADD,			},	/*	 = 211,	*/
	{	"SDL_SCANCODE_KP_MEMSUBTRACT",		SDL_SCANCODE_KP_MEMSUBTRACT,	},	/*	 = 212,	*/
	{	"SDL_SCANCODE_KP_MEMMULTIPLY",		SDL_SCANCODE_KP_MEMMULTIPLY,	},	/*	 = 213,	*/
	{	"SDL_SCANCODE_KP_MEMDIVIDE",		SDL_SCANCODE_KP_MEMDIVIDE,		},	/*	 = 214,	*/
	{	"SDL_SCANCODE_KP_PLUSMINUS",		SDL_SCANCODE_KP_PLUSMINUS,		},	/*	 = 215,	*/
	{	"SDL_SCANCODE_KP_CLEAR",			SDL_SCANCODE_KP_CLEAR,			},	/*	 = 216,	*/
	{	"SDL_SCANCODE_KP_CLEARENTRY",		SDL_SCANCODE_KP_CLEARENTRY,		},	/*	 = 217,	*/
	{	"SDL_SCANCODE_KP_BINARY",			SDL_SCANCODE_KP_BINARY,			},	/*	 = 218,	*/
	{	"SDL_SCANCODE_KP_OCTAL",			SDL_SCANCODE_KP_OCTAL,			},	/*	 = 219,	*/
	{	"SDL_SCANCODE_KP_DECIMAL",			SDL_SCANCODE_KP_DECIMAL,		},	/*	 = 220,	*/
	{	"SDL_SCANCODE_KP_HEXADECIMAL",		SDL_SCANCODE_KP_HEXADECIMAL,	},	/*	 = 221,	*/
	{	"SDL_SCANCODE_LCTRL",				SDL_SCANCODE_LCTRL,				},	/*	 = 224,	*/
	{	"SDL_SCANCODE_LSHIFT",				SDL_SCANCODE_LSHIFT,			},	/*	 = 225,	*/
	{	"SDL_SCANCODE_LALT",				SDL_SCANCODE_LALT,				},	/*	 = 226,	*/
	{	"SDL_SCANCODE_LGUI",				SDL_SCANCODE_LGUI,				},	/*	 = 227,	*/
	{	"SDL_SCANCODE_RCTRL",				SDL_SCANCODE_RCTRL,				},	/*	 = 228,	*/
	{	"SDL_SCANCODE_RSHIFT",				SDL_SCANCODE_RSHIFT,			},	/*	 = 229,	*/
	{	"SDL_SCANCODE_RALT",				SDL_SCANCODE_RALT,				},	/*	 = 230,	*/
	{	"SDL_SCANCODE_RGUI",				SDL_SCANCODE_RGUI,				},	/*	 = 231,	*/
	{	"SDL_SCANCODE_MODE",				SDL_SCANCODE_MODE,				},	/*	 = 257,	*/
	{	"SDL_SCANCODE_AUDIONEXT",			SDL_SCANCODE_AUDIONEXT,			},	/*	 = 258,	*/
	{	"SDL_SCANCODE_AUDIOPREV",			SDL_SCANCODE_AUDIOPREV,			},	/*	 = 259,	*/
	{	"SDL_SCANCODE_AUDIOSTOP",			SDL_SCANCODE_AUDIOSTOP,			},	/*	 = 260,	*/
	{	"SDL_SCANCODE_AUDIOPLAY",			SDL_SCANCODE_AUDIOPLAY,			},	/*	 = 261,	*/
	{	"SDL_SCANCODE_AUDIOMUTE",			SDL_SCANCODE_AUDIOMUTE,			},	/*	 = 262,	*/
	{	"SDL_SCANCODE_MEDIASELECT",			SDL_SCANCODE_MEDIASELECT,		},	/*	 = 263,	*/
	{	"SDL_SCANCODE_WWW",					SDL_SCANCODE_WWW,				},	/*	 = 264,	*/
	{	"SDL_SCANCODE_MAIL",				SDL_SCANCODE_MAIL,				},	/*	 = 265,	*/
	{	"SDL_SCANCODE_CALCULATOR",			SDL_SCANCODE_CALCULATOR,		},	/*	 = 266,	*/
	{	"SDL_SCANCODE_COMPUTER",			SDL_SCANCODE_COMPUTER,			},	/*	 = 267,	*/
	{	"SDL_SCANCODE_AC_SEARCH",			SDL_SCANCODE_AC_SEARCH,			},	/*	 = 268,	*/
	{	"SDL_SCANCODE_AC_HOME",				SDL_SCANCODE_AC_HOME,			},	/*	 = 269,	*/
	{	"SDL_SCANCODE_AC_BACK",				SDL_SCANCODE_AC_BACK,			},	/*	 = 270,	*/
	{	"SDL_SCANCODE_AC_FORWARD",			SDL_SCANCODE_AC_FORWARD,		},	/*	 = 271,	*/
	{	"SDL_SCANCODE_AC_STOP",				SDL_SCANCODE_AC_STOP,			},	/*	 = 272,	*/
	{	"SDL_SCANCODE_AC_REFRESH",			SDL_SCANCODE_AC_REFRESH,		},	/*	 = 273,	*/
	{	"SDL_SCANCODE_AC_BOOKMARKS",		SDL_SCANCODE_AC_BOOKMARKS,		},	/*	 = 274,	*/
	{	"SDL_SCANCODE_BRIGHTNESSDOWN",		SDL_SCANCODE_BRIGHTNESSDOWN,	},	/*	 = 275,	*/
	{	"SDL_SCANCODE_BRIGHTNESSUP",		SDL_SCANCODE_BRIGHTNESSUP,		},	/*	 = 276,	*/
	{	"SDL_SCANCODE_DISPLAYSWITCH",		SDL_SCANCODE_DISPLAYSWITCH,		},	/*	 = 277,	*/
	{	"SDL_SCANCODE_KBDILLUMTOGGLE",		SDL_SCANCODE_KBDILLUMTOGGLE,	},	/*	 = 278,	*/
	{	"SDL_SCANCODE_KBDILLUMDOWN",		SDL_SCANCODE_KBDILLUMDOWN,		},	/*	 = 279,	*/
	{	"SDL_SCANCODE_KBDILLUMUP",			SDL_SCANCODE_KBDILLUMUP,		},	/*	 = 280,	*/
	{	"SDL_SCANCODE_EJECT",				SDL_SCANCODE_EJECT,				},	/*	 = 281,	*/
	{	"SDL_SCANCODE_SLEEP",				SDL_SCANCODE_SLEEP,				},	/*	 = 282,	*/
	{	"SDL_SCANCODE_APP1",				SDL_SCANCODE_APP1,				},	/*	 = 283,	*/
	{	"SDL_SCANCODE_APP2",				SDL_SCANCODE_APP2,				},	/*	 = 284,	*/
};

/* デバッグ用 */
static const char *debug_table(int scancode)
{
	int i;
	for (i = 0; i < COUNTOF(sdlscancode_list); i++) {
		if (scancode == sdlscancode_list[i].val) {
			return sdlscancode_list[i].name;
		}
	}
	return "*** TABLE ERROR ! ***";
}


/* キー設定ファイルの、識別タグをチェックするコールバック関数 */

static const char *identify_callback(const char *parm1,
									 const char *parm2, const char *parm3)
{
	if (my_strcmp(parm1, "[SDL2]") == 0) {
		return NULL;							/* 有効 */
	}

	return "";									/* 無効 */
}

/* キー設定ファイルの、設定を処理するコールバック関数 */

static const char *setting_callback(int type,
									int code, int key88, int alter_key88)
{
	static int binding_cnt = 0;
	int i;
	int with_shift;

	/* type によらず、 SCANCODE となる */
	if (code >= SDL_NUM_SCANCODES) {
		return "scancode too large";			/* 無効 */
	}

	/* シフトキー併用時の KEY88 コードは、 scancode2key88_default に従う */
	with_shift = 0;
	for (i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (scancode2key88_default[i][0] == key88) {
			with_shift = scancode2key88_default[i][1];
			break;
		}
	}
	scancode2key88[code][0] = key88;
	scancode2key88[code][1] = with_shift;


	if (alter_key88 >= 0) {
		if (binding_cnt >= COUNTOF(binding)) {
			return "too many alternative-keycode";		/* 無効 */
		}

		binding[binding_cnt].type = KEYCODE_SCAN;
		binding[binding_cnt].code = code;
		binding[binding_cnt].new_key88[0] = alter_key88;
		binding[binding_cnt].new_key88[1] = 0;

		binding_cnt++;
	}

	return NULL;								/* 有効 */
}

/* キー設定ファイルの処理関数 */

static int analyze_keyconf_file(void)
{
	return
		config_read_keyconf_file(file_keyboard,			/* キー設定ファイル  */
								 identify_callback,			/*識別タグ行 関数*/
								 sdlscancode_list,			/*変換テーブル   */
								 COUNTOF(sdlscancode_list),	/*テーブルサイズ */
								 TRUE,						/*大小文字無視   */
								 setting_callback);			/*設定行 関数    */
}
