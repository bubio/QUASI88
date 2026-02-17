/* #define USE_BUILDIN_NEWMODE */
/* #define USE_BUILDIN_OLDMODE */

/* タッチキーボードの配列データ */

typedef struct {
	unsigned char keytop_no;			/* キートップ番号 (このキートップ画像を使用) */
	unsigned char lock_type;			/* ロックの種類 LOCK_TYPE_xxx */
	short         cx, cy;				/* キーの中心座標 (キーボード背景画像 基準) */
} T_LAYOUT_BUILDIN;

#ifdef USE_BUILDIN_NEWMODE
#define BUILDIN_NEWMODEL_W			920
#define	BUILDIN_NEWMODEL_H			260
#define	BUILDIN_NEWMODEL_SCALE_W	100
#define	BUILDIN_NEWMODEL_SCALE_H	100
static const T_LAYOUT_BUILDIN buildin_newmodel[] = {
	{ KEYTOP_STOP         , LOCK_TYPE_NONE      ,	 20,  20,	},
	{ KEYTOP_COPY         , LOCK_TYPE_NONE      ,	 70,  20,	},
	{ KEYTOP_F1           , LOCK_TYPE_NONE      ,	130,  20,	},
	{ KEYTOP_F2           , LOCK_TYPE_NONE      ,	180,  20,	},
	{ KEYTOP_F3           , LOCK_TYPE_NONE      ,	230,  20,	},
	{ KEYTOP_F4           , LOCK_TYPE_NONE      ,	280,  20,	},
	{ KEYTOP_F5           , LOCK_TYPE_NONE      ,	330,  20,	},
	{ KEYTOP_F6           , LOCK_TYPE_NONE      ,	395,  20,	},
	{ KEYTOP_F7           , LOCK_TYPE_NONE      ,	445,  20,	},
	{ KEYTOP_F8           , LOCK_TYPE_NONE      ,	495,  20,	},
	{ KEYTOP_F9           , LOCK_TYPE_NONE      ,	545,  20,	},
	{ KEYTOP_F10          , LOCK_TYPE_NONE      ,	595,  20,	},
	{ KEYTOP_ROLLUP       , LOCK_TYPE_NONE      ,	670,  20,	},
	{ KEYTOP_ROLLDOWN     , LOCK_TYPE_NONE      ,	710,  20,	},

	{ KEYTOP_ESC          , LOCK_TYPE_NONE      ,	 25,  80,	},
	{ KEYTOP_1            , LOCK_TYPE_NONE      ,	 70,  80,	},
	{ KEYTOP_2            , LOCK_TYPE_NONE      ,	110,  80,	},
	{ KEYTOP_3            , LOCK_TYPE_NONE      ,	150,  80,	},
	{ KEYTOP_4            , LOCK_TYPE_NONE      ,	190,  80,	},
	{ KEYTOP_5            , LOCK_TYPE_NONE      ,	230,  80,	},
	{ KEYTOP_6            , LOCK_TYPE_NONE      ,	270,  80,	},
	{ KEYTOP_7            , LOCK_TYPE_NONE      ,	310,  80,	},
	{ KEYTOP_8            , LOCK_TYPE_NONE      ,	350,  80,	},
	{ KEYTOP_9            , LOCK_TYPE_NONE      ,	390,  80,	},
	{ KEYTOP_0            , LOCK_TYPE_NONE      ,	430,  80,	},
	{ KEYTOP_MINUS        , LOCK_TYPE_NONE      ,	470,  80,	},
	{ KEYTOP_CARET        , LOCK_TYPE_NONE      ,	510,  80,	},
	{ KEYTOP_YEN          , LOCK_TYPE_NONE      ,	550,  80,	},
	{ KEYTOP_BS           , LOCK_TYPE_NONE      ,	595,  80,	},
	{ KEYTOP_INS          , LOCK_TYPE_NONE      ,	670,  80,	},
	{ KEYTOP_DEL          , LOCK_TYPE_NONE      ,	710,  80,	},
	{ KEYTOP_HOME         , LOCK_TYPE_NONE      ,	780,  80,	},
	{ KEYTOP_HELP         , LOCK_TYPE_NONE      ,	820,  80,	},
	{ KEYTOP_KP_SUB       , LOCK_TYPE_NONE      ,	860,  80,	},
	{ KEYTOP_KP_DIVIDE    , LOCK_TYPE_NONE      ,	900,  80,	},

	{ KEYTOP_TAB          , LOCK_TYPE_NONE      ,	 35, 120,	},
	{ KEYTOP_Q            , LOCK_TYPE_NONE      ,	 90, 120,	},
	{ KEYTOP_W            , LOCK_TYPE_NONE      ,	130, 120,	},
	{ KEYTOP_E            , LOCK_TYPE_NONE      ,	170, 120,	},
	{ KEYTOP_R            , LOCK_TYPE_NONE      ,	210, 120,	},
	{ KEYTOP_T            , LOCK_TYPE_NONE      ,	250, 120,	},
	{ KEYTOP_Y            , LOCK_TYPE_NONE      ,	290, 120,	},
	{ KEYTOP_U            , LOCK_TYPE_NONE      ,	330, 120,	},
	{ KEYTOP_I            , LOCK_TYPE_NONE      ,	370, 120,	},
	{ KEYTOP_O            , LOCK_TYPE_NONE      ,	410, 120,	},
	{ KEYTOP_P            , LOCK_TYPE_NONE      ,	450, 120,	},
	{ KEYTOP_AT           , LOCK_TYPE_NONE      ,	490, 120,	},
	{ KEYTOP_BRACKETLEFT  , LOCK_TYPE_NONE      ,	530, 120,	},
	{ KEYTOP_RETURNL      , LOCK_TYPE_NONE      ,	590, 140,	},
	{ KEYTOP_KP_7         , LOCK_TYPE_NONE      ,	780, 120,	},
	{ KEYTOP_KP_8         , LOCK_TYPE_NONE      ,	820, 120,	},
	{ KEYTOP_KP_9         , LOCK_TYPE_NONE      ,	860, 120,	},
	{ KEYTOP_KP_MULTIPLY  , LOCK_TYPE_NONE      ,	900, 120,	},

	{ KEYTOP_CTRL         , LOCK_TYPE_DOUBLE    ,	 20, 160,	},
	{ KEYTOP_CAPS         , LOCK_TYPE_SINGLE    ,	 60, 160,	},
	{ KEYTOP_A            , LOCK_TYPE_NONE      ,	100, 160,	},
	{ KEYTOP_S            , LOCK_TYPE_NONE      ,	140, 160,	},
	{ KEYTOP_D            , LOCK_TYPE_NONE      ,	180, 160,	},
	{ KEYTOP_F            , LOCK_TYPE_NONE      ,	220, 160,	},
	{ KEYTOP_G            , LOCK_TYPE_NONE      ,	260, 160,	},
	{ KEYTOP_H            , LOCK_TYPE_NONE      ,	300, 160,	},
	{ KEYTOP_J            , LOCK_TYPE_NONE      ,	340, 160,	},
	{ KEYTOP_K            , LOCK_TYPE_NONE      ,	380, 160,	},
	{ KEYTOP_L            , LOCK_TYPE_NONE      ,	420, 160,	},
	{ KEYTOP_SEMICOLON    , LOCK_TYPE_NONE      ,	460, 160,	},
	{ KEYTOP_COLON        , LOCK_TYPE_NONE      ,	500, 160,	},
	{ KEYTOP_BRACKETRIGHT , LOCK_TYPE_NONE      ,	540, 160,	},
	{ KEYTOP_UP           , LOCK_TYPE_NONE      ,	690, 160,	},
	{ KEYTOP_KP_4         , LOCK_TYPE_NONE      ,	780, 160,	},
	{ KEYTOP_KP_5         , LOCK_TYPE_NONE      ,	820, 160,	},
	{ KEYTOP_KP_6         , LOCK_TYPE_NONE      ,	860, 160,	},
	{ KEYTOP_KP_ADD       , LOCK_TYPE_NONE      ,	900, 160,	},

	{ KEYTOP_SHIFTL       , LOCK_TYPE_DOUBLE    ,	 50, 200,	},
	{ KEYTOP_Z            , LOCK_TYPE_NONE      ,	120, 200,	},
	{ KEYTOP_X            , LOCK_TYPE_NONE      ,	160, 200,	},
	{ KEYTOP_C            , LOCK_TYPE_NONE      ,	200, 200,	},
	{ KEYTOP_V            , LOCK_TYPE_NONE      ,	240, 200,	},
	{ KEYTOP_B            , LOCK_TYPE_NONE      ,	280, 200,	},
	{ KEYTOP_N            , LOCK_TYPE_NONE      ,	320, 200,	},
	{ KEYTOP_M            , LOCK_TYPE_NONE      ,	360, 200,	},
	{ KEYTOP_COMMA        , LOCK_TYPE_NONE      ,	400, 200,	},
	{ KEYTOP_PERIOD       , LOCK_TYPE_NONE      ,	440, 200,	},
	{ KEYTOP_SLASH        , LOCK_TYPE_NONE      ,	480, 200,	},
	{ KEYTOP_UNDERSCORE   , LOCK_TYPE_NONE      ,	520, 200,	},
	{ KEYTOP_SHIFTR       , LOCK_TYPE_DOUBLE    ,	580, 200,	},
	{ KEYTOP_LEFT         , LOCK_TYPE_NONE      ,	650, 200,	},
	{ KEYTOP_DOWN         , LOCK_TYPE_NONE      ,	690, 200,	},
	{ KEYTOP_RIGHT        , LOCK_TYPE_NONE      ,	730, 200,	},
	{ KEYTOP_KP_1         , LOCK_TYPE_NONE      ,	780, 200,	},
	{ KEYTOP_KP_2         , LOCK_TYPE_NONE      ,	820, 200,	},
	{ KEYTOP_KP_3         , LOCK_TYPE_NONE      ,	860, 200,	},
	{ KEYTOP_KP_EQUAL     , LOCK_TYPE_NONE      ,	900, 200,	},

	{ KEYTOP_ROMAJI       , LOCK_TYPE_SINGLE    ,	 20, 240,	},
	{ KEYTOP_KANA         , LOCK_TYPE_SINGLE    ,	 80, 240,	},
	{ KEYTOP_GRAPH        , LOCK_TYPE_DOUBLE    ,	120, 240,	},
	{ KEYTOP_KETTEI       , LOCK_TYPE_NONE      ,	180, 240,	},
	{ KEYTOP_SPACE        , LOCK_TYPE_NONE      ,	270, 240,	},
	{ KEYTOP_HENKAN       , LOCK_TYPE_NONE      ,	370, 240,	},
	{ KEYTOP_PC           , LOCK_TYPE_NONE      ,	440, 240,	},
	{ KEYTOP_ZENKAKU      , LOCK_TYPE_NONE      ,	480, 240,	},
	{ KEYTOP_KP_0         , LOCK_TYPE_NONE      ,	780, 240,	},
	{ KEYTOP_KP_COMMA     , LOCK_TYPE_NONE      ,	820, 240,	},
	{ KEYTOP_KP_PERIOD    , LOCK_TYPE_NONE      ,	860, 240,	},
	{ KEYTOP_RETURNR      , LOCK_TYPE_NONE      ,	900, 240,	},
	{ KEYTOP_NUM          , LOCK_TYPE_SINGLE    ,	520, 240,	},
	{ KEYTOP_PREV         , LOCK_TYPE_NONE      ,	560, 240,	},
	{ KEYTOP_NEXT         , LOCK_TYPE_NONE      ,	600, 240,	},

	{ KEYTOP_END          , 0                   ,     0,   0,   },
};
#endif /* USE_BUILDIN_NEWMODE */

