#pragma once 
#include <pico/stdlib.h>

#define HX711_GAIN_32 2
#define HX711_GAIN_64 3
#define HX711_GAIN_128 1

class HX711 {
public:
    HX711(uint DO, uint CK, int gain);
    int32_t GetRawReading();
    void SetGain(int gain);
    float GetCompensatedReading();
private:
    uint do_;
    uint ck_;
    int gain_;
};