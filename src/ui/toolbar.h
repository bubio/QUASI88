#ifndef TOOLBAR_H_INCLUDED
#define TOOLBAR_H_INCLUDED


extern void toolbar_init(void);
extern void toolbar_controll(int controll);

extern void askreset_top(void);
extern void askquit_top(void);
extern void askspeedup_top(void);
extern void askopenfile_top(void);
extern void askselectdisk_top(void);
extern void askstatefile_top(void);

extern void toolbar_speedup_change(int rate);

extern char *toolbar_layout;

#endif /* TOOLBAR_H_INCLUDED */
