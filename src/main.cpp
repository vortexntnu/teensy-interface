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

String channel_names[5] = {"A0", "A1", "B0", "B1", "C0"};

void print_to_csv(uint16_t nb_samples, uint8_t nb_channels);
void print_all_buffers_to_csv(uint16_t nb_samples, uint8_t nb_channels);

int main()
{
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial)
    {
    }
    // Serial.printf("Serial connected\r\n");
#endif

    /* ----------------------------------------------------------------------------- */
    // clock of teensy is 600MHz after normal boot
    // Serial.print("F_CPU_ACTUAL: ");
    // Serial.println(F_CPU_ACTUAL);
    // clock::setup();
    // Serial.print("F_CPU_ACTUAL, after clock::setup(): ");
    // Serial.println(F_CPU_ACTUAL);
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
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF); // | (1 << CONFIG_PD_C);
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
    // Serial.println("Conversion will be started");
#endif
    // to be safe should be a bit under 1500. If it sampled more than 1500 for some reason,
    // the data gathered will be inconsistent.
    uint16_t number_samples = 3 * SAMPLE_LENGTH_ADC;
    float sample_period = 2.3; // >= MIN_SAMP_PERIOD_TIMER
    // Serial.println(clock::get_clockcycles_micro(sample_period));
    if (number_samples > 3 * SAMPLE_LENGTH_ADC) // to not overfill ringbuffer
    {
        number_samples = 3 * SAMPLE_LENGTH_ADC;
    }
    // gathering samples
    adc::startConversion(sample_period, adc::BLOCKING);
    // delayMicroseconds(sample_period * number_samples); // will sample "number_samples" samples
    uint8_t buffer_to_check = adc::active_buffer;
    // Serial.println(buffer_to_check);
    uint8_t found = 0;
    while (!found)
    {
        while (!adc::buffer_filled[buffer_to_check])
        {
            ;
        }
        // doing the necessary checks
        for (uint16_t index = 0; index < SAMPLE_LENGTH_ADC; index++)
        {
            // here checking if channel A0 voltage is higher than 15000 (=4.68V)
            if (adc::channel_buff_ptr[0][buffer_to_check][index] > (int16_t)15000)
            {
                // found to one once the event happend (will be peak detection)
                found = 1;
                break;
            }
        }

        /*
        THIS IS WHAT NEED TO BE CHANGED TO TEST CONTINOUSLY WITH PEAK DETECTION
        !! might be changed depending on which channel the hydrophone is connected. Check also plots!!!!
        q15_t *samplesFiltered = filter_butterwort_9th_order_50kHz(adc::channel_buff_ptr[0][buffer_to_check]);

        // Preform FFT calculations on filtered samples
        q15_t *FFTResultsRaw = FFT_raw(samplesFiltered);
        q15_t *FFTResults = FFT_mag(FFTResultsRaw);
        q31_t **peaks = peak_detection(FFTResultsRaw, FFTResults);
        int lengthOfPeakArray = peaks[0][0];

         ------------------------------------------------
        */

        // ------ end of checks
        // to check that the next buffer is filled

        buffer_to_check = (buffer_to_check + 1) % BUFFER_PER_CHANNEL;
        if (buffer_to_check != adc::active_buffer)
        {
            found = 2;
            // comment this if code should continue, but it will be unexpected behaviour
            // break;
        }
    }

    if (found == 2)
    {
        Serial.println("The check on the buffer took longer than filling the next one");
        return 1;
    }
    // lets the next buffer fill to have 3 buffers to make calculations for pinger detection
    while (!adc::buffer_filled[buffer_to_check])
    {
        ;
    }

    adc::stopConversion();
    print_all_buffers_to_csv(number_samples, 5);
    return 0;
    // end of gathering samples

    // ! for printing during sampling
    // Serial.println(",Time,A0,A1,B0,B1,C1");

    number_samples = SAMPLE_LENGTH_ADC * 1;
    for (uint8_t i = 0; i < 1; i++)
    {
        adc::sample_fasfb(number_samples);
    }
    // return 0;

    if (42)
    {
        print_all_buffers_to_csv(number_samples, 5);
        return 0;
    }

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
        // Serial.print(next_time_sample - previous_time_sample);
        Serial.print(next_time_sample);
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

    for (uint16_t i = 0; i < number_samples; i++)
    {
#ifdef SERIAL_DEBUG
        Serial.print((int16_t)adc::ChannelA1.get());
        Serial.print(", ");
#endif
    }
