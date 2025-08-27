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

// MOTOR 
#define MAX_ACCELERATION 360*5 // degrees/s^2
#define MAX_DECELERATION (MAX_ACCELERATION * 3)
#define MAX_SPEED (360 / 2)
#define UPDATE_RATE_HZ 100 // Hz (for motor)
#define MOTOR_ANGLE_PER_FULLSTEP (1.8f / 2)
#define MICROSTEPS 32
#define ANGLE_PER_STEP (MOTOR_ANGLE_PER_FULLSTEP / MICROSTEPS)
#define MOTOR_UP_DIR_PIN_STATE 1 // this will flip the direction of the motor

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
// #define BUTTON_ACTIVE_STATE 0 // active low

// FORCE SENSOR
#define FORCE_SENSOR_UPDATE_PERIOD 1 // ms