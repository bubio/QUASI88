#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quasi88.h"
#include "fname.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "pc88cpu.h"
#include "pc88main.h"

#include "drive.h"
#include "image.h"
#include "file-op.h"
#include "suspend.h"
#include "event.h"


#ifdef USE_MONITOR

/*--------------------------------------------------------------
 * drive [show]
 * drive eject [<drive_no>]
 * drive empty <drive_no>
 * drive set <drive_no> <filename >
 *      ドライブ関連処理
 *              ドライブに設定されたファイルの状態を見る
 *              ドライブを空にする
 *              ドライブを一時的に空にする
 *              ドライブにファイルをセット(交換)
 *--------------------------------------------------------------*/
void mon_help_drive(int oneline)
{
	if (oneline) {
		printf(
			"operate disk drive"
			);
	} else {
		printf(
			"  drive\n"
			"  drive show\n"
			"  drive empty [<drive_no>]\n"
			"  drive eject [<drive_no>]\n"
			"  drive set <drive_no> <filename> [<image_no>]\n"
			"    Show drive information, Eject Disk, Set Disk.\n"
			"      drive [show] ... Show now drive information.\n"
			"      drive empty  ... Set/Unset drive <drive_no> empty.\n"
			"      drive eject  ... Eject disk from drive <drive_no>\n"
			"                       <drive_no> omit, eject all disk.\n"
			"      drive set    ... Eject disk and insert new disk\n"
			"        <drive_no> ... 1 | 2  mean  DRIVE 1: | DRIVE 2:\n"
			"        <filename> ... if filename is '-' , disk not change\n"
			"        <image_no> ... image number (1..%d max)\n"
			"                       <image_no> omit, set image number 1.\n"
			, MAX_NR_IMAGE
			);
	}
}

