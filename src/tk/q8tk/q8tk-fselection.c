/************************************************************************/
/* QUASI88 メニュー用 Tool Kit                                          */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "q8tk.h"
#include "q8tk-common.h"
#include "q8tk-glib.h"

#include "file-op.h"


/*---------------------------------------------------------------------------
 * FILE SELECTION
 *---------------------------------------------------------------------------
 * ・ウィジットを組み合わせて、ファイルセレクションを作り出す。
 * ・扱えるファイル名の長さは、パスを含めて最大で
 *   Q8TK_MAX_FILENAME までである。(文字列終端の\0を含む)
 * ・ファイル名の一覧は、システム依存関数にて処理する。
 * --------------------------------------------------------------------------
 * Q8tkWidget *q8tk_file_selection_new(const char *title, int select_ro)
 *    ファイルセレクションの生成。
 *    見出し表示用の文字列 title を指定する。
 *    select_ro が 1 なら、「Read Only」チェックボタンが表示され、チェック
 *    される。0 なら、「Read Only」チェックボタンが表示されるが、チェックは
 *    されない。-1 なら、「Read Only」チェックボタンは表示されない。
 *     (チェックされたかどうかは、 q8tk_file_selection_get_readonly にて
 *      取得できるので、呼び出し元でリードオンリーで開くかどうかを決定する)
 *
 * Q8tkWidget *q8tk_file_selection_simple_new(void)
 *    簡易版ファイルセレクションの生成。
 *    通常のファイルセレクションと外観以外に、以下が異なる
 *      o ディレクトリ名やファイル数は表示されない
 *      o エントリは表示されない (ファイル名を入力しての選択は不可)
 *      o VIEW ボタンは表示されない
 *      o Read Only チェックボックスは表示されない (Read Only の選択は不可)
 *      o OK ボタンは表示されない。ファイルの選択はリストアイテムで行う
 *      o EJECT ボタンが追加で表示される
 *      o 対象ドライブを指定する Drive N: チェックボックスが追加で表示される
 *
 * const char *q8tk_file_selection_get_filename(Q8tkWidget *fselect)
 *    選択(入力)されているファイル名を取得する。
 *
 * void q8tk_file_selection_set_filename(Q8tkWidget *fselect,
 *                                       const char *filename)
 *    ファイル名 (ディレクトリ名でも可) を指定する。
 *
 * int q8tk_file_selection_get_readonly(Q8tkWidget *fselect)
 *    真 なら、「リードオンリーで開く」がチェックされている。
 * --------------------------------------------------------------------------
 *     ：……→【FILE SELECTION】
 *     ↓
 * 【WINDOW】←┐
 * ┌─────┘
 * └→【VBOX】←→【LABEL】       q8tk_file_selection_new()の引数
 *                   ↑↓
 *             ←  【HSEPARATOR】
 *                   ↑↓
 *             ←  【LABEL】       ディレクトリ名
 *                   ↑↓
 *             ←  【HBOX】←→【LABEL】 (すきま)
 *                   ↑｜        ↑↓
 *                   ｜｜  ←  【SCROLLED WINDOW】←→【LIST BOX】←→(A)
 *                   ｜｜        ↑｜    ｜｜
 *                   ｜｜        ｜｜    ｜└─  【ADJUSTMENT】
 *                   ｜｜        ｜｜    └──  【ADJUSTMENT】
 *                   ｜｜        ｜↓
 *                   ｜｜  ←  【LABEL】 (すきま)
 *                   ｜｜        ↑↓
 *                   ｜｜  ←  【VBOX】←→【LABEL】 (すきま)
 *                   ｜｜                    ↑↓
 *                   ｜｜              ←  【LABEL】 ファイル数
 *                   ｜｜                    ↑↓
 *                   ｜｜              ←  【LABEL】 (すきま×4)
 *                   ｜｜                    ↑↓
 *                   ｜｜              ←  【BUTTON】←→【LABEL】
 *                   ｜｜                    ↑｜          VIEW
 *                   ｜｜                    ｜↓
 *                   ｜｜              ←  【LABEL】 (すきま×2)
 *                   ｜｜                    ↑↓
 *                   ｜｜              ←  【CHECK BUTTON】←→【LABEL】
 *                   ｜｜                    ↑｜                Read only
 *                   ｜｜                    ｜↓
 *                   ｜｜              ←  【BUTTON】←→【LABEL】
 *                   ｜｜                    ↑｜          (ok_button)
 *                   ｜｜                    ｜↓
 *                   ｜｜              ←  【BUTTON】←→【LABEL】
 *                   ｜｜                                  (cancel_button)
 *                   ｜↓
 *             ←  【LABEL】       (空行)
 *                   ↑↓
 *             ←  【HBOX】←→【LABEL】   Filename(見出し)
 *                               ↑↓
 *                         ←  【ENTRY】   ファイル名入力
 *
 * q8tk_file_selection_new() の返り値は 【WINDOW】 のウィジットである。
 * モーダルにする場合は、このウィジットを q8tk_grab_add() する。
 *
 * cancel_button、ok_button はユーザが任意にシグナルを設定できる。
 *
 *        fsel = q8tk_file_selection_new();
 *        q8tk_widget_show(fsel);
 *        q8tk_grab_add(fsel);
 *        q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->ok_button,
 *                            "clicked", ユーザ関数、ユーザ引数);
 *        q8tk_signal_connect(Q8TK_FILE_SELECTION(fsel)->cancel_button,
 *                            "clicked", ユーザ関数、ユーザ引数);
 *
 * 簡易版ファイルセレクションの場合、上記の一部のウィジットが非表示に
 * なり、さらに構成も異なる。 eject_button が新たに追加されており、
 * cancel_button、ok_button、eject_button はユーザが任意にシグナルを
 * 設定できる。(なお、 ok_button は表示されない)
 *
 * ---------------------------------------------------------------------
 * (A)  ←→【LIST ITEM】←→【LABEL】
 *            ↑↓
 *      ←  【LIST ITEM】←→【LABEL】
 *            ↑↓
 *      ←  【LIST ITEM】←→【LABEL】
 *            ↑↓
 *
 * q8tk_file_selection_set_filename() にてディレクトリ名をセットすると、
 * そのディレクトリのファイル一覧を取得し、(この処理はシステム依存)、
 * 全てのファイル名をリストアイテムにして、リストボックスに乗せる。
 * ファイル名をセットした場合も、そこからディレクトリ名を切り出して
 * (ここの処理もシステム依存)、同様にディレクトリのファイル一覧を生成
 * する。この場合、エントリにもファイル名をセットする。
 *
 * ---------------------------------------------------------------------
 * 以下の場合、ok_button に "clicked" シグナルが発生する。
 *    ・OK のボタンをクリックした場合
 *    ・ファイル名一覧のリストアイテムの、アクティブなのをクリックし
 *      それがディレクトリでなかった場合。
 *    ・エントリにて入力してリターンキーを押下し、
 *      それがディレクトリでなかった場合。
 *
 * つまり、ok_button に "clicked" シグナルを設定しておけば、
 * 一般的な操作によるファイル選択(入力)を検知できる。
 *
 * ---------------------------------------------------------------------
 * q8tk_widget_destroy() の際は、すべてのウィジットを破壊する。
 * (不定数の LIST ITEM ウイジットも含む)
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * 以下の関数の引数 fselect は q8tk_file_selection_new() の戻り値の、
 * ウインドウウィジットである。ファイルセレクションウィジットの変数
 * (fselect->stat.fselect.変数名) にアクセスするには、以下のようにする。
 * Q8TK_FILE_SELECTION((Q8tkWidget*)fselect)->変数名
 *---------------------------------------------------------------------------*/

