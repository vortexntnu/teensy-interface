#include "clock.h"

namespace clock {
    int clockFreq;
    void setup() {
        
        #ifndef SERIAL_DEBUG
        // Setup Peripheral Clock
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0); //Using PPL2 as source clock(~600MHz IN THEORY) (~536MHz IN PRACTICE)
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0); // divide /1 -> AHB Clock
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4 (Unable to change -Christian)
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
        // Clock: 150 MHz -> Period: 6.667 ns (IN THEORY)
        #endif

        #ifdef SERIAL_DEBUG
        // Setup Peripheral Clock
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0); //Using PPL2 as source clock(~600MHz IN THEORY) (~536MHz IN PRACTICE)
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(2); // divide /3 -> AHB Clock
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4 (Unable to change -Christian)
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 |= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
        // Clock: 50 MHz -> Period: 20 ns
        // PPL2 is totally divided by 12 instead of 4
        // Also, this is 3 times slower than "max" possible frequency
        #endif 
    }
};
