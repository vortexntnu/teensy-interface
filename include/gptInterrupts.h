#include "imxrt.h"



namespace gpt {

    extern uint32_t prescalerMask;
    extern uint32_t clearCLKSRCMask;
    extern int waiting;

    void setup();
    void startTimer(int clockcycles);
    void ISR();
};