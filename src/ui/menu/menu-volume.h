#ifndef MENU_VOLUME_H_INCLUDED
#define MENU_VOLUME_H_INCLUDED

#include "snddrv.h"

#define NR_SD_CFG_LOCAL	(5)

typedef union {
	int			i;
	float		f;
} SD_CFG_LOCAL_VAL;

typedef struct {

	int			sound_board;
	int			use_fmgen;
	int			sample_freq;
	int			use_samples;

	/* 以下、システム依存の設定 */
	int			local_cnt;
	struct {
		T_SNDDRV_CONFIG			*info;
		SD_CFG_LOCAL_VAL		val;
	} local[ NR_SD_CFG_LOCAL ];

} t_sd_cfg;

extern t_sd_cfg sd_cfg_init;
extern t_sd_cfg sd_cfg_now;


/* メインページ 音量 */
extern Q8tkWidget *menu_volume(void);

#endif /* MENU_VOLUME_H_INCLUDED */
