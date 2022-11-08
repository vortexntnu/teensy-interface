#include "adc.h"
#include "clock.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "gpio.h"

namespace adc {

static int channels_processed;

//set up sampling
void setup() {
    gpio::setup();
    clock::setup();     //// the clockfrequency needs to be defined somewhere
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
    periodicTimer::startPeriodic3(250);
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
 
    gpioInterrupt::setUpGpioISR(readLoop);
    //#endif //DISABLING THIS AND SIMULATING "BUSY" INSTEAD

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
    for (int i = 0; i<5; i++) {
    gpio::write_pin(_RD, 0, IMXRT_GPIO7);
    sampleData[channels_processed] = gpio::read_pins();
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    }
    for (int i = 5; i<8; i++) {
        gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        if (1) {gpio::write_pin(_RD, 0, IMXRT_GPIO7);}
        gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    }
    transferData();
}

void transferData() {
    ChannelA0.insert(sampleData[0]);
    ChannelA1.insert(sampleData[1]);
    ChannelB0.insert(sampleData[2]);
    ChannelB1.insert(sampleData[3]);
    ChannelC0.insert(sampleData[4]);
}

void config() {

    // see write access timing diagram on p.19 of ADC data sheet.
    // when _CS and _WR are low, we can write to the ADC config registers.
    // writes to the 16 most significant bits first.
    // check p.39 for info about config register

    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);

    gpio::configPin(_WR, 1, IMXRT_GPIO7);
    gpio::write_pin(_WR, 0, IMXRT_GPIO7); // start 1st write access, needs to be low for AT LEAST 15ns

    //We want BUSY/INT in interrupt mode, therefore we want the 27th bit in the ADC's config register to be high,
    //meaning DB(27-16) = DB11 = 1 (IN THE SECOND CONFIG CYCLE).

    
    delayNanoseconds(5); // t_SUDI/t_HDI

    gpio::write_pin(_WR, 1, IMXRT_GPIO7); // stop 1st write access, needs to be high AT LEAST 10ns

    delayNanoseconds(10); // t_WRH

    gpio::write_pin(_WR, 0, IMXRT_GPIO7); // start 2nd write access

    gpio::configPin(DB11, 1, IMXRT_GPIO6);
    gpio::write_pin(DB11, 1, IMXRT_GPIO6);
    delayNanoseconds(5); // t_SUDI/t_HDI

    gpio::write_pin(_WR, 1, IMXRT_GPIO7); // stop 2nd write access
    gpio::write_pin(_CS, 1, IMXRT_GPIO7);
}



} // adc
