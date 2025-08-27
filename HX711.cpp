#include "HX711.h"
#include <math.h>
#include "ProgramConfig.h"


HX711::HX711(uint DO, uint CK, int gain) {
    gpio_init(DO);
    gpio_init(CK);

    gpio_set_dir(DO, GPIO_IN);
    gpio_set_dir(CK, GPIO_OUT);

    gpio_put(CK, 0);

    do_ = DO;
    ck_ = CK;

    gain_ = gain;
    SetGain(gain);
}
int32_t HX711::GetRawReading() {
    int32_t val = 0;
    // basically SPI (24 bit)
    for (int i = 0; i < 24; i++) {
        gpio_put(ck_, 1);
        sleep_us(1);
        bool bit = gpio_get(do_);
        gpio_put(ck_, 0);
        sleep_us(1);
        val |= ((uint32_t)bit << (24 - 1 - i));
    }
    // set gain for next reading (see datasheet of HX711)
    for (int i = 0; i < gain_; i++) {
        gpio_put(ck_, 1);
        sleep_us(1);
        gpio_put(ck_, 0);
        sleep_us(1);
    }
    // Sign extend 24-bit to 32-bit
    if (val & 0x800000) {
        val |= 0xFF000000;
    }
    return val;
}
void HX711::SetGain(int gain) {
    gain_ = gain;
    uint32_t dummy = GetRawReading();
}
float HX711::GetCompensatedReading() {
    return (float)GetRawReading() * LOAD_CELL_CONSTANT;
}




