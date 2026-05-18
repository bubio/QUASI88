#include <stdio.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "pc88cpu.h"
#include "pc88main.h"
#include "pc88sub.h"
#include "fdc.h"

#include "emu.h"
#include "event.h"


#ifdef USE_MONITOR

/*--------------------------------------------------------------
 * go
 *      実行
 *--------------------------------------------------------------*/
void mon_help_go(int oneline)
{
	if (oneline) {
		printf(
			"exec emu"
			);
	} else {
		printf(
			"  go\n"
			"    execute MAIN and|or SUB CPU\n"
			);
	}
}

void mon_exec_go(void)
{
	if (mon_exist_argv()) {
		error();
	}

	quasi88_exec();
}



/*--------------------------------------------------------------
 * trace <step>
 * trace #<step>
 *      指定したステップ数分、ステップイン実行。
 *--------------------------------------------------------------*/
void mon_help_trace(int oneline)
{
	if (oneline) {
		printf(
			"trace emu (step-in)"
			);
	} else {
		printf(
			"  trace [<cpu>] [#<steps>|<steps>]\n"
			"    execute MAIN and|or SUB CPU in steps for specified times\n"
			"    [all omit]        ... trace 1 step\n"
			"    <cpu>             ... CPU select MAIN|SUB (-cpu 2 only)\n"
			"                          [omit]... previous choiced cpu\n"
			"    #<steps>, <steps> ... counts of steps ( you can omit '#' )\n"
			);
	}
}

void mon_exec_trace(void)
{
	int count = 1;
	int cpu = -1;

	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			cpu = argv.val;
			mon_shift();
		}

		if (mon_exist_argv()) {

			/* [<count|#count>] */
			if (mon_argv_is(ARGV_SIZE) ||
				mon_argv_is(ARGV_NUM)) {
				count = argv.val;
				mon_shift();

			} else {
				error();
			}
		}
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu_timing <= 1) {
		if (cpu == -1) {
			;/* OK */
		} else {
			/* -cpu 0 / -cpu 1 時は、CPUの指定は禁止 */
			error();
		}
	} else {
		if (cpu == -1) {
			; /* OK */
		} else {
			select_main_cpu = (cpu == ARG_MAIN) ? TRUE : FALSE;
		}
	}

	trace_counter = count;
	quasi88_exec_trace();
}




/*--------------------------------------------------------------
 * step
 *      1ステップ分、ステップオーバー実行。
 *      CALL、RST、はスキップオーバーする
 *--------------------------------------------------------------*/
void mon_help_step(int oneline)
{
	if (oneline) {
		printf(
			"trace emu (step-over)"
			);
	} else {
		printf(
			"  step [<cpu>]\n"
			"    execute MAIN and|or SUB CPU in steps for 1 time\n"
			"    following instructions will step-over\n"
			"        CALL/RST\n"
			"    <cpu>       ... CPU select MAIN|SUB (-cpu 1 or 2 only)\n"
			"                    [omit]... suitable cpu (-cpu 0 or 1) or\n"
			"                              previous choiced cpu (-cpu 2)\n"
			);
	}
}

static void exec_step_or_next(int do_next, int cpu, int count)
{
	int use_breakpoint = FALSE;
	z80arch *z80 = (cpu == ARG_MAIN) ? &z80main_cpu : &z80sub_cpu;

	use_breakpoint = z80_breakpoint_if_step_or_next(z80, do_next);

	if (use_breakpoint) {
		quasi88_exec();
	} else {
		trace_counter = count;
		quasi88_exec_step();
	}
}

void mon_exec_step(void)
{
	int count = 1;
	int cpu = -1;

	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			cpu = argv.val;
			mon_shift();
		}
#if 0
		if (mon_exist_argv()) {

			/* [<count|#count>] */
			if (mon_argv_is(ARGV_SIZE) ||
				mon_argv_is(ARGV_NUM)) {
				count = argv.val;
				mon_shift();

			} else {
				error();
			}
		}
#endif
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu_timing <= 0) {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		} else {
			/* -cpu 0 時は、CPUの指定は禁止 */
			error();
		}
	} else if (cpu_timing == 1) {
		if (cpu == -1) {
			cpu = ARG_MAIN;
		} else {
			if ((dual_cpu_count == 0) && (cpu != ARG_MAIN)) {
				printf("currently, only MAIN CPU specified\n");
				return;
			}
		}
	} else {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		} else {
			select_main_cpu = (cpu == ARG_MAIN) ? TRUE : FALSE;
		}
	}

	exec_step_or_next(FALSE, cpu, count);
}



