#pragma once
#include "pico/stdlib.h"
#include "PWMFunc.h"


class PWMFunc {
public:
    PWMFunc(uint pin);
    void SetPWMFreq(int freq);
private:
    uint pwm_slice_num_;
    uint pwm_chan_;
};
