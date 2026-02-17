#ifndef MENU_TOP_MESSAGE_H_INCLUDED
#define MENU_TOP_MESSAGE_H_INCLUDED

/***************************************************************
 * QUASI88 の メニューにて使用する文字列
 ****************************************************************/

/*--------------------------------------------------------------
 * メインメニュー画面
 *--------------------------------------------------------------*/
enum {
	DATA_TOP_RESET,
	DATA_TOP_CPU,
	DATA_TOP_GRAPH,
	DATA_TOP_VOLUME,
	DATA_TOP_DISK,
	DATA_TOP_KEY,
	DATA_TOP_MOUSE,
	DATA_TOP_TAPE,
	DATA_TOP_MISC,
	DATA_TOP_ABOUT
};
static const t_menudata data_top[] = {
	{ { " RESET ",	"リセット",	}, DATA_TOP_RESET,	},
	{ { " CPU ",	" CPU  ",	}, DATA_TOP_CPU,	},
	{ { " SCREEN ",	" 画面 ",	}, DATA_TOP_GRAPH,	},
	{ { " VOLUME ",	" 音量 ",	}, DATA_TOP_VOLUME,	},
	{ { " DISK ",	"ディスク",	}, DATA_TOP_DISK,	},
	{ { " KEY ",	" キー ",	}, DATA_TOP_KEY,	},
	{ { " MOUSE ",	" マウス ",	}, DATA_TOP_MOUSE,	},
	{ { " TAPE ",	" テープ ",	}, DATA_TOP_TAPE,	},
	{ { " MISC ",	"  他  ",	}, DATA_TOP_MISC,	},
	{ { " ABOUT ",	"※",		}, DATA_TOP_ABOUT,	},
};


enum {
	DATA_TOP_STATUS_TOOLBAR,
	DATA_TOP_STATUS_STATUS,
	DATA_TOP_STATUS_LABEL
};
static const t_menulabel data_top_status[] = {
	{ { "Toolbar",			"ツールバー",		} },
	{ { "Status",			"ステータス",		} },
	{ { "        (F11) ",	"         (F11)",	} },
};


enum {
	DATA_TOP_MONITOR_PAD,
	DATA_TOP_MONITOR_BTN
};
static const t_menulabel data_top_monitor[] = {
	{ { "            ",	"            ",	} },
	{ { " MONITOR  ",	" モニター ",	} },
};


enum {
	DATA_TOP_SAVECFG,
	DATA_TOP_QUIT,
	DATA_TOP_EXIT
};
static const t_menudata data_top_button[] = {
	{ { " Save Cfg.",	" 設定保存 ",	}, DATA_TOP_SAVECFG,	},
	{ { " QUIT(F12) ",	" 終了(F12) ",	}, DATA_TOP_QUIT,		},
	{ { " EXIT(ESC) ",	" 戻る(ESC) ",	}, DATA_TOP_EXIT,		},
};


enum {
	DATA_TOP_SAVECFG_TITLE,
	DATA_TOP_SAVECFG_INFO,
	DATA_TOP_SAVECFG_AUTO,
	DATA_TOP_SAVECFG_OK,
	DATA_TOP_SAVECFG_CANCEL
};
static const t_menulabel data_top_savecfg[] = {
	{ { "Save settings in following file. ",	"現在の設定を、以下の環境設定ファイルに保存します",	} },
	{ { "(Some settings are not saved)    ",	"（一部の設定は、保存されません）                ",	} },
	{ { "Save when QUASI88 exit. ",				"終了時に、自動で保存する",							} },
	{ { "   OK   ",								" 保存 ",											} },
	{ { " CANCEL ",								" 取消 ",											} },
};


enum {
	DATA_TOP_QUIT_TITLE,
	DATA_TOP_QUIT_OK,
	DATA_TOP_QUIT_CANCEL
};
static const t_menulabel data_top_quit[] = {
	{ { " *** QUIT NOW, REALLY ? *** ",	"本当に終了して、よろしいか？",	} },
	{ { "   OK   (F12) ",				" 終了 (F12) ",					} },
	{ { " CANCEL (ESC) ",				" 取消 (ESC) ",					} },
};


static const t_menudata data_quickres_basic[] = {
	{ { "V2 ",	"V2 ",	}, BASIC_V2,	},
	{ { "V1H",	"V1H",	}, BASIC_V1H,	},
	{ { "V1S",	"V1S",	}, BASIC_V1S,	},
	{ { "N",	"N",	}, BASIC_N,		}, /* 非表示… */
};
static const t_menudata data_quickres_clock[] = {
	{ { "4MHz",	"4MHz",	}, CLOCK_4MHZ,	},
	{ { "8MHz",	"8MHz",	}, CLOCK_8MHZ,	},
};
static const t_menulabel data_quickres_reset[] = {
	{ { "RST",	"RST",	} },
};

#endif /* MENU_TOP_MESSAGE_H_INCLUDED */
