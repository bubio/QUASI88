#ifndef MENU_MISC_MESSAGE_H_INCLUDED
#define MENU_MISC_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「その他」 タブ
 *--------------------------------------------------------------*/

enum {
	DATA_MISC_SUSPEND,
	DATA_MISC_SNAPSHOT,
	DATA_MISC_WAVEOUT,
};
static const t_menulabel data_misc[] = {
	{ { "State Save     ",	"ステートセーブ ",	}, },
	{ { "Screen Shot    ",	"画面保存       ",	}, },
	{ { "Sound Record   ",	"サウンド保存   ",	}, },
};





enum {
	DATA_MISC_SUSPEND_CHANGE,
	DATA_MISC_SUSPEND_SAVE,
	DATA_MISC_SUSPEND_LOAD,
	DATA_MISC_SUSPEND_NUMBER,
	DATA_MISC_SUSPEND_FSEL
};
static const t_menulabel data_misc_suspend[] = {
	{ { " Change ",								" ファイル変更 ",						}, },
	{ { " SAVE ",								" セーブ ",								}, },
	{ { " LOAD ",								" ロード ",								}, },
	{ { "                         number : ",	"                 連番： ",				}, },
	{ { " Input (Select) a state filename. ",	" ステートファイル名を入力して下さい ",	}, },
};


static const t_menudata data_misc_suspend_num[] = {
	{ { "(none)",	"(なし)",	}, 0,	},
	{ { "0",		"0",		}, '0',	},
	{ { "1",		"1",		}, '1',	},
	{ { "2",		"2",		}, '2',	},
	{ { "3",		"3",		}, '3',	},
	{ { "4",		"4",		}, '4',	},
	{ { "5",		"5",		}, '5',	},
	{ { "6",		"6",		}, '6',	},
	{ { "7",		"7",		}, '7',	},
	{ { "8",		"8",		}, '8',	},
	{ { "9",		"9",		}, '9',	},
};




enum {
	DATA_MISC_SUSPEND_OK,
	DATA_MISC_RESUME_OK,
	DATA_MISC_SUSPEND_LINE,
	DATA_MISC_SUSPEND_INFO,
	DATA_MISC_SUSPEND_AGREE,
	DATA_MISC_SUSPEND_ERR,
	DATA_MISC_RESUME_ERR,
	DATA_MISC_SUSPEND_REALLY,
	DATA_MISC_SUSPEND_OVERWRITE,
	DATA_MISC_SUSPEND_CANCEL,
	DATA_MISC_RESUME_CANTOPEN
};
static const t_menulabel data_misc_suspend_err[] = {
	{ { "State save Finished.",								"状態を保存しました。",									}, },
	{ { "State load Finished.",								"状態を復元しました。",									}, },
	{ { "----------------------------------------------",	"----------------------------------------------",		}, },
	{ { "      ( Following image files are set )       ",	" ( 以下のイメージファイルが設定されています ) ",		}, },
	{ { " OK ",												"確認",													}, },
	{ { "Error / State save failed.",						"エラー／状態は保存されませんでした。",					}, },
	{ { "Error / State load failed. Reset done",			"エラー／状態の復元に失敗しました。リセットします。",	}, },
	{ { "State-file already exist, Over write ?",			"ファイルはすでに存在します。上書きしますか？",			}, },
	{ { " Over Write ",										"上書き",												}, },
	{ { " Cancel ",											"取消",													}, },
	{ { "State-file not exist or broken.",					"ステートファイルが無いか、壊れています。",				}, },
};



enum {
	DATA_MISC_SNAPSHOT_FORMAT,
	DATA_MISC_SNAPSHOT_CHANGE,
	DATA_MISC_SNAPSHOT_PADDING,
	DATA_MISC_SNAPSHOT_BUTTON,
	DATA_MISC_SNAPSHOT_FSEL,
	DATA_MISC_SNAPSHOT_CMD
};
static const t_menulabel data_misc_snapshot[] = {
	{ { " Format   ",											" 画像形式   ",											}, },
	{ { " Change ",												" ベース名変更 ",										}, },
	{ { "                    ",									"            ",											}, },
	{ { " SAVE ",												" 保存 ",												}, },
	{ { " Input (Select) a screen-snapshot base-filename. ",	" 保存するファイル (ベース名) を入力して下さい ",		}, },
#ifdef USE_SSS_CMD
	{ { "Exec following Command",								"次のコマンドを実行する",								}, },
#endif /* USE_SSS_CMD */
};

static const t_menudata data_misc_snapshot_format[] = {
	{ { " BMP ",	" BMP ",	}, 0,	},
	{ { " PPM ",	" PPM ",	}, 1,	},
	{ { " RAW ",	" RAW ",	}, 2,	},
};



enum {
	DATA_MISC_WAVEOUT_CHANGE,
	DATA_MISC_WAVEOUT_START,
	DATA_MISC_WAVEOUT_STOP,
	DATA_MISC_WAVEOUT_PADDING,
	DATA_MISC_WAVEOUT_FSEL
};
static const t_menulabel data_misc_waveout[] = {
	{ { " Change ",											" ベース名変更 ",									}, },
	{ { " START ",											" 開始 ",											}, },
	{ { " STOP ",											" 停止 ",											}, },
	{ { "                                            ",		"                                       ",			}, },
	{ { " Input (Select) a sound-record base-filename. ",	" 出力するファイル (ベース名) を入力して下さい ",	}, },
};



static const t_menulabel data_misc_sync[] = {
	{ { "synchronize filename with disk-image filename",	"各ファイル名をディスクイメージのファイル名に合わせる",	}, },
};

#endif /* MENU_MISC_MESSAGE_H_INCLUDED */
