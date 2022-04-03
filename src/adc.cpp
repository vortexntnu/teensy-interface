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
    gpioInterrupt::setup(); 

    gpio::configPin(CONVST, 1, IMXRT_GPIO7);
    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::configPin(_RD, 1, IMXRT_GPIO7);

    gpio::write_pin(_CS, 0, IMXRT_GPIO7);
    gpio::write_pin(_RD, 1, IMXRT_GPIO7); 

    #ifdef SERIAL_DEBUG
    gpio::dump_GPIO_registers();
    Serial.printf("_CS: %d\n", (((IMXRT_GPIO7.DR) & (0x1<<_CS))>>_CS));
    Serial.printf("_RD: %u\n", (((IMXRT_GPIO7.DR) & (0x1<<_RD))>>_RD));
    #endif

}


void stopConversion() {
    IMXRT_GPIO7.DR_CLEAR |= (0x1<<CONVST);
    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to low.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}


void startConversion() {
    periodicTimer::setUpPeriodicISR(triggerConversion);
    periodicTimer::startPeriodic(); 
} 


void beginRead() {
    channels_processed = 0;
    gpt::setUpGptISR(readData);
    gpio::write_pin(_RD, 0, IMXRT_GPIO7);
    gpt::startTimer(132000000); // fix magic number.
}

void stopRead() {
    void (*void_func)(void);
    // set gpio interrupt to do nothing.
    gpioInterrupt::setUpGpioISR(void_func);
}

void triggerConversion() {
    gpio::write_pin(CONVST,1,IMXRT_GPIO7);
    gpioInterrupt::setUpGpioISR(beginRead);

    #ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to high.");
    Serial.printf("%d\n",((IMXRT_GPIO7.DR) & (0x1<<CONVST))>>CONVST); 
    #endif
}

void readData() {
    sampleData[channels_processed] = gpio::read_pins();
    gpio::write_pin(_RD, 1, IMXRT_GPIO7); // go to next channel;
    channels_processed++;
    //will automatically stop 
    if (channels_processed==N_CHANNELS) {
        stopRead();
    } else {
        // probably need to wait a bit before doing next read.
        gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        gpt::startTimer(132000000);
    }
}

} // adc
