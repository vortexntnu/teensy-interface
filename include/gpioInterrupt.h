#include "imxrt.h"
#include <Arduino.h>
#include "stdlib.h"

namespace gpio {
    extern volatile uint32_t triggeredPin;

    extern uint32_t inputPinsMask;
    extern uint32_t ICR2activeHighMask;
    extern uint32_t ICR2clearMask;

    void setup();

    void ISR(void);
};