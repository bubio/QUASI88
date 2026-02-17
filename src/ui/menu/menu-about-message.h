#ifndef MENU_ABOUT_MESSAGE_H_INCLUDED
#define MENU_ABOUT_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * メニューの '*' タブにて表示される情報
 *--------------------------------------------------------------*/

enum {
	DATA_ABOUT_THEMA
};
static const t_menulabel data_about[] = {
	{ { " Thema: ",				" テーマ： ",		}, },
};

static const char *data_about_en[] = {
#ifdef USE_SOUND
	"MAME Sound Driver ... Available",
	"   " Q_MAME_COPYRIGHT,
	"@MAMEVER",
#ifdef USE_FMGEN
	"",
	"FM Sound Generator ... Available",
	"   " Q_FMGEN_COPYRIGHT,
	"@FMGENVER",
#endif
#else
	"SOUND OUTPUT ... Not available",
#endif
	"",
#ifdef USE_MONITOR
	"Monitor mode ... Supported",
	"",
#endif

	NULL, /* 終端 */
};


static const char *data_about_jp[] = {
#ifdef USE_SOUND
	"MAME サウンドドライバ が組み込まれています",
	"   " Q_MAME_COPYRIGHT,
	"@MAMEVER",
#ifdef USE_FMGEN
	"",
	"FM Sound Generator が組み込まれています",
	"   " Q_FMGEN_COPYRIGHT,
	"@FMGENVER",
#endif
#else
	"サウンド出力 は組み込まれていません",
#endif
	"",
#ifdef USE_MONITOR
	"モニターモードが使用できます",
	"",
#endif

	NULL, /* 終端 */
};

static const t_menudata data_about_thema[] = {
	{ { "Light   ",		"Light   ",		}, MENU_THEMA_LIGHT,	},
	{ { "Dark    ",		"Dark    ",		}, MENU_THEMA_DARK,		},
	{ { "Classic ",		"Classic ",		}, MENU_THEMA_CLASSIC,	},
};

#endif /* MENU_ABOUT_MESSAGE_H_INCLUDED */
