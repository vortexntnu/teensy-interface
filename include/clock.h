#include "imxrt.h"
#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

/// @brief to activate the clock
namespace clock
{
    extern uint32_t PITclockFreq; //// init in the .cpp but no values given !!!
    void dumpClockRegisters();
    void setup();

    uint32_t get_clockcycles_micro(uint32_t microseconds);
    uint32_t get_clockcycles_nano(uint32_t nanoseconds);
};
