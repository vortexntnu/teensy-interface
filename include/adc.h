#include "imxrt.h"
#include "core_pins.h"

#define CORE_PIN_37_BIT 19
#define CORE_PIN_36_BIT 18
#define CORE_PIN_35_BIT 28

namespace adc
{

    #define _CS    CORE_PIN_37_BIT //GPIO 7
    #define _RD    CORE_PIN_36_BIT //GPIO 7
    #define CONVST CORE_PIN_35_BIT //GPIO 7


    static uint16_t ChannelA0Data;
    static uint16_t ChannelA1Data;
    static uint16_t ChannelB0Data;
    static uint16_t ChannelB1Data;
    static uint16_t ChannelC0Data;


    void setup(); //setup the ADC

    static void triggerConversion(void); 
    
    void startConversion(); // setup periodic timer interrupts. 
    void stopConversion();  // stop periodic timer interrupts
    uint16_t getData(int hydrophone); 
};