/* ファイルセレクションのリストボックス内のリストアイテムがクリックされた場合
 *
 * ・直前に選択していたリストボックスと違うのがクリックされた場合
 *     fsel_list_selection_changed() ("selection_changed"イベントによる)
 *     fsel_item_selected()          ("select"           イベントによる)
 *   の順に呼び出される。
 *
 * ・直前に選択していたリストボックスと同じのがクリックされた場合
 *     fsel_item_selected()          ("select"           イベントによる)
 *   だけが呼び出される。 ("selection_changed" イベントは発生しないので)
 */


static void fsel_update(Q8tkWidget *fselect, const char *filename, int type);
static int fsel_opendir(Q8tkWidget *fselect, const char *filename, int type);


/* ファイル名一覧 LIST BOX にて、選択中のファイルが変わった時のコールバック */
static void fsel_selection_changed_callback(UNUSED_WIDGET, void *fselect)
{
	Q8TK_FILE_SELECTION((Q8tkWidget *)fselect)->selection_changed = TRUE;
}


/* ファイル名の LIST ITEM にて、 クリックした時のコールバック */
static void fsel_selected_callback(Q8tkWidget *item, void *fselect)
{
	const char *name;
	if (item->name) {
		name = item->name;
	} else if (item->child && item->child->name) {
		name = item->child->name;
	} else {
		name = "";
	}


	if (Q8TK_FILE_SELECTION((Q8tkWidget *)fselect)->selection_changed) {

		/* 直前に fsel_selection_changed_callback() が呼び出されている場合、
		 * つまり『前とは違う』リストアイテムがクリックされた場合は、
		 * そのリストアイテムの保持するファイル名を、エントリにセットする */

		Q8TK_FILE_SELECTION((Q8tkWidget *)fselect)->selection_changed = FALSE;

		q8tk_entry_set_text(Q8TK_FILE_SELECTION((Q8tkWidget *)fselect)
							->selection_entry, name);

	} else {

		/* 今回 fsel_selected_callback() だけが呼び出された場合、
		 * つまり『前と同じ』リストアイテムがクリックされた場合は、
		 * そのリストアイテムの保持するファイル名で、ファイル一覧更新 */

		fsel_update((Q8tkWidget *)fselect, name, item->stat.listitem.fsel_type);
	}
}


