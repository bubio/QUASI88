#ifndef MENU_LOCAL_MESSAGE_H_INCLUDED
#define MENU_LOCAL_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * ファイル操作エラーダイアログ
 *--------------------------------------------------------------*/

static const t_menulabel data_err_drive[] = {
	{ { " OK ",														" 確認 ",														}, },
	{ { "File in DRIVE %d: / can't open the file, or bad format.",	"ドライブ %d:／ファイルが開けないか、ファイル形式が違います。",	}, },
	{ { "File in DRIVE %d: / can't write the file.",				"ドライブ %d:／このファイルには書き込みができません。",			}, },
	{ { "File in DRIVE %d: / maybe broken.",						"ドライブ %d:／ファイルが(多分)壊れています。",					}, },
	{ { "File in DRIVE %d: / SEEK Error.",							"ドライブ %d:／シークエラーが発生しました。",					}, },
	{ { "File in DRIVE %d: / WRITE Error.",							"ドライブ %d:／書き込みエラーが発生しました。",					}, },
	{ { "File in DRIVE %d: / strings too long.",					"ドライブ %d:／入力文字列が長過ぎます。",						}, },
	{ { "File in DRIVE %d: / UNEXPECTED Error.",					"ドライブ %d:／予期せぬエラーが発生しました。",					}, },
};
static const t_menulabel data_err_file[] = {
	{ { " OK ",								" 確認 ",											}, },
	{ { "Error / can't open the file.",		"エラー／ファイルが開けません。",					}, },
	{ { "Error / can't write the file.",	"エラー／このファイルには書き込みができません。",	}, },
	{ { "Error / maybe broken.",			"エラー／ファイルが(多分)壊れています。",			}, },
	{ { "Error / SEEK Error.",				"エラー／シークエラーが発生しました。",				}, },
	{ { "Error / WRITE Error.",				"エラー／書き込みエラーが発生しました。",			}, },
	{ { "Error / strings too long.",		"エラー／入力文字列が長過ぎます。",					}, },
	{ { "Error / UNEXPECTED Error.",		"エラー／予期せぬエラーが発生しました。",			}, },
};

#endif /* MENU_LOCAL_MESSAGE_H_INCLUDED */
