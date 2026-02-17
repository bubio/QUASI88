#ifndef MENU_VOLUME_MESSAGE_H_INCLUDED
#define MENU_VOLUME_MESSAGE_H_INCLUDED

/*--------------------------------------------------------------
 * 「音量」 タブ
 *--------------------------------------------------------------*/
enum {
	DATA_VOLUME_TOTAL,
	DATA_VOLUME_LEVEL,
	DATA_VOLUME_DEPEND,
	DATA_VOLUME_AUDIO,
	DATA_VOLUME_AUDIO_SET,
	DATA_VOLUME_AUDIO_QUIT,
	DATA_VOLUME_AUDIO_INFO,
};
static const t_menulabel data_volume[] = {
	{ { " Volume ",				" 音量 ",						}, },
	{ { " Level ",				" レベル ",						}, },
	{ { " depend on FM-level ",	" 以下はＦＭ音量に依存します ",	}, },

	{ { " Setting ",												" 詳細設定 ",											}, },
	{ { " <<< Sound-device Setting >>> ",							" <<< サウンドデバイス詳細設定 >>> ",					}, },
	{ { " EXIT ",													" 戻る ",												}, },
	{ { " The settings are applied in the return from the menu. ",	" 設定は、メニューモードから戻る際に反映されます。",	}, },
};



static const t_menulabel data_volume_no[] = {
	{ { " SoundBoard (OPN)                        [ Sound Output is not available. ] ",	" サウンドボード (OPN)                [ サウンド出力は組み込まれていません ] ",	}, },
	{ { " SoundBoard II (OPNA)                    [ Sound Output is not available. ] ",	" サウンドボードII (OPNA)             [ サウンド出力は組み込まれていません ] ",	}, },
	{ { " SoundBoard (OPN)                   [ Sound Output is OFF. ] ",				" サウンドボード (OPN)       [ サウンド出力はオフ状態です ] ",					}, },
	{ { " SoundBoard II (OPNA)               [ Sound Output is OFF. ] ",				" サウンドボードII (OPNA)    [ サウンド出力はオフ状態です ] ",					}, },
};



static const t_menulabel data_volume_type[] = {
	{ { " SoundBoard (OPN)            [ MAME built-in FM Generator ] ",	" サウンドボード (OPN)      [ FM音源ジェネレータ：MAME内蔵 ] ",	}, },
	{ { " SoundBoard II (OPNA)        [ MAME built-in FM-Generator ] ",	" サウンドボードII (OPNA)   [ FM音源ジェネレータ：MAME内蔵 ] ",	}, },
	{ { " SoundBoard (OPN)                    [ fmgen FM-Generator ] ",	" サウンドボード (OPN)         [ FM音源ジェネレータ：fmgen ] ",	}, },
	{ { " SoundBoard II (OPNA)                [ fmgen FM-Generator ] ",	" サウンドボードII (OPNA)      [ FM音源ジェネレータ：fmgen ] ",	}, },
};






enum {
	VOL_TOTAL,
	VOL_FM,
	VOL_PSG,
	VOL_BEEP,
	VOL_PCG,
	VOL_RHYTHM,
	VOL_ADPCM,
	VOL_FMGEN,
	VOL_SAMPLE
};

static const t_volume data_volume_total[] = {
	{ { " VOLUME [db]    :",	" 音量 [ｄｂ]    ：",	}, VOL_TOTAL,	VOL_MIN,		VOL_MAX,		1,	4,	},
};

static const t_volume data_volume_level[] = {
	{ { " FM sound   [%] :",	" ＦＭ音量   [％]：",	}, VOL_FM,		FMVOL_MIN,		FMVOL_MAX,		1,	10,	},
	{ { " PSG sound  [%] :",	" ＰＳＧ音量 [％]：",	}, VOL_PSG,		PSGVOL_MIN,		PSGVOL_MAX,		1,	10,	},
};

static const t_volume data_volume_rhythm[] = {
	{ { " RHYTHM     [%] :",	" リズム音量 [％]：",	}, VOL_RHYTHM,	RHYTHMVOL_MIN,	RHYTHMVOL_MAX,	1,	10,	},
	{ { " ADPCM      [%] :",	" ADPCM 音量 [％]：",	}, VOL_ADPCM,	ADPCMVOL_MIN,	ADPCMVOL_MAX,	1,	10,	},
};

static const t_volume data_volume_fmgen[] = {
	{ { " FM & PSG   [%] :",	" FM/PSG音量 [％]：",	}, VOL_FMGEN,	FMGENVOL_MIN,	FMGENVOL_MAX,	1,	10,	},
};

static const t_volume data_volume_misc[] = {
	{ { " BEEP sound [%] :",	" ＢＥＥＰ音 [％]：",	}, VOL_BEEP,	BEEPVOL_MIN,	BEEPVOL_MAX,	1,	10,	},
	{ { " PCG sound  [%] :",	" PCG-8100音 [％]：",	}, VOL_PCG,		PCGVOL_MIN,		PCGVOL_MAX,		1,	10,	},
};

static const t_volume data_volume_sample[] = {
	{ { " SAMPLE snd [%] :",	" サンプル音 [％]：",	}, VOL_SAMPLE,	SAMPLEVOL_MIN,	SAMPLEVOL_MAX,	1,	10,	},
};


enum {
	DATA_VOLUME_AUDIO_FMGEN,
	DATA_VOLUME_AUDIO_FREQ,
	DATA_VOLUME_AUDIO_SAMPLE,
};
static const t_menulabel data_volume_audio[] = {
	{ { " FM Generator     ",								" FM音源ジェネレータ       ",						}, },
	{ { " Sample-Frequency ([Hz], Range = 8000-48000) ",	" サンプリング周波数 ([Hz],範囲＝8000 - 48000) ",	}, },
	{ { " Sample Data      ",								" サンプル音の使用有無     ",						}, },
};

static const t_menudata data_volume_audio_fmgen[] = {
	{ { " MAME built-in ",	" MAME 内蔵  ",	}, FALSE,	},
	{ { " fmgen",			" fmgen",		}, TRUE,	},
};
static const t_menudata data_volume_audio_freq_combo[] = {
	{ { "48000",	"48000",	}, 48000,	},
	{ { "44100",	"44100",	}, 44100,	},
	{ { "22050",	"22050",	}, 22050,	},
	{ { "11025",	"11025",	}, 11025,	},
};
static const t_menudata data_volume_audio_sample[] = {
	{ { " Not Use       ",	" 使用しない ",	}, FALSE,	},
	{ { " Use",				" 使用する ",	}, TRUE,	},
};


static const t_menulabel data_volume_audiodevice_stop[] = {
	{ { " (The sound device is stopping.)",	" （サウンドデバイスは、停止中です）",	}, },
};


#endif /* MENU_VOLUME_MESSAGE_H_INCLUDED */
