#pragma once
#include <pico/stdlib.h>
#include <vector>


class MovingAverageFilter {
public:
    MovingAverageFilter(int n_samples);
    int PushValue(int new_val);
private:
    std::vector<int> buffer_;
    int write_idx_ = 0;
    int n_samples_;
    int accumulator_ = 0;
};