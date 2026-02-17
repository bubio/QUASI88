#ifndef PCGINTF_H
#define PCGINTF_H

#define MAX_PCG8100 1

struct PCG8100interface
{
	int num;
	int baseclock;
	int mixing_level[MAX_PCG8100];
};


WRITE_HANDLER( PCG8100_gate_port_0_w );
WRITE_HANDLER( PCG8100_counter0_port_0_w );
WRITE_HANDLER( PCG8100_counter1_port_0_w );
WRITE_HANDLER( PCG8100_counter2_port_0_w );
WRITE_HANDLER( PCG8100_control_port_0_w );

int PCG8100_sh_start(const struct MachineSound *msound);
void PCG8100_sh_stop(void);
void PCG8100_sh_reset(void);

void PCG8100UpdateRequest(int chip);



int PCG8100Init(int num, int clock, int sample_rate );
void PCG8100Shutdown(void);
void PCG8100ResetChip(int num);
void PCG8100UpdateOne(int num, INT16 *buffer, int length);

void PCG8100Gate(int n,int v);
void PCG8100Counter0(int n,int v);
void PCG8100Counter1(int n,int v);
void PCG8100Counter2(int n,int v);
void PCG8100Control(int n,int v);

#endif
