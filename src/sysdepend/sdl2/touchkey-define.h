/* キートップ番号。タッチキーボードには、これらのキートップが配置できる */

enum {
	KEYTOP_STOP			,
	KEYTOP_COPY			,
	KEYTOP_F1			,
	KEYTOP_F2			,
	KEYTOP_F3			,
	KEYTOP_F4			,
	KEYTOP_F5			,
	KEYTOP_F6			,
	KEYTOP_F7			,
	KEYTOP_F8			,
	KEYTOP_F9			,
	KEYTOP_F10			,
	KEYTOP_ROLLUP		,
	KEYTOP_ROLLDOWN		,

	KEYTOP_ESC			,
	KEYTOP_1			,
	KEYTOP_2			,
	KEYTOP_3			,
	KEYTOP_4			,
	KEYTOP_5			,
	KEYTOP_6			,
	KEYTOP_7			,
	KEYTOP_8			,
	KEYTOP_9			,
	KEYTOP_0			,
	KEYTOP_MINUS		,
	KEYTOP_CARET		,
	KEYTOP_YEN			,
	KEYTOP_BS			,
	KEYTOP_INS			,
	KEYTOP_DEL			,
	KEYTOP_HOME			,
	KEYTOP_HELP			,
	KEYTOP_KP_SUB		,
	KEYTOP_KP_DIVIDE	,

	KEYTOP_TAB			,
	KEYTOP_Q			,
	KEYTOP_W			,
	KEYTOP_E			,
	KEYTOP_R			,
	KEYTOP_T			,
	KEYTOP_Y			,
	KEYTOP_U			,
	KEYTOP_I			,
	KEYTOP_O			,
	KEYTOP_P			,
	KEYTOP_AT			,
	KEYTOP_BRACKETLEFT	,
	KEYTOP_RETURNL		,
	KEYTOP_KP_7			,
	KEYTOP_KP_8			,
	KEYTOP_KP_9			,
	KEYTOP_KP_MULTIPLY	,

	KEYTOP_CTRL			,
	KEYTOP_CAPS			,
	KEYTOP_A			,
	KEYTOP_S			,
	KEYTOP_D			,
	KEYTOP_F			,
	KEYTOP_G			,
	KEYTOP_H			,
	KEYTOP_J			,
	KEYTOP_K			,
	KEYTOP_L			,
	KEYTOP_SEMICOLON	,
	KEYTOP_COLON		,
	KEYTOP_BRACKETRIGHT	,
	KEYTOP_UP			,
	KEYTOP_KP_4			,
	KEYTOP_KP_5			,
	KEYTOP_KP_6			,
	KEYTOP_KP_ADD		,

	KEYTOP_SHIFTL		,
	KEYTOP_Z			,
	KEYTOP_X			,
	KEYTOP_C			,
	KEYTOP_V			,
	KEYTOP_B			,
	KEYTOP_N			,
	KEYTOP_M			,
	KEYTOP_COMMA		,
	KEYTOP_PERIOD		,
	KEYTOP_SLASH		,
	KEYTOP_UNDERSCORE	,
	KEYTOP_SHIFTR		,
	KEYTOP_LEFT			,
	KEYTOP_DOWN			,
	KEYTOP_RIGHT		,
	KEYTOP_KP_1			,
	KEYTOP_KP_2			,
	KEYTOP_KP_3			,
	KEYTOP_KP_EQUAL		,

	KEYTOP_KANA			,
	KEYTOP_GRAPH		,
	KEYTOP_KETTEI		,
	KEYTOP_SPACE		,
	KEYTOP_HENKAN		,
	KEYTOP_PC			,
	KEYTOP_ZENKAKU		,
	KEYTOP_KP_0			,
	KEYTOP_KP_COMMA		,
	KEYTOP_KP_PERIOD	,
	KEYTOP_RETURNR		,

	KEYTOP_F1_OLD		,		/* 旧キーボードのF1キー (サイズ大きめ) */
	KEYTOP_F2_OLD		,		/* 旧キーボードのF2キー (サイズ大きめ) */
	KEYTOP_F3_OLD		,		/* 旧キーボードのF3キー (サイズ大きめ) */
	KEYTOP_F4_OLD		,		/* 旧キーボードのF4キー (サイズ大きめ) */
	KEYTOP_F5_OLD		,		/* 旧キーボードのF5キー (サイズ大きめ) */
	KEYTOP_SPACE_OLD	,		/* 旧キーボードのスペースキー (サイズ大きめ) */
	KEYTOP_SHIFTL_OLD	,		/* 旧キーボードの左シフトキー (サイズ同じ) */
	KEYTOP_SHIFTR_OLD	,		/* 旧キーボードの右シフトキー (サイズ同じ) */
	KEYTOP_RETURNL_OLD	,		/* 旧キーボードの左リターンキー (サイズ同じ) */
	KEYTOP_RETURNR_OLD	,		/* 旧キーボードの右リターンキー (サイズ同じ) */
	KEYTOP_INSDEL		,		/* 旧キーボードのINSDELキー (サイズ同じ) */

	KEYTOP_2_4			,		/* 独自キー テンキー 2+4 */
	KEYTOP_2_6			,		/* 独自キー テンキー 2+6 */
	KEYTOP_8_4			,		/* 独自キー テンキー 8+4 */
	KEYTOP_8_6			,		/* 独自キー テンキー 8+4 */
	KEYTOP_ESC_PAD		,		/* 独自キー ESCキー (サイズ大きめ) */
	KEYTOP_SHIFT_PAD	,		/* 独自キー 左シフトキー (サイズ大きめ) */
	KEYTOP_SPACE_PAD	,		/* 独自キー スペースキー (サイズ大きめ) */
	KEYTOP_RETURN_PAD	,		/* 独自キー 左リターンキー (サイズ大きめ) */
	KEYTOP_Z_PAD		,		/* 独自キー Zキー (サイズ大きめ) */
	KEYTOP_X_PAD		,		/* 独自キー Xキー (サイズ大きめ) */
	KEYTOP_SHIFT_MINI	,		/* 独自キー SHIFTキー (文字キーサイズ) */
	KEYTOP_SHIFTL_MINI	,		/* 独自キー SHIFTキー (文字キーサイズ) */
	KEYTOP_SHIFTR_MINI	,		/* 独自キー SHIFTキー (文字キーサイズ) */
	KEYTOP_ROMAJI		,		/* 独自キー ローマ字キー */
	KEYTOP_NUM			,		/* 独自キー NUMキー */
	KEYTOP_F1_MINI		,		/* 独自キー F1キー (文字キーサイズ) */
	KEYTOP_F2_MINI		,		/* 独自キー F2キー (文字キーサイズ) */
	KEYTOP_F3_MINI		,		/* 独自キー F3キー (文字キーサイズ) */
	KEYTOP_F4_MINI		,		/* 独自キー F4キー (文字キーサイズ) */
	KEYTOP_F5_MINI		,		/* 独自キー F5キー (文字キーサイズ) */
	KEYTOP_F6_MINI		,		/* 独自キー F6キー (文字キーサイズ) */
	KEYTOP_F7_MINI		,		/* 独自キー F7キー (文字キーサイズ) */
	KEYTOP_F8_MINI		,		/* 独自キー F8キー (文字キーサイズ) */
	KEYTOP_F9_MINI		,		/* 独自キー F9キー (文字キーサイズ) */
	KEYTOP_F10_MINI		,		/* 独自キー F10キー (文字キーサイズ) */
	KEYTOP_ESC_MINI		,		/* 独自キー ESCキー (文字キーサイズ) */
	KEYTOP_TAB_MINI		,		/* 独自キー TABキー (文字キーサイズ) */
	KEYTOP_BS_MINI		,		/* 独自キー BSキー (文字キーサイズ) */
	KEYTOP_INSDEL_MINI	,		/* 独自キー INSDELキー (文字キーサイズ) */
	KEYTOP_SPACE_MINI	,		/* 独自キー SPACEキー (文字キーサイズ) */
	KEYTOP_KETTEI_MINI	,		/* 独自キー 決定キー (文字キーサイズ) */
	KEYTOP_HENKAN_MINI	,		/* 独自キー 変換キー (文字キーサイズ) */
	KEYTOP_RETURN_MINI	,		/* 独自キー リターンキー (文字キーサイズ) */
	KEYTOP_RETURNL_MINI	,		/* 独自キー 左リターンキー (文字キーサイズ) */
	KEYTOP_RETURNR_MINI	,		/* 独自キー 右リターンキー (文字キーサイズ) */
	KEYTOP_F6_FUNC		,		/* 独自キー F6機能キー */
	KEYTOP_F7_FUNC		,		/* 独自キー F7機能キー */
	KEYTOP_F8_FUNC		,		/* 独自キー F8機能キー */
	KEYTOP_F9_FUNC		,		/* 独自キー F9機能キー */
	KEYTOP_F10_FUNC		,		/* 独自キー F10機能キー */
	KEYTOP_PREV			,		/* 独自キー PREVキー */
	KEYTOP_NEXT			,		/* 独自キー NEXTキー */

