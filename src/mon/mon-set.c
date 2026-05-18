#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "quasi88.h"
#include "initval.h"
#include "debug.h"
#include "monitor.h"
#include "mon-arg.h"
#include "mon-cmd.h"

#include "pc88cpu.h"
#include "pc88main.h"
#include "pc88sub.h"
#include "crtcdmac.h"
#include "memory.h"
#include "graph.h"
#include "intr.h"
#include "keyboard.h"
#include "pio.h"
#include "soundbd.h"
#include "fdc.h"

#include "emu.h"
#include "statusbar.h"
#include "menu.h"
#include "snddrv.h"


#ifdef USE_MONITOR

static struct {
	char        *var_name;
	char        *port_mes;
	int         var_type;
	void        *var_ptr;
} monitor_variable[] = {
	{ "boot_dipsw",			"(boot:3031)",	MTYPE_INT_C,	&boot_dipsw,		},
	{ "boot_from_rom",		"(boot:40>>3)",	MTYPE_INT_C,	&boot_from_rom,		},
	{ "boot_clock_4mhz",	"(boot:6E>>7)",	MTYPE_INT_C,	&boot_clock_4mhz,	},
	{ "boot_basic",			"(boot:3031)",	MTYPE_INT_C,	&boot_basic,		},
	{ "sound_board",		"(-sd/-sd2)",	MTYPE_INT_C,	&sound_board,		},
	{ "use_extram",			"(-extram)",	MTYPE_INT_C,	&use_extram,		},
	{ "use_jisho_rom",		"(-jisho)",		MTYPE_INT_C,	&use_jisho_rom,		},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},

	{ "sys_ctrl",			"(OUT:30)",		MTYPE_BYTE_C,	&sys_ctrl,			},
	{ "grph_ctrl",			"(OUT:31)",		MTYPE_BYTE_C,	&grph_ctrl,			},
	{ "misc_ctrl",			"(I/O:32)",		MTYPE_BYTE_C,	&misc_ctrl,			},
	{ "ALU1_ctrl",			"(OUT:34)",		MTYPE_BYTE,		&ALU1_ctrl,			},
	{ "ALU2_ctrl",			"(OUT:35)",		MTYPE_BYTE_C,	&ALU2_ctrl,			},
	{ "ctrl_signal",		"(OUT:40)",		MTYPE_BYTE_C,	&ctrl_signal,		},
	{ "grph_pile",			"(OUT:53)",		MTYPE_BYTE_C,	&grph_pile,			},
	{ "intr_level",			"(OUT:E4&07)",	MTYPE_INT_C,	&intr_level,		},
	{ "intr_priority",		"(OUT:E4&08)",	MTYPE_INT_C,	&intr_priority,		},
	{ "intr_sio_enable",	"(OUT:E6&04)",	MTYPE_INT_C,	&intr_sio_enable,	},
	{ "intr_vsync_enable",	"(OUT:E6&02)",	MTYPE_INT_C,	&intr_vsync_enable,	},
	{ "intr_rtc_enable",	"(OUT:E6&01)",	MTYPE_INT_C,	&intr_rtc_enable,	},
	{ "intr_sound_enable",	"(IO:~32AA&80)", MTYPE_INT_C,	&intr_sound_enable,	},
	{ "sound_ENABLE_A",		"(sd[27])",		MTYPE_INT,		&sound_ENABLE_A,	},
	{ "sound_ENABLE_B",		"(sd[27])",		MTYPE_INT,		&sound_ENABLE_B,	},
	{ "sound_LOAD_A",		"(sd[27])",		MTYPE_INT,		&sound_LOAD_A,		},
	{ "sound_LOAD_B",		"(sd[27])",		MTYPE_INT,		&sound_LOAD_B,		},
	{ "sound_FLAG_A",		"(IN:44)",		MTYPE_INT,		&sound_FLAG_A,		},
	{ "sound_FLAG_B",		"(IN:44)",		MTYPE_INT,		&sound_FLAG_B,		},
	{ "sound_TIMER_A",		"(sd[24]-[25])", MTYPE_INT,		&sound_TIMER_A,		},
	{ "sound_TIMER_B",		"(sd[26])",		MTYPE_INT,		&sound_TIMER_B,		},
	{ "sound_prescaler",	"(sd[2D]-[2F])", MTYPE_INT,		&sound_prescaler,	},
	{ "sound_reg[27]",		"(sd[27])",		MTYPE_BYTE,		&sound_reg[0x27],	},
	{ "sound2_EN_EOS",		"(sd[29])",		MTYPE_INT,		&sound2_EN_EOS,		},
	{ "sound2_EN_BRDY",		"(sd[29])",		MTYPE_INT,		&sound2_EN_BRDY,	},
	{ "sound2_EN_ZERO",		"(sd[29])",		MTYPE_INT,		&sound2_EN_ZERO,	},
	{ "use_cmdsing",		"",				MTYPE_BEEP,		&use_cmdsing,		},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},
	{ "RS232C_flag",		"",				MTYPE_INT,		&RS232C_flag,		},
	{ "VSYNC_flag",			"",				MTYPE_INT,		&VSYNC_flag,		},
	{ "ctrl_vrtc",			"",				MTYPE_INT,		&ctrl_vrtc,			},
	{ "RTC_flag",			"",				MTYPE_INT,		&RTC_flag,			},
	{ "SOUND_flag",			"",				MTYPE_INT,		&SOUND_flag,		},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},

	{ "mem",				"",				MTYPE_MEM,		NULL,				},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},
	{ "key",				"(IN:00..0F)",	MTYPE_KEY,		NULL,				},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},
	{ "palette",			"(OUT:5254..5B)", MTYPE_PALETTE, NULL,				},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},
	{ "crtc",				"",				MTYPE_CRTC,		NULL,				},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},
	{ "pio",				"(IO:FC..FF)",	MTYPE_PIO,		NULL,				},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},

