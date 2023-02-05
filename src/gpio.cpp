#include "gpio.h"

#include "Arduino.h"

namespace gpio
{

    /*
        Basic explanations:
        By including core_pins.h in the header file, we have access to register names of Teensy

    */

    void setup()
    {
#ifdef SERIAL_DEBUG
        Serial.printf("Setting up GPIO registers\n");
#endif
        //! For DMA we need to use the slower ports (1), not the fast mode
        //* bad way of doing, there are other pins on these ports that are not used for the ADC, they might be used by something else
        IMXRT_GPIO6.GDIR = 0xFFFFFFFF; /// sets as output
        ////IMXRT_GPIO7.GDIR = 0xFFFFFFFF;
        IMXRT_GPIO6.DR_CLEAR = 0xFFFFFFFF; /// clears the pins(outputting 0)
        ////IMXRT_GPIO7.DR_CLEAR = 0xFFFFFFFF;
        IMXRT_GPIO6.GDIR = 0x0; //? should do the same as the 2 lines above???
                                ////IMXRT_GPIO7.GDIR = 0x0;
#ifdef SERIAL_DEBUG
        dump_GPIO_registers();
#endif
    }

    void configPin(int pin, int mode, IMXRT_GPIO_t &GPIO_n)
    {
        // set to output mode.
        if (mode)
        {
            GPIO_n.GDIR |= (1 << pin);
        }
        // set to input mode.
        else
        {
            GPIO_n.GDIR &= !(1 << pin);
        }
    }

    void configPort(IMXRT_GPIO_t &GPIO_n, uint32_t reg_val, uint32_t mask = 0xFFFFFFFF)
    {
        // keeping the values we mask, and overwriting the other
        GPIO_n.GDIR = (GPIO_n.GDIR & ~mask) | (reg_val & mask);

        // GPIO_n.G = reg_value & mask;
        // GPIO_n.DR_CLEAR = ~reg_value & mask; // flipping bits to set the bits that needs to be cleared
    }

    // info: inline: kinda like a macro, replace the fonction call with directly the
    // code. saves time to call, because the call takes longer than the actual code
    // inside
    inline void read_pin(int pin, uint16_t *data, uint32_t reg)
    {
        //? does this work, not same syntax as writing? reg is not a pointer but a value
        *data |= (((reg) & (0x1 << pin)) >> (pin - core_to_sample_bit[pin])); /// moves the pin value to right position in
                                                                              /// data, see adc.h, this is hardcoded to
                                                                              /// match the acoustics board
    }

    // return 0 or 1
    uint8_t read_pin(int pin, IMXRT_GPIO_t &GPIO_n)
    {
        return (((GPIO_n.PSR) & (0x1 << pin)) >> (pin - core_to_sample_bit[pin]));
    }

    void write_pin(int pin, uint8_t value, IMXRT_GPIO_t &GPIO_n)
    {
        if (value)
        {
            GPIO_n.DR_SET |= (1 << pin); /// the MCU has registers to set or clear registers
        }
        else
        {
            GPIO_n.DR_CLEAR |= (1 << pin); /// putting a 1 in the clear register will
                                           /// clear the bit in the GPIO_n register
        }
        // or
        /*
            GPIO_n.DR |= ((value & 0b1) << pin);
        */
    }

    void toggle_pin(int pin, IMXRT_GPIO_t &GPIO_n)
    {
        // same as above, there are registers (.DR_TOGGLE) that will toggle the bits where a 1 is written
        GPIO_n.DR_TOGGLE |= (1 << pin);
    }

    void write_port(uint32_t reg_value, IMXRT_GPIO_t &GPIO_n, uint32_t mask = 0xFFFFFFFF)
    {
        GPIO_n.DR_SET = reg_value & mask;
        GPIO_n.DR_CLEAR = ~reg_value & mask; // flipping bits to set the bits that needs to be cleared
    }

    uint32_t read_port(IMXRT_GPIO_t &GPIO_n)
    {
        return GPIO_n.PSR;
    }

    //? what is this function supposed to do?
    void test_write()
    {
        IMXRT_GPIO6.GDIR |= (0x1 << CORE_PIN21_BIT);
        Serial.printf("%d\n", ((IMXRT_GPIO7.DR) & (0x1 << CORE_PIN21_BIT)) >> CORE_PIN21_BIT);
        IMXRT_GPIO6.DR_SET |= (0x1 << CORE_PIN21_BIT);
        Serial.printf("%d\n", ((IMXRT_GPIO7.DR) & (0x1 << CORE_PIN21_BIT)) >> CORE_PIN21_BIT);
        IMXRT_GPIO6.DR_CLEAR |= (0x1 << CORE_PIN21_BIT);
    }

    //! needs to be tested by pulling pins to VCC or GND and see if value match
    // only 12 pins, not the 16 for the bus
    uint16_t read_pins()
    {
        uint16_t data = 0;
        uint32_t DR = IMXRT_GPIO7.DR; /// hopefully the adress is a uint32_t

        /// reading all the pins defined as DBX in adc.h
        read_pin(CORE_PIN38_BIT, &data, DR);
        read_pin(CORE_PIN39_BIT, &data, DR);
        read_pin(CORE_PIN40_BIT, &data, DR);
        read_pin(CORE_PIN41_BIT, &data, DR);

        read_pin(CORE_PIN14_BIT, &data, DR);
        read_pin(CORE_PIN15_BIT, &data, DR);
        read_pin(CORE_PIN16_BIT, &data, DR);
        read_pin(CORE_PIN18_BIT, &data, DR);
        read_pin(CORE_PIN17_BIT, &data, DR);
        read_pin(CORE_PIN19_BIT, &data, DR);
        read_pin(CORE_PIN20_BIT, &data, DR);
        read_pin(CORE_PIN21_BIT, &data, DR);

        return data;
    }

    // debug function
    void dump_GPIO_registers()
    {
        Serial.printf("GPIO6\nGDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO6.GDIR, IMXRT_GPIO6.DR);
        Serial.printf("GPIO7\nGDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO7.GDIR, IMXRT_GPIO7.DR);
    }

} // namespace gpio