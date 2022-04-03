#include "imxrt.h"
#include <Arduino.h>
#include "stdlib.h"

namespace gpioInterrupt {
    extern volatile uint32_t triggeredPin;

    extern uint32_t inputPinsMask;
    extern uint32_t ICR2activeHighMask;
    extern uint32_t ICR2clearMask;

    void setup();

    void ISR(void);

    void configPin(int pin, int mode);
};