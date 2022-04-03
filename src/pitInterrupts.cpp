#include "pitInterrupts.h"


namespace timer {

#ifdef SERIAL_DEBUG
void dumpPeriodicRegisters() {
    Serial.printf("PIT.MCR: 0x%X\r\n", PIT_MCR);
    Serial.printf("PIT.LDVAL0: 0x%X\r\n", PIT_LDVAL0);
    Serial.printf("PIT.LDVAL1: 0x%X\r\n", PIT_LDVAL1);
    Serial.printf("PIT.LDVAL2: 0x%X\r\n", PIT_LDVAL2);
    Serial.printf("PIT.LDVAL3: 0x%X\r\n", PIT_LDVAL3);
    Serial.printf("PIT.TCTRL0: 0x%X\r\n", PIT_TCTRL0);
    Serial.printf("PIT.TCTRL1: 0x%X\r\n", PIT_TCTRL1);
    Serial.printf("PIT.TCTRL2: 0x%X\r\n", PIT_TCTRL2);
    Serial.printf("PIT.TCTRL3: 0x%X\r\n", PIT_TCTRL3);
}
#endif


static void dummyISR(void) {}
void (*isr_periodic_func)(void) = dummyISR; 

void ISR() {
    __disable_irq();

    PIT_TFLG3 = 0x1;
    (*isr_periodic_func)(); 

    __enable_irq();
}

void setUpPeriodicISR(void_function_ptr function) {
    isr_periodic_func = function; 
}

void setUpPeriodic() {

    // Setup Peripheral Clock
    // derive Pre-Peripheral Clock from PLL2
    CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0);
    // derive Peripheral from Pre-Peripheral Clock
    CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
    CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0); // divide /1 -> AHB Clock
    CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4
    CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
    CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
    // Clock: 132 Mhz -> Period: 7.5757576 ns
    // Fs: 500 kHz -> Period: 2 us -> 264 Clock Periods (-1)

    PIT_MCR &= ~PIT_MCR_MDIS;

    PIT_TCTRL0 = 0x0; 

    PIT_LDVAL3 = 0x0FFFFF; //max counting period
    PIT_TCTRL3 |= PIT_TCTRL_TIE;
    
    attachInterruptVector(IRQ_PIT, ISR);
    NVIC_ENABLE_IRQ(IRQ_PIT);
    #ifdef SERIAL_DEBUG
    dumpPeriodicRegisters();
    #endif
}

void startPeriodic() {
    #ifdef SERIAL_DEBUG
    Serial.println(">>> timer::startPeriodic()");
#endif
    PIT_TCTRL3 |= PIT_TCTRL_TEN; // Enable Timer
#ifdef SERIAL_DEBUG
    Serial.println("<<< timer::startPeriodic()");
#endif
}

} //namespace timer