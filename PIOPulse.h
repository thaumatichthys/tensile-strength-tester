#pragma once
#include <pico/stdlib.h>
#include <hardware/pio.h>


class PIOPulse {
public:
    PIOPulse(uint pulse_pin, uint dir_pin);
    void SetFreq(int freq, bool dir);
private:
    uint pul_pin_;
    uint dir_pin_;
    uint sm_;
    uint program_offset_;
    pio_hw_t* pio_;

};