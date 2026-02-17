#ifndef MENU_KEY_MESSAGE_H_INCLUDED
#define MENU_KEY_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「キー」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_KEY_FKEY,
	DATA_KEY_CURSOR,
	DATA_KEY_CURSOR_SPACING,
	DATA_KEY_SKEY,
	DATA_KEY_SKEY2
};
static const t_menulabel data_key[] = {
	{ { " Function key Config ",				" ファンクションキー設定 ",			}, },
	{ { " Curosr Key Config ",					" カーソルキー設定 ",				}, },
	{ { "                                 ",	"                                ",	}, },
	{ { " Software ",							" ソフトウェア ",					}, },
	{ { "   Keyboard  ",						"  キーボード  ",					}, },
};



enum {
	DATA_KEY_CFG_TENKEY,
	DATA_KEY_CFG_NUMLOCK
};
static const t_menudata data_key_cfg[] = {
	{ { "Set numeric key to TEN-key (-tenkey) ",	"数字キーをテンキーに割り当てる   (-tenkey)  ",	}, DATA_KEY_CFG_TENKEY,		},
	{ { "software NUM-Lock ON       (-numlock)",	"ソフトウェア NUM Lock をオンする (-numlock) ",	}, DATA_KEY_CFG_NUMLOCK,	},
};



static const t_menudata data_key_fkey[] = {
	{ { "   f6  key ",	"   f6  キー ",		},  6,	},
	{ { "   f7  key ",	"   f7  キー ",		},  7,	},
	{ { "   f8  key ",	"   f8  キー ",		},  8,	},
	{ { "   f9  key ",	"   f9  キー ",		},  9,	},
	{ { "   f10 key ",	"   f10 キー ",		}, 10,	},
};
static const t_menudata data_key_fkey_fn[] = {
	{ { "----------- : function or another key",	"----------- : ファンクションまたは任意キー",	},  FN_FUNC,		},
	{ { "FRATE-UP    : Frame Rate  Up",				"FRATE-UP    : フレームレート 上げる ",			},  FN_FRATE_UP,	},
	{ { "FRATE-DOWN  : Frame Rate  Down",			"FRATE-DOWN  : フレームレート 下げる ",			},  FN_FRATE_DOWN,	},
	{ { "VOLUME-UP   : Volume  Up",					"VOLUME-UP   : 音量 上げる",					},  FN_VOLUME_UP,	},
	{ { "VOLUME-DOWN : Volume  Down",				"VOLUME-DOWN : 音量 下げる",					},  FN_VOLUME_DOWN,	},
	{ { "PAUSE       : Pause",						"PAUSE       : 一時停止",						},  FN_PAUSE,		},
	{ { "RESIZE      : Resize",						"RESIZE      : 画面サイズ変更",					},  FN_RESIZE,		},
	{ { "NOWAIT      : No-Wait",					"NOWAIT      : ウエイトなし",					},  FN_NOWAIT,		},
	{ { "SPEED-UP    : Speed Up",					"SPEED-UP    : 速度 上げる ",					},  FN_SPEED_UP,	},
	{ { "SPEED-DOWN  : Speed Down",					"SPEED-DOWN  : 速度 下げる ",					},  FN_SPEED_DOWN,	},
	{ { "FULLSCREEN  : Full Screen Mode",			"FULLSCREEN  : フルスクリーン切替",				},  FN_FULLSCREEN,	},
	{ { "SNAPSHOT    : Save Screen Snapshot",		"SNAPSHOT    : スクリーンスナップショット",		},  FN_SNAPSHOT,	},
	{ { "IMAGE-NEXT1 : Drive 1:  Next Image",		"IMAGE-NEXT1 : Drive 1:  次イメージ",			},  FN_IMAGE_NEXT1,	},
	{ { "IMAGE-PREV1 : Drive 1:  Prev Image",		"IMAGE-PREV1 : Drive 1:  前イメージ",			},  FN_IMAGE_PREV1,	},
	{ { "IMAGE-NEXT2 : Drive 2:  Next Image",		"IMAGE-NEXT2 : Drive 2:  次イメージ",			},  FN_IMAGE_NEXT2,	},
	{ { "IMAGE-PREV2 : Drive 2:  Prev Image",		"IMAGE-PREV2 : Drive 2:  前イメージ",			},  FN_IMAGE_PREV2,	},
	{ { "NUMLOCK     : Software NUM Lock",			"NUMLOCK     : ソフトウェア NUM Lock",			},  FN_NUMLOCK,		},
	{ { "RESET       : Reset switch",				"RESET       : リセット スイッチ",				},  FN_RESET,		},
	{ { "KANA        : KANA key",					"KANA        : カナ キー",						},  FN_KANA,		},
	{ { "ROMAJI      : KANA(ROMAJI) Key",			"ROMAJI      : カナ(ローマ字入力) キー",		},  FN_ROMAJI,		},
	{ { "CAPS        : CAPS Key",					"CAPS        : CAPS キー",						},  FN_CAPS,		},
	{ { "MAX-SPEED   : Max Speed",					"MAX-SPEED   : 速度最大設定値",					},  FN_MAX_SPEED,	},
	{ { "MAX-CLOCK   : Max CPU-Clock",				"MAX-CLOCK   : CPUクロック最大設定値",			},  FN_MAX_CLOCK,	},
	{ { "MAX-BOOST   : Max Boost",					"MAX-BOOST   : ブースト最大設定値",				},  FN_MAX_BOOST,	},
	{ { "STATUS      : Display status",				"STATUS      : ステータス表示のオン／オフ",		},  FN_STATUS,		},
	{ { "MENU        : Go Menu-Mode",				"MENU        : メニュー",						},  FN_MENU,		},
};



