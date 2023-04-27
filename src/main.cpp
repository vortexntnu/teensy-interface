// #include <ethernetModule.h>
#include <Arduino.h>

// #include "DMAChannel.h"

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

// #include "OctoWS2811.h"  // to follow a similar way for the ADC DMA interface
// https://www.pjrc.com/teensy/td_libs_OctoWS2811.html

// ! To use testing function --->
// #define TESTING // comment/uncomment depending if you use the testing functions

#ifdef TESTING
#include "testing.h" // all the test function should be here
#endif

#define SERIAL_DEBUG

enum State
{
    IDLE,
    CONFIG_ADC,
    SAMPLE
};

int main()
{
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial)
    {
    }
    Serial.printf("Serial connected\r\n");
#endif

    /* ----------------------------------------------------------------------------- */
    // clock of teensy is 600MHz after normal boot
    clock::setup();
    /* ----------------------------------------------------------------------------- */

#ifdef TESTING
    // * call testing function here, TESTING needs to be defined
    // test_complex_DMA();
    // test_ADC_DMA();
#endif

    /* ----------------------------------------------------------------------------- */
    /*
        First part: initializing ADC before being able to use it
    */

    adc::init();
    // defining value of register
    uint32_t ADC_reg_config;
    // WRITE_EN needs to be set to update REG, internal clock, BUSY mode active high,
    // powering off channel D because we don't need it, internal ref because nothing external connected, reference voltage to 2.5V
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC);
    // value of channel A doubles by dividing range by 2 (works as expected)
    // ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_RANGE_A);
    adc::config(ADC_reg_config);
    adc::setup(); // adc is now ready to use
    /* ----------------------------------------------------------------------------- */
    /* ----------------------------------------------------------------------------- */
    /*
        2nd Part: Sampling the data that is then put into the ringbuffers
    */
#ifdef SERIAL_DEBUG
    Serial.println("Conversion will be started");
#endif
    // to be safe should be a bit under 1500. If it sampled more than 1500 for some reason,
    // the data gathered will be inconsistent.
    uint16_t number_samples = 1000;
    uint32_t sample_period = 3; // >= MIN_SAMP_PERIOD_TIMER
    if (number_samples > 1500)  // to not overfill ringbuffer
    {
        number_samples = 1500;
    }
    // gathering samples
    adc::startConversion(sample_period, adc::BLOCKING);
    delayMicroseconds(sample_period * number_samples); // will sample "number_samples" samples
    adc::stopConversion();
    // end of gathering samples

    for (uint8_t i = 0; i < 1; i++)
    {
        adc::sample_fasfb(1000);
    }
    // return 0;

    /* ----------------------------------------------------------------------------- */
    /* ----------------------------------------------------------------------------- */
    /*
        3rd Part: The data is now in the ringbuffers.
        First the timestamps are processed and the each of the channels
    */
#ifdef SERIAL_DEBUG
    Serial.println("Conversion stopped");

    Serial.println("delta t between samples : ");
#endif
    // first the time stamps are processed
    // * This is not the only one way of using them
    uint32_t previous_time_sample = adc::sampleTime.get();
    while (!adc::sampleTime.isEmpty())
    {
        uint32_t next_time_sample = adc::sampleTime.get();
#ifdef SERIAL_DEBUG
        Serial.print(next_time_sample - previous_time_sample);
        Serial.print(", ");
#endif
        previous_time_sample = next_time_sample;
    }
#ifdef SERIAL_DEBUG
    Serial.println("");
#endif
    /* ----------------------------------------------------------------------------- */
    /*
        Processing the samples now.
        Here they are only printed, if the need to be in a specific array for further processing
        they should be put in that array here
        Here only channel A0 is being printed out, but the same code applay to the 4 other ringbuffer
    */
    for (uint16_t i = 0; i < number_samples; i++)
    {
#ifdef SERIAL_DEBUG
        Serial.print((int16_t)adc::ChannelA0.get());
        Serial.print(", ");
#endif
    }
#ifdef SERIAL_DEBUG
    Serial.println("");
#endif
    /* ----------------------------------------------------------------------------- */
    /* ----------------------------------------------------------------------------- */

    /*
        Do sample and process repeatedly:
            1: reset all the ringbuffer so that the new values are written from the start
            2: Sample again for the wanted number of samples
            3: process the new values
            4: go back to 1 and reset the ringbuffers
        Following can be implemented to sample "continuously":
            -init clock
            - setup adc
            - define sample number and frequency
            loop:
                - sample
                - process the data in the ringbuffer
                - reset ringbuffer for new sampling
            end of loop
    */
}
