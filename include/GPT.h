#include "imxrt.h"
#include "Arduino.h"
#include "clock.h"

//// this was created to be used for delays in the nanoseconds range, but did't work reliably. Probably because the time of execution of the different lines where not 
//// compensated. ---->

//// USE the inbuilt function delaynanoseconds INSTEAD !!!!!!!!!!!!!!!!!!!!!!!!!! -----------------------------

/// @brief GENERAL PURPOSE TIMER
namespace gpt {
    extern int waiting;

    typedef void (*void_function_ptr)(void);

    void setup();
    void startTimer(int clockcycles);
    void ISR(void);         //// should not be called from outside!!
    void setUpGptISR(void_function_ptr function);
};
