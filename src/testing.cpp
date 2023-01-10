#include "testing.h"

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

#include "TimerOne.h"

void test_timers();
void print_dummy_1();
void print_dummy_2();

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

void print_dummy_1()
{
    Serial.println("Dummy 1");
}
void print_dummy_2()
{
    Serial.println("Dummy  2");
}

// connect a LED with resistor from pin 4 to ground, it will blink
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

    // pinMode(4, OUTPUT); // on GPIO port 9 //* IOMUX configured to fast GPIO

    // comparison of GPIO4 and 9
    Serial.printf("GPIO4 GDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO4.GDIR, IMXRT_GPIO4.DR);
    Serial.printf("GPIO9 GDIR: 0x%X\nDR: 0x%X\n", IMXRT_GPIO9.GDIR, IMXRT_GPIO9.DR);

    while (42)
    {
        gpio::write_pin(adc::_RD, 1, IMXRT_GPIO4);
        //  digitalWrite(4, HIGH);
        Serial.printf("GPIO4 GDIR: 0x%X, DR: 0x%X\n", IMXRT_GPIO4.GDIR, IMXRT_GPIO4.DR);
        Serial.printf("GPIO9 GDIR: 0x%X, DR: 0x%X\n", IMXRT_GPIO9.GDIR, IMXRT_GPIO9.DR);
        delay(1000);
        gpio::write_pin(adc::_RD, 0, IMXRT_GPIO4);
        //  digitalWrite(4, LOW);
        Serial.printf("GPIO4 GDIR: 0x%X, DR: 0x%X\n", IMXRT_GPIO4.GDIR, IMXRT_GPIO4.DR);
        Serial.printf("GPIO9 GDIR: 0x%X, DR: 0x%X\n", IMXRT_GPIO9.GDIR, IMXRT_GPIO9.DR);
        Serial.println("loop");
        delay(1000);
    }
}