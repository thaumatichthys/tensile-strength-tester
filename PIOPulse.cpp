#include "PIOPulse.h"
#include "PulseCounter.pio.h"
#include "math.h"


PIOPulse::PIOPulse(uint pulse_pin, uint dir_pin, int timeout_ms) {
    pul_pin_ = pulse_pin;
    dir_pin_ = dir_pin;
    pio_ = pio1;
    sm_ = pio_claim_unused_sm(pio_, true);
    program_offset_ = pio_add_program(pio_, &PulseCounter_program);
    timeout_ = timeout_ms;
    //(PIO pio, uint sm, uint offset, uint pulse_pin, uint dir_pin, float div)
    PulseCounter_program_init(pio_, sm_, program_offset_, pul_pin_, dir_pin, 1.0f);
}
int32_t PIOPulse::GetPulses() {
    // first clear the FIFO of the outdated reading
    pio_sm_get(pio_, sm_);
    // then read again
    return pio_sm_get_blocking(pio_, sm_);
}