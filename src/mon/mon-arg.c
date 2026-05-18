/***************************************************************
 * 引数の種類判定テーブル
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "quasi88.h"
#include "utility.h"
#include "initval.h"
#include "monitor.h"
#include "mon-arg.h"

#include "pc88cpu.h"
#include "pc88main.h"
#include "pc88sub.h"
#include "memory.h"

#ifdef USE_MONITOR

static  struct {
	char        *str;
	int         type;
	int         val;
} monitor_argv[] = {
	/* <cpu> */
	{ "MAIN",			ARGV_CPU,		ARG_MAIN,		},
	{ "SUB",			ARGV_CPU,		ARG_SUB,		},

	/* <bank> */
	{ "MAIN",			ARGV_BANK,		ARG_MAIN,		},
	{ "ROM",			ARGV_BANK,		ARG_ROM,		},
	{ "RAM",			ARGV_BANK,		ARG_RAM,		},
	{ "N",				ARGV_BANK,		ARG_N,			},
	{ "HIGH",			ARGV_BANK,		ARG_HIGH,		},
	{ "EXT0",			ARGV_BANK,		ARG_EXT0,		},
	{ "EXT1",			ARGV_BANK,		ARG_EXT1,		},
	{ "EXT2",			ARGV_BANK,		ARG_EXT2,		},
	{ "EXT3",			ARGV_BANK,		ARG_EXT3,		},
	{ "B",				ARGV_BANK,		ARG_B,			},
	{ "R",				ARGV_BANK,		ARG_R,			},
	{ "G",				ARGV_BANK,		ARG_G,			},
	{ "SUB",			ARGV_BANK,		ARG_SUB,		},
	{ "PCG",			ARGV_BANK,		ARG_PCG,		},

	/* <reg> */
	{ "AF",				ARGV_REG,		ARG_AF,			},
	{ "BC",				ARGV_REG,		ARG_BC,			},
	{ "DE",				ARGV_REG,		ARG_DE,			},
	{ "HL",				ARGV_REG,		ARG_HL,			},
	{ "IX",				ARGV_REG,		ARG_IX,			},
	{ "IY",				ARGV_REG,		ARG_IY,			},
	{ "SP",				ARGV_REG,		ARG_SP,			},
	{ "PC",				ARGV_REG,		ARG_PC,			},
	{ "AF'",			ARGV_REG,		ARG_AF1,		},
	{ "BC'",			ARGV_REG,		ARG_BC1,		},
	{ "DE'",			ARGV_REG,		ARG_DE1,		},
	{ "HL'",			ARGV_REG,		ARG_HL1,		},
	{ "I",				ARGV_REG,		ARG_I,			},
	{ "R",				ARGV_REG,		ARG_R,			},
	{ "IFF",			ARGV_REG,		ARG_IFF,		},
	{ "IM",				ARGV_REG,		ARG_IM,			},
	{ "HALT",			ARGV_REG,		ARG_HALT,		},
	{ "CK",				ARGV_REG,		ARG_CK,			},
	{ "A",				ARGV_REG,		ARG_A,			},
	{ "F",				ARGV_REG,		ARG_F,			},
	{ "B",				ARGV_REG,		ARG_B,			},
	{ "C",				ARGV_REG,		ARG_C,			},
	{ "D",				ARGV_REG,		ARG_D,			},
	{ "E",				ARGV_REG,		ARG_E,			},
	{ "H",				ARGV_REG,		ARG_H,			},
	{ "L",				ARGV_REG,		ARG_L,			},
	{ "A'",				ARGV_REG,		ARG_A1,			},
	{ "F'",				ARGV_REG,		ARG_F1,			},
	{ "B'",				ARGV_REG,		ARG_B1,			},
	{ "C'",				ARGV_REG,		ARG_C1,			},
	{ "D'",				ARGV_REG,		ARG_D1,			},
	{ "E'",				ARGV_REG,		ARG_E1,			},
	{ "H'",				ARGV_REG,		ARG_H1,			},
	{ "L'",				ARGV_REG,		ARG_L1,			},
	{ "IXH",			ARGV_REG,		ARG_IXH,		},
	{ "IXL",			ARGV_REG,		ARG_IXL,		},
	{ "IYH",			ARGV_REG,		ARG_IYH,		},
	{ "IYL",			ARGV_REG,		ARG_IYL,		},

	/* <action> */
	{ "PC",				ARGV_BREAK,		ARG_PC,			},
	{ "P",				ARGV_BREAK,		ARG_P,			},
	{ "READ",			ARGV_BREAK,		ARG_READ,		},
	{ "R",				ARGV_BREAK,		ARG_R,			},
	{ "WRITE",			ARGV_BREAK,		ARG_WRITE,		},
	{ "W",				ARGV_BREAK,		ARG_W,			},
	{ "IN",				ARGV_BREAK,		ARG_IN,			},
	{ "I",				ARGV_BREAK,		ARG_I,			},
	{ "OUT",			ARGV_BREAK,		ARG_OUT,		},
	{ "O",				ARGV_BREAK,		ARG_O,			},
	{ "CLEARALL",		ARGV_BREAK,		ARG_CLEARALL,	},
	{ "CLEAR",			ARGV_BREAK,		ARG_CLEAR,		},
	{ "C",				ARGV_BREAK,		ARG_C,			},
	{ "ENABLE",			ARGV_BREAK,		ARG_ENABLE,		},
	{ "E",				ARGV_BREAK,		ARG_E,			},
	{ "DISABLE",		ARGV_BREAK,		ARG_DISABLE,	},
	{ "D",				ARGV_BREAK,		ARG_D,			},
	{ "TEMPORARY",		ARGV_BREAK,		ARG_TEMPORARY,	},
	{ "T",				ARGV_BREAK,		ARG_T,			},
	{ "LASTING",		ARGV_BREAK,		ARG_LASTING,	},
	{ "L",				ARGV_BREAK,		ARG_L,			},

	/* <action> */
	{ "READ",			ARGV_FBREAK,	ARG_READ,		},
	{ "WRITE",			ARGV_FBREAK,	ARG_WRITE,		},
	{ "DIAG",			ARGV_FBREAK,	ARG_DIAG,		},
	{ "CLEAR",			ARGV_FBREAK,	ARG_CLEAR,		},

	/* <basic-mode> */
	{ "V2",				ARGV_BASMODE,	ARG_V2,			},
	{ "V1H",			ARGV_BASMODE,	ARG_V1H,		},
	{ "V1S",			ARGV_BASMODE,	ARG_V1S,		},
	{ "N",				ARGV_BASMODE,	ARG_N,			},
	/* <clock-mode> */
	{ "8MHZ",			ARGV_CKMODE,	ARG_8MHZ,		},
	{ "4MHZ",			ARGV_CKMODE,	ARG_4MHZ,		},
	/* <sound-board> */
	{ "SD",				ARGV_SDMODE,	ARG_SD,			},
	{ "SD2",			ARGV_SDMODE,	ARG_SD2,		},

	/* trace */
	{ "CHANGE",			ARGV_CHANGE,	ARG_CHANGE,		},
	{ "SET",			ARGV_CHANGE,	ARG_SET,		},
	{ "UNSET",			ARGV_CHANGE,	ARG_UNSET,		},

	/* step */
	{ "CALL",			ARGV_STEP,		ARG_CALL,		},
	{ "JP",				ARGV_STEP,		ARG_JP,			},
	{ "REP",			ARGV_STEP,		ARG_REP,		},
	{ "ALL",			ARGV_STEP,		ARG_ALL,		},

	/* reg */
	{ "ALL",			ARGV_ALL,		ARG_ALL,		},

	/* resize */
	{ "FULL",			ARGV_RESIZE,	ARG_FULL,		},
	{ "HALF",			ARGV_RESIZE,	ARG_HALF,		},