#ifdef USE_BUILDIN_OLDMODE
#define BUILDIN_OLDMODEL_W			800
#define BUILDIN_OLDMODEL_H			260
#define BUILDIN_OLDMODEL_SCALE_W	100
#define BUILDIN_OLDMODEL_SCALE_H	100
static const T_LAYOUT_BUILDIN buildin_oldmodel[] = {
	{ KEYTOP_STOP         , LOCK_TYPE_NONE      ,	 20,  20,	},
	{ KEYTOP_COPY         , LOCK_TYPE_NONE      ,	 60,  20,	},
	{ KEYTOP_F1_OLD       , LOCK_TYPE_NONE      ,	150,  20,	},
	{ KEYTOP_F2_OLD       , LOCK_TYPE_NONE      ,	230,  20,	},
	{ KEYTOP_F3_OLD       , LOCK_TYPE_NONE      ,	310,  20,	},
	{ KEYTOP_F4_OLD       , LOCK_TYPE_NONE      ,	390,  20,	},
	{ KEYTOP_F5_OLD       , LOCK_TYPE_NONE      ,	470,  20,	},
	{ KEYTOP_ROLLUP       , LOCK_TYPE_NONE      ,	560,  20,	},
	{ KEYTOP_ROLLDOWN     , LOCK_TYPE_NONE      ,	600,  20,	},
	{ KEYTOP_UP           , LOCK_TYPE_NONE      ,	660,  20,	},
	{ KEYTOP_DOWN         , LOCK_TYPE_NONE      ,	700,  20,	},
	{ KEYTOP_LEFT         , LOCK_TYPE_NONE      ,	740,  20,	},
	{ KEYTOP_RIGHT        , LOCK_TYPE_NONE      ,	780,  20,	},

	{ KEYTOP_ESC          , LOCK_TYPE_NONE      ,	 25,  80,	},
	{ KEYTOP_1            , LOCK_TYPE_NONE      ,	 70,  80,	},
	{ KEYTOP_2            , LOCK_TYPE_NONE      ,	110,  80,	},
	{ KEYTOP_3            , LOCK_TYPE_NONE      ,	150,  80,	},
	{ KEYTOP_4            , LOCK_TYPE_NONE      ,	190,  80,	},
	{ KEYTOP_5            , LOCK_TYPE_NONE      ,	230,  80,	},
	{ KEYTOP_6            , LOCK_TYPE_NONE      ,	270,  80,	},
	{ KEYTOP_7            , LOCK_TYPE_NONE      ,	310,  80,	},
	{ KEYTOP_8            , LOCK_TYPE_NONE      ,	350,  80,	},
	{ KEYTOP_9            , LOCK_TYPE_NONE      ,	390,  80,	},
	{ KEYTOP_0            , LOCK_TYPE_NONE      ,	430,  80,	},
	{ KEYTOP_MINUS        , LOCK_TYPE_NONE      ,	470,  80,	},
	{ KEYTOP_CARET        , LOCK_TYPE_NONE      ,	510,  80,	},
	{ KEYTOP_YEN          , LOCK_TYPE_NONE      ,	550,  80,	},
	{ KEYTOP_INSDEL       , LOCK_TYPE_NONE      ,	595,  80,	},
	{ KEYTOP_HOME         , LOCK_TYPE_NONE      ,	660,  80,	},
	{ KEYTOP_HELP         , LOCK_TYPE_NONE      ,	700,  80,	},
	{ KEYTOP_KP_SUB       , LOCK_TYPE_NONE      ,	740,  80,	},
	{ KEYTOP_KP_DIVIDE    , LOCK_TYPE_NONE      ,	780,  80,	},

	{ KEYTOP_TAB          , LOCK_TYPE_NONE      ,	 35, 120,	},
	{ KEYTOP_Q            , LOCK_TYPE_NONE      ,	 90, 120,	},
	{ KEYTOP_W            , LOCK_TYPE_NONE      ,	130, 120,	},
	{ KEYTOP_E            , LOCK_TYPE_NONE      ,	170, 120,	},
	{ KEYTOP_R            , LOCK_TYPE_NONE      ,	210, 120,	},
	{ KEYTOP_T            , LOCK_TYPE_NONE      ,	250, 120,	},
	{ KEYTOP_Y            , LOCK_TYPE_NONE      ,	290, 120,	},
	{ KEYTOP_U            , LOCK_TYPE_NONE      ,	330, 120,	},
	{ KEYTOP_I            , LOCK_TYPE_NONE      ,	370, 120,	},
	{ KEYTOP_O            , LOCK_TYPE_NONE      ,	410, 120,	},
	{ KEYTOP_P            , LOCK_TYPE_NONE      ,	450, 120,	},
	{ KEYTOP_AT           , LOCK_TYPE_NONE      ,	490, 120,	},
	{ KEYTOP_BRACKETLEFT  , LOCK_TYPE_NONE      ,	530, 120,	},
	{ KEYTOP_RETURNL      , LOCK_TYPE_NONE      ,	590, 140,	},
	{ KEYTOP_KP_7         , LOCK_TYPE_NONE      ,	660, 120,	},
	{ KEYTOP_KP_8         , LOCK_TYPE_NONE      ,	700, 120,	},
	{ KEYTOP_KP_9         , LOCK_TYPE_NONE      ,	740, 120,	},
	{ KEYTOP_KP_MULTIPLY  , LOCK_TYPE_NONE      ,	780, 120,	},

	{ KEYTOP_CTRL         , LOCK_TYPE_DOUBLE    ,	 20, 160,	},
	{ KEYTOP_CAPS         , LOCK_TYPE_SINGLE    ,	 60, 160,	},
	{ KEYTOP_A            , LOCK_TYPE_NONE      ,	100, 160,	},
	{ KEYTOP_S            , LOCK_TYPE_NONE      ,	140, 160,	},
	{ KEYTOP_D            , LOCK_TYPE_NONE      ,	180, 160,	},
	{ KEYTOP_F            , LOCK_TYPE_NONE      ,	220, 160,	},
	{ KEYTOP_G            , LOCK_TYPE_NONE      ,	260, 160,	},
	{ KEYTOP_H            , LOCK_TYPE_NONE      ,	300, 160,	},
	{ KEYTOP_J            , LOCK_TYPE_NONE      ,	340, 160,	},
	{ KEYTOP_K            , LOCK_TYPE_NONE      ,	380, 160,	},
	{ KEYTOP_L            , LOCK_TYPE_NONE      ,	420, 160,	},
	{ KEYTOP_SEMICOLON    , LOCK_TYPE_NONE      ,	460, 160,	},
	{ KEYTOP_COLON        , LOCK_TYPE_NONE      ,	500, 160,	},
	{ KEYTOP_BRACKETRIGHT , LOCK_TYPE_NONE      ,	540, 160,	},
	{ KEYTOP_KP_4         , LOCK_TYPE_NONE      ,	660, 160,	},
	{ KEYTOP_KP_5         , LOCK_TYPE_NONE      ,	700, 160,	},
	{ KEYTOP_KP_6         , LOCK_TYPE_NONE      ,	740, 160,	},
	{ KEYTOP_KP_ADD       , LOCK_TYPE_NONE      ,	780, 160,	},

	{ KEYTOP_SHIFTL       , LOCK_TYPE_DOUBLE    ,	 50, 200,	},
	{ KEYTOP_Z            , LOCK_TYPE_NONE      ,	120, 200,	},
	{ KEYTOP_X            , LOCK_TYPE_NONE      ,	160, 200,	},
	{ KEYTOP_C            , LOCK_TYPE_NONE      ,	200, 200,	},
	{ KEYTOP_V            , LOCK_TYPE_NONE      ,	240, 200,	},
	{ KEYTOP_B            , LOCK_TYPE_NONE      ,	280, 200,	},
	{ KEYTOP_N            , LOCK_TYPE_NONE      ,	320, 200,	},
	{ KEYTOP_M            , LOCK_TYPE_NONE      ,	360, 200,	},
	{ KEYTOP_COMMA        , LOCK_TYPE_NONE      ,	400, 200,	},
	{ KEYTOP_PERIOD       , LOCK_TYPE_NONE      ,	440, 200,	},
	{ KEYTOP_SLASH        , LOCK_TYPE_NONE      ,	480, 200,	},
	{ KEYTOP_UNDERSCORE   , LOCK_TYPE_NONE      ,	520, 200,	},
	{ KEYTOP_SHIFTR       , LOCK_TYPE_DOUBLE    ,	580, 200,	},
	{ KEYTOP_KP_1         , LOCK_TYPE_NONE      ,	660, 200,	},
	{ KEYTOP_KP_2         , LOCK_TYPE_NONE      ,	700, 200,	},
	{ KEYTOP_KP_3         , LOCK_TYPE_NONE      ,	740, 200,	},
	{ KEYTOP_KP_EQUAL     , LOCK_TYPE_NONE      ,	780, 200,	},

	{ KEYTOP_ROMAJI       , LOCK_TYPE_SINGLE    ,	 20, 240,	},
	{ KEYTOP_KANA         , LOCK_TYPE_SINGLE    ,	120, 240,	},
	{ KEYTOP_GRAPH        , LOCK_TYPE_DOUBLE    ,	160, 240,	},
	{ KEYTOP_SPACE_OLD    , LOCK_TYPE_NONE      ,	340, 240,	},
	{ KEYTOP_KP_0         , LOCK_TYPE_NONE      ,	660, 240,	},
	{ KEYTOP_KP_COMMA     , LOCK_TYPE_NONE      ,	700, 240,	},
	{ KEYTOP_KP_PERIOD    , LOCK_TYPE_NONE      ,	740, 240,	},
	{ KEYTOP_RETURNR      , LOCK_TYPE_NONE      ,	780, 240,	},
	{ KEYTOP_NUM          , LOCK_TYPE_SINGLE    ,	520, 240,	},
	{ KEYTOP_PREV         , LOCK_TYPE_NONE      ,	560, 240,	},
	{ KEYTOP_NEXT         , LOCK_TYPE_NONE      ,	600, 240,	},

	{ KEYTOP_END          , 0                   ,     0,   0,   },
};
#endif /* USE_BUILDIN_OLDMODE */

