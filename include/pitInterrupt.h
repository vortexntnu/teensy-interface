#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

namespace periodicTimer {

typedef void (*void_function_ptr)(void);

void ISR();
void setUpPeriodicISR(void_function_ptr function);
void setup();
void startPeriodic();

} //periodicTimer 