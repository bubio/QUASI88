/************************************************************************/
/*                                                                      */
/* 画面の表示           16bpp                                           */
/*                                                                      */
/************************************************************************/

#include <string.h>

#include "quasi88.h"
#include "screen.h"
#include "screen-common.h"
#include "screen-func.h"
#include "crtcdmac.h"
#include "memory.h"
#include "q8tk.h"


#ifdef SUPPORT_16BPP

#define TYPE					bit16

#define SCREEN_WIDTH			WIDTH
#define SCREEN_HEIGHT			HEIGHT
#define SCREEN_TOP				screen_buf
#define SCREEN_START			screen_user_start

#define COLOR_PIXEL(x)			(TYPE) color_pixel[ x ]
#define MIXED_PIXEL(a,b)		(TYPE) color_half_pixel[ a ][ b ]
#define MENU_COLOR_PIXEL(x)		(TYPE) menu_pixel[ x ]
#define MENU_MIXED_PIXEL(a,b)	(TYPE) menu_half_pixel[ a ][ b ]
#define BLACK					(TYPE) black_pixel

/*===========================================================================
 * 等倍サイズ
 *===========================================================================*/

/*----------------------------------------------------------------------
 *                      ● 200ライン                    標準
 *----------------------------------------------------------------------*/
#define	NORMAL

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_F_N_16
#include										"screen-vram-full.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_F_N_16
#include										"screen-vram-full.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_F_N_16
#include										"screen-vram-full.h"
#undef	UNDISP

#undef	NORMAL
/*----------------------------------------------------------------------
 *						● 200ライン					ラインスキップ
 *----------------------------------------------------------------------*/
#define	SKIPLINE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_F_S_16
#include										"screen-vram-full.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_F_S_16
#include										"screen-vram-full.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_F_S_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_F_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_F_S_16
#include										"screen-vram-full.h"
#undef	UNDISP

#undef	SKIPLINE
/*----------------------------------------------------------------------
 *						● 200ライン					インターレース
 *----------------------------------------------------------------------*/
#define	INTERLACE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_F_I_16
#include										"screen-vram-full.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_F_I_16
#include										"screen-vram-full.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_F_I_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_F_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_F_I_16
#include										"screen-vram-full.h"
#undef	UNDISP

#undef	INTERLACE
/*----------------------------------------------------------------------
 *						● 400ライン					標準
 *----------------------------------------------------------------------*/
#define	HIRESO											/* 白黒  640x400 */

#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x20_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x25_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x25_F_N_16
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x20_F_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x20_F_N_16
#include										"screen-vram-full.h"

#undef	HIRESO

/*===========================================================================
 * 半分サイズ
 *===========================================================================*/

/*----------------------------------------------------------------------
 *						● 200ライン					標準
 *----------------------------------------------------------------------*/
#define	NORMAL

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_H_N_16
#include										"screen-vram-half.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_H_N_16
#include										"screen-vram-half.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */

#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_H_N_16
#include										"screen-vram-half.h"
#undef	UNDISP

#undef	NORMAL
/*----------------------------------------------------------------------
 *						● 200ライン					色補完
 *----------------------------------------------------------------------*/
#define	INTERPOLATE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_H_P_16
#include										"screen-vram-half.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_H_P_16
#include										"screen-vram-half.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_H_P_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_H_P_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_H_P_16
#include										"screen-vram-half.h"
#undef	UNDISP

#undef	INTERPOLATE
/*----------------------------------------------------------------------
 *						● 400ライン					標準
 *----------------------------------------------------------------------*/
#define	HIRESO											/* 白黒  640x400 */

#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x20_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x25_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x25_H_N_16
#include										"screen-vram-half.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x20_H_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x20_H_N_16
#include										"screen-vram-half.h"

#undef	HIRESO

/*===========================================================================
 * 二倍サイズ
 *===========================================================================*/
#ifdef	SUPPORT_DOUBLE
/*----------------------------------------------------------------------
 *						● 200ライン					標準
 *----------------------------------------------------------------------*/
#define NORMAL

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_N_16
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_N_16
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_N_16
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	NORMAL
/*----------------------------------------------------------------------
 *						● 200ライン					ラインスキップ
 *----------------------------------------------------------------------*/
#define	SKIPLINE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_S_16
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_S_16
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_S_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_S_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_S_16
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	SKIPLINE
/*----------------------------------------------------------------------
 *						● 200ライン					インターレース
 *----------------------------------------------------------------------*/