/* ファイル名入力の ENTRY にて、リターンキー押下時のコールバック */
static void fsel_activate_callback(Q8tkWidget *entry, void *fselect)
{
	const char *input_filename = q8tk_entry_get_text(entry);
	int   type = -1;    /* ディレクトリかどうかは不明 */

	/* 入力したファイル名が、リストの name に一致すれば、
	 * ファイルタイプを確定することができる。
	 * (あえて確定させる必要はないので、適当に一致チェックする) */

	Q8tkWidget *match_list;
	match_list = q8tk_listbox_search_items(
					 Q8TK_FILE_SELECTION((Q8tkWidget *)fselect)->file_list,
					 input_filename);

	if (match_list) {
		type = match_list->stat.listitem.fsel_type;
	}


	/* 入力したファイル名で、ファイル一覧更新 */
	fsel_update((Q8tkWidget *)fselect, input_filename, type);
}


/* ファイル一覧 LIST BOX を更新する。 ディレクトリ名なら表示更新、
 *  ファイル名が確定した場合は、OK ボタンを押したことにする */
static void fsel_update(Q8tkWidget *fselect, const char *filename, int type)
{
	char wk[ Q8TK_MAX_FILENAME ];

	if (osd_path_join(Q8TK_FILE_SELECTION(fselect)->pathname, filename,
					  wk, Q8TK_MAX_FILENAME) == FALSE) {
		strcpy(wk, "");
	}

	if (fsel_opendir(fselect, wk, type) != FILE_STAT_DIR) {
		widget_signal_do(Q8TK_FILE_SELECTION(fselect)->ok_button, "clicked");
	}
}


/* 指定されたパス名を、ディレクトリ名とファイル名に分け、 ディレクトリを開いて
 * リストにファイル一覧をセット、 ファイル名をエントリにセットする */
