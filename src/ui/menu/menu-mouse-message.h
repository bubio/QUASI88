#ifndef MENU_MOUSE_MESSAGE_H_INCLUDED
#define MENU_MOUSE_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「マウス」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_MOUSE_MODE,
	DATA_MOUSE_SERIAL,

	DATA_MOUSE_SYSTEM,

	DATA_MOUSE_DEVICE_MOUSE,
	DATA_MOUSE_DEVICE_JOY,
	DATA_MOUSE_DEVICE_JOY2,
	DATA_MOUSE_DEVICE_ABOUT,

	DATA_MOUSE_DEVICE_NUM,

	DATA_MOUSE_CONNECTING,
	DATA_MOUSE_SWAP_MOUSE,
	DATA_MOUSE_SWAP_JOY,
	DATA_MOUSE_SWAP_JOY2
};
static const t_menulabel data_mouse[] = {
	{ { " Mouse / Joystick setting ",			" マウス／ジョイスティック接続 ",	}, },
	{ { " Serial-mouse ",						" シリアルマウス ",					}, },

	{ { " [ System Setup  (Some settings are disabled in some systems.)]               ",	" 【システム設定  (設定の一部は、システムによっては無効です)】                 ",	}, },

	{ { " Mouse ",								" マウス入力 ",						}, },
	{ { " Joystick ",							" ジョイスティック入力 ",			}, },
	{ { " Joystick(2) ",						" ジョイスティック(2)入力 ",		}, },
	{ { " About ",								" ※ ",								}, },

	{ { " %d Joystick(s) is found.",			"  %d 個のジョイスティックが使用できます。",	}, },

	{ { "  Connecting mouse-port   ",			"  マウスポートに接続中    ",					}, },
	{ { "Swap mouse buttons",					"左右ボタンを入れ替える",						}, },
	{ { "Swap joystick buttons (-joy_swap)",	"ＡＢボタンを入れ替える (-joyswap)",			}, },
	{ { "Swap joystick buttons",				"ＡＢボタンを入れ替える",						}, },
};



static const t_menudata data_mouse_mode[] = {
	{ { "Not Connect               (-nomouse) ",	"なにも接続しない                     (-nomouse) ",	}, MOUSE_NONE,		},
	{ { "Connect Mouse             (-mouse)   ",	"マウスを接続                         (-mouse)   ",	}, MOUSE_MOUSE,		},
	{ { "Connect Mouse as joystick (-joymouse)",	"マウスをジョイスティックモードで接続 (-joymouse)",	}, MOUSE_JOYMOUSE,	},
	{ { "Connect joystick          (-joystick)",	"ジョイスティックを接続               (-joystick)",	}, MOUSE_JOYSTICK,	},
};



static const t_menulabel data_mouse_serial[] = {
	{ { "Connect (-serialmouse)",	"接続 (-serialmouse)",	}, },
};



static const t_menudata data_mouse_mouse_key_mode[] = {
	{ { " Not Assigned ",			" キー割り当てなし",		},  0,	},
	{ { " Assign to 2,4,6,8,x,z ",	" 2,4,6,8,x,zを割り当て ",	},  1,	},
	{ { " Assign arbitrarily    ",	" 任意のキーを割り当て",	},  2,	},
};
static const t_menudata data_mouse_mouse[] = {
	{ { "             ",		"             ",		},   0,	},
	{ { "                \036",	"               ↑",	},  -1,	},
	{ { " ",					" ",					},   2,	},
	{ { "\035           \034 ",	"←        → ",		},   3,	},
	{ { "                \037",	"               ↓",	},  -1,	},
	{ { "             ",		"             ",		},   1,	},
	{ { "",						"",						},  -1,	},
	{ { " L ",					" 左 ",					},   4,	},
	{ { "",						"",						},  -1,	},
	{ { " R ",					" 右 ",					},   5, },
};



