#include "gpioInterrupt.h"
#include "stdlib.h"

void ISR(void) {

    NVIC_DISABLE_IRQ(IRQ_GPIO3_16_31);

    if (1 << 18 & GPIO8_ISR) {
        (GPIO8_ISR &= ~(1 << 18));
        Serial.print("Pin 28 detected a signal!\n");
    }
    if (1 << 22 & GPIO8_ISR) {
        GPIO8_ISR &= ~(1 << 22);
        Serial.print("Pin 31 detected a signal!\n");
    }
    if (1 << 23 & GPIO8_ISR) {
        GPIO8_ISR &= ~(1 << 23);
        Serial.print("Pin 30 detected a signal!\n");
    }

    NVIC_ENABLE_IRQ(IRQ_GPIO3_16_31);
}

void setupGPIOInterrupt() {

    Serial.begin(9600);

    uint32_t inputPinsMask = 1 << 18 | 1 << 22 | 1 << 23;

    Serial.println(inputPinsMask, HEX);
    Serial.println(GPIO8_DR, HEX);

    GPIO8_DR &= ~inputPinsMask; //Writes in zeros in given pins to mark as input
    GPIO8_DR = 0xB00B5;

    Serial.println(GPIO8_DR, HEX);
    GPIO8_IMR |= inputPinsMask; //Writes in ones in given pins to active their interrupt
    Serial.println(0xB00B5, HEX);

    //ICR has 4 different configs, using 2 bits per pin, and
    //therefore has 2 32-bit registers for covering all pins

    uint32_t ICR2activeHighMask = 10 << (18-16) | 10 << (22-16) | 10 << (23-16);
    uint32_t ICR2clearMask = ~( 11 << (18-16) | 11 << (22-16) | 11 << (23-16));

    GPIO8_ICR2 &= ICR2clearMask;
    GPIO8_ICR2 |= ICR2activeHighMask;

    attachInterruptVector(IRQ_GPIO3_16_31, ISR);
}