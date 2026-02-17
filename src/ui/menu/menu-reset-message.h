#ifndef MENU_RESET_MESSAGE_H_INCLUDED
#define MENU_RESET_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「リセット」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_RESET_CURRENT,
	DATA_RESET_BASIC,
	DATA_RESET_CLOCK,
	DATA_RESET_VERSION,
	DATA_RESET_DIPSW,
	DATA_RESET_DIPSW_BTN,
	DATA_RESET_SOUND,
	DATA_RESET_EXTRAM,
	DATA_RESET_JISHO,
	DATA_RESET_NOTICE,
	DATA_RESET_DIPSW_SET,
	DATA_RESET_DIPSW_QUIT,
	DATA_RESET_BOOT,
	DATA_RESET_NOW,
	DATA_RESET_INFO
};
static const t_menulabel data_reset[] = {
	{ { " Current Mode : ",			" 現在のモード ：",					}, },
	{ { " BASIC MODE ",				" BASIC モード ",					}, },
	{ { " CPU CLOCK ",				" CPU クロック ",					}, },
	{ { " ROM VERSION ",			" ROM バージョン ",					}, },
	{ { " DIP-Switch ",				" ディップスイッチ ",				}, },
	{ { " Setting ",				" 設定 ",							}, },
	{ { " Sound Board",				" サウンドボード ",					}, },
	{ { " ExtRAM",					" 拡張RAM ",						}, },
	{ { " Dict.ROM",				" 辞書ROM ",						}, },
	{ { "(*) When checked, Real CPU clock depend on the 'CPU'-TAB setting. ",	"(＊) チェックが入っている場合、実際のクロックは『CPU』タブ設定のままとなります",	}, },
	{ { " <<< DIP-SW Setting >>> ",	" <<< ディップスイッチ設定 >>> ",	}, },
	{ { " EXIT ",					" 戻る ",							}, },
	{ { " BOOT ",					" 起動 ",							}, },
	{ { " RESET now ! ",			" この設定でリセットする ",			}, },
	{ { " (Without a reset, the setting is not applied.) ",						" （リセットをしないと、設定は反映されません）",									}, },
};

static const t_menudata data_reset_basic[] = {
	{ { " N88 V2  ",	" N88 V2  ",	}, BASIC_V2,	},
	{ { " N88 V1H ",	" N88 V1H ",	}, BASIC_V1H,	},
	{ { " N88 V1S ",	" N88 V1S ",	}, BASIC_V1S,	},
	{ { " N       ",	" N       ",	}, BASIC_N,		},
};

static const t_menudata data_reset_clock[] = {
	{ { " 4MHz ",		" 4MHz ",		}, CLOCK_4MHZ,	},
	{ { " 8MHz ",		" 8MHz ",		}, CLOCK_8MHZ,	},
};

static const t_menulabel data_reset_clock_async[] = {
	{ { "Async (*) ",	"非連動 (＊) ",	}, },
};

static const t_menudata data_reset_version[] = {
	{ { "Default",	" 既定値",	}, 0,	},
	{ { "  1.0",	"  1.0",	}, '0',	},
	{ { "  1.1",	"  1.1",	}, '1',	},
	{ { "  1.2",	"  1.2",	}, '2',	},
	{ { "  1.3",	"  1.3",	}, '3',	},
	{ { "  1.4",	"  1.4",	}, '4',	},
	{ { "  1.5",	"  1.5",	}, '5',	},
	{ { "  1.6",	"  1.6",	}, '6',	},
	{ { "  1.7",	"  1.7",	}, '7',	},
	{ { "  1.8",	"  1.8",	}, '8',	},
	{ { "  1.9*",	"  1.9*",	}, '9',	},
};

static const t_menulabel data_reset_boot[] = {
	{ { " Boot from DISK  ",	"  ディスク ",	}, },
	{ { " Boot from ROM   ",	"  ＲＯＭ   ",	}, },
};

static const t_menudata data_reset_sound[] = {
	{ { " Sound board    (OPN)  ",	" サウンドボード   (OPN)  ",	}, SOUND_I,		},
	{ { " Sound board II (OPNA) ",	" サウンドボードII (OPNA) ",	}, SOUND_II,	},
};

static const t_menudata data_reset_extram[] = {
	{ { " Nothing ",	" なし    ",	},  0,	},
	{ { "    128KB",	"    128KB",	},  1,	},
	{ { "    256KB",	"    256KB",	},  2,	},
	{ { "    384KB",	"    384KB",	},  3,	},
	{ { "    512KB",	"    512KB",	},  4,	},
	{ { "      1MB",	"      1MB",	},  8,	},
	{ { " 1M+128KB",	" 1M+128KB",	},  9,	},
	{ { " 1M+256KB",	" 1M+256KB",	}, 10,	},
	{ { "      2MB",	"      2MB",	}, 16,	},
};

static const t_menudata data_reset_jisho[] = {
	{ { " no-jisho ",	" なし ",		}, 0,	},
	{ { " has jisho ",	" あり ",		}, 1,	},
};

static const t_menulabel data_reset_current[] = {
	{ { " ExtRAM",		"拡張RAM",		}, },
	{ { "DictROM",		"辞書ROM",		}, },
};

static const t_menulabel data_reset_detail[] = {
	{ { " Misc. << ",	" その他 << ",	}, },
	{ { " Misc. >> ",	" その他 >> ",	}, },
};


/*--------------------------------------------------------------
 * 「DIP-SW」
 *--------------------------------------------------------------*/