static const t_menudata data_key_fkey2[] = {
	{ { "   ",	"   ",	},  6,	},
	{ { "   ",	"   ",	},  7,	},
	{ { "   ",	"   ",	},  8,	},
	{ { "   ",	"   ",	},  9,	},
	{ { "   ",	"   ",	}, 10,	},
};



enum {
	DATA_SKEY_BUTTON_SETUP,
	DATA_SKEY_BUTTON_OFF,
	DATA_SKEY_BUTTON_QUIT
};
static const t_menulabel data_skey_set[] = {
	{ { "Setting",					" 設定 ",				}, },
	{ { "All key release & QUIT",	" 全てオフにして戻る ",	}, },
	{ { " QUIT ",					" 戻る ",				}, },
};



static const t_menudata data_key_cursor_mode[] = {
	{ { " Default(CursorKey)",		" 標準(カーソルキー)",		},  0,	},
	{ { " Assign to 2,4,6,8",		" 2,4,6,8 を割り当て",		},  1,	},
	{ { " Assign arbitrarily ",		" 任意のキーを割り当て ",	},  2,	},
};
static const t_menudata data_key_cursor[] = {
	{ { "             ",			"             ",		},   0,	},
	{ { "                \036",		"               ↑",	},  -1,	},
	{ { " ",						" ",					},   2,	},
	{ { "\035           \034 ",		"←        → ",		},   3,	},
	{ { "                \037",		"               ↓",	},  -1,	},
	{ { "             ",			"             ",		},   1,	},
};


/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/

/* キーボード配列 (新旧別) のウィジット生成用データ */

