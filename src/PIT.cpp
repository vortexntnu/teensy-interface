#include "PIT.h"


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
void (*isr_periodic_func3)(void) = dummyISR; 
void (*isr_periodic_func2)(void) = dummyISR;

void ISR() {
    NVIC_DISABLE_IRQ(IRQ_PIT);

    if (PIT_TFLG3) {
        PIT_TFLG3 = 0x1;
        (*isr_periodic_func3)(); 
    }
    
    if (PIT_TFLG2) {
        (*isr_periodic_func2)();
    }

    NVIC_ENABLE_IRQ(IRQ_PIT);
}

void setUpPeriodicISR3(void_function_ptr function) {
    isr_periodic_func3 = function; 
}

void setUpPeriodicISR2(void_function_ptr function) {
    isr_periodic_func2 = function; 
}

void setup() {
    PIT_MCR &= ~PIT_MCR_MDIS;

    PIT_TCTRL0 = 0x0;

    
    attachInterruptVector(IRQ_PIT, ISR);
    NVIC_ENABLE_IRQ(IRQ_PIT);
    #ifdef SERIAL_DEBUG
    dumpPeriodicRegisters();
    #endif
}

void startPeriodic3(int clockcycles) {
    #ifdef SERIAL_DEBUG
    Serial.println(">>> timer::startPeriodic()");
#endif
    PIT_TCTRL3 |= PIT_TCTRL_TIE;
    PIT_LDVAL3 = clockcycles; //counting period
    PIT_TCTRL3 |= PIT_TCTRL_TEN; // Enable Timer
#ifdef SERIAL_DEBUG
    Serial.println("<<< timer::startPeriodic()");
#endif
}

void startPeriodic2(int clockcycles) {
    #ifdef SERIAL_DEBUG
    Serial.println(">>> timer::startPeriodic()");
#endif
    PIT_TCTRL2 |= PIT_TCTRL_TIE;
    PIT_LDVAL2 = clockcycles; //counting period
    PIT_TCTRL2 |= PIT_TCTRL_TEN; // Enable Timer
#ifdef SERIAL_DEBUG
    Serial.println("<<< timer::startPeriodic()");
#endif
}

void stopPeriodic2() {
    PIT_TCTRL2 &= ~PIT_TCTRL_TIE;
    PIT_TCTRL2 &= ~PIT_TCTRL_TEN; // Enable Timer
}
    
void stopPeriodic3() {
    PIT_TCTRL3 &= ~PIT_TCTRL_TIE;
    PIT_TCTRL3 &= ~PIT_TCTRL_TEN; // Enable Timer
}

} //namespace timer