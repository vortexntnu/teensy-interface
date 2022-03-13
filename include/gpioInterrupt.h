#include "imxrt.h"
#include <Arduino.h>

extern uint32_t inputPinsMask;

extern uint32_t ICR2activeHighMask;
extern uint32_t ICR2clearMask;

extern void setupGPIOInterrupt();

extern void ISR(void);