#define	INTERLACE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_I_16
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_I_16
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_I_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_I_16
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_I_16
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	INTERLACE
/*----------------------------------------------------------------------
 *						● 400ライン					標準
 *----------------------------------------------------------------------*/
#define	HIRESO											/* 白黒  640x400 */

#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H80x20_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x25_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x25_D_N_16
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x20_D_N_16
#define			VRAM2SCREEN_ALL					v2s_all_H40x20_D_N_16
#include										"screen-vram-double.h"
#undef	HIRESO

#endif	/* SUPPORT_DOUBLE */


#define DIRECT
/*===========================================================================
 * 等倍サイズ
 *===========================================================================*/

/*----------------------------------------------------------------------
 *						● 200ライン					標準
 *----------------------------------------------------------------------*/
#define	NORMAL

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_F_N_16_d
#include										"screen-vram-full.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_F_N_16_d
#include										"screen-vram-full.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_F_N_16_d
#include										"screen-vram-full.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_F_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_F_N_16_d
#include										"screen-vram-full.h"
#undef	UNDISP

#undef	NORMAL

/*===========================================================================
 * 二倍サイズ
 *===========================================================================*/
#ifdef	SUPPORT_DOUBLE
/*----------------------------------------------------------------------
 *						● 200ライン					標準
 *----------------------------------------------------------------------*/
#define	NORMAL

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_N_16_d
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_N_16_d
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_N_16_d
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	NORMAL
/*----------------------------------------------------------------------
 *						● 200ライン					ラインスキップ
 *----------------------------------------------------------------------*/
#define	SKIPLINE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_S_16_d
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_S_16_d
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_S_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_S_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_S_16_d
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	SKIPLINE
/*----------------------------------------------------------------------
 *						● 200ライン					インターレース
 *----------------------------------------------------------------------*/
#define	INTERLACE

#define	COLOR											/* カラー640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C80x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C80x20_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_C40x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_C40x20_D_I_16_d
#include										"screen-vram-double.h"
#undef	COLOR

#define	MONO											/* 白黒  640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M80x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M80x20_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_M40x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_M40x20_D_I_16_d
#include										"screen-vram-double.h"
#undef	MONO

#define	UNDISP											/* 非表示640x200 */
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U80x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U80x20_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x25_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x25_D_I_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_U40x20_D_I_16_d
#define			VRAM2SCREEN_ALL					v2s_all_U40x20_D_I_16_d
#include										"screen-vram-double.h"
#undef	UNDISP

#undef	INTERLACE
/*----------------------------------------------------------------------
 *						● 400ライン					標準
 *----------------------------------------------------------------------*/
#define	HIRESO											/* 白黒  640x400 */

#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_H80x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		80
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H80x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_H80x20_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				25
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x25_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_H40x25_D_N_16_d
#include										"screen-vram-double.h"
#define			TEXT_WIDTH		40
#define			TEXT_HEIGHT				20
#define			VRAM2SCREEN_DIFF				v2s_dif_H40x20_D_N_16_d
#define			VRAM2SCREEN_ALL					v2s_all_H40x20_D_N_16_d
#include										"screen-vram-double.h"
#undef	HIRESO

#endif	/* SUPPORT_DOUBLE */
#undef	DIRECT


/*===========================================================================
 * 画面消去
 *===========================================================================*/

#define			SCREEN_BUF_INIT					screen_buf_init_16
#include										"screen-vram-clear.h"


/*===========================================================================
 * メニュー画面
 *===========================================================================*/

#define			MENU_W			Q8GR_TVRAM_USER_W
#define			MENU_H			Q8GR_TVRAM_USER_H
#define			MENU_X			Q8GR_TVRAM_USER_X
#define			MENU_Y			Q8GR_TVRAM_USER_Y
#define			MENU_START		screen_user_start
#define			MENU_DECORATE	0
#define			MENU2SCREEN						menu2screen_F_N_16
#include										"screen-menu-full.h"

#define			MENU_W			Q8GR_TVRAM_TOOL_W
#define			MENU_H			Q8GR_TVRAM_TOOL_H
#define			MENU_X			Q8GR_TVRAM_TOOL_X
#define			MENU_Y			Q8GR_TVRAM_TOOL_Y
#define			MENU_START		screen_tool_start
#define			MENU_DECORATE	1
#define			MENU2SCREEN						tool2screen_F_N_16
#include										"screen-menu-full.h"