#ifdef USE_SOUND
	{ "volume",				"(-vol)",		MTYPE_VOLUME,			NULL,		},
	{ "fm-mixer",			"(-fmvol)",		MTYPE_FMMIXER,			NULL,		},
	{ "psg-mixer",			"(-psgvol)",	MTYPE_PSGMIXER,			NULL,		},
	{ "beep-mixer",			"(-beepvol)",	MTYPE_BEEPMIXER,		NULL,		},
	{ "pcg-mixer",			"(-pcgvol)",	MTYPE_PCGMIXER,			NULL,		},
	{ "rhythm-mixer",		"(-rhythmvol)",	MTYPE_RHYTHMMIXER,		NULL,		},
	{ "adpcm-mixer",		"(-adpcmvol)",	MTYPE_ADPCMMIXER,		NULL,		},
#ifdef USE_FMGEN
	{ "fmgen-mixer",		"(-fmgenvol)",	MTYPE_FMGENMIXER,		NULL,		},
#endif
	{ "sample-mixer",		"(-samplevol)",	MTYPE_SAMPLEMIXER,		NULL,		},
	{ "",					"",				MTYPE_MIXER,			NULL,		},
	{ "",					"",				MTYPE_NEWLINE,			NULL,		},
#endif

	{ "cpu_timing",			"(-cpu)",		MTYPE_CPUTIMING, &cpu_timing,		},
	{ "select_main_cpu",	"",				MTYPE_INT,		&select_main_cpu,	},
	{ "dual_cpu_count",		"",				MTYPE_INT,		&dual_cpu_count,	},
	{ "CPU_1_COUNT",		"",				MTYPE_INT,		&CPU_1_COUNT,		},
	{ "cpu_slice_us",		"(-cpu2us)",	MTYPE_INT,		&cpu_slice_us,		},
	{ "calendar_stop",		"(-timestop)",	MTYPE_INT,		&calendar_stop,		},
	{ "cmt_speed",			"(-cmt_speed)",	MTYPE_INT,		&cmt_speed,			},
	{ "cmt_intr",			"(-cmt_intr)",	MTYPE_INT,		&cmt_intr,			},
	{ "cmt_wait",			"(-cmt_wait)",	MTYPE_INT,		&cmt_wait,			},
	{ "highspeed_mode",		"(-hsbasic)",	MTYPE_MEMWAIT,	&highspeed_mode,	},
	{ "monitor_15k",		"(-15k)",		MTYPE_INT,		&monitor_15k,		},
	{ "use_pcg",			"(-pcg)",		MTYPE_FONT,		&use_pcg,			},
	{ "font_kind",			"",				MTYPE_FONT,		&font_kind,			},
	{ "memory_wait",		"(-mem_wait)",	MTYPE_MEMWAIT,		&memory_wait,		},
	{ "sub_load_rate",		"(-subload)",	MTYPE_INT,		&sub_load_rate,		},
	{ "disk_exchange",		"(-exchange)",	MTYPE_INT,		&disk_exchange,		},
	{ "fdc_debug_mode",		"(-fdcdebug)",	MTYPE_INT,		&fdc_debug_mode,	},
	{ "fdc_ignore_readonly", "(-ignore_ro)", MTYPE_INT,		&fdc_ignore_readonly, },
	{ "fdc_wait",			"(-fdc_wait)",	MTYPE_INT,		&fdc_wait,			},
	{ "frameskip_rate",		"(-frameskip)",	MTYPE_FRAMESKIP, &frameskip_rate,	},
	{ "monitor_analog",		"(-analog)",	MTYPE_INT,		&monitor_analog,	},
	{ "use_auto_skip",		"(-autoskip)",	MTYPE_INT,		&use_auto_skip,		},
	{ "use_interlace",		"(-interlace)",	MTYPE_INTERLACE, &use_interlace,	},
	{ "use_half_interp",	"(-interp)",	MTYPE_INTERP,	&use_half_interp,	},
	{ "mon_aspect",			"(-aspect)",	MTYPE_DOUBLE,	&mon_aspect,		},
	{ "hide_mouse",			"(-hide_mouse)", MTYPE_INT,		&hide_mouse,		},
	{ "grab_mouse",			"(-grab_mouse)", MTYPE_INT,		&grab_mouse,		},
	{ "mouse_mode",			"(-mouse)",		MTYPE_INT,		&mouse_mode,		},
	{ "mouse_swap_button",	"(-mouseswap)",	MTYPE_INT,		&mouse_swap_button,	},
	{ "mouse_sensitivity",	"(-mousespeed)", MTYPE_INT,		&mouse_sensitivity,	},
	{ "joy_key_mode",		"(-joykey)",	MTYPE_INT,		&joy_key_mode,		},
	{ "joy_swap_button",	"(-joyswap)",	MTYPE_INT,		&joy_swap_button,	},
	{ "joy2_key_mode",		"",				MTYPE_INT,		&joy2_key_mode,		},
	{ "joy2_swap_button",	"",				MTYPE_INT,		&joy2_swap_button,	},
	{ "tenkey_emu",			"(-tenkey)",	MTYPE_INT,		&tenkey_emu,		},
	{ "numlock_emu",		"(-numlock)",	MTYPE_INT,		&numlock_emu,		},
	{ "function_f[1]",		"",				MTYPE_INT,		&function_f[1],		},
	{ "function_f[2]",		"",				MTYPE_INT,		&function_f[2],		},
	{ "function_f[3]",		"",				MTYPE_INT,		&function_f[3],		},
	{ "function_f[4]",		"",				MTYPE_INT,		&function_f[4],		},
	{ "function_f[5]",		"",				MTYPE_INT,		&function_f[5],		},
	{ "function_f[6]",		"(-f6)",		MTYPE_INT,		&function_f[6],		},
	{ "function_f[7]",		"(-f7)",		MTYPE_INT,		&function_f[7],		},
	{ "function_f[8]",		"(-f8)",		MTYPE_INT,		&function_f[8],		},
	{ "function_f[9]",		"(-f9)",		MTYPE_INT,		&function_f[9],		},
	{ "function_f[10]",		"(-f10)",		MTYPE_INT,		&function_f[10],	},
	{ "function_f[11]",		"",				MTYPE_INT,		&function_f[11],	},
	{ "function_f[12]",		"",				MTYPE_INT,		&function_f[12],	},
	{ "function_f[13]",		"",				MTYPE_INT,		&function_f[13],	},
	{ "function_f[14]",		"",				MTYPE_INT,		&function_f[14],	},
	{ "function_f[15]",		"",				MTYPE_INT,		&function_f[15],	},
	{ "function_f[16]",		"",				MTYPE_INT,		&function_f[16],	},
	{ "function_f[17]",		"",				MTYPE_INT,		&function_f[17],	},
	{ "function_f[18]",		"",				MTYPE_INT,		&function_f[18],	},
	{ "function_f[19]",		"",				MTYPE_INT,		&function_f[19],	},
	{ "function_f[20]",		"",				MTYPE_INT,		&function_f[20],	},
	{ "romaji_type",		"(-romaji)",	MTYPE_INT,		&romaji_type,		},
	{ "need_focus",			"(-focus)",		MTYPE_INT,		&need_focus,		},
	{ "cpu_clock_mhz",		"(-clock)",		MTYPE_CLOCK,	&cpu_clock_mhz,		},
	{ "sound_clock_mhz",	"(-soundclock)", MTYPE_CLOCK,	&sound_clock_mhz,	},
	{ "vsync_freq_hz",		"(-vsync)",		MTYPE_CLOCK,	&vsync_freq_hz,		},
	{ "wait_rate",			"(-speed)",		MTYPE_INT,		&wait_rate,			},
	{ "wait_by_sleep",		"(-sleep)",		MTYPE_INT,		&wait_by_sleep,		},
	{ "no_wait",			"(-nowait)",	MTYPE_INT,		&no_wait,			},
	/*{ "wait_sleep_min_us",	"(-sleepparm)", MTYPE_INT,		&wait_sleep_min_us, },*/
	{ "status_imagename",	"(-statusimage)", MTYPE_INT,	&status_imagename,	},
	{ "menu_lang",			"(-english)",	MTYPE_INT,		&menu_lang,			},
	{ "menu_readonly",		"(-ro)",		MTYPE_INT,		&menu_readonly,		},
	{ "menu_swapdrv",		"(-swapdrv)",	MTYPE_INT,		&menu_swapdrv,		},
	{ "filename_coding",	"(-euc/-sjis)",	MTYPE_INT,		&filename_coding,	},
	{ "filename_synchronize", "",			MTYPE_INT,		&filename_synchronize, },
	{ "verbose_proc",		"(-verbose&01)", MTYPE_INT,		&verbose_proc,		},
	{ "verbose_z80",		"(-verbose&02)", MTYPE_INT,		&verbose_z80,		},
	{ "verbose_io",			"(-verbose&04)", MTYPE_INT,		&verbose_io,		},
	{ "verbose_pio",		"(-verbose&08)", MTYPE_INT,		&verbose_pio,		},
	{ "verbose_fdc",		"(-verbose&10)", MTYPE_INT,		&verbose_fdc,		},
	{ "verbose_wait",		"(-verbose&20)", MTYPE_INT,		&verbose_wait,		},
	{ "verbose_suspend",	"(-verbose&40)", MTYPE_INT,		&verbose_suspend,	},
	{ "verbose_snd",		"(-verbose&80)", MTYPE_INT,		&verbose_snd,		},
	{ "",					"",				MTYPE_NEWLINE,	NULL,				},

