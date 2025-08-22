#include "stdio.h" // for printf
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "TaskScheduler.h"
#include "hardware/adc.h"
#include "math.h"
#include "AccelerateMotor.h"
#include "MovingAverageFilter.h"
/*
    In case of build issues after copying the template, delete the entire build directory and in VSCode:
    Restart VSCode, click on "Terminal" on the top bar, and select GCC 10.2.1 arm-none-eabi

    Overclocking: https://youtu.be/G2BuoFNLoDM (The "catch" is already fixed in CMakeLists.txt, uncomment the three lines at the bottom)
    PIO: https://youtu.be/JSis2NU65w8
    Explaining PIO ASM instructions: https://youtu.be/yYnQYF_Xa8g

    - Max X, 2022-01-07
*/

#define PULSE_PIN 18
#define EN_PIN 16
#define DIR_PIN 17
#define BUTTON_PIN 15
#define BUTTON_ACTIVE_STATE 0 // active LOW
#define POTENTIOMETER_PIN 26

#define MAX_ACCELERATION 360*5 // degrees/s^2
#define MAX_DECELERATION (MAX_ACCELERATION * 3)
#define MAX_SPEED (360 / 2)
#define UPDATE_RATE_HZ 100 // Hz (for motor)
#define POTENTIOMETER_UPDATE_RATE 20 // Hz
#define MOTOR_ANGLE_PER_FULLSTEP (1.8f / 2)
#define MICROSTEPS 32
#define ANGLE_PER_STEP (MOTOR_ANGLE_PER_FULLSTEP / MICROSTEPS)
#define POTENTIOMETER_MOVING_AVERAGE_N (POTENTIOMETER_UPDATE_RATE / 2)


AccelerateMotor motor(MAX_ACCELERATION, MAX_DECELERATION, 0, UPDATE_RATE_HZ, ANGLE_PER_STEP, PULSE_PIN);
MovingAverageFilter moving_average(POTENTIOMETER_MOVING_AVERAGE_N);

void motor_update_task() {
    motor.Update(gpio_get(BUTTON_PIN) == BUTTON_ACTIVE_STATE);
    AddTask(motor_update_task, 1000000 / UPDATE_RATE_HZ);
}

void potentiometer_update_task() {
    adc_select_input(POTENTIOMETER_PIN - 26); // adc channel 0 = pin 26
    uint16_t pot_val = adc_read();
    //printf("adc: %d\n", pot_val);

    const int scale_factor = 1000;
    int max_val = scale_factor * pot_val * MAX_SPEED / 4095;
    float averaged = (float)moving_average.PushValue(max_val) / scale_factor;
    motor.SetMaxSpeed(averaged);
    printf("averaged = %f\n", averaged);

    AddTask(potentiometer_update_task, 1000000 / POTENTIOMETER_UPDATE_RATE);
}

void blink_led_task() {
    static bool prev_state = 0;
    gpio_put(25, prev_state);
    prev_state = !prev_state;

    AddTask(blink_led_task, 500000);
}

int main() {
    
    stdio_init_all(); // for printf

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);


    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_pulls(BUTTON_PIN, 1, 0); // set pullup
    gpio_init(EN_PIN);
    gpio_set_dir(EN_PIN, GPIO_OUT);
    gpio_init(DIR_PIN);
    gpio_set_dir(DIR_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(POTENTIOMETER_PIN);
    

    gpio_put(EN_PIN, 0); // enable driver
    gpio_put(DIR_PIN, 1); 

    //InitPWM(PULSE_PIN);
    //SetPWMFreq(1000);

    
    AddTask(motor_update_task, 1000000 / UPDATE_RATE_HZ);
    AddTask(potentiometer_update_task, 1000000 / POTENTIOMETER_UPDATE_RATE);
    blink_led_task();


    while(1) { // rekt noob timeam
        UpdateTasks();
        
        //printf("asdasd\n");

        //gpio_put(25, 1);
        //gpio_put(PULSE_PIN, 1);
        
        //sleep_us(50);
        //gpio_put(25, 0);
        //gpio_put(PULSE_PIN, 0);
        
        //sleep_us(1000000 / UPDATE_RATE_HZ);
    }
}