/*--------------------------------------------------------------
 * next
 *      1ステップ分、ステップオーバー実行。
 *      CALL、RST、DJNZ、LDIR、HALT、はスキップオーバーする
 *--------------------------------------------------------------*/
void mon_help_next(int oneline)
{
	if (oneline) {
		printf(
			"trace emu (step-over)"
			);
	} else {
		printf(
			"  next [<cpu>]\n"
			"    execute MAIN and|or SUB CPU in steps for 1 time\n"
			"    following instructions will step-over\n"
			"        CALL/RST/DJNZ/LD*R/CP*R/IN*R/OT*R/HALT\n"
			"    <cpu>       ... CPU select MAIN|SUB (-cpu 1 or 2 only)\n"
			"                    [omit]... suitable cpu (-cpu 0 or 1) or\n"
			"                              previous choiced cpu (-cpu 2)\n"
			);
	}
}

void mon_exec_next(void)
{
	int count = 1;
	int cpu = -1;

	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			cpu = argv.val;
			mon_shift();
		}
#if 0
		if (mon_exist_argv()) {

			/* [<count|#count>] */
			if (mon_argv_is(ARGV_SIZE) ||
				mon_argv_is(ARGV_NUM)) {
				count = argv.val;
				mon_shift();

			} else {
				error();
			}
		}
#endif
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu_timing <= 0) {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		} else {
			/* -cpu 0 時は、CPUの指定は禁止 */
			error();
		}
	} else if (cpu_timing == 1) {
		if (cpu == -1) {
			cpu = ARG_MAIN;
		} else {
			if ((dual_cpu_count == 0) && (cpu != ARG_MAIN)) {
				printf("currently, only MAIN CPU specified\n");
				return;
			}
		}
	} else {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		} else {
			select_main_cpu = (cpu == ARG_MAIN) ? TRUE : FALSE;
		}
	}

	exec_step_or_next(TRUE, cpu, count);
}



/*--------------------------------------------------------------
 * change [SET|UNSET]
 *      CPU処理が変わるまで、実行
 *--------------------------------------------------------------*/
void mon_help_change(int oneline)
{
	if (oneline) {
		printf(
			"trace emu (until CPU switch)"
			);
	} else {
		printf(
			"  change\n"
			"  change unset\n"
			"    execute until the CPU switches\n"
			"    change        ... execute until the CPU switches.\n"
			"    change unset  ... cancel and execute normaly.\n"
			"    CAUTION)\n"
			"        this is work under condition -cpu 0\n"
			);
	}
}
void mon_exec_change(void)
{
	static int save_change_break = FALSE;
	int change = TRUE;

	if (mon_exist_argv()) {
		if (mon_argv_is(ARGV_CHANGE)) {
			if (argv.val == ARG_SET) {
				change = TRUE;
			} else if (argv.val == ARG_UNSET) {
				change = FALSE;
			}
		} else {
			error();
		}
		mon_shift();
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu_timing > 0) {
		printf("command 'change' can use when -cpu 0\n");
		return;
	}

	save_change_break = change;
	if (change) {
		quasi88_exec_trace_change();
	} else {
		quasi88_exec();
	}
}



/*--------------------------------------------------------------
 * return
 *      RET 命令を処処理するまで実行。
 *--------------------------------------------------------------*/
void mon_help_return(int oneline)
{
	if (oneline) {
		printf(
			"exec emu until RET instruction (step-out)"
			);
	} else {
		printf(
			"  return [<cpu>]\n"
			"    execute MAIN and|or SUB CPU until RET instruction\n"
			"    <cpu>       ... CPU select MAIN|SUB\n"
			"                    [omit]... suitable cpu (-cpu 0 or 1) or\n"
			"                              previous choiced cpu (-cpu 2)\n"
			"    CAUTION)\n"
			"        this command is experimental\n"
			);
	}
}

