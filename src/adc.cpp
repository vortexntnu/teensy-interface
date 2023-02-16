#include "adc.h"
#include "GPT.h"
#include "PIT.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"

/// How it should work (is planned in the code) at the moment:
/*
First call adc::setup(), to set up all the timers and GPIOS needed for the communication
 - adc::config: writing register values to adc with custom parallel bus interface
 Then, for actual conversion:
 - adc::startConversion() : sets a timer every 250 clk cycles to call adc::triggerConversion()
    This function might be called only once, then conversion will be done every 250 clk cycles
 - weird timer to then call
   adc::readLoop() : loops to get all channels from adc, not using channel 6-7-8.
   Then calling adc::tranferData()
 - adc::transferData(): puts the data from adc into the ringbuffers

 A different way of doing it seems to be also implemented:

*/

/*
    How it should work: from ADC datasheet
    init():
        -configuring the pins: maybe need to reconfigure from write to read --> test
            -CS(13), RD(12), WR(11), PAR/SER(8), HW/SW(37) CONVST as output, BUSY/INT(35) as input
            -floating pins in software: XCLK/RANGE, REFEN
        -settings pins to enable software and parallel mode
        Config ADC:
        -finding the right register value
        -write value to register
    config_adc():
        -PAR-pins as output (16 pins)
        -CS first and then WR low (for at least 15ns)
        -to avoid any timing problem, output register value before pulling low
         data (16 MSB) must be on the bus at least 5ns to be read correctly
        -WR high, CS stays low. High pulse for at least 10ns
        -writing second part (16 LSB) to the pins
        -WR to low for at least 15ns
        -First WR high, then CS, data must be valid for 5ns more!
        IF bit 30, the register values can be read (maybe on parallel interface) on SDO_A
    start_convertion():
        -pull CONVST high to start conversion (all channels at the same time)
        -CONVST can stay high or pulled low after, best is when BUSY/INT happens
        -if using BUSY: max time to BUSY high: 25ns. Then stays high for max 1,7 micro sec
            depending on ADC model (12-14-16 bit), don't care about time, using BUSY pin
        -wait for at least 86ns (model dependant, we have 12-bit resolution)
        -CS low, then RD low(no delay needed between)
        -CS can stay low
        Repeat:
        -RD has to be low for at least 20ns
        -value on PAR-bus is ready after max 15ns after RD low
        -RD must be high for at least 2ns for next data. data is valid for 5ns after rising edge
        -pull read down again if other data expected, go to Repeat.
        End of transmission:
        -RD to high again, same timing than during transmission
        -CS to high, can be done straight after RD to high
        -BUS goes to tree-state after max 10ns.
        -On ADS8528: 0 ns between CS rising edge to next CONVST rising edge
        ! min time between 2 CONVST rising edges: 240ns

*/

namespace adc
{
#define TESTING
#define DB_MASK 0xFFFF0000 // bit 16 to 31 of port 1 are the parallel interface
#define DB_REG_SHIFT 16    // values in GPIO reg must be shifted by 16
    uint8_t channels_processed;
#ifdef TESTING
    uint16_t testing_value_par;
#endif

    // sets pins accordingly to value (no control signals)
    void write_ADC_par(uint16_t value);
    // returns value on par-bus to ADC (no control signals)
    uint16_t read_ADC_par();

    void init()
    {
        // going back to normal GPIO mode (port 1-5)
        IOMUXC_GPR_GPR26 = 0x0; // 0xFFFFFFFF to use fast GPIO
        IOMUXC_GPR_GPR27 = 0x0; // each bit can be configured normal or fast
        IOMUXC_GPR_GPR28 = 0x0;
        IOMUXC_GPR_GPR29 = 0x0;

        // BUSYINT as input
        gpio::configPin(BUSYINT, 0, BUSYINT_GPIO_PORT_NORMAL);
        // ? configuring interrupt fot this pin, will be activated in start_conversion

        // _CS as output, high because interface is not enable on start-up
        gpio::configPin(_CS, 1, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        // HWSW as output, HIGH to select software mode
        gpio::configPin(HWSW, 1, HWSW_GPIO_PORT_NORMAL);
        gpio::write_pin(HWSW, 1, HWSW_GPIO_PORT_NORMAL);

        // PAR/SER as output, LOW to select parallel interface
        gpio::configPin(PARSER, 1, HWSW_GPIO_PORT_NORMAL);
        gpio::write_pin(PARSER, 0, HWSW_GPIO_PORT_NORMAL);

        // we are in software mode: XCLK as output, LOW (to ground) because NOT used
        gpio::configPin(XCLK, 1, XCLK_GPIO_PORT_NORMAL);
        gpio::write_pin(XCLK, 0, XCLK_GPIO_PORT_NORMAL);

        // _RD as output, HIGH, communication inactive
        gpio::configPin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);

        // _STBY as output, LOW because we use software mode (p8)
        gpio::configPin(STBY, 1, STBY_GPIO_PORT_NORMAL);
        gpio::write_pin(STBY, 0, STBY_GPIO_PORT_NORMAL);

        // CONVST as output, LOW
        gpio::configPin(CONVST, 1, CONVST_GPIO_PORT_NORMAL);
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);

