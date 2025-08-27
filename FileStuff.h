#pragma once
#include "pico/stdlib.h"
#include "ff.h"


struct CSVData {
    float load_cell_force_N_;
    float delta_position_mm_;
    float speed_degrees_sec_;
    int elapsed_time_ms_;
    int delta_steps_;
};

void WriteTestFile();
void ErrorHandler(FRESULT error);
bool StartRecording();
void UpdateRecording(CSVData data);
void StopRecording();
bool IsRecording();
bool IsError();
uint32_t GetStartTime();