#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"
#include "pcg.h"
#include "memory.h"


static const int mask[] = { 0x08, 0x40, 0x80, (0x08 | 0x40 | 0x80), };

/* ---------- work of PCG emulator ----------- */
typedef struct pcg8100_f {
	void			*param;
	int				clock;			/* 3993600Hz */
	int				sample_rate;	/* 44100/22050 Hz */
	int				sample_bit_8;	/* TRUE / FALSE */

	struct {
		UINT8			gate;		/* !FALSE / FALSE*/
		UINT8			audible;	/* TRUE / FALSE */
		UINT16			value;
		UINT16			count;
		UINT8			low_count;
		UINT8			access;
		UINT8			mode;
		UINT8			bcd;
		UINT8			phase;
		UINT8			access_step;
	} ch[3];
	UINT8				gate;

} PCG8100;


/* ---------- misc ----------- */
static int bcd_to_dec(int bcd, int val)
{
	if (bcd	== 0) {
		return (val == 0) ? 0x10000 : val;
	}

	if (val == 0) {
		return 10000;
	} else {
		/* If BCD value includes A to F, then as is. */
		return (((val >> 12) & 0xf) * 1000 +
				((val >>  8) & 0xf) *  100 +
				((val >>  4) & 0xf) *   10 +
				( val        & 0xf));
	}
}

static int subtract(int bcd, int value, int elapse)
{
	int i, num, carry, digit[4];

	/*assert(value >= elase);*/

	if (bcd	== 0) {
		return value - elapse;
	}

	/* If BCD value includes A to F, then as is. */
	/* However, when counting down, the next 0 is 9, not F. */
	digit[0] =  value        & 0xf;
	digit[1] = (value >>  4) & 0xf;
	digit[2] = (value >>  8) & 0xf;
	digit[3] = (value >> 12) & 0xf;

	carry = 0;
	for (i = 0; i < 4; i++) {
		num = (elapse % 10) + carry;
		elapse /= 10;

		if (digit[i] >= num) {
			digit[i] = digit[i] - num;
			carry = 0;
		} else {
			digit[i] = 10 + digit[i] - num;
			carry = 1;
		}
	}

	return ((digit[3] << 12) |
			(digit[2] <<  8) |
			(digit[1] <<  4) |
			(digit[0]));
}

static void stream_update(stream_sample_t **buf_ptr, int len, stream_sample_t d, int *rest_samples)
{
	stream_sample_t *buf = *buf_ptr;

	if (len > *rest_samples) {
		len = *rest_samples;
	}

	if (d) {
		int i;
		for (i = 0; i < len; i++) {
			*buf ++ += d;
		}
	} else {
		buf += len;
	}

	*buf_ptr = buf;
	*rest_samples -= len;
}