void mon_exec_drive(void)
{
	int i, j, command, drv = -1, img = 0;
	char *filename = NULL;

	if (! mon_exist_argv()) {
		command = ARG_SHOW;
	} else {

		/* <command> */
		if (! mon_argv_is(ARGV_DRIVE)) {
			error();
		}
		command = argv.val;
		mon_shift();

		switch (command) {
		case ARG_SHOW: /* show */
			break;
		case ARG_EJECT: /* eject */
			if (mon_exist_argv()) {
				/* [<drive_no>] */
				if (! mon_argv_is(ARGV_DRV)) {
					error();
				}
				drv = argv.val - 1;
				mon_shift();
			}
			break;
		case ARG_EMPTY: /* empty */
			/* <drive_no> */
			if (! mon_argv_is(ARGV_DRV)) {
				error();
			}
			drv = argv.val - 1;
			mon_shift();
			break;
		case ARG_SET: /* set */
			/* <drive_no> */
			if (! mon_argv_is(ARGV_DRV)) {
				error();
			}
			drv = argv.val - 1;
			mon_shift();
			/* <filename> */
			if (! mon_exist_argv()) {
				error();
			}
			filename = argv.str;
			mon_shift();
			if (mon_exist_argv()) {
				/* [<image_no>] */
				if (! mon_argv_is(ARGV_IMG)) {
					error();
				}
				img = argv.val - 1;
				mon_shift();
			}
			break;
		}

	}
	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	switch (command) {
	case ARG_SHOW:
		/* drive show */
		for (i = 0; i < NR_DRIVE; i++) {
			printf("DRIVE %d: lamp[ %s ]%s\n",
				   i + 1, get_drive_ready(i) ? " " : "#",
				   ((drive[i].fp && drive[i].empty)
					? "  ..... *** Empty ***"
					: ""));
			if (disk_image_exist(i)) {
				/*printf("    filename = %s\n", drive[i].filename);*/
				printf("    filename = %s\n", filename_get_disk(i));
				printf("    FILE *fp = %p : read only? = %s\n",
					   (void *)drive[i].fp,
					   (drive[i].read_only) ? "Read Only" : "Read Write");
				printf("    Selected image No = %d/%d%s : protect = %s : media = %s\n",
					   drive[i].selected_image + 1,
					   drive[i].image_nr,
					   ((drive[i].detect_broken_image)
						? " + broken"
						: ((drive[i].over_image)
						   ? " + alpha "
						   : "         ")),
					   (drive[i].protect == DISK_PROTECT_TRUE) ? "RO" : "RW",
					   ((drive[i].type == DISK_TYPE_2HD)
						? "2HD"
						: ((drive[i].type == DISK_TYPE_2DD)
						   ? "2DD"
						   : "2D")));
				printf("    ------------------------------------------\n");
				for (j = 0; j < drive[i].image_nr; j++) {
					printf("    %s% 3d %-17s  %s  %s  %ld\n",
						   (j == drive[i].selected_image) ? "*" : " ",
						   j + 1,
						   drive[i].image[j].name,
						   ((drive[i].image[j].protect == DISK_PROTECT_TRUE)
							? "RO"
							: ((drive[i].image[j].protect == DISK_PROTECT_FALSE)
							   ? "RW"
							   : "??")),
						   ((drive[i].image[j].type == DISK_TYPE_2D)
							? "2D "
							: ((drive[i].image[j].type == DISK_TYPE_2DD)
							   ? "2DD"
							   : ((drive[i].image[j].type == DISK_TYPE_2HD)
								  ? "2HD"
								  : "???"))),
						   drive[i].image[j].size);
				}
			}
			printf("\n");
		}
		break;
	case ARG_EMPTY: /* drive empty n */
		if (disk_image_exist(drv)) {
			drive_change_empty(drv);
			if (drive_check_empty(drv)) {
				printf("** Set DRIVE %d: Empty **\n", drv + 1);
			} else {
				printf("** Unset DRIVE %d: Empty **\n", drv + 1);
			}
		}
		break;
	case ARG_EJECT:
		switch (drv) {
		case 0: /* drive eject 0 */
		case 1: /* drive eject 1 */
			if (disk_image_exist(drv)) {
				quasi88_disk_eject(drv);
				printf("-- Disk Eject from DRIVE %d: --\n", drv + 1);
			}
			break;
		default: /* drive eject */
			for (i = 0; i < 2; i++) {
				if (disk_image_exist(i)) {
					quasi88_disk_eject(i);
					printf("-- Disk Eject from DRIVE %d: --\n", i + 1);
				}
			}
			break;
		}
		break;
	case ARG_SET:
		/* drive set x yyy z */
		if (strcmp(filename, "-") == 0) {
			switch (disk_change_image(drv, img)) {
			case 0: /* OK */
				printf("== Image change in DRIVE %d: ==\n", drv + 1);
				printf("   image number ->[%d]\n", img + 1);
				break;
			case 1: /* no disk */
				printf("** Disk not exist in DRIVE:%d **\n", drv + 1);
				break;
			case 2: /* no image */
				printf("** Image number %d is not exist in DRIVE:%d **\n",
					   img + 1, drv + 1);
				break;
			}
		} else {
			if (disk_image_exist(drv)) {
				quasi88_disk_eject(drv);
				printf("-- Disk Eject from DRIVE %d: --\n", drv + 1);
			}
			if (quasi88_disk_insert(drv, filename, img, FALSE)) {
				/* Success */
				printf("== Disk insert to DRIVE %d: ==\n", drv + 1);
				printf("   file ->[%s] image number ->[%d]\n",
					   filename, img + 1);
			} else {
				/* Failed */
				printf("** Disk %s can't insert **\n", filename);
			}
		}
		break;
	}


	return;
}



/*--------------------------------------------------------------
 * file show <filename>
 * file create <filename>
 * file protect <filename> <image_no>
 * file unprotect <filename> <image_no>
 * file format <filename> <image_no>
 * file unformat <filename> <image_no>
 * file rename <filename> <image_no> <image_name>
 *      ファイル関連処理
 *              ファイルを見る
 *              ブランクイメージをファイルに追加(作成)
 *              イメージのライトプロテクトを設定
 *              イメージのライトプロテクトを解除
 *              イメージをフォーマット
 *              イメージをアンフォーマット
 *              イメージ名を変更
 *--------------------------------------------------------------*/
void mon_help_file(int oneline)
{
	if (oneline) {
		printf(
			"disk image file utility"
			);
	} else {
		printf(
			"  file show <filename>\n"
			"  file create <filename>\n"
			"  file protect <filename> <image_no>\n"
			"  file unprotect <filename> <image_no>\n"
			"  file format <filename> <image_no>\n"
			"  file unformat <filename> <image_no>\n"
			"  file rename <filename> <image_no> <image_name>\n"
			"    Disk image file utility.\n"
			"      file show      ... Show file information.\n"
			"      file create    ... Create / Append blank disk image in file.\n"
			"      file protect   ... Set protect.\n"
			"      file unprotect ... Unset protect.\n"
			"      file format    ... format image by N88DISK-BASIC DATA DISK format.\n"
			"      file unformat  ... Unformat image.\n"
			"      file rename    ... Rename disk image.\n"
			"        <filename>   ... filename\n"
			"        <image_no>   ... image number (1..%d max)\n"
			"        <image_name> ... image name (MAX 16chars)\n"
			, MAX_NR_IMAGE
			);
	}
}