#ifdef SUPPORT_DOUBLE
	{ "DOUBLE",			ARGV_RESIZE,	ARG_DOUBLE,		},
#endif
	{ "FULLSCREEN",		ARGV_RESIZE,	ARG_FULLSCREEN,	},
	{ "WINDOW",			ARGV_RESIZE,	ARG_WINDOW,		},

	/* drive */
	{ "SHOW",			ARGV_DRIVE,		ARG_SHOW,		},
	{ "EJECT",			ARGV_DRIVE,		ARG_EJECT,		},
	{ "EMPTY",			ARGV_DRIVE,		ARG_EMPTY,		},
	{ "SET",			ARGV_DRIVE,		ARG_SET,		},

	/* file */
	{ "SHOW",			ARGV_FILE,		ARG_SHOW,		},
	{ "CREATE",			ARGV_FILE,		ARG_CREATE,		},
	{ "RENAME",			ARGV_FILE,		ARG_RENAME,		},
	{ "PROTECT",		ARGV_FILE,		ARG_PROTECT,	},
	{ "UNPROTECT",		ARGV_FILE,		ARG_UNPROTECT,	},
	{ "FORMAT",			ARGV_FILE,		ARG_FORMAT,		},
	{ "UNFORMAT",		ARGV_FILE,		ARG_UNFORMAT,	},

	/* savebas */
	{ "BINARY",			ARGV_BASIC,		ARG_BINARY,		},
	{ "ASCII",			ARGV_BASIC,		ARG_ASCII,		},

	/* snapshot */
	{ "BMP",			ARGV_SNAPSHOT,	ARG_BMP,		},
	{ "PPM",			ARGV_SNAPSHOT,	ARG_PPM,		},
	{ "RAW",			ARGV_SNAPSHOT,	ARG_RAW,		},
};






