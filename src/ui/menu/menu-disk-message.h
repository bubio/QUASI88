#ifndef MENU_DISK_MESSAGE_H_INCLUDED
#define MENU_DISK_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「ディスク」 タブ
 *--------------------------------------------------------------*/
static const t_menulabel data_disk_image_drive[] = {
	{ { " <<< DRIVE [1:] >>> ",	" <<< DRIVE [1:] >>> ",	}, },
	{ { " <<< DRIVE [2:] >>> ",	" <<< DRIVE [2:] >>> ",	}, },
};
static const t_menulabel data_disk_info_drive[] = {
	{ { "   DRIVE [1:]   ",		"   DRIVE [1:]   ",		}, },
	{ { "   DRIVE [2:]   ",		"   DRIVE [2:]   ",		}, },
};

enum {
	DATA_DISK_IMAGE_EMPTY,
	DATA_DISK_IMAGE_BLANK
};
static const t_menulabel data_disk_image[] = {
	{ { "< EMPTY >                 ",	"< なし >                  ",	}, },
	{ { "  Create Blank  ",				" ブランクの作成 ",				}, },
};

enum {
	DATA_DISK_INFO_STAT,		/* "STATUS     READY" で16文字 */
	DATA_DISK_INFO_STAT_READY,
	DATA_DISK_INFO_STAT_BUSY,
	DATA_DISK_INFO_ATTR,		/* "ATTR  READ/WRITE" で16文字 */
	DATA_DISK_INFO_ATTR_RW,
	DATA_DISK_INFO_ATTR_RO,
	DATA_DISK_INFO_NR,			/* "IMAGE  xxxxxxxxx" で16文字、 x は 9文字 */
	DATA_DISK_INFO_NR_BROKEN,
	DATA_DISK_INFO_NR_OVER
};
static const t_menulabel data_disk_info[] = {
	{ { "STATUS     ",		"状態       ",		} },
	{ {            "READY",	           "READY",	} },
	{ {            "BUSY ",	           "BUSY ",	} },
	{ { "ATTR  ",			"属性  ",			} },
	{ {       "  Writable",	      "    書込可",	} },
	{ {       " Read Only",	      "  読込専用",	} },
	{ { "IMAGE  ",			"総数   ",			} },
	{ {          "+BROKEN",	          " +破損",	} },
	{ {            " OVER",	          " 以上 ",	} },
};



enum {
	IMG_OPEN,
	IMG_CLOSE,
	IMG_BOTH,
	IMG_COPY,
	IMG_ATTR
};
static const t_menulabel data_disk_button_drv1[] = {
	{ { " DRIVE [1:]           OPEN ",	" DRIVE [1:]           開く ",	}, },
	{ { " DRIVE [1:]          CLOSE ",	" DRIVE [1:]         閉じる ",	}, },
	{ { " DRIVE [1:][2:] BOTH  OPEN ",	" DRIVE [1:][2:] 両方に開く ",	}, },
	{ { " DRIVE [1:] <= [2:]   OPEN ",	" DRIVE [1:] ← [2:]   開く ",	}, },
	{ { " CHANGE ATTRIBUTE of IMAGE ",	" イメージの 属性を変更する ",	}, },
};
static const t_menulabel data_disk_button_drv2[] = {
	{ { " DRIVE [2:]           OPEN ",	" DRIVE [2:]           開く ",	}, },
	{ { " DRIVE [2:]          CLOSE ",	" DRIVE [2:]         閉じる ",	}, },
	{ { " DRIVE [1:][2:] BOTH  OPEN ",	" DRIVE [1:][2:] 両方に開く ",	}, },
	{ { " DRIVE [1:] => [2:]   OPEN ",	" DRIVE [1:] → [2:]   開く ",	}, },
	{ { " CHANGE ATTRIBUTE of IMAGE ",	" イメージの 属性を変更する ",	}, },
};
static const t_menulabel data_disk_button_drv1swap[] = {
	{ { " OPEN           DRIVE [1:] ",	" 開く           DRIVE [1:] ",	}, },
	{ { " CLOSE          DRIVE [1:] ",	" 閉じる         DRIVE [1:] ",	}, },
	{ { " OPEN  BOTH DRIVE [1:][2:] ",	" 両方に開く DRIVE [2:][1:] ",	}, },
	{ { " OPEN   DRIVE [1:] => [2:] ",	" 開く   DRIVE [2:] → [1:] ",	}, },
	{ { " CHANGE ATTRIBUTE of IMAGE ",	" イメージの 属性を変更する ",	}, },
};
static const t_menulabel data_disk_button_drv2swap[] = {
	{ { " OPEN           DRIVE [2:] ",	" 開く           DRIVE [2:] ",	}, },
	{ { " CLOSE          DRIVE [2:] ",	" 閉じる         DRIVE [2:] ",	}, },
	{ { " OPEN  BOTH DRIVE [1:][2:] ",	" 両方に開く DRIVE [2:][1:] ",	}, },
	{ { " OPEN   DRIVE [1:] <= [2:] ",	" 開く   DRIVE [2:] ← [1:] ",	}, },
	{ { " CHANGE ATTRIBUTE of IMAGE ",	" イメージの 属性を変更する ",	}, },
};