#ifdef PROFILER
	{ "debug_profiler",		"for debug",	MTYPE_INT,		&debug_profiler,	},
#endif
#ifdef DEBUGLOG
	{ "pio_debug",			"for debug",	MTYPE_INT,		&pio_debug,			},
	{ "fdc_debug",			"for debug",	MTYPE_INT,		&fdc_debug,			},
	{ "main_debug",			"for debug",	MTYPE_INT,		&main_debug,		},
	{ "sub_debug",			"for debug",	MTYPE_INT,		&sub_debug,			},
#endif

	{ "mon.reg.style",		"for monitor",	MTYPE_INT,		&mon.reg.style,		},
	{ "mon.reg.line",		"for monitor",	MTYPE_INT,		&mon.reg.line,		},
};

static struct {
	char        *block_name;
	int         start;
	int         end;
} monitor_variable_block[] = {
	{ "boot",	0,		0	},
	{ "main",	1,		2	},
	{ "intr",	2,		2	},
#ifdef USE_SOUND
	{ "vol",	8,		8	},
	{ "emu",	9,		10	},
#else
	{ "emu",	8,		9	},
#endif
};


/*--------------------------------------------------------------
 * set [<variable> [<value>] ]
 * show [<variable> ]
 *      内部変数を表示／変更
 *--------------------------------------------------------------*/
