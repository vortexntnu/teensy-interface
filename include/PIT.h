#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

namespace periodicTimer {

typedef void (*void_function_ptr)(void);

////void ISR();     /// should not be called from outside!!
void setUpPeriodicISR3(void_function_ptr function);
void setUpPeriodicISR2(void_function_ptr function);
void setup();
void startPeriodic3(int clockcycles);
void startPeriodic2(int clockcycles);
void stopPeriodic2();
void stopPeriodic3();
} // periodicTimer