#include <ethernetModule.h>
#include <Arduino.h>

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

#include "testing.h" // all the test function should be here

#define SERIAL_DEBUG

// This can be any wanted character array / string

/* static void send_on_interrupt(void) {
    uint16_t data = read_pins();
    #ifdef SERIAL_DEBUG
    Serial.printf("%u\n",data);
    #endif
    eth::write(data);
}  */

enum State
{
    IDLE,
    CONFIG_ADC,
    SAMPLE
};

void test_timers();
void print_dummy_1();
void print_dummy_2();

#ifdef PIN_DEBUG
void setupgptIndicator()
{
    gpio::configPin(CORE_PIN32_BIT, 1, IMXRT_GPIO7);
}
void gptIndicator()
{
    gpio::toggle_pin(CORE_PIN32_BIT, IMXRT_GPIO7);
}
#endif

#ifdef SERIAL_DEBUG
static void blink_on_interrupt(void)
{ /// function to call on interupts, works ony once
    gpio::write_pin(CORE_PIN13_BIT, 1, IMXRT_GPIO6);
    digitalWrite(LED_BUILTIN, HIGH);
}

void test_interrupts() /// function that inits pins to be called on interrupts I guess
{
    gpio::configPin(CORE_PIN6_BIT, 1, IMXRT_GPIO7);
    gpio::write_pin(CORE_PIN6_BIT, 1, IMXRT_GPIO7);

    gpio::configPin(CORE_PIN13_BIT, 1, IMXRT_GPIO6); // config LED.
}
#endif

int main()
{
// clock of teensy is 600MHz after normal boot
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial)
    {
    }
    Serial.printf("Serial connected\r\n");
#endif

    Serial.print("F_CPU actual : ");
    Serial.println(F_CPU_ACTUAL);
    // adc::setup();
    Serial.print("F_CPU actual : ");
    Serial.println(F_CPU_ACTUAL);

    blinking_led();
    // testing_timers_basic(); // in test file

    // State state = State::IDLE;

    /// Why no adc::setup() call?
    /// this state machine was never testet and also it was not exactly decided how to do it. This is where team 2022 stopped about.

    // while (1)
    // {
    //     switch (state)
    //     {
    //     case IDLE: {

    //         break;
    //     }
    //     case CONFIG_ADC: {
    //         //config function
    //         adc::config();
    //         state = State::IDLE;
    //         break;
    //     }
    //     case SAMPLE: {
    //         adc::startConversion();

    //         break;
    //     }
    //     default:
    //         break;
    //     }

    //     #ifdef PIN_DEBUG
    //     setupgptIndicator();
    //     gpt::startTimer(3750); //Takes in amount of clock cycles it needs before execute
    //     Serial.printf("2sec delay\n");
    //     delay(2000);
    //     #endif
    //     delay(1000);
    // }
}
