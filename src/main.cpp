#include <ethernetModule.h>
#include <Arduino.h>

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"


// This can be any wanted character array / string

/* static void send_on_interrupt(void) {
    uint16_t data = read_pins();
    #ifdef SERIAL_DEBUG
    Serial.printf("%u\n",data); 
    #endif
    eth::write(data); 
}  */

enum State {
    IDLE, CONFIG_ADC, SAMPLE
};
 
#ifdef PIN_DEBUG
void setupgptIndicator() {
    gpio::configPin(CORE_PIN32_BIT, 1, IMXRT_GPIO7);
}
void gptIndicator() {
    gpio::toggle_pin(CORE_PIN32_BIT, IMXRT_GPIO7);
}
#endif

#ifdef SERIAL_DEBUG
static void blink_on_interrupt(void) {
    gpio::write_pin(CORE_PIN13_BIT, 1, IMXRT_GPIO6);
    digitalWrite(LED_BUILTIN, HIGH);
}

void test_interrupts()
{
    gpio::configPin(CORE_PIN6_BIT, 1, IMXRT_GPIO7);
    gpio::write_pin(CORE_PIN6_BIT, 1, IMXRT_GPIO7);

    gpio::configPin(CORE_PIN13_BIT, 1, IMXRT_GPIO6); // config LED. 
}
#endif

int main() {
    #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial) {}
    Serial.printf("Serial connected\r\n");
    #endif

    State state = State::IDLE;

    //// Why no adc::setup() call?
    
    while (1)
    {   
        switch (state)
        {
        case IDLE: {

            break;
        }
        case CONFIG_ADC: {
            //config function
            adc::config();
            state = State::IDLE;
            break;
        }
        case SAMPLE: {
            adc::startConversion();

            break;
        }
        default:
            break;
        }
        
        #ifdef PIN_DEBUG
        setupgptIndicator();
        gpt::startTimer(3750); //Takes in amount of clock cycles it needs before execute
        Serial.printf("2sec delay\n");
        delay(2000);
        #endif
        delay(1000);
    }
}
