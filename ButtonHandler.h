#pragma once 
#include <pico/stdlib.h>


class TwoButtonHold {
public:
    TwoButtonHold(uint pinA, uint pinB, int holdTime, bool activeState);
    bool Update();
private:
    uint pinA_;
    uint pinB_;
    uint32_t time_last_fail_; // last time that condition was not met
    bool activeState_;
    int holdTime_;
    bool previously_activated_ = false;
};