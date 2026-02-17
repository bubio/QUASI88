/*===========================================================================
 *
 * メインページ 音量
 *
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quasi88.h"
#include "initval.h"
#include "menu.h"

#include "soundbd.h"
#include "snddrv.h"

#include "q8tk.h"

#include "menu-common.h"

#include "menu-volume.h"
#include "menu-volume-message.h"


t_sd_cfg sd_cfg_init;
t_sd_cfg sd_cfg_now;

#ifdef USE_SOUND

static Q8tkWidget *audio_window_base;
static Q8tkWidget *audio_window_focus;

#endif

/*----------------------------------------------------------------------*/
/* ボリュームやレベルの変更処理 */
#ifdef USE_SOUND
static int get_volume(int type)
{
	switch (type) {
	case VOL_TOTAL:
		return xmame_cfg_get_mastervolume();
	case VOL_FM:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_FM);
	case VOL_PSG:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_PSG);
	case VOL_BEEP:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_BEEP);
	case VOL_PCG:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_PCG);
	case VOL_RHYTHM:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_RHYTHM);
	case VOL_ADPCM:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_ADPCM);
	case VOL_FMGEN:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_FMGEN);
	case VOL_SAMPLE:
		return xmame_cfg_get_mixer_volume(XMAME_MIXER_SAMPLE);
	}
	return 0;
}
static void cb_volume(Q8tkWidget *widget, void *p)
{
	int vol = Q8TK_ADJUSTMENT(widget)->value;

	switch (P2INT(p)) {
	case VOL_TOTAL:
		xmame_cfg_set_mastervolume(vol);
		break;
	case VOL_FM:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_FM, vol);
		break;
	case VOL_PSG:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_PSG, vol);
		break;
	case VOL_BEEP:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_BEEP, vol);
		break;
	case VOL_PCG:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_PCG, vol);
		break;
	case VOL_RHYTHM:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_RHYTHM, vol);
		break;
	case VOL_ADPCM:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_ADPCM, vol);
		break;
	case VOL_FMGEN:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_FMGEN, vol);
		break;
	case VOL_SAMPLE:
		xmame_cfg_set_mixer_volume(XMAME_MIXER_SAMPLE, vol);
		break;
	}
}


static Q8tkWidget *menu_volume_unit(const t_volume *p, int count)
{
	int i;
	Q8tkWidget *vbox, *hbox;

	vbox = PACK_VBOX(NULL);
	{
		for (i = 0; i < count; i++, p++) {

			hbox = PACK_HBOX(vbox);
			{
				PACK_LABEL(hbox, GET_LABEL(p, 0));

				PACK_HSCALE(hbox,
							p,
							get_volume(p->val),
							cb_volume, INT2P(p->val));
			}
		}
	}

	return vbox;
}


static Q8tkWidget *menu_volume_total(void)
{
	return menu_volume_unit(data_volume_total, COUNTOF(data_volume_total));
}
static Q8tkWidget *menu_volume_level(void)
{
	return menu_volume_unit(data_volume_level, COUNTOF(data_volume_level));
}
static Q8tkWidget *menu_volume_rhythm(void)
{
	return menu_volume_unit(data_volume_rhythm, COUNTOF(data_volume_rhythm));
}
static Q8tkWidget *menu_volume_fmgen(void)
{
	return menu_volume_unit(data_volume_fmgen, COUNTOF(data_volume_fmgen));
}
static Q8tkWidget *menu_volume_misc(void)
{
	return menu_volume_unit(data_volume_misc, COUNTOF(data_volume_misc));
}
static Q8tkWidget *menu_volume_sample(void)
{
	return menu_volume_unit(data_volume_sample, COUNTOF(data_volume_sample));
}
#endif
/*----------------------------------------------------------------------*/
/* サウンドなし時メッセージ */