void mon_exec_file(void)
{
	int command, num, drv, img = 0, result = -1, ro = FALSE;
	char *filename, *imagename = NULL;
	long offset;
	OSD_FILE *fp;
	Uchar c[32];
	char *s = NULL;



	/* <command> */
	if (! mon_argv_is(ARGV_FILE)) {
		error();
	}
	command = argv.val;
	mon_shift();

	/* <filename> */
	if (! mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	switch (command) {
	case ARG_SHOW:   /* show */
	case ARG_CREATE: /* create */
		break;
	case ARG_PROTECT:   /* protect */
	case ARG_UNPROTECT: /* unprotect */
	case ARG_FORMAT:    /* format */
	case ARG_UNFORMAT:  /* unformat */
		/* <image_no> */
		if (! mon_argv_is(ARGV_IMG)) {
			error();
		}
		img = argv.val - 1;
		mon_shift();
		break;
	case ARG_RENAME: /* rename */
		/* <image_no> */
		if (! mon_argv_is(ARGV_IMG)) {
			error();
		}
		img = argv.val - 1;
		mon_shift();
		/* <imagename> */
		if (! mon_exist_argv()) {
			error();
		}
		imagename = argv.str;
		mon_shift();
		break;
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	switch (command) {
	case ARG_SHOW:
		if ((fp = osd_fopen(FTYPE_DISK, filename, "rb")) == NULL) {
			if ((fp = osd_fopen(FTYPE_DISK, filename, "r+b")) == NULL) {
				printf("Open error! %s\n", filename);
				break;
			}
		}

		offset = -1;
		if (osd_fseek(fp, 0,  SEEK_END) == 0) {
			offset = osd_ftell(fp);
		}

		printf("filename = %s   size = %ld\n", filename, offset);
		printf("  -No------Name-----------R/W-Type---Size--\n");
		offset = 0;
		num = 0;
		while ((result = d88_read_header(fp, offset, c)) == 0) {
			c[16] = '\0';
			printf("  % 3d   %-17s  %s  %s  %ld\n",
				   num + 1,
				   c,
				   ((c[DISK_PROTECT] == DISK_PROTECT_TRUE)
					? "RO"
					: ((c[DISK_PROTECT] == DISK_PROTECT_FALSE)
					   ? "RW"
					   : "??")),
				   ((c[DISK_TYPE] == DISK_TYPE_2D)
					? "2D "
					: ((c[DISK_TYPE] == DISK_TYPE_2DD)
					   ? "2DD"
					   : ((c[DISK_TYPE] == DISK_TYPE_2HD)
						  ? "2HD"
						  : "???"))),
				   READ_SIZE_IN_HEADER(c));
			offset += READ_SIZE_IN_HEADER(c);
			num++;
			if (num > 255) {
				result = -1;
				break;
			}
		}
		printf("\n");
		switch (result) {
		case -1:
			printf("Image number too many (over 255)\n");
			break;
		case D88_SUCCESS:
			break;
		case D88_NO_IMAGE:
			break;
		case D88_BAD_IMAGE:
			printf("Image No. %d is broken\n", num + 1);
			break;
		case D88_ERR_SEEK:
			printf("Seek Error\n");
			break;
		case D88_ERR_READ:
			printf("Read Error\n");
			break;
		default:
			printf("Internal Error\n");
		}
		if (drive[0].fp != fp && drive[1].fp != fp) {
			osd_fclose(fp);
		}
		break;


	case ARG_CREATE:
	case ARG_PROTECT:
	case ARG_UNPROTECT:
	case ARG_FORMAT:
	case ARG_UNFORMAT:
	case ARG_RENAME:

		/* ファイルを開く */
		/* "r+b" でオープン */
		fp = osd_fopen(FTYPE_DISK, c, "r+b");
		if (fp == NULL) {
			/* "rb" でオープン */
			fp = osd_fopen(FTYPE_DISK, c, "rb");
			ro = TRUE;
		}

		if (fp) {
			/* オープンできたら すでにドライブに開いてないかをチェックする */
			if (fp == drive[ 0 ].fp) {
				drv = 0;
			} else if (fp == drive[ 1 ].fp) {
				drv = 1;
			} else {
				drv = -1;
			}
		}


		if (fp == NULL) {
			/* オープン失敗 */
			printf("Open error! %s\n", filename);
			break;
		} else if (ro) {
			/* リードオンリーなので処理不可 */
			if (drv < 0) {
				osd_fclose(fp);
			}
			printf("File %s is read only", filename);
			if (drv < 0) {
				printf("\n");
			} else {
				printf("(in DRIVE %d:)\n", drv + 1);
			}
			break;
		} else if (drv >= 0 &&
				   drive[ drv ].detect_broken_image) {
			/* 壊れたイメージが含まれるので不可 */
			printf("Warning! File %s maybe be broken!"
				   " ..... continued, but not update drive status.\n",
				   filename);
		}


		/* コマンド別処理 */
		switch (command) {
		case ARG_CREATE:
			result = d88_append_blank(fp, drv);
			s = "Create blank image";
			break;
		case ARG_PROTECT:
			c[0] = DISK_PROTECT_TRUE;
			result = d88_write_protect(fp, drv, img, (char *)c);
			s = "Set Protect";
			break;
		case ARG_UNPROTECT:
			c[0] = DISK_PROTECT_FALSE;
			result = d88_write_protect(fp, drv, img, (char *)c);
			s = "Unset Protect";
			break;
		case ARG_FORMAT:
			result = d88_write_format(fp, drv, img);
			s = "Format";
			break;
		case ARG_UNFORMAT:
			result = d88_write_unformat(fp, drv, img);
			s = "Unformat";
			break;
		case ARG_RENAME:
			strncpy((char *)c, imagename, 17);
			result = d88_write_name(fp, drv, img, (char *)c);
			s = "Rename image";
			break;
		}
		/* エラー表示 */
		switch (result) {
		case D88_SUCCESS:
			printf("%s complete.\n", s);
			break;
		case D88_NO_IMAGE:
			printf("Image No. %d not exist.\n", img + 1);
			break;
		case D88_BAD_IMAGE:
			printf("Image No. %d is broken.\n", img + 1);
			break;
		case D88_MANY_IMAGE:
			printf("Image number over\n");
			break;
		case D88_ERR_SEEK:
			printf("Seek Error\n");
			break;
		case D88_ERR_WRITE:
			printf("Write error\n");
			break;
		case D88_ERR_READ:
			printf("Read error\n");
			break;
		case D88_ERR:
			printf("Internal error\n");
			break;
		}

		/* 終了処理 */
		if (drv < 0) {
			osd_fclose(fp);
		} else {
			if (result != D88_SUCCESS) {
				printf("Fatal error in File %s ( in DRIVE %d: )\n",
					   filename, drv + 1);
				printf("File %s maybe be broken.\n", filename);
			}
		}
		break;
	}

	return;
}



/*--------------------------------------------------------------
 * tapeload [<filename>]
 *      ロード用テープイメージファイルのセット
 *--------------------------------------------------------------*/
void mon_help_tapeload(int oneline)
{
	if (oneline) {
		printf(
			"set tape-image as load"
			);
	} else {
		printf(
			"  tapeload <filename>\n"
			"    set tape-image-file as load\n"
			"    <filename> ... specify tape-image-file filename.\n"
			"                   filename \"-\" mean \'unset image\'\n"
			);
	}
}

void mon_exec_tapeload(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		const char *filename = filename_get_tape(CLOAD);
		if (filename) {
			printf("  Tape load image is -> %s\n", filename);
		} else {
			printf("  Tape load image is NOT set\n");
		}
		return;
	}

	if (strcmp(filename, "-") == 0) {
		quasi88_load_tape_eject();
		printf("  Tape load image is NOT set\n");
	} else {
		if (quasi88_load_tape_insert(filename)) {
			printf("-- Tape set as load --\n");
			printf("   file ->[%s] \n", filename);
		} else {
			printf("** Tape %s can't set **\n", filename);
		}
	}
	return;
}



/*--------------------------------------------------------------
 * tapesave [<filename>]
 *      セーブ用テープイメージファイルのセット
 *--------------------------------------------------------------*/
void mon_help_tapesave(int oneline)
{
	if (oneline) {
		printf(
			"set tape-image as save"
			);
	} else {
		printf(
			"  tapesave <filename>\n"
			"    set tape-image-file as save\n"
			"    <filename> ... specify tape-image-file filename.\n"
			"                   filename \"-\" mean \'unset image\'\n"
			);
	}
}

void mon_exec_tapesave(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		const char *filename = filename_get_tape(CSAVE);
		if (filename) {
			printf("  Tape save image is -> %s\n", filename);
		} else {
			printf("  Tape save image is NOT set\n");
		}
		return;
	}

	if (strcmp(filename, "-") == 0) {
		quasi88_save_tape_eject();
		printf("  Tape save image is NOT set\n");
	} else {
		if (quasi88_save_tape_insert(filename)) {
			printf("-- Tape set as save --\n");
			printf("   file ->[%s] \n", filename);
		} else {
			printf("** Tape %s can't set **\n", filename);
		}
	}
	return;
}



