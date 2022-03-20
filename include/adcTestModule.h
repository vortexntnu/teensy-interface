#include "imxrt.h"

void gptSetup();

class ADC {
private:
    uint16_t one;
    uint16_t two;
    uint16_t three;
public:
    ADC(){one=1,two=2,three=3;};
    void setValues(int one, int two, int three);
};

extern ADC adc;


void gptSetup();
void gpt1ISR();