#ifndef FNAME_H_INCLUDED
#define FNAME_H_INCLUDED

/****************************************************************************
 * 各種ファイル名 (グローバル変数)
 *
 *      起動後、関数 quasi88() を呼び出す前に、オープンしたいファイル名を
 *      設定しておく。設定不要の場合は、空文字列 (または NULL) をセット。
 *      なお、関数 quasi88() の呼び出し以後は、変更しないこと !
 *
 *      char file_disk[2][QUASI88_MAX_FILENAME]
 *              ドライブ 1: / 2: にセットするディスクイメージファイル名。
 *              同じ文字列 (ファイル名) がセットされている場合は、両ドライブ
 *              に、同じディスクイメージファイルをセットすることを意味する。
 *
 *      int image_disk[2]
 *              ディスクイメージがセットされている場合、イメージの番号。
 *              0..31 なら 1..32番目、 -1 ならば、自動的に番号を割り振る。
 *
 *      int readonly_disk[2]
 *              真なら、リードオンリーでディスクイメージファイルを開く。
 *
 *      char *file_compatrom
 *              このファイルを P88SR.exe の ROMイメージファイルとして開く。
 *              NULL の場合は通常通りの ROMイメージファイルを開く。
 *
 *      char file_tape[2][QUASI88_MAX_FILENAME]
 *              テープロード用/セーブ用ファイル名。
 *
 *      char file_prn[QUASI88_MAX_FILENAME]
 *              プリンタ出力用ファイル名。
 *
 *      char file_sin[QUASI88_MAX_FILENAME]
 *      char file_sout[QUASI88_MAX_FILENAME]
 *              シリアル入力用/出力用ファイル名。
 *
 *      char file_state[QUASI88_MAX_FILENAME]
 *              ステートセーブ/ロード用のファイル名。
 *              空文字列の場合は、デフォルトのファイル名が使用される。
 *
 *      char file_snap[QUASI88_MAX_FILENAME]
 *              画面スナップショット保存用のファイルのベース名。
 *              実際にスナップショットを保存する場合は、このファイル名に
 *              4桁の数字 + 拡張子 ( 0000.bmp など ) を連結したファイル名になる
 *              空文字列の場合は、デフォルトのファイル名が使用される。
 *
 *      char file_wav[QUASI88_MAX_FILENAME]
 *              サウンド出力保存用のファイルのベース名。
 *              実際にサウンド出力を保存する場合は、このファイル名に
 *              4桁の数字 + 拡張子 ( 0000.wav ) を連結したファイル名になる
 *              空文字列の場合は、デフォルトのファイル名が使用される。
 *
 *      char *file_rec / char *file_pb
 *              キー入力の記録用/再生用ファイル名。
 *              使用しないなら、NULL にしておく。
 *****************************************************************************/

extern char file_disk[2][QUASI88_MAX_FILENAME];	/* ディスクイメージファイル名*/
extern int  image_disk[2];						/* イメージ番号0..31,-1は自動*/
extern int  readonly_disk[2];					/* リードオンリーで開くなら真*/

extern char file_tape[2][QUASI88_MAX_FILENAME];	/* テープ入出力のファイル名 */
extern char file_prn[QUASI88_MAX_FILENAME];		/* パラレル出力のファイル名 */
extern char file_sin[QUASI88_MAX_FILENAME];		/* シリアル出力のファイル名 */
extern char file_sout[QUASI88_MAX_FILENAME];	/* シリアル入力のファイル名 */

extern char file_state[QUASI88_MAX_FILENAME];	/* ステートファイル名 */
extern char file_snap[QUASI88_MAX_FILENAME];	/* スナップショットベース部 */
extern char file_wav[QUASI88_MAX_FILENAME];		/* サウンド出力ベース部 */

extern char *file_compatrom;			/* P88SR.exeのROMを使うならファイル名*/
extern char *file_rec;					/* キー入力記録するなら、ファイル名 */
extern char *file_pb;					/* キー入力再生するなら、ファイル名 */



void imagefile_all_open(int stateload);
void imagefile_all_close(void);
void imagefile_status(void);


/*----------------------------------------------------------------------
 *
 *----------------------------------------------------------------------*/
const char *filename_get_disk(int drv);
const char *filename_get_tape(int mode);
const char *filename_get_prn(void);
const char *filename_get_sin(void);
const char *filename_get_sout(void);
const char *filename_get_disk_or_dir(int drv);
const char *filename_get_tape_or_dir(int mode);
const char *filename_get_disk_name(int drv);
const char *filename_get_tape_name(int mode);

char *filename_alloc_diskname(const char *filename);
char *filename_alloc_romname(const char *filename);
char *filename_alloc_global_cfgname(void);
char *filename_alloc_local_cfgname(const char *imagename);
char *filename_alloc_keyboard_cfgname(void);
char *filename_alloc_touchkey_cfgname(void);



#endif /* FNAME_H_INCLUDED */