void mon_exec_return(void)
{
	int cpu = -1;
	z80arch *z80;

	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			cpu = argv.val;
			mon_shift();
		}
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu_timing <= 0) {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		}
	} else if (cpu_timing == 1) {
		if (cpu == -1) {
			cpu = ARG_MAIN;
		}
	} else {
		if (cpu == -1) {
			cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
		} else {
			select_main_cpu = (cpu == ARG_MAIN) ? TRUE : FALSE;
		}
	}

	if (cpu == ARG_MAIN) {
		z80 = &z80main_cpu;
	} else {
		z80 = &z80sub_cpu;
	}
	z80_return_break_set(z80, TRUE);

	quasi88_exec();
}



/*--------------------------------------------------------------
 * break [<cpu>] [PC|READ|WRITE|IN|OUT] <addr|port> [#<No>]
 * break [<cpu>] [CLEAR|ENABLE|DISABLE|TEMPORARY|LASTING] #<No>
 * break [<cpu>] CLEARALL #0
 * break
 *      ブレークポイントの設定／解除／表示
 *--------------------------------------------------------------*/
void mon_help_break(int oneline)
{
	if (oneline) {
		printf(
			"set break point"
			);
	} else {
		printf(
			"  break  [<cpu>] [<action>] <addr|port> [#<No>]\n"
			"  breakt [<cpu>] [<action>] <addr|port> [#<No>]\n"
			"  break\n"
			"    set break point\n"
			"    (breakt command set one-time break point)\n"
			"    [all omit]  ... show all break points\n"
			"    <cpu>       ... CPU select MAIN|SUB\n"
			"                    [omit]... select MAIN\n"
			"    <action>    ... set action of conditon\n"
			"                    P or PC    ... break if PC reach addr\n"
			"                    R or READ  ... break if data is read\n"
			"                    W or WRITE ... break if data is written\n"
			"                    I or IN    ... break if data is input\n"
			"                    O or OUT   ... break if data is output\n"
			"                    C or CLEAR   ... clear break point\n"
			"                    E or ENABLE  ... enable break point\n"
			"                    D or DISABLE ... disable break point\n"
			"                    T or TEMPORARY ... set temporary\n"
			"                    L or LASTING   ... set lasting\n"
			"                    CLEARALL     ... clear all break point\n"
			"                    [omit]... select PC\n"
			"    <addr|port> ... specify address or port\n"
			"                    if <action> is C, E, D, T, L, or CLEARALL,\n"
			"                    don't set this argument\n"
			"    #<No>       ... number of break point. (#1..#9)\n"
			"                    if <action> is C, E, D, T, or L,\n"
			"                    this argument can't omit\n"
			"                    if <action> is CLEARALL, set #0 to confirm\n"
			"                    [omit]... select unused number\n"
			);
	}
}