static Q8tkWidget *menu_volume_no_available(void)
{
	int type;
	Q8tkWidget *l;

#ifdef USE_SOUND
	type = 2;
#else
	type = 0;
#endif

	if (sound_board == SOUND_II) {
		type |= 1;
	}

	l = q8tk_label_new(GET_LABEL(data_volume_no, type));

	q8tk_widget_show(l);

	return l;
}

/*----------------------------------------------------------------------*/
/* サウンドドライバの種別表示ラベル */
#ifdef USE_SOUND
static Q8tkWidget *menu_volume_type(void)
{
	int type;
	Q8tkWidget *l;

#ifdef USE_FMGEN
	if (xmame_cfg_get_use_fmgen()) {
		type = 2;
	} else
#endif
	{
		type = 0;
	}

	if (sound_board == SOUND_II) {
		type |= 1;
	}

	l = q8tk_label_new(GET_LABEL(data_volume_type, type));

	q8tk_widget_show(l);

	return l;
}
#endif

/*----------------------------------------------------------------------*/
/* 詳細設定ダイアログ */
#ifdef USE_SOUND
static void audio_create(void);
static void audio_start(void);
static void audio_finish(void);

static void cb_volume_audio(UNUSED_WIDGET, UNUSED_PARM)
{
	audio_start();
}

static Q8tkWidget *menu_volume_audio(void)
{
	Q8tkWidget *hbox;

	hbox = PACK_HBOX(NULL);
	{
		/* 空行 */
		PACK_LABEL(hbox, " ");

		PACK_BUTTON(hbox,
					GET_LABEL(data_volume, DATA_VOLUME_AUDIO),
					cb_volume_audio, NULL);
		/* 空行 */
		PACK_LABEL(hbox, " ");
	}
	return hbox;
}
#endif

/*----------------------------------------------------------------------*/

Q8tkWidget *menu_volume(void)
{
	Q8tkWidget *vbox, *hbox, *vbox2;
	Q8tkWidget *w;
	const t_menulabel *l = data_volume;

	if (xmame_has_sound() == FALSE) {

		w = PACK_FRAME(NULL, "", menu_volume_no_available());
		q8tk_frame_set_shadow_type(w, Q8TK_SHADOW_ETCHED_OUT);

		return w;
	}


#ifdef USE_SOUND
	audio_window_base = NULL;

	vbox = PACK_VBOX(NULL);
	{
		hbox = PACK_HBOX(NULL);
		{
			w = PACK_FRAME(hbox, "", menu_volume_type());
			q8tk_frame_set_shadow_type(w, Q8TK_SHADOW_ETCHED_OUT);

			PACK_LABEL(hbox, "  ");

			PACK_BUTTON(hbox, GET_LABEL(data_volume, DATA_VOLUME_AUDIO),
						cb_volume_audio, NULL);
		}
		q8tk_box_pack_start(vbox, hbox);

		if (xmame_has_mastervolume()) {
			PACK_FRAME(vbox,
					   GET_LABEL(l, DATA_VOLUME_TOTAL), menu_volume_total());
		}

		vbox2 = PACK_VBOX(NULL);
		{
#ifdef  USE_FMGEN
			if (xmame_cfg_get_use_fmgen()) {
				w = menu_volume_fmgen();
			} else
#endif
			{
				w = menu_volume_level();
			}
			q8tk_box_pack_start(vbox2, w);

			q8tk_box_pack_start(vbox2, menu_volume_misc());

			if (xmame_cfg_get_use_samples()) {
				q8tk_box_pack_start(vbox2, menu_volume_sample());
			}
		}
		PACK_FRAME(vbox, GET_LABEL(l, DATA_VOLUME_LEVEL), vbox2);

#ifdef  USE_FMGEN
		if (xmame_cfg_get_use_fmgen()) {
			;
		} else
#endif
		{
			if (sound_board == SOUND_II) {
				PACK_FRAME(vbox,
						   GET_LABEL(l, DATA_VOLUME_DEPEND),
						   menu_volume_rhythm());
			}
		}

		if (xmame_has_audiodevice() == FALSE) {
			PACK_LABEL(vbox, "");
			PACK_LABEL(vbox, GET_LABEL(data_volume_audiodevice_stop, 0));
		}
	}

	return vbox;
#endif
}


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 *
 * サブウインドウ  AUDIO
 *
 * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef USE_SOUND

