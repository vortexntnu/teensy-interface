#include "adc.h"
#include "clock.h"
#include "pitInterrupt.h"
#include "gpioInterrupt.h"
#include "gptInterrupt.h"
#include "gpio.h"

namespace adc {
//function currently turns convst signal on and of. 
static void triggerConversion() {
    // checks if CONVST pin is not set. 
    //if ( !(((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST) ) {
    IMXRT_GPIO7.DR_SET |= (0x1<<CONVST); //DR can also be used directly.
    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to high.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

//set up sampling
void setup() {
    Serial.printf("Input pin mask: 0x%X\n", gpioInterrupt::inputPinsMask);
    gpio::setup();
    clock::setup();
    gpt::setup(); 
    periodicTimer::setup();
    gpioInterrupt::setup(); 

    gpio::configPin(CONVST, 1, IMXRT_GPIO7);
    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::configPin(_RD, 1, IMXRT_GPIO7);

    gpio::write_pin(_CS, 0, IMXRT_GPIO7);
    gpio::write_pin(_RD, 1, IMXRT_GPIO7); 

    #ifdef SERIAL_DEBUG
    gpio::dump_GPIO_registers();
    #endif

    IMXRT_GPIO7.DR_CLEAR |= (0x1<<_CS); // set _CS pin to low, enabling parallell interface
    IMXRT_GPIO7.DR_SET |= (0x1<<_RD); // set _RD pin to high. (disabled)
    #ifdef SERIAL_DEBUG
    Serial.printf("_CS: %d\n", (((IMXRT_GPIO7.DR) & (0x1<<_CS))>>_CS));
    Serial.printf("_RD: %u\n", (((IMXRT_GPIO7.DR) & (0x1<<_RD))>>_RD));
    #endif

    clock::setup(); 
    periodicTimer::setup();
    triggerConversion(); 

}


void startConversion() {
    periodicTimer::setUpPeriodicISR(triggerConversion);
    periodicTimer::startPeriodic(); 
} 

void stopConversion() {
    IMXRT_GPIO7.DR_CLEAR |= (0x1<<CONVST);
    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to low.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

} // namespace adc