#define BUILDIN_JISKEY_W			640
#define BUILDIN_JISKEY_H			205
#define BUILDIN_JISKEY_SCALE_W		100
#define BUILDIN_JISKEY_SCALE_H		100
static const T_LAYOUT_BUILDIN buildin_jiskey[] = {
	{ KEYTOP_ESC          , LOCK_TYPE_NONE      ,	 35,  20,	},
	{ KEYTOP_1            , LOCK_TYPE_NONE      ,	 80,  20,	},
	{ KEYTOP_2            , LOCK_TYPE_NONE      ,	120,  20,	},
	{ KEYTOP_3            , LOCK_TYPE_NONE      ,	160,  20,	},
	{ KEYTOP_4            , LOCK_TYPE_NONE      ,	200,  20,	},
	{ KEYTOP_5            , LOCK_TYPE_NONE      ,	240,  20,	},
	{ KEYTOP_6            , LOCK_TYPE_NONE      ,	280,  20,	},
	{ KEYTOP_7            , LOCK_TYPE_NONE      ,	320,  20,	},
	{ KEYTOP_8            , LOCK_TYPE_NONE      ,	360,  20,	},
	{ KEYTOP_9            , LOCK_TYPE_NONE      ,	400,  20,	},
	{ KEYTOP_0            , LOCK_TYPE_NONE      ,	440,  20,	},
	{ KEYTOP_MINUS        , LOCK_TYPE_NONE      ,	480,  20,	},
	{ KEYTOP_CARET        , LOCK_TYPE_NONE      ,	520,  20,	},
	{ KEYTOP_YEN          , LOCK_TYPE_NONE      ,	560,  20,	},
	{ KEYTOP_BS           , LOCK_TYPE_NONE      ,	605,  20,	},

	{ KEYTOP_TAB          , LOCK_TYPE_NONE      ,	 45,  60,	},
	{ KEYTOP_Q            , LOCK_TYPE_NONE      ,	100,  60,	},
	{ KEYTOP_W            , LOCK_TYPE_NONE      ,	140,  60,	},
	{ KEYTOP_E            , LOCK_TYPE_NONE      ,	180,  60,	},
	{ KEYTOP_R            , LOCK_TYPE_NONE      ,	220,  60,	},
	{ KEYTOP_T            , LOCK_TYPE_NONE      ,	260,  60,	},
	{ KEYTOP_Y            , LOCK_TYPE_NONE      ,	300,  60,	},
	{ KEYTOP_U            , LOCK_TYPE_NONE      ,	340,  60,	},
	{ KEYTOP_I            , LOCK_TYPE_NONE      ,	380,  60,	},
	{ KEYTOP_O            , LOCK_TYPE_NONE      ,	420,  60,	},
	{ KEYTOP_P            , LOCK_TYPE_NONE      ,	460,  60,	},
	{ KEYTOP_AT           , LOCK_TYPE_NONE      ,	500,  60,	},
	{ KEYTOP_BRACKETLEFT  , LOCK_TYPE_NONE      ,	540,  60,	},
	{ KEYTOP_RETURNL      , LOCK_TYPE_NONE      ,	600,  80,	},

	{ KEYTOP_CTRL         , LOCK_TYPE_DOUBLE    ,	 30, 100,	},
	{ KEYTOP_CAPS         , LOCK_TYPE_SINGLE    ,	 70, 100,	},
	{ KEYTOP_A            , LOCK_TYPE_NONE      ,	110, 100,	},
	{ KEYTOP_S            , LOCK_TYPE_NONE      ,	150, 100,	},
	{ KEYTOP_D            , LOCK_TYPE_NONE      ,	190, 100,	},
	{ KEYTOP_F            , LOCK_TYPE_NONE      ,	230, 100,	},
	{ KEYTOP_G            , LOCK_TYPE_NONE      ,	270, 100,	},
	{ KEYTOP_H            , LOCK_TYPE_NONE      ,	310, 100,	},
	{ KEYTOP_J            , LOCK_TYPE_NONE      ,	350, 100,	},
	{ KEYTOP_K            , LOCK_TYPE_NONE      ,	390, 100,	},
	{ KEYTOP_L            , LOCK_TYPE_NONE      ,	430, 100,	},
	{ KEYTOP_SEMICOLON    , LOCK_TYPE_NONE      ,	470, 100,	},
	{ KEYTOP_COLON        , LOCK_TYPE_NONE      ,	510, 100,	},
	{ KEYTOP_BRACKETRIGHT , LOCK_TYPE_NONE      ,	550, 100,	},

	{ KEYTOP_SHIFTL       , LOCK_TYPE_DOUBLE    ,	 60, 140,	},
	{ KEYTOP_Z            , LOCK_TYPE_NONE      ,	130, 140,	},
	{ KEYTOP_X            , LOCK_TYPE_NONE      ,	170, 140,	},
	{ KEYTOP_C            , LOCK_TYPE_NONE      ,	210, 140,	},
	{ KEYTOP_V            , LOCK_TYPE_NONE      ,	250, 140,	},
	{ KEYTOP_B            , LOCK_TYPE_NONE      ,	290, 140,	},
	{ KEYTOP_N            , LOCK_TYPE_NONE      ,	330, 140,	},
	{ KEYTOP_M            , LOCK_TYPE_NONE      ,	370, 140,	},
	{ KEYTOP_COMMA        , LOCK_TYPE_NONE      ,	410, 140,	},
	{ KEYTOP_PERIOD       , LOCK_TYPE_NONE      ,	450, 140,	},
	{ KEYTOP_SLASH        , LOCK_TYPE_NONE      ,	490, 140,	},
	{ KEYTOP_UNDERSCORE   , LOCK_TYPE_NONE      ,	530, 140,	},
	{ KEYTOP_SHIFTR       , LOCK_TYPE_DOUBLE    ,	590, 140,	},

	{ KEYTOP_ROMAJI       , LOCK_TYPE_SINGLE    ,	 30, 180,	},
	{ KEYTOP_KANA         , LOCK_TYPE_SINGLE    ,	 90, 180,	},
	{ KEYTOP_GRAPH        , LOCK_TYPE_DOUBLE    ,	130, 180,	},
	{ KEYTOP_KETTEI       , LOCK_TYPE_NONE      ,	190, 180,	},
	{ KEYTOP_SPACE        , LOCK_TYPE_NONE      ,	280, 180,	},
	{ KEYTOP_HENKAN       , LOCK_TYPE_NONE      ,	380, 180,	},
	{ KEYTOP_PC           , LOCK_TYPE_NONE      ,	450, 180,	},
	{ KEYTOP_ZENKAKU      , LOCK_TYPE_NONE      ,	490, 180,	},
	{ KEYTOP_NUM          , LOCK_TYPE_SINGLE    ,	530, 180,	},
	{ KEYTOP_PREV         , LOCK_TYPE_NONE      ,	570, 180,	},
	{ KEYTOP_NEXT         , LOCK_TYPE_NONE      ,	610, 180,	},

	{ KEYTOP_END          , 0                   ,     0,   0,   },
};