/*----------------------------------------------------------------------*/
/* FM音源ジェネレータ */
#ifdef USE_FMGEN
static int get_volume_audio_fmgen(void)
{
	return sd_cfg_now.use_fmgen;
}
static void cb_volume_audio_fmgen(UNUSED_WIDGET, void *p)
{
	sd_cfg_now.use_fmgen = xmame_cfg_set_use_fmgen(P2INT(p));
}


static Q8tkWidget *volume_audio_fmgen(void)
{
	Q8tkWidget *box;
	const t_menulabel *l = data_volume_audio;

	box = PACK_HBOX(NULL);
	{
		PACK_LABEL(box, GET_LABEL(l, DATA_VOLUME_AUDIO_FMGEN));

		PACK_RADIO_BUTTONS(box,
						   data_volume_audio_fmgen,
						   COUNTOF(data_volume_audio_fmgen),
						   get_volume_audio_fmgen(), cb_volume_audio_fmgen);
	}

	return box;
}
#endif

/*----------------------------------------------------------------------*/
/* サンプリング周波数 */
static int get_volume_audio_freq(void)
{
	return sd_cfg_now.sample_freq;
}
static void cb_volume_audio_freq(Q8tkWidget *widget, void *mode)
{
	int i;
	const t_menudata *p = data_volume_audio_freq_combo;
	const char *combo_str = q8tk_combo_get_text(widget);
	char buf[16], *conv_end;
	int val = 0;
	int fit = FALSE;

	/* COMBO BOX から ENTRY に一致するものを探す */
	for (i = 0; i < COUNTOF(data_volume_audio_freq_combo); i++, p++) {
		if (strcmp(p->str[menu_lang], combo_str) == 0) {
			val = p->val;
			fit = TRUE;
			/* 一致した値を適用 */
			break;
		}
	}

	/* COMBO BOX に該当がない場合 */
	if (fit == FALSE) {
		strncpy(buf, combo_str, 15);
		buf[15] = '\0';

		val = strtoul(buf, &conv_end, 10);

		if ((P2INT(mode) == 0) &&
			(strlen(buf) == 0 || val == 0)) {
			/* 空白 + ENTER や 0 + ENTER 時は デフォルト値を適用 */
			val = 44100;
			fit = TRUE;

		} else if (*conv_end != '\0') {
			/* 数字変換失敗なら その値は使えない */
			fit = FALSE;

		} else {
			/* 数字変換成功なら その値を適用する */
			fit = TRUE;
		}
	}

	/* 適用した値が有効範囲なら、セット */
	if (fit) {
		if (8000 <= val && val <= 48000) {
			sd_cfg_now.sample_freq = xmame_cfg_set_sample_freq(val);
		}
	}

	/* COMBO ないし ENTER時は、値を再表示 */
	if (P2INT(mode) == 0) {
		sprintf(buf, "%5d", get_volume_audio_freq());
		q8tk_combo_set_text(widget, buf);
	}
}


static Q8tkWidget *volume_audio_freq(void)
{
	Q8tkWidget *box;
	char buf[16];
	const t_menulabel *l = data_volume_audio;

	box = PACK_HBOX(NULL);
	{
		PACK_LABEL(box, GET_LABEL(l, DATA_VOLUME_AUDIO_FREQ));

		sprintf(buf, "%5d", get_volume_audio_freq());
		PACK_COMBO(box,
				   data_volume_audio_freq_combo,
				   COUNTOF(data_volume_audio_freq_combo),
				   get_volume_audio_freq(), buf, 6,
				   cb_volume_audio_freq, (void *)0,
				   cb_volume_audio_freq, (void *)1);
	}

	return box;
}