enum {
	DATA_DISK_OPEN_OPEN,
	DATA_DISK_OPEN_BOTH
};
static const t_menulabel data_disk_open_drv1[] = {
	{ { " OPEN FILE in DRIVE [1:] ",		" DRIVE [1:] にイメージファイルをセットします ",			}, },
	{ { " OPEN FILE in DRIVE [1:] & [2:] ",	" DRIVE [1:] と [2:] にイメージファイルをセットします ",	}, },
};
static const t_menulabel data_disk_open_drv2[] = {
	{ { " OPEN FILE in DRIVE [2:] ",		" DRIVE [2:] にイメージファイルをセットします ",			}, },
	{ { " OPEN FILE in DRIVE [1:] & [2:] ",	" DRIVE [1:] と [2:] にイメージファイルをセットします ",	}, },
};



enum {
	DATA_DISK_ATTR_TITLE1,
	DATA_DISK_ATTR_TITLE1_,
	DATA_DISK_ATTR_TITLE2,
	DATA_DISK_ATTR_RENAME,
	DATA_DISK_ATTR_PROTECT,
	DATA_DISK_ATTR_FORMAT,
	DATA_DISK_ATTR_BLANK,
	DATA_DISK_ATTR_CANCEL
};
static const t_menulabel data_disk_attr[] = {
	{ { " Change Attribute of the image at drive 1: ",	" ドライブ 1: のイメージ ",		}, },
	{ { " Change Attribute of the image at drive 2: ",	" ドライブ 2: のイメージ ",		}, },
	{ { " ",											" の 属性変更などを行います ",	}, },
	{ { "RENAME",										"名前変更",						}, },
	{ { "PROTECT",										"属性変更",						}, },
	{ { "(UN)FORMAT",									"(アン)フォーマット",			}, },
	{ { "APPEND BLANK",									"ブランクの追加",				}, },
	{ { "CANCEL",										" 取消 ",						}, },
};

enum {
	DATA_DISK_ATTR_RENAME_TITLE1,
	DATA_DISK_ATTR_RENAME_TITLE1_,
	DATA_DISK_ATTR_RENAME_TITLE2,
	DATA_DISK_ATTR_RENAME_OK,
	DATA_DISK_ATTR_RENAME_CANCEL
};
static const t_menulabel data_disk_attr_rename[] = {
	{ { " Rename the image at drive 1: ",	" ドライブ 1: のイメージ ",	}, },
	{ { " Rename the image at drive 2: ",	" ドライブ 2: のイメージ ",	}, },
	{ { " ",								" の 名前を変更します ",	}, },
	{ { "  OK  ",							" 変更 ",					}, },
	{ { "CANCEL",							" 取消 ",					}, },
};

enum {
	DATA_DISK_ATTR_PROTECT_TITLE1,
	DATA_DISK_ATTR_PROTECT_TITLE1_,
	DATA_DISK_ATTR_PROTECT_TITLE2,
	DATA_DISK_ATTR_PROTECT_SET,
	DATA_DISK_ATTR_PROTECT_UNSET,
	DATA_DISK_ATTR_PROTECT_CANCEL
};
static const t_menulabel data_disk_attr_protect[] = {
	{ { " (Un)Peotect the image at drive 1: ",	" ドライブ 1: のイメージ ",			}, },
	{ { " (Un)Peotect the image at drive 2: ",	" ドライブ 2: のイメージ ",			}, },
	{ { " ",									" の プロテクト状態を変更します ",	}, },
	{ { " SET PROTECT ",						" プロテクト状態にする ",			}, },
	{ { " UNSET PROTECT ",						" プロテクトを解除する ",			}, },
	{ { " CANCEL ",								" 取消 ",							}, },
};

enum {
	DATA_DISK_ATTR_FORMAT_TITLE1,
	DATA_DISK_ATTR_FORMAT_TITLE1_,
	DATA_DISK_ATTR_FORMAT_TITLE2,
	DATA_DISK_ATTR_FORMAT_WARNING,
	DATA_DISK_ATTR_FORMAT_DO,
	DATA_DISK_ATTR_FORMAT_NOT,
	DATA_DISK_ATTR_FORMAT_CANCEL
};
static const t_menulabel data_disk_attr_format[] = {
	{ { " (Un)Format the image at drive 1: ",		" ドライブ 1: のイメージ ",						}, },
	{ { " (Un)Format the image at drive 2: ",		" ドライブ 2: のイメージ ",						}, },
	{ { " ",										" を （アン）フォーマットします ",				}, },
	{ { "[WARNING : data in the image will lost!]",	"[注意:イメージ内のデータは消去されます！]",	}, },
	{ { " FORMAT ",									" フォーマットする ",							}, },
	{ { " UNFORMAT ",								" アンフォーマットする ",						}, },
	{ { " CANCEL ",									" 取消 ",										}, },
};

