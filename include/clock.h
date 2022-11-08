#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

/// @brief to activate the clock
namespace clock {
    extern int clockFreq; //// init in the .cpp but no values given !!!
    void dumpClockRegisters();
    void setup();
};