static const t_keymap keymap_old0[] = {
	{ "STOP",		KEY88_STOP,		},
	{ "COPY",		KEY88_COPY,		},
	{ " ",			0,				},
	{ "   f1   ",	KEY88_F1,		},
	{ "   f2   ",	KEY88_F2,		},
	{ "   f3   ",	KEY88_F3,		},
	{ "   f4   ",	KEY88_F4,		},
	{ "   f5   ",	KEY88_F5,		},
	{ "   ",		0,				},
	{ "R-UP",		KEY88_ROLLUP,	},
	{ "R-DN",		KEY88_ROLLDOWN,	},
	{ "   ",		0,				},
	{ " \036 ",		KEY88_UP,		},
	{ " \037 ",		KEY88_DOWN,		},
	{ " \035 ",		KEY88_LEFT,		},
	{ " \034 ",		KEY88_RIGHT,	},
	{ 0,			0,				},
};
static const t_keymap keymap_old1[] = {
	{ " ESC ",		KEY88_ESC,			},
	{ " 1 ",		KEY88_1,			},
	{ " 2 ",		KEY88_2,			},
	{ " 3 ",		KEY88_3,			},
	{ " 4 ",		KEY88_4,			},
	{ " 5 ",		KEY88_5,			},
	{ " 6 ",		KEY88_6,			},
	{ " 7 ",		KEY88_7,			},
	{ " 8 ",		KEY88_8,			},
	{ " 9 ",		KEY88_9,			},
	{ " 0 ",		KEY88_0,			},
	{ " - ",		KEY88_MINUS,		},
	{ " ^ ",		KEY88_CARET,		},
	{ " \\ ",		KEY88_YEN,			},
	{ " BS ",		KEY88_INS_DEL,		},
	{ "   ",		0,					},
	{ "CLR",		KEY88_HOME,			},
	{ "HLP",		KEY88_HELP,			},
	{ " - ",		KEY88_KP_SUB,		},
	{ " / ",		KEY88_KP_DIVIDE,	},
	{ 0,			0,					},
};
static const t_keymap keymap_old2[] = {
	{ "  TAB  ",	KEY88_TAB,			},
	{ " Q ",		KEY88_q,			},
	{ " W ",		KEY88_w,			},
	{ " E ",		KEY88_e,			},
	{ " R ",		KEY88_r,			},
	{ " T ",		KEY88_t,			},
	{ " Y ",		KEY88_y,			},
	{ " U ",		KEY88_u,			},
	{ " I ",		KEY88_i,			},
	{ " O ",		KEY88_o,			},
	{ " P ",		KEY88_p,			},
	{ " @ ",		KEY88_AT,			},
	{ " [ ",		KEY88_BRACKETLEFT,	},
	{ "RETURN ",	KEY88_RETURN,		},
	{ "   ",		0,					},
	{ " 7 ",		KEY88_KP_7,			},
	{ " 8 ",		KEY88_KP_8,			},
	{ " 9 ",		KEY88_KP_9,			},
	{ " * ",		KEY88_KP_MULTIPLY,	},
	{ 0,			0,					},
};
static const t_keymap keymap_old3[] = {
	{ "CTRL",		KEY88_CTRL,			},
	{ "CAPS",		KEY88_CAPS,			},
	{ " A ",		KEY88_a,			},
	{ " S ",		KEY88_s,			},
	{ " D ",		KEY88_d,			},
	{ " F ",		KEY88_f,			},
	{ " G ",		KEY88_g,			},
	{ " H ",		KEY88_h,			},
	{ " J ",		KEY88_j,			},
	{ " K ",		KEY88_k,			},
	{ " L ",		KEY88_l,			},
	{ " ; ",		KEY88_SEMICOLON,	},
	{ " : ",		KEY88_COLON,		},
	{ " ] ",		KEY88_BRACKETRIGHT,	},
	{ "         ",	0,					},
	{ " 4 ",		KEY88_KP_4,			},
	{ " 5 ",		KEY88_KP_5,			},
	{ " 6 ",		KEY88_KP_6,			},
	{ " + ",		KEY88_KP_ADD,		},
	{ 0,			0,					},
};
static const t_keymap keymap_old4[] = {
	{ "    SHIFT   ",	KEY88_SHIFT,		},
	{ " Z ",			KEY88_z,			},
	{ " X ",			KEY88_x,			},
	{ " C ",			KEY88_c,			},
	{ " V ",			KEY88_v,			},
	{ " B ",			KEY88_b,			},
	{ " N ",			KEY88_n,			},
	{ " M ",			KEY88_m,			},
	{ " , ",			KEY88_COMMA,		},
	{ " . ",			KEY88_PERIOD,		},
	{ " / ",			KEY88_SLASH,		},
	{ " _ ",			KEY88_UNDERSCORE,	},
	{ " SHIFT ",		KEY88_SHIFT,		},
	{ "   ",			0,					},
	{ " 1 ",			KEY88_KP_1,			},
	{ " 2 ",			KEY88_KP_2,			},
	{ " 3 ",			KEY88_KP_3,			},
	{ " = ",			KEY88_KP_EQUAL,		},
	{ 0,				0,					},
};
static const t_keymap keymap_old5[] = {
	{ "       ",										0,					},
	{ "KANA",											KEY88_KANA,			},
	{ "GRPH",											KEY88_GRAPH,		},
	{ "                                           ",	KEY88_SPACE,		},
	{ "                 ",								0,					},
	{ " 0 ",											KEY88_KP_0,			},
	{ " , ",											KEY88_KP_COMMA,		},
	{ " . ",											KEY88_KP_PERIOD,	},
	{ "RET",											KEY88_RETURN,		},
	{ 0,												0,					},
};

