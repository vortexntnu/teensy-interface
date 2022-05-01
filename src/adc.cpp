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
    gpio::configPin(_WR, 1, IMXRT_GPIO7);

    gpio::configPin(CONVST, 0, IMXRT_GPIO7);
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);
    gpio::write_pin(_RD, 1, IMXRT_GPIO7); 
    gpio::write_pin(_WR, 1, IMXRT_GPIO7);

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

    //beginRead();
    readLoop();
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

void readLoop() {
    for (int i = 0; i<8; i++) {
    gpio::write_pin(_RD, 0, IMXRT_GPIO7);
    sampleData[channels_processed] = gpio::read_pins();
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    }
}

void configureADC() {

    // see write access timing diagram on p.19 of ADC data sheet.
    // when _CS and _WR are low, we can write to the ADC config registers.
    // writes to the 16 most significant bits first.

    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);

    gpio::configPin(_WR, 1, IMXRT_GPIO7);
    gpio::write_pin(_WR, 0, IMXRT_GPIO7); // start 1st write access

    
    int BUSY_SEL = CORE_PIN21_BIT; // set this pin to high to enable interrupt from BUSY pin on ADC.
    //int BUSY_SEL = CORE_PIN16_BIT; // alternative
    int BUSY_POL_BIT = 26; 

    gpio::configPin(BUSY_SEL, 1, IMXRT_GPIO6);
    gpio::write_pin(BUSY_SEL, 1, IMXRT_GPIO6);

    gpio::write_pin(_WR, 1, IMXRT_GPIO7); // stop 1st write access
    delayNanoseconds(10); // t_WRH
    gpio::write_pin(_WR, 0, IMXRT_GPIO7); // start 2nd write access
    if (1);
    gpio::write_pin(_WR, 1, IMXRT_GPIO7); // stop 2nd write access

}



} // adc
