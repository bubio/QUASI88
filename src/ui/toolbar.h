#ifndef TOOLBAR_H_INCLUDED
#define TOOLBAR_H_INCLUDED


extern void submenu_init(void);
extern void submenu_controll(int controll);

extern void pause_top(void);
extern void reset_top(void);
extern void quit_top(void);
extern void speedup_top(void);
extern void diskchange_top(void);

extern void toolbar_speedup_change(int rate);


#endif /* TOOLBAR_H_INCLUDED */
