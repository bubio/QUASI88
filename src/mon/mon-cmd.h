#ifndef CMD_CMD_H_INCLUDED
#define CMD_CMD_H_INCLUDED

/***************************************************************
 * 命令のテーブル
 ****************************************************************/
typedef struct {
	const char  *cmd;
	const char  *short_cmd;
	void (*help)(int oneline);
	void (*exec)(void);
	void (*init)(void);
} t_monitor_cmd;

extern t_monitor_cmd monitor_cmd[];

/***************************************************************
 * 命令別処理
 ****************************************************************/
void mon_exec_help(void);
void mon_help_help(int oneline);

void mon_exec_menu(void);
void mon_help_menu(int oneline);

void mon_help_quit(int oneline);
void mon_exec_quit(void);

void mon_exec_go(void);
void mon_help_go(int oneline);

void mon_exec_trace(void);
void mon_help_trace(int oneline);

void mon_exec_step(void);
void mon_help_step(int oneline);

void mon_exec_next(void);
void mon_help_next(int oneline);

void mon_exec_change(void);
void mon_help_change(int oneline);

void mon_exec_return(void);
void mon_help_return(int oneline);

void mon_exec_break(void);
void mon_exec_breakt(void);
void mon_help_break(int oneline);

void mon_exec_reset(void);
void mon_help_reset(int oneline);

void mon_exec_reg(void);
void mon_help_reg(int oneline);

void mon_init_disasm(void);
void mon_exec_disasm(void);
void mon_help_disasm(int oneline);

void mon_exec_fbreak(void);
void mon_help_fbreak(int oneline);

void mon_exec_read(void);
void mon_help_read(int oneline);

void mon_exec_write(void);
void mon_help_write(int oneline);

void mon_init_dump(void);
void mon_exec_dump(void);
void mon_help_dump(int oneline);

void mon_init_dumpext(void);
void mon_exec_dumpext(void);
void mon_help_dumpext(int oneline);

void mon_exec_fill(void);
void mon_help_fill(int oneline);

void mon_exec_move(void);
void mon_help_move(int oneline);

void mon_exec_search(void);
void mon_help_search(int oneline);

void mon_exec_in(void);
void mon_help_in(int oneline);

void mon_exec_out(void);
void mon_help_out(int oneline);

void mon_exec_loadmem(void);
void mon_help_loadmem(int oneline);

void mon_exec_savemem(void);
void mon_help_savemem(int oneline);

void mon_exec_text(void);
void mon_help_text(int oneline);

void mon_exec_set(void);
void mon_help_set(int oneline);

void mon_exec_show(void);
void mon_help_show(int oneline);

void mon_exec_drive(void);
void mon_help_drive(int oneline);

void mon_exec_file(void);
void mon_help_file(int oneline);

void mon_exec_tapeload(void);
void mon_help_tapeload(int oneline);

void mon_exec_tapesave(void);
void mon_help_tapesave(int oneline);

void mon_exec_printer(void);
void mon_help_printer(int oneline);

void mon_exec_serialin(void);
void mon_help_serialin(int oneline);

void mon_exec_serialout(void);
void mon_help_serialout(int oneline);

void mon_exec_textscr(void);
void mon_help_textscr(int oneline);

void mon_exec_loadbas(void);
void mon_help_loadbas(int oneline);

void mon_exec_savebas(void);
void mon_help_savebas(int oneline);

void mon_exec_redraw(void);
void mon_help_redraw(int oneline);

void mon_exec_resize(void);
void mon_help_resize(int oneline);

void mon_exec_statesave(void);
void mon_help_statesave(int oneline);

void mon_exec_stateload(void);
void mon_help_stateload(int oneline);

void mon_exec_snapshot(void);
void mon_help_snapshot(int oneline);

void mon_exec_loadfont(void);
void mon_help_loadfont(int oneline);

void mon_exec_savefont(void);
void mon_help_savefont(int oneline);

void mon_exec_misc(void);
void mon_help_misc(int oneline);



#ifdef USE_GNU_READLINE
char *set_arg_generator(char *text, int state);
void initialize_readline(void);
#endif

void mon_command(void);


/***************************************************************
 * モニター変数
 ****************************************************************/
typedef struct {
	struct {
		int style;
		int line;
	} reg;

	struct {
		int addr;
		int size;
		int bank;
		byte data;
	} search;

	struct {
		int cpu;
		int addr[2];
	} disasm;

	struct {
		int addr;
		int bank;
	} dump;

	struct {
		int addr;
		int bank;
		int board;
	} dumpext;
} t_monitor_var;

extern t_monitor_var mon;
extern void mon_var_init(void);


#endif /* CMD_CMD_H_INCLUDED */
