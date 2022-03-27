#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include "pitInterrupts.h"
#include "gpioInterrupt.h"
#include "gpt_interrupts.h"


char defaultMessage[] = "No package received";
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
    //setupGPIOInterrupt();
    gpt_setup();
    //eth::setup();

    //timer::setUpPeriodic();
    //timer::setUpPeriodicISR(send_on_interrupt);
    //timer::startPeriodic();   


    while (1)
    {
        Serial.printf("%d\r\n", GPT1_CNT);
    }
 
}
