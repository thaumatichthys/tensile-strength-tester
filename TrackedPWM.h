#pragma once
#include <pico/stdlib.h>
#include <hardware/pio.h>



class TrackedPWM {
public:
    TrackedPWM(uint pulse_pin, uint dir_pin);
    void SetPWMFreq(int freq);
    int32_t GetPulses();
private:
    uint pwm_slice_num_;
    uint pwm_chan_;

    uint pul_pin_;
    uint dir_pin_;
    uint sm_;
    uint program_offset_;
    pio_hw_t* pio_;
};