#ifndef PCGINTF_H
#define PCGINTF_H

WRITE8_HANDLER( PCG8100_gate_port_0_w );
WRITE8_HANDLER( PCG8100_counter0_port_0_w );
WRITE8_HANDLER( PCG8100_counter1_port_0_w );
WRITE8_HANDLER( PCG8100_counter2_port_0_w );
WRITE8_HANDLER( PCG8100_control_port_0_w );

extern void PCG8100_set_volume(float volume);

#endif
