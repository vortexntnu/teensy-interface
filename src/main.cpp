#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include <gpioInterrupt.h>

char defaultMessage[] = "No package received";
int main() {
  Serial.begin(9600);
  while (!Serial) {}
  setupGPIOInterrupt();
  while(1) {

    Serial.printf("TOUCHED PINS: %X\r\n", GPIO8_DR);
    delay(1000);
    GPIO8_DR &= ~0xC40000;
  }
}
