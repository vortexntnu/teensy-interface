#include "adc.h"
#include "GPT.h"
#include "PIT.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"

/// How it should work (is planned in the code) at the moment:
/*
First call adc::setup(), to set up all the timers and GPIOS needed for the communication
 - adc::config: writing register values to adc with custom parallel bus interface
 Then, for actual conversion:
 - adc::startConversion() : sets a timer every 250 clk cycles to call adc::triggerConversion()
    This function might be called only once, then conversion will be done every 250 clk cycles
 - weird timer to then call
   adc::readLoop() : loops to get all channels from adc, not using channel 6-7-8.
   Then calling adc::tranferData()
 - adc::transferData(): puts the data from adc into the ringbuffers

 A different way of doing it seems to be also implemented:

*/

namespace adc {

static int channels_processed;

// set up sampling
void setup()
{
    gpio::setup(); /// set the data bus pins as output and cleares them
    clock::setup(); /// the clockfrequency needs to be defined somewhere, does it need to be called also if adc is not init()
    gpt::setup();
    periodicTimer::setup();
    // gpioInterrupt::setup(); //NEEDS TO BE FIXED

    gpio::configPin(CONVST, 1, IMXRT_GPIO7);
    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::configPin(_RD, 1, IMXRT_GPIO7);
    gpio::configPin(_WR, 1, IMXRT_GPIO7);

    gpio::configPin(CONVST, 0, IMXRT_GPIO7); /// why do the same again, but CONVST and _CS as input now?
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    gpio::write_pin(_WR, 1, IMXRT_GPIO7);

#ifdef SERIAL_DEBUG
    Serial.printf("_CS: %d\n", (((IMXRT_GPIO7.DR) & (0x1 << _CS)) >> _CS));
    Serial.printf("_RD: %u\n", (((IMXRT_GPIO7.DR) & (0x1 << _RD)) >> _RD));
#endif
}

/// never called
void stopConversion()
{
    gpio::write_pin(CONVST, 0, IMXRT_GPIO7);
    periodicTimer::stopPeriodic3();
#ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to low.");
    Serial.printf("%d\n", ((IMXRT_GPIO7.DR) & (0x1 << CONVST)) >> CONVST);
#endif
}

void startConversion()
{
    periodicTimer::setUpPeriodicISR3(triggerConversion); /// this is the only time setUpPeriodicISR3 gets called
    periodicTimer::startPeriodic3(250); /// will it call triggerConversion every 250 clockcycles???
}

/// never used, is an alternative to the readloop
void beginRead()
{
    channels_processed = 0;
    periodicTimer::setUpPeriodicISR2(readData); /// why??? can't just readData?
    periodicTimer::startPeriodic2(1); /// the 1 clockcycle was only to check something, has nothing to do with the real code to read adc

    // gpt::setUpGptISR(readData);
    // gpio::write_pin(_RD, 0, IMXRT_GPIO7);
    // gpt::startTimer(132000000); // fix magic number.
}

/// never used, will be called if begin read is called.
void stopRead()
{
    periodicTimer::stopPeriodic2();

    // void (*void_func)(void);
    //  set gpio interrupt to do nothing.
    // gpioInterrupt::setUpGpioISR(void_func);
}

/// @brief function to start the conversion of data from ADC. once ADC is ready to output data, GpioISR will be triggered by the BUSY pin
void triggerConversion()
{

    /// will pull the CONVST line high, that indicates to the adc to start conversion
    gpio::write_pin(CONVST, 1, IMXRT_GPIO7);
    if (1) {
        gpio::write_pin(CONVST, 1, IMXRT_GPIO7);
    } // This creates a 50ns delay
    gpio::write_pin(CONVST, 0, IMXRT_GPIO7);

    // beginRead();

    gpioInterrupt::setUpGpioISR(readLoop); /// why doing it every time and not in an init() function
    /// problem?: this only sets the function that will be called, but doesn't start or stop the timer.
    ///

    // #endif //DISABLING THIS AND SIMULATING "BUSY" INSTEAD

#ifdef SERIAL_DEBUG
    Serial.println("Set CONVST pin to high.");
    Serial.printf("%d\n", ((IMXRT_GPIO7.DR) & (0x1 << CONVST)) >> CONVST);
#endif
}

/// @brief reads the data of 1 channel (called from interupt routine)
/// called from timer 2 when called beginRead before
void readData()
{
    gpio::write_pin(_RD, 0, IMXRT_GPIO7); // go to next channel;
    sampleData[channels_processed] = gpio::read_pins(); //// sampledata len = 8, maybe change if not needed (readloop would only need len = 5)
    gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    channels_processed++;
    // will automatically stop
    if (channels_processed == N_CHANNELS) {
        stopRead();
    } /* else {
        /// probably need to wait a bit before doing next read.          /// depends on frequency of timer that calls this function
        gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        gpt::startTimer(132000000);
    } */
}

/// is trying to to the same than readData but without interrupt, back top back. Written by two different persons
void readLoop()
{
    for (int i = 0; i < 5; i++) {
        gpio::write_pin(_RD, 0, IMXRT_GPIO7); /// tells the adc to put data on the line
        sampleData[channels_processed] = gpio::read_pins(); //// reads value from ADC on parallel data bus
        gpio::write_pin(_RD, 1, IMXRT_GPIO7); /// finish read operation, next loop the adc will put data from next channel on the line
    }
    for (int i = 5; i < 8; i++) { /// this would be channel 6 to 8, that we don't need
        gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        /// could probably be replaced with a real delay, it happened to be exactly the right delay to call this function
        if (1) {
            gpio::write_pin(_RD, 0, IMXRT_GPIO7);
        } /// not reading it, just faking it
        gpio::write_pin(_RD, 1, IMXRT_GPIO7);
    }
    transferData();
}

void transferData()
{
    ChannelA0.insert(sampleData[0]);
    ChannelA1.insert(sampleData[1]);
    ChannelB0.insert(sampleData[2]);
    ChannelB1.insert(sampleData[3]);
    ChannelC0.insert(sampleData[4]);
}

/// @brief configures the ADC --> is probably not correct, depends on what is needed
void config()
{

    // see write access timing diagram on p.19 of ADC data sheet.
    // when _CS and _WR are low, we can write to the ADC config registers.
    // writes to the 16 most significant bits first.
    // check p.39 for info about config register

    gpio::configPin(_CS, 1, IMXRT_GPIO7);
    gpio::write_pin(_CS, 0, IMXRT_GPIO7);

    gpio::configPin(_WR, 1, IMXRT_GPIO7);
    gpio::write_pin(_WR, 0, IMXRT_GPIO7); // start 1st write access, needs to be low for AT LEAST 15ns

    // We want BUSY/INT in interrupt mode, therefore we want the 27th bit in the ADC's config register to be high,
    // meaning DB(27-16) = DB11 = 1 (IN THE SECOND CONFIG CYCLE).

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
