#include "sndintrf.h"
#include "streams.h"
#include "pcgintf.h"
#include "pcg.h"

struct pcg8100_info
{
	sound_stream *	stream;
	void *			chip;
};


/* update request from pcg.c */
void PCG8100UpdateRequest(void *param)
{
	struct pcg8100_info *info = param;
	stream_update(info->stream);
}


static void pcg8100_stream_update(void *param, stream_sample_t **inputs, stream_sample_t **buffer, int length)
{
	struct pcg8100_info *info = param;
	PCG8100UpdateOne(info->chip, buffer[0], length);
}


static void *pcg8100_start(int sndindex, int clock, const void *config)
{
	struct pcg8100_info *info;

	info = auto_malloc(sizeof(*info));
	memset(info, 0, sizeof(*info));

	/* stream system initialize */
	info->stream = stream_create(0,1,Machine->sample_rate,info,pcg8100_stream_update);

	/* Initialize pcg emurator */
	info->chip = PCG8100Init(info,sndindex,clock,Machine->sample_rate);

	if (info->chip)
		return info;

	/* error */
	/* stream close */
	return NULL;
}

static void pcg8100_stop(void *token)
{
	struct pcg8100_info *info = token;
	PCG8100Shutdown(info->chip);
}

static void pcg8100_reset(void *token)
{
	struct pcg8100_info *info = token;
    PCG8100ResetChip(info->chip);
}


WRITE8_HANDLER( PCG8100_gate_port_0_w )
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	PCG8100Gate(info->chip,data);
}

WRITE8_HANDLER( PCG8100_counter0_port_0_w )
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	PCG8100Counter0(info->chip,data);
}

WRITE8_HANDLER( PCG8100_counter1_port_0_w )
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	PCG8100Counter1(info->chip,data);
}

WRITE8_HANDLER( PCG8100_counter2_port_0_w )
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	PCG8100Counter2(info->chip,data);
}

WRITE8_HANDLER( PCG8100_control_port_0_w )
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	PCG8100Control(info->chip,data);
}

void PCG8100_set_volume(float volume)
{
	struct pcg8100_info *info = sndti_token(SOUND_PCG8100, 0);
	stream_set_output_gain(info->stream, 0, volume);
}





/**************************************************************************
 * Generic get_info
 **************************************************************************/

static void pcg8100_set_info(void *token, UINT32 state, sndinfo *info)
{
	switch (state)
	{
		/* no parameters to set */
	}
}


void pcg8100_get_info(void *token, UINT32 state, sndinfo *info)
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case SNDINFO_PTR_SET_INFO:						info->set_info = pcg8100_set_info;		break;
		case SNDINFO_PTR_START:							info->start = pcg8100_start;			break;
		case SNDINFO_PTR_STOP:							info->stop = pcg8100_stop;				break;
		case SNDINFO_PTR_RESET:							info->reset = pcg8100_reset;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case SNDINFO_STR_NAME:							info->s = "PCG8100";					break;
		case SNDINFO_STR_CORE_FAMILY:					info->s = "Quasi88";					break;
		case SNDINFO_STR_CORE_VERSION:					info->s = "1.0";						break;
		case SNDINFO_STR_CORE_FILE:						info->s = __FILE__;						break;
		case SNDINFO_STR_CORE_CREDITS:					info->s = "Copyright (c) 2025, S.F";	break;
	}
}
