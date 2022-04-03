#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include "pitInterrupts.h"
#include "gpioInterrupt.h"
#include "gptInterrupts.h"
#include "adc.h"
#include "clock.h"


// This can be any wanted character array / string

/* static void send_on_interrupt(void) {
    uint16_t data = read_pins();
    #ifdef SERIAL_DEBUG
    Serial.printf("%u\n",data); 
    #endif
    eth::write(data); 
}  */

int main() {

    Serial.begin(9600);
    while (!Serial) {}
    Serial.printf("Serial connected\r\n");
    //gpio_setup(); 
    //gpio::setup();
    //eth::setup();
    clock::setup();
    gpt::setup();
    //timer::setup();
    //timer::setUpPeriodicISR(print_on_interrupt);
    //timer::startPeriodic();

    gpt::startTimer(132000000); //Takes in amount of clock cycles it needs before execute
    Serial.printf("Starting wait...\n");
    while (1)
    {
        while(gpt::waiting) {}
        //CODE TO BE EXECUTED AFTER DELAY
    }
 
}
