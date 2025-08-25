#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_SPEED       800000

#define DEVICE_ADDRESS  0x3C
#define DISPLAY_HEIGHT  32
#define DISPLAY_WIDTH   128

#define BUFFER_SIZE     DISPLAY_HEIGHT * DISPLAY_WIDTH / 8

class SSD1306_Driver {
    private:
    i2c_inst *instance;
    public:
    uint8_t orientation_bits = 0b00000000;
    void WritePixel(uint8_t buffer[], uint8_t x, uint8_t y, bool value);
    uint8_t display_buffer[BUFFER_SIZE];
    SSD1306_Driver(i2c_inst *i2c, uint sda, uint scl);
    void Send();
    void DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool polarity);
    void DrawText(uint8_t pos_x, uint8_t pos_y, const char* text, uint8_t n);
    void DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool polarity);
    void RenderThaumatichthys();
    void ClearBuffer();
};

const uint8_t refresh_setup_values[] = {
    0x22, // page address write
    0,
    0xFF,
    0x21,
    0,
    127 // width-1
};

const uint8_t set_up_values[] = { // clean this up
    // yoinked from https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp
    // and its header file
    0xAE,
    0xD5,
    0x80,
    0xA8,

    31,

    0xD3,
    0x00,
    0x00,
    0x8D,

    0x14, // vccstate, idk which one to set
        
    0x20,
    0x00,
    0xA0 | 0x1,
    0xC8,

    0xDA, // set_compins
    0x02, // compins
    0x81, // set contrast
    0x8F, // contrast

    0xD9, // set precharge
    0xF1, // not external

    0xDB,
    0x40,
    0xA4,
    0xA6,
    0x2E,
    0xAF
};