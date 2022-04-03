#include "imxrt.h"
#include <Arduino.h>
#include "stdlib.h"

namespace gpioInterrupt {

    #define BUSY   CORE_PIN31_BIT //GPIO 9
    
    typedef void (*void_function_ptr)(void);

    extern volatile uint32_t triggeredPin;

    extern uint32_t inputPinsMask;
    extern uint32_t ICR2activeHighMask;
    extern uint32_t ICR2clearMask;

    void setup();

    void ISR(void);

    void setUpGpioISR(void_function_ptr function);

    void configPin(int pin, int mode);
};