#define			MENU_W			Q8GR_TVRAM_STAT_W
#define			MENU_H			Q8GR_TVRAM_STAT_H
#define			MENU_X			Q8GR_TVRAM_STAT_X
#define			MENU_Y			Q8GR_TVRAM_STAT_Y
#define			MENU_START		screen_stat_start
#define			MENU_DECORATE	2
#define			MENU2SCREEN						stat2screen_F_N_16
#include										"screen-menu-full.h"

#define			MENU_W			Q8GR_TVRAM_USER_W
#define			MENU_H			Q8GR_TVRAM_USER_H
#define			MENU_X			Q8GR_TVRAM_USER_X
#define			MENU_Y			Q8GR_TVRAM_USER_Y
#define			MENU_START		screen_user_start
#define			MENU_DECORATE	0
#define			MENU2SCREEN						menu2screen_H_N_16
#include										"screen-menu-half.h"

#define			MENU_W			Q8GR_TVRAM_TOOL_W
#define			MENU_H			Q8GR_TVRAM_TOOL_H
#define			MENU_X			Q8GR_TVRAM_TOOL_X
#define			MENU_Y			Q8GR_TVRAM_TOOL_Y
#define			MENU_START		screen_tool_start
#define			MENU_DECORATE	1
#define			MENU2SCREEN						tool2screen_H_N_16
#include										"screen-menu-half.h"

#define			MENU_W			Q8GR_TVRAM_STAT_W
#define			MENU_H			Q8GR_TVRAM_STAT_H
#define			MENU_X			Q8GR_TVRAM_STAT_X
#define			MENU_Y			Q8GR_TVRAM_STAT_Y
#define			MENU_START		screen_stat_start
#define			MENU_DECORATE	2
#define			MENU2SCREEN						stat2screen_H_N_16
#include										"screen-menu-half.h"

#define			MENU_W			Q8GR_TVRAM_USER_W
#define			MENU_H			Q8GR_TVRAM_USER_H
#define			MENU_X			Q8GR_TVRAM_USER_X
#define			MENU_Y			Q8GR_TVRAM_USER_Y
#define			MENU_START		screen_user_start
#define			MENU_DECORATE	0
#define			MENU2SCREEN						menu2screen_H_P_16
#include										"screen-menu-half-p.h"

#define			MENU_W			Q8GR_TVRAM_TOOL_W
#define			MENU_H			Q8GR_TVRAM_TOOL_H
#define			MENU_X			Q8GR_TVRAM_TOOL_X
#define			MENU_Y			Q8GR_TVRAM_TOOL_Y
#define			MENU_START		screen_tool_start
#define			MENU_DECORATE	1
#define			MENU2SCREEN						tool2screen_H_P_16
#include										"screen-menu-half-p.h"

#define			MENU_W			Q8GR_TVRAM_STAT_W
#define			MENU_H			Q8GR_TVRAM_STAT_H
#define			MENU_X			Q8GR_TVRAM_STAT_X
#define			MENU_Y			Q8GR_TVRAM_STAT_Y
#define			MENU_START		screen_stat_start
#define			MENU_DECORATE	2
#define			MENU2SCREEN						stat2screen_H_P_16
#include										"screen-menu-half-p.h"

#ifdef	SUPPORT_DOUBLE
#define			MENU_W			Q8GR_TVRAM_USER_W
#define			MENU_H			Q8GR_TVRAM_USER_H
#define			MENU_X			Q8GR_TVRAM_USER_X
#define			MENU_Y			Q8GR_TVRAM_USER_Y
#define			MENU_START		screen_user_start
#define			MENU_DECORATE	0
#define			MENU2SCREEN						menu2screen_D_N_16
#include										"screen-menu-double.h"

#define			MENU_W			Q8GR_TVRAM_TOOL_W
#define			MENU_H			Q8GR_TVRAM_TOOL_H
#define			MENU_X			Q8GR_TVRAM_TOOL_X
#define			MENU_Y			Q8GR_TVRAM_TOOL_Y
#define			MENU_START		screen_tool_start
#define			MENU_DECORATE	1
#define			MENU2SCREEN						tool2screen_D_N_16
#include										"screen-menu-double.h"

