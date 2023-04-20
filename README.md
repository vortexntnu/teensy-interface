
# Branches
## temp/comments_team_2023
The branch temp/comments_team_2023 is here to add all the comments necessary to understand the work previously done.
It is missing some more comments that happened after branching from this branch.
This branch is depreciated. Only use this branch if you want to see the state before team 2022/2023
## dev/adc_interface_2023 
This branch is the developping branch to interface the adc. 
Two implementations of the ADC interface have been tested and work both at 100kHz(90.9kHz for timers) for 5 Channels.
How to get the sampled values is described in the section "SAMPLING"

---
# Sampling
To have further information on how to config the ADC, please read it's datasheet or the wiki page.
So far 2 ways of sampling are implemented. One with timers to leave the processor as free as possible to use interrupts.
And one that is blocking the processer during a complete read of the ADC. 
It might be that the overall time used by the processor is smaller using the blocking method, since the timings from the ADC are
so small that the timer interrupt event takes more time. 

To-DO : use DMA to be even faster 

To choose between the implementations, call the function start_conversion with the right enum-value:
- BLOCKING: each read blocks processor, one sample every X microseconds (min 10us)
- TIMER: timer in between channel readings, sample every X microseconds (min 11us)
- DMA: not implememnted yet
## Continous sampling
A code that would work is implemented in the main.cpp file.
Here the code/pseudo code:
```
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
Serial.println("Conversion will be started");
// to be safe should be a bit under 1500. If it sampled more than 1500 for some reason,
// the data gathered will be inconsistent.
uint16_t number_samples = 500;
uint32_t sample_period = 11; // >= MIN_SAMP_PERIOD_TIMER
if (number_samples > 1500)   // to not overfill ringbuffer
{
    number_samples = 1500;
}
// gathering samples
adc::startConversion(sample_period, adc::TIMER);
delayMicroseconds(sample_period * number_samples); // will sample "number_samples" samples
adc::stopConversion();
Serial.println("Conversion stopped");   
// end of gathering samples
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
/*
    3rd Part: The data is now in the ringbuffers.
    First the timestamps are processed and the each of the channels
*/
Serial.println("delta t between samples : ");
// first the time stamps are processed
// * This is not the only one way of using them
uint32_t previous_time_sample = adc::sampleTime.get();
while (!adc::sampleTime.isEmpty())
{
    uint32_t next_time_sample = adc::sampleTime.get();
    Serial.print(next_time_sample - previous_time_sample);
    Serial.print(", ");
    previous_time_sample = next_time_sample;
}
Serial.println("");
/* ----------------------------------------------------------------------------- */
/*
    Processing the samples now.
    Here they are only printed, if the need to be in a specific array for further processing
    they should be put in that array here
    Here only channel A0 is being printed out, but the same code applay to the 4 other ringbuffer
*/
for (uint16_t i = 0; i < number_samples; i++)
{
    Serial.print((int16_t)adc::ChannelA0.get());
    Serial.print(", ");
}
Serial.println("");
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */

/*
Do sample and process repeatedly:
    1: reset all the ringbuffer so that the new values are written from the start
    2: Sample again for the wanted number of samples
    3: process the new values
    4: go back to 1 and reset the ringbuffers
Following can be implemented to sample "continuously":
    - setup adc
    - define sample number and frequency
    loop:
        - sample
        - process the data in the ringbuffer
        - reset ringbuffer for new sampling
    end of loop
*/
```
## Single sample
After setting up the ADC by calling init(), setup() and config() with the desired configuration, a single sample from each channel can be obtained by:
- calling adc::trigger_conversion()
- getting the first unread value from the ringbuffer : ChannelXX.get()
- this not necessarely gives the last value, read until the buffer is empty to have the last value
- To make sure all buffers are synchronized get() should be called on each channel

---
# Code

## adc.h/cpp
The higher level function to interface the ADC are here:
- init(): configuring the pins used for the ADC interface
- setup(): calls setups from used submodules, and sets up timers that will be used, if timer-variant is used
- config(uint32_t reg_value): is writing to the config registers of the ADC, needs to be called. The value for the config register needs to be determined based on the use.
- start/stopConversion(): enables/disables timers that trigger conversion.
- trigger_conversion: can be called once to do one sample.

The others are not needed to start and stop conversion from the main file
## gpio.h/cpp
basic functions to configure and manage pins/ports. Are supposed to be faster than native arduino function, eventhough some special care is needed with ports.
The ports needs to be chosen accordingly if the GPIOs are in standart or fast mode. They are in fast mode by default, and can be changeg individually (to be checked which bits corresponds to each pin) to normal mode with these lines of code: 

```
// going back to normal GPIO mode (port 1-5)
IOMUXC_GPR_GPR26 = 0x0; // 0xFFFFFFFF to use fast GPIO
IOMUXC_GPR_GPR27 = 0x0;
IOMUXC_GPR_GPR28 = 0x0;
IOMUXC_GPR_GPR29 = 0x0;
```

## PIT.h/cpp
This files configures and runs the 4 Periodic Interval Timers, it is inspired by the already existing library from Teensy: ```#include <IntervalTimer.h>```

# PlatformIO environments
In the platformio.ini file are more than one build environment. The others are commented because every environment would build all the time, so only one is not commented to save compilation time. 
They should be use to select if printing is enabled, the clock is slower to be able to check with a slow oscilloscope but it has not really been used. 
So far the needed defines have been defined in the main file.