/*--------------------------------------------------------------
 * printer [<filename>]
 *      プリントアウト用イメージファイルのセット
 *--------------------------------------------------------------*/
void mon_help_printer(int oneline)
{
	if (oneline) {
		printf(
			"set printout-image-file"
			);
	} else {
		printf(
			"  printer <filename>\n"
			"    set printout-image-file\n"
			"    <filename> ... specify printout-image-file filename.\n"
			"                   filename \"-\" mean \'unset image\'\n"
			);
	}
}

void mon_exec_printer(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		const char *filename = filename_get_prn();
		if (filename) {
			printf("  Printout image is -> %s\n", filename);
		} else {
			printf("  Printout image is NOT set\n");
		}
		return;
	}

	if (strcmp(filename, "-") == 0) {
		quasi88_printer_remove();
		printf("  Printout image is NOT set\n");
	} else {
		if (quasi88_printer_connect(filename)) {
			printf("-- Printout image set --\n");
			printf("   file ->[%s] \n", filename);
		} else {
			printf("** Printout image %s can't set **\n", filename);
		}
	}
	return;
}



/*--------------------------------------------------------------
 * serialin [<filename>]
 *      シリアル入力用イメージファイルのセット
 *--------------------------------------------------------------*/
void mon_help_serialin(int oneline)
{
	if (oneline) {
		printf(
			"set serial-in-image-file"
			);
	} else {
		printf(
			"  serialin <filename>\n"
			"    set serial-in-image-file\n"
			"    <filename> ... specify serial-in-image-file filename.\n"
			"                   filename \"-\" mean \'unset image\'\n"
			);
	}
}