static const t_keymap keymap_new0[] = {
	{ "STOP",		KEY88_STOP,		},
	{ "COPY",		KEY88_COPY,		},
	{ "  ",			0,				},
	{ " f1 ",		KEY88_F1,		},
	{ " f2 ",		KEY88_F2,		},
	{ " f3 ",		KEY88_F3,		},
	{ " f4 ",		KEY88_F4,		},
	{ " f5 ",		KEY88_F5,		},
	{ "   ",		0,				},
	{ " f6 ",		KEY88_F6,		},
	{ " f7 ",		KEY88_F7,		},
	{ " f8 ",		KEY88_F8,		},
	{ " f9 ",		KEY88_F9,		},
	{ " f10 ",		KEY88_F10,		},
	{ "   ",		0,				},
	{ "ROLUP",		KEY88_ROLLUP,	},
	{ "ROLDN",		KEY88_ROLLDOWN,	},
	{ 0,			0,				},
};
static const t_keymap keymap_new1[] = {
	{ " ESC ",		KEY88_ESC,			},
	{ " 1 ",		KEY88_1,			},
	{ " 2 ",		KEY88_2,			},
	{ " 3 ",		KEY88_3,			},
	{ " 4 ",		KEY88_4,			},
	{ " 5 ",		KEY88_5,			},
	{ " 6 ",		KEY88_6,			},
	{ " 7 ",		KEY88_7,			},
	{ " 8 ",		KEY88_8,			},
	{ " 9 ",		KEY88_9,			},
	{ " 0 ",		KEY88_0,			},
	{ " - ",		KEY88_MINUS,		},
	{ " ^ ",		KEY88_CARET,		},
	{ " \\ ",		KEY88_YEN,			},
	{ " BS ",		KEY88_BS,			},
	{ "   ",		0,					},
	{ " DEL ",		KEY88_DEL,			},
	{ " INS ",		KEY88_INS,			},
	{ "  ",			0,					},
	{ "CLR",		KEY88_HOME,			},
	{ "HLP",		KEY88_HELP,			},
	{ " - ",		KEY88_KP_SUB,		},
	{ " / ",		KEY88_KP_DIVIDE,	},
	{ 0,			0,					},
};
static const t_keymap keymap_new2[] = {
	{ "  TAB  ",				KEY88_TAB,			},
	{ " Q ",					KEY88_q,			},
	{ " W ",					KEY88_w,			},
	{ " E ",					KEY88_e,			},
	{ " R ",					KEY88_r,			},
	{ " T ",					KEY88_t,			},
	{ " Y ",					KEY88_y,			},
	{ " U ",					KEY88_u,			},
	{ " I ",					KEY88_i,			},
	{ " O ",					KEY88_o,			},
	{ " P ",					KEY88_p,			},
	{ " @ ",					KEY88_AT,			},
	{ " [ ",					KEY88_BRACKETLEFT,	},
	{ "RETURN ",				KEY88_RETURNL,		},
	{ "                   ",	0,					},
	{ " 7 ",					KEY88_KP_7,			},
	{ " 8 ",					KEY88_KP_8,			},
	{ " 9 ",					KEY88_KP_9,			},
	{ " * ",					KEY88_KP_MULTIPLY,	},
	{ 0,						0,					},
};
static const t_keymap keymap_new3[] = {
	{ "CTRL",			KEY88_CTRL,			},
	{ "CAPS",			KEY88_CAPS,			},
	{ " A ",			KEY88_a,			},
	{ " S ",			KEY88_s,			},
	{ " D ",			KEY88_d,			},
	{ " F ",			KEY88_f,			},
	{ " G ",			KEY88_g,			},
	{ " H ",			KEY88_h,			},
	{ " J ",			KEY88_j,			},
	{ " K ",			KEY88_k,			},
	{ " L ",			KEY88_l,			},
	{ " ; ",			KEY88_SEMICOLON,	},
	{ " : ",			KEY88_COLON,		},
	{ " ] ",			KEY88_BRACKETRIGHT,	},
	{ "             ",	0,					},
	{ " \036 ",			KEY88_UP,			},
	{ "       ",		0,					},
	{ " 4 ",			KEY88_KP_4,			},
	{ " 5 ",			KEY88_KP_5,			},
	{ " 6 ",			KEY88_KP_6,			},
	{ " + ",			KEY88_KP_ADD,		},
	{ 0,				0,					},
};
static const t_keymap keymap_new4[] = {
	{ "    SHIFT   ",	KEY88_SHIFTL,		},
	{ " Z ",			KEY88_z,			},
	{ " X ",			KEY88_x,			},
	{ " C ",			KEY88_c,			},
	{ " V ",			KEY88_v,			},
	{ " B ",			KEY88_b,			},
	{ " N ",			KEY88_n,			},
	{ " M ",			KEY88_m,			},
	{ " , ",			KEY88_COMMA,		},
	{ " . ",			KEY88_PERIOD,		},
	{ " / ",			KEY88_SLASH,		},
	{ " _ ",			KEY88_UNDERSCORE,	},
	{ " SHIFT ",		KEY88_SHIFTR,		},
	{ "  ",				0,					},
	{ " \035 ",			KEY88_LEFT,			},
	{ " \037 ",			KEY88_DOWN,			},
	{ " \034 ",			KEY88_RIGHT,		},
	{ "  ",				0,					},
	{ " 1 ",			KEY88_KP_1,			},
	{ " 2 ",			KEY88_KP_2,			},
	{ " 3 ",			KEY88_KP_3,			},
	{ " = ",			KEY88_KP_EQUAL,		},
	{ 0,				0,					},
};
static const t_keymap keymap_new5[] = {
	{ "       ",							0,					},
	{ "KANA",								KEY88_KANA,			},
	{ "GRPH",								KEY88_GRAPH,		},
	{ " KETTEI ",							KEY88_KETTEI,		},
	{ "           ",						KEY88_SPACE,		},
	{ "  HENKAN  ",							KEY88_HENKAN,		},
	{ "PC ",								KEY88_PC,			},
	{ "ZEN",								KEY88_ZENKAKU,		},
	{ "                                 ",	0               	},
	{ " 0 ",								KEY88_KP_0,			},
	{ " , ",								KEY88_KP_COMMA,		},
	{ " . ",								KEY88_KP_PERIOD,	},
	{ "RET",								KEY88_RETURNR,		},
	{ 0,									0,					},
};

static const t_keymap *keymap_line[2][6] = {
	{
		keymap_old0,
		keymap_old1,
		keymap_old2,
		keymap_old3,
		keymap_old4,
		keymap_old5,
	},
	{
		keymap_new0,
		keymap_new1,
		keymap_new2,
		keymap_new3,
		keymap_new4,
		keymap_new5,
	},
};

#endif /* MENU_KEY_MESSAGE_H_INCLUDED */
