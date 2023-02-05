#include "testing.h"

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

#include "TimerOne.h"

void print_dummy_1();
void print_dummy_2();
void print_dummy_3();
void print_dummy_4();

void testing_timers_basic()
{
    clock::setup();
    periodicTimer::setup();

    periodicTimer::setUpPeriodicISR2(print_dummy_1);
    periodicTimer::setUpPeriodicISR3(print_dummy_2);
    delay(3000);

    while (42)
    {
        delay(1000);
        periodicTimer::startPeriodic2(0xFFFFFFF);
        periodicTimer::startPeriodic3(0xFFFFFF);

        for (uint8_t i = 0; i < 10; i++)
        {
            delay(1000);
            Serial.println("waited 1s");
        }

        periodicTimer::stopPeriodic2();
        periodicTimer::stopPeriodic3();
        Serial.println("timers disabled");
        delay(4000);
        Serial.println("waited 4 seconds, timers in 1s");
    }
}

void testing_timers_general()
{
    clock::setup();
    periodicTimer::setup();
    Serial.println("Be prepared, start in 2s");
    delay(2000);
    Serial.println("Initialisation and starting PIT0, dummy3");
    periodicTimer::startPeriodic(print_dummy_3, 0xFFFFFFF, periodicTimer::PIT_0);
    delay(8500);
    periodicTimer::stopPeriodic(periodicTimer::PIT_0);
    Serial.println("PIT0 stopped");
    delay(5000);

    periodicTimer::setUpPeriodicISR(print_dummy_1, periodicTimer::PIT_0);
    periodicTimer::setUpPeriodicISR(print_dummy_2, periodicTimer::PIT_1);
    periodicTimer::setUpPeriodicISR(print_dummy_3, periodicTimer::PIT_2);
    periodicTimer::setUpPeriodicISR(print_dummy_4, periodicTimer::PIT_3);

    Serial.println("Setting up timers with ISR functions and starting them");
    delay(3000);

    periodicTimer::startPeriodic(clock::get_clockcycles_micro(1000), periodicTimer::PIT_0);
    periodicTimer::startPeriodic(clock::get_clockcycles_micro(1500), periodicTimer::PIT_1);
    periodicTimer::startPeriodic(clock::get_clockcycles_micro(2000), periodicTimer::PIT_2);
    periodicTimer::startPeriodic(clock::get_clockcycles_micro(3000), periodicTimer::PIT_3);

    while (42)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            delay(1000);
            Serial.println("waited 1s");
        }

        for (uint8_t i = 0; i <= 3; i++)
        {
            periodicTimer::stopPeriodic(i);
        }
        Serial.println("timers disabled");
        delay(4000);
        Serial.println("waited 4 seconds, timers in 1s");
        delay(1000);

        for (uint8_t i = 0; i <= 3; i++)
        {
            // checking the sub function
            periodicTimer::startPeriodic(i);
        }
    }
}

void print_dummy_1()
{
    Serial.println("Dummy 1");
}
void print_dummy_2()
{
    Serial.println("Dummy  2");
}
void print_dummy_3()
{
    Serial.println("Dummy 3");
}
void print_dummy_4()
{
    Serial.println("Dummy  4");
}

// connect a LED with resistor from pin 4 to ground, it will blink
// GPIO Port 1: connect a led to pin 19 (it will correspond to DB0)
void blinking_led()
{
    // going back to normal GPIO mode (port 1-5)
    IOMUXC_GPR_GPR26 = 0x0; // 0xFFFFFFFF to use fast GPIO
    IOMUXC_GPR_GPR27 = 0x0; // each bit can be configured normal or fast
    IOMUXC_GPR_GPR28 = 0x0;
    IOMUXC_GPR_GPR29 = 0x0;

    // configure _RD pin as output (on port 4)
    IMXRT_GPIO4.GDIR |= (1 << adc::_RD);
    // IMXRT_GPIO9.GDIR |= (1 << adc::_RD); // if fast GPIO
    // using a declared const register for DB0
    DB_GPIO_PORT_NORMAL.GDIR |= 1 << adc::DB0;

    // pinMode(4, OUTPUT); // on GPIO port 9 //* IOMUX configured to fast GPIO

    // comparison of GPIO4 and 9
    Serial.printf("GPIO4 GDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO4.GDIR, IMXRT_GPIO4.DR);
    Serial.printf("GPIO9 GDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO9.GDIR, IMXRT_GPIO9.DR);

    while (42)
    {
        gpio::write_pin(adc::_RD, 1, IMXRT_GPIO4);
        gpio::write_pin(adc::DB0, 1, DB_GPIO_PORT_NORMAL);
        //  digitalWrite(4, HIGH);
        delay(1000);
        gpio::write_pin(adc::_RD, 0, IMXRT_GPIO4);
        gpio::write_pin(adc::DB0, 0, DB_GPIO_PORT_NORMAL);
        //  digitalWrite(4, LOW);
        Serial.println("loop");
        delay(1000);
    }
}

// testing the parallel interface
// connect LEDs to the data pins with a resistor
// connect the WR pin also to a LED
// info: this is with delays close to 1 sec, this is to see it with the eyes.
// * we will use timers to control the databus (or DMA), see further down for that
void test_par_interface()
{
    Serial.println("Starting parallel bus interface test with LEDs");
    // going back to normal GPIO mode (port 1-5)
    IOMUXC_GPR_GPR26 = 0x0; // 0xFFFFFFFF to use fast GPIO
    IOMUXC_GPR_GPR27 = 0x0; // each bit can be configured normal or fast
    IOMUXC_GPR_GPR28 = 0x0;
    IOMUXC_GPR_GPR29 = 0x0;

    // configure _WR and _CS pin as output (on port 4)
    IMXRT_GPIO4.GDIR |= (1 << adc::_WR);
    IMXRT_GPIO2.DR_SET |= (1 << adc::_WR);

    IMXRT_GPIO2.GDIR |= (1 << adc::_CS);
    gpio::write_pin(adc::_CS, 1, IMXRT_GPIO2);

    Serial.println("Turning _RD pin on");
    // checking the RD pin, on for 3s
    IMXRT_GPIO4.GDIR |= (1 << adc::_RD);
    IMXRT_GPIO4.DR_SET |= (1 << adc::_RD);
    delay(3000);
    IMXRT_GPIO4.DR_CLEAR |= (1 << adc::_RD);
    Serial.println("Turning _RD pin off");

    // configuring the 16 pins for the databus (only 8 are tested with LEDs)
    DB_GPIO_PORT_NORMAL.GDIR = 0xFFFF0000; // bits 16-31 as outputs, rest input
    delay(1000);

    while (42)
    {
        // we start writing to adc
        gpio::write_pin(adc::_CS, 0, IMXRT_GPIO2);

        Serial.println("Starting to send values on parallel bus");
        // sending values from 0 to 255
        for (uint16_t i = 0; i < 256; i++)
        {
            // WR to low, we write to ADC
            gpio::write_pin(adc::_WR, 0, IMXRT_GPIO4);
            delay(30);
            // putting value on parallel bus
            gpio::write_port(i << 16, DB_GPIO_PORT_NORMAL, 0xFFFF0000);
            delay(300);
            // finished with this data, WR high again
            gpio::write_pin(adc::_WR, 1, IMXRT_GPIO4);
            // delay to be able to debug it
            delay(100);
        }
        Serial.println("Finished sending values up to 255");
        gpio::write_pin(adc::_CS, 1, IMXRT_GPIO2);
        delay(2000);
    }
}