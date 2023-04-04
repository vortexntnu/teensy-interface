#include <ethernetModule.h>
#include <Arduino.h>

#include "DMAChannel.h"

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

#include "OctoWS2811.h"
// https://www.pjrc.com/teensy/td_libs_OctoWS2811.html

// #define TESTING // comment/uncomment depending if you use the testing functions

#ifdef TESTING
#include "testing.h" // all the test function should be here
#endif

// #define SERIAL_DEBUG

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
    clock::setup();
    uint32_t timer_freq = clock::get_clockcycles_micro(1000);
    Serial.print("Clockccles for 1s : ");
    Serial.println(timer_freq, HEX);

#ifdef TESTING
    // blinking_led();
    // testing_timers_general(); // in test file
    // testing_timers_basic();
    //  PIT::startPeriodic(3000, PIT::PIT_1);
    // test_par_interface();
    // test_ADC_timer();
    // test_basic_DMA();
    // test_interupt_arduino();
    test_complex_DMA();
#endif

    adc::init();

    Serial.println("Conversion will be started");

    adc::startConversion();
    delayMicroseconds(MAX_SAMPLING_PERIOD * 1500 * 2);
    adc::stopConversion();
    Serial.println("Conversion stopped");

    Serial.println("delta t between samples : ");
    uint32_t previous_time_sample = adc::sampleTime.get();
    while (!adc::sampleTime.isEmpty())
    {

        uint32_t next_time_sample = adc::sampleTime.get();
        Serial.print(next_time_sample - previous_time_sample);
        Serial.print(", ");
        previous_time_sample = next_time_sample;
    }
    Serial.println("");

    // while (!adc::ChannelA0.isEmpty())
    for (uint16_t i = 0; i < 1500; i++)
    {
        Serial.print((int16_t)adc::ChannelA0.get());
        Serial.print(", ");
    }
    Serial.println("");
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
