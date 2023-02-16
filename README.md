
# Branches
## temp/comments_team_2023
The branch temp/comments_team_2023 is here to add all the comments necessary to understand the work previously done.
It is missing some more comments that happened after branching from this branch.
This branch is depreciated. Only use this branch if you want to see the state before team 2022/2023
## dev/adc_interface_2023 
This branch is the developping branch to interface the adc. So far nothing has been tested, due to lacking hardware. 
A timer-based approach is implemented, only the timings need to be adjusted 

---

# Code

# adc.h/cpp
The higher level function to interface the ADC are here:
- init(): configuring the pins used for the ADC interface, calls setup.
- setup(): calls setups from used submodules, and sets up timers that will be used.
- config(): is writing to the config registers of the ADC, needs to be called. Change configs in this function if needed
- start/stopConversion(): enables/disables timers that trigger conversion.

The others are not needed to start and stop conversion from the main file
# gpio.h/cpp
basic functions to configure and manage pins/ports. Are supposed to be faster than native arduino function, eventhough some special care is needed with ports.
The ports needs to be chosen accordingly if the GPIOs are in standart or fast mode. They are in fast mode by default, and can be changeg individually (to be checked which bits corresponds to each pin) to normal mode with these lines of code: 

```
// going back to normal GPIO mode (port 1-5)
IOMUXC_GPR_GPR26 = 0x0; // 0xFFFFFFFF to use fast GPIO
IOMUXC_GPR_GPR27 = 0x0;
IOMUXC_GPR_GPR28 = 0x0;
IOMUXC_GPR_GPR29 = 0x0;
```

# PIT.h/cpp
This files configures and runs the 4 Periodic Interval Timers, it is inspired by the already existing library from Teensy: ```#include <IntervalTimer.h>```