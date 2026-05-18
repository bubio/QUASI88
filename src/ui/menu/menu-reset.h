#ifndef MENU_RESET_H_INCLUDED
#define MENU_RESET_H_INCLUDED

/* 起動デバイスの制御に必要 */
extern Q8tkWidget *widget_dipsw_b_boot_disk;
extern Q8tkWidget *widget_dipsw_b_boot_rom;

extern void set_reset_dipsw_boot(void);


/* メインページ リセット */
extern Q8tkWidget *menu_reset(void);

#endif /* MENU_RESET_H_INCLUDED */
