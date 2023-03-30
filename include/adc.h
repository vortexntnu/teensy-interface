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
        *see chapter about IO, chapter 10, 11 and especially 12 from datasheet
    */
    // GPIO port+bit is commented
    //! if pin is reallocated, change here the port. Never use directly the port
    const int STBY = CORE_PIN2_BIT; // GPIO 4.4
#define STBY_GPIO_PORT_NORMAL IMXRT_GPIO4
    const int RESET = CORE_PIN3_BIT; // GPIO 4.5
#define RESET_GPIO_PORT_NORMAL IMXRT_GPIO4
    const int _RD = CORE_PIN4_BIT; // GPIO 4.6
#define _RD_GPIO_PORT_NORMAL IMXRT_GPIO4
    const int _CS = CORE_PIN11_BIT; // GPIO 2.2
#define _CS_GPIO_PORT_NORMAL IMXRT_GPIO2
    const int PARSER = CORE_PIN12_BIT; // GPIO 2.1
#define PARSER_GPIO_PORT_NORMAL IMXRT_GPIO2
    const int REFEN = CORE_PIN5_BIT; // GPIO 4.8
    const int _WR = CORE_PIN5_BIT;   // same pin as REFEN, depends if PAR or SER
#define REFEN_GPIO_PORT_NORMAL IMXRT_GPIO4
#define _WR_GPIO_PORT_NORMAL IMXRT_GPIO4
    const int CONVST = CORE_PIN33_BIT; // GPIO 4.07
#define CONVST_GPIO_PORT_NORMAL IMXRT_GPIO4
    const int ASLEEP = CORE_PIN34_BIT; // GPIO 2.29
#define ASLEEP_GPIO_PORT_NORMAL IMXRT_GPIO2
    const int BUSYINT = CORE_PIN35_BIT; // GPIO 2.28
#define BUSYINT_ARDUINO_PIN 28          // pin number to use for built-in arduino libraries
#define BUSYINT_GPIO_PORT_NORMAL IMXRT_GPIO2
    const int RANGE = CORE_PIN36_BIT; // GPIO 2.18
    const int XCLK = CORE_PIN36_BIT;  // same pin as RANGE, depending if HW or SW
#define RANGE_GPIO_PORT_NORMAL IMXRT_GPIO2
#define XCLK_GPIO_PORT_NORMAL IMXRT_GPIO2
    const int HWSW = CORE_PIN37_BIT; // GPIO 2.19
#define HWSW_GPIO_PORT_NORMAL IMXRT_GPIO2

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
#define DB_GPIO_PORT_NORMAL IMXRT_GPIO1

    // Timing definitions

    // Configuration of the ADC control register
#define CONFIG_WRITE_EN 31
#define CONFIG_READ_EN 30
#define CONFIG_CLKSEL 29
#define CONFIG_CLKOUT 28
#define CONFIG_BUSYINT 27
#define CONFIG_BUSY_POL 26
#define CONFIG_STBY 25
#define CONFIG_RANGE_A 24
#define CONFIG_RANGE_B 23
#define CONFIG_PD_B 22
#define CONFIG_RANGE_C 21
#define CONFIG__PD_C 20
#define CONFIG_RANGE_D 19
#define CONFIG_PD_D 18
#define CONFIG_REFEN 15
#define CONFIG_REFBUF 14
#define CONFIG_VREF 13
#define CONFIG_REFDAC 0

// * TIMINGS -----------
#define T_RDL
#define T_RDH 1e-6
#define T_PDDO
#define T_WRL
#define T_WRH

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

    void init();
    void setup();  // setup the ADC
    void config(); // configure ADC so it's ready to send data.

    void startConversion(); // setup periodic timer interrupts.
    void stopConversion();  // stop periodic timer interrupts

    void triggerConversion(); // tell ADC to start converting.
    void readData();
    void next_RD(); // pulling _RD down, starting timer for next read
    void beginRead();
    void stopRead();
    /// void readData();
    void busyOVER();
    void readLoop();

    void transferData(); // transfer data to ringbuffers.

    void setting_up_timers_DMA();
};
