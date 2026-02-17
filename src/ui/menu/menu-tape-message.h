#ifndef MENU_TAPE_MESSAGE_H_INCLUDED
#define MENU_TAPE_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「テープ」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_TAPE_IMAGE,
	DATA_TAPE_INTR
};
static const t_menulabel data_tape[] = {
	{ { " Tape image ",			" テープイメージ ",			}, },
	{ { " Tape Load Timing ",	" テープロードの処理方法 ",	}, },
};



enum {
	DATA_TAPE_FOR,
	DATA_TAPE_CHANGE,
	DATA_TAPE_EJECT,
	DATA_TAPE_FSEL,
	DATA_TAPE_REWIND,
	DATA_TAPE_WARN_0,
	DATA_TAPE_WARN_1,
	DATA_TAPE_WARN_APPEND,
	DATA_TAPE_WARN_CANCEL
};
static const t_menulabel data_tape_load[] = {
	{ { " for Load :",												" ロード用：",										}, },
	{ { " Change File ",											" ファイル変更 ",									}, },
	{ { " Eject  ",													" 取出し ",											}, },
	{ { " Input (Select) a tape-load-image filename. (CMT/T88)",	" ロード用テープイメージ(CMT/T88)を入力して下さい",	}, },
	{ { " Rewind ",													" 巻戻し ",											}, },
};
static const t_menulabel data_tape_save[] = {
	{ { " for Save :",												" セーブ用：",										}, },
	{ { " Change File ",											" ファイル変更 ",									}, },
	{ { " Eject  ",													" 取出し ",											}, },
	{ { " Input (Select) a tape-save-image filename. (CMT)",		" セーブ用テープイメージ(CMT)を入力して下さい",		}, },
	{ { NULL,														NULL,												}, },
	{ { " This File Already Exist. ",								" 指定したファイルはすでに存在します。 ",			}, },
	{ { " Append a tape image ? ",									" テープイメージを追記していきますか？ ",			}, },
	{ { " OK ",														" 追記する ",										}, },
	{ { " CANCEL ",													" 取消 ",											}, },
};


static const t_menudata data_tape_intr[] = {
	{ { " Use Interrupt     (Choose in N88-BASIC mode) ",					" 割り込みを使う     (N88-BASIC では、必ずこちらを選択してください) ",	}, TRUE,	},
	{ { " Not Use Interrupt (Choose in N-BASIC mode for LOAD speed-up) ",	" 割り込みを使わない (N-BASIC は、こちらでも可。ロードが速くなります) ", }, FALSE,	},
};

#endif /* MENU_TAPE_MESSAGE_H_INCLUDED */