#define BUILDIN_TENKEY_W			640
#define BUILDIN_TENKEY_H			205
#define BUILDIN_TENKEY_SCALE_W		100
#define BUILDIN_TENKEY_SCALE_H		100
static const T_LAYOUT_BUILDIN buildin_tenkey[] = {
	{ KEYTOP_STOP         , LOCK_TYPE_NONE      ,	 30,  20,	},
	{ KEYTOP_F1           , LOCK_TYPE_NONE      ,	 90,  20,	},
	{ KEYTOP_F2           , LOCK_TYPE_NONE      ,	140,  20,	},
	{ KEYTOP_F3           , LOCK_TYPE_NONE      ,	190,  20,	},
	{ KEYTOP_F4           , LOCK_TYPE_NONE      ,	240,  20,	},
	{ KEYTOP_F5           , LOCK_TYPE_NONE      ,	290,  20,	},
	{ KEYTOP_ROLLUP       , LOCK_TYPE_NONE      ,	380,  20,	},
	{ KEYTOP_ROLLDOWN     , LOCK_TYPE_NONE      ,	420,  20,	},
	{ KEYTOP_HOME         , LOCK_TYPE_NONE      ,	490,  20,	},
	{ KEYTOP_HELP         , LOCK_TYPE_NONE      ,	530,  20,	},
	{ KEYTOP_KP_SUB       , LOCK_TYPE_NONE      ,	570,  20,	},
	{ KEYTOP_KP_DIVIDE    , LOCK_TYPE_NONE      ,	610,  20,	},

	{ KEYTOP_COPY         , LOCK_TYPE_NONE      ,	 30,  60,	},
	{ KEYTOP_F6           , LOCK_TYPE_NONE      ,	 90,  60,	},
	{ KEYTOP_F7           , LOCK_TYPE_NONE      ,	140,  60,	},
	{ KEYTOP_F8           , LOCK_TYPE_NONE      ,	190,  60,	},
	{ KEYTOP_F9           , LOCK_TYPE_NONE      ,	240,  60,	},
	{ KEYTOP_F10          , LOCK_TYPE_NONE      ,	290,  60,	},
	{ KEYTOP_INS          , LOCK_TYPE_NONE      ,	380,  60,	},
	{ KEYTOP_DEL          , LOCK_TYPE_NONE      ,	420,  60,	},
	{ KEYTOP_KP_7         , LOCK_TYPE_NONE      ,	490,  60,	},
	{ KEYTOP_KP_8         , LOCK_TYPE_NONE      ,	530,  60,	},
	{ KEYTOP_KP_9         , LOCK_TYPE_NONE      ,	570,  60,	},
	{ KEYTOP_KP_MULTIPLY  , LOCK_TYPE_NONE      ,	610,  60,	},

	{ KEYTOP_ESC_PAD      , LOCK_TYPE_NONE      ,	 40, 100,	},
	{ KEYTOP_SHIFT_PAD    , LOCK_TYPE_NONE      ,	100, 100,	},
	{ KEYTOP_KP_4         , LOCK_TYPE_NONE      ,	490, 100,	},
	{ KEYTOP_KP_5         , LOCK_TYPE_NONE      ,	530, 100,	},
	{ KEYTOP_KP_6         , LOCK_TYPE_NONE      ,	570, 100,	},
	{ KEYTOP_KP_ADD       , LOCK_TYPE_NONE      ,	610, 100,	},

	{ KEYTOP_SPACE_PAD    , LOCK_TYPE_NONE      ,	 40, 140,	},
	{ KEYTOP_RETURN_PAD   , LOCK_TYPE_NONE      ,	100, 140,	},
	{ KEYTOP_UP           , LOCK_TYPE_NONE      ,	400, 140,	},
	{ KEYTOP_KP_1         , LOCK_TYPE_NONE      ,	490, 140,	},
	{ KEYTOP_KP_2         , LOCK_TYPE_NONE      ,	530, 140,	},
	{ KEYTOP_KP_3         , LOCK_TYPE_NONE      ,	570, 140,	},
	{ KEYTOP_KP_EQUAL     , LOCK_TYPE_NONE      ,	610, 140,	},

	{ KEYTOP_Z_PAD        , LOCK_TYPE_NONE      ,	 40, 180,	},
	{ KEYTOP_X_PAD        , LOCK_TYPE_NONE      ,	100, 180,	},
	{ KEYTOP_LEFT         , LOCK_TYPE_NONE      ,	360, 180,	},
	{ KEYTOP_DOWN         , LOCK_TYPE_NONE      ,	400, 180,	},
	{ KEYTOP_RIGHT        , LOCK_TYPE_NONE      ,	440, 180,	},
	{ KEYTOP_KP_0         , LOCK_TYPE_NONE      ,	490, 180,	},
	{ KEYTOP_KP_COMMA     , LOCK_TYPE_NONE      ,	530, 180,	},
	{ KEYTOP_KP_PERIOD    , LOCK_TYPE_NONE      ,	570, 180,	},
	{ KEYTOP_RETURNR      , LOCK_TYPE_NONE      ,	610, 180,	},

	{ KEYTOP_PREV         , LOCK_TYPE_NONE      ,	255, 180,	},
	{ KEYTOP_NEXT         , LOCK_TYPE_NONE      ,	295, 180,	},

	{ KEYTOP_END          , 0                   ,     0,   0,   },
};

