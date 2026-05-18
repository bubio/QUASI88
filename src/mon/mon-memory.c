#include <stdio.h>
#include <ctype.h>

#include "quasi88.h"
#include "utility.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "pc88cpu.h"
#include "pc88main.h"
#include "pc88sub.h"
#include "crtcdmac.h"
#include "memory.h"
#include "graph.h"


#ifdef USE_MONITOR

/*--------------------------------------------------------------
 * read [<bank>] <addr>
 *      特定のアドレスをリード
 *--------------------------------------------------------------*/
void mon_help_read(int oneline)
{
	if (oneline) {
		printf(
			"read memory"
			);
	} else {
		printf(
			"  read [<bank>] <addr>\n"
			"    read memory.\n"
			"    <bank> ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"               [omit]... current memory bank of MAIN.\n"
			"    <addr> ... specify address\n");
	}
}

void mon_exec_read(void)
{
	int i, j;
	int addr;
	byte data;
	int bank = ARG_MAIN;



	if (!mon_exist_argv()) {
		error();
	}

	/* [<bank>] */
	if (mon_argv_is(ARGV_BANK)) {
		bank = argv.val;
		mon_shift();
	}

	/* <addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	addr = argv.val;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	data = mon_peek_memory(bank, addr);

	printf("READ memory %s[ %04XH ] -> %02X  (= %d | %+d | ", mon_argv2str(bank),
		   addr, (Uint)data, (Uint)data, (int)((char)data));
	for (i = 0, j = 0x80; i < 8; i++, j >>= 1) {
		printf("%d", (data & j) ? 1 : 0);
	}
	printf("B )\n");

	return;
}



/*--------------------------------------------------------------
 * write [<bank>] <addr> <data>
 *      特定のアドレスにライト
 *--------------------------------------------------------------*/
void mon_help_write(int oneline)
{
	if (oneline) {
		printf(
			"write memory"
			);
	} else {
		printf(
			"  write [<bank>] <addr> <data>\n"
			"    write memory.\n"
			"    <bank> ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"               [omit]... current memory bank of MAIN.\n"
			"    <addr> ... specify address\n"
			"    <data> ... write data\n"
			);
	}
}

void mon_exec_write(void)
{
	int i, j;
	int addr;
	byte data;
	int bank = ARG_MAIN;


	if (!mon_exist_argv()) {
		error();
	}

	/* [<bank>] */
	if (mon_argv_is(ARGV_BANK)) {
		bank = argv.val;
		mon_shift();
	}

	/* <addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	addr = argv.val;
	mon_shift();

	/* <data> */
	if (!mon_argv_is(ARGV_INT)) {
		error();
	}
	data = argv.val;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	mon_poke_memory(bank, addr, data);

	printf("WRITE memory %s[ %04XH ] <- %02X  (= %d | %+d | ", mon_argv2str(bank),
		   addr, (Uint)data, (Uint)data, (int)((char)data));
	for (i = 0, j = 0x80; i < 8; i++, j >>= 1) {
		printf("%d", (data & j) ? 1 : 0);
	}
	printf("B )\n");

	return;
}



/*--------------------------------------------------------------
 * dump [<bank>] <start-addr> [<end-addr>]
 * dump [<bank>] <start-addr> [#<size>]
 *      メモリダンプを表示する
 *--------------------------------------------------------------*/
void mon_help_dump(int oneline)
{
	if (oneline) {
		printf(
			"dump memory"
			);
	} else {
		printf(
			"  dump [<bank>] <start-addr> [<end-addr>]\n"
			"  dump [<bank>] <start-addr> [#<size>]\n"
			"    dump memory.\n"
			"    <bank>       ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"                     [omit]... current memory bank of MAIN.\n"
			"    <start-addr> ... dump start address\n"
			"    <end-addr>   ... dump end address\n"
			"                     [omit]... <start-address>+256\n"
			"    #<size>      ... dump size\n"
			"                     [omit]... 256 byte\n"
			);
	}
}

void mon_init_dump(void)
{
	mon.dump.addr = -1;
	mon.dump.bank = ARG_MAIN;
}

void mon_exec_dump(void)
{
	int i, j;
	byte c;
	int bank  = mon.dump.bank;
	int start = mon.dump.addr;
	int size  = 256;


	if (!mon_exist_argv()) {
		if (mon.dump.addr == -1) {
			error();
		}
		/* else skip */
	} else {

		/* [<bank>] */
		if (mon_argv_is(ARGV_BANK)) {
			bank = argv.val;
			mon_shift();
		}

		/* <addr> */
		if (!mon_argv_is(ARGV_ADDR)) {
			error();
		}
		start = argv.val;
		mon_shift();

		/* [<addr|#size>] */
		if (!mon_exist_argv()) {
			/* skip */;
		} else if (mon_argv_is(ARGV_SIZE)) {
			size = argv.val;
		} else if (mon_argv_is(ARGV_ADDR)) {
			size = argv.val - start + 1;
		} else {
			error();
		}
		mon_shift();
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	/* 毎回ダンプしたアドレスを覚えておく (連続ダンプ用) */
	mon.dump.addr = start + size;
	mon.dump.bank = bank;

	size = (size + 15) / 16;

	printf("addr : +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F\n");
	printf("---- : -----------------------------------------------\n");
	for (i = 0; i < size; i++) {
		printf("%04X : ", (start + i * 16) & 0xffff);
		for (j = 0; j < 16; j++) {
			printf("%02X ", mon_peek_memory(bank, start + i * 16 + j));
		}
		printf("|");
		for (j = 0; j < 16; j++) {
			c = mon_peek_memory(bank, start + i * 16 + j);
			if (!isprint(c)) {
				c = '.';
			}
			printf("%c", c);
		}
		printf("|\n");
	}
	printf("\n");

	return;
}

/*----------------------------------------------------------------------
 * dumpext [<bank>] [#<board>] <start-addr> [<end-addr>]
 * dumpext [<bank>] [#<board>] <start-addr> [#<size>]
 *      拡張RAMのメモリダンプを表示する
 *                              この機能は peach氏により実装されました
 *----------------------------------------------------------------------*/
void mon_help_dumpext(int oneline)
{
	if (oneline) {
		printf(
			"dump external ram memory"
			);
	} else {
		printf(
			"  dumpext [<bank>] [#<board>] <start-addr> [<end-addr>]\n"
			"  dumpext [<bank>] [#<board>] <start-addr> [#<size>]\n"
			"    dump external ram memory.\n"
			"    <bank>       ... memory bank EXT0|EXT1|EXT2|EXT3\n"
			"                     [omit]... current memory bank of EXT0.\n"
			"    #<board>     ... board number (1..16).\n"
			"                     [omit]... board #1.\n"
			"    <start-addr> ... dump start address(0x0000..0x7fff)\n"
			"    <end-addr>   ... dump end address(0x0000..0x7fff)\n"
			"                     [omit]... <start-address>+256\n"
			"    #<size>      ... dump size\n"
			"                     [omit]... 256 byte\n"
			);
	}
}

void mon_init_dumpext(void)
{
	mon.dumpext.addr = -1;
	mon.dumpext.bank = ARG_EXT0;
	mon.dumpext.board = 1;
}

void mon_exec_dumpext(void)
{
	int i, j;
	byte c;
	int bank  = mon.dumpext.bank;
	int start = mon.dumpext.addr;
	int board = mon.dumpext.board;
	int size  = 256;


	if (!mon_exist_argv()) {
		if (mon.dumpext.addr == -1) {
			error();
		}
		/* else skip */
	} else {

		/* [<bank>] */
		if (mon_argv_is(ARGV_BANK)) {
			bank = argv.val;
			if (bank < ARG_EXT0 || ARG_EXT3 < bank) {
				error();
			}
			mon_shift();
		}

		/* [#<board>] */
		if (mon_argv_is(ARGV_SIZE)) {
			board = argv.val;
			if (board < 1 || use_extram < board) {
				error();
			}
			mon_shift();
		}

		/* <addr> */
		if (!mon_argv_is(ARGV_ADDR)) {
			error();
		}
		start = argv.val;
		if (start >= 0x8000) {
			error();
		}
		mon_shift();

		/* [<addr|#size>] */
		if (!mon_exist_argv()) {
			/* skip */;
		} else if (mon_argv_is(ARGV_SIZE)) {
			size = argv.val;
		} else if (mon_argv_is(ARGV_ADDR)) {
			size = argv.val - start + 1;
		} else {
			error();
		}
		if (start + size >= 0x8000) {
			error();
		}
		mon_shift();
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	/* 毎回ダンプしたアドレスを覚えておく (連続ダンプ用) */
	mon.dumpext.addr  = start + size;
	mon.dumpext.bank  = bank;
	mon.dumpext.board = board;

	size = (size + 15) / 16;
	bank = bank - ARG_EXT0 + (board - 1) * 4;

	printf("addr : +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F\n");
	printf("---- : -----------------------------------------------\n");
	for (i = 0; i < size; i++) {
		printf("%04X : ", (start + i * 16) & 0xffff);
		for (j = 0; j < 16; j++) {
			printf("%02X ", ext_ram[bank][start + i * 16 + j]) ;
		}
		printf("|");
		for (j = 0; j < 16; j++) {
			c = ext_ram[bank][start + i * 16 + j];
			if (!isprint(c)) {
				c = '.';
			}
			printf("%c", c);
		}
		printf("|\n");
	}
	printf("\n");

	return;
}



/*--------------------------------------------------------------
 * fill [<bank>] <start-addr> <end-addr> <value>
 * fill [<bank>] <start-addr> #<size>    <value>
 *      メモリを埋める
 *--------------------------------------------------------------*/
void mon_help_fill(int oneline)
{
	if (oneline) {
		printf(
			"fill memory"
			);
	} else {
		printf(
			"  fill [<bank>] <start-addr> <end-addr> <value>\n"
			"  fill [<bank>] <start-addr> #<size>    <value>\n"
			"    fill memory by specify value. \n"
			"    <bank>       ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"                     [omit]... current memory bank of MAIN.\n"
			"    <start-addr> ... fill start address\n"
			"    <end-addr>   ... fill end address\n"
			"    #<size>      ... fill size\n"
			"    <value>      ... fill value\n"
			);
	}
}

void mon_exec_fill(void)
{
	int i;
	int bank = ARG_MAIN;
	int start, size, value;


	if (!mon_exist_argv()) {
		error();
	}

	/* [<bank>] */
	if (mon_argv_is(ARGV_BANK)) {
		bank = argv.val;
		mon_shift();
	}

	/* <addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	start = argv.val;
	mon_shift();

	/* [<addr|#size>] */
	if (mon_argv_is(ARGV_SIZE)) {
		size = argv.val;
	} else if (mon_argv_is(ARGV_ADDR)) {
		size = argv.val - start + 1;
	} else {
		error();
	}
	mon_shift();

	/* <data> */
	if (!mon_argv_is(ARGV_INT)) {
		error();
	}
	value = argv.val;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	for (i = 0; i < size; i++) {
		mon_poke_memory(bank, start + i, value);
	}

	return;
}



/*--------------------------------------------------------------
 * move [<bank>] <src-addr> <end-addr> [<bank>] <dist-addr>
 * move [<bank>] <src-addr> #size      [<bank>] <dist-addr>
 *      メモリ転送
 *--------------------------------------------------------------*/
void mon_help_move(int oneline)
{
	if (oneline) {
		printf(
			"move memory"
			);
	} else {
		printf(
			"  move [<src-bank>] <src-addr> <end-addr> [<dist-bank>] <dist-addr>\n"
			"  move [<src-bank>] <src-addr> #<size>    [<dist-bank>] <dist-addr>\n"
			"    move memory. \n"
			"    <src-bank>  ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"                    [omit]... current memory bank of MAIN.\n"
			"    <src-addr>  ... move source start address\n"
			"    <end-addr>  ... move source end   address\n"
			"    #<size>     ... move size\n"
			"    <dist-bank> ... memory bank\n"
			"                    [omit]... same as <src-bank>\n"
			"    <dist-addr> ... move distination address\n"
			);
	}
}

void mon_exec_move(void)
{
	int i;
	int s_bank  = ARG_MAIN;
	int d_bank  = ARG_MAIN;
	int start, size, dist;
	byte data;


	if (!mon_exist_argv()) {
		error();
	}

	/* [<bank>] */
	if (mon_argv_is(ARGV_BANK)) {
		s_bank = argv.val;
		d_bank = s_bank;
		mon_shift();
	}

	/* <addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	start = argv.val;
	mon_shift();

	/* [<addr|#size>] */
	if (mon_argv_is(ARGV_SIZE)) {
		size = argv.val;
	} else if (mon_argv_is(ARGV_ADDR)) {
		size = argv.val - start + 1;
	} else {
		error();
	}
	mon_shift();

	/* [<bank>] */
	if (mon_argv_is(ARGV_BANK)) {
		d_bank = argv.val;
		mon_shift();
	}

	/* <addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	dist = argv.val;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (start + size <= dist) {
		/* 転送元-転送先が 重ならない */
		for (i = 0; i < size; i++) {
			data = mon_peek_memory(s_bank, start + i);
			mon_poke_memory(d_bank, dist + i, data);
		}
	} else {
		/* 転送元-転送先が 重なる */
		for (i = size - 1; i >= 0; i--) {
			data = mon_peek_memory(s_bank, start + i);
			mon_poke_memory(d_bank, dist + i, data);
		}
	}

	return;
}



/*--------------------------------------------------------------
 * search [<value> [[<bank>] <start-addr> <end-addr>]]
 *      特定の定数 (1バイト) をサーチ
 *--------------------------------------------------------------*/
void mon_help_search(int oneline)
{
	if (oneline) {
		printf(
			"search memory"
			);
	} else {
		printf(
			"  search [<value> [[<bank>] <start-addr> <end-addr>]]\n"
			"    search memory. \n"
			"    <value>      ... search value\n"
			"    <bank>       ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"                     [omit]... current memory bank of MAIN.\n"
			"    <start-addr> ... search start address\n"
			"    <end-addr>   ... search end address\n"
			"    [omit-all]   ... search previous value\n"
			);
	}
}

void mon_exec_search(void)
{
	int i, j;

	if (!mon_exist_argv()) {
		if (mon.search.addr == -1) {
			error();
		}
		/* else skip */
	} else {

		/* <value> */
		if (!mon_argv_is(ARGV_INT)) {
			error();
		}
		mon.search.data = argv.val;
		mon_shift();

		if (!mon_exist_argv()) {
			if (mon.search.addr == -1) {
				error();
			}
			/* else skip */
		} else {

			/* [<bank>] */
			if (mon_argv_is(ARGV_BANK)) {
				mon.search.bank = argv.val;
				mon_shift();
			}

			/* <addr> */
			if (!mon_argv_is(ARGV_ADDR)) {
				error();
			}
			mon.search.addr = argv.val;
			mon_shift();

			/* <end-addr|#size>*/
			if (mon_argv_is(ARGV_SIZE)) {
				mon.search.size = argv.val;
			} else if (mon_argv_is(ARGV_ADDR)) {
				mon.search.size = argv.val - mon.search.addr + 1;
			} else {
				error();
			}
			mon_shift();
		}

	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	/* 一致したアドレスを列挙 */
	j = 0;
	for (i = 0; i < mon.search.size; i++) {
		if (mon_peek_memory(mon.search.bank, mon.search.addr + i)
			== mon.search.data) {
			printf("[%04X] ", mon.search.addr + i);
			if (++j == 11) {
				printf("\n");
				j = 0;
			}
		}
	}
	if (j != 0) {
		printf("\n");
	}

	return;
}



/*--------------------------------------------------------------
 * in [<cpu>] <port>
 *      特定のポートから入力
 *--------------------------------------------------------------*/
void mon_help_in(int oneline)
{
	if (oneline) {
		printf(
			"input port"
			);
	} else {
		printf(
			"  in [<cpu>] <port>\n"
			"    input I/O port.\n"
			"    <cpu>  ... CPU select MAIN|SUB\n"
			"               [omit]... select MAIN\n"
			"    <port> ... in port address\n"
			);
	}
}

void mon_exec_in(void)
{
	int i, j;
	int cpu = ARG_MAIN, port;
	byte data;


	if (!mon_exist_argv()) {
		error();
	}

	/* [<cpu>] */
	if (mon_argv_is(ARGV_CPU)) {
		cpu = argv.val;
		mon_shift();
	}

	/* <port> */
	if (!mon_argv_is(ARGV_PORT)) {
		error();
	}
	port = argv.val;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu == ARG_MAIN) {
		data = main_io_in(port) & 0xff;
	} else {
		data = sub_io_in(port) & 0xff;
	}

	printf("IN port %s[ %02X ] -> %02X (", mon_argv2str(cpu), port, data);
	for (i = 0, j = 0x80; i < 8; i++, j >>= 1) {
		printf("%d", (data & j) ? 1 : 0);
	}
	printf(")\n");

	return;
}



/*--------------------------------------------------------------
 * out [<cpu>] <port> <data>
 *      特定のポートに出力
 *--------------------------------------------------------------*/
void mon_help_out(int oneline)
{
	if (oneline) {
		printf(
			"output port"
			);
	} else {
		printf(
			"  out [<cpu>] <port> <data>\n"
			"    output I/O port.\n"
			"    <cpu>  ... CPU select MAIN|SUB\n"
			"               [omit]... select MAIN\n"
			"    <port> ... out port address\n"
			"    <data> ... output data\n"
			);
	}
}

void mon_exec_out(void)
{
	int i, j;
	int cpu = ARG_MAIN, port;
	byte data;


	if (!mon_exist_argv()) {
		error();
	}

	/* [<cpu>] */
	if (mon_argv_is(ARGV_CPU)) {
		cpu = argv.val;
		mon_shift();
	}

	/* <port> */
	if (!mon_argv_is(ARGV_PORT)) {
		error();
	}
	port = argv.val;
	mon_shift();

	/* <data> */
	if (!mon_argv_is(ARGV_INT)) {
		error();
	}
	data = argv.val & 0xff;
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu == ARG_MAIN) {
		main_io_out(port, data);
	} else {
		sub_io_out(port, data);
	}

	printf("OUT port %s[ %02X ] <- %02X (", mon_argv2str(cpu), port, data);
	for (i = 0, j = 0x80; i < 8; i++, j >>= 1) {
		printf("%d", (data & j) ? 1 : 0);
	}
	printf(")\n");

	return;
}


/*--------------------------------------------------------------
 * loadmem <filename> <bank> <start-addr> [<end-addr>]
 * loadmem <filename> <bank> <start-addr> [#<size>]
 *      ファイルからメモリにロード
 *--------------------------------------------------------------*/
void mon_help_loadmem(int oneline)
{
	if (oneline) {
		printf(
			"load memory from file"
			);
	} else {
		printf(
			"  loadmem <filename> <bank> <start-addr> [<end-addr>]\n"
			"  loadmem <filename> <bank> <start-addr> [#<size>]\n"
			"    load memory from binary file.\n"
			"    <filename>   ... binary filename.\n"
			"    <bank>       ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"    <start-addr> ... load start addr\n"
			"    <end-addr>   ... load end addr\n"
			"    #<size>      ... load size\n"
			"                     [omit] set filesize as binary size\n"
			);
	}
}

void mon_exec_loadmem(void)
{
	int addr, size, bank;
	char *filename;
	FILE *fp;
	int c, i;

	/* <filename> */
	if (!mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	/* <bank> */
	if (!mon_argv_is(ARGV_BANK)) {
		error();
	}
	bank = argv.val;
	mon_shift();

	/* <start-addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	addr = argv.val;
	mon_shift();

	/* #<size>|<end-addr>*/
	if (mon_argv_is(ARGV_SIZE)) {
		size = argv.val;
		mon_shift();
	} else if (mon_argv_is(ARGV_ADDR)) {
		size = argv.val - addr + 1;
		mon_shift();
	} else {
		size = -1;
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if ((fp = fopen(filename, "rb"))) {
		if (size < 0) {
#if 0
			struct stat filestats;

			if (fstat(fileno(fp), &filestats)) {
				size = 0;
			} else {
				size = filestats.st_size;
			}
#else
			if (fseek(fp, 0, SEEK_END) == 0) {
				size = ftell(fp);
				if (size < 0) {
					size = 0;
				}
			}
			fseek(fp, 0, SEEK_SET);
#endif
		}
		for (i = 0; i < size; i++) {
			if ((c = getc(fp)) ==  EOF) {
				printf("Warning : loadmem : file size too short (<%d)\n", size);
				break;
			}
			mon_poke_memory(bank, addr + i, c);
		}
		fclose(fp);
		printf("Load [%s] -> %s (size %d )\n", filename, mon_argv2str(bank), i);
	} else {
		printf("file [%s] can't open\n", filename);
	}
}



/*--------------------------------------------------------------
 * savemem <filename> <bank> <start-addr> <end-addr>
 * savemem <filename> <bank> <start-addr> #<size>
 *      メモリをファイルにセーブ
 *--------------------------------------------------------------*/
void mon_help_savemem(int oneline)
{
	if (oneline) {
		printf(
			"save memory to file"
			);
	} else {
		printf(
			"  savemem <filename> <bank> <start-addr> <end-addr>\n"
			"  savemem <filename> <bank> <start-addr> #<size>\n"
			"    save memory image to file.\n"
			"    <filename>   ... filename.\n"
			"    <bank>       ... memory bank ROM|RAM|N|EXT0|EXT1|EXT2|EXT3|B|R|G|HIGH|SUB\n"
			"    <start-addr> ... save start addr\n"
			"    <end-addr>   ... save end addr\n"
			"    #<size>      ... save size\n"
			);
	}
}

void mon_exec_savemem(void)
{
	int addr, size, bank;
	char *filename;
	FILE *fp;
	int c, i;

	/* <filename> */
	if (!mon_exist_argv()) {
		error();
	}
	filename = argv.str;
	mon_shift();

	/* <bank> */
	if (!mon_argv_is(ARGV_BANK)) {
		error();
	}
	bank = argv.val;
	mon_shift();

	/* <start-addr> */
	if (!mon_argv_is(ARGV_ADDR)) {
		error();
	}
	addr = argv.val;
	mon_shift();

	/* #<size>|<end-addr>*/
	if (mon_argv_is(ARGV_SIZE)) {
		size = argv.val;
	} else if (mon_argv_is(ARGV_ADDR)) {
		size = argv.val - addr + 1;
	} else {
		error();
	}
	mon_shift();

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if ((fp = fopen(filename, "wb"))) {
		for (i = 0; i < size; i++) {
			c = mon_peek_memory(bank, addr + i);
			if (putc(c, fp) == EOF) {
				printf("Warning : savemem : file write failed\n");
				break;
			}
		}
		fclose(fp);
		printf("Save [%s] -> %s (size %d )\n", filename, mon_argv2str(bank), i);
	} else {
		printf("file [%s] can't open\n", filename);
	}
}



/*----------------------------------------------------------------------
 * text [INFO|TEXT|ATTR]
 *      テキスト情報の表示
 *----------------------------------------------------------------------*/
void mon_help_text(int oneline)
{
	if (oneline) {
		printf(
			"print text information"
			);
	} else {
		printf(
			"  text [<type>]\n"
			"    print text information\n"
			"    <type> ... specity infomation type.\n"
			"               INFO ... crtc/dmac setting\n"
			"               TEXT ... text data dump\n"
			"               ATTR ... attribute data dump\n"
			"               CRT  ... crt output image\n"
			"          [default] ... internal text image\n"
			);
	}
}

void mon_exec_text(void)
{
	enum { INFO = 1, TEXT = 2, ATTR = 4, CRT = 8, COOKED = 16 } type = (COOKED);
	int i, j;
	int next_chara = (sys_ctrl & 0x01) ? +1 : +2;
	Ushort start = dmac_address[2].W;

	if (mon_exist_argv()) {
		if (my_strcmp(argv.str, "INFO") == 0) {
			type = INFO;
		} else if (my_strcmp(argv.str, "TEXT") == 0) {
			type = TEXT;
		} else if (my_strcmp(argv.str, "ATTR") == 0) {
			type = ATTR;
		} else if (my_strcmp(argv.str, "CRT") == 0) {
			type = CRT;
		} else if (my_strcmp(argv.str, "COOKED") == 0) {
			type = COOKED;
		} else if (my_strcmp(argv.str, "ALL") == 0) {
			type = (INFO | TEXT | ATTR | COOKED);
		} else {
			error();
		}
		mon_shift();
	}

	if (type & INFO) {
		printf("I/O : width = %d, height = %d\n",
			   (sys_ctrl & 0x01) ? 80 : 40, (grph_ctrl & 0x20) ? 25 : 20);
		printf("CRTC: width = %d, height = %d\n",
			   (crtc_format[0] & 0x7f) + 2, crtc_sz_lines);
		printf("DMAC: addr = %04XH, size = %04XH\n",
			   dmac_address[2].W, dmac_counter[2].W & 0x3fff);
	}

	if (type & (TEXT | CRT)) {
		Ushort addr = start;
		for (i = 0; i < crtc_sz_lines; i++) {
			for (j = 0; j < 80; j += next_chara) {
				Uchar c = main_ram[ addr ];
				if (type & (CRT)) {
					if (c == 0x00 ||
						c == 0x60 ||
						c == 0x7f ||
						c == 0xa0 ||
						c >= 0xf8) {
						c = ' ';
					} else if (BETWEEN(0x20, c, 0x7e)) {
						;
					} else {
						c = 'X';
					}
					printf("%c", c);
				} else {
					printf("%02X", c);
				}
				addr = addr + next_chara;
			}
			printf("\n");
			addr += crtc_sz_attrs * 2;
			if (crtc_skip_line) {
				if (++i < crtc_sz_lines) {
					printf("(skip line)\n");
				}
			}
		}
	}

	if (type & ATTR) {
		Ushort addr = start + 80;
		for (i = 0; i < crtc_sz_lines; i++) {
			for (j = 0; j < crtc_sz_attrs; j++) {
				if (j > 0) {
					printf(" ");
				}
				printf("%02X-%02X", main_ram[addr + 0], main_ram[addr + 1]);
				addr += 2;
			}
			printf("\n");
			addr += 80;
			if (crtc_skip_line) {
				if (++i < crtc_sz_lines) {
					printf("(skip line)\n");
				}
			}
		}
	}

	if (type & COOKED) {
		int line;
		if (grph_ctrl & 0x20) {
			line = 25;
		} else {
			line = 20;
		}
		for (i = 0; i < line; i++) {
			for (j = 0; j < 80; j += next_chara) {
				Uchar c = text_attr_buf[ text_attr_flipflop ][i * 80 + j] >> 8;
				if (! isprint(c)) {
					c = alt_char;
				}
				printf("%c", c);
			}
			printf("\n");
		}
	}
}



#endif /* USE_MONITOR */
