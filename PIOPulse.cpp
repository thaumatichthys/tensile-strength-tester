#include "PIOPulse.h"
#include "PulseMotor.pio.h"
#include "math.h"


PIOPulse::PIOPulse(uint pulse_pin, uint dir_pin) {
    pul_pin_ = pulse_pin;
    dir_pin_ = dir_pin;
    pio_ = pio1;
    sm_ = pio_claim_unused_sm(pio_, true);
    program_offset_ = pio_add_program(pio_, &PulseMotor_program);

    //(PIO pio, uint sm, uint offset, uint pulse_pin, uint dir_pin, float div)
    PulseMotor_program_init(pio_, sm_, program_offset_, pulse_pin, dir_pin, 65535.0f);
}
void PIOPulse::SetFreq(int freq, bool dir) {
    float divider = (float) SYS_CLK_HZ / (float) freq / 6; // PIO prog takes 6 cycles to produce one clock cycle 

    const int MAX_OVF_VAL = 65535; // PWM block has 16 bit register
    float f_prescaler = floor((float) SYS_CLK_HZ / (freq * MAX_OVF_VAL)) + 1.0f;
    if (f_prescaler < 1.0f) f_prescaler = 1.0f;
    int ovf_val = SYS_CLK_HZ / (freq * f_prescaler);


    pio_sm_set_clkdiv(pio_, sm_, divider);
}