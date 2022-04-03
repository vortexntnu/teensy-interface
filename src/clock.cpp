#include "clock.h"

namespace clock {
    void setup() {
        // Setup Peripheral Clock
        CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0);
        // derive Peripheral from Pre-Peripheral Clock
        CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
        CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0); // divide /1 -> AHB Clock
        CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
        CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
        // Clock: 132 Mhz -> Period: 7.5757576 ns
        // Fs: 500 kHz -> Period: 2 us -> 264 Clock Periods (-1)
    }
};