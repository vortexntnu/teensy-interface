#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include "pitInterrupt.h"
#include "gpioInterrupt.h"
#include "gptInterrupt.h"
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
 
#ifdef PIN_DEBUG
void setupgptIndicator() {
    gpio::configPin(CORE_PIN32_BIT, 1, IMXRT_GPIO7);
}
void gptIndicator() {
    gpio::toggle_pin(CORE_PIN32_BIT, IMXRT_GPIO7);
}
#endif



int main() {
    #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial) {}
    Serial.printf("Serial connected\r\n");
    #endif
    
    #ifdef ADC_DEBUG_CHRISTIAN
    adc::setup();
    #endif

    #ifdef PIN_DEBUG
    clock::setup();
    gpt::setup();
    gpt::setUpGptISR(*gptIndicator);
    gpio::setup();
    #endif
    
    while (1)
    {   
        adc::startConversion();
        //gpio::write_pin(CONVST, 1, IMXRT_GPIO7);
        #ifdef PIN_DEBUG
        setupgptIndicator();
        gpt::startTimer(3750); //Takes in amount of clock cycles it needs before execute
        Serial.printf("2sec delay\n");
        delay(2000);
        #endif
        delay(1000);
    }
}
