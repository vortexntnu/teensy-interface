#include "adc.h"
#include "pitInterrupts.h"

using namespace adc; 

void adc::setup() {
    IMXRT_GPIO7.GDIR |= (0x1<<CONVST); //set convst pin to output mode
    IMXRT_GPIO7.GDIR |= (0x1<<_CS); //set _CS pin to output mode
    IMXRT_GPIO7.GDIR |= (0x1<<_RD); //set _RD pin to output mode

    IMXRT_GPIO7.DR_CLEAR |= (0x1<<_CS); //set _CS pin to low, enabling parallell interface
    #ifdef SERIAL_DEBUG
    Serial.printf("_CS: %d\n", (((IMXRT_GPIO7.DR) & (0x1<<_CS))>>_CS));
    Serial.printf("_RD: %d\n", (((IMXRT_GPIO7.DR) & (0x1<<_RD))>>_RD));
    #endif

    timer::setUpPeriodic();  
}

//function currently turns convst signal on and of. 
static void adc::triggerConversion() {
    if ( !(((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST) ) {
        IMXRT_GPIO7.DR_SET |= (0x1<<CONVST); //DR can also be used directly.
        #ifdef SERIAL_DEBUG
        Serial.println("Setting register");
        #endif
    }
    else {
        IMXRT_GPIO7.DR_CLEAR |= (0x1<<CONVST);
        #ifdef SERIAL_DEBUG
        Serial.println("Clearing register");
        #endif
    }
    #ifdef SERIAL_DEBUG
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

void adc::startConversion() {
    timer::setUpPeriodicISR(triggerConversion);
    timer::startPeriodic(); 
} 


