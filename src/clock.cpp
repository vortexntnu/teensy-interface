#include "clock.h"

namespace clock {
    int clockFreq;
    void setup() {

        // Setup Peripheral Clock
        //CCM_CACRR |= CCM_CACRR_ARM_PODF(0);
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0); //Deriving from PPL2 (600Mhz)
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0); // divide /1 -> AHB Clock
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(0); // divide /4 (Unable to change -Christian)
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
        // Clock: 150 Mhz -> Period: 6.667 ns

        // For a 25ns delay, we would need 15 cycles
    }
};