/* ---------- update one of chip ----------- */
void PCG8100UpdateOne(void *chip, stream_sample_t *buffer, int length)
{
	PCG8100 *PCG = chip;
	const int LOW = 0;
	int HIGH = (PCG->sample_bit_8) ? 0x20 : 0x2000;
	int ch, value;
	int elapse_cycle;				/* elapsed cycles */

	memset(buffer, 0, length * sizeof(*buffer));

	if (use_pcg == FALSE) {
		return;
	}

	if (length <= 1075) {
		elapse_cycle = (unsigned int) PCG->clock * length / PCG->sample_rate;
	} else {
		/* avoid overflow...*/
		elapse_cycle = ((PCG->clock / 100) * length) / (PCG->sample_rate / 100);
	}

	for (ch = 0; ch < 3; ch++) {
		stream_sample_t *buf = buffer;		/* top of sampling-buffer */
		int samples = length;				/* remaining samples */
		int elapse = elapse_cycle;
		UINT64 fixed = 0;

		while (samples > 0) {
			int frags, out;

			switch (PCG->ch[ch].mode) {
			case 0:
				/* MODE 0
				 *                           _______  ==> keep
				 *  out    ###|_____________|
				 *  value           4 3 2 1 0             countdown if gate==1
				 *  phase     0....1........2.......
				 *                 *                  * = set counter
				 */
				switch (PCG->ch[ch].phase) {
				case 0:
					out = LOW;
					frags = samples;
					break;
				case 1:
					out = LOW;
					if (PCG->ch[ch].access_step == 0 &&
						PCG->ch[ch].gate) {
						/* do countdown... */
						value = bcd_to_dec(PCG->ch[ch].bcd, PCG->ch[ch].value);

						if (value > elapse) {
							PCG->ch[ch].value = subtract(PCG->ch[ch].bcd, PCG->ch[ch].value, elapse);
							frags = samples;
						} else {
							PCG->ch[ch].value = 0;
							PCG->ch[ch].phase = 2;
							frags = PCG->sample_rate * value / PCG->clock;
							elapse -= value;
						}
					} else {
						frags = samples;
					}
					break;
				case 2:
					out = HIGH;
					frags = samples;
					break;
				}
				break;

			case 1:
				/* MODE 1
				 *             ____          _______  ==> keep
				 *  out    ###|    |________|
				 *  value           4 3 2 1 0             countdown
				 *  phase     0....1........0.......
				 *                 *                  * = gate 0->1
				 */
				switch (PCG->ch[ch].phase) {
				case 0:
					out = HIGH;
					frags = samples;
					break;
				case 1:
					out = LOW;
					/* do countdown... */
					value = bcd_to_dec(PCG->ch[ch].bcd, PCG->ch[ch].value);

					if (value > elapse) {
						PCG->ch[ch].value = subtract(PCG->ch[ch].bcd, PCG->ch[ch].value, elapse);
						frags = samples;
					} else {
						PCG->ch[ch].value = 0;
						PCG->ch[ch].phase = 0;
						frags = PCG->sample_rate * value / PCG->clock;
						elapse -= value;
					}
					break;
				}
				break;

			case 2:
				/* MODE 2
				 *             ____________   _____   ___  ==> repeat
				 *  out    ###|            |_|     |_|   
				 *  value          4 3 2 1 0 3 2 1 0 3 2       countdown if gate==1
				 *  phase     0...1........1.......1.....
				 *                *                        * = gate 0->1 / set counter
				 */
				switch (PCG->ch[ch].phase) {
				case 0:
					out = HIGH;
					frags = samples;
					break;
				case 1:
					out = HIGH;
					if (PCG->ch[ch].gate) {
						/* do countdown... */
						value = bcd_to_dec(PCG->ch[ch].bcd, PCG->ch[ch].value);

						if (value > elapse) {
							PCG->ch[ch].value = subtract(PCG->ch[ch].bcd, PCG->ch[ch].value, elapse);
							frags = samples;
						} else {
							/* The last one clock minute outputs LOW, but ignore it. */
							PCG->ch[ch].value = 0;
							PCG->ch[ch].phase = 1;
							frags = PCG->sample_rate * value / PCG->clock;
							elapse -= value;
						}
					} else {
						frags = samples;
					}
					break;
				}
				break;

			case 3:
				/*
				 *             _______     ___     ___  ==> repeat
				 * MODE 3  ###|       |___|   |___|   
				 *  value         4 2 4 2 4 2 4 2 4 2       countdown if gate==1
				 *  phase     0..1....2...1...2...1...
				 *               *                      * = gate 0->1 / set counter
				 *
				 *             _________     ___     _  ==> repeat
				 * MODE 3  ###|         |___|   |___| 
				 *  value         5 4 2 5 2 5 2 5 2 5       countdown if gate==1
				 *  phase     0..1......2...1...2...1.
				 *               *                      * = gate 0->1 / set counter
				 *
				 */
				switch (PCG->ch[ch].phase) {
				case 0:
					if (pcg_level == 0) {
						out = LOW;
					} else {
						out = HIGH;
					}
					frags = samples;
					break;
				case 1:
				case 2:
					if (PCG->ch[ch].gate) {
						if (PCG->ch[ch].audible) {
							out = (PCG->ch[ch].phase == 1) ? HIGH : LOW;
						} else {
							out = LOW;
						}
						/* do countdown... (Ignore even and odd count values.) */
						value = bcd_to_dec(PCG->ch[ch].bcd, PCG->ch[ch].value);

						if (value > elapse * 2) {
							PCG->ch[ch].value = subtract(PCG->ch[ch].bcd, PCG->ch[ch].value, elapse * 2);
							frags = samples;
						} else {
							PCG->ch[ch].value = PCG->ch[ch].count;
							PCG->ch[ch].phase = (PCG->ch[ch].phase == 1) ? 2 : 1;
#if 1
							fixed += ((UINT64) (PCG->sample_rate * ((value + 1) / 2)) << 16) / PCG->clock;
							frags = (int) (fixed >> 16);
							fixed &= 0xffff;
#else
							frags = PCG->sample_rate * ((value + 1) / 2) / PCG->clock;
#endif
							elapse -= (value + 1) / 2;
						}
					} else {
						if (pcg_level == 0) {
							out = LOW;
						} else {
							out = HIGH;
						}
						PCG->ch[ch].phase = 1; /* ??? */
						frags = samples;
					}
					break;
				}
				break;

			case 4:
			case 5:
				/* MODE 4/5
				 *             ___________   ____  ==> keep
				 *  out    ###|           |_|         
				 *  value         4 3 2 1 0            countdown if gate==1
				 *  phase     0..1........1......
				 *               *                 * = set counter (MODE 4)
				 *                                 * = gate 0->1   (MODE 5)
				 */
				switch (PCG->ch[ch].phase) {
				case 0:
					out = HIGH;
					frags = samples;
					break;
				case 1:
					out = HIGH;
					if (PCG->ch[ch].mode == 5 ||
						PCG->ch[ch].gate) {
						/* do countdown... */
						value = bcd_to_dec(PCG->ch[ch].bcd, PCG->ch[ch].value);

						if (value > elapse) {
							PCG->ch[ch].value = subtract(PCG->ch[ch].bcd, PCG->ch[ch].value, elapse);
							frags = samples;
						} else {
							/* The last one clock minute outputs LOW, but ignore it. */
							PCG->ch[ch].value = 0;
							PCG->ch[ch].phase = 0;
							frags = PCG->sample_rate * value / PCG->clock;
							elapse -= value;
						}
					} else {
						frags = samples;
					}
					break;
				}
				break;

			default:
				return;
			}

			stream_update(&buf, frags, out, &samples);
		}
	}
}


