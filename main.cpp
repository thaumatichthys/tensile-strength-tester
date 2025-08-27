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
#include "ButtonHandler.h"
#include "PIOPulse.h"
/*
    In case of build issues after copying the template, delete the entire build directory and in VSCode:
    Restart VSCode, click on "Terminal" on the top bar, and select GCC 10.2.1 arm-none-eabi

    Overclocking: https://youtu.be/G2BuoFNLoDM (The "catch" is already fixed in CMakeLists.txt, uncomment the three lines at the bottom)
    PIO: https://youtu.be/JSis2NU65w8
    Explaining PIO ASM instructions: https://youtu.be/yYnQYF_Xa8g

    - Max X, 2022-01-07
*/
PIOPulse pulse_counter(PULSE_PIN, DIR_PIN, 100);
AccelerateMotor motor(MAX_ACCELERATION, MAX_DECELERATION, 0, UPDATE_RATE_HZ, ANGLE_PER_STEP, PULSE_PIN);
MovingAverageFilter moving_average(POTENTIOMETER_MOVING_AVERAGE_N);
TwoButtonHold hold_buttons(UP_BUTTON_PIN, DN_BUTTON_PIN, RECORD_HOLD_DURATION, BUTTON_ACTIVE_STATE);
SSD1306_Driver display(DISPLAY_I2C_INST, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);


void motor_update_task() {
    bool up_button = gpio_get(UP_BUTTON_PIN) == BUTTON_ACTIVE_STATE;
    bool dn_button = gpio_get(DN_BUTTON_PIN) == BUTTON_ACTIVE_STATE;
    bool held = (up_button != dn_button); // logical XOR
    
    if (held) {
        if (up_button == MOTOR_UP_DIR_PIN_STATE)
            gpio_put(DIR_PIN, 1);
        else 
            gpio_put(DIR_PIN, 0);
    }
    motor.Update(held);
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
    //printf("averaged = %f\n", averaged);

    AddTask(potentiometer_update_task, 1000000 / POTENTIOMETER_UPDATE_RATE);
}

void blink_led_task() {
    static bool prev_state = 0;
    gpio_put(25, prev_state);
    prev_state = !prev_state;

    AddTask(blink_led_task, 500000);
}

void update_force_task() {
    float force = 15233.124142;
    float delta_position = 123;
    float speed = motor.GetCurrentSpeed();
    int elapsed_time = to_ms_since_boot(get_absolute_time()) - GetStartTime();
    int delta_steps = 123412;

    char temp_buf[32];
    snprintf(temp_buf, 32, "Fload: %-.3f N", force);
    //display.DrawText(1, 1, "                                ", 32);
    display.DrawText(1, 1, temp_buf, 32);
    display.Send();

    /*
        float load_cell_force_N_;
        float delta_position_mm_;
        float speed_degrees_sec_;
        int elapsed_time_ms_;
        int delta_steps_;
    */

    CSVData data = {
        force,
        delta_position,
        speed,
        elapsed_time,
        delta_steps
    };

    UpdateRecording(data);
    if (IsError()) {

        display.DrawText(1, 24, "SD error occurred", 17);
        display.Send();
    }
    int32_t in = pulse_counter.GetPulses();
    printf("\npulses: %d, bits = ", in);
    
    for (int i = 0; i < 32; i++) {
        printf("%d", (bool)(in & (1 << (31 - i))));
    }


    AddTask(update_force_task, FORCE_SENSOR_UPDATE_PERIOD * 1000);
}

int main() {
    
    stdio_init_all(); // for printf

    //WriteTestFile();

    
    display.RenderThaumatichthys();
    display.DrawText(1, 1, "Max Xiang", 9);
    display.DrawText(65, 24, "23/08/2025", 10);
    display.Send();

    sleep_ms(500);
    display.ClearBuffer();
    display.Send();

    CSVData dummy;


    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);


    //gpio_init(BUTTON_PIN);
    //gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_pulls(UP_BUTTON_PIN, 1, 0); // set pullup
    gpio_set_pulls(DN_BUTTON_PIN, 1, 0); // set pullup
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
    update_force_task();


    while(1) { // rekt noob timeam
        UpdateTasks();
        
        //printf("asdasd\n");

        //gpio_put(25, 1);
        //gpio_put(PULSE_PIN, 1);
        
        //sleep_us(50);
        //gpio_put(25, 0);
        //gpio_put(PULSE_PIN, 0);
        
        //sleep_us(1000000 / UPDATE_RATE_HZ);
        if (hold_buttons.Update()) {
            
            printf("asd buttons preswsed\n");
            if (IsRecording()) {
                StopRecording();
                display.DrawText(1, 24, "Stopped Recording", 17);
            }
            else {
                bool start_status = StartRecording();
                if (start_status) {
                    display.DrawText(1, 24, "Started Recording", 17);
                }
                else {
                    display.DrawText(1, 24, "SD Card error", 13);
                }
            }
            display.Send();
        }
    }
}