void mon_help_set(int oneline)
{
	if (oneline) {
		printf(
			"show/set variable"
			);
	} else {
		printf(
			"  set [[<variabe-name> [<value>]]]\n"
			"    show & set variables.\n"
			"    [all omit]     ... show all variable.\n"
			"    <variabe-name> ... specify variable name.\n"
			"    <value>        ... set value\n"
			"                       [omit]... show value of variable\n"
			);
	}
}

static void monitor_set_mem_printf(void) /*** set mem ***/
{
	const char *r0000, *r6000, *r8000, *r8400, *rC000, *rF000;
	const char *w0000, *w6000, *w8000, *w8400, *wC000, *wF000;

	if (grph_ctrl & GRPH_CTRL_64RAM) {
		/* 64KB RAM mode */
		r0000 = r6000 = "MAIN RAM";
	} else {
		/* ROM/RAM mode */
		if (grph_ctrl & GRPH_CTRL_N) {
			/* N BASIC */
			r0000 = r6000 = "N ROM";
		} else {
			/* N88 BASIC */
			r0000 = "MAIN ROM";
			if (ext_rom_bank & EXT_ROM_NOT) {
				/* 通常ROM */
				r6000 = "MAIN ROM";
			} else {
				/* 拡張ROM */
				r6000 = "EXT ROM";			/*misc_ctrl & MISC_CTRL_EBANK*/
			}
		}
	}
	w0000 = w6000 = "MAIN_RAM";
	if (ext_ram_ctrl & 0x01) {
		/* 拡張RAM R可 */
		if (ext_ram_bank < use_extram * 4) {
			r0000 = r6000 = "EXT RAM";		/*ext_ram_bank*/
		}
	}
	if (ext_ram_ctrl & 0x10) {
		/* 拡張RAM W可 */
		if (ext_ram_bank < use_extram * 4) {
			w0000 = w6000 = "EXT RAM";		/*ext_ram_bank*/
		}
	}

	if (grph_ctrl & (GRPH_CTRL_64RAM | GRPH_CTRL_N)) {
		r8000 = w8000 = "MAIN RAM";
	} else {
		r8000 = w8000 = "WINDOW";			/*window_offset*/
	}

	r8400 = w8400 = "MAIN RAM";

	if ((misc_ctrl & MISC_CTRL_EVRAM)   &&
		(ALU2_ctrl & ALU2_CTRL_VACCESS)) {
		/* 拡張アクセスモード  VRAM拡張アクセス */
		rC000 = rF000 = wC000 = wF000 = "VRAM Ext-Acc.";

	} else if (!(misc_ctrl & MISC_CTRL_EVRAM) &&
			   (memory_bank != MEMORY_BANK_MAIN)) {
		/* 独立アクセスモード メインBANKでない  */
		rC000 = rF000 = wC000 = wF000 = ((memory_bank == 0)
										 ? "VRAM B"
										 : ((memory_bank == 1)
											? "VRAM R"
											: "VRAM G"));
	} else {
		rC000 = wC000 = "MAIN RAM";
		if (high_mode &&
			(misc_ctrl & MISC_CTRL_TEXT_MAIN) == 0) {
			/* 高速RAM */
			rF000 = wF000 = "HIGH RAM";
		} else {
			rF000 = wF000 = "MAIN RAM";
		}
		if (jisho_rom_ctrl == FALSE) {
			/* 辞書ROM */
			rC000 = rF000 = "JISHO ROM";	/*jisho_rom_bank*/
		}
	}

	printf("  Memory mapping\n");

	printf("    %-12s%-12s%-12s        ", "Addr.",   "Read", "Write");
	printf("Bank-Status\n");

	printf("    %-12s%-12s%-12s        ", "0000-5FFF", r0000, w0000);
	printf("Ext. Ram Bank(0-%d)  = %d\n", use_extram * 4, ext_ram_bank);

	printf("    %-12s%-12s%-12s        ", "6000-7FFF", r6000, w6000);
	printf("Ext. Rom Bank(0-3)  = %d\n", misc_ctrl & MISC_CTRL_EBANK);

	printf("    %-12s%-12s%-12s        ", "8000-83FF", r8000, w8000);
	printf("Window offset       = %04XH\n", window_offset);

	printf("    %-12s%-12s%-12s        ", "8400-BFFF", r8400, w8400);
	printf("\n");

	printf("    %-12s%-12s%-12s        ", "C000-EFFF", rC000, wC000);
	printf("JishoRom Bank(0-31) = %d\n", jisho_rom_bank);

	printf("    %-12s%-12s%-12s        ", "F000-FFFF", rF000, wF000);
	printf("\n");
}
static void monitor_set_key_printf(void) /*** set key ***/
{
	int j;
	printf("  %-23s %-15s", "key_scan[0]-[15]", "(IN:00..0F)");
	for (j = 0; j < 0x8; j++) {
		printf("%02X ", key_scan[j]);
	}
	printf("\n");
	printf("  %-23s %-15s", "", "");
	for (; j < 0x10; j++) {
		printf("%02X ", key_scan[j]);
	}
	printf("\n");
}
static void monitor_set_palette_printf(void) /*** set palette ***/
{
	int j;
	const char *now = "    [Pal-mode is   ]";
	const char *pal = ((misc_ctrl & MISC_CTRL_ANALOG)
					   ? "    [      *Analog*]"
					   : "    [     *Digital*]");

	printf("  %-23s %-15sG:R:B = %01X:%01X:%01X\n",
		   "vram_bg_palette",
		   "(OUT:52/54)",
		   vram_bg_palette.green,
		   vram_bg_palette.red,
		   vram_bg_palette.blue);

	for (j = 0; j < 8; j++) {
		printf("  %-23s (OUT:%02X)       G:R:B = %01X:%01X:%01X\n",
			   ((j == 0)
				? "vram_palette"
				: ((j == 2)
				   ? now
				   : ((j == 3)
					  ? pal
					  : ""))),
			   j + 0x54,
			   vram_palette[j].green,
			   vram_palette[j].red,
			   vram_palette[j].blue);
	}
}
static void monitor_set_crtc_printf(void) /*** set crtc ***/
{
	int j;
	const char *dmamode[] = {
		"(burst)", "(character)"
	};
	const char *blinktime[] = {
		"(x3)", "(x1.5)", "(x1)", "(x0.75)"
	};
	const char *skipline[] = {
		"(normal)", "(skip)"
	};
	const char *cursortype[] = {
		"(noblink, underline)", "(blink, underline)",
		"(noblink, block)",     "(blink, block)"
	};
	const char *lineschar[] = {
		"(forbid)", "(forbid)", "", "", "", "", "",
		"(200line, 25row)", "", "(200line, 20row)", "", "", "",
		"", "", "(400line, 25row)", "", "", "", "(400line, 20row)",
		"", "", "", "", "", "", "", "", "", "", "", "",
	};
	const char *vwide[] = {
		"", "(400line, 20row)", "(400line, 25row)", "",
		"", "(200line, 20row)", "(200line, 25row)", ""
	};
	const char *hwide[] = {
		"(forbid)", "(forbid)", "(forbid)", "(forbid)", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"", "(400line)", "", "", "", "", "", "(200line)", "",
	};
	const char *attr[] = {
		"(mono, sepatare, o)", "(no-attr, x)",
		"(color, sepatare, o)", "(forbid)", "(mono, mixed, o)",
		"(mono, mixed, x)", "(forbid)", "(forbid)"
	};

	printf("  CRTC & DMAC internal variable\n");

	printf("    %-38s%s\n",
		   "CRTC active", (crtc_active) ? "Yes" : "No");
	printf("    %-38s%02x\n",
		   "Interrupt mask", crtc_intr_mask);
	printf("    %-38s%s\n",
		   "Reverse", (crtc_reverse_display) ? "Yes" : "No");
	printf("    %-38s%s\n",
		   "Line Skip", (crtc_skip_line) ? "Yes" : "No");
	printf("    %-38s%02XH(%d) , %02XH(%d)\n",
		   "Cursor position[X,Y]",
		   crtc_cursor[0], crtc_cursor[0], crtc_cursor[1], crtc_cursor[1]);


	printf("    Format[0] %02XH  ", crtc_format[0]);
	j = crtc_format[0] >> 7;
	printf("C-------:DMA mode      %2d %s\n", j, dmamode[ j ]);
	printf("                   ");
	j = (crtc_format[0] & 0x7f) + 2;
	printf("-HHHHHHH:chars/line    %2d\n", j);

	printf("    Format[1] %02XH  ", crtc_format[1]);
	j = crtc_format[1] >> 6;
	printf("BB------:blink time    %2d %s\n", j, blinktime[ j ]);
	printf("                   ");
	j = (crtc_format[1] & 0x3f) + 1;
	printf("--LLLLLL:lines/screen  %2d\n", j);

	printf("    Format[2] %02XH  ", crtc_format[2]);
	j = crtc_format[2] >> 7;
	printf("S-------:skip line     %2d %s\n", j, skipline[ j ]);
	printf("                   ");
	j = (crtc_format[2] >> 5) & 0x03;
	printf("-CC-----:cursor type   %2d %s\n", j, cursortype[ j ]);
	printf("                   ");
	j = (crtc_format[2] & 0x1f) + 1;
	printf("---RRRRR:lines/char    %2d\n", j);

	printf("    Format[3] %02XH  ", crtc_format[3]);
	j = (crtc_format[3] >> 5);
	printf("VVV-----:v wide(line)  %2d %s\n", j + 1, vwide[ j ]);
	printf("                   ");
	j = (crtc_format[3] & 0x1f);
	printf("---ZZZZZ:h wide(char)  %2d %s\n", j + 2, hwide[ j ]);

	printf("    Format[4] %02XH  ", crtc_format[4]);
	j = (crtc_format[4] >> 5);
	printf("TTT-----:attr type     %2d %s\n", j, attr[ j ]);
	printf("                   ");
	j = (crtc_format[4] & 0x1f) + 1;
	printf("---AAAAA:attr size     %2d\n", j);

	for (j = 0; j < 4; j++) {
		printf("  %-23s Ch.%d%-11s %04XH .. +%04XH, %s\n",
			   (j == 0) ? "  DMAC addr/cntr/mode" : "", j, "",
			   dmac_address[j].W, dmac_counter[j].W & 0x3fff,
			   (((dmac_counter[j].W & 0xc000) == 0x0000)
				? "Verify"
				: (((dmac_counter[j].W & 0xc000) == 0x4000)
				   ? "Write"
				   : (((dmac_counter[j].W & 0xc000) == 0x8000)
					  ? "Read"
					  : "BAD")))
			  );
	}
	printf("  %-23s %-15s %02X (%X%X%X%X%X%X%X%X)\n",
		   "", "Mode",
		   dmac_mode,
		   (dmac_mode >> 7) & 0x01, (dmac_mode >> 6) & 0x01,
		   (dmac_mode >> 5) & 0x01, (dmac_mode >> 4) & 0x01,
		   (dmac_mode >> 3) & 0x01, (dmac_mode >> 2) & 0x01,
		   (dmac_mode >> 1) & 0x01, (dmac_mode >> 0) & 0x01
		  );
	printf("  %-23s %-15s %s\n",
		   "  text_display",    "",
		   ((text_display == TEXT_DISABLE)
			? "TEXT_DISABLE"
			: ((text_display == TEXT_ENABLE)
			   ? "TEXT_ENABLE"
			   : "TEXT_ATTR_ONLY")));
}
static void monitor_set_pio_printf(void) /*** set pio ***/
{
	printf(
		"  pio_AB[0][0].type     %s    ___    ___  %s   pio_AB[1][0].type\n",
		(pio_AB[0][0].type == PIO_READ) ? "R" : "W",
		(pio_AB[1][0].type == PIO_READ) ? "R" : "W");

	printf(
		"              .exist    %s       \\  /     %s               .exist\n",
		(pio_AB[0][0].exist == PIO_EXIST) ? "*" : "-",
		(pio_AB[1][0].exist == PIO_EXIST) ? "*" : "-");

	printf(
		"              .data     %02XH      \\/      %02XH             .data\n",
		(pio_AB[0][0].data),
		(pio_AB[1][0].data));

	printf("                                 /\\\n");

	printf(
		"  pio_AB[0][1].type     %s    ___/  \\___  %s   pio_AB[1][1].type\n",
		(pio_AB[0][1].type == PIO_READ) ? "R" : "W",
		(pio_AB[1][1].type == PIO_READ) ? "R" : "W");

	printf(
		"              .exist    %s                %s               .exist\n",
		(pio_AB[0][1].exist == PIO_EXIST) ? "*" : "-",
		(pio_AB[1][1].exist == PIO_EXIST) ? "*" : "-");

	printf(
		"              .data     %02XH              %02XH             .data\n",
		(pio_AB[0][1].data),
		(pio_AB[1][1].data));

	printf(
		"                                                               \n");

	printf("  \n");

	printf(
		"  pio_C[0][0].type      %s    ___    ___  %s   pio_C[1][0].type\n",
		(pio_C[0][0].type == PIO_READ) ? "R" : "W",
		(pio_C[1][0].type == PIO_READ) ? "R" : "W");

	printf(
		"             .data      %02XH     \\  /     %02XH            .data\n",
		(pio_C[0][0].data),
		(pio_C[1][0].data));

	printf(
		"             .cont_f    % 2d       \\/      % 2d             .cont_f\n",
		(pio_C[0][0].cont_f),
		(pio_C[1][0].cont_f));

	printf("                                 /\\\n");

	printf(
		"  pio_C[0][1].type      %s    ___/  \\___  %s   pio_C[1][1].type\n",
		(pio_C[0][1].type == PIO_READ) ? "R" : "W",
		(pio_C[1][1].type == PIO_READ) ? "R" : "W");

	printf(
		"             .data      %02XH              %02XH            .data\n",
		(pio_C[0][1].data),
		(pio_C[1][1].data));

	printf(
		"             .cont_f    % 2d               % 2d             .cont_f\n",
		(pio_C[0][1].cont_f),
		(pio_C[1][1].cont_f));
}


