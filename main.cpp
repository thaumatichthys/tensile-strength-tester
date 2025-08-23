#include "stdio.h" // for printf
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "TaskScheduler.h"
#include "hardware/adc.h"
#include "math.h"
#include "AccelerateMotor.h"
#include "MovingAverageFilter.h"
#include "FileStuff.h"
#include "ProgramConfig.h"
#include "ssd1306_driver.h"
#include "hardware/i2c.h"
/*
    In case of build issues after copying the template, delete the entire build directory and in VSCode:
    Restart VSCode, click on "Terminal" on the top bar, and select GCC 10.2.1 arm-none-eabi

    Overclocking: https://youtu.be/G2BuoFNLoDM (The "catch" is already fixed in CMakeLists.txt, uncomment the three lines at the bottom)
    PIO: https://youtu.be/JSis2NU65w8
    Explaining PIO ASM instructions: https://youtu.be/yYnQYF_Xa8g

    - Max X, 2022-01-07
*/

AccelerateMotor motor(MAX_ACCELERATION, MAX_DECELERATION, 0, UPDATE_RATE_HZ, ANGLE_PER_STEP, PULSE_PIN);
MovingAverageFilter moving_average(POTENTIOMETER_MOVING_AVERAGE_N);

void motor_update_task() {
    motor.Update(gpio_get(BUTTON_PIN) == BUTTON_ACTIVE_STATE);
    AddTask(motor_update_task, 1000000 / UPDATE_RATE_HZ);
}

/*
    BEHAVIOUR: 

    > UP --> motor goes up
    > DN --> motor goes down
    
    > UP+DN HOLD 3s --> starts recording
    > UP+DN HOLD 3s --> stops recording

    record: elapsed time, force, delta position, speed

*/

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

    WriteTestFile();

    SSD1306_Driver display(DISPLAY_I2C_INST, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    display.RenderThaumatichthys();
    display.DrawText(1, 1, "Max Xiang", 9);
    display.DrawText(65, 24, "23/08/2025", 10);
    display.Send();

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