#include "gpioInterrupt.h"
#include "core_pins.h"


namespace gpioInterrupt {

volatile uint32_t triggeredPin;
// uint32_t inputPinsMask = 0xC40000; //Activate GPIO8 pin 18, 22, 23
uint32_t inputPinsMask = ((0x1 << CORE_PIN28_BIT) | (0x1 << CORE_PIN30_BIT) | (0x1 << BUSY));
// uint32_t ICR2activeHighMask = 0xA020;
uint32_t configMask = ( ((2 << (2 * (BUSY % 16)))) | ((2 << (2 * (CORE_PIN28_BIT % 16)))) | ((2 << (2 * (CORE_PIN30_BIT % 16)))) );
uint32_t ICR2clearMask = ~(0xF030);

//uint32_t intConfig = (2 << 12);//((2 << (2 * (BUSY % 16)))); // set interrupt pin to be rising edge sensitive.

void (*isr_convert_func)(void);

void ISR(void)
{

    NVIC_DISABLE_IRQ(IRQ_GPIO6789);

    if ((0x1<<CORE_PIN28_BIT) & GPIO8_ISR) {
        GPIO8_ISR &= 0x1<<CORE_PIN28_BIT;
        Serial.print("Pin 28 detected a signal!\n");
    }
    if ((0x1<<CORE_PIN30_BIT) & GPIO8_ISR) {
        GPIO8_ISR &= 0x1<<CORE_PIN30_BIT;
        Serial.print("Pin 30 detected a signal!\n");
    }

    if ((0x1 << BUSY) & GPIO8_ISR)
    {
        GPIO8_ISR &= 0x1 << BUSY;
    #ifdef SERIAL_DEBUG
        Serial.print("ADC finished converting\n");
    #endif
        (*isr_convert_func)();
    }

    NVIC_ENABLE_IRQ(IRQ_GPIO6789);
}

void setUpGpioISR(void_function_ptr function)
{
    isr_convert_func = function;
}

void setup()
{
#ifdef SERIAL_DEBUG
    Serial.printf("Setting up GPIO interrupts\n");
#endif
    GPIO8_GDIR &= ~inputPinsMask; // Use inputPinsMask to mark pins as input

    GPIO8_IMR = inputPinsMask; // Use inputPinsMask to active interrups on chosen pins

    //GPIO8_ICR2 &= ICR2clearMask; // Use ICR2clearMask to clear out currently existing values in chosen pins
    //GPIO8_ICR2 |= configMask; // Use ICR2activeHighMask to mark pins as rising edge interrupt trigger
    GPIO8_ICR2 = configMask;

    GPIO8_DR_CLEAR = 0xFFFFFFFF; // Clear current content in data register

#ifdef SERIAL_DEBUG
    dump_GPIO_interrupt_registers();
#endif

    attachInterruptVector(IRQ_GPIO6789, ISR);
    
#ifdef SERIAL_DEBUG 
    Serial.printf("Enabling interrupts..\n");
#endif 
    NVIC_ENABLE_IRQ(IRQ_GPIO6789);

#ifdef SERIAL_DEBUG
    Serial.printf("Setup complete\n");
#endif
}

void dump_GPIO_interrupt_registers() 
{
    Serial.printf("GDIR: 0x%X\nIMR: 0x%X\nICR2: 0x%X\n", GPIO8_GDIR, GPIO8_IMR, GPIO8_ICR2);
}

}; // gpioInterrupt