void mon_exec_serialin(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {

		char buf[16];
		long cur, end;
		const char *filename = filename_get_sin();
		buf[0] = '\0';
		if (filename) {
			if (sio_com_pos(&cur, &end)) {
				if (end == 0) {
					sprintf(buf, " (END)");
				} else {
					sprintf(buf, " (%3ld%%)", cur * 100 / end);
				}
			}
		}

		if (filename) {
			printf("  Serial-In image is -> %s%s\n", filename, buf);
		} else {
			printf("  Serial-In image is NOT set\n");
		}
		return;
	}

	if (strcmp(filename, "-") == 0) {
		quasi88_serial_in_remove();
		printf("  Serial-In image is NOT set\n");
	} else {
		if (quasi88_serial_in_connect(filename)) {
			printf("-- Serial-In image set --\n");
			printf("   file ->[%s] \n", filename);
		} else {
			printf("** Serial-In image %s can't set **\n", filename);
		}
	}
	return;
}



/*--------------------------------------------------------------
 * serialout <filename>
 *      シリアル出力用イメージファイルのセット
 *--------------------------------------------------------------*/
void mon_help_serialout(int oneline)
{
	if (oneline) {
		printf(
			"set serial-out-image-file"
			);
	} else {
		printf(
			"  serialout <filename>\n"
			"    set serial-out-image-file\n"
			"    <filename> ... specify serial-out-image-file filename.\n"
			"                   filename \"-\" mean \'unset image\'\n"
			);
	}
}

void mon_exec_serialout(void)
{
	char *filename = NULL;

	if (mon_exist_argv()) {
		filename = argv.str;
		mon_shift();
		if (mon_exist_argv()) {
			error();
		}
	} else {
		const char *filename = filename_get_sout();
		if (filename) {
			printf("  Serial-Out image is -> %s\n", filename);
		} else {
			printf("  Serial-Out image is NOT set\n");
		}
		return;
	}

	if (strcmp(filename, "-") == 0) {
		quasi88_serial_out_remove();
		printf("  Serial-Out image is NOT set\n");
	} else {
		if (quasi88_serial_out_connect(filename)) {
			printf("-- Serial-Out image set --\n");
			printf("   file ->[%s] \n", filename);
		} else {
			printf("** Serial-Out image %s can't set **\n", filename);
		}
	}
	return;
}

#endif /* USE_MONITOR */
