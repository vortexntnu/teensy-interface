#include "gpio.h"

void gpio_setup()
{
    IMXRT_GPIO6.GDIR = 0x0;
}

inline void read_pin(int pin, uint16_t* data)
{
    *data |= (((IMXRT_GPIO6.DR)&(0x1<<pin))>>(pin-core_to_sample_bit[pin])); 
}


uint16_t read_pins()
{
    uint16_t data =0;

    read_pin(CORE_PIN38_BIT, &data);
    read_pin(CORE_PIN39_BIT, &data);
    read_pin(CORE_PIN40_BIT, &data);
    read_pin(CORE_PIN41_BIT, &data);
    
    read_pin(CORE_PIN14_BIT, &data);
    read_pin(CORE_PIN15_BIT, &data);
    read_pin(CORE_PIN16_BIT, &data);
    read_pin(CORE_PIN17_BIT, &data);
    read_pin(CORE_PIN18_BIT, &data);
    read_pin(CORE_PIN19_BIT, &data);
    read_pin(CORE_PIN20_BIT, &data);
    read_pin(CORE_PIN21_BIT, &data);

    return data;
}
