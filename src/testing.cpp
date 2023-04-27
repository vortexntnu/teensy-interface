#include "testing.h"

#include "gpio.h"
#include "PIT.h"
#include "gpioInterrupt.h"
#include "GPT.h"
#include "clock.h"
#include "adc.h"

#include "TimerOne.h"

#include "DMAChannel.h"

void print_dummy_1();
void print_dummy_2();
void print_dummy_3();
void print_dummy_4();

void testing_timers_general()
{
    clock::setup();
    PIT::setup();
    Serial.println("Be prepared, start in 2s");
    delay(2000);
    Serial.println("Initialisation and starting PIT0, dummy3");
    PIT::startPeriodic(print_dummy_3, 0xFFFFFFF, PIT::PIT_0);
    delay(8500);
    PIT::stopPeriodic(PIT::PIT_0);
    Serial.println("PIT0 stopped");
    delay(5000);

    PIT::setUpPeriodicISR(print_dummy_1, PIT::PIT_0);
    PIT::setUpPeriodicISR(print_dummy_2, PIT::PIT_1);
    PIT::setUpPeriodicISR(print_dummy_3, PIT::PIT_2);
    PIT::setUpPeriodicISR(print_dummy_4, PIT::PIT_3);

    Serial.println("Setting up timers with ISR functions and starting them");
    delay(3000);

    PIT::startPeriodic(clock::get_clockcycles_micro(1000), PIT::PIT_0);
    PIT::startPeriodic(clock::get_clockcycles_micro(1500), PIT::PIT_1);
    PIT::startPeriodic(clock::get_clockcycles_micro(2000), PIT::PIT_2);
    PIT::startPeriodic(clock::get_clockcycles_micro(3000), PIT::PIT_3);

    while (42)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            delay(1000);
            Serial.println("waited 1s");
        }

        for (uint8_t i = 0; i <= 3; i++)
        {
            PIT::stopPeriodic(i);
        }
        Serial.println("timers disabled");
        delay(4000);
        Serial.println("waited 4 seconds, timers in 1s");
        delay(1000);

        for (uint8_t i = 0; i <= 3; i++)
        {
            // checking the sub function
            PIT::startPeriodic(i);
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

    // going back to normal speed ports for the pins used
    // default is GPIO ports 6-9 but we want the normal speed ports, at least for the parallel interface
    // because DMA doesn't work with the fast ports.
    gpio::set_normal_GPIO(1 << adc::_WR, _WR_GPIO_PORT_NORMAL);
    gpio::set_normal_GPIO(1 << adc::_RD, _RD_GPIO_PORT_NORMAL);
    gpio::set_normal_GPIO(1 << adc::_CS, _CS_GPIO_PORT_NORMAL);
    gpio::set_normal_GPIO(0xFFFF0000, DB_GPIO_PORT_NORMAL);

    // configure _WR and _CS pin as output (on port 4)
    _WR_GPIO_PORT_NORMAL.GDIR |= (1 << adc::_WR);
    _WR_GPIO_PORT_NORMAL.DR_SET |= (1 << adc::_WR);

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
            gpio::write_pin(adc::_WR, 0, _WR_GPIO_PORT_NORMAL);
            delay(30);
            // putting value on parallel bus
            gpio::write_port(i << 16, DB_GPIO_PORT_NORMAL, 0xFFFF0000);
            delay(300);
            // finished with this data, WR high again
            gpio::write_pin(adc::_WR, 1, _WR_GPIO_PORT_NORMAL);
            // delay to be able to debug it
            delay(100);
        }
        Serial.println("Finished sending values up to 255");
        gpio::write_pin(adc::_CS, 1, IMXRT_GPIO2);
        delay(2000);
    }
}

/*
testing the order of parallel databus access. The timer period needs to be slow enough to see
* connect LEDS like for testing parallel interface
* add a LED on pin for CONVST
*/
void test_ADC_timer()
{
    adc::init();

    adc::startConversion(MIN_SAMP_PERIOD_TIMER, adc::TIMER);
    Serial.println("Conversion started");

    delay(60000);
    adc::stopConversion();
    Serial.println("Conversion stopped");
}

/*
    define an address in memory, write a value
    write another value at a different location
    Set up a DMA channel that:
    - transfers the from second address to first
    - check if it worked
    Then do a repeated DMA, change the source value to see if it get's updated
    - level 3: DMA triggered from timer (--> see OctoWS library)
*/
// creating a DMA channel, first free channel is chosen
DMAChannel channel_basic = DMAChannel();

