#ifndef PCG_H
#define PCG_H

void PCG8100UpdateRequest(void *param);

void * PCG8100Init(void *param, int index, int clock, int sample_rate);
void PCG8100Shutdown(void *chip);
void PCG8100ResetChip(void *chip);
void PCG8100UpdateOne(void *chip, stream_sample_t *buffer, int length);

void PCG8100Gate(void *chip,int v);
void PCG8100Counter0(void *chip,int v);
void PCG8100Counter1(void *chip,int v);
void PCG8100Counter2(void *chip,int v);
void PCG8100Control(void *chip,int v);

#endif
