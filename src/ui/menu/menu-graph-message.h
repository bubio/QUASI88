#ifndef MENU_GRAPH_MESSAGE_H_INCLUDED
#define MENU_GRAPH_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「画面」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_GRAPH_FRATE,
	DATA_GRAPH_RESIZE,
	DATA_GRAPH_PCG,
	DATA_GRAPH_FONT,
};
static const t_menulabel data_graph[] = {
	{ { " <<< FRAME RATE >>> ",	" フレームレート ",	}, },
	{ { " <<< RESIZE >>> ",		" 画面サイズ ",		}, },
	{ { " <<< PCG-8100 >>>",	" PCG-8100 ",		}, },
	{ { " <<< FONT >>> ",		" フォント ",		}, },
};

static const t_menudata data_graph_frate[] = {
	{ { "60",	"60",	},   1,	},
	{ { "30",	"30",	},   2,	},
	{ { "20",	"20",	},   3,	},
	{ { "15",	"15",	},   4,	},
	{ { "12",	"12",	},   5,	},
	{ { "10",	"10",	},   6,	},
	{ { "6",	"6",	},  10,	},
	{ { "5",	"5",	},  12,	},
	{ { "4",	"4",	},  15,	},
	{ { "3",	"3",	},  20,	},
	{ { "2",	"2",	},  30,	},
	{ { "1",	"1",	},  60,	},
};

static const t_menulabel data_graph_autoskip[] = {
	{ { "Auto frame skip (-autoskip) ",	"オートスキップを有効にする (-autoskip) ",	}, },
};

static const t_menudata data_graph_resize[] = {
	{ { " HALF SIZE (-half) ",		" 半分サイズ (-half) ",	}, SCREEN_SIZE_HALF,	},
	{ { " FULL SIZE (-full) ",		" 標準サイズ (-full) ",	}, SCREEN_SIZE_FULL,	},
#ifdef  SUPPORT_DOUBLE
	{ { " DOUBLE SIZE (-double) ",	" 倍サイズ (-double) ",	}, SCREEN_SIZE_DOUBLE,	},
#endif
};

static const t_menulabel data_graph_fullscreen[] = {
	{ { "Full Screen (-fullscreen)",	"フルスクリーン (-fullscreen) ",	}, },
};

enum {
	DATA_GRAPH_MISC_15K,
	DATA_GRAPH_MISC_DIGITAL,
	DATA_GRAPH_MISC_NOINTERP
};
static const t_menudata data_graph_misc[] = {
	{ { "Monitor Freq. 15k       (-15k)",		"モニタ周波数を15kに設定      (-15k)",		}, DATA_GRAPH_MISC_15K,			},
	{ { "Digital Monitor         (-digital)",	"デジタルモニタに設定         (-digital)",	}, DATA_GRAPH_MISC_DIGITAL,		},
	{ { "No reduce interpolation (-nointerp)",	"半分サイズ時に縮小補間しない (-nointerp)",	}, DATA_GRAPH_MISC_NOINTERP,	},
};

static const t_menudata data_graph_misc2[] = {
	{ { "Fill-Line Display       (-noskipline)",	"ラインの隙間を埋める         (-noskipline)",	}, SCREEN_INTERLACE_NO,		},
	{ { "Skip-Line Display       (-skipline)",		"1ラインおきに表示する        (-skipline)",		}, SCREEN_INTERLACE_SKIP,	},
	{ { "Interlace Display       (-interlace)",		"インターレース表示する       (-interlace)",	}, SCREEN_INTERLACE_YES,	},
};

static const t_menudata data_graph_pcg[] = {
	{ { " Noexist ",	" なし ",	}, FALSE,	},
	{ { " Exist ",		" あり  ",	}, TRUE,	},
};

#if 0
static const t_menudata data_graph_font[] = {
	{ { " Standard Font ",		" 標準フォント ",	}, 0,	},
	{ { " 2nd Font ",			" 第２フォント ",	}, 1,	},
	{ { " 3rd Font ",			" 第３フォント ",	}, 2,	},
};
#else
static const t_menudata data_graph_font1[2] = {
	{ { " Built-in Font ",		" 内 蔵 フォント ",	}, 0,	},
	{ { " Standard Font ",		" 標 準 フォント ",	}, 0,	},
};
static const t_menudata data_graph_font2[2] = {
	{ { " Hiragana Font ",		" 平仮名フォント ",	}, 1,	},
	{ { " 2nd Font ",			" 第 ２ フォント ",	}, 1,	},
};
static const t_menudata data_graph_font3[2] = {
	{ { " Transparent Font ",	" 透 明 フォント ",	}, 2,	},
	{ { " 3rd Font ",			" 第 ３ フォント ",	}, 2,	},
};
#endif

#endif /* MENU_GRAPH_MESSAGE_H_INCLUDED */