/* ---------- reset one of chip ---------- */
void PCG8100ResetChip(void *chip)
{
    PCG8100 *PCG = chip;
	int ch;

	for (ch = 0; ch < 3; ch++) {
		PCG->ch[ch].gate = mask[ch];
		PCG->ch[ch].audible = TRUE;
		PCG->ch[ch].value = 0;
		PCG->ch[ch].count = 0;
		PCG->ch[ch].access = 3;
		PCG->ch[ch].mode = 0;
		PCG->ch[ch].bcd = 0;
		PCG->ch[ch].phase = 0;
		PCG->ch[ch].access_step = 0;
	}
    PCG->gate = mask[3];
}


/* ----------  Initialize PCG emulator(s) ---------- */
void * PCG8100Init(void *param, int index, int clock, int sample_rate)
{
	PCG8100 *PCG;

	/* allocate pcg8100 state space */
	if( (PCG = (PCG8100 *)malloc(sizeof(PCG8100)))==NULL)
	return NULL;
	/* clear */
	memset(PCG,0,sizeof(PCG8100));

	PCG->param  = param;
	PCG->clock	 = clock;
	PCG->sample_rate  = sample_rate;
	PCG->sample_bit_8 = FALSE;   /* ...always false... */

	PCG8100ResetChip(PCG);

	return PCG;
}


