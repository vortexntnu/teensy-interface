#include "pitInterrupt.h"


namespace periodicTimer {

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

void setup() {
    PIT_MCR &= ~PIT_MCR_MDIS;

    PIT_TCTRL0 = 0x0; 

    PIT_LDVAL3 = 0x0FFFFF;
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