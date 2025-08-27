#pragma once 


// PINS 
#define PULSE_PIN 1//18
#define EN_PIN 4//16
#define DIR_PIN 0//17
//#define BUTTON_PIN 15
#define UP_BUTTON_PIN 15
#define DN_BUTTON_PIN 14
#define POTENTIOMETER_PIN 26

#define SD_DAT3_CS_PIN 22
#define SD_CMD_MOSI_PIN 18
#define SD_CLK_SCLK_PIN 17
#define SD_DAT0_MISO_PIN 19
#define SD_DAT1_X_PIN 20
#define SD_DAT2_X_PIN 21

#define REC_LED_PIN 11

// MOTOR 
#define MAX_ACCELERATION 360*5 // degrees/s^2
#define MAX_DECELERATION (MAX_ACCELERATION * 3)
#define MAX_SPEED (360 / 2)
#define UPDATE_RATE_HZ 100 // Hz (for motor)
#define MOTOR_ANGLE_PER_FULLSTEP (1.8f / 2)
#define MICROSTEPS 32
#define ANGLE_PER_STEP (MOTOR_ANGLE_PER_FULLSTEP / MICROSTEPS)
#define MOTOR_UP_DIR_PIN_STATE 1 // this will flip the direction of the motor
#define LEAD_SCREW_MM_PER_REVOLUTION 2.3

// POTENTIOMETER
#define POTENTIOMETER_UPDATE_RATE 20 // Hz
#define POTENTIOMETER_MOVING_AVERAGE_N (POTENTIOMETER_UPDATE_RATE / 2)

// BUTTONS
#define BUTTON_ACTIVE_STATE 0 // active LOW

// DISPLAY
#define DISPLAY_SDA_PIN 2
#define DISPLAY_SCL_PIN 3
#define DISPLAY_I2C_INST i2c1

// MISC
#define RECORD_HOLD_DURATION 500 // ms
#define TARE_HOLD_DURATION 2000 // ms
#define RECORD_LED_BLINK_PERIOD 200 // ms
#define ROUND_ELAPSED_TIME true
// #define BUTTON_ACTIVE_STATE 0 // active low

// FORCE SENSOR
#define FORCE_SENSOR_UPDATE_PERIOD 50 // ms
#define ADC_PER_VOLT (8388608.0 / 0.02) // cuz sign extended (also, 20mV full range in ONE direction)
#define SENSOR_MAX_VOLTS_PER_VOLT 0.002 // this is also max value in ONE direction
#define SENSOR_MAX_LOAD_KGF 100.0
#define SENSOR_INPUT_VOLTS 4.68 // ??????????
#define GRAVITY 9.807
#define LOAD_CELL_CONSTANT (SENSOR_MAX_LOAD_KGF * GRAVITY / (ADC_PER_VOLT * SENSOR_MAX_VOLTS_PER_VOLT * SENSOR_INPUT_VOLTS))

/*
    load cell: at 100 kgf, output = 2 mV/V (output/input)
    ADC: 20mV for 2^24
    adc_per_volt = 2^24 / (20*10^-3)
    volts_per_100kg = (2.0 * VREG * 10^-3)
    ones_per_100 = 100
    N_per_kgf = 9.807
*/