#ifdef USE_SOUND
static void monitor_set_volume_printf(int index)
{
	if (xmame_has_mastervolume()) {
		printf("  %-23s %-15s %d\n",
			   monitor_variable[index].var_name,
			   monitor_variable[index].port_mes,
			   xmame_cfg_get_mastervolume());
	} else {
		printf("  %-23s %-15s --\n",
			   monitor_variable[index].var_name, "");
	}
}
static void monitor_set_fmmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_FM));
}
static void monitor_set_psgmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_PSG));
}
static void monitor_set_beepmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_BEEP));
}
static void monitor_set_pcgmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_PCG));
}
static void monitor_set_rhythmmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_RHYTHM));
}
static void monitor_set_adpcmmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_ADPCM));
}
#ifdef USE_FMGEN
static void monitor_set_fmgenmixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_FMGEN));
}
#endif
static void monitor_set_samplemixer_printf(int index)
{
	printf("  %-23s %-15s %d\n",
		   monitor_variable[index].var_name,
		   monitor_variable[index].port_mes,
		   xmame_cfg_get_mixer_volume(XMAME_MIXER_SAMPLE));
}
static void monitor_set_mixer_printf(void)
{
	if (xmame_has_sound()) {
		printf("\n  Following is mixing level of xmame-sound-driver.\n");
		xmame_cfg_set_mixer_volume(-1, -1);
	}
}

