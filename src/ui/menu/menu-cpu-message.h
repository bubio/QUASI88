#ifndef MENU_CPU_MESSAGE_H_INCLUDED
#define MENU_CPU_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「CPU」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_CPU_CPU,
	DATA_CPU_CLOCK,
	DATA_CPU_WAIT,
	DATA_CPU_BOOST,
	DATA_CPU_HELP
};
static const t_menulabel data_cpu[] = {
	{ { " <<< SUB-CPU MODE >>> ",	" SUB-CPU駆動 <変更時はリセットを推奨> ",	}, },
	{ { " << CLOCK >> ",			" CPU クロック (-clock) ",					}, },
	{ { " << WAIT >> ",				" 速度 (-speed, -nowait) ",					}, },
	{ { " << BOOST >> ",			" ブースト (-boost) ",						}, },
	{ { " HELP ",					" 説明 ",									}, },
};



static const t_menudata data_cpu_cpu[] = {
	{ { "   0  Run SUB-CPU only during the disk access. (-cpu 0)  ",	"   0  ディスク処理中、サブCPUのみ駆動させる (-cpu 0)  ",	}, 0,	},
	{ { "   1  Run both CPUs during the disk access.    (-cpu 1)  ",	"   1  ディスク処理中、両CPUを駆動させる     (-cpu 1)  ",	}, 1,	},
	{ { "   2  Always run both CPUs.                    (-cpu 2)  ",	"   2  常時、両CPUを駆動させる               (-cpu 2)  ",	}, 2,	},
};


enum {
	DATA_CPU_CLOCK_CLOCK,
	DATA_CPU_CLOCK_MHZ,
	DATA_CPU_CLOCK_INFO
};
static const t_menulabel data_cpu_clock[] = {
	{ { " CLOCK     ",			" 周波数 ",					}, },
	{ { "[MHz] ",				"[MHz] ",					}, },
	{ { "(Range = 0.1-999.9) ",	"（範囲＝0.1 - 999.9）",	}, },
};
static const t_menudata data_cpu_clock_combo[] = {
	{ { " ( 1MHz) ",	" ( 1MHz) ",	}, (int)(CONST_4MHZ_CLOCK * 1000000.0 / 4),	},
	{ { " ( 2MHz) ",	" ( 2MHz) ",	}, (int)(CONST_4MHZ_CLOCK * 1000000.0 / 2),	},
	{ { "== 4MHz==",	"== 4MHz==",	}, (int)(CONST_4MHZ_CLOCK * 1000000.0),		},
	{ { " ( 8MHz) ",	" ( 8MHz) ",	}, (int)(CONST_8MHZ_CLOCK * 1000000.0),		},
	{ { " (16MHz) ",	" (16MHz) ",	}, (int)(CONST_8MHZ_CLOCK * 1000000.0 * 2),	},
	{ { " (32MHz) ",	" (32MHz) ",	}, (int)(CONST_8MHZ_CLOCK * 1000000.0 * 4),	},
	{ { " (64MHz) ",	" (64MHz) ",	}, (int)(CONST_8MHZ_CLOCK * 1000000.0 * 8),	},
};


enum {
	DATA_CPU_WAIT_NOWAIT,
	DATA_CPU_WAIT_RATE,
	DATA_CPU_WAIT_PERCENT,
	DATA_CPU_WAIT_INFO
};
static const t_menulabel data_cpu_wait[] = {
	{ { "No Wait             ",	"ウエイトなしにする  ",		}, },
	{ { " Rate of Speed ",		" 速度比     ",				}, },
	{ { "[%]   ",				"[％]  ",					}, },
	{ { "(Range =   5-5000)  ",	"（範囲＝  5 - 5000） ",	}, },
};
static const t_menudata data_cpu_wait_combo[] = {
	{ { "  25",	"  25",	},   25, },
	{ { "  50",	"  50",	},   50, },
	{ { " 100",	" 100",	},  100, },
	{ { " 200",	" 200",	},  200, },
	{ { " 400",	" 400",	},  400, },
	{ { " 800",	" 800",	},  800, },
	{ { "1600",	"1600",	}, 1600, },
};