void ISR_timer_basic_DMA();
void test_basic_DMA()
{
    PIT::setup();

    // testing variables
    // works only with 8bit, 16 and 32 don't work as expected
    volatile uint8_t source[2] = {0b1000001, 17};
    volatile uint8_t destination[2] = {0, 0};

    // configuring the channel
    channel_basic.sourceBuffer(source, 2);
    // channel_basic.TCD->SADDR = &source;
    channel_basic.destinationBuffer(destination, 2);
    // channel_basic.TCD->DADDR = &destination;
    // ! this is done is the buffer setting function just above
    // channel_basic.transferSize(4);  // variable is 32bits, so 4 bytes
    // channel_basic.transferCount(4); // only once

    // setting up timer to trigger manually DMA, every 4 seconds
    PIT::setUpPeriodicISR(ISR_timer_basic_DMA, clock::get_clockcycles_micro(4000000), PIT::PIT_0);
    Serial.print("Value should be overwritten to every 4 seconds : ");
    Serial.println(source[0]);
    PIT::startPeriodic(PIT::PIT_0);
    delay(200);
    // loop is taking 100% of CPU
    while (42)
    {
        Serial.print("Value of variable (default 0) : ");
        Serial.print(destination[0]);
        Serial.print(" , ");
        Serial.println(destination[1]);
        // setting to 0 to see if DMA is triggered every time timer triggers
        destination[0] = 0;
        destination[1] = 0;
        delay(500);
    }
}

void ISR_timer_basic_DMA()
{
    Serial.println("int");
    channel_basic.triggerManual();
}

DMAChannel dma1 = DMAChannel();
DMAChannel dma2 = DMAChannel();
DMAChannel dma3 = DMAChannel();

void test_complex_DMA()
{
    // source variables
    uint8_t a, b, c;
    // destination variables
    uint8_t d, e, f;
    a = 1;
    b = 2;
    c = 3;
    d = 0;
    e = 0;
    f = 0;

    // works only with 8bit, 16 and 32 don't work as expected
    volatile uint8_t source[1] = {a};
    uint32_t led_source[1] = {0xFF00};
    volatile uint8_t destination[1] = {d};
    // blinking led

    // configuring the channel
    dma1.sourceBuffer(source, 1);
    dma1.destinationBuffer(destination, 1);
    // blinking led:
    dma2.sourceBuffer(led_source, 1);
    dma2.destination(IMXRT_GPIO1.DR_TOGGLE);
    gpio::set_normal_GPIO(0xFFFF0000, DB_GPIO_PORT_NORMAL);
    DB_GPIO_PORT_NORMAL.GDIR = 0xFFFF0000; // bits 16-31 as outputs, rest input

    // is setting up timers and linking output to XBAR, tjhat can be used for DMA
    adc::setting_up_timers_DMA();

    dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_XBAR1_0);
    dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_XBAR1_1);

    // starting timers, enabling channel
    dma1.enable();
    dma2.enable();
    TMR4_ENBL |= 7;

    // checking if d is changed:
    while (42)
    {
        Serial.print("value of d : ");
        Serial.println(destination[0]); // if DMA works, it should be 1 again
        destination[0] = 0;
        Serial.print("d : ");
        Serial.println(destination[0]); // should be 0
        // delay(500);
        delay(1);
    }
}

void test_ADC_DMA()
{
    adc::init();
    adc::setup();
    adc::setting_up_timers_DMA();
    adc::setting_up_DMA_channels();
    // starting timers
    uint32_t nb_high = 0;
    uint32_t nb_low = 0;
    unsigned long start_time = millis();
    TMR4_ENBL |= 7;
    while (millis() - start_time < 100)
    {
        // if (gpio::read_pin(adc::_RD, _RD_GPIO_PORT_NORMAL))
        if (DMA_test_variable)
        {
            nb_high++;
        }
        else
        {
            nb_low++;
        }
        delayNanoseconds(100);
    }

    Serial.print("nb of HIGH : ");
    Serial.println(nb_high);
    Serial.print("nb of LOW : ");
    Serial.println(nb_low);

    while (42)
    {
        delay(1000);
    }
}

void myInterrupt()
{
    Serial.println("My interrupt");
}

// testing the builtin interrupt handler
void test_interupt_arduino()
{
    // the Busy pin is number 28
    attachInterrupt(digitalPinToInterrupt(BUSYINT_ARDUINO_PIN), myInterrupt, FALLING);
}
