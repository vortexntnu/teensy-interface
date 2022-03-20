#include "imxrt.h"
#include <Arduino.h>

extern volatile uint32_t triggeredPin;

extern uint32_t inputPinsMask;
extern uint32_t ICR2activeHighMask;
extern uint32_t ICR2clearMask;

void setupGPIOInterrupt();

void ISR(void);