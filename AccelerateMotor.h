#pragma once
#include <pico/stdlib.h>
#include "TrackedPWM.h"


class AccelerateMotor {
public:
    AccelerateMotor(
        float max_acceleration_degrees_ss, 
        float max_deceleration_degrees_ss, 
        float max_speed, 
        float update_rate_hz, 
        float motor_angle_per_step, 
        uint motor_pin,
        uint dir_pin
    );
    void Update(bool buttonPressed);
    void SetMaxSpeed(float new_val);
    float GetCurrentSpeed();
    int32_t GetPulses();
private:
    float velocity_ = 0;
    float max_velocity_;
    float update_rate_;
    float acceleration_;
    float deceleration_;
    float angle_per_step_;

    TrackedPWM pwm_;
};