#ifdef SERIAL_DEBUG
    Serial.println("");
#endif

    for (uint16_t i = 0; i < number_samples; i++)
    {
#ifdef SERIAL_DEBUG
        Serial.print((int16_t)adc::ChannelB0.get());
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

void print_to_csv(uint16_t nb_samples, uint8_t nb_channels)
{
    // creating column names for later in pandas
    Serial.print(",Time");
    for (uint8_t i = 0; i < nb_channels; i++)
    {
        Serial.print(",");
        Serial.print(channel_names[i]);
    }
    Serial.println("");

    for (uint16_t i = 0; i < nb_samples; i++)
    {
        Serial.print(i);
        Serial.print(",");
        Serial.print((uint32_t)adc::sampleTime.get());

        if (nb_channels >= 1)
        {
            Serial.print(",");
            Serial.print((int16_t)adc::ChannelA0.get());
        }
        if (nb_channels >= 2)
        {
            Serial.print(",");
            Serial.print((int16_t)adc::ChannelA1.get());
        }
        if (nb_channels >= 3)
        {
            Serial.print(",");
            Serial.print((int16_t)adc::ChannelB0.get());
        }
        if (nb_channels >= 4)
        {
            Serial.print(",");
            Serial.print((int16_t)adc::ChannelB1.get());
        }
        if (nb_channels >= 5)
        {
            Serial.print(",");
            Serial.print((int16_t)adc::ChannelC0.get());
        }

        Serial.println("");
        delay(10);
    }
}

void print_all_buffers_to_csv(uint16_t nb_samples, uint8_t nb_channels)
{
    if (nb_samples > BUFFER_PER_CHANNEL * SAMPLE_LENGTH_ADC)
    {
        nb_samples = BUFFER_PER_CHANNEL * SAMPLE_LENGTH_ADC;
    }
    // creating column names for later in pandas
    Serial.print(",Time");
    for (uint8_t i = 0; i < nb_channels; i++)
    {
        Serial.print(",");
        Serial.print(channel_names[i]);
    }
    Serial.println("");

    // active buffer is one further than the last filled one, which is the oldest one now
    uint8_t to_print_buffer = adc::active_buffer;
    uint8_t index_buffer = 0;

    // printing sample values
    for (uint8_t buff = 0; buff < BUFFER_PER_CHANNEL; buff++)
    {
        for (uint16_t sample_nb = 0; sample_nb < SAMPLE_LENGTH_ADC; sample_nb++)
        {
            if (index_buffer * SAMPLE_LENGTH_ADC + sample_nb >= nb_samples)
            {
                return;
            }
            // sample number
            Serial.print(index_buffer * SAMPLE_LENGTH_ADC + sample_nb);
            Serial.print(",");
            Serial.print((uint32_t)adc::timestamps[to_print_buffer][sample_nb]);

            for (uint8_t channel = 0; channel < nb_channels; channel++)
            {
                Serial.print(",");
                Serial.print((int16_t)adc::channel_buff_ptr[channel][to_print_buffer][sample_nb]);
            }
            Serial.println("");
            delay(10);
        }
        // next buffer
        adc::buffer_filled[to_print_buffer] = 0;
        to_print_buffer = (to_print_buffer + 1) % BUFFER_PER_CHANNEL;
        index_buffer++; // starts at 0 so will never go over 3
    }
}
