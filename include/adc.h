#pragma once
#include "core_pins.h"
#include "imxrt.h"
#include "ringBuffer.h"

namespace adc
{
    const int N_CHANNELS = 8; // no. of channels on the ADC
    const int N_HYDROPHONES = 5;
    const int SAMPLE_SIZE = 12; // bits in one sample

    /*
        *Pin definition of Control signals
        The CORE_PINX_BIT define is the offset in the GPIO register. For example CORE_PIN39_BIT
        and CORE_PIN34_BIT are 29, but they are on a different GPIO port (1 and 2, respectively)
        To access the right control and output registers we need the corresponding port.
        GPIO1 and GPIO6 are the same port, GPIO6 operates the pins in fast mode //!(not compatible with DMA)
        Same with GPIO2 and GPIO7 and so on.
        *see chapter about IO, chapter 10, 11 and espacially 12 from datasheet
    */
    // GPIO port+bit is commented
    // const int REFEN = CORE_PIN1_BIT;   // GPIO 1.2, planned on pin 5 now
    // const int _WR = CORE_PIN1_BIT;     // same pin as REFEN, depends if PAR or SER
    const int STBY = CORE_PIN2_BIT;    // GPIO 4.4
    const int RESET = CORE_PIN3_BIT;   // GPIO 4.5
    const int _RD = CORE_PIN4_BIT;     // GPIO 4.6
    const int _CS = CORE_PIN11_BIT;    // GPIO 2.2
    const int PARSER = CORE_PIN12_BIT; // GPIO 2.1

    // /* this is for the accoustic board V2.0, but the pins will probably be rerouted*/
    // const int DB0 = CORE_PIN25_BIT;     // GPIO 1.13
    // const int DB1 = CORE_PIN24_BIT;     // GPIO 1.12
    // const int DB2 = CORE_PIN38_BIT;     // GPIO 1.28
    // const int DB3 = CORE_PIN39_BIT;     // GPIO 1.29
    // const int DB4 = CORE_PIN40_BIT;     // GPIO 1.20
    // const int DB5 = CORE_PIN41_BIT;     // GPIO 1.21
    // const int DB6 = CORE_PIN14_BIT;     // GPIO 1.18
    // const int DB7 = CORE_PIN15_BIT;     // GPIO 1.19
    // const int DB8 = CORE_PIN16_BIT;     // GPIO 1.23
    // const int DB9 = CORE_PIN17_BIT;     // GPIO 1.22
    // const int DB10 = CORE_PIN18_BIT;    // GPIO 1.17
    // const int DB11 = CORE_PIN19_BIT;    // GPIO 1.16
    // const int DB12 = CORE_PIN20_BIT;    // GPIO 1.26
    // const int DB13 = CORE_PIN21_BIT;    // GPIO 1.27
    // const int DB14 = CORE_PIN22_BIT;    // GPIO 1.24
    // const int DB15 = CORE_PIN23_BIT;    // GPIO 1.25

    // The pins are rearranged in a way to have the values from adc in the GPIO 1 (or 6 in fast mode)
    // the data will be at bits [16:31], so we only need a shift of 16.
    const int DB0 = CORE_PIN19_BIT;  // GPIO 1.16
    const int DB1 = CORE_PIN18_BIT;  // GPIO 1.17
    const int DB2 = CORE_PIN14_BIT;  // GPIO 1.18
    const int DB3 = CORE_PIN15_BIT;  // GPIO 1.19
    const int DB4 = CORE_PIN40_BIT;  // GPIO 1.20
    const int DB5 = CORE_PIN41_BIT;  // GPIO 1.21
    const int DB6 = CORE_PIN17_BIT;  // GPIO 1.22
    const int DB7 = CORE_PIN16_BIT;  // GPIO 1.23
    const int DB8 = CORE_PIN22_BIT;  // GPIO 1.24
    const int DB9 = CORE_PIN23_BIT;  // GPIO 1.25
    const int DB10 = CORE_PIN20_BIT; // GPIO 1.26
    const int DB11 = CORE_PIN21_BIT; // GPIO 1.27
    const int DB12 = CORE_PIN38_BIT; // GPIO 1.28
    const int DB13 = CORE_PIN39_BIT; // GPIO 1.29
    const int DB14 = CORE_PIN26_BIT; // GPIO 1.30
    const int DB15 = CORE_PIN27_BIT; // GPIO 1.31
// the port of the DB pins is GPIO port 1 (DMA needs the slower ports)
// const IMXRT_GPIO_t DB_GPIO_PORT_NORMAL IMXRT_GPIO1;
#define DB_GPIO_PORT_NORMAL IMXRT_GPIO1

    const int REFEN = CORE_PIN5_BIT;    // GPIO 4.8
    const int _WR = CORE_PIN5_BIT;      // GPIO 4.8
    const int CONVST = CORE_PIN33_BIT;  // GPIO 4.07
    const int ASLEEP = CORE_PIN34_BIT;  // GPIO 2.29
    const int BUSYINT = CORE_PIN35_BIT; // GPIO 2.28
    const int RANGE = CORE_PIN36_BIT;   // GPIO 2.18
    const int XCLK = CORE_PIN36_BIT;    // same pin as RANGE, depending if HW or SW
    const int HWSW = CORE_PIN37_BIT;    // GPIO 2.19

    // Interrupt Signals.
    /* static uint16_t ChannelA0Data;
    static uint16_t ChannelA1Data;
    static uint16_t ChannelB0Data;
    static uint16_t ChannelB1Data;
    static uint16_t ChannelC0Data; */

    // Ringbuffers to store data, one for each channel (not yet implemented)
    static RingBuffer ChannelA0;
    static RingBuffer ChannelA1;
    static RingBuffer ChannelB0;
    static RingBuffer ChannelB1;
    static RingBuffer ChannelB2;
    static RingBuffer ChannelC0; /// not planned on using

    static uint16_t sampleData[N_CHANNELS]; // array where the measurements will be stored before put into ringbuffer

    void setup();  // setup the ADC
    void config(); // configure ADC so it's ready to send data.

    void startConversion(); // setup periodic timer interrupts.
    void stopConversion();  // stop periodic timer interrupts

    void triggerConversion(); // tell ADC to start converting.
    void readData();
    void beginRead();
    void stopRead();
    /// void readData();
    void busyOVER();
    void readLoop();

    void transferData(); // transfer data to ringbuffers.
};
