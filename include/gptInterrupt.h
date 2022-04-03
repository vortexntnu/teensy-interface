#include "imxrt.h"
#include "Arduino.h"

namespace gpt {
    extern int waiting;

    void setup();
    void startTimer(int clockcycles);
    void ISR(void);
};
