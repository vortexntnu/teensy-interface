#pragma once
#include "core_pins.h"

namespace gpio {

const uint8_t core_to_sample_bit[32] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    9,8,4,5,2,3,7,6,0,0,10,11,0,1,
    30,31
};

void setup();

// set pin to input or output mode. 1->output. 0->input. 
// param pin: should be of type CORE_PINX_BIT, i.e. the pins
// number on the teensy. 
void configPin(int pin, int mode, IMXRT_GPIO_t& GPIO_n);

inline void read_pin(int pin, uint16_t* data);

void write_pin(int pin, uint8_t value, IMXRT_GPIO_t& GPIO_n); 

uint16_t read_pins();

void dump_GPIO_registers(); 


} // namespace gpio