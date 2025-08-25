#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/i2c.h"
#include "ssd1306_driver.h"
#include "typeface.h"

SSD1306_Driver::SSD1306_Driver(i2c_inst *i2c_instance, uint sda, uint scl) {
    gpio_init(sda);
    gpio_init(scl);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    this->instance = i2c_instance;
    i2c_init(this->instance, I2C_SPEED);
    for (int i = 0; i < sizeof(set_up_values); i++) { // shove all the hex bytes into the chip, tbh I don't know what half of them do :/
        uint8_t tx[] = {
            0x00, // write address
            set_up_values[i]
        };
        i2c_write_blocking(this->instance, DEVICE_ADDRESS, tx, 2, false);
    }
    for (int i = 0; i < sizeof(refresh_setup_values); i++) { // clear the display
        uint8_t tx[] = {
            0x00, // write address
            refresh_setup_values[i]
        };
        i2c_write_blocking(this->instance, DEVICE_ADDRESS, tx, 2, false);
    }
    for (int i = 0; i < BUFFER_SIZE; i++) {
        this->display_buffer[i] = 0;
        uint8_t tx[] = {
            0x40, // write address
            this->display_buffer[i] // data
        };
        i2c_write_blocking(this->instance, DEVICE_ADDRESS, tx, 2, false);
    }
}

void SSD1306_Driver::Send() {
    for (int i = 0; i < sizeof(refresh_setup_values); i++) {
        uint8_t tx[] = {
            0x00, // write address
            refresh_setup_values[i]
        };
        i2c_write_blocking(this->instance, DEVICE_ADDRESS, tx, 2, false);
    }
    for (int i = 0; i < BUFFER_SIZE; i++) {
        // write the buffer to the display
        uint8_t tx[] = {
            0x40, // write address
            this->display_buffer[i] // data
        };
        i2c_write_blocking(this->instance, DEVICE_ADDRESS, tx, 2, false);
    }
}

void SSD1306_Driver::ClearBuffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        display_buffer[i] = 0;
    }
}

void SSD1306_Driver::WritePixel(uint8_t buffer[], uint8_t x, uint8_t y, bool value) {
    // (0,0) is the top left corner
    if (x < 0 || x >= DISPLAY_WIDTH) { return; }
    if (y < 0 || y >= DISPLAY_HEIGHT) { return; }
    if (this->orientation_bits & (1 << 0)) {
        x = DISPLAY_WIDTH - (x + 1);
    }
    if (this->orientation_bits & (1 << 1)) {
        y = DISPLAY_HEIGHT - (y + 1);
    }
    uint8_t bit = y % 8;
    uint8_t column = (y - bit) / 8;
    uint16_t index = x + DISPLAY_WIDTH * column;
    uint8_t bits = (uint8_t) 1 << bit;
    if (value) {
        buffer[index] |= bits;
    }
    else {
        buffer[index] &= ~bits;
    }
    //printf("\nbit: %d. index: %d. column: %d. x: %d. y: %d\n", bit, index, column, x, y);
}

void SSD1306_Driver::DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool polarity) {
    uint8_t x_upperbound = x2;
    uint8_t x_lowerbound = x1;
    if (x1 > x2) {
        x_upperbound = x1;
        x_lowerbound = x2;
    }
    uint8_t y_upperbound = y2;
    uint8_t y_lowerbound = y1;
    if (y1 > y2) {
        y_upperbound = y1;
        y_lowerbound = y2;
    }

    uint8_t dx = x_upperbound - x_lowerbound;
    uint8_t dy = y_upperbound - y_lowerbound;

    float ratio = (float) dx / dy;

    for (int i = 0; i < dx; i++) {
        uint8_t x = i + x_lowerbound;
        uint8_t y = (float) i / ratio + y_lowerbound;
        this->WritePixel(this->display_buffer, x, y, polarity);
    }
}

void SSD1306_Driver::DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool polarity) {
    uint8_t x_upperbound = x2;
    uint8_t x_lowerbound = x1;
    if (x1 > x2) {
        x_upperbound = x1;
        x_lowerbound = x2;
    }
    uint8_t y_upperbound = y2;
    uint8_t y_lowerbound = y1;
    if (y1 > y2) {
        y_upperbound = y1;
        y_lowerbound = y2;
    }
    for (uint8_t x = x_lowerbound; x <= x_upperbound; x++) {
        for (uint8_t y = y_lowerbound; y <= y_upperbound; y++) {
            this->WritePixel(this->display_buffer, x, y, polarity);
        }
    }
}

void SSD1306_Driver::DrawText(uint8_t pos_x, uint8_t pos_y, const char* text, uint8_t n) {
    const uint8_t character_spacing = 6; // pixels wide per char
    for (int i = 0; i < n && text[i] != '\0'; i++) {
        for (int x = pos_x + character_spacing * i;
             x < pos_x + character_spacing * (i + 1);
             x++) {
            for (int y = pos_y; y < pos_y + 8; y++) {
                uint8_t x_index = (x - pos_x) % character_spacing;
                bool value = false;
                if (x_index < 5) {
                    value = (font[(uint8_t)text[i]][x_index] & (1 << (y - pos_y)));
                }
                this->WritePixel(this->display_buffer, x, y, value);
            }
        }
    }
}

void SSD1306_Driver::RenderThaumatichthys() {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            this->WritePixel(this->display_buffer, x, y, (bool)(thaumatichthys[x] & (1 << y)));
        }
    }
}