#define			MENU_W			Q8GR_TVRAM_STAT_W
#define			MENU_H			Q8GR_TVRAM_STAT_H
#define			MENU_X			Q8GR_TVRAM_STAT_X
#define			MENU_Y			Q8GR_TVRAM_STAT_Y
#define			MENU_START		screen_stat_start
#define			MENU_DECORATE	2
#define			MENU2SCREEN						stat2screen_D_N_16
#include										"screen-menu-double.h"
#endif


#undef	TYPE			/* bit16 */












/* ========================================================================= */
/* 等倍サイズ - 標準 */

int (*vram2screen_list_F_N_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_F_N_16, v2s_all_C80x25_F_N_16 },
		{ v2s_dif_C80x20_F_N_16, v2s_all_C80x20_F_N_16 },
		{ v2s_dif_C40x25_F_N_16, v2s_all_C40x25_F_N_16 },
		{ v2s_dif_C40x20_F_N_16, v2s_all_C40x20_F_N_16 },
	},
	{
		{ v2s_dif_M80x25_F_N_16, v2s_all_M80x25_F_N_16 },
		{ v2s_dif_M80x20_F_N_16, v2s_all_M80x20_F_N_16 },
		{ v2s_dif_M40x25_F_N_16, v2s_all_M40x25_F_N_16 },
		{ v2s_dif_M40x20_F_N_16, v2s_all_M40x20_F_N_16 },
	},
	{
		{ v2s_dif_U80x25_F_N_16, v2s_all_U80x25_F_N_16 },
		{ v2s_dif_U80x20_F_N_16, v2s_all_U80x20_F_N_16 },
		{ v2s_dif_U40x25_F_N_16, v2s_all_U40x25_F_N_16 },
		{ v2s_dif_U40x20_F_N_16, v2s_all_U40x20_F_N_16 },
	},
	{
		{ v2s_dif_H80x25_F_N_16, v2s_all_H80x25_F_N_16 },
		{ v2s_dif_H80x20_F_N_16, v2s_all_H80x20_F_N_16 },
		{ v2s_dif_H40x25_F_N_16, v2s_all_H40x25_F_N_16 },
		{ v2s_dif_H40x20_F_N_16, v2s_all_H40x20_F_N_16 },
	},
};

/* 等倍サイズ - スキップライン */

int (*vram2screen_list_F_S_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_F_S_16, v2s_all_C80x25_F_S_16 },
		{ v2s_dif_C80x20_F_S_16, v2s_all_C80x20_F_S_16 },
		{ v2s_dif_C40x25_F_S_16, v2s_all_C40x25_F_S_16 },
		{ v2s_dif_C40x20_F_S_16, v2s_all_C40x20_F_S_16 },
	},
	{
		{ v2s_dif_M80x25_F_S_16, v2s_all_M80x25_F_S_16 },
		{ v2s_dif_M80x20_F_S_16, v2s_all_M80x20_F_S_16 },
		{ v2s_dif_M40x25_F_S_16, v2s_all_M40x25_F_S_16 },
		{ v2s_dif_M40x20_F_S_16, v2s_all_M40x20_F_S_16 },
	},
	{
		{ v2s_dif_U80x25_F_S_16, v2s_all_U80x25_F_S_16 },
		{ v2s_dif_U80x20_F_S_16, v2s_all_U80x20_F_S_16 },
		{ v2s_dif_U40x25_F_S_16, v2s_all_U40x25_F_S_16 },
		{ v2s_dif_U40x20_F_S_16, v2s_all_U40x20_F_S_16 },
	},
	{
		{ v2s_dif_H80x25_F_N_16, v2s_all_H80x25_F_N_16 },
		{ v2s_dif_H80x20_F_N_16, v2s_all_H80x20_F_N_16 },
		{ v2s_dif_H40x25_F_N_16, v2s_all_H40x25_F_N_16 },
		{ v2s_dif_H40x20_F_N_16, v2s_all_H40x20_F_N_16 },
	},
};

/* 等倍サイズ - インターレース */

