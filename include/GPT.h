#include "imxrt.h"
#include "Arduino.h"
#include "clock.h"

namespace gpt {
    extern int waiting;

    typedef void (*void_function_ptr)(void);

    void setup();
    void startTimer(int clockcycles);
    void ISR(void);
    void setUpGptISR(void_function_ptr function);
};