#define BUILDIN_GAMEKEY_W			640
#define BUILDIN_GAMEKEY_H			125
#define BUILDIN_GAMEKEY_SCALE_W		100
#define BUILDIN_GAMEKEY_SCALE_H		100
static const T_LAYOUT_BUILDIN buildin_gamekey[] = {
	{ KEYTOP_8_4          , LOCK_TYPE_NONE      ,	  30,  20,	},
	{ KEYTOP_KP_8         , LOCK_TYPE_NONE      ,	  70,  20,	},
	{ KEYTOP_8_6          , LOCK_TYPE_NONE      ,	 110,  20,	},
	{ KEYTOP_ESC_PAD      , LOCK_TYPE_NONE      ,	-100,  20,	},
	{ KEYTOP_SHIFT_PAD    , LOCK_TYPE_NONE      ,	 -40,  20,	},

	{ KEYTOP_KP_4         , LOCK_TYPE_NONE      ,	  30,  60,	},
	{ KEYTOP_KP_6         , LOCK_TYPE_NONE      ,	 110,  60,	},
	{ KEYTOP_SPACE_PAD    , LOCK_TYPE_NONE      ,	-100,  60,	},
	{ KEYTOP_RETURN_PAD   , LOCK_TYPE_NONE      ,	 -40,  60,	},

	{ KEYTOP_2_4          , LOCK_TYPE_NONE      ,	  30, 100,	},
	{ KEYTOP_KP_2         , LOCK_TYPE_NONE      ,	  70, 100,	},
	{ KEYTOP_2_6          , LOCK_TYPE_NONE      ,	 110, 100,	},
	{ KEYTOP_X_PAD        , LOCK_TYPE_NONE      ,	-100, 100,	},
	{ KEYTOP_Z_PAD        , LOCK_TYPE_NONE      ,	 -40, 100,	},

	{ KEYTOP_PREV         , LOCK_TYPE_NONE      ,	 255, 100,	},
	{ KEYTOP_NEXT         , LOCK_TYPE_NONE      ,	 295, 100,	},

	{ KEYTOP_END          , 0                   ,      0,   0,   },
};


/* タッチキーボードの全レイアウトデータ */

typedef struct {
	unsigned char keytop_no;		/* キートップ番号 (この画像を表示) */
	unsigned char lock_type;		/* ロックの種類 LOCK_TYPE_xxx */
	unsigned char code[NR_KEYCOMBO]; /* 押下したことになるキー */
	float x0, y0;					/* キー表示範囲 (押下検知範囲兼用) */
	float x1, y1;					/* (x0, y0) - (x1, y1) */
	float sx, sy;					/* キー表示サイズ */
} T_LAYOUT;

typedef struct {
	T_LAYOUT *layout;			/* レイアウト情報の配列 */
	int sz_array;				/* 配列の数。最大 NR_KEYNO */
	int base_width;				/* キーボード背景幅 */
	int base_height;			/* キーボード背景高 */
	int scale_w;				/* 表示時の横倍率 % */
	int scale_h;				/* 表示時の縦倍率 % */
	Uint8 r, g, b;				/* キーボード背景色 */
} T_TOUCHKEY_LIST;