static int fsel_opendir_sub(Q8tkWidget *fselect, const char *dirname);
static int fsel_opendir(Q8tkWidget *fselect, const char *filename, int type)
{
	int save_code = q8tk_set_kanjicode(osd_kanji_code());

	char path[ Q8TK_MAX_FILENAME ];
	char dir [ Q8TK_MAX_FILENAME ];
	char file[ Q8TK_MAX_FILENAME ];


	if (filename == NULL || filename[0] == '\0') {
		/* filenameが無効な場合は
		 * カレントディレクトリを代わりに使うことにする */
		filename = osd_dir_cwd();
		type = FILE_STAT_DIR;
	}

	/* 正規化 */
	if (osd_path_normalize(filename, path, Q8TK_MAX_FILENAME)
		== FALSE) {
		strcpy(path, "");
		type = FILE_STAT_DIR;
	}


	/* filename をディレクトリ名とファイル名に分離する */

	if (type < 0) {
		/* ディレクトリかどうか不明なら、まずは属性を取得   */
		type = osd_file_stat(path);
	}

	if (type == FILE_STAT_DIR) {
		/* filename はディレクトリ */

		/* ファイル名は空 */
		strcpy(dir,  path);
		strcpy(file, "");

	} else {
		/* filename はファイルらしい */

		/* ディレクトリ名と分離する  */
		if (osd_path_split(path, dir, file, Q8TK_MAX_FILENAME) == FALSE) {
			/* 失敗したら適当にごまかす */
			strcpy(dir,  "");
			strcpy(file, path);
		}
	}

	/* ディレクトリ名でディレクトリを開く */

	if (fsel_opendir_sub(fselect, dir) >= 0) {
		/* 開くのに成功した */

		Q8TK_FILE_SELECTION(fselect)->pathname[0] = '\0';
		strncat(Q8TK_FILE_SELECTION(fselect)->pathname, dir,
				Q8TK_MAX_FILENAME - 1);

		q8tk_entry_set_text(Q8TK_FILE_SELECTION(fselect)->selection_entry,
							file);
	} else {
		/* 開けなかった…… */

		q8tk_entry_set_text(Q8TK_FILE_SELECTION(fselect)->selection_entry,
							path);
	}


	q8tk_set_kanjicode(save_code);

	/* 指定されたパス名がディレクトリだったかどうかを返す */
	return type;
}


static int fsel_opendir_sub(Q8tkWidget *fselect, const char *dirname)
{
	T_DIR_INFO  *dirp;
	T_DIR_ENTRY *dirent;
	char        wk[ Q8TK_MAX_FILENAME ];
	int         nr, i, width;
	char        old_path[ Q8TK_MAX_FILENAME ];
	char        *old_dirname = NULL;

	/* 強引だけど、簡易版は LIST ITEM の表示幅最大値を固定にする */
	int w = (Q8TK_FILE_SELECTION(fselect)->scrolled_window
			 ->stat.scrolled.hpolicy == Q8TK_POLICY_NEVER) ? 54 - 5 : 0;

	/* 既存の LIST ITEM 削除 */

	if (Q8TK_FILE_SELECTION(fselect)->file_list->child) {
		q8tk_listbox_clear_items(Q8TK_FILE_SELECTION(fselect)->file_list, 0, -1);
	}
	q8tk_adjustment_set_value(Q8TK_FILE_SELECTION(fselect)->scrolled_window
							  ->stat.scrolled.hadj, 0);
	q8tk_adjustment_set_value(Q8TK_FILE_SELECTION(fselect)->scrolled_window
							  ->stat.scrolled.vadj, 0);


	/* ディレクトリを調べ、ファイル名を LIST ITEM として登録 */
	/* アクセス可能なファイル名も併せて LIST ITEM に保持しておく */
	/* ついでなので、ファイルのタイプも LIST ITEM に保持しておく */

	if (dirname[0] != '\0' &&
		(dirp = osd_opendir(dirname))) {

		/* 旧パス名を上位パスとディレクトリ名に分離する。
		 * この上位パス名と、引数のパス名が一致すれば、
		 * この分離したディレクトリ名を覚えておく */
		if (Q8TK_FILE_SELECTION(fselect)->pathname[0] != '\0' &&
			osd_path_split(Q8TK_FILE_SELECTION(fselect)->pathname,
						   wk,
						   old_path,
						   Q8TK_MAX_FILENAME)) {
			if (strcmp(wk, dirname) == 0) {
				old_dirname = old_path;
			}
		}

		nr = 0;
		while ((dirent = osd_readdir(dirp))) {
			Q8tkWidget *item;

			wk[0] = '\0';
			strncat(wk, dirent->str, Q8TK_MAX_FILENAME - 1);

			item = q8tk_list_item_new_with_label(wk, w);
			q8tk_list_item_set_string(item, dirent->name);
			item->stat.listitem.fsel_type = dirent->type;
			q8tk_container_add(Q8TK_FILE_SELECTION(fselect)->file_list, item);
			q8tk_widget_show(item);

			/* 覚えたディレクトリ名に一致するアイテムがあれば、アクティブに */
			if (old_dirname && (strcmp(old_dirname, dirent->name) == 0)) {
				q8tk_listbox_select_child(
					Q8TK_FILE_SELECTION(fselect)->file_list, item);
				old_dirname = NULL;
			}

			q8tk_signal_connect(item, "select",
								fsel_selected_callback, fselect);
			nr ++;
		}
		osd_closedir(dirp);


		if (Q8TK_IS_NORMAL_FILE_SELECTION(fselect)) {
			i = (int) strlen(dirname);
			width = Q8TK_FILE_SELECTION(fselect)->width;
			if (i + 6 > width) {                    /* 6 == sizeof("DIR = ") */
				i = i - (width - 6 - 3);            /* 3 == sizeof("...")    */
				if (q8gr_strchk(Q8TK_FILE_SELECTION(fselect)->dir_name->code,
								dirname, i) == 2) {
					/* 漢字の途中は避ける*/
					i ++;
				}
			} else {
				i = 0;
			}
			wk[0] = '\0';
			strncat(wk, "DIR = ", Q8TK_MAX_FILENAME - 1);
			if (i) {
				strncat(wk, "...", Q8TK_MAX_FILENAME - 1 - strlen(wk));
			}
			strncat(wk, &dirname[i], Q8TK_MAX_FILENAME - 1 - strlen(wk));
		}


	} else {

		nr = -1;

		if (Q8TK_IS_NORMAL_FILE_SELECTION(fselect)) {
			wk[0] = '\0';
			strncat(wk, "DIR = non existant", Q8TK_MAX_FILENAME - 1);
		}

	}

	if (Q8TK_IS_NORMAL_FILE_SELECTION(fselect)) {
		q8tk_label_set(Q8TK_FILE_SELECTION(fselect)->dir_name, wk);

		sprintf(wk, "%4d file(s)", (nr < 0) ? 0 : nr);
		q8tk_label_set(Q8TK_FILE_SELECTION(fselect)->nr_files, wk);
	}


	return nr;
}




