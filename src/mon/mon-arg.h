#ifndef CMD_ARG_H_INCLUDED
#define CMD_ARG_H_INCLUDED

enum ArgvType {
	ARGV_END      = 0x0000000,
	ARGV_STR      = 0x0000001,				/* strings                 */
	ARGV_PORT     = 0x0000002,				/* 0..0xff                 */
	ARGV_ADDR     = 0x0000004,				/* 0..0xffff               */
	ARGV_NUM      = 0x0000008,				/* 0..0x7fffffff           */
	ARGV_INT      = 0x0000010,				/* -0x7fffffff..0x7fffffff */
	ARGV_DRV      = 0x0000020,				/* 1..2                    */
	ARGV_IMG      = 0x0000040,				/* 1..MAX_NR_IMAGE         */
	ARGV_SIZE     = 0x0000080,				/* #1..#0x7fffffff         */
	ARGV_CPU      = 0x0000100,				/* CpuName                 */
	ARGV_BANK     = 0x0000200,				/* MemoryName              */
	ARGV_REG      = 0x0000400,				/* RegisterName            */
	ARGV_BREAK    = 0x0000800,				/* BreakAction             */
	ARGV_BASMODE  = 0x0001000,				/* ResetCommand            */
	ARGV_CKMODE   = 0x0002000,				/* ResetCommand            */
	ARGV_SDMODE   = 0x0004000,				/* ResetCommand            */
	ARGV_CHANGE   = 0x0008000,				/* TraceCommand            */
	ARGV_STEP     = 0x0010000,				/* StepCommand             */
	ARGV_ALL      = 0x0020000,				/* RegCommand              */
	ARGV_RESIZE   = 0x0040000,				/* ResizeCommand           */
	ARGV_FILE     = 0x0080000,				/* FileCommand             */
	ARGV_DRIVE    = 0x0100000,				/* DriveCommand            */
	ARGV_FBREAK   = 0x0200000,				/* FBreakAction            */
	ARGV_BASIC    = 0x0400000,				/* BasicCodeType           */
	ARGV_SNAPSHOT = 0x0800000,				/* SnapshotFormatType      */

	EndofArgvType
};

enum ArgvName {

	/* <cpu> */
	ARG_MAIN,
	ARG_SUB,

	/* <bank> */
	/*ARG_MAIN,*/
	ARG_ROM,		ARG_RAM,		ARG_N,			ARG_HIGH,
	ARG_EXT0,		ARG_EXT1,		ARG_EXT2,		ARG_EXT3,
	ARG_B,			ARG_R,			ARG_G,			ARG_PCG,
	/*ARG_SUB*/

	/* <reg> */
	ARG_AF,			ARG_BC,			ARG_DE,			ARG_HL,
	ARG_IX,			ARG_IY,			ARG_SP,			ARG_PC,
	ARG_AF1,		ARG_BC1,		ARG_DE1,		ARG_HL1,
	ARG_I,			/*ARG_R,*/
	ARG_IFF,		ARG_IM,			ARG_HALT,		ARG_CK,
	ARG_A,			ARG_F,			/*ARG_B,*/		ARG_C,
	ARG_D,			ARG_E,			ARG_H,			ARG_L,
	ARG_A1,			ARG_F1,			ARG_B1,			ARG_C1,
	ARG_D1,			ARG_E1,			ARG_H1,			ARG_L1,
	ARG_IXH,		ARG_IXL,		ARG_IYH,		ARG_IYL,

	/* <action>*/
	/*ARG_PC,*/		ARG_READ,		ARG_WRITE,		ARG_IN,
	ARG_OUT,		ARG_DIAG,		ARG_CLEARALL,
	ARG_CLEAR,		ARG_ENABLE,		ARG_DISABLE,	ARG_TEMPORARY,
	ARG_LASTING,
	ARG_P,			/*ARG_R,*/		ARG_W,			/*ARG_I,*/
	ARG_O,			
	/*ARG_C,*/		/*ARG_E,*/		/*ARG_D,*/		ARG_T,
	/*ARG_L,*/

	/* <mode> */
	ARG_V2,			ARG_V1H,		ARG_V1S,		/*ARG_N,*/
	ARG_8MHZ,		ARG_4MHZ,		ARG_SD,			ARG_SD2,

	/* trace change */
	ARG_CHANGE,
	ARG_SET,		ARG_UNSET,

	/* step <cmd> */
	/*ARG_ALL*/
	ARG_CALL,		ARG_JP,			ARG_REP,

	/* reg all */
	ARG_ALL,

	/* resize <arg> */
	ARG_FULL,		ARG_HALF,		ARG_DOUBLE,
	ARG_FULLSCREEN,	ARG_WINDOW,

	/* drive <cmd> */
	ARG_SHOW,		ARG_EJECT,
	ARG_EMPTY,		/*ARG_SET,*/

	/* file <cmd> */
	/*ARG_SHOW,*/
	ARG_CREATE,		ARG_RENAME,
	ARG_PROTECT,	ARG_UNPROTECT,
	ARG_FORMAT,		ARG_UNFORMAT,

	/* savebas <type> */
	ARG_BINARY,		ARG_ASCII,

	/* snapshot <fmt> */
	ARG_BMP,		ARG_PPM,		ARG_RAW,

	EndofArgName
};

enum SetType {
	MTYPE_NEWLINE,
	MTYPE_INT,		MTYPE_BYTE,		MTYPE_WORD,		MTYPE_DOUBLE,
	MTYPE_INT_C,	MTYPE_BYTE_C,	MTYPE_WORD_C,	MTYPE_DOUBLE_C,
	MTYPE_KEY,		MTYPE_PALETTE,	MTYPE_CRTC,		MTYPE_PIO,
	MTYPE_MEM,		MTYPE_FONT,		MTYPE_FRAMESKIP,	MTYPE_INTERLACE,
	MTYPE_INTERP,		MTYPE_CLOCK,    MTYPE_BEEP,
	MTYPE_CPUTIMING,	MTYPE_MEMWAIT,
	MTYPE_VOLUME,		MTYPE_FMMIXER,	MTYPE_PSGMIXER,	MTYPE_BEEPMIXER,
	MTYPE_PCGMIXER, MTYPE_RHYTHMMIXER,	MTYPE_ADPCMMIXER, MTYPE_FMGENMIXER,
	MTYPE_SAMPLEMIXER,	MTYPE_MIXER,
	EndofTYPE
};


byte mon_peek_memory(int bank, word addr);
void mon_poke_memory(int bank, word addr, byte data);

#define MON_MAX_ARGS		(8)
#define MON_MAX_CHRS		(256)

extern char d_buf[MON_MAX_CHRS];
extern int  d_argc;
extern char *d_argv[MON_MAX_ARGS];

void mon_getarg(void);


typedef struct {
	int         type;					/* 引数の種類   ARGV_xxx */
	int         val;					/* 引数の値     ARG_xxx または、数 */
	char        *str;					/* 引数の文字列 d_argv[xxx]と同じ */
} t_mon_argv;
extern t_mon_argv argv;


void mon_shift(void);
int  mon_exist_argv(void);
int  mon_argv_is(int type);
char *mon_argv2str(int argv_val);
void mon_error(void);

/*==============================================================
 * エラー関連
 *==============================================================*/
#define error()														\
		do {														\
			mon_error();											\
			return;													\
		} while(0)


#endif /* CMD_ARG_H_INCLUDED */
