#include "adc.h"
#include "clock.h"
#include "pitInterrupt.h"
#include "gpioInterrupt.h"
#include "gptInterrupt.h"
#include "gpio.h"

namespace adc {

static int channels_processed;

//set up sampling
void setup() {
    gpio::setup();
    clock::setup();
    gpt::setup(); 
    periodicTimer::setup();
    //gpioInterrupt::setup(); //NEEDS TO BE FIXED

    gpio::configPin(CONVST, 1, IMXRT_GPIO7);
    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::configPin(_RD, 1, IMXRT_GPIO7);

    gpio::write_pin(CONVST, 0, IMXRT_GPIO7);    //DEFAULT LOW
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);       //DEFAULT LOW
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);       //DEFAULT HIGH
    

    #ifdef SERIAL_DEBUG
    Serial.printf("_CS: %d\n", (((IMXRT_GPIO7.DR) & (0x1<<_CS))>>_CS));
    Serial.printf("_RD: %u\n", (((IMXRT_GPIO7.DR) & (0x1<<_RD))>>_RD));
    #endif

}

void stopConversion() {
    gpio::write_pin(CONVST,0,IMXRT_GPIO7);
    periodicTimer::stopPeriodic3();
    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to low.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

void startConversion() {
    periodicTimer::setUpPeriodicISR3(triggerConversion);
    periodicTimer::startPeriodic3(700);
} 


void beginRead() {
    channels_processed = 0;
    periodicTimer::setUpPeriodicISR2(readData);
    periodicTimer::startPeriodic2(1);
    
    
    //gpt::setUpGptISR(readData);
    //gpio::write_pin(_RD, 0, IMXRT_GPIO7);
    //gpt::startTimer(132000000); // fix magic number.
}

void stopRead() {
    periodicTimer::stopPeriodic2();
    //void (*void_func)(void);
    // set gpio interrupt to do nothing.
    //gpioInterrupt::setUpGpioISR(void_func);
}

void triggerConversion() {

    gpio::write_pin(CONVST,1,IMXRT_GPIO7);
    if (1) {gpio::write_pin(CONVST,1,IMXRT_GPIO7);} //This creates a 50ns delay
    gpio::write_pin(CONVST,0,IMXRT_GPIO7);

    beginRead();

    #ifndef ADC_DEBUG_CHRISTIAN
    gpioInterrupt::setUpGpioISR(beginRead);
    #endif //DISABLING THIS AND SIMULATING "BUSY" INSTEAD

    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to high.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

void readData() {
    
    gpio::write_pin(_RD, 0, IMXRT_GPIO7); // go to next channel;
    sampleData[channels_processed] = gpio::read_pins();
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    channels_processed++;
    //will automatically stop 
    if (channels_processed==N_CHANNELS) {
        stopRead();
    } /* else {
        // probably need to wait a bit before doing next read.
        gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        gpt::startTimer(132000000);
    } */
}
} // adc