int (*vram2screen_list_F_I_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_F_I_16, v2s_all_C80x25_F_I_16 },
		{ v2s_dif_C80x20_F_I_16, v2s_all_C80x20_F_I_16 },
		{ v2s_dif_C40x25_F_I_16, v2s_all_C40x25_F_I_16 },
		{ v2s_dif_C40x20_F_I_16, v2s_all_C40x20_F_I_16 },
	},
	{
		{ v2s_dif_M80x25_F_I_16, v2s_all_M80x25_F_I_16 },
		{ v2s_dif_M80x20_F_I_16, v2s_all_M80x20_F_I_16 },
		{ v2s_dif_M40x25_F_I_16, v2s_all_M40x25_F_I_16 },
		{ v2s_dif_M40x20_F_I_16, v2s_all_M40x20_F_I_16 },
	},
	{
		{ v2s_dif_U80x25_F_I_16, v2s_all_U80x25_F_I_16 },
		{ v2s_dif_U80x20_F_I_16, v2s_all_U80x20_F_I_16 },
		{ v2s_dif_U40x25_F_I_16, v2s_all_U40x25_F_I_16 },
		{ v2s_dif_U40x20_F_I_16, v2s_all_U40x20_F_I_16 },
	},
	{
		{ v2s_dif_H80x25_F_N_16, v2s_all_H80x25_F_N_16 },
		{ v2s_dif_H80x20_F_N_16, v2s_all_H80x20_F_N_16 },
		{ v2s_dif_H40x25_F_N_16, v2s_all_H40x25_F_N_16 },
		{ v2s_dif_H40x20_F_N_16, v2s_all_H40x20_F_N_16 },
	},
};

/* ========================================================================= */
/* 半分サイズ - 標準 */

int (*vram2screen_list_H_N_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_H_N_16, v2s_all_C80x25_H_N_16 },
		{ v2s_dif_C80x20_H_N_16, v2s_all_C80x20_H_N_16 },
		{ v2s_dif_C40x25_H_N_16, v2s_all_C40x25_H_N_16 },
		{ v2s_dif_C40x20_H_N_16, v2s_all_C40x20_H_N_16 },
	},
	{
		{ v2s_dif_M80x25_H_N_16, v2s_all_M80x25_H_N_16 },
		{ v2s_dif_M80x20_H_N_16, v2s_all_M80x20_H_N_16 },
		{ v2s_dif_M40x25_H_N_16, v2s_all_M40x25_H_N_16 },
		{ v2s_dif_M40x20_H_N_16, v2s_all_M40x20_H_N_16 },
	},
	{
		{ v2s_dif_U80x25_H_N_16, v2s_all_U80x25_H_N_16 },
		{ v2s_dif_U80x20_H_N_16, v2s_all_U80x20_H_N_16 },
		{ v2s_dif_U40x25_H_N_16, v2s_all_U40x25_H_N_16 },
		{ v2s_dif_U40x20_H_N_16, v2s_all_U40x20_H_N_16 },
	},
	{
		{ v2s_dif_H80x25_H_N_16, v2s_all_H80x25_H_N_16 },
		{ v2s_dif_H80x20_H_N_16, v2s_all_H80x20_H_N_16 },
		{ v2s_dif_H40x25_H_N_16, v2s_all_H40x25_H_N_16 },
		{ v2s_dif_H40x20_H_N_16, v2s_all_H40x20_H_N_16 },
	},
};

/* 半分サイズ - 色補完 */

int (*vram2screen_list_H_P_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_H_P_16, v2s_all_C80x25_H_P_16 },
		{ v2s_dif_C80x20_H_P_16, v2s_all_C80x20_H_P_16 },
		{ v2s_dif_C40x25_H_P_16, v2s_all_C40x25_H_P_16 },
		{ v2s_dif_C40x20_H_P_16, v2s_all_C40x20_H_P_16 },
	},
	{
		{ v2s_dif_M80x25_H_P_16, v2s_all_M80x25_H_P_16 },
		{ v2s_dif_M80x20_H_P_16, v2s_all_M80x20_H_P_16 },
		{ v2s_dif_M40x25_H_P_16, v2s_all_M40x25_H_P_16 },
		{ v2s_dif_M40x20_H_P_16, v2s_all_M40x20_H_P_16 },
	},
	{
		{ v2s_dif_U80x25_H_P_16, v2s_all_U80x25_H_P_16 },
		{ v2s_dif_U80x20_H_P_16, v2s_all_U80x20_H_P_16 },
		{ v2s_dif_U40x25_H_P_16, v2s_all_U40x25_H_P_16 },
		{ v2s_dif_U40x20_H_P_16, v2s_all_U40x20_H_P_16 },
	},
	{
		{ v2s_dif_H80x25_H_N_16, v2s_all_H80x25_H_N_16 },
		{ v2s_dif_H80x20_H_N_16, v2s_all_H80x20_H_N_16 },
		{ v2s_dif_H40x25_H_N_16, v2s_all_H40x25_H_N_16 },
		{ v2s_dif_H40x20_H_N_16, v2s_all_H40x20_H_N_16 },
	},
};

