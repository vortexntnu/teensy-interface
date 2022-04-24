#pragma once
#include "imxrt.h"
#include "core_pins.h"


namespace adc
{

    #define N_CHANNELS 8 // no. of channels on the ADC
    #define N_HYDROPHONES 5
    #define SAMPLE_SIZE 12  //bits in one sample

    // Control signals
    #define _CS    CORE_PIN37_BIT //GPIO 7
    #define _RD    CORE_PIN36_BIT //GPIO 7
    #define CONVST CORE_PIN35_BIT //GPIO 7

    // Interrupt Signals. 
    static uint16_t ChannelA0Data;
    static uint16_t ChannelA1Data;
    static uint16_t ChannelB0Data;
    static uint16_t ChannelB1Data;
    static uint16_t ChannelC0Data;

    static uint16_t sampleData[N_CHANNELS];


    void setup(); // setup the ADC

    //static void triggerConversion(void); // tell ADC to start converting.
    void startConversion(); // setup periodic timer interrupts. 
    void stopConversion();  // stop periodic timer interrupts

    void triggerConversion();
    void readData();
    void beginRead();
    void stopRead();
    void readData();

};