static void monitor_set_volume(int vol)
{
	xmame_cfg_set_mastervolume(vol);
}
static void monitor_set_fmmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_FM, vol);
}
static void monitor_set_psgmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_PSG, vol);
}
static void monitor_set_beepmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_BEEP, vol);
}
static void monitor_set_pcgmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_PCG, vol);
}
static void monitor_set_rhythmmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_RHYTHM, vol);
}
static void monitor_set_adpcmmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_ADPCM, vol);
}
#ifdef USE_FMGEN
static void monitor_set_fmgenmixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_FMGEN, vol);
}
#endif
static void monitor_set_samplemixer(int vol)
{
	xmame_cfg_set_mixer_volume(XMAME_MIXER_SAMPLE, vol);
}
#endif /* USE_SOUND */

static void monitor_set_show_printf(int index) /*** set (print) ***/
{
	int val;

	switch (monitor_variable[index].var_type) {

	case MTYPE_INT:
	case MTYPE_INT_C:
	case MTYPE_FONT:
	case MTYPE_FRAMESKIP:
	case MTYPE_INTERLACE:
	case MTYPE_INTERP:
	case MTYPE_CPUTIMING:
	case MTYPE_MEMWAIT:
	case MTYPE_BEEP:
		val = *((int *)monitor_variable[index].var_ptr);
		goto MTYPE_numeric_variable;

	case MTYPE_BYTE:
	case MTYPE_BYTE_C:
		val = *((byte *)monitor_variable[index].var_ptr);
		goto MTYPE_numeric_variable;

	case MTYPE_WORD:
	case MTYPE_WORD_C:
		val = *((word *)monitor_variable[index].var_ptr);
		goto MTYPE_numeric_variable;

	MTYPE_numeric_variable:
		;
		printf("  %-23s %-15s %08XH (%d)\n",
			   monitor_variable[index].var_name,
			   monitor_variable[index].port_mes, val, val);
		break;

	case MTYPE_DOUBLE:
	case MTYPE_DOUBLE_C:
	case MTYPE_CLOCK:
		printf("  %-23s %-15s %8.4f\n",
			   monitor_variable[index].var_name,
			   monitor_variable[index].port_mes,
			   *((double *)monitor_variable[index].var_ptr));
		break;

	case MTYPE_MEM:
		monitor_set_mem_printf();
		break;
	case MTYPE_KEY:
		monitor_set_key_printf();
		break;
	case MTYPE_PALETTE:
		monitor_set_palette_printf();
		break;
	case MTYPE_CRTC:
		monitor_set_crtc_printf();
		break;
	case MTYPE_PIO:
		monitor_set_pio_printf();
		break;

#ifdef USE_SOUND
	case MTYPE_VOLUME:
		monitor_set_volume_printf(index);
		break;
	case MTYPE_FMMIXER:
		monitor_set_fmmixer_printf(index);
		break;
	case MTYPE_PSGMIXER:
		monitor_set_psgmixer_printf(index);
		break;
	case MTYPE_BEEPMIXER:
		monitor_set_beepmixer_printf(index);
		break;
	case MTYPE_PCGMIXER:
		monitor_set_pcgmixer_printf(index);
		break;
	case MTYPE_RHYTHMMIXER:
		monitor_set_rhythmmixer_printf(index);
		break;
	case MTYPE_ADPCMMIXER:
		monitor_set_adpcmmixer_printf(index);
		break;
#ifdef  USE_FMGEN
	case MTYPE_FMGENMIXER:
		monitor_set_fmgenmixer_printf(index);
		break;
#endif
	case MTYPE_SAMPLEMIXER:
		monitor_set_samplemixer_printf(index);
		break;
	case MTYPE_MIXER:
		monitor_set_mixer_printf();
		break;
#endif

	case MTYPE_NEWLINE:
		printf("\n");
		break;
	}
}