	KEYTOP_END /* = 121 */
};

/* キートップ画像番号 */

enum {
	KT_STOP				,	/* STOP       36x36 */
	KT_COPY				,	/* COPY       36x36 */
	KT_F1				,	/* F1         46x36 */
	KT_F2				,	/* F2         46x36 */
	KT_F3				,	/* F3         46x36 */
	KT_F4				,	/* F4         46x36 */
	KT_F5				,	/* F5         46x36 */
	KT_F6				,	/* F6         46x36 */
	KT_F7				,	/* F7         46x36 */
	KT_F8				,	/* F8         46x36 */
	KT_F9				,	/* F9         46x36 */
	KT_F10				,	/* F10        46x36 */
	KT_ROLLUP			,	/* ROLL UP    36x36 */
	KT_ROLLDOWN			,	/* ROLL DOWN  36x36 */

	KT_ESC				,	/* ESC        46x36 */
	KT_1				,	/* 1          36x36 */
	KT_EXCLAM			,	/* !          36x36 */
	KT_KANA_NU			,	/* ヌ         36x36 */
	KT_2				,	/* 2          36x36 */
	KT_QUOTEDBL			,	/* "          36x36 */
	KT_KANA_HU			,	/* フ         36x36 */
	KT_3				,	/* 3          36x36 */
	KT_NUMBERSIGN		,	/* #          36x36 */
	KT_KANA_A			,	/* ア         36x36 */
	KT_KANA_XA			,	/* ァ         36x36 */
	KT_4				,	/* 4          36x36 */
	KT_DOLLAR			,	/* $          36x36 */
	KT_KANA_U			,	/* ウ         36x36 */
	KT_KANA_XU			,	/* ゥ         36x36 */
	KT_5				,	/* 5          36x36 */
	KT_PERCENT			,	/* %          36x36 */
	KT_KANA_E			,	/* エ         36x36 */
	KT_KANA_XE			,	/* ェ         36x36 */
	KT_6				,	/* 6          36x36 */
	KT_AMPERSAND		,	/* &          36x36 */
	KT_KANA_O			,	/* オ         36x36 */
	KT_KANA_XO			,	/* ォ         36x36 */
	KT_7				,	/* 7          36x36 */
	KT_APOSTROPHE		,	/* '          36x36 */
	KT_KANA_YA			,	/* ヤ         36x36 */
	KT_KANA_XYA			,	/* ャ         36x36 */
	KT_8				,	/* 8          36x36 */
	KT_PARENLEFT		,	/* (          36x36 */
	KT_KANA_YU			,	/* ユ         36x36 */
	KT_KANA_XYU			,	/* ュ         36x36 */
	KT_9				,	/* 9          36x36 */
	KT_PARENRIGHT		,	/* )          36x36 */
	KT_KANA_YO			,	/* ヨ         36x36 */
	KT_KANA_XYO			,	/* ョ         36x36 */
	KT_0				,	/* 0          36x36 */
	KT_KANA_WA			,	/* ワ         36x36 */
	KT_KANA_WO			,	/* ヲ         36x36 */
	KT_MINUS			,	/* -          36x36 */
	KT_EQUAL			,	/* =          36x36 */
	KT_KANA_HO			,	/* ホ         36x36 */
	KT_CARET			,	/* ^          36x36 */
	KT_KANA_HE			,	/* ヘ         36x36 */
	KT_YEN				,	/* \          36x36 */
	KT_BAR				,	/* |          36x36 */
	KT_KANA__			,	/* ー         36x36 */
	KT_BS				,	/* BS         46x36 */
	KT_INS				,	/* INS        36x36 */
	KT_DEL				,	/* DEL        36x36 */
	KT_HOME				,	/* HOME       36x36 */
	KT_HELP				,	/* HELP       36x36 */
	KT_KP_SUB			,	/* [-]        36x36 */
	KT_KP_DIVIDE		,	/* [/]        36x36 */

	KT_TAB				,	/* TAB        66x36 */
	KT_Q				,	/* Q          36x36 */
	KT_KANA_TA			,	/* タ         36x36 */
	KT_W				,	/* W          36x36 */
	KT_KANA_TE			,	/* テ         36x36 */
	KT_E				,	/* E          36x36 */
	KT_KANA_I			,	/* イ         36x36 */
	KT_KANA_XI			,	/* ィ         36x36 */
	KT_R				,	/* R          36x36 */
	KT_KANA_SU			,	/* ス         36x36 */
	KT_T				,	/* T          36x36 */
	KT_KANA_KA			,	/* カ         36x36 */
	KT_Y				,	/* Y          36x36 */
	KT_KANA_NN			,	/* ン         36x36 */
	KT_U				,	/* U          36x36 */
	KT_KANA_NA			,	/* ナ         36x36 */
	KT_I				,	/* I          36x36 */
	KT_KANA_NI			,	/* ニ         36x36 */
	KT_O				,	/* O          36x36 */
	KT_KANA_RA			,	/* ラ         36x36 */
	KT_P				,	/* P          36x36 */
	KT_KANA_SE			,	/* セ         36x36 */
	KT_AT				,	/* @          36x36 */
	KT_TILDE			,	/* ~          36x36 */
	KT_KANA_DD			,	/* ゛         36x36 */
	KT_BRACKETLEFT		,	/* [          36x36 */
	KT_BRACELEFT		,	/* {          36x36 */
	KT_KANA_PP			,	/* ゜         36x36 */
	KT_KANA_LEFT		,	/* 「         36x36 */
	KT_RETURNL			,	/* RETURN     56x76 */
	KT_KP_7				,	/* [7]        36x36 */
	KT_KP_8				,	/* [8]        36x36 */
	KT_KP_9				,	/* [9]        36x36 */
	KT_KP_MULTIPLY		,	/* [*]        36x36 */

