#include "gptInterrupts.h"
#include <Arduino.h>

// GENERAL PURPOSE TIMER SETUP, READ ON PAGE 2945-2969

namespace gpt {
    int waiting = 0;

    void setup() {
        
        //Prescaler setup (We want to turn to divide by 1)
        uint32_t prescalerMask = 0xFFFF;
        GPT1_PR &= ~prescalerMask;

        //Control register setup
        uint32_t clearCLKSRCMask = 0x1C0;
        GPT1_CR &= ~GPT_CR_FRR; //Sets to restart mode

        GPT1_CR &= ~clearCLKSRCMask;
        GPT1_CR |= GPT_CR_CLKSRC(0x1);

        GPT1_CR |= GPT_CR_ENMOD;
        //Enables GPT1 module and resetting of counter register after resetting EN bit (0th-bit)
        
        GPT1_IR |= GPT_IR_OF1IE;

        attachInterruptVector(IRQ_GPT1, ISR);
        NVIC_ENABLE_IRQ(IRQ_GPT1);
    }

    void startTimer(int clockcycles) {
        GPT1_OCR1 = clockcycles; //Sets compare register value
        GPT1_CR |= GPT_CR_EN; //Turns on timer
        waiting = 1;
    }

    void ISR() {
        GPT1_CR &= ~GPT_CR_EN; //Turns off timer
        NVIC_DISABLE_IRQ(IRQ_GPT1);
        if (GPT1_SR & GPT_SR_OF1) {
            GPT1_SR |= GPT_SR_OF1;
            
            Serial.printf("Reached capture register interrupt!\n");
            waiting = 0;
        }

        NVIC_ENABLE_IRQ(IRQ_GPT1);
    }
};