/*----------------------------------------------------------------------*/
/* サンプル音の使用有無 */
static int get_volume_audio_sample(void)
{
	return sd_cfg_now.use_samples;
}
static void cb_volume_audio_sample(UNUSED_WIDGET, void *p)
{
	sd_cfg_now.use_samples = xmame_cfg_set_use_samples(P2INT(p));
}


static Q8tkWidget *volume_audio_sample(void)
{
	Q8tkWidget *box;
	const t_menulabel *l = data_volume_audio;

	box = PACK_HBOX(NULL);
	{
		PACK_LABEL(box, GET_LABEL(l, DATA_VOLUME_AUDIO_SAMPLE));

		PACK_RADIO_BUTTONS(box,
						   data_volume_audio_sample,
						   COUNTOF(data_volume_audio_sample),
						   get_volume_audio_sample(), cb_volume_audio_sample);
	}

	return box;
}

/*----------------------------------------------------------------------*/

static void cb_audio_config(Q8tkWidget *widget, void *modes)
{
	int index = (P2INT(modes)) / 2;
	int mode  = (P2INT(modes)) & 1;			/* 0:ENTER 1:INPUT */

	T_SNDDRV_CONFIG *p       = sd_cfg_now.local[index].info;
	SD_CFG_LOCAL_VAL def_val = sd_cfg_now.local[index].val;

	const char *entry_str = q8tk_entry_get_text(widget);
	char buf[16], *conv_end;
	SD_CFG_LOCAL_VAL val = { 0 };
	int fit = FALSE;
	int zero = FALSE;

	strncpy(buf, entry_str, 15);
	buf[15] = '\0';

	switch (p->type) {
	case SNDDRV_INT:
		val.i = (int)strtoul(buf, &conv_end, 10);
		if (val.i == 0) {
			zero = TRUE;
		}
		break;

	case SNDDRV_FLOAT:
		val.f = (float)strtod(buf, &conv_end);
		if (val.f == 0) {
			zero = TRUE;
		}
		break;
	}

	if (((int) mode == 0) &&
		(strlen(buf) == 0 || zero)) {
		/* 空白 + ENTER や 0 + ENTER 時は 直前の有効値を適用 */
		val = def_val;
		fit = TRUE;

	} else if (*conv_end != '\0') {
		/* 数字変換失敗なら その値は使えない */
		fit = FALSE;

	} else {
		/* 数字変換成功なら その値を適用する */
		fit = TRUE;
	}

	/* 適用した値が有効範囲なら、セット */
	if (fit) {
		switch (p->type) {
		case SNDDRV_INT:
			if ((int)(p->low) <= val.i && val.i <= (int)(p->high)) {
				sd_cfg_now.local[index].val.i =
					*((int *)(p->work)) = val.i;
			}
			break;

		case SNDDRV_FLOAT:
			if ((float)(p->low) <= val.f && val.f <= (float)(p->high)) {
				sd_cfg_now.local[index].val.f =
					*((float *)(p->work)) = val.f;
			}
			break;
		}
	}

	/* COMBO ないし ENTER時は、値を再表示*/
	if ((int) mode == 0) {
		switch (p->type) {
		case SNDDRV_INT:
			sprintf(buf, "%7d", sd_cfg_now.local[index].val.i);
			break;

		case SNDDRV_FLOAT:
			sprintf(buf, "%7.3f", sd_cfg_now.local[index].val.f);
			break;
		}

		q8tk_entry_set_text(widget, buf);
	}

	/*if(p->type==SNDDRV_INT)  printf("%d\n", *((int *)(p->work)));*/
	/*if(p->type==SNDDRV_FLOAT)printf("%f\n", *((float *)(p->work)));fflush(stdout);*/
}

