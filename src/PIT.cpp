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

//// will be changed later, this keeps it generic
static void dummyISR(void) {}
void (*isr_periodic_func3)(void) = dummyISR; 
void (*isr_periodic_func2)(void) = dummyISR;

void ISR() {
    NVIC_DISABLE_IRQ(IRQ_PIT);

    if (PIT_TFLG3) {        //// what does that represent?
        PIT_TFLG3 = 0x1;
        (*isr_periodic_func3)(); 
    }
    
    if (PIT_TFLG2) {            //// what does that represent?
        (*isr_periodic_func2)();
    }

    NVIC_ENABLE_IRQ(IRQ_PIT);
}

/// @brief function called when IRQ_PIT interrupt happens
/// @param function function of top level file that will be called on IRQ_PIT interrupt, if (PIT_TFLG3)
void setUpPeriodicISR3(void_function_ptr function) {
    isr_periodic_func3 = function; 
}

/// @brief function called when IRQ_PIT interrupt happens
/// @param function function of top level file that will be called on IRQ_PIT interrupt, if (PIT_TFLG2)
void setUpPeriodicISR2(void_function_ptr function) {
    isr_periodic_func2 = function; 
}

void setup() {
    PIT_MCR &= ~PIT_MCR_MDIS;       //// check datasheet

    PIT_TCTRL0 = 0x0;

    
    attachInterruptVector(IRQ_PIT, ISR);
    NVIC_ENABLE_IRQ(IRQ_PIT);       //// is activating the interrupt management for IRQ_PIT
    #ifdef SERIAL_DEBUG
    dumpPeriodicRegisters();
    #endif
}

//// usefull info from datasheet: MCR[FRZ] can freeze the timers to debug. Datasheet page 2975
//// these timers run until stopped

void startPeriodic3(int clockcycles) {      //// the register can take a clockcycles uint32_t value
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