enum {
	DATA_CPU_BOOST_MAGNIFY,
	DATA_CPU_BOOST_UNIT,
	DATA_CPU_BOOST_INFO
};
static const t_menulabel data_cpu_boost[] = {
	{ { " Power         ",		" 倍率       ",				}, },
	{ { "      ",				"倍    ",					}, },
	{ { "(Range =   1-100)   ",	"（範囲＝  1 - 100）  ",	}, },
};
static const t_menudata data_cpu_boost_combo[] = {
	{ { "   1",		"   1",	},  1,	},
	{ { "   2",		"   2",	},  2,	},
	{ { "   4",		"   4",	},  4,	},
	{ { "   8",		"   8",	},  8,	},
	{ { "  16",		"  16",	}, 16,	},
};



enum {
	DATA_CPU_MISC_FDCWAIT,
	DATA_CPU_MISC_FDCWAIT_X,
	DATA_CPU_MISC_BLANK,
	DATA_CPU_MISC_HSBASIC,
	DATA_CPU_MISC_HSBASIC_X,
	DATA_CPU_MISC_BLANK2,
	DATA_CPU_MISC_MEMWAIT,
	DATA_CPU_MISC_MEMWAIT_X,
	DATA_CPU_MISC_CMDSING,
};
static const t_menudata data_cpu_misc[] = {
	{ { "FDC Wait ON",			"FDCウエイトあり ",		}, DATA_CPU_MISC_FDCWAIT,	},
	{ { "(-fdc_wait)",			"(-fdc_wait)",			}, -1,						},
	{ { "",						"",						}, -1,						},
	{ { "HighSpeed BASIC ON",	"高速BASIC処理 有効 ",	}, DATA_CPU_MISC_HSBASIC,	},
	{ { "(-hsbasic)",			"(-hsbasic)",			}, -1,						},
	{ { "",						"",						}, -1,						},
	{ { "Memory Wait(dummy)",	"偽メモリウェイト",		}, DATA_CPU_MISC_MEMWAIT,	},
	{ { "(-mem_wait)",			"(-mem_wait)",			}, -1,						},

#if 0
	{ { "",						"",						}, -1,						},
	{ { "CMD SING",				"CMD SING",				}, DATA_CPU_MISC_CMDSING,	},
#endif
};


/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/

static const char *help_jp[] = {
	"  ディスクイメージを使用するアプリケーションをエミュレートする場合、",
	"『SUB-CPU駆動』と『FDCウェイト』の設定が適切でないと、正常に動作",
	"しないことがあります。",
	"",
	"『SUB-CPU駆動』と『FDCウェイト』の設定の組合せは、以下のとおりです。",
	"",
	"           設定  |  SUB-CPU駆動  | FDCウェイト |        ",
	"          -------+---------------+-------------+--------",
	"            (1)  |  0  (-cpu 0)  |    なし     |  高速  ",
	"            (2)  |  1  (-cpu 1)  |    なし     |   ↑   ",
	"            (3)  |  1  (-cpu 1)  |    あり     |        ",
	"            (4)  |  2  (-cpu 2)  |    なし     |   ↓   ",
	"            (5)  |  2  (-cpu 2)  |    あり     |  正確  ",
	"",
	"  設定(1) …… 最も高速で、アプリケーションの大部分が動作します。",
	"               デフォルトの設定はこれになります。",
	"  設定(2) …… やや高速です。一部のアプリケーションはこの設定でない",
	"               と動作しません。",
	"  設定(3) …… やや低速です。まれにこの設定でないと動作しないアプリ",
	"               ケーションがあります",
	"  設定(4)(5)… 最も低速です。この設定でないと動作しないアプリケー",
	"               ションはほとんど無いと思います。多分。",
	"",
	"  設定(1)でアプリケーションが動作しない場合、設定(2)、(3) … と変え",
	"てみてください。 また、動作するけれどもディスクのアクセス時に速度が",
	"低下する、サウンドが途切れる、などの場合も設定を変えると改善する",
	"可能性があります。",
	0,
};

static const char *help_en[] = {
	" I'm waiting for translator... ",
	0,
};

#endif /* MENU_CPU_MESSAGE_H_INCLUDED */