        // RESET as output, active high
        gpio::configPin(RESET, 1, RESET_GPIO_PORT_NORMAL);
        gpio::write_pin(RESET, 0, RESET_GPIO_PORT_NORMAL);

        // configuring parallel interface, as input
#ifndef TESTING
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0x00000000, DB_MASK);
#endif
#ifdef TESTING
        // output, for testing pourpuse (LEDs)
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0xFFFF0000, DB_MASK);
#endif

        // defining value of register
        // uint32_t ADC_reg_config;
        // config(ADC_reg_config);
        setup();
    }

    // set up sampling
    void setup()
    {
        clock::setup(); /// the clockfrequency needs to be defined somewhere, does it need to be called also if adc is not init()
        // gpt::setup();
        periodicTimer::setup();
        // gpioInterrupt::setup(); //NEEDS TO BE FIXED
        periodicTimer::setUpPeriodicISR(readData, clock::get_clockcycles_micro(1000000), periodicTimer::PIT_1);
        periodicTimer::setUpPeriodicISR(next_RD, clock::get_clockcycles_micro(1000000), periodicTimer::PIT_2);
        // ! connect beginRead() to BUSY/INT interrupt
    }

    // is starting the major loop timer, PIT0 that will trigger the conversion until stopped
    void startConversion()
    {
        Serial.println("Starting conversion");
        // ? do it in one call?
        periodicTimer::setUpPeriodicISR(triggerConversion, clock::get_clockcycles_micro(1000000 * 15), periodicTimer::PIT_0);

        periodicTimer::startPeriodic(periodicTimer::PIT_0); // will call triggerConversion every 250 clockcycles
    }

    void stopConversion()
    {
        Serial.println("Stopping conversion and resetting pins");
        for (uint8_t i = 0; i < 3; i++)
        {
            periodicTimer::stopPeriodic(i);
        }
        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        // resetting ADC from actual conversion
        gpio::write_pin(RESET, 1, RESET_GPIO_PORT_NORMAL);
        delay(1);
        gpio::write_pin(RESET, 0, RESET_GPIO_PORT_NORMAL);
    }

    /// @brief function to start the conversion of data from ADC. once ADC is ready to output data, GpioISR will be triggered by the BUSY pin
    void triggerConversion()
    {
        // will pull the CONVST line high, that indicates to the adc to start conversion on all channels
        gpio::write_pin(CONVST, 1, CONVST_GPIO_PORT_NORMAL);

        // ! enable interrupt on BUSY/INT pin
        // ! function to be called :
        // gpioInterrupt::setUpGpioISR(beginRead); //? why doing it every time and not in an init() function
        // so far for testing:
        Serial.println("Delaying (conversion)");
        delay(2000);
        beginRead();
    }

    // resetting channels_processed and CONVST, continues with readData
    void beginRead()
    {
        Serial.println("beginRead()");
        // ! disable interrupts from BUSY/INT, will be enabled again by next triggerConvst()
        // PIT0 is continuing to run, will trigger next call in (250?) clockcycles

        channels_processed = 0;
        testing_value_par = 1;
        // no need to have CONVST high now
        gpio::write_pin(CONVST, 0, CONVST_GPIO_PORT_NORMAL);

        gpio::write_pin(_CS, 0, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
        // should take at least 20ns to trigger timer
        periodicTimer::startPeriodic(periodicTimer::PIT_1); // will call readData()
    }

    // reads the data on the parallel bus, when beginRead() was called
    void readData()
    {
        Serial.print("readData, channel nb : ");
        Serial.println(channels_processed);
        // disabling timer, so far it is not a periodic timer
        periodicTimer::stopPeriodic(periodicTimer::PIT_1);

#ifndef TESTING
        sampleData[channels_processed] = read_ADC_par(); /// sampledata len = 8, maybe change if not needed (readloop would only need len = 5)
#endif
#ifdef TESTING
        write_ADC_par(testing_value_par);
        testing_value_par *= 2;
#endif

        gpio::write_pin(_RD, 1, _RD_GPIO_PORT_NORMAL);
        channels_processed++;

        // will automatically stop
        if (channels_processed == N_HYDROPHONES)
        {
            stopRead();
        }
        else
        {
            periodicTimer::startPeriodic(periodicTimer::PIT_2);
        }
    }

    void next_RD()
    {
        Serial.println("next_RD");
        periodicTimer::stopPeriodic(periodicTimer::PIT_2);
        gpio::write_pin(_RD, 0, _RD_GPIO_PORT_NORMAL);
        // will call readData
        periodicTimer::startPeriodic(periodicTimer::PIT_1);
    }

    // turning of
    void stopRead()
    {
        Serial.println("stopRead");
        // timers are already off, no need to do anything
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);
    }

    /// is trying to to the same than readData but without interrupt, back to back. Written by two different persons
    void readLoop()
    {
        for (int i = 0; i < 5; i++)
        {
            gpio::write_pin(_RD, 0, IMXRT_GPIO7);               /// tells the adc to put data on the line
            sampleData[channels_processed] = gpio::read_pins(); /// reads value from ADC on parallel data bus
            gpio::write_pin(_RD, 1, IMXRT_GPIO7);               /// finish read operation, next loop the adc will put data from next channel on the line
        }
        for (int i = 5; i < 8; i++)
        { /// this would be channel 6 to 8, that we don't need
            gpio::write_pin(_RD, 0, IMXRT_GPIO7);
            /// could probably be replaced with a real delay, it happened to be exactly the right delay to call this function
            if (1)
            {
                gpio::write_pin(_RD, 0, IMXRT_GPIO7);
            } /// not reading it, just faking it
            gpio::write_pin(_RD, 1, IMXRT_GPIO7);
        }
        transferData();
    }

    void transferData()
    {
        ChannelA0.insert(sampleData[0]);
        ChannelA1.insert(sampleData[1]);
        ChannelB0.insert(sampleData[2]);
        ChannelB1.insert(sampleData[3]);
        ChannelC0.insert(sampleData[4]);
    }

    /// @brief configures the ADC
    // !--> needs to be tested !!!
    void config(uint32_t reg_val)
    {
        // pins as output
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0xFFFF0000, DB_MASK);
        //* see write access timing diagram on p.19 of ADC data sheet
        // check p.39 for info about config register
        // starting write access to ADC
        gpio::write_pin(_CS, 0, _CS_GPIO_PORT_NORMAL);
        gpio::write_pin(_WR, 0, _WR_GPIO_PORT_NORMAL);

        // writing MSBs first
        write_ADC_par(reg_val >> 16);

        delayNanoseconds(15); // t_WRL; t_SUDI/t_HDI

        gpio::write_pin(_WR, 1, _WR_GPIO_PORT_NORMAL);

        delayNanoseconds(10); // t_WRH

        // then writing LSBs, timing of t_HDI is respected
        write_ADC_par(reg_val & 0xFFFF);
        gpio::write_pin(_WR, 0, _WR_GPIO_PORT_NORMAL);

        delayNanoseconds(15); // t_WRL

        gpio::write_pin(_WR, 1, _WR_GPIO_PORT_NORMAL); // stop 2nd write access
        gpio::write_pin(_CS, 1, _CS_GPIO_PORT_NORMAL);

        delayNanoseconds(5); // t_HDI

        // pins back as inputs
        gpio::configPort(DB_GPIO_PORT_NORMAL, 0x00000000, DB_MASK);
    }

    void write_ADC_par(uint16_t value)
    {
        gpio::write_port(value << DB_REG_SHIFT, DB_GPIO_PORT_NORMAL, DB_MASK);
    }
    uint16_t read_ADC_par()
    {
        return gpio::read_port(DB_GPIO_PORT_NORMAL) >> DB_REG_SHIFT;
    }
}; // adc
