#include "TrackedPWM.h"
#include "PulseCounter.pio.h"
#include <hardware/pwm.h>
#include <math.h>


TrackedPWM::TrackedPWM(uint pulse_pin, uint dir_pin) {

    pul_pin_ = pulse_pin;
    dir_pin_ = dir_pin;
    pio_ = pio1;
    sm_ = pio_claim_unused_sm(pio_, true);
    program_offset_ = pio_add_program(pio_, &PulseCounter_program);
    //(PIO pio, uint sm, uint offset, uint pulse_pin, uint dir_pin, float div)
    PulseCounter_program_init(pio_, sm_, program_offset_, pul_pin_, dir_pin, 1.0f);
    
    gpio_init(pulse_pin);

    gpio_set_function(pulse_pin, GPIO_FUNC_PWM);

    if (pulse_pin % 2) pwm_chan_ = PWM_CHAN_B;
    else pwm_chan_ = PWM_CHAN_A;

    pwm_slice_num_ = pwm_gpio_to_slice_num(pulse_pin);

    //pwm_set_wrap(pwm_slice_num_, 499);
    //pwm_set_chan_level(pwm_slice_num_, pwm_chan_, 0);
}

void TrackedPWM::SetPWMFreq(int freq) {
    pwm_set_enabled(pwm_slice_num_, false);
    const int MAX_OVF_VAL = 65535; // PWM block has 16 bit register
    float f_prescaler = floor((float) SYS_CLK_HZ / (freq * MAX_OVF_VAL)) + 1.0f;
    if (f_prescaler < 1.0f) f_prescaler = 1.0f;
    int ovf_val = SYS_CLK_HZ / (freq * f_prescaler);
    // MAX ovf = 65536
    // ovf = SYS_CLK / (freq * prescaler)
    // prescaler = sys_clk / (freq * max_ovf)
    pwm_set_clkdiv(pwm_slice_num_, f_prescaler);
    pwm_set_wrap(pwm_slice_num_, ovf_val);
    pwm_set_chan_level(pwm_slice_num_, pwm_chan_, ovf_val/2);
    pwm_set_counter(pwm_slice_num_, 0);
    pwm_set_enabled(pwm_slice_num_, true);
}

int32_t TrackedPWM::GetPulses() {
    // first clear the FIFO of the outdated reading
    pio_sm_get(pio_, sm_);
    // then read again
    return pio_sm_get_blocking(pio_, sm_);
}