/*--------------------------------------------------------------
 * メモリ READ/WRITE 関数
 *--------------------------------------------------------------*/
byte mon_peek_memory(int bank, word addr)
{
	int  verbose_save;
	byte wk;

	switch (bank) {
	case ARG_MAIN:
		return main_mem_read(addr);

	case ARG_ROM:
		if (addr < 0x8000) {
			return main_rom[addr];
		} else {
			return main_mem_read(addr);
		}

	case ARG_RAM:
		if (0xf000 <= addr && high_mode) {
			return main_high_ram[addr - 0xf000];
		} else {
			return main_ram[addr];
		}

	case ARG_N:
		if (addr < 0x8000) {
			return main_rom_n[addr];
		} else {
			return main_mem_read(addr);
		}

	case ARG_EXT0:
		if (0x6000 <= addr && addr < 0x8000) {
			return main_rom_ext[0][addr - 0x6000];
		} else {
			return main_mem_read(addr);
		}

	case ARG_EXT1:
		if (0x6000 <= addr && addr < 0x8000) {
			return main_rom_ext[1][addr - 0x6000];
		} else {
			return main_mem_read(addr);
		}

	case ARG_EXT2:
		if (0x6000 <= addr && addr < 0x8000) {
			return main_rom_ext[2][addr - 0x6000];
		} else {
			return main_mem_read(addr);
		}

	case ARG_EXT3:
		if (0x6000 <= addr && addr < 0x8000) {
			return main_rom_ext[3][addr - 0x6000];
		} else {
			return main_mem_read(addr);
		}

	case ARG_B:
		if (0xc000 <= addr) {
			return main_vram[addr - 0xc000][0];
		} else {
			return main_mem_read(addr);
		}

	case ARG_R:
		if (0xc000 <= addr) {
			return main_vram[addr - 0xc000][1];
		} else {
			return main_mem_read(addr);
		}

	case ARG_G:
		if (0xc000 <= addr) {
			return main_vram[addr - 0xc000][2];
		} else {
			return main_mem_read(addr);
		}

	case ARG_HIGH:
		if (0xf000 <= addr) {
			if (high_mode) {
				return main_ram[addr];
			} else {
				return main_high_ram[addr - 0xf000];
			}
		} else {
			return main_mem_read(addr);
		}

	case ARG_SUB:
		verbose_save = verbose_io;
		verbose_io = 0;
		wk = sub_mem_read(addr);
		verbose_io = verbose_save;
		return wk;

	case ARG_PCG:
		if (addr < 8 * 256 * 2) {
			return font_pcg[addr];
		} else {
			return 0xff;
		}
	}
	return 0xff;
}
void mon_poke_memory(int bank, word addr, byte data)
{
	int verbose_save;

	switch (bank) {
	case ARG_MAIN:
		main_mem_write(addr, data);
		return;

	case ARG_ROM:
		if (addr < 0x8000) {
			main_rom[addr] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_RAM:
		if (0xf000 <= addr && high_mode) {
			main_high_ram[addr - 0xf000] = data;
		} else {
			main_ram[addr] = data;
		}
		return;

	case ARG_N:
		if (addr < 0x8000) {
			main_rom_n[addr] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_EXT0:
		if (0x6000 <= addr && addr < 0x8000) {
			main_rom_ext[0][addr - 0x6000] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_EXT1:
		if (0x6000 <= addr && addr < 0x8000) {
			main_rom_ext[1][addr - 0x6000] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_EXT2:
		if (0x6000 <= addr && addr < 0x8000) {
			main_rom_ext[2][addr - 0x6000] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_EXT3:
		if (0x6000 <= addr && addr < 0x8000) {
			main_rom_ext[3][addr - 0x6000] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_B:
		if (0xc000 <= addr) {
			main_vram[addr - 0xc000][0] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_R:
		if (0xc000 <= addr) {
			main_vram[addr - 0xc000][1] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_G:
		if (0xc000 <= addr) {
			main_vram[addr - 0xc000][2] = data;
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_HIGH:
		if (0xf000 <= addr) {
			if (high_mode) {
				main_ram[addr] = data;
			} else {
				main_high_ram[addr - 0xf000] = data;
			}
		} else {
			main_mem_write(addr, data);
		}
		return;

	case ARG_SUB:
		verbose_save = verbose_io;
		verbose_io = 0;
		sub_mem_write(addr, data);
		verbose_io = verbose_save;
		return;

	case ARG_PCG:
		if (addr < 8 * 256 * 2) {
			font_pcg[addr] = data;
		}
		return;
	}
}








/*==============================================================
 * 引数処理
 *==============================================================*/

/*
 * buf[] の文字列から、単語を取り出す。区切りは、SPC と TAB。
 * 取り出した各単語の先頭アドレスが、*d_argv[] に格納される。
 * 単語は最大 MAX_ARGS 個取り出す。単語の数は、d_argc にセット。
 * 単語の数が MAX_ARGS よりも多い時は、d_argc に MAX_ARGS+1 をセット、
 * この時、MAX_ARGS 個までは、*d_argv[] が格納されている。
 */

char d_buf[MON_MAX_CHRS];
int  d_argc;
char *d_argv[MON_MAX_ARGS];

static int argv_counter;

void mon_getarg(void)
{
	char *p = &d_buf[0];

	argv_counter = 1;

	d_argc = 0;
	for (;;) {

		for (;;) {
			if (*p == '\n' || *p == '\0') {
				return;
			} else if (*p == ' '  || *p == '\t') {
				p++;
			} else {
				if (d_argc == MON_MAX_ARGS) {
					d_argc++;
					return;
				} else {
					d_argv[ d_argc++ ] = p;
					break;
				}
			}
		}
		for (;;) {
			if (*p == '\n' || *p == '\0') {
				*p   = '\0';
				return;
			} else if (*p == ' '  || *p == '\t') {
				*p++ = '\0';
				break;
			} else {
				p++;
			}
		}

	}

	return;
}


/*
 * getarg() により、main()の引数と同じような形式で、int d_argc, char *d_argv[]
 * が設定されるが、これをもう少し簡単に処理したいので、shift() 関数を用意した。
 *
 * shift() 関数を呼ぶと、一番最初の引数が解析され、その結果が argv ワークに
 * 格納される。この後で、argv.type をチェックすれば、その引数の種類が、
 * argv.val をチェックすれば、その引数の値がわかる。
 *
 * shift() 関数により、引数が見かけ上一つずつ前にずれていく。
 * ゆえに、shift() 関数を連続して呼べば、常に次の引数が解析される。
 *
 *   shift();
 *   if (argv.type == XXX) { 処理() };
 *   shift();
 *   if (argv.type == YYY) { 処理() };
 *   ...
 */

t_mon_argv argv;


void mon_shift(void)
{
	int i, size = FALSE;
	char *p, *chk;


	if (argv_counter > MON_MAX_ARGS ||
		argv_counter >= d_argc) {
		/* これ以上引数が無い */

		argv.type = ARGV_END;

	} else {
		/* まだ引数があるので解析 */

		p = d_argv[ argv_counter ];
		if (*p == '#') {
			size = TRUE;
			p++;
		}

		argv.type = 0;
		argv.val  = strtol(p, &chk, 0);
		argv.str  = d_argv[ argv_counter ];

		if (p != chk && *chk == '\0') {
			/* 数値の場合 */

			if (size) {
				/* #で始まる */
				if (argv.val <= 0) {
					argv.type = ARGV_STR;
				} else {
					argv.type = ARGV_SIZE;
				}
			} else {
				/*数で始まる */
				argv.type |= ARGV_INT;
				if (argv.val >= 0) {
					argv.type |= ARGV_NUM;
				}
				if (argv.val <= 0xff) {
					argv.type |= ARGV_PORT;
				}
				if (argv.val <= 0xffff) {
					argv.type |= ARGV_ADDR;
				}
				if (BETWEEN(1, argv.val, NR_DRIVE)) {
					argv.type |= ARGV_DRV;
				}
				if (BETWEEN(1, argv.val, MAX_NR_IMAGE)) {
					argv.type |= ARGV_IMG;
				}
			}

		} else {
			/* 文字列の場合 */

			if (size) {
				/* #で始まる */
				argv.type = ARGV_STR;
			} else {
				/*字で始まる */
				for (i = 0; i < COUNTOF(monitor_argv); i++) {
					if (my_strcmp(p, monitor_argv[i].str) == 0) {
						argv.type |= monitor_argv[i].type;
						argv.val   = monitor_argv[i].val;
					}
				}
				if (argv.type == 0) {
					argv.type = ARGV_STR;
				}
			}

		}

		argv_counter ++;

	}
}


/* shift() した結果、引数が設定されたかどうかをチェック */

int mon_exist_argv(void)
{
	return argv.type ? TRUE : FALSE;
}


/* shift() した結果、処理された引数の種類をチェック */

int mon_argv_is(int type)
{
	return (argv.type & (type));
}


/* 引数の値 (ARG_xxx) から、引数の文字列 (大文字) を得る */

char *mon_argv2str(int argv_val)
{
	int i;

	for (i = 0; i < COUNTOF(monitor_argv); i++) {
		if (argv_val == monitor_argv[i].val) {
			return monitor_argv[i].str;
		}
	}
	return "";
}




/*==============================================================
 * エラー関連
 *==============================================================*/
void mon_error(void)
{
	printf("Invalid argument (arg %d)\n",argv_counter);
}

#endif /* USE_MONITOR */
