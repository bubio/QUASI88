#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED


#include <SDL.h>
#include <graph.h>


/*
 * src/sysdepend/sdl2/ 以下でのグローバル変数
 */
extern int sdl_mouse_rel_move;			/* マウス相対移動量検知可能か */
extern int sdl_repeat_on;				/* キーリピートONか */

extern Uint32 sdl_window_id;
extern int sdl_fullscreen;
extern int sdl_width;
extern int sdl_height;
extern int sdl_buildin_touchkey;
extern T_GRAPH_RECT sdl_area_main;
extern T_GRAPH_RECT sdl_area_touch;
extern int sdl_req_mouse_show;
extern int sdl_in_touchkey_area;
extern int key_width;
extern int key_height;
extern int key_nr_finger_clicks;

extern int separate_touchkey;



/*
 * src/sysdepend/sdl2/ 以下でのグローバル変数 (オプション設定可能な変数)
 */
extern int use_software_rendering;		/*SDL_RENDERER_SOFTWARE を使う       */
extern int use_desktop_fullscreen;		/*SDL_WINDOW_FULLSCREEN_DESKTOPを使う*/

extern int use_ime;						/* メニューでIMEを使えるようにする   */

extern int enable_touchkey;		/* タッチキーボードの可否 */
extern int touchkey_fit;		/* 全画面の幅いっぱいに広げるか否か */
extern int touchkey_overlap;	/* メイン部との重なり度合 0:なし .. 100:全て */
extern int touchkey_reduce;		/* タッチキーの縮小度合   0:なし .. 90:90%減 */
extern int touchkey_opaque;		/* 重なった時のアルファ値 0:透明 .. 255:なし */

extern int use_menukey;					/* 左Win/左Cmdキーでメニューへ遷移   */
extern int keyboard_type;				/* キーボードの種類                  */
extern char *file_keyboard;				/* キー設定ファイル名                */
extern int use_joydevice;				/* ジョイスティックデバイスを開く?   */



/*
 * src/sysdepend/sdl2/ 以下でのグローバル関数
 */
int sdl2_init(void);
void sdl2_exit(void);

void sdl2_set_icon(SDL_Window *);

int  sdl2_touchkey_init(SDL_Renderer *renderer);
void sdl2_touchkey_exit(void);
void sdl2_touchkey(int request);
void sdl2_touchkey_hide(int do_resize);
int  sdl2_touchkey_need_rendering(void);
void sdl2_touchkey_rendering(int no, SDL_Renderer *renderer, SDL_Rect *dstrect,
							 int with_keybase, Uint8 alpha);
void sdl2_touchkey_draw(void);

void touchkey_cfg_overlap(int delta);
void touchkey_cfg_recude(int delta);
void touchkey_cfg_fit(int do_fit);
void touchkey_cfg_orientation(Uint32 display);
void touchkey_cfg_opaque(int delta);

void touchkey_mouse_down(int button_x, int button_y, int width, int height,
						 int clicks);
void touchkey_mouse_move(int motion_x, int motion_y, int width, int height);
void touchkey_mouse_up(void);
void touchkey_finger_down(SDL_TouchID touchId, SDL_FingerID fingerId,
						  int finger_x, int finger_y, int width, int height,
						  int clicks);
void touchkey_finger_move(SDL_TouchID touchId, SDL_FingerID fingerId,
						  int finger_x, int finger_y, int width, int height);
void touchkey_finger_up(SDL_TouchID touchId, SDL_FingerID fingerId);
int touchkey_config_action(const SDL_Event *E);

enum {
	M_F_EVENT_IGNORE	= 0,
	M_F_EVENT_IGNORE2,
	M_F_EVENT_EMU_MENU,
	M_F_EVENT_TOUCHKEY,
};
int get_mouse_finger_event_in_main_window(const SDL_Event *E);
int get_mouse_finger_event_in_touchkey_window(const SDL_Event *E);
void action_window_event_in_touchkey_window(const SDL_Event *E);


#endif /* DEVICE_H_INCLUDED */
