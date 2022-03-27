#include "imxrt.h"

extern uint32_t prescalerMask;
extern uint32_t clearCLKSRCMask;

void gpt_setup();
void gpt1ISR();
