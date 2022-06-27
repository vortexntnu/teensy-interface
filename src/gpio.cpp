#include "gpio.h"
#include "Arduino.h"

namespace gpio {

void setup()
{
    IMXRT_GPIO6.GDIR = 0xFFFFFFFF;
    IMXRT_GPIO7.GDIR = 0xFFFFFFFF;
    IMXRT_GPIO6.DR_CLEAR = 0xFFFFFFFF;
    IMXRT_GPIO7.DR_CLEAR = 0xFFFFFFFF;
    IMXRT_GPIO6.GDIR = 0x0;
    IMXRT_GPIO7.GDIR = 0x0;
    #ifdef SERIAL_DEBUG
    dump_GPIO_registers(); 
    #endif
}

void configPin(int pin, int mode, IMXRT_GPIO_t& GPIO_n)  
{
    // set to output mode.
    if (mode) {
        GPIO_n.GDIR |= (1 << pin);
    }
    // set to input mode.
    else {
        GPIO_n.GDIR &= !(1 << pin); 
    }
}


inline void read_pin(int pin, uint16_t* data, uint32_t reg)
{
    *data |= (((reg)&(0x1<<pin))>>(pin-core_to_sample_bit[pin])); 
}

void write_pin(int pin, uint8_t value, IMXRT_GPIO_t& GPIO_n)
{
    if (value) {
        GPIO_n.DR_SET |= (1 << pin); 
    }
    else {
        GPIO_n.DR_CLEAR |= (1 << pin); 
    }
}

#ifdef ADC_DEBUG_CHRISTIAN
void toggle_pin(int pin, IMXRT_GPIO_t& GPIO_n) {
    GPIO_n.DR_TOGGLE |= (1 << pin);
}

#endif
void test_write()
{
    IMXRT_GPIO6.GDIR |= (0x1 << CORE_PIN21_BIT);
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CORE_PIN21_BIT))>>CORE_PIN21_BIT);
    IMXRT_GPIO6.DR_SET |= (0x1 << CORE_PIN21_BIT);
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CORE_PIN21_BIT))>>CORE_PIN21_BIT);
    IMXRT_GPIO6.DR_CLEAR |= (0x1 << CORE_PIN21_BIT);
}

uint16_t read_pins()
{
    uint16_t data = 0;
    uint32_t DR = IMXRT_GPIO7.DR;

    read_pin(CORE_PIN38_BIT, &data,DR);
    read_pin(CORE_PIN39_BIT, &data,DR);
    read_pin(CORE_PIN40_BIT, &data,DR);
    read_pin(CORE_PIN41_BIT, &data,DR);
    
    read_pin(CORE_PIN14_BIT, &data,DR);
    read_pin(CORE_PIN15_BIT, &data,DR);
    read_pin(CORE_PIN16_BIT, &data,DR);
    read_pin(CORE_PIN17_BIT, &data,DR);
    read_pin(CORE_PIN18_BIT, &data,DR);
    read_pin(CORE_PIN19_BIT, &data,DR);
    read_pin(CORE_PIN20_BIT, &data,DR);
    read_pin(CORE_PIN21_BIT, &data,DR);

    return data;
}

void dump_GPIO_registers() {
    Serial.printf("GPIOX.GDIR: 0x%X\nGPIOX.DR: 0x%X\n", IMXRT_GPIO6.GDIR, IMXRT_GPIO6.DR);
    Serial.printf("GPIOX.GDIR: 0x%X\nGPIOX.DR: 0x%X\n", IMXRT_GPIO7.GDIR, IMXRT_GPIO7.DR);
}

} // gpio