static void exec_break_and_breakt(int onetime)
{
	int show = FALSE;
	char *s = NULL;
	z80arch *z80 = &z80main_cpu;
	int action = ARG_PC, type = BP_TYPE_PC, addr = 0;
	int number = 0;


	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			if (argv.val == ARG_MAIN) {
				z80 = &z80main_cpu;
			} else {
				z80 = &z80sub_cpu;
			}
			mon_shift();
		}

		/* [<action>] */
		if (mon_argv_is(ARGV_BREAK)) {
			action = argv.val;
			mon_shift();
		}

		/* <addr|port> */
		switch (action) {
		case ARG_IN:
		case ARG_I:
		case ARG_OUT:
		case ARG_O:
			if (!mon_argv_is(ARGV_PORT)) {
				error();
			}
			addr = argv.val;
			mon_shift();
			break;
		case ARG_PC:
		case ARG_P:
		case ARG_READ:
		case ARG_R:
		case ARG_WRITE:
		case ARG_W:
			if (!mon_argv_is(ARGV_ADDR)) {
				error();
			}
			addr = argv.val;
			mon_shift();
			break;
		case ARG_CLEAR:
		case ARG_C:
		case ARG_ENABLE:
		case ARG_E:
		case ARG_DISABLE:
		case ARG_D:
		case ARG_TEMPORARY:
		case ARG_T:
		case ARG_LASTING:
		case ARG_L:
		case ARG_CLEARALL:
			if (! mon_exist_argv()) {
				/* これらは必ず次の引数が必要 */
				error();
			}
			break;
		}

		/* [#<No>] */
		if (mon_exist_argv()) {
			if (action == ARG_CLEARALL) {
				if (mon_argv_is(ARGV_STR) &&
					strcmp(argv.str, "#0") == 0) {
					;
				} else {
					error();
				}
			} else {
				if (mon_argv_is(ARGV_SIZE) &&
					BETWEEN( 1, argv.val, BP_MAX - 1 )) {
					;
				} else {
					error();
				}
			}
			number = argv.val;
			mon_shift();
		}

	} else {

		show = TRUE;

	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (show) {
		z80_breakpoint_show(&z80main_cpu);
		z80_breakpoint_show(&z80sub_cpu);

	} else {
		int ctrl, num;

		switch (action) {

		case ARG_CLEARALL:
			z80_breakpoint_clear(z80);
			printf("clear break point %s - all\n", z80->cpuname);
			break;

		case ARG_CLEAR:
		case ARG_C:
		case ARG_ENABLE:
		case ARG_E:
		case ARG_DISABLE:
		case ARG_D:
		case ARG_TEMPORARY:
		case ARG_T:
		case ARG_LASTING:
		case ARG_L:
			if ((action == ARG_ENABLE) ||
				(action == ARG_E)) {
				ctrl = BP_CTRL_ENABLE;
				s = "enable";
			} else if ((action == ARG_DISABLE) ||
					   (action == ARG_D)) {
				ctrl = BP_CTRL_DISABLE;
				s = "disable";
			} else if ((action == ARG_TEMPORARY) ||
					   (action == ARG_T)) {
				ctrl = BP_CTRL_TEMPORARY;
				s = "set temporary";
			} else if ((action == ARG_LASTING) ||
					   (action == ARG_L)) {
				ctrl = BP_CTRL_LASTING;
				s = "set lasting";
			} else {
				ctrl = BP_CTRL_CLEAR;
				s = "delete";
			}
			if (z80_breakpoint_ctrl(z80, number, ctrl) == 0) {
				printf("break point is not used %s - #%d\n", z80->cpuname, number);
			} else {
				printf("%s break point %s - #%d\n", s, z80->cpuname, number);
			}
			break;

		default:
			switch (action) {
			case ARG_PC:
			case ARG_P:
				type = BP_TYPE_PC;
				s = "PC : %04XH";
				break;
			case ARG_READ:
			case ARG_R:
				type = BP_TYPE_READ;
				s = "READ : %04XH";
				break;
			case ARG_WRITE:
			case ARG_W:
				type = BP_TYPE_WRITE;
				s = "WRITE : %04XH";
				break;
			case ARG_IN:
			case ARG_I:
				type = BP_TYPE_IN;
				s = "IN : %02XH";
				break;
			case ARG_OUT:
			case ARG_O:
				type = BP_TYPE_OUT;
				s = "OUT : %02XH";
				break;
			}

			num = z80_breakpoint_set(z80, number, onetime, type, addr);

			if (num) {
				printf("set break point %s - #%d [ ", z80->cpuname, num);
				printf(s, addr);
				printf(" ]\n");
			} else {
				printf("break point %s is not enough\n", z80->cpuname);
			}
		}
	}

}
void mon_exec_break(void)
{
	exec_break_and_breakt(FALSE);
}
void mon_exec_breakt(void)
{
	exec_break_and_breakt(TRUE);
}



/*--------------------------------------------------------------
 * reset [<bas-mode>] [<clock-mode>] [<sound-board>] [<dipsw>]
 *      リセット。モードとサウンドボードとディップを設定できる
 *--------------------------------------------------------------*/
void mon_help_reset(int oneline)
{
	if (oneline) {
		printf(
			"reset PC8800 system"
			);
	} else {
		printf(
			"  reset [<basic-mode>] [<clock-mode>] [<sound-board>] [<dipsw>]\n"
			"    reset PC8800 (not execute)\n"
			"    <basic-mode>  ... BASIC mode N|V1S|V1H|V2\n"
			"                      [omit] select current BASIC mode\n"
			"    <clock-mode>  ... CPU Clock 4MHZ|8MHZ\n"
			"                      [omit] select current CLOCK mode\n"
			"    <sound-board> ... sound-board type SD|SD2\n"
			"                      [omit] select current sound-board\n"
			"    <dipsw>       ... dip-switch setting \n"
			"                      [omit] select current dip-switch setting\n"
			);
	}
}

