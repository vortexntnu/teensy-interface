#include "core_pins.h"

const uint8_t core_to_sample_bit[32] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    9,8,4,5,2,3,7,6,0,0,10,11,0,1,
    30,31
};

void gpio_setup();

inline void read_pin(int pin, uint16_t* data);

uint16_t read_pins();