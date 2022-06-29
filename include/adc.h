#pragma once
#include "imxrt.h"
#include "core_pins.h"
#include "ringBuffer.h"

namespace adc
{
    const int N_CHANNELS =      8; // no. of channels on the ADC
    const int N_HYDROPHONES =   5;
    const int SAMPLE_SIZE =     12; //bits in one sample

    // Control signals
    const int REFEN =   CORE_PIN1_BIT;
    const int _WR =     CORE_PIN1_BIT;
    const int STBY =    CORE_PIN2_BIT;
    const int RESET =   CORE_PIN3_BIT;
    const int _RD =     CORE_PIN4_BIT;
    const int _CS =     CORE_PIN11_BIT;
    const int PARSER =  CORE_PIN12_BIT;
    const int DB6 =     CORE_PIN14_BIT;
    const int DB7 =     CORE_PIN15_BIT;
    const int DB8 =     CORE_PIN16_BIT;
    const int DB9 =     CORE_PIN17_BIT;
    const int DB10 =    CORE_PIN18_BIT;
    const int DB11 =    CORE_PIN19_BIT;
    const int DB12 =    CORE_PIN20_BIT;
    const int DB13 =    CORE_PIN21_BIT;
    const int DB14 =    CORE_PIN22_BIT;
    const int DB15 =    CORE_PIN23_BIT;
    const int DB1 =     CORE_PIN24_BIT;
    const int DB0 =     CORE_PIN25_BIT;
    const int CONVST =  CORE_PIN33_BIT;
    const int ASLEEP =  CORE_PIN34_BIT;
    const int BUSYINT = CORE_PIN35_BIT;
    const int RANGE =   CORE_PIN36_BIT;
    const int XCLK =    CORE_PIN36_BIT;
    const int HWSW =    CORE_PIN37_BIT;
    const int DB2 =     CORE_PIN38_BIT;
    const int DB3 =     CORE_PIN39_BIT;
    const int DB4 =     CORE_PIN40_BIT;
    const int DB5 =     CORE_PIN41_BIT;

    // Interrupt Signals. 
    /* static uint16_t ChannelA0Data;
    static uint16_t ChannelA1Data;
    static uint16_t ChannelB0Data;
    static uint16_t ChannelB1Data;
    static uint16_t ChannelC0Data; */
    static RingBuffer ChannelA0;
    static RingBuffer ChannelA1;
    static RingBuffer ChannelB0;
    static RingBuffer ChannelB1;
    static RingBuffer ChannelB2;
    static RingBuffer ChannelC0;

    static uint16_t sampleData[N_CHANNELS];


    void setup(); // setup the ADC
    void config(); // configure ADC so it's ready to send data.

    //static void triggerConversion(void); // tell ADC to start converting.
    void startConversion(); // setup periodic timer interrupts. 
    void stopConversion();  // stop periodic timer interrupts

    void triggerConversion();
    void readData();
    void beginRead();
    void stopRead();
    void readData();
    void busyOVER();
    void readLoop();

    void transferData(); //transfer data to ringbuffers.
};
