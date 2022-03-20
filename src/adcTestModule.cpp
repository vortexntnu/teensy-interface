#include "adcTestModule.h"
#include <Arduino.h>

// GENERAL PURPOSE TIMER SETUP, READ ON PAGE 2945-2969

void gptSetup() {
    
    uint32_t prescalerMask = ~0xFFFF;

    GPT1_PR &= prescalerMask;

    uint32_t clearCLKSRCMask = ~0x1C0;
    uint32_t inputCLKSRCMask = 0x80;

    GPT1_CR &= clearCLKSRCMask;
    GPT1_CR |= inputCLKSRCMask;
    GPT1_CR |= 0x3; //Enables GPT1 module and resetting of counter register after resetting EN bit (0th-bit)
    
    attachInterruptVector(IRQ_GPT1, gpt1ISR);
    NVIC_ENABLE_IRQ(IRQ_GPT1);

}

void gpt1ISR() {

    NVIC_DISABLE_IRQ(IRQ_GPT1);

    if (GPT1_SR & 0x20) {
        GPT1_SR &= ~0x20;
        Serial.printf("Reached rollover interrupt!");
    }

    NVIC_ENABLE_IRQ(IRQ_GPT1);
}

// 