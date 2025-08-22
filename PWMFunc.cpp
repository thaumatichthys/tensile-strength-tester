#include "PWMFunc.h"
#include <hardware/pwm.h>
#include <math.h>


PWMFunc::PWMFunc(uint pin) {
    
    gpio_init(pin);

    gpio_set_function(pin, GPIO_FUNC_PWM);

    if (pin % 2) pwm_chan_ = PWM_CHAN_B;
    else pwm_chan_ = PWM_CHAN_A;

    pwm_slice_num_ = pwm_gpio_to_slice_num(pin);

    //pwm_set_wrap(pwm_slice_num_, 499);
    //pwm_set_chan_level(pwm_slice_num_, pwm_chan_, 0);
}

void PWMFunc::SetPWMFreq(int freq) {
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