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

void testing_timers_basic(){
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

        for (uint8_t i = 0; i < 10; i++){
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

void print_dummy_1(){
    Serial.println("Dummy 1");
}
void print_dummy_2(){
    Serial.println("Dummy  2");
}