/* ---------- shut down PCG emulator ----------- */
void PCG8100Shutdown(void *chip)
{
	PCG8100 *PCG = chip;

	free(PCG);
}


/* ---------- PCG I/O interface ---------- */
void PCG8100Gate(void *chip,int v)
{
	PCG8100 *PCG = chip;
	int ch;

	if ((PCG->gate ^ v) & mask[3]) {
		PCG8100UpdateRequest(PCG->param);
	}

	for (ch = 0; ch < 3; ch++) {
		if ((PCG->ch[ch].gate == 0x00) && (v & mask[ch])) {
			/* Gate OFF -> ON */

			switch (PCG->ch[ch].mode) {
			case 0:
			case 4:
				break;

			case 1:
			case 2:
			case 3:
			case 5:
				PCG->ch[ch].value = PCG->ch[ch].count;
				PCG->ch[ch].phase = 1;
				break;
			}
		}
		PCG->ch[ch].gate = (v & mask[ch]);
	}

	PCG->gate = v;
}


static void write_counter(void *chip, int ch, int v)
{
	PCG8100 *PCG = chip;

	if (PCG->ch[ch].access == 1) {
		PCG->ch[ch].count = v;
	} else if (PCG->ch[ch].access == 2) {
		PCG->ch[ch].count = (v << 8);
	} else { /* access == 3 */
		if (PCG->ch[ch].access_step == 0) {
			PCG->ch[ch].low_count = v;
		} else {
			PCG->ch[ch].count = (UINT16) PCG->ch[ch].low_count | (v << 8);
		}
		PCG->ch[ch].access_step ^= 1;
	}

	if ((0 < PCG->ch[ch].count) && (PCG->ch[ch].count <= 266)) {
		/* 3993600[Clock] / 266 > 15000[Hz] */
		PCG->ch[ch].audible = FALSE;
	} else {
		PCG->ch[ch].audible = TRUE;
	}

	switch (PCG->ch[ch].mode) {
	case 0:
	case 4:
		if (PCG->ch[ch].access_step == 0) {
			PCG8100UpdateRequest(PCG->param);
			PCG->ch[ch].value = PCG->ch[ch].count;
			PCG->ch[ch].phase = 1;
		}
		break;

	case 1:
	case 5:
		break;

	case 2:
	case 3:
		if (PCG->ch[ch].access_step == 0) {
			if (PCG->ch[ch].phase == 0) {
				PCG8100UpdateRequest(PCG->param);
				PCG->ch[ch].value = PCG->ch[ch].count;
				PCG->ch[ch].phase = 1;
			}
		}
		break;
	}

}
void PCG8100Counter0(void *chip,int v)
{
	write_counter(chip, 0, v);
}
void PCG8100Counter1(void *chip,int v)
{
	write_counter(chip, 1, v);
}
void PCG8100Counter2(void *chip,int v)
{
	write_counter(chip, 2, v);
}


void PCG8100Control(void *chip,int v)
{
	PCG8100 *PCG = chip;
	int ch = (v >> 6) & 0x03;

	if (ch <= 2) {
		UINT8 access = (v >> 4) & 0x03;

		if (access >= 1) {
			UINT8 mode = (v >> 1) & 0x07;
			if (mode == 6) {
				mode = 2;
			} else if (mode == 7) {
				mode = 3;
			}

			if (PCG->ch[ch].mode != mode) {
				PCG8100UpdateRequest(PCG->param);

				PCG->ch[ch].mode = mode;
				PCG->ch[ch].phase = 0;
			}
			if (PCG->ch[ch].access != access) {
				PCG->ch[ch].access = access;
				PCG->ch[ch].access_step = 0;
			}

			PCG->ch[ch].bcd = v & 0x01;
		}
	}
}
