#pragma once
#include "core_pins.h"

namespace gpio {

//// used to shift reading of 1 GPIO to the right position
//// in read_pin()
const uint8_t core_to_sample_bit[32] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    9,8,4,5,2,3,7,6,0,0,10,11,0,1,
    30,31
};

/// @brief configures the parallel bus pins to adc as outputs. why??
void setup();

// set pin to input or output mode. 1->output. 0->input. 
// param pin: should be of type CORE_PINX_BIT, i.e. the pins
// number on the teensy. 
void configPin(int pin, int mode, IMXRT_GPIO_t& GPIO_n);

//// reads the pin of register reg, and modifies the corresponding bit in data
//// (using const uint8_t core_to_sample_bit[32])
inline void read_pin(int pin, uint16_t* data, uint32_t reg);

void write_pin(int pin, uint8_t value, IMXRT_GPIO_t& GPIO_n); 

void toggle_pin(int pin, IMXRT_GPIO_t& GPIO_n);

//// reads consecutively all pins 14-21 and 38-42 (see adc.h to see which pins they represent: it is the parallel data bus
uint16_t read_pins();

//// prints values of registers IMXRT_GPIO6.GDIR, IMXRT_GPIO6.DR and IMXRT_GPIO7.GDIR, IMXRT_GPIO7.DR
void dump_GPIO_registers(); 


} // namespace gpio