static void fsel_selection_view_callback(UNUSED_WIDGET, void *fselect)
{
	q8tk_utility_view(q8tk_file_selection_get_filename(fselect));
}



Q8tkWidget *q8tk_file_selection_new(const char *title, int select_ro)
{
	Q8tkWidget *fselect, *window, *vbox, *hbox, *vv, *wk;
	int i, save_code;

	fselect = malloc_widget();
	fselect->type = Q8TK_TYPE_FILE_SELECTION;
	fselect->sensitive = TRUE;

	fselect->stat.fselect.pathname = (char *)calloc(Q8TK_MAX_FILENAME, 1);
	fselect->stat.fselect.filename = (char *)calloc(Q8TK_MAX_FILENAME, 1);
	Q8tkAssert(fselect->stat.fselect.pathname, "memory exhoused");
	Q8tkAssert(fselect->stat.fselect.filename, "memory exhoused");

	window = q8tk_window_new(Q8TK_WINDOW_DIALOG);
	window->stat.window.work = fselect;

	vbox = q8tk_vbox_new();
	q8tk_container_add(window, vbox);
	q8tk_widget_show(vbox);

	/* 見出し(引数より) */
	wk = q8tk_label_new(title);
	q8tk_box_pack_start(vbox, wk);
	q8tk_widget_show(wk);
	q8tk_misc_set_placement(wk, Q8TK_PLACEMENT_X_CENTER, Q8TK_PLACEMENT_Y_TOP);
	fselect->stat.fselect.width = MAX(q8gr_strlen(wk->code, wk->name), 60);

	wk = q8tk_hseparator_new();                         /* ---------------- */
	q8tk_box_pack_start(vbox, wk);
	q8tk_widget_show(wk);
	/* DIR = /...       */
	fselect->stat.fselect.dir_name = q8tk_label_new("DIR =");
	q8tk_box_pack_start(vbox, fselect->stat.fselect.dir_name);
	q8tk_widget_show(fselect->stat.fselect.dir_name);
	q8tk_misc_set_placement(fselect->stat.fselect.dir_name,
							Q8TK_PLACEMENT_X_LEFT, Q8TK_PLACEMENT_Y_TOP);

	hbox = q8tk_hbox_new();
	q8tk_box_pack_start(vbox, hbox);
	q8tk_widget_show(hbox);
	{
		/* すきま */
		wk = q8tk_label_new("    ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);

		/* [リスト] */
		fselect->stat.fselect.scrolled_window
			= q8tk_scrolled_window_new(NULL, NULL);
		q8tk_box_pack_start(hbox, fselect->stat.fselect.scrolled_window);
		q8tk_widget_show(fselect->stat.fselect.scrolled_window);

		fselect->stat.fselect.file_list = q8tk_listbox_new();
		q8tk_container_add(fselect->stat.fselect.scrolled_window,
						   fselect->stat.fselect.file_list);
		q8tk_widget_show(fselect->stat.fselect.file_list);
		q8tk_signal_connect(fselect->stat.fselect.file_list,
							"selection_changed",
							fsel_selection_changed_callback, window);

		q8tk_misc_set_placement(fselect->stat.fselect.scrolled_window,
								Q8TK_PLACEMENT_X_CENTER,
								Q8TK_PLACEMENT_Y_CENTER);
		q8tk_scrolled_window_set_policy(fselect->stat.fselect.scrolled_window,
										Q8TK_POLICY_AUTOMATIC,
										Q8TK_POLICY_ALWAYS);
		q8tk_misc_set_size(fselect->stat.fselect.scrolled_window, 40, 18);
		q8tk_misc_set_size(fselect->stat.fselect.file_list, 40 - 3, 0);
		q8tk_listbox_set_placement(fselect->stat.fselect.file_list, -2, +1);


		/* すきま */
		wk = q8tk_label_new("  ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);


		vv = q8tk_vbox_new();
		q8tk_box_pack_start(hbox, vv);
		q8tk_widget_show(vv);
		{

			/* 空行 */
			wk = q8tk_label_new("");
			q8tk_box_pack_start(vv, wk);
			q8tk_widget_show(wk);

			/* n file(s) */
			fselect->stat.fselect.nr_files = q8tk_label_new("0000 file(s)");
			q8tk_box_pack_start(vv, fselect->stat.fselect.nr_files);
			q8tk_widget_show(fselect->stat.fselect.nr_files);
			q8tk_misc_set_placement(fselect->stat.fselect.nr_files,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
#if 0
			/* 空行 */
			for (i = 0; i < 9; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vv, wk);
				q8tk_widget_show(wk);
			}
#else
			/* 空行 */
			for (i = 0; i < 4; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vv, wk);
				q8tk_widget_show(wk);
			}
			/* [VIEW] */
			wk = q8tk_button_new_with_label(" VIEW ");
			q8tk_box_pack_start(vv, wk);
			q8tk_widget_show(wk);
			q8tk_misc_set_placement(wk,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
			q8tk_signal_connect(wk, "clicked",
								fsel_selection_view_callback, window);

			/* 空行 */
			for (i = 0; i < 2; i++) {
				wk = q8tk_label_new("");
				q8tk_box_pack_start(vv, wk);
				q8tk_widget_show(wk);
			}
#endif

			/* Read only */
			fselect->stat.fselect.ro_button =
				q8tk_check_button_new_with_label("Read only");
			q8tk_box_pack_start(vv, fselect->stat.fselect.ro_button);
			q8tk_misc_set_placement(fselect->stat.fselect.ro_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
			if (select_ro >= 0) {
				if (select_ro > 0) {
					q8tk_toggle_button_set_state(
						fselect->stat.fselect.ro_button, TRUE);
				}
				q8tk_widget_show(fselect->stat.fselect.ro_button);
			}

			/* [OK] */
			fselect->stat.fselect.ok_button =
				q8tk_button_new_with_label("  OK  ");
			q8tk_box_pack_start(vv, fselect->stat.fselect.ok_button);
			q8tk_widget_show(fselect->stat.fselect.ok_button);
			q8tk_misc_set_placement(fselect->stat.fselect.ok_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);

			/* [CANCEL] */
			fselect->stat.fselect.cancel_button =
				q8tk_button_new_with_label("CANCEL");
			q8tk_box_pack_start(vv, fselect->stat.fselect.cancel_button);
			q8tk_widget_show(fselect->stat.fselect.cancel_button);
			q8tk_misc_set_placement(fselect->stat.fselect.cancel_button,
									Q8TK_PLACEMENT_X_CENTER,
									Q8TK_PLACEMENT_Y_CENTER);
		}

#if 0
		/* すきま */
		wk = q8tk_label_new(" ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);
#endif
	}


	/* 空行 */
	wk = q8tk_label_new("");
	q8tk_box_pack_start(vbox, wk);
	q8tk_widget_show(wk);

	hbox = q8tk_hbox_new();
	q8tk_box_pack_start(vbox, hbox);
	q8tk_widget_show(hbox);
	{
		/* File name: */
		wk = q8tk_label_new("Filename ");
		q8tk_box_pack_start(hbox, wk);
		q8tk_widget_show(wk);

		/* [エントリ部] */
		save_code = q8tk_set_kanjicode(osd_kanji_code());
		fselect->stat.fselect.selection_entry = q8tk_entry_new();
		q8tk_box_pack_start(hbox, fselect->stat.fselect.selection_entry);
		q8tk_widget_show(fselect->stat.fselect.selection_entry);
		q8tk_misc_set_size(fselect->stat.fselect.selection_entry, 50, 0);
		q8tk_signal_connect(fselect->stat.fselect.selection_entry, "activate",
							fsel_activate_callback, window);
		q8tk_set_kanjicode(save_code);
	}

	/*  q8tk_file_selection_set_filename(window, Q8TK_FILE_SELECTION(fselect)->pathname);*/


	/* 通常版は eject_button が NULL。簡易版は 非NULL */
	/* これで通常版か簡易版かを判定している。専用の変数を用意すべきだが… */
	fselect->stat.fselect.eject_button = NULL;

	return window;
}

Q8tkWidget *q8tk_file_selection_simple_new(void)
{
	Q8tkWidget *fselect, *window, *vbox, *hbox, *wk;
	int i, save_code;

	fselect = malloc_widget();
	fselect->type = Q8TK_TYPE_FILE_SELECTION;
	fselect->sensitive = TRUE;

	fselect->stat.fselect.pathname = (char *)calloc(Q8TK_MAX_FILENAME, 1);
	fselect->stat.fselect.filename = (char *)calloc(Q8TK_MAX_FILENAME, 1);
	Q8tkAssert(fselect->stat.fselect.pathname, "memory exhoused");
	Q8tkAssert(fselect->stat.fselect.filename, "memory exhoused");

	window = q8tk_window_new(Q8TK_WINDOW_DIALOG);
	window->stat.window.work = fselect;

	hbox = q8tk_hbox_new();
	q8tk_container_add(window, hbox);
	q8tk_widget_show(hbox);

	vbox = q8tk_vbox_new();
	q8tk_box_pack_start(hbox, vbox);
	q8tk_widget_show(vbox);
	{
		Q8tkWidget *f, *vv;
		f = q8tk_frame_new("Target");
		q8tk_box_pack_start(vbox, f);
		q8tk_widget_show(f);

		vv = q8tk_vbox_new();
		q8tk_container_add(f, vv);
		q8tk_widget_show(vv);

		for (i = 0; i < 2; i++) {
			wk = q8tk_label_new("");
			q8tk_box_pack_start(vv, wk);
			q8tk_widget_show(wk);

			fselect->stat.fselect.drv_button[i] =
				q8tk_check_button_new_with_label((i == 0) ?
												 "Drive 1:" : "Drive 2:");
			q8tk_box_pack_start(vv, fselect->stat.fselect.drv_button[i]);
			q8tk_toggle_button_set_state(
				fselect->stat.fselect.drv_button[i], TRUE);
			q8tk_widget_show(fselect->stat.fselect.drv_button[i]);
		}

		wk = q8tk_label_new("");
		q8tk_box_pack_start(vv, wk);
		q8tk_widget_show(wk);

		for (i = 0; i < 10; i++) {
			wk = q8tk_label_new("");
			q8tk_box_pack_start(vbox, wk);
			q8tk_widget_show(wk);
		}

		/* [EJECT] */
		fselect->stat.fselect.eject_button =
			q8tk_button_new_with_label("EJECT ");
		q8tk_box_pack_start(vbox, fselect->stat.fselect.eject_button);
		q8tk_widget_show(fselect->stat.fselect.eject_button);
		q8tk_misc_set_placement(fselect->stat.fselect.eject_button,
								Q8TK_PLACEMENT_X_CENTER,
								Q8TK_PLACEMENT_Y_CENTER);

		/* [CANCEL] */
		fselect->stat.fselect.cancel_button =
			q8tk_button_new_with_label("CANCEL");
		q8tk_box_pack_start(vbox, fselect->stat.fselect.cancel_button);
		q8tk_widget_show(fselect->stat.fselect.cancel_button);
		q8tk_misc_set_placement(fselect->stat.fselect.cancel_button,
								Q8TK_PLACEMENT_X_CENTER,
								Q8TK_PLACEMENT_Y_CENTER);
	}

	/* すきま */
	wk = q8tk_label_new(" ");
	q8tk_box_pack_start(hbox, wk);
	q8tk_widget_show(wk);

	/* [リスト] */
	fselect->stat.fselect.scrolled_window
		= q8tk_scrolled_window_new(NULL, NULL);
	q8tk_box_pack_start(hbox, fselect->stat.fselect.scrolled_window);
	q8tk_widget_show(fselect->stat.fselect.scrolled_window);

	fselect->stat.fselect.file_list = q8tk_listbox_new();
	q8tk_container_add(fselect->stat.fselect.scrolled_window,
					   fselect->stat.fselect.file_list);
	q8tk_widget_show(fselect->stat.fselect.file_list);
	q8tk_signal_connect(fselect->stat.fselect.file_list,
						"selection_changed",
						fsel_selection_changed_callback, window);

	q8tk_scrolled_window_set_frame(fselect->stat.fselect.scrolled_window,
								   FALSE);
	q8tk_scrolled_window_set_adj_size(fselect->stat.fselect.scrolled_window,
									  2);
	q8tk_misc_set_placement(fselect->stat.fselect.scrolled_window,
							Q8TK_PLACEMENT_X_CENTER,
							Q8TK_PLACEMENT_Y_CENTER);
	q8tk_scrolled_window_set_policy(fselect->stat.fselect.scrolled_window,
									Q8TK_POLICY_NEVER,
									Q8TK_POLICY_ALWAYS);
	q8tk_misc_set_size(fselect->stat.fselect.scrolled_window, 54, 23);
	q8tk_misc_set_size(fselect->stat.fselect.file_list, (54 - 1) - 2, 0);
	/* LIST ITEM と ADJUSTMENT の間を、1コマ分空けよう       ↑ */
	q8tk_listbox_set_placement(fselect->stat.fselect.file_list, -2, +1);
	q8tk_listbox_set_big(fselect->stat.fselect.file_list, TRUE);


	/* 以下のウィジットは表示しないが、参照しているので生成する必要あり */

	/* [OK] */
	fselect->stat.fselect.ok_button = q8tk_button_new();
	q8tk_box_pack_start(hbox, fselect->stat.fselect.ok_button);

	/* [エントリ部] */
	save_code = q8tk_set_kanjicode(osd_kanji_code());
	fselect->stat.fselect.selection_entry = q8tk_entry_new();
	q8tk_misc_set_size(fselect->stat.fselect.selection_entry, 50, 0);
	q8tk_set_kanjicode(save_code);
	q8tk_box_pack_start(hbox, fselect->stat.fselect.selection_entry);

	return window;
}

const char *q8tk_file_selection_get_filename(Q8tkWidget *fselect)
{
	if (osd_path_join(Q8TK_FILE_SELECTION(fselect)->pathname,
					  q8tk_entry_get_text(Q8TK_FILE_SELECTION(fselect)
										  ->selection_entry),
					  Q8TK_FILE_SELECTION(fselect)->filename,
					  Q8TK_MAX_FILENAME)
		== FALSE) {
		return "";
	}

	return Q8TK_FILE_SELECTION(fselect)->filename;
}

void q8tk_file_selection_set_filename(Q8tkWidget *fselect,
									  const char *filename)
{
	fsel_opendir(fselect, filename, -1);
}

int q8tk_file_selection_get_readonly(Q8tkWidget *fselect)
{
	if (Q8TK_IS_NORMAL_FILE_SELECTION(fselect)) {
		return Q8TK_TOGGLE_BUTTON(Q8TK_FILE_SELECTION(fselect)->ro_button)->active;
	} else {
		return FALSE;
	}
}

int q8tk_file_selection_simple_get_drive(Q8tkWidget *fselect)
{
	int i;
	int drv = 0;

	if (Q8TK_IS_NORMAL_FILE_SELECTION(fselect)) {
		return 0;
	}

	for (i = 0; i < 2; i++) {
		if (Q8TK_TOGGLE_BUTTON(Q8TK_FILE_SELECTION(fselect)->
							   drv_button[i])->active) {
			drv |= (1 << i);
		}
	}

	return drv;
}