static int audio_config_widget(Q8tkWidget *box)
{
	Q8tkWidget *hbox;
	char buf[32];
	int i;
	T_SNDDRV_CONFIG *p;

	for (i = 0; i < sd_cfg_now.local_cnt; i++) {

		p = sd_cfg_now.local[i].info;

		switch (p->type) {
		case SNDDRV_INT:
			sprintf(buf, "%7d", sd_cfg_now.local[i].val.i);
			break;

		case SNDDRV_FLOAT:
			sprintf(buf, "%7.3f", sd_cfg_now.local[i].val.f);
			break;
		}

		hbox = PACK_HBOX(NULL);
		{
			PACK_LABEL(hbox, p->title);

			PACK_ENTRY(hbox,
					   8, 9, buf,
					   cb_audio_config, INT2P(i * 2),
					   cb_audio_config, INT2P(i * 2 + 1));
		}
		q8tk_box_pack_start(box, hbox);
		PACK_LABEL(box, "");
	}

	return i;
}


/*----------------------------------------------------------------------*/

static void cb_volume_audio_end(UNUSED_WIDGET, UNUSED_PARM)
{
	audio_finish();
}

static void audio_create(void)
{
	Q8tkWidget *w, *a, *f, *v, *h, *b, *l;
	const t_menulabel *p = data_volume;

	/* メインとなるウインドウ */
	{
		w = q8tk_window_new(Q8TK_WINDOW_DIALOG);
		a = q8tk_accel_group_new();
		q8tk_accel_group_attach(a, w);
	}
	/* に、フレームを乗せて */
	{
		f = q8tk_frame_new(GET_LABEL(p, DATA_VOLUME_AUDIO_SET));
		q8tk_frame_set_shadow_type(f, Q8TK_SHADOW_OUT);
		q8tk_container_add(w, f);
		q8tk_widget_show(f);
	}
	/* それにボックスを乗せる */
	{
		v = q8tk_vbox_new();
		q8tk_container_add(f, v);
		q8tk_widget_show(v);
		/* ボックスには */
		{
			/* AUDIOメニュー と */
			int i;
			Q8tkWidget *vbox;
			const t_menulabel *l = data_volume;

			vbox = PACK_VBOX(NULL);
			{
				PACK_HSEP(vbox);

#ifdef USE_FMGEN
				q8tk_box_pack_start(vbox, volume_audio_fmgen());
				PACK_HSEP(vbox);
#else
				PACK_LABEL(vbox, "");
				PACK_LABEL(vbox, "");
#endif
				q8tk_box_pack_start(vbox, volume_audio_freq());
				PACK_LABEL(vbox, "");
				q8tk_box_pack_start(vbox, volume_audio_sample());
				PACK_HSEP(vbox);

				i = audio_config_widget(vbox);

				for (; i < 5; i++) {
					PACK_LABEL(vbox, "");
					PACK_LABEL(vbox, "");
				}
				PACK_HSEP(vbox);
			}
			q8tk_box_pack_start(v, vbox);
		}
		{
			/* さらにボックス */
			h = q8tk_hbox_new();
			q8tk_box_pack_start(v, h);
			q8tk_widget_show(h);
			/* ボックスには */
			{
				/* 終了ボタンを配置 */
				b = PACK_BUTTON(h,
								GET_LABEL(p, DATA_VOLUME_AUDIO_QUIT),
								cb_volume_audio_end, NULL);

				q8tk_accel_group_add(a, Q8TK_KEY_ESC, b, "clicked");

				/* ラベルも配置 */
				l = PACK_LABEL(h, GET_LABEL(p, DATA_VOLUME_AUDIO_INFO));
				q8tk_misc_set_placement(l, 0, Q8TK_PLACEMENT_Y_CENTER);
			}
		}
	}

	audio_window_base = w;
	audio_window_focus = b;
}

static void audio_start(void)
{
	if (audio_window_base == NULL) {
		audio_create();
	}

	q8tk_widget_show(audio_window_base);
	q8tk_grab_add(audio_window_base);

	q8tk_widget_set_focus(audio_window_focus);
}

static void audio_finish(void)
{
	q8tk_grab_remove(audio_window_base);
	q8tk_widget_hide(audio_window_base);
}

#endif
