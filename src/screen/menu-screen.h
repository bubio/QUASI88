#ifndef MENU_SCREEN_H_INCLUDED
#define MENU_SCREEN_H_INCLUDED


/*--------------------------------------------------------------
 * パレットに設定するカラー
 *--------------------------------------------------------------*/

#define MENU_COLOR_CLASSIC_FOREGROUND	(0x000000)
#define MENU_COLOR_CLASSIC_BACKGROUND	(0xd6d6d6)
#define MENU_COLOR_CLASSIC_LIGHT		(0xffffff)
#define MENU_COLOR_CLASSIC_SHADOW		(0x000000)
#define MENU_COLOR_CLASSIC_FONT_FG		(0x00009c)
#define MENU_COLOR_CLASSIC_FONT_BG		(0xffffff)
#define MENU_COLOR_CLASSIC_SCALE_SLD	(0xf0f0f0)
#define MENU_COLOR_CLASSIC_SCALE_BAR	(0xb0b0b0)

#define MENU_COLOR_LIGHT_FOREGROUND		(0x500000)
#define MENU_COLOR_LIGHT_BACKGROUND		(0xfdf5e6)
#define MENU_COLOR_LIGHT_LIGHT			(0xd3d3d3)
#define MENU_COLOR_LIGHT_SHADOW			(0x606060)
#define MENU_COLOR_LIGHT_FONT_FG		(0x4169e1)
#define MENU_COLOR_LIGHT_FONT_BG		(0xf7f7f7)
#define MENU_COLOR_LIGHT_SCALE_SLD		(0xf0f0f0)
#define MENU_COLOR_LIGHT_SCALE_BAR		(0xdcdcdc)

#define MENU_COLOR_DARK_FOREGROUND		(0xe0e0e0)
#define MENU_COLOR_DARK_BACKGROUND		(0x303030)
#define MENU_COLOR_DARK_LIGHT			(0x606060)
#define MENU_COLOR_DARK_SHADOW			(0xffffff)
#define MENU_COLOR_DARK_FONT_FG			(0xffffff)
#define MENU_COLOR_DARK_FONT_BG			(0x500050)
#define MENU_COLOR_DARK_SCALE_SLD		(0xb97a57)
#define MENU_COLOR_DARK_SCALE_BAR		(0x533c1c)



/*--------------------------------------------------------------
 *
 *--------------------------------------------------------------*/

extern  byte    menu_cursor_on[16];
extern  byte    menu_cursor_off[16];



#endif  /* MENU_SCREEN_H_INCLUDED */
