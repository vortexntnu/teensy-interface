#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

namespace clock {
    extern int clockFreq;
    void dumpClockRegisters();
    void setup();
};
