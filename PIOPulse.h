#pragma once
#include <pico/stdlib.h>
#include <hardware/pio.h>


class PIOPulse {
public:
    PIOPulse(uint pulse_pin, uint dir_pin, int timeout_ms);
    int32_t GetPulses();
private:
    uint pul_pin_;
    uint dir_pin_;
    uint sm_;
    uint program_offset_;
    pio_hw_t* pio_;
    int timeout_;
};