static const t_menudata data_mouse_joy_key_mode[] = {
	{ { " Not Assigned ",			" キー割り当てなし",		},  0,	},
	{ { " Assign to 2,4,6,8,x,z ",	" 2,4,6,8,x,zを割り当て ",	},  1,	},
	{ { " Assign arbitrarily    ",	" 任意のキーを割り当て",	},  2,	},
};
static const t_menudata data_mouse_joy[] = {
	{ { "             ",			"             ",		},   0,	},
	{ { "                \036",		"               ↑",	},  -1,	},
	{ { " ",						" ",					},   2,	},
	{ { "\035           \034 ",		"←        → ",		},   3,	},
	{ { "                \037",		"               ↓",	},  -1,	},
	{ { "             ",			"             ",		},   1,	},
	{ { " A ",						" Ａ ",					},   4,	},
	{ { " B ",						" Ｂ ",					},   5,	},
	{ { " C ",						" Ｃ ",					},   6,	},
	{ { " D ",						" Ｄ ",					},   7,	},
	{ { " E ",						" Ｅ ",					},   8,	},
	{ { " F ",						" Ｆ ",					},   9,	},
	{ { " G ",						" Ｇ ",					},  10,	},
	{ { " H ",						" Ｈ ",					},  11,	},
};
static const t_menudata data_mouse_joy2_key_mode[] = {
	{ { " Not Assigned ",			" キー割り当てなし",		},  0,	},
	{ { " Assign to 2,4,6,8,x,z ",	" 2,4,6,8,x,zを割り当て ",	},  1,	},
	{ { " Assign arbitrarily    ",	" 任意のキーを割り当て",	},  2,	},
};
static const t_menudata data_mouse_joy2[] = {
	{ { "             ",			"             ",		},   0,	},
	{ { "                \036",		"               ↑",	},  -1,	},
	{ { " ",						" ",					},   2,	},
	{ { "\035           \034 ",		"←        → ",		},   3,	},
	{ { "                \037",		"               ↓",	},  -1,	},
	{ { "             ",			"             ",		},   1,	},
	{ { " A ",						" Ａ ",					},   4,	},
	{ { " B ",						" Ｂ ",					},   5,	},
	{ { " C ",						" Ｃ ",					},   6,	},
	{ { " D ",						" Ｄ ",					},   7,	},
	{ { " E ",						" Ｅ ",					},   8,	},
	{ { " F ",						" Ｆ ",					},   9,	},
	{ { " G ",						" Ｇ ",					},  10,	},
	{ { " H ",						" Ｈ ",					},  11,	},
};



static const t_volume data_mouse_sensitivity[] = {
	{ { " Sensitivity [%] :",	" マウス感度 [％]：",	}, -1, 10, 200, 1, 10},
};



static const t_menulabel data_mouse_misc_msg[] = {
	{ { " Mouse Cursor : ",	" マウスカーソルを ",	}, },
};
static const t_menudata data_mouse_misc[] = {
	{ { "Always show the mouse cursor            (-show_mouse) ",	"常に表示する             (-show_mouse) ",	}, SHOW_MOUSE,	},
	{ { "Always Hide the mouse cursor            (-hide_mouse) ",	"常に隠す                 (-hide_mouse) ",	}, HIDE_MOUSE,	},
	{ { "Auto-hide the mouse cutsor              (-auto_mouse) ",	"自動的に隠す             (-auto_mouse) ",	}, AUTO_MOUSE,	},
	{ { "Confine the mouse cursor on the screend (-grab_mouse) ",	"画面に閉じ込める（隠す） (-grab_mouse) ",	}, -1        ,	},
	{ { "Confine the mouse cursor when mouse clicked           ",	"クリックで閉じ込める     (-auto_grab)  ",	}, -2        ,	},
};



static const t_menudata data_mouse_debug_hide[] = {
	{ { " SHOW ",	" 表示 ",	}, SHOW_MOUSE,		},
	{ { " HIDE ",	" 隠す ",	}, HIDE_MOUSE,		},
	{ { " AUTO ",	" 自動 ",	}, AUTO_MOUSE,		},
};
static const t_menudata data_mouse_debug_grab[] = {
	{ { " UNGRAB ",	" 離す ",	}, UNGRAB_MOUSE,	},
	{ { " GRAB   ",	" 掴む ",	}, GRAB_MOUSE,		},
	{ { " AUTO   ",	" 自動 ",	}, AUTO_MOUSE,		},
};

#endif /* MENU_MOUSE_MESSAGE_H_INCLUDED */