enum {
	DATA_DISK_ATTR_BLANK_TITLE1,
	DATA_DISK_ATTR_BLANK_TITLE1_,
	DATA_DISK_ATTR_BLANK_TITLE2,
	DATA_DISK_ATTR_BLANK_OK,
	DATA_DISK_ATTR_BLANK_CANCEL,
	DATA_DISK_ATTR_BLANK_END
};
static const t_menulabel data_disk_attr_blank[] = {
	{ { " Append Blank image at drive 1: ",	" ドライブ 1: のファイルに ",		}, },
	{ { " Append Blank image at drive 2: ",	" ドライブ 2: のファイルに ",		}, },
	{ { " ",								" ブランクイメージを追加します ",	}, },
	{ { " APPEND ",							" ブランクイメージの追加 ",			}, },
	{ { " CANCEL ",							" 取消 ",							}, },
};



enum {
	DATA_DISK_BLANK_FSEL,
	DATA_DISK_BLANK_WARN_0,
	DATA_DISK_BLANK_WARN_1,
	DATA_DISK_BLANK_WARN_APPEND,
	DATA_DISK_BLANK_WARN_CANCEL
};
static const t_menulabel data_disk_blank[] = {
	{ { " Create a new file as blank image file.",	" ブランクイメージファイルを新規作成します ",	}, },
	{ { " This File Already Exist. ",				" 指定したファイルはすでに存在します。 ",		}, },
	{ { " Append a blank image ? ",					" ブランクイメージを追加しますか？ ",			}, },
	{ { " APPEND ",									" 追加する ",									}, },
	{ { " CANCEL ",									" 取消 ",										}, },
};


enum {
	DATA_DISK_FNAME,
	DATA_DISK_FNAME_TITLE,
	DATA_DISK_FNAME_LINE,
	DATA_DISK_FNAME_SAME,
	DATA_DISK_FNAME_SEP,
	DATA_DISK_FNAME_RO,
	DATA_DISK_FNAME_RO_1,
	DATA_DISK_FNAME_RO_2,
	DATA_DISK_FNAME_RO_X,
	DATA_DISK_FNAME_RO_Y,
	DATA_DISK_FNAME_OK
};
static const t_menulabel data_disk_fname[] = {
	{ { " Show Filename  ",									" ファイル名確認 ",									}, },
	{ { " Disk Image Filename ",							" ディスクイメージファイル名確認 ",					}, },
	{ { "------------------------------------------------",	"------------------------------------------------",	}, },
	{ { " Same file Drive 1: as Drive 2 ",					" ドライブ 1: と 2: は同じファイルです ",			}, },
	{ { " ",												" ",												}, },
	{ { "    * The disk image file(s) is read-only.      ",	"●読込専用のディスクイメージファイルがあります。",	}, },
	{ { "      All images in this file are regarded      ",	"  このファイルに含まれるすべてのイメージは、    ",	}, },
	{ { "      as WRITE-PROTECTED.                       ",	"  ライトプロテクト状態と同様に扱われます。      ",	}, },
	{ { "      Writing to the image is ignored, but      ",	"  このファイルへの書き込みは全て無視されますが、",	}, },
	{ { "      not error depending on situation.         ",	"  エラーとは認識されない場合があります。        ",	}, },
	{ { "  OK  ",											" 確認 ",											}, },
};



enum {
	DATA_DISK_DISPSWAP,
	DATA_DISK_DISPSWAP_INFO_1,
	DATA_DISK_DISPSWAP_INFO_2,
	DATA_DISK_DISPSWAP_OK
};
static const t_menulabel data_disk_dispswap[] = {
	{ { "Swap Drv-Disp",					"ドライブ逆並べ",										}, },
	{ { "Swap Drive-Display placement",		"DRIVE [1:] と [2:] の表示位置を、左右入れ換えます",	}, },
	{ { "This setting effects next time. ",	"この設定は次回のメニューモードより有効となります ",	}, },
	{ { "  OK  ",							" 確認 ",												}, },
};



enum {
	DATA_DISK_DISPSTATUS
};
static const t_menulabel data_disk_dispstatus[] = {
	{ { "Show Imagename",					"イメージ名表示",										}, },
};

#endif /* MENU_DISK_MESSAGE_H_INCLUDED */
