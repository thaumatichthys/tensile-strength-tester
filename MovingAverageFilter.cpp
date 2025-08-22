#include "MovingAverageFilter.h"

MovingAverageFilter::MovingAverageFilter(int n_samples) {
    buffer_.resize(n_samples);
    n_samples_ = n_samples;
}
int MovingAverageFilter::PushValue(int new_val) {
    int previous = buffer_[write_idx_];
    buffer_[write_idx_] = new_val;
    accumulator_ += new_val;
    accumulator_ -= previous;
    write_idx_ = (write_idx_ + 1) % n_samples_;
    
    return accumulator_ / n_samples_;
}