/* ========================================================================= */
#ifdef SUPPORT_DOUBLE
/* 二倍サイズ - 標準 */

int (*vram2screen_list_D_N_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_N_16, v2s_all_C80x25_D_N_16 },
		{ v2s_dif_C80x20_D_N_16, v2s_all_C80x20_D_N_16 },
		{ v2s_dif_C40x25_D_N_16, v2s_all_C40x25_D_N_16 },
		{ v2s_dif_C40x20_D_N_16, v2s_all_C40x20_D_N_16 },
	},
	{
		{ v2s_dif_M80x25_D_N_16, v2s_all_M80x25_D_N_16 },
		{ v2s_dif_M80x20_D_N_16, v2s_all_M80x20_D_N_16 },
		{ v2s_dif_M40x25_D_N_16, v2s_all_M40x25_D_N_16 },
		{ v2s_dif_M40x20_D_N_16, v2s_all_M40x20_D_N_16 },
	},
	{
		{ v2s_dif_U80x25_D_N_16, v2s_all_U80x25_D_N_16 },
		{ v2s_dif_U80x20_D_N_16, v2s_all_U80x20_D_N_16 },
		{ v2s_dif_U40x25_D_N_16, v2s_all_U40x25_D_N_16 },
		{ v2s_dif_U40x20_D_N_16, v2s_all_U40x20_D_N_16 },
	},
	{
		{ v2s_dif_H80x25_D_N_16, v2s_all_H80x25_D_N_16 },
		{ v2s_dif_H80x20_D_N_16, v2s_all_H80x20_D_N_16 },
		{ v2s_dif_H40x25_D_N_16, v2s_all_H40x25_D_N_16 },
		{ v2s_dif_H40x20_D_N_16, v2s_all_H40x20_D_N_16 },
	},
};

/* 二倍サイズ - スキップライン */

int (*vram2screen_list_D_S_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_S_16, v2s_all_C80x25_D_S_16 },
		{ v2s_dif_C80x20_D_S_16, v2s_all_C80x20_D_S_16 },
		{ v2s_dif_C40x25_D_S_16, v2s_all_C40x25_D_S_16 },
		{ v2s_dif_C40x20_D_S_16, v2s_all_C40x20_D_S_16 },
	},
	{
		{ v2s_dif_M80x25_D_S_16, v2s_all_M80x25_D_S_16 },
		{ v2s_dif_M80x20_D_S_16, v2s_all_M80x20_D_S_16 },
		{ v2s_dif_M40x25_D_S_16, v2s_all_M40x25_D_S_16 },
		{ v2s_dif_M40x20_D_S_16, v2s_all_M40x20_D_S_16 },
	},
	{
		{ v2s_dif_U80x25_D_S_16, v2s_all_U80x25_D_S_16 },
		{ v2s_dif_U80x20_D_S_16, v2s_all_U80x20_D_S_16 },
		{ v2s_dif_U40x25_D_S_16, v2s_all_U40x25_D_S_16 },
		{ v2s_dif_U40x20_D_S_16, v2s_all_U40x20_D_S_16 },
	},
	{
		{ v2s_dif_H80x25_D_N_16, v2s_all_H80x25_D_N_16 },
		{ v2s_dif_H80x20_D_N_16, v2s_all_H80x20_D_N_16 },
		{ v2s_dif_H40x25_D_N_16, v2s_all_H40x25_D_N_16 },
		{ v2s_dif_H40x20_D_N_16, v2s_all_H40x20_D_N_16 },
	},
};

/* 二倍サイズ - インターレース */