enum {
	DATA_DIPSW_B,
	DATA_DIPSW_R
};
static const t_menulabel data_dipsw[] = {
	{ { " Boot up ",	" 初期設定 ",		}, },
	{ { " RC232C ",		" RS232C 設定 ",	}, },
};


typedef struct {
	char  *str[2];
	int   val;
	const t_menudata *p;
} t_dipsw;




static const t_menudata data_dipsw_b_term[] = {
	{ { "TERMINAL   ",	"ターミナル ",	}, (0 << 1) | 0,	},
	{ { "BASIC      ",	"ＢＡＳＩＣ ",	}, (0 << 1) | 1,	},
};
static const t_menudata data_dipsw_b_ch80[] = {
	{ { "80ch / line",	"８０字     ",	}, (1 << 1) | 0,	},
	{ { "40ch / line",	"４０字     ",	}, (1 << 1) | 1,	},
};
static const t_menudata data_dipsw_b_ln25[] = {
	{ { "25line/scrn",	"２５行     ",	}, (2 << 1) | 0,	},
	{ { "20line/scrn",	"２０行     ",	}, (2 << 1) | 1,	},
};
static const t_menudata data_dipsw_b_boot[] = {
	{ { "DISK       ",	"ディスク   ",	}, FALSE,			},
	{ { "ROM        ",	"ＲＯＭ     ",	}, TRUE,			},
};

static const t_dipsw data_dipsw_b[] = {
	{ { "BOOT MODE           :",	"立ち上げモード     ：",	},  1,	data_dipsw_b_term,	},
	{ { "Chars per Line      :",	"１行あたりの文字数 ：",	},  2,	data_dipsw_b_ch80,	},
	{ { "Lines per screen    :",	"１画面あたりの行数 ：",	},  3,	data_dipsw_b_ln25,	},
};
static const t_dipsw data_dipsw_b2[] = {
	{ { "Boot Up from        :",	"システムの立ち上げ ：",	}, -1,	data_dipsw_b_boot,	},
};



static const t_menudata data_dipsw_r_baudrate[] = {
	{ {    "75",	"75",		}, 0,	},
	{ {   "150",	"150",		}, 1,	},
	{ {   "300",	"300",		}, 2,	},
	{ {   "600",	"600",		}, 3,	},
	{ {  "1200",	"1200",		}, 4,	},
	{ {  "2400",	"2400",		}, 5,	},
	{ {  "4800",	"4800",		}, 6,	},
	{ {  "9600",	"9600",		}, 7,	},
	{ { "19200",	"19200",	}, 8,	},
};


static const t_menudata data_dipsw_r_hdpx[] = {
	{ { "HALF       ",	"半二重     ",	}, (0 << 1) | 0,	},
	{ { "FULL       ",	"全二重     ",	}, (0 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_xprm[] = {
	{ { "Enable     ",	"有  効     ",	}, (1 << 1) | 0,	},
	{ { "Disable    ",	"無  効     ",	}, (1 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_st2b[] = {
	{ { "2 bit      ",	"２ bit     ",	}, (2 << 1) | 0,	},
	{ { "1 bit      ",	"１ bit     ",	}, (2 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_dt8b[] = {
	{ { "8 bit      ",	"８ bit     ",	}, (3 << 1) | 0,	},
	{ { "7 bit      ",	"７ bit     ",	}, (3 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_sprm[] = {
	{ { "Enable     ",	"有  効     ",	}, (4 << 1) | 0,	},
	{ { "Disable    ",	"無  効     ",	}, (4 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_pdel[] = {
	{ { "Enable     ",	"有  効     ",	}, (5 << 1) | 0,	},
	{ { "Disable    ",	"無  効     ",	}, (5 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_enpty[] = {
	{ { "Yes        ",	"有  り     ",	}, (6 << 1) | 0,	},
	{ { "No         ",	"無  し     ",	}, (6 << 1) | 1,	},
};
static const t_menudata data_dipsw_r_evpty[] = {
	{ { "Even       ",	"偶  数     ",	}, (7 << 1) | 0,	},
	{ { "Odd        ",	"奇  数     ",	}, (7 << 1) | 1,	},
};

static const t_menulabel data_dipsw_r2[] = {
	{ { "Baud Rate (BPS)     :",	"通信速度［ボー］   ：",	}, },
};
static const t_dipsw data_dipsw_r[] = {
	{ { "Duplex              :",	"通  信  方  式     ：",	}, 5 + 8,	data_dipsw_r_hdpx,	},
	{ { "X parameter         :",	"Ｘパラメータ       ：",	}, 4 + 8,	data_dipsw_r_xprm,	},
	{ { "Stop Bit            :",	"ストップビット長   ：",	}, 3 + 8,	data_dipsw_r_st2b,	},
	{ { "Data Bit            :",	"データビット長     ：",	}, 2 + 8,	data_dipsw_r_dt8b,	},
	{ { "S parameter         :",	"Ｓパラメータ       ：",	}, 4,		data_dipsw_r_sprm,	},
	{ { "DEL code            :",	"ＤＥＬコード       ：",	}, 5,		data_dipsw_r_pdel,	},
	{ { "Patiry Check        :",	"パリティチェック   ：",	}, 0 + 8,	data_dipsw_r_enpty,	},
	{ { "Patiry              :",	"パ  リ  ティ       ：",	}, 1 + 8,	data_dipsw_r_evpty,	},
};

#endif /* MENU_RESET_MESSAGE_H_INCLUDED */
