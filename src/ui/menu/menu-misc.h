#ifndef MENU_MISC_H_INCLUDED
#define MENU_MISC_H_INCLUDED

/*===========================================================================
 * ディスク挿入 & 排出
 *===========================================================================*/
extern void sub_misc_suspend_update(void);
extern void sub_misc_snapshot_update(void);
extern void sub_misc_waveout_update(void);


/* メインページ その他 */
extern Q8tkWidget *menu_misc(void);

#endif /* MENU_MISC_H_INCLUDED */
