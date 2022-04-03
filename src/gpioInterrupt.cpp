#include "gpioInterrupt.h"
#include "core_pins.h"

namespace gpioInterrupt {

    volatile uint32_t triggeredPin;
    //uint32_t inputPinsMask = 0xC40000; //Activate GPIO8 pin 18, 22, 23
    uint32_t inputPinsMask = ((0x1<<CORE_PIN28_BIT) | (0x1<<CORE_PIN30_BIT) | (0x1<<CORE_PIN31_BIT)); 
    uint32_t ICR2activeHighMask = 0xA020;
    uint32_t ICR2clearMask = ~(0xF030);


    void ISR(void) {

        NVIC_DISABLE_IRQ(IRQ_GPIO6789);

        if ((0x1<<CORE_PIN28_BIT) & GPIO8_ISR) {
            GPIO8_ISR &= 0x1<<CORE_PIN28_BIT;
            Serial.print("Pin 28 detected a signal!\n");
        }
        if ((0x1<<CORE_PIN31_BIT) & GPIO8_ISR) {
            GPIO8_ISR &= 0x1<<CORE_PIN31_BIT;
            Serial.print("Pin 31 detected a signal!\n");
        }
        if ((0x1<<CORE_PIN30_BIT) & GPIO8_ISR) {
            GPIO8_ISR &= 0x1<<CORE_PIN30_BIT;
            Serial.print("Pin 30 detected a signal!\n");
        }

        NVIC_ENABLE_IRQ(IRQ_GPIO6789);
    }

    void setup() {

        GPIO8_GDIR &= ~inputPinsMask; //Use inputPinsMask to mark input pins

        GPIO8_IMR = inputPinsMask; //Use inputPinsMask to active interrups on chosen pins

        GPIO8_ICR2 &= ICR2clearMask; //Use ICR2clearMask to clear out currently existing values in chosen pins
        GPIO8_ICR2 |= ICR2activeHighMask; // Use ICR2activeHighMask to mark pins as rising edge interrupt trigger

        GPIO8_DR_CLEAR = 0xFFFFFFFF; //Clear current content in data register

        attachInterruptVector(IRQ_GPIO6789, ISR);
        NVIC_ENABLE_IRQ(IRQ_GPIO6789);
    }

};