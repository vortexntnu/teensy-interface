#include "imxrt.h"
#include "Arduino.h"
#include "clock.h"

/// @brief GENERAL PURPOSE TIMER
namespace gpt {
    extern int waiting;

    typedef void (*void_function_ptr)(void);

    void setup();
    void startTimer(int clockcycles);
    void ISR(void);         //// should not be called from outside!!
    void setUpGptISR(void_function_ptr function);
};
