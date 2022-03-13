#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include <gpioInterrupt.h>

char defaultMessage[] = "No package received";
int main() {
  setupGPIOInterrupt();
  while(1) {
    delay(1000);
  }
}