int (*vram2screen_list_D_I_16[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_I_16, v2s_all_C80x25_D_I_16 },
		{ v2s_dif_C80x20_D_I_16, v2s_all_C80x20_D_I_16 },
		{ v2s_dif_C40x25_D_I_16, v2s_all_C40x25_D_I_16 },
		{ v2s_dif_C40x20_D_I_16, v2s_all_C40x20_D_I_16 },
	},
	{
		{ v2s_dif_M80x25_D_I_16, v2s_all_M80x25_D_I_16 },
		{ v2s_dif_M80x20_D_I_16, v2s_all_M80x20_D_I_16 },
		{ v2s_dif_M40x25_D_I_16, v2s_all_M40x25_D_I_16 },
		{ v2s_dif_M40x20_D_I_16, v2s_all_M40x20_D_I_16 },
	},
	{
		{ v2s_dif_U80x25_D_I_16, v2s_all_U80x25_D_I_16 },
		{ v2s_dif_U80x20_D_I_16, v2s_all_U80x20_D_I_16 },
		{ v2s_dif_U40x25_D_I_16, v2s_all_U40x25_D_I_16 },
		{ v2s_dif_U40x20_D_I_16, v2s_all_U40x20_D_I_16 },
	},
	{
		{ v2s_dif_H80x25_D_N_16, v2s_all_H80x25_D_N_16 },
		{ v2s_dif_H80x20_D_N_16, v2s_all_H80x20_D_N_16 },
		{ v2s_dif_H40x25_D_N_16, v2s_all_H40x25_D_N_16 },
		{ v2s_dif_H40x20_D_N_16, v2s_all_H40x20_D_N_16 },
	},
};
#endif /* SUPPORT_DOUBLE */

/* ------------------------------------------------------------------------- */
/* 等倍サイズ - 標準 */

int (*vram2screen_list_F_N_16_d[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_F_N_16_d, v2s_all_C80x25_F_N_16_d },
		{ v2s_dif_C80x20_F_N_16_d, v2s_all_C80x20_F_N_16_d },
		{ v2s_dif_C40x25_F_N_16_d, v2s_all_C40x25_F_N_16_d },
		{ v2s_dif_C40x20_F_N_16_d, v2s_all_C40x20_F_N_16_d },
	},
	{
		{ v2s_dif_M80x25_F_N_16_d, v2s_all_M80x25_F_N_16_d },
		{ v2s_dif_M80x20_F_N_16_d, v2s_all_M80x20_F_N_16_d },
		{ v2s_dif_M40x25_F_N_16_d, v2s_all_M40x25_F_N_16_d },
		{ v2s_dif_M40x20_F_N_16_d, v2s_all_M40x20_F_N_16_d },
	},
	{
		{ v2s_dif_U80x25_F_N_16_d, v2s_all_U80x25_F_N_16_d },
		{ v2s_dif_U80x20_F_N_16_d, v2s_all_U80x20_F_N_16_d },
		{ v2s_dif_U40x25_F_N_16_d, v2s_all_U40x25_F_N_16_d },
		{ v2s_dif_U40x20_F_N_16_d, v2s_all_U40x20_F_N_16_d },
	},
	{
		{ v2s_dif_H80x25_F_N_16, v2s_all_H80x25_F_N_16 },
		{ v2s_dif_H80x20_F_N_16, v2s_all_H80x20_F_N_16 },
		{ v2s_dif_H40x25_F_N_16, v2s_all_H40x25_F_N_16 },
		{ v2s_dif_H40x20_F_N_16, v2s_all_H40x20_F_N_16 },
	},
};

#ifdef SUPPORT_DOUBLE
/* 二倍サイズ - 標準 */

int (*vram2screen_list_D_N_16_d[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_N_16_d, v2s_all_C80x25_D_N_16_d },
		{ v2s_dif_C80x20_D_N_16_d, v2s_all_C80x20_D_N_16_d },
		{ v2s_dif_C40x25_D_N_16_d, v2s_all_C40x25_D_N_16_d },
		{ v2s_dif_C40x20_D_N_16_d, v2s_all_C40x20_D_N_16_d },
	},
	{
		{ v2s_dif_M80x25_D_N_16_d, v2s_all_M80x25_D_N_16_d },
		{ v2s_dif_M80x20_D_N_16_d, v2s_all_M80x20_D_N_16_d },
		{ v2s_dif_M40x25_D_N_16_d, v2s_all_M40x25_D_N_16_d },
		{ v2s_dif_M40x20_D_N_16_d, v2s_all_M40x20_D_N_16_d },
	},
	{
		{ v2s_dif_U80x25_D_N_16_d, v2s_all_U80x25_D_N_16_d },
		{ v2s_dif_U80x20_D_N_16_d, v2s_all_U80x20_D_N_16_d },
		{ v2s_dif_U40x25_D_N_16_d, v2s_all_U40x25_D_N_16_d },
		{ v2s_dif_U40x20_D_N_16_d, v2s_all_U40x20_D_N_16_d },
	},
	{
		{ v2s_dif_H80x25_D_N_16_d, v2s_all_H80x25_D_N_16_d },
		{ v2s_dif_H80x20_D_N_16_d, v2s_all_H80x20_D_N_16_d },
		{ v2s_dif_H40x25_D_N_16_d, v2s_all_H40x25_D_N_16_d },
		{ v2s_dif_H40x20_D_N_16_d, v2s_all_H40x20_D_N_16_d },
	},
};

