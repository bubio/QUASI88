#ifndef MENU_RESET_H_INCLUDED
#define MENU_RESET_H_INCLUDED

/* リセット時に要求する設定を、保存 */
extern T_RESET_CFG reset_req;

/* 起動デバイスの制御に必要 */
extern Q8tkWidget *widget_dipsw_b_boot_disk;
extern Q8tkWidget *widget_dipsw_b_boot_rom;

/* メニュー下部のリセットと、リセットタグのリセットを連動させたいので、
 * 片方が選択されたら、反対のも選択されるように、ウィジットを記憶 */
extern Q8tkWidget *widget_reset_basic[2][4];
extern Q8tkWidget *widget_reset_clock[2][2];


extern void cb_reset_now(UNUSED_WIDGET, UNUSED_PARM);
extern void set_reset_dipsw_boot(void);


/* メインページ リセット */
extern Q8tkWidget *menu_reset(void);

#endif /* MENU_RESET_H_INCLUDED */