void mon_exec_set(void)
{
	void *var_ptr;
	int set_type, index = 0, value = 0, i, block = 0;
	double dvalue = 0.0;
	int key_flag = 0;

	set_type = 0;
	if (mon_exist_argv()) {

		for (index = 0; index < COUNTOF(monitor_variable_block); index++) {
			if (strcmp(argv.str, monitor_variable_block[index].block_name) == 0) {
				break;
			}
		}
		if (index < COUNTOF(monitor_variable_block)) {
			block = TRUE;
			set_type = index;
			mon_shift();
		} else {

			for (index = 0; index < COUNTOF(monitor_variable); index++) {
				if (strcmp(argv.str, monitor_variable[index].var_name) == 0) {
					break;
				}
			}
			if (index == COUNTOF(monitor_variable)) {
				error();
			}
			mon_shift();
			set_type = 1;
			if (mon_exist_argv()) {
				if (mon_argv_is(ARGV_INT)) {
					value  = argv.val;
					dvalue = (double)argv.val;
				} else if (mon_argv_is(ARGV_STR)) {
					if (monitor_variable[index].var_type != MTYPE_KEY) {
						char *chk;
						dvalue = strtod(argv.str, &chk);
						if (*chk != '\0') {
							error();
						}
					} else {
						if (argv.str[0] != '~') {
							error();
						}
						key_flag = 1;
					}
				} else {
					error();
				}
				mon_shift();
				set_type = 2;
			}
		}
	}

	if (mon_exist_argv()) {
		error();
	}


	/*================*/

	if (block) {
		index = 0;
		block = 0;
		for (; index < COUNTOF(monitor_variable); index++) {
			if (monitor_variable_block[set_type].start <= block &&
				block <= monitor_variable_block[set_type].end) {
				monitor_set_show_printf(index);
			}
			if (monitor_variable[index].var_type == MTYPE_NEWLINE) {
				block ++;
			}
		}
		return;
	}

	switch (set_type) {
	case 0:
		for (index = 0; index < COUNTOF(monitor_variable); index++) {
			monitor_set_show_printf(index);
		}
		break;

	case 1:
		monitor_set_show_printf(index);
		break;

	case 2:
		var_ptr = monitor_variable[index].var_ptr;
		switch (monitor_variable[index].var_type) {

		case MTYPE_INT_C:
		case MTYPE_BYTE_C:
		case MTYPE_WORD_C:
		case MTYPE_DOUBLE_C:
			printf("Sorry! This variable can't set value. \n");
			break;

		case MTYPE_INT:
			*((int *)var_ptr)  = value;
			break;
		case MTYPE_BYTE:
			*((byte *)var_ptr) = value;
			break;
		case MTYPE_WORD:
			*((word *)var_ptr) = value;
			break;
		case MTYPE_DOUBLE:
			*((double *)var_ptr) = dvalue;
			break;

		case MTYPE_FONT:
			*((int *)var_ptr) = value;
			memory_set_font();
			screen_update_immidiate();
			break;
		case MTYPE_FRAMESKIP:
			quasi88_cfg_set_frameskip_rate(value);
			break;
		case MTYPE_INTERLACE:
			quasi88_cfg_set_interlace(value);
			screen_update_immidiate();
			break;
		case MTYPE_INTERP:
			quasi88_cfg_set_interp(value);
			screen_update_immidiate();
			break;
		case MTYPE_CPUTIMING:
			*((int *)var_ptr) = value;
			emu_reset();
			break;
		case MTYPE_MEMWAIT:
			*((int *)var_ptr) = value;
			pc88main_cpu_update();
			pc88sub_cpu_update();
			break;

		case MTYPE_CLOCK:
			*((double *)var_ptr) = dvalue;
			interval_work_init_all();
			break;

		case MTYPE_BEEP:
			*((int *)var_ptr) = value;
#ifdef USE_SOUND
			xmame_dev_beep_cmd_sing((byte) use_cmdsing);
#endif
			break;


		case MTYPE_KEY:
			if (key_flag == 0) {
				for (i = 0; i < 0x10; i++) {
					key_scan[i] = value;
				}
				printf("     key_scan[0..15] = %d\n", value);
			} else {
				char c_new[16];
				char *p = &argv.str[1];
				int i, j, x;
				for (i = 0; i < (int)sizeof(c_new) && *p; i++) {
					for (j = 0; j < 2; j++) {
						x = -1;
						if ('0' <= *p && *p <= '9') {
							x = *p - '0';
						} else if ('a' <= *p && *p <= 'f') {
							x = *p - 'a' + 10;
						} else if ('A' <= *p && *p <= 'F') {
							x = *p - 'A' + 10;
						}
						p++;
						if ((*p == '\0' && j == 0) || x < 0) {
							goto set_key_break;
						}
						if (j == 0) {
							c_new[i] = x * 16;
						} else {
							c_new[i] = (c_new[i] + x);
						}
					}
				}
				if (*p) {
					goto set_key_break;
				}

				for (j = 0; j < i; j++) {
					key_scan[j] = ~c_new[j];
				}
				monitor_set_show_printf(index);
			}
			break;

		set_key_break:
			printf("Invalid parameter %s.\n", argv.str);
			break;

		case MTYPE_MEM:
		case MTYPE_PALETTE:
		case MTYPE_CRTC:
		case MTYPE_PIO:
			printf("Sorry! This variable can't set value. "
				   "(palette,crtc,pio, and so on)\n");
			break;

#ifdef USE_SOUND
		case MTYPE_VOLUME:
			monitor_set_volume(value);
			break;
		case MTYPE_FMMIXER:
			monitor_set_fmmixer(value);
			break;
		case MTYPE_PSGMIXER:
			monitor_set_psgmixer(value);
			break;
		case MTYPE_BEEPMIXER:
			monitor_set_beepmixer(value);
			break;
		case MTYPE_PCGMIXER:
			monitor_set_pcgmixer(value);
			break;
		case MTYPE_RHYTHMMIXER:
			monitor_set_rhythmmixer(value);
			break;
		case MTYPE_ADPCMMIXER:
			monitor_set_adpcmmixer(value);
			break;
#ifdef USE_FMGEN
		case MTYPE_FMGENMIXER:
			monitor_set_fmgenmixer(value);
			break;
#endif
		case MTYPE_SAMPLEMIXER:
			monitor_set_samplemixer(value);
			break;
		case MTYPE_MIXER:
			break;
#endif

		}
		break;

	}
}

