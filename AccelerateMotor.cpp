#include "AccelerateMotor.h"
#include <math.h>
#include <stdio.h>

AccelerateMotor::AccelerateMotor(
        float max_acceleration_degrees_ss, 
        float max_deceleration_degrees_ss, 
        float max_speed, 
        float update_rate_hz, 
        float motor_angle_per_step, 
        uint motor_pin
    ) : pwm_(motor_pin) {
    
    velocity_ = 0.0f;
    max_velocity_ = max_speed;
    update_rate_ = update_rate_hz;
    acceleration_ = max_acceleration_degrees_ss;
    angle_per_step_ = motor_angle_per_step;
    deceleration_ = max_deceleration_degrees_ss;
}

void AccelerateMotor::Update(bool buttonPressed) {
    if (buttonPressed) {
        float increment = acceleration_ * (1.0f / update_rate_);
        // split into two regimes: large error and small error
        float abs_error = abs(velocity_ - max_velocity_);

        if (abs_error > increment) {
            // large error
            if (velocity_ < max_velocity_) {
                velocity_ += increment;
            } 
            else if (velocity_ > max_velocity_) {
                velocity_ -= increment;
            }
        }
        else {
            // small error
            velocity_ = max_velocity_;
        }

        
        printf("velocity = %f, maxvelocity = %f\n", velocity_, max_velocity_);
    }
    else {
        // decrease speed
        float increment = deceleration_ * (1.0f / update_rate_);
        if (velocity_ > 0) {
            velocity_ -= increment;
        }
        else if (velocity_ < 2 * increment) {
            velocity_ = 0;
        }
    }

    // convert velocity into pulses per second
    // velocity is degrees/sec
    // k = degrees/pulse
    // hz = velocity / k
    float freq = velocity_ / angle_per_step_;
    pwm_.SetPWMFreq(round(freq));
}
void AccelerateMotor::SetMaxSpeed(float new_val) {
    max_velocity_ = new_val;
}