/* 二倍サイズ - スキップライン */

int (*vram2screen_list_D_S_16_d[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_S_16_d, v2s_all_C80x25_D_S_16_d },
		{ v2s_dif_C80x20_D_S_16_d, v2s_all_C80x20_D_S_16_d },
		{ v2s_dif_C40x25_D_S_16_d, v2s_all_C40x25_D_S_16_d },
		{ v2s_dif_C40x20_D_S_16_d, v2s_all_C40x20_D_S_16_d },
	},
	{
		{ v2s_dif_M80x25_D_S_16_d, v2s_all_M80x25_D_S_16_d },
		{ v2s_dif_M80x20_D_S_16_d, v2s_all_M80x20_D_S_16_d },
		{ v2s_dif_M40x25_D_S_16_d, v2s_all_M40x25_D_S_16_d },
		{ v2s_dif_M40x20_D_S_16_d, v2s_all_M40x20_D_S_16_d },
	},
	{
		{ v2s_dif_U80x25_D_S_16_d, v2s_all_U80x25_D_S_16_d },
		{ v2s_dif_U80x20_D_S_16_d, v2s_all_U80x20_D_S_16_d },
		{ v2s_dif_U40x25_D_S_16_d, v2s_all_U40x25_D_S_16_d },
		{ v2s_dif_U40x20_D_S_16_d, v2s_all_U40x20_D_S_16_d },
	},
	{
		{ v2s_dif_H80x25_D_N_16_d, v2s_all_H80x25_D_N_16_d },
		{ v2s_dif_H80x20_D_N_16_d, v2s_all_H80x20_D_N_16_d },
		{ v2s_dif_H40x25_D_N_16_d, v2s_all_H40x25_D_N_16_d },
		{ v2s_dif_H40x20_D_N_16_d, v2s_all_H40x20_D_N_16_d },
	},
};

/* 二倍サイズ - インターレース */

int (*vram2screen_list_D_I_16_d[4][4][2])(T_RECTANGLE *updated_area) = {
	{
		{ v2s_dif_C80x25_D_I_16_d, v2s_all_C80x25_D_I_16_d },
		{ v2s_dif_C80x20_D_I_16_d, v2s_all_C80x20_D_I_16_d },
		{ v2s_dif_C40x25_D_I_16_d, v2s_all_C40x25_D_I_16_d },
		{ v2s_dif_C40x20_D_I_16_d, v2s_all_C40x20_D_I_16_d },
	},
	{
		{ v2s_dif_M80x25_D_I_16_d, v2s_all_M80x25_D_I_16_d },
		{ v2s_dif_M80x20_D_I_16_d, v2s_all_M80x20_D_I_16_d },
		{ v2s_dif_M40x25_D_I_16_d, v2s_all_M40x25_D_I_16_d },
		{ v2s_dif_M40x20_D_I_16_d, v2s_all_M40x20_D_I_16_d },
	},
	{
		{ v2s_dif_U80x25_D_I_16_d, v2s_all_U80x25_D_I_16_d },
		{ v2s_dif_U80x20_D_I_16_d, v2s_all_U80x20_D_I_16_d },
		{ v2s_dif_U40x25_D_I_16_d, v2s_all_U40x25_D_I_16_d },
		{ v2s_dif_U40x20_D_I_16_d, v2s_all_U40x20_D_I_16_d },
	},
	{
		{ v2s_dif_H80x25_D_N_16_d, v2s_all_H80x25_D_N_16_d },
		{ v2s_dif_H80x20_D_N_16_d, v2s_all_H80x20_D_N_16_d },
		{ v2s_dif_H40x25_D_N_16_d, v2s_all_H40x25_D_N_16_d },
		{ v2s_dif_H40x20_D_N_16_d, v2s_all_H40x20_D_N_16_d },
	},
};
#endif /* SUPPORT_DOUBLE */

#endif /* SUPPORT_16BPP */