	KT_CTRL				,	/* CTRL       36x36 */
	KT_CAPS				,	/* CAPS       36x36 */
	KT_A				,	/* A          36x36 */
	KT_KANA_TI			,	/* チ         36x36 */
	KT_S				,	/* S          36x36 */
	KT_KANA_TO			,	/* ト         36x36 */
	KT_D				,	/* D          36x36 */
	KT_KANA_SI			,	/* シ         36x36 */
	KT_F				,	/* F          36x36 */
	KT_KANA_HA			,	/* ハ         36x36 */
	KT_G				,	/* G          36x36 */
	KT_KANA_KI			,	/* キ         36x36 */
	KT_H				,	/* H          36x36 */
	KT_KANA_KU			,	/* ク         36x36 */
	KT_J				,	/* J          36x36 */
	KT_KANA_MA			,	/* マ         36x36 */
	KT_K				,	/* K          36x36 */
	KT_KANA_NO			,	/* ノ         36x36 */
	KT_L				,	/* L          36x36 */
	KT_KANA_RI			,	/* リ         36x36 */
	KT_SEMICOLON		,	/* ;          36x36 */
	KT_PLUS				,	/* +          36x36 */
	KT_KANA_RE			,	/* レ         36x36 */
	KT_COLON			,	/* :          36x36 */
	KT_ASTERISK			,	/* *          36x36 */
	KT_KANA_KE			,	/* ケ         36x36 */
	KT_BRACKETRIGHT		,	/* ]          36x36 */
	KT_BRACERIGHT		,	/* }          36x36 */
	KT_KANA_MU			,	/* ム         36x36 */
	KT_KANA_RIGHT		,	/* 」         36x36 */
	KT_UP				,	/* ↑         36x36 */
	KT_KP_4				,	/* [4]        36x36 */
	KT_KP_5				,	/* [5]        36x36 */
	KT_KP_6				,	/* [6]        36x36 */
	KT_KP_ADD			,	/* [+]        36x36 */

	KT_SHIFTL			,	/* SHIFT左    96x36 */
	KT_Z				,	/* Z          36x36 */
	KT_KANA_TU			,	/* ツ         36x36 */
	KT_KANA_XTU			,	/* ッ         36x36 */
	KT_X				,	/* X          36x36 */
	KT_KANA_SA			,	/* サ         36x36 */
	KT_C				,	/* C          36x36 */
	KT_KANA_SO			,	/* ソ         36x36 */
	KT_V				,	/* V          36x36 */
	KT_KANA_HI			,	/* ヒ         36x36 */
	KT_B				,	/* B          36x36 */
	KT_KANA_KO			,	/* コ         36x36 */
	KT_N				,	/* N          36x36 */
	KT_KANA_MI			,	/* ミ         36x36 */
	KT_M				,	/* M          36x36 */
	KT_KANA_MO			,	/* モ         36x36 */
	KT_COMMA			,	/* ,          36x36 */
	KT_LESS				,	/* <          36x36 */
	KT_KANA_NE			,	/* ネ         36x36 */
	KT_KANA_TEN			,	/* 、         36x36 */
	KT_PERIOD			,	/* .          36x36 */
	KT_GREATER			,	/* >          36x36 */
	KT_KANA_RU			,	/* ル         36x36 */
	KT_KANA_MARU		,	/* 。         36x36 */
	KT_SLASH			,	/* /          36x36 */
	KT_QUESTION			,	/* ?          36x36 */
	KT_KANA_ME			,	/* メ         36x36 */
	KT_KANA_NAKA		,	/* ・         36x36 */
	KT_SPACE_MINI		,	/*            36x36 */
	KT_UNDERSCORE		,	/* _          36x36 */
	KT_KANA_RO			,	/* ロ         36x36 */
	KT_SHIFTR			,	/* SHIFT右    76x36 */
	KT_LEFT				,	/* ←         36x36 */
	KT_DOWN				,	/* ↓         36x36 */
	KT_RIGHT			,	/* →         36x36 */
	KT_KP_1				,	/* [1]        36x36 */
	KT_KP_2				,	/* [2]        36x36 */
	KT_KP_3				,	/* [3]        36x36 */
	KT_KP_EQUAL			,	/* [=]        36x36 */

	KT_KANA				,	/* カナ       36x36 */
	KT_GRAPH			,	/* GRAPH      36x36 */
	KT_KETTEI			,	/* 決定       76x36 */
	KT_SPACE			,	/* space      96x36 */
	KT_HENKAN			,	/* 変換       96x36 */
	KT_PC				,	/* PC         36x36 */
	KT_ZENKAKU			,	/* 全角       36x36 */
	KT_KP_0				,	/* [0]        36x36 */
	KT_KP_COMMA			,	/* [,]        36x36 */
	KT_KP_PERIOD		,	/* [.]        36x36 */
	KT_RETURNR			,	/* [ret]      36x36 */

	KT_F1_OLD			,	/* F1(旧)     76x36 */
	KT_F2_OLD			,	/* F2(旧)     76x36 */
	KT_F3_OLD			,	/* F3(旧)     76x36 */
	KT_F4_OLD			,	/* F4(旧)     76x36 */
	KT_F5_OLD			,	/* F5(旧)     76x36 */
	KT_SPACE_OLD		,	/* space(旧) 316x36 */
	KT_INSDEL			,	/* INSDEL     46x36 */

	KT_2_4				,	/* [2][4]     36x36 */
	KT_2_6				,	/* [2][6]     36x36 */
	KT_8_4				,	/* [8][4]     36x36 */
	KT_8_6				,	/* [8][6]     36x36 */
	KT_ESC_PAD			,	/* [ESC]      66x36 */
	KT_SHIFT_PAD		,	/* [SHIFT]    66x36 */
	KT_SPACE_PAD		,	/* [SPACE]    66x36 */
	KT_RETURN_PAD		,	/* [RETURN]   66x36 */
	KT_Z_PAD			,	/* [Z]        66x36 */
	KT_X_PAD			,	/* [X]        66x36 */
	KT_SHIFT_MINI		,	/* SHIFT(小)  36x36 */
	KT_ROMAJI			,	/* ローマ字   36x36 */
	KT_NUM				,	/* NUM        36x36 */
	KT_F1_MINI			,	/* F1(小)     36x36 */
	KT_F2_MINI			,	/* F2(小)     36x36 */
	KT_F3_MINI			,	/* F3(小)     36x36 */
	KT_F4_MINI			,	/* F4(小)     36x36 */
	KT_F5_MINI			,	/* F5(小)     36x36 */
	KT_F6_MINI			,	/* F6(小)     36x36 */
	KT_F7_MINI			,	/* F7(小)     36x36 */
	KT_F8_MINI			,	/* F8(小)     36x36 */
	KT_F9_MINI			,	/* F9(小)     36x36 */
	KT_F10_MINI			,	/* F10(小)    36x36 */
	KT_ESC_MINI			,	/* ESC(小)    36x36 */
	KT_TAB_MINI			,	/* TAB(小)    36x36 */
	KT_BS_MINI			,	/* BS(小)     36x36 */
	KT_INSDEL_MINI		,	/* INSDEL(小) 36x36 */
	KT_KETTEI_MINI		,	/* 決定(小)   36x36 */
	KT_HENKAN_MINI		,	/* 変換(小)   36x36 */
	KT_F6_FUNC			,	/* F6機能     36x36 */
	KT_F7_FUNC			,	/* F7機能     36x36 */
	KT_F8_FUNC			,	/* F8機能     36x36 */
	KT_F9_FUNC			,	/* F9機能     36x36 */
	KT_F10_FUNC			,	/* F10機能    36x36 */
	KT_PREV				,	/* (PREV)     36x36 */
	KT_NEXT				,	/* (NEXT)     36x36 */