void mon_exec_reset(void)
{
	T_RESET_CFG cfg;
	int dipsw = -1, bas_mode = -1, ck_mode = -1, sd_mode = -1;


	while (mon_exist_argv()) {
		if (mon_argv_is(ARGV_BASMODE)) {
			/* <bas-mode> */
			if (bas_mode != -1) {
				error();
			}
			bas_mode = argv.val;
			mon_shift();
		} else if (mon_argv_is(ARGV_CKMODE)) {
			/* <ck-mode> */
			if (ck_mode != -1) {
				error();
			}
			ck_mode = argv.val;
			mon_shift();
		} else if (mon_argv_is(ARGV_SDMODE)) {
			/* <sound-board> */
			if (sd_mode != -1) {
				error();
			}
			sd_mode = argv.val;
			mon_shift();
		} else if (mon_argv_is(ARGV_NUM)) {
			/* <dipsw> */
			if (dipsw != -1) {
				error();
			}
			dipsw = argv.val & 0xffff;
			mon_shift();
		} else {
			error();
		}
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/
	quasi88_get_reset_cfg(&cfg);

	switch (bas_mode) {
	case ARG_N:
		cfg.boot_basic = BASIC_N;
		break;
	case ARG_V1S:
		cfg.boot_basic = BASIC_V1S;
		break;
	case ARG_V1H:
		cfg.boot_basic = BASIC_V1H;
		break;
	case ARG_V2:
		cfg.boot_basic = BASIC_V2;
		break;
	}

	switch (ck_mode) {
	case ARG_8MHZ:
		cfg.boot_clock_4mhz = FALSE;
		break;
	case ARG_4MHZ:
		cfg.boot_clock_4mhz = TRUE;
		break;
	}

	switch (sd_mode) {
	case ARG_SD:
		cfg.sound_board = SOUND_I;
		break;
	case ARG_SD2:
		cfg.sound_board = SOUND_II;
		break;
	}

	if (dipsw != -1) {
		cfg.boot_dipsw = dipsw;
	}

	quasi88_reset(&cfg);

	/* quasi88_exec(); */
	/* go はしない。 reset-go というコマンドがあると便利？ */
}

/*--------------------------------------------------------------
 *  reg [[<cpu>] [<name> <value>]]
 *  reg all
 *      レジスタの内容を表示／変更
 *--------------------------------------------------------------*/
void mon_help_reg(int oneline)
{
	if (oneline) {
		printf(
			"show/set CPU register"
			);
	} else {
		printf(
			"  reg [[<cpu>] [<name> <value>]]\n"
			"  reg all\n"
			"    show & set register.\n"
			"    [all omit] ... show all register (MAIN and|or SUB).\n"
			"    <cpu>      ... CPU select MAIN|SUB\n"
			"                   [omit]... select suitable CPU\n"
			"    <name>     ... specity register name.\n"
			"                   AF|BC|DE|HL|AF'|BC'|DE'|HL'|IX|IY|SP|PC|I|R|IFF|IM|\n"
			"                   A|F|B|C|D|E|H|L|A'|F'|B'|C'|D'|E'|H'|L'|IXH|IXL|IYH|IYL\n"
			"    <value>    ... set value\n"
			"                   [omit]... show value of register\n"
			);
	}
}

void mon_exec_reg(void)
{
	int all = FALSE;
	int cpu = -1, reg = -1, value = 0;
	z80arch *z80;


	if (mon_exist_argv()) {

		if (mon_argv_is(ARGV_ALL)) {
			/* all */
			all = TRUE;
			mon_shift();
		} else {

			/* [<cpu>] */
			if (mon_argv_is(ARGV_CPU)) {
				cpu = argv.val;
				mon_shift();
			}

			if (mon_exist_argv()) {
				/* [<name>] */
				if (!mon_argv_is(ARGV_REG)) {
					error();
				}
				reg = argv.val;
				mon_shift();
				/* [<value>] */
				if (!mon_argv_is(ARGV_INT)) {
					error();
				}
				value = argv.val;
				mon_shift();
			}
		}
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	/* レジスタ表示 */
	if (reg == -1) {
		if (all) {
			monitor_show_cpu_status(TRUE);
			monitor_show_cpu_status(FALSE);
		} else {
			if (cpu == ARG_MAIN) {
				monitor_show_cpu_status(TRUE);
			} else if (cpu == ARG_SUB){
				monitor_show_cpu_status(FALSE);
			} else {
				if (cpu_timing <= 0) {
					if (select_main_cpu) {
						monitor_show_cpu_status(TRUE);
					} else {
						monitor_show_cpu_status(FALSE);
					}
				} else if (cpu_timing == 1) {
					if (dual_cpu_count == 0) {
						monitor_show_cpu_status(TRUE);
					} else {
						monitor_show_cpu_status(TRUE);
						monitor_show_cpu_status(FALSE);
					}
				} else {
					if (select_main_cpu) {
						monitor_show_cpu_status(TRUE);
					} else {
						monitor_show_cpu_status(FALSE);
					}
				}
			}
		}
		return;
	}

	/* レジスタ代入 */
	if (cpu == -1) {
		if (cpu_timing <= 0) {
			if (select_main_cpu) {
				cpu = ARG_MAIN;
			} else {
				cpu = ARG_SUB;
			}
		} else {
			printf("need to specofy cpu\n");
			return;
		}
	}

	if (cpu == ARG_MAIN) {
		z80 = &z80main_cpu;
	} else {
		z80 = &z80sub_cpu;
	}

	switch (reg) {
	case ARG_AF:
		z80->AF.W = value;
		break;
	case ARG_BC:
		z80->BC.W = value;
		break;
	case ARG_DE:
		z80->DE.W = value;
		break;
	case ARG_HL:
		z80->HL.W = value;
		break;
	case ARG_IX:
		z80->IX.W = value;
		break;
	case ARG_IY:
		z80->IY.W = value;
		break;
	case ARG_SP:
		z80->SP.W = value;
		break;
	case ARG_PC:
		z80->PC.W = value;
		break;
	case ARG_AF1:
		z80->AF1.W = value;
		break;
	case ARG_BC1:
		z80->BC1.W = value;
		break;
	case ARG_DE1:
		z80->DE1.W = value;
		break;
	case ARG_HL1:
		z80->HL1.W = value;
		break;
	case ARG_I:
		value &= 0xff;
		z80->I = value;
		break;
	case ARG_R:
		value &= 0xff;
		z80->R = value;
		break;
	case ARG_IFF:
		if (value) {
			value = 1;
		}
		z80->IFF = value;
		break;
	case ARG_IM:
		if (value > 3) {
			value = 2;
		}
		z80->IM = value;
		break;
	case ARG_HALT:
		if (value) {
			value = 1;
		}
		z80->HALT = value;
		break;
	case ARG_CK:
		z80->ck = value;
		break;
	case ARG_A:
		z80->AF.B.h = value & 0xff;
		break;
	case ARG_F:
		z80->AF.B.l = value & 0xff;
		break;
	case ARG_B:
		z80->BC.B.h = value & 0xff;
		break;
	case ARG_C:
		z80->BC.B.l = value & 0xff;
		break;
	case ARG_D:
		z80->DE.B.h = value & 0xff;
		break;
	case ARG_E:
		z80->DE.B.l = value & 0xff;
		break;
	case ARG_H:
		z80->HL.B.h = value & 0xff;
		break;
	case ARG_L:
		z80->HL.B.l = value & 0xff;
		break;
	case ARG_A1:
		z80->AF1.B.h = value & 0xff;
		break;
	case ARG_F1:
		z80->AF1.B.l = value & 0xff;
		break;
	case ARG_B1:
		z80->BC1.B.h = value & 0xff;
		break;
	case ARG_C1:
		z80->BC1.B.l = value & 0xff;
		break;
	case ARG_D1:
		z80->DE1.B.h = value & 0xff;
		break;
	case ARG_E1:
		z80->DE1.B.l = value & 0xff;
		break;
	case ARG_H1:
		z80->HL1.B.h = value & 0xff;
		break;
	case ARG_L1:
		z80->HL1.B.l = value & 0xff;
		break;
	case ARG_IXH:
		z80->IX.B.h = value & 0xff;
		break;
	case ARG_IXL:
		z80->IX.B.l = value & 0xff;
		break;
	case ARG_IYH:
		z80->IY.B.h = value & 0xff;
		break;
	case ARG_IYL:
		z80->IY.B.l = value & 0xff;
		break;
	}

	printf("[%s] reg %s <- %04X\n", mon_argv2str(cpu), mon_argv2str(reg), value);
}



/*--------------------------------------------------------------
 * disasm [[<cpu>][<start-addr>][#<steps>]]
 *      逆アセンブル
 *--------------------------------------------------------------*/
void mon_help_disasm(int oneline)
{
	if (oneline) {
		printf(
			"disassemble"
			);
	} else {
		printf(
			"  disasm [<cpu>] [<start-addr>][#<steps>]\n"
			"  disasm\n"
			"  disasm #0\n"
			"    disassemble.\n"
			"    [all omit]   ... disasmble 16 steps from last address.\n"
			"    <cpu>        ... CPU select MAIN|SUB\n"
			"                     [omit]... select MAIN\n"
			"    <start-addr> ... disassemble start address\n"
			"                     [omit]... reg PC address\n"
			"    #<steps>     ... disassemble steps\n"
			"                     [omit]... 16 steps\n"
			"                     if set #0, reset last address\n"
			);
	}
}

void mon_init_disasm(void)
{
	mon.disasm.cpu     = -1;
	mon.disasm.addr[0] = -1;
	mon.disasm.addr[1] = -1;
}

void mon_exec_disasm(void)
{
	int i, pc;
	int addr = -1;
	int cpu  = -1;
	int step = 16;
	z80arch *z80;


	if (mon_exist_argv()) {

		/* [<cpu>] */
		if (mon_argv_is(ARGV_CPU)) {
			cpu = argv.val;
			mon_shift();
		}

		/* [<addr>] */
		if (mon_argv_is(ARGV_ADDR)) {
			addr = argv.val;
			mon_shift();
		}

		/* [#<step>] */
		if (mon_argv_is(ARGV_SIZE)) {
			step = argv.val;
			mon_shift();

		} else if (mon_argv_is(ARGV_STR) && strcmp(argv.str, "#0") == 0) {
			if (cpu == -1) {
				mon.disasm.cpu     = -1;
				mon.disasm.addr[0] = -1;
				mon.disasm.addr[1] = -1;
				printf("disasm all reset\n");
			} else {
				mon.disasm.addr[cpu] = -1;
				printf("disasm addr reset\n");
			}
			return;
		}

	}
	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (cpu == -1) {
		/* CPU 未指定時 */
		cpu = mon.disasm.cpu;
		if (cpu == -1) {
			if (cpu_timing <= 0) {
				cpu = (select_main_cpu) ? ARG_MAIN : ARG_SUB;
			} else {
				cpu = ARG_MAIN;
			}
		}
	}

	if (cpu == ARG_MAIN) {
		z80 = &z80main_cpu;
	} else {
		z80 = &z80sub_cpu;
	}

	if (addr == -1) {
		/* ADDR 未指定時 */
		addr = mon.disasm.addr[ cpu ];
		if (addr == -1) {
			addr = z80->PC.W;
		}
	}


	pc = 0;
	for (i = 0; i < step; i++) {
		char asmcode[36];
		pc += z80_line_disasm(z80, addr + pc, asmcode);
		printf("%s\n", asmcode);
	}

	mon.disasm.cpu       = cpu;
	mon.disasm.addr[cpu] = (addr + pc) & 0xffff;
}



/*----------------------------------------------------------------------
 * fbreak [<cpu>] [READ|WRITE|DIAG] <drive> <track> [<sector>] [#<No>]
 * fbreak [<cpu>] CLEAR [#<No>]
 * fbreak
 *      FDC ブレークポイントの設定／解除／表示
 *                              この機能は peach氏により実装されました
 *----------------------------------------------------------------------*/
void mon_help_fbreak(int oneline)
{
	if (oneline) {
		printf(
			"set fdc break point"
			);
	} else {
		printf(
			"  fbreak [<action>] <drive> <track> [<sector>] [#<No>]\n"
			"  fbreak CLEAR [#<No>]\n"
			"  fbreak\n"
			"    set fdc break point\n"
			"    [all omit] ... show all break points\n"
			"    <action>   ... set action of conditon READ|WRITE|DIAG or CLEAR\n"
			"                   READ  ... break if fdc command is read\n"
			"                   WRITE ... break if fdc command is write\n"
			"                   DIAG  ... break if fdc command is diag\n"
			"                   CLEAR ... clear all break point\n"
			"                   [omit]... select READ\n"
			"    <drive>    ... specify drive (1 or 2)\n"
			"                   if <action> is CLEAR, this argument is invalid\n"
			"    <track>    ... specify track (0...)\n"
			"    <sector>   ... specify sector (1...)\n"
			"                   [omit]... select all sector\n"
			"    #<No>      ... number of break point. (#1..#10)\n"
			"                   #0    ... all break point when <action> is CLEAR\n"
			"                   [omit]... select #1\n"
			);
	}
}

void mon_exec_fbreak(void)
{
	int show = FALSE, i;
	char *s = NULL;
	int action = ARG_READ, number = 0;
	int drive = -1, track = -1, sector = -1;

	if (mon_exist_argv()) {
		/* <action> */
		if (mon_argv_is(ARGV_FBREAK)) {
			action = argv.val;
			mon_shift();
		}

		if (action == ARG_READ || action == ARG_WRITE || action == ARG_DIAG) {
			if (! mon_argv_is(ARGV_DRV)) {
				error();
			}
			drive = argv.val;
			mon_shift();
			if (! mon_argv_is(ARGV_NUM) || argv.val < 0 || argv.val > 163) {
				error();
			}
			track = argv.val;
			mon_shift();
			if (mon_exist_argv() && ! mon_argv_is(ARGV_SIZE)) {
				if (! mon_argv_is(ARGV_NUM) || argv.val < 0) {
					error();
				}
				sector = argv.val;
				mon_shift();
			}
		}

		/* [#<No>] */
		if (mon_exist_argv()) {
			if (argv.val < 0 || argv.val > NR_BPF ||
				(action != ARG_CLEAR && argv.val < 1)) {
				error();
			}
			number = argv.val - 1;
			mon_shift();
		}
	} else {
		show = TRUE;
	}

	if (mon_exist_argv()) {
		error();
	}

	if (show) {
		printf("  %s:\n", "FDC");
		for (i = 0; i < NR_BPF; i++) {
			/*if (break_point_fdc[i].type == BP_FDC_NONE) { continue; }*/
			printf("    #%d  ", i + 1);
			if (i < 9) {
				printf(" ");
			}
			drive = break_point_fdc[i].drive;
			track = break_point_fdc[i].track;
			sector = break_point_fdc[i].sector;
			if (break_point_fdc[i].type == BP_FDC_NONE) {
				printf("-- none --\n");
			} else {
				switch (break_point_fdc[i].type) {
				case BP_FDC_READ:
					printf("FDC READ from ");
					break;
				case BP_FDC_WRITE:
					printf("FDC WRITE  to ");
					break;
				case BP_FDC_DIAG:
					printf("FDC DIAG   in ");
					break;
				}
				printf("D:%d T:%d", drive, track);
				if (sector >= 0) {
					printf(" S:%d", sector);
				}
				printf("\n");
			}
		}
	} else {
		if (action == ARG_CLEAR) {
			if (number < 0) {
				for (i = 0; i < 9; i++) {
					break_point_fdc[i].type = BP_FDC_NONE;
				}
				printf("clear break point %s - all\n", "FDC");
			} else {
				break_point_fdc[number].type = BP_FDC_NONE;
				printf("clear break point %s - #%d\n", "FDC", number + 1);
			}
		} else {
			switch (action) {
			case ARG_READ:
				break_point_fdc[number].type = BP_FDC_READ;
				s = "READ";
				break;
			case ARG_WRITE:
				break_point_fdc[number].type = BP_FDC_WRITE;
				s = "WRITE";
				break;
			case ARG_DIAG:
				break_point_fdc[number].type = BP_FDC_DIAG;
				s = "DIAG";
				break;
			}
			break_point_fdc[number].drive = drive;
			break_point_fdc[number].track = track;
			break_point_fdc[number].sector = sector;
			printf("set break point - #%d [ %s : D:%d T:%d ",
				   number + 1, s, drive, track);
			if (sector >= 0) {
				printf("S:%d ", sector);
			}
			printf("]\n");
		}
	}

	pc88fdc_break_point();
}

#endif /* USE_MONITOR */
