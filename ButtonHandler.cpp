#include "ButtonHandler.h"



TwoButtonHold::TwoButtonHold(uint pinA, uint pinB, int holdTime, bool activeState) {
    pinA_ = pinA;
    pinB_ = pinB;
    activeState_ = activeState;
    gpio_init(pinA);
    gpio_set_dir(pinA, GPIO_IN);
    gpio_init(pinB);
    gpio_set_dir(pinB, GPIO_IN);
    time_last_fail_ = 0;
    holdTime_ = holdTime;
}
bool TwoButtonHold::Update() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    bool pinAState = gpio_get(pinA_);
    bool pinBState = gpio_get(pinB_);
    bool condition_met = true;
    // check condition
    if (!((pinAState == activeState_) && (pinBState == activeState_))) {
        time_last_fail_ = current_time;
        condition_met = false;
        previously_activated_ = false;
    }

    if ((current_time - time_last_fail_ > holdTime_) && !previously_activated_) {
        previously_activated_ = true;
        return true;
    }
    return false;
}