	KT_END /* = 204 */
};	

/* キートップ番号とキートップ画像番号の対応 */

enum {
	KEYTOP_STAT_NORMAL       = 0,	/* 通常時の画像 */
	KEYTOP_STAT_SHIFT        = 1,	/* シフトキー押下中の画像 */
	KEYTOP_STAT_KANA         = 2,	/* カナキー押下中の画像 */
	KEYTOP_STAT_KANA_SHIFT   = 3,	/* カナキー&シフトキー押下中の画像 */
	KEYTOP_STAT_ROMAJI       = 4,	/* ローマ字キー押下中の画像 */
	KEYTOP_STAT_ROMAJI_SHIFT = 5,	/* ローマ字キー&シフトキー押下中の画像 */
	KEYTOP_STAT_NUM          = 6,	/* NUMキー押下中の画像 or 0 */
	KEYTOP_STAT_END          = 7
};
typedef struct {
	unsigned char keytop[KEYTOP_STAT_END];	/* キー画像 */
	unsigned char code[NR_KEYCOMBO];		/* 押下したことになるキー */
	short         width;					/* ビットマップサイズ */
	short         height;
} T_KEYTOP_SPEC;

static const T_KEYTOP_SPEC keytop_spec[] = {
	{ { KT_STOP         , KT_STOP         , KT_STOP         , KT_STOP         , KT_STOP         , KT_STOP         , KT_STOP         , }, { KEY88_STOP         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_STOP			*/
	{ { KT_COPY         , KT_COPY         , KT_COPY         , KT_COPY         , KT_COPY         , KT_COPY         , KT_COPY         , }, { KEY88_COPY         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_COPY			*/
	{ { KT_F1           , KT_F1           , KT_F1           , KT_F1           , KT_F1           , KT_F1           , KT_F1           , }, { KEY88_F1           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F1			*/
	{ { KT_F2           , KT_F2           , KT_F2           , KT_F2           , KT_F2           , KT_F2           , KT_F2           , }, { KEY88_F2           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F2			*/
	{ { KT_F3           , KT_F3           , KT_F3           , KT_F3           , KT_F3           , KT_F3           , KT_F3           , }, { KEY88_F3           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F3			*/
	{ { KT_F4           , KT_F4           , KT_F4           , KT_F4           , KT_F4           , KT_F4           , KT_F4           , }, { KEY88_F4           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F4			*/
	{ { KT_F5           , KT_F5           , KT_F5           , KT_F5           , KT_F5           , KT_F5           , KT_F5           , }, { KEY88_F5           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F5			*/
	{ { KT_F6           , KT_F6           , KT_F6           , KT_F6           , KT_F6           , KT_F6           , KT_F6           , }, { KEY88_SYS_F6       , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F6			*/
	{ { KT_F7           , KT_F7           , KT_F7           , KT_F7           , KT_F7           , KT_F7           , KT_F7           , }, { KEY88_SYS_F7       , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F7			*/
	{ { KT_F8           , KT_F8           , KT_F8           , KT_F8           , KT_F8           , KT_F8           , KT_F8           , }, { KEY88_SYS_F8       , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F8			*/
	{ { KT_F9           , KT_F9           , KT_F9           , KT_F9           , KT_F9           , KT_F9           , KT_F9           , }, { KEY88_SYS_F9       , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F9			*/
	{ { KT_F10          , KT_F10          , KT_F10          , KT_F10          , KT_F10          , KT_F10          , KT_F10          , }, { KEY88_SYS_F10      , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_F10			*/
	{ { KT_ROLLUP       , KT_ROLLUP       , KT_ROLLUP       , KT_ROLLUP       , KT_ROLLUP       , KT_ROLLUP       , KT_ROLLUP       , }, { KEY88_ROLLUP       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_ROLLUP		*/
	{ { KT_ROLLDOWN     , KT_ROLLDOWN     , KT_ROLLDOWN     , KT_ROLLDOWN     , KT_ROLLDOWN     , KT_ROLLDOWN     , KT_ROLLDOWN     , }, { KEY88_ROLLDOWN     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_ROLLDOWN		*/

	{ { KT_ESC          , KT_ESC          , KT_ESC          , KT_ESC          , KT_ESC          , KT_ESC          , KT_ESC          , }, { KEY88_ESC          , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_ESC			*/
	{ { KT_1            , KT_EXCLAM       , KT_KANA_NU      , KT_KANA_NU      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_F1_MINI      , }, { KEY88_1            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_1				*/
	{ { KT_2            , KT_QUOTEDBL     , KT_KANA_HU      , KT_KANA_HU      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_F2_MINI      , }, { KEY88_2            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_2				*/
	{ { KT_3            , KT_NUMBERSIGN   , KT_KANA_A       , KT_KANA_XA      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_F3_MINI      , }, { KEY88_3            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_3				*/
	{ { KT_4            , KT_DOLLAR       , KT_KANA_U       , KT_KANA_XU      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_F4_MINI      , }, { KEY88_4            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_4				*/
	{ { KT_5            , KT_PERCENT      , KT_KANA_E       , KT_KANA_XE      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_F5_MINI      , }, { KEY88_5            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_5				*/
	{ { KT_6            , KT_AMPERSAND    , KT_KANA_O       , KT_KANA_XO      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_HOME         , }, { KEY88_6            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_6				*/
	{ { KT_7            , KT_APOSTROPHE   , KT_KANA_YA      , KT_KANA_XYA     , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_7         , }, { KEY88_7            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_7				*/
	{ { KT_8            , KT_PARENLEFT    , KT_KANA_YU      , KT_KANA_XYU     , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_8         , }, { KEY88_8            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_8				*/
	{ { KT_9            , KT_PARENRIGHT   , KT_KANA_YO      , KT_KANA_XYO     , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_9         , }, { KEY88_9            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_9				*/
	{ { KT_0            , KT_0            , KT_KANA_WA      , KT_KANA_WO      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_MULTIPLY  , }, { KEY88_0            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_0				*/
	{ { KT_MINUS        , KT_EQUAL        , KT_KANA_HO      , KT_KANA_HO      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_SUB       , }, { KEY88_MINUS        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_MINUS			*/
	{ { KT_CARET        , KT_CARET        , KT_KANA_HE      , KT_KANA_HE      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , }, { KEY88_CARET        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_CARET			*/
	{ { KT_YEN          , KT_BAR          , KT_KANA__       , KT_KANA__       , KT_KANA__       , KT_KANA__       , KT_SPACE_MINI   , }, { KEY88_YEN          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_YEN			*/
	{ { KT_BS           , KT_BS           , KT_BS           , KT_BS           , KT_BS           , KT_BS           , KT_BS           , }, { KEY88_BS           , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_BS			*/
	{ { KT_INS          , KT_INS          , KT_INS          , KT_INS          , KT_INS          , KT_INS          , KT_INS          , }, { KEY88_INS          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_INS			*/
	{ { KT_DEL          , KT_DEL          , KT_DEL          , KT_DEL          , KT_DEL          , KT_DEL          , KT_DEL          , }, { KEY88_DEL          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_DEL			*/
	{ { KT_HOME         , KT_HOME         , KT_HOME         , KT_HOME         , KT_HOME         , KT_HOME         , KT_HOME         , }, { KEY88_HOME         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_HOME			*/
	{ { KT_HELP         , KT_HELP         , KT_HELP         , KT_HELP         , KT_HELP         , KT_HELP         , KT_HELP         , }, { KEY88_HELP         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_HELP			*/
	{ { KT_KP_SUB       , KT_KP_SUB       , KT_KP_SUB       , KT_KP_SUB       , KT_KP_SUB       , KT_KP_SUB       , KT_KP_SUB       , }, { KEY88_KP_SUB       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_SUB		*/
	{ { KT_KP_DIVIDE    , KT_KP_DIVIDE    , KT_KP_DIVIDE    , KT_KP_DIVIDE    , KT_KP_DIVIDE    , KT_KP_DIVIDE    , KT_KP_DIVIDE    , }, { KEY88_KP_DIVIDE    , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_DIVIDE		*/

	{ { KT_TAB          , KT_TAB          , KT_TAB          , KT_TAB          , KT_TAB          , KT_TAB          , KT_TAB          , }, { KEY88_TAB          , KEY88_INVALID      , },  66, 36, },	/* KEYTOP_TAB			*/
	{ { KT_Q            , KT_Q            , KT_KANA_TA      , KT_KANA_TA      , KT_Q            , KT_Q            , KT_SPACE_MINI   , }, { KEY88_q            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_Q				*/
	{ { KT_W            , KT_W            , KT_KANA_TE      , KT_KANA_TE      , KT_W            , KT_W            , KT_SPACE_MINI   , }, { KEY88_w            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_W				*/
	{ { KT_E            , KT_E            , KT_KANA_I       , KT_KANA_XI      , KT_E            , KT_E            , KT_UP           , }, { KEY88_e            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_E				*/
	{ { KT_R            , KT_R            , KT_KANA_SU      , KT_KANA_SU      , KT_R            , KT_R            , KT_SPACE_MINI   , }, { KEY88_r            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_R				*/
	{ { KT_T            , KT_T            , KT_KANA_KA      , KT_KANA_KA      , KT_T            , KT_T            , KT_SPACE_MINI   , }, { KEY88_t            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_T				*/
	{ { KT_Y            , KT_Y            , KT_KANA_NN      , KT_KANA_NN      , KT_Y            , KT_Y            , KT_HELP         , }, { KEY88_y            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_Y				*/
	{ { KT_U            , KT_U            , KT_KANA_NA      , KT_KANA_NA      , KT_U            , KT_U            , KT_KP_4         , }, { KEY88_u            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_U				*/
	{ { KT_I            , KT_I            , KT_KANA_NI      , KT_KANA_NI      , KT_I            , KT_I            , KT_KP_5         , }, { KEY88_i            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_I				*/
	{ { KT_O            , KT_O            , KT_KANA_RA      , KT_KANA_RA      , KT_O            , KT_O            , KT_KP_6         , }, { KEY88_o            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_O				*/
	{ { KT_P            , KT_P            , KT_KANA_SE      , KT_KANA_SE      , KT_P            , KT_P            , KT_KP_ADD       , }, { KEY88_p            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_P				*/
	{ { KT_AT           , KT_TILDE        , KT_KANA_DD      , KT_KANA_DD      , KT_KANA_DD      , KT_KANA_DD      , KT_KP_DIVIDE    , }, { KEY88_AT           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_AT			*/
	{ { KT_BRACKETLEFT  , KT_BRACELEFT    , KT_KANA_PP      , KT_KANA_LEFT    , KT_KANA_PP      , KT_KANA_LEFT    , KT_SPACE_MINI   , }, { KEY88_BRACKETLEFT  , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_BRACKETLEFT	*/
	{ { KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , }, { KEY88_RETURNL      , KEY88_INVALID      , },  56, 76, },	/* KEYTOP_RETURNL		*/
	{ { KT_KP_7         , KT_KP_7         , KT_KP_7         , KT_KP_7         , KT_KP_7         , KT_KP_7         , KT_KP_7         , }, { KEY88_KP_7         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_7			*/
	{ { KT_KP_8         , KT_KP_8         , KT_KP_8         , KT_KP_8         , KT_KP_8         , KT_KP_8         , KT_KP_8         , }, { KEY88_KP_8         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_8			*/
	{ { KT_KP_9         , KT_KP_9         , KT_KP_9         , KT_KP_9         , KT_KP_9         , KT_KP_9         , KT_KP_9         , }, { KEY88_KP_9         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_9			*/
	{ { KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , KT_KP_MULTIPLY  , }, { KEY88_KP_MULTIPLY  , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_MULTIPLY	*/

	{ { KT_CTRL         , KT_CTRL         , KT_CTRL         , KT_CTRL         , KT_CTRL         , KT_CTRL         , KT_CTRL         , }, { KEY88_CTRL         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_CTRL			*/
	{ { KT_CAPS         , KT_CAPS         , KT_CAPS         , KT_CAPS         , KT_CAPS         , KT_CAPS         , KT_CAPS         , }, { KEY88_CAPS         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_CAPS			*/
	{ { KT_A            , KT_A            , KT_KANA_TI      , KT_KANA_TI      , KT_A            , KT_A            , KT_SPACE_MINI   , }, { KEY88_a            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_A				*/
	{ { KT_S            , KT_S            , KT_KANA_TO      , KT_KANA_TO      , KT_S            , KT_S            , KT_LEFT         , }, { KEY88_s            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_S				*/
	{ { KT_D            , KT_D            , KT_KANA_SI      , KT_KANA_SI      , KT_D            , KT_D            , KT_RIGHT        , }, { KEY88_d            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_D				*/
	{ { KT_F            , KT_F            , KT_KANA_HA      , KT_KANA_HA      , KT_F            , KT_F            , KT_SPACE_MINI   , }, { KEY88_f            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F				*/
	{ { KT_G            , KT_G            , KT_KANA_KI      , KT_KANA_KI      , KT_G            , KT_G            , KT_SPACE_MINI   , }, { KEY88_g            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_G				*/
	{ { KT_H            , KT_H            , KT_KANA_KU      , KT_KANA_KU      , KT_H            , KT_H            , KT_SPACE_MINI   , }, { KEY88_h            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_H				*/
	{ { KT_J            , KT_J            , KT_KANA_MA      , KT_KANA_MA      , KT_J            , KT_J            , KT_KP_1         , }, { KEY88_j            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_J				*/
	{ { KT_K            , KT_K            , KT_KANA_NO      , KT_KANA_NO      , KT_K            , KT_K            , KT_KP_2         , }, { KEY88_k            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_K				*/
	{ { KT_L            , KT_L            , KT_KANA_RI      , KT_KANA_RI      , KT_L            , KT_L            , KT_KP_3         , }, { KEY88_l            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_L				*/
	{ { KT_SEMICOLON    , KT_PLUS         , KT_KANA_RE      , KT_KANA_RE      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_KP_EQUAL     , }, { KEY88_SEMICOLON    , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SEMICOLON		*/
	{ { KT_COLON        , KT_ASTERISK     , KT_KANA_KE      , KT_KANA_KE      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , }, { KEY88_COLON        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_COLON			*/
	{ { KT_BRACKETRIGHT , KT_BRACERIGHT   , KT_KANA_MU      , KT_KANA_RIGHT   , KT_SPACE_MINI   , KT_KANA_RIGHT   , KT_SPACE_MINI   , }, { KEY88_BRACKETRIGHT , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_BRACKETRIGHT	*/
	{ { KT_UP           , KT_UP           , KT_UP           , KT_UP           , KT_UP           , KT_UP           , KT_UP           , }, { KEY88_SYS_UP       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_UP			*/
	{ { KT_KP_4         , KT_KP_4         , KT_KP_4         , KT_KP_4         , KT_KP_4         , KT_KP_4         , KT_KP_4         , }, { KEY88_KP_4         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_4			*/
	{ { KT_KP_5         , KT_KP_5         , KT_KP_5         , KT_KP_5         , KT_KP_5         , KT_KP_5         , KT_KP_5         , }, { KEY88_KP_5         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_5			*/
	{ { KT_KP_6         , KT_KP_6         , KT_KP_6         , KT_KP_6         , KT_KP_6         , KT_KP_6         , KT_KP_6         , }, { KEY88_KP_6         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_6			*/
	{ { KT_KP_ADD       , KT_KP_ADD       , KT_KP_ADD       , KT_KP_ADD       , KT_KP_ADD       , KT_KP_ADD       , KT_KP_ADD       , }, { KEY88_KP_ADD       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_ADD		*/

	{ { KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , }, { KEY88_SHIFTL       , KEY88_INVALID      , },  96, 36, },	/* KEYTOP_SHIFTL		*/
	{ { KT_Z            , KT_Z            , KT_KANA_TU      , KT_KANA_XTU     , KT_Z            , KT_Z            , KT_SPACE_MINI   , }, { KEY88_z            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_Z				*/
	{ { KT_X            , KT_X            , KT_KANA_SA      , KT_KANA_SA      , KT_X            , KT_X            , KT_DOWN         , }, { KEY88_x            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_X				*/
	{ { KT_C            , KT_C            , KT_KANA_SO      , KT_KANA_SO      , KT_C            , KT_C            , KT_SPACE_MINI   , }, { KEY88_c            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_C				*/
	{ { KT_V            , KT_V            , KT_KANA_HI      , KT_KANA_HI      , KT_V            , KT_V            , KT_SPACE_MINI   , }, { KEY88_v            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_V				*/
	{ { KT_B            , KT_B            , KT_KANA_KO      , KT_KANA_KO      , KT_B            , KT_B            , KT_SPACE_MINI   , }, { KEY88_b            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_B				*/
	{ { KT_N            , KT_N            , KT_KANA_MI      , KT_KANA_MI      , KT_N            , KT_N            , KT_SPACE_MINI   , }, { KEY88_n            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_N				*/
	{ { KT_M            , KT_M            , KT_KANA_MO      , KT_KANA_MO      , KT_M            , KT_M            , KT_KP_0         , }, { KEY88_m            , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_M				*/
	{ { KT_COMMA        , KT_LESS         , KT_KANA_NE      , KT_KANA_TEN     , KT_KANA_TEN     , KT_KANA_TEN     , KT_KP_COMMA     , }, { KEY88_COMMA        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_COMMA			*/
	{ { KT_PERIOD       , KT_GREATER      , KT_KANA_RU      , KT_KANA_MARU    , KT_KANA_MARU    , KT_KANA_MARU    , KT_KP_PERIOD    , }, { KEY88_PERIOD       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_PERIOD		*/
	{ { KT_SLASH        , KT_QUESTION     , KT_KANA_ME      , KT_KANA_NAKA    , KT_KANA_NAKA    , KT_KANA_NAKA    , KT_RETURNR      , }, { KEY88_SLASH        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SLASH			*/
	{ { KT_SPACE_MINI   , KT_UNDERSCORE   , KT_KANA_RO      , KT_KANA_RO      , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , }, { KEY88_UNDERSCORE   , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_UNDERSCORE	*/ 
	{ { KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , }, { KEY88_SHIFTR       , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_SHIFTR		*/
	{ { KT_LEFT         , KT_LEFT         , KT_LEFT         , KT_LEFT         , KT_LEFT         , KT_LEFT         , KT_LEFT         , }, { KEY88_SYS_LEFT     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_LEFT			*/
	{ { KT_DOWN         , KT_DOWN         , KT_DOWN         , KT_DOWN         , KT_DOWN         , KT_DOWN         , KT_DOWN         , }, { KEY88_SYS_DOWN     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_DOWN			*/
	{ { KT_RIGHT        , KT_RIGHT        , KT_RIGHT        , KT_RIGHT        , KT_RIGHT        , KT_RIGHT        , KT_RIGHT        , }, { KEY88_SYS_RIGHT    , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RIGHT			*/
	{ { KT_KP_1         , KT_KP_1         , KT_KP_1         , KT_KP_1         , KT_KP_1         , KT_KP_1         , KT_KP_1         , }, { KEY88_KP_1         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_1			*/
	{ { KT_KP_2         , KT_KP_2         , KT_KP_2         , KT_KP_2         , KT_KP_2         , KT_KP_2         , KT_KP_2         , }, { KEY88_KP_2         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_2			*/
	{ { KT_KP_3         , KT_KP_3         , KT_KP_3         , KT_KP_3         , KT_KP_3         , KT_KP_3         , KT_KP_3         , }, { KEY88_KP_3         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_3			*/
	{ { KT_KP_EQUAL     , KT_KP_EQUAL     , KT_KP_EQUAL     , KT_KP_EQUAL     , KT_KP_EQUAL     , KT_KP_EQUAL     , KT_KP_EQUAL     , }, { KEY88_KP_EQUAL     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_EQUAL		*/

	{ { KT_KANA         , KT_KANA         , KT_KANA         , KT_KANA         , KT_KANA         , KT_KANA         , KT_KANA         , }, { KEY88_KANA         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KANA			*/
	{ { KT_GRAPH        , KT_GRAPH        , KT_GRAPH        , KT_GRAPH        , KT_GRAPH        , KT_GRAPH        , KT_GRAPH        , }, { KEY88_GRAPH        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_GRAPH			*/
	{ { KT_KETTEI       , KT_KETTEI       , KT_KETTEI       , KT_KETTEI       , KT_KETTEI       , KT_KETTEI       , KT_KETTEI       , }, { KEY88_KETTEI       , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_KETTEI		*/
	{ { KT_SPACE        , KT_SPACE        , KT_SPACE        , KT_SPACE        , KT_SPACE        , KT_SPACE        , KT_SPACE        , }, { KEY88_SPACE        , KEY88_INVALID      , },  96, 36, },	/* KEYTOP_SPACE			*/
	{ { KT_HENKAN       , KT_HENKAN       , KT_HENKAN       , KT_HENKAN       , KT_HENKAN       , KT_HENKAN       , KT_HENKAN       , }, { KEY88_HENKAN       , KEY88_INVALID      , },  96, 36, },	/* KEYTOP_HENKAN		*/
	{ { KT_PC           , KT_PC           , KT_PC           , KT_PC           , KT_PC           , KT_PC           , KT_PC           , }, { KEY88_PC           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_PC			*/
	{ { KT_ZENKAKU      , KT_ZENKAKU      , KT_ZENKAKU      , KT_ZENKAKU      , KT_ZENKAKU      , KT_ZENKAKU      , KT_ZENKAKU      , }, { KEY88_ZENKAKU      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_ZENKAKU		*/
	{ { KT_KP_0         , KT_KP_0         , KT_KP_0         , KT_KP_0         , KT_KP_0         , KT_KP_0         , KT_KP_0         , }, { KEY88_KP_0         , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_0			*/
	{ { KT_KP_COMMA     , KT_KP_COMMA     , KT_KP_COMMA     , KT_KP_COMMA     , KT_KP_COMMA     , KT_KP_COMMA     , KT_KP_COMMA     , }, { KEY88_KP_COMMA     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_COMMA		*/
	{ { KT_KP_PERIOD    , KT_KP_PERIOD    , KT_KP_PERIOD    , KT_KP_PERIOD    , KT_KP_PERIOD    , KT_KP_PERIOD    , KT_KP_PERIOD    , }, { KEY88_KP_PERIOD    , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KP_PERIOD		*/
	{ { KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , }, { KEY88_RETURNR      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RETURNR		*/

	{ { KT_F1_OLD       , KT_F1_OLD       , KT_F1_OLD       , KT_F1_OLD       , KT_F1_OLD       , KT_F1_OLD       , KT_F1_OLD       , }, { KEY88_F1           , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_F1_OLD		*/
	{ { KT_F2_OLD       , KT_F2_OLD       , KT_F2_OLD       , KT_F2_OLD       , KT_F2_OLD       , KT_F2_OLD       , KT_F2_OLD       , }, { KEY88_F2           , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_F2_OLD		*/
	{ { KT_F3_OLD       , KT_F3_OLD       , KT_F3_OLD       , KT_F3_OLD       , KT_F3_OLD       , KT_F3_OLD       , KT_F3_OLD       , }, { KEY88_F3           , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_F3_OLD		*/
	{ { KT_F4_OLD       , KT_F4_OLD       , KT_F4_OLD       , KT_F4_OLD       , KT_F4_OLD       , KT_F4_OLD       , KT_F4_OLD       , }, { KEY88_F4           , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_F4_OLD		*/
	{ { KT_F5_OLD       , KT_F5_OLD       , KT_F5_OLD       , KT_F5_OLD       , KT_F5_OLD       , KT_F5_OLD       , KT_F5_OLD       , }, { KEY88_F5           , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_F5_OLD		*/
	{ { KT_SPACE_OLD    , KT_SPACE_OLD    , KT_SPACE_OLD    , KT_SPACE_OLD    , KT_SPACE_OLD    , KT_SPACE_OLD    , KT_SPACE_OLD    , }, { KEY88_SPACE        , KEY88_INVALID      , }, 316, 36, },	/* KEYTOP_SPACE_OLD		*/
	{ { KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , KT_SHIFTL       , }, { KEY88_SHIFT        , KEY88_INVALID      , },  96, 36, },	/* KEYTOP_SHIFTL_OLD	*/
	{ { KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , KT_SHIFTR       , }, { KEY88_SHIFT        , KEY88_INVALID      , },  76, 36, },	/* KEYTOP_SHIFTR_OLD	*/
	{ { KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , KT_RETURNL      , }, { KEY88_RETURN       , KEY88_INVALID      , },  56, 76, },	/* KEYTOP_RETURNL_OLD	*/
	{ { KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , }, { KEY88_RETURN       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RETURNR_OLD	*/
	{ { KT_INSDEL       , KT_INSDEL       , KT_INSDEL       , KT_INSDEL       , KT_INSDEL       , KT_INSDEL       , KT_INSDEL       , }, { KEY88_INS_DEL      , KEY88_INVALID      , },  46, 36, },	/* KEYTOP_INSDEL		*/

	{ { KT_2_4          , KT_2_4          , KT_2_4          , KT_2_4          , KT_2_4          , KT_2_4          , KT_2_4          , }, { KEY88_KP_2         , KEY88_KP_4         , },  36, 36, },	/* KEYTOP_2_4			*/
	{ { KT_2_6          , KT_2_6          , KT_2_6          , KT_2_6          , KT_2_6          , KT_2_6          , KT_2_6          , }, { KEY88_KP_2         , KEY88_KP_6         , },  36, 36, },	/* KEYTOP_2_6			*/
	{ { KT_8_4          , KT_8_4          , KT_8_4          , KT_8_4          , KT_8_4          , KT_8_4          , KT_8_4          , }, { KEY88_KP_8         , KEY88_KP_4         , },  36, 36, },	/* KEYTOP_8_4			*/
	{ { KT_8_6          , KT_8_6          , KT_8_6          , KT_8_6          , KT_8_6          , KT_8_6          , KT_8_6          , }, { KEY88_KP_8         , KEY88_KP_6         , },  36, 36, },	/* KEYTOP_8_6			*/
	{ { KT_ESC_PAD      , KT_ESC_PAD      , KT_ESC_PAD      , KT_ESC_PAD      , KT_ESC_PAD      , KT_ESC_PAD      , KT_ESC_PAD      , }, { KEY88_ESC          , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_ESC_PAD 		*/
	{ { KT_SHIFT_PAD    , KT_SHIFT_PAD    , KT_SHIFT_PAD    , KT_SHIFT_PAD    , KT_SHIFT_PAD    , KT_SHIFT_PAD    , KT_SHIFT_PAD    , }, { KEY88_SHIFTL       , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_SHIFT_PAD 	*/
	{ { KT_SPACE_PAD    , KT_SPACE_PAD    , KT_SPACE_PAD    , KT_SPACE_PAD    , KT_SPACE_PAD    , KT_SPACE_PAD    , KT_SPACE_PAD    , }, { KEY88_SPACE        , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_SPACE_PAD 	*/
	{ { KT_RETURN_PAD   , KT_RETURN_PAD   , KT_RETURN_PAD   , KT_RETURN_PAD   , KT_RETURN_PAD   , KT_RETURN_PAD   , KT_RETURN_PAD   , }, { KEY88_RETURNL      , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_RETURN_PAD 	*/
	{ { KT_Z_PAD        , KT_Z_PAD        , KT_Z_PAD        , KT_Z_PAD        , KT_Z_PAD        , KT_Z_PAD        , KT_Z_PAD        , }, { KEY88_z            , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_Z_PAD 		*/
	{ { KT_X_PAD        , KT_X_PAD        , KT_X_PAD        , KT_X_PAD        , KT_X_PAD        , KT_X_PAD        , KT_X_PAD        , }, { KEY88_x            , KEY88_INVALID      , },  56, 36, },	/* KEYTOP_X_PAD 		*/
	{ { KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , }, { KEY88_SHIFT        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SHIFT_MINI	*/
	{ { KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , }, { KEY88_SHIFTL       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SHIFTL_MINI	*/
	{ { KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , KT_SHIFT_MINI   , }, { KEY88_SHIFTR       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SHIFTR_MINI	*/
	{ { KT_ROMAJI       , KT_ROMAJI       , KT_ROMAJI       , KT_ROMAJI       , KT_ROMAJI       , KT_ROMAJI       , KT_ROMAJI       , }, { KEY88_SYS_ROMAJI   , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_ROMAJI		*/
	{ { KT_NUM          , KT_NUM          , KT_NUM          , KT_NUM          , KT_NUM          , KT_NUM          , KT_NUM          , }, { KEY88_SYS_NUM      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_NUM			*/
	{ { KT_F1_MINI      , KT_F1_MINI      , KT_F1_MINI      , KT_F1_MINI      , KT_F1_MINI      , KT_F1_MINI      , KT_F1_MINI      , }, { KEY88_F1           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F1_MINI		*/
	{ { KT_F2_MINI      , KT_F2_MINI      , KT_F2_MINI      , KT_F2_MINI      , KT_F2_MINI      , KT_F2_MINI      , KT_F2_MINI      , }, { KEY88_F2           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F2_MINI		*/
	{ { KT_F3_MINI      , KT_F3_MINI      , KT_F3_MINI      , KT_F3_MINI      , KT_F3_MINI      , KT_F3_MINI      , KT_F3_MINI      , }, { KEY88_F3           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F3_MINI		*/
	{ { KT_F4_MINI      , KT_F4_MINI      , KT_F4_MINI      , KT_F4_MINI      , KT_F4_MINI      , KT_F4_MINI      , KT_F4_MINI      , }, { KEY88_F4           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F4_MINI		*/
	{ { KT_F5_MINI      , KT_F5_MINI      , KT_F5_MINI      , KT_F5_MINI      , KT_F5_MINI      , KT_F5_MINI      , KT_F5_MINI      , }, { KEY88_F5           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F5_MINI		*/
	{ { KT_F6_MINI      , KT_F6_MINI      , KT_F6_MINI      , KT_F6_MINI      , KT_F6_MINI      , KT_F6_MINI      , KT_F6_MINI      , }, { KEY88_SYS_F6       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F6_MINI		*/
	{ { KT_F7_MINI      , KT_F7_MINI      , KT_F7_MINI      , KT_F7_MINI      , KT_F7_MINI      , KT_F7_MINI      , KT_F7_MINI      , }, { KEY88_SYS_F7       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F7_MINI		*/
	{ { KT_F8_MINI      , KT_F8_MINI      , KT_F8_MINI      , KT_F8_MINI      , KT_F8_MINI      , KT_F8_MINI      , KT_F8_MINI      , }, { KEY88_SYS_F8       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F8_MINI		*/
	{ { KT_F9_MINI      , KT_F9_MINI      , KT_F9_MINI      , KT_F9_MINI      , KT_F9_MINI      , KT_F9_MINI      , KT_F9_MINI      , }, { KEY88_SYS_F9       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F9_MINI		*/
	{ { KT_F10_MINI     , KT_F10_MINI     , KT_F10_MINI     , KT_F10_MINI     , KT_F10_MINI     , KT_F10_MINI     , KT_F10_MINI     , }, { KEY88_SYS_F10      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F10_MINI		*/
	{ { KT_ESC_MINI     , KT_ESC_MINI     , KT_ESC_MINI     , KT_ESC_MINI     , KT_ESC_MINI     , KT_ESC_MINI     , KT_ESC_MINI     , }, { KEY88_ESC          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_ESC_MINI		*/
	{ { KT_TAB_MINI     , KT_TAB_MINI     , KT_TAB_MINI     , KT_TAB_MINI     , KT_TAB_MINI     , KT_TAB_MINI     , KT_TAB_MINI     , }, { KEY88_TAB          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_TAB_MINI		*/
	{ { KT_BS_MINI      , KT_BS_MINI      , KT_BS_MINI      , KT_BS_MINI      , KT_BS_MINI      , KT_BS_MINI      , KT_BS_MINI      , }, { KEY88_BS           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_BS_MINI		*/
	{ { KT_INSDEL_MINI  , KT_INSDEL_MINI  , KT_INSDEL_MINI  , KT_INSDEL_MINI  , KT_INSDEL_MINI  , KT_INSDEL_MINI  , KT_INSDEL_MINI  , }, { KEY88_INS_DEL      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_INSDEL_MINI	*/
	{ { KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , KT_SPACE_MINI   , }, { KEY88_SPACE        , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_SPACE_MINI	*/
	{ { KT_KETTEI_MINI  , KT_KETTEI_MINI  , KT_KETTEI_MINI  , KT_KETTEI_MINI  , KT_KETTEI_MINI  , KT_KETTEI_MINI  , KT_KETTEI_MINI  , }, { KEY88_KETTEI       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_KETTEI_MINI	*/
	{ { KT_HENKAN_MINI  , KT_HENKAN_MINI  , KT_HENKAN_MINI  , KT_HENKAN_MINI  , KT_HENKAN_MINI  , KT_HENKAN_MINI  , KT_HENKAN_MINI  , }, { KEY88_HENKAN       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_HENKAN_MINI	*/
	{ { KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , }, { KEY88_RETURN       , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RETURN_MINI	*/
	{ { KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , }, { KEY88_RETURNL      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RETURNL_MINI	*/
	{ { KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , KT_RETURNR      , }, { KEY88_RETURNR      , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_RETURNR_MINI	*/
	{ { KT_F6_FUNC      , KT_F6_FUNC      , KT_F6_FUNC      , KT_F6_FUNC      , KT_F6_FUNC      , KT_F6_FUNC      , KT_F6_FUNC      , }, { KEY88_F6           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F6_FUNC		*/
	{ { KT_F7_FUNC      , KT_F7_FUNC      , KT_F7_FUNC      , KT_F7_FUNC      , KT_F7_FUNC      , KT_F7_FUNC      , KT_F7_FUNC      , }, { KEY88_F7           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F7_FUNC		*/
	{ { KT_F8_FUNC      , KT_F8_FUNC      , KT_F8_FUNC      , KT_F8_FUNC      , KT_F8_FUNC      , KT_F8_FUNC      , KT_F8_FUNC      , }, { KEY88_F8           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F8_FUNC		*/
	{ { KT_F9_FUNC      , KT_F9_FUNC      , KT_F9_FUNC      , KT_F9_FUNC      , KT_F9_FUNC      , KT_F9_FUNC      , KT_F9_FUNC      , }, { KEY88_F9           , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F9_FUNC		*/
	{ { KT_F10_FUNC     , KT_F10_FUNC     , KT_F10_FUNC     , KT_F10_FUNC     , KT_F10_FUNC     , KT_F10_FUNC     , KT_F10_FUNC     , }, { KEY88_F10          , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_F10_FUNC		*/
	{ { KT_PREV         , KT_PREV         , KT_PREV         , KT_PREV         , KT_PREV         , KT_PREV         , KT_PREV         , }, { KEY88_SYS_PREV     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_PREV			*/
	{ { KT_NEXT         , KT_NEXT         , KT_NEXT         , KT_NEXT         , KT_NEXT         , KT_NEXT         , KT_NEXT         , }, { KEY88_SYS_NEXT     , KEY88_INVALID      , },  36, 36, },	/* KEYTOP_NEXT			*/
};