void mon_help_show(int oneline)
{
	if (oneline) {
		printf(
			"show variable"
			);
	} else {
		printf(
			"  show [<variabe-name>]\n"
			"    show variables.\n"
			"    [all omit]     ... show all variable.\n"
			"    <variabe-name> ... specify variable name.\n"
			);
	}
}

void mon_exec_show(void)
{
	int set_type, index = 0;

	set_type = 0;
	if (mon_exist_argv()) {
		for (index = 0; index < COUNTOF(monitor_variable); index++) {
			if (strcmp(argv.str, monitor_variable[index].var_name) == 0) {
				break;
			}
		}
		if (index == COUNTOF(monitor_variable)) {
			error();
		}
		mon_shift();
		set_type = 1;
	}

	if (mon_exist_argv()) {
		error();
	}


	switch (set_type) {
	case 0:
		for (index = 0; index < COUNTOF(monitor_variable); index++) {
			monitor_set_show_printf(index);
		}
		break;

	case 1:
		monitor_set_show_printf(index);
		break;
	}
}



#ifdef USE_GNU_READLINE
/*--------------------------------------------------------------
 * readline
 *--------------------------------------------------------------*/
char *set_arg_generator(char *text, int state)
{
	/* set コマンドが入力済みの場合 */
	static int count, len;
	char *name;

	if (state == 0) {
		count = 0;
		len = strlen(text);
	}

	while (count < COUNTOF(monitor_variable)) {
		/* 変数名を検索 */

		name = monitor_variable[count].var_name;
		count ++;

		if (strcmp(name, "") == 0) {
			continue;
		}

		if (strncmp(name, text, len) == 0) {
			char *p = malloc(strlen(name) + 1);
			if (p) {
				strcpy(p, name);
			}
			return p;
		}
	}

	return NULL;
}
#endif /* USE_GNU_READLINE */

#endif /* USE_MONITOR */
