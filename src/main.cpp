#include <ethernetModule.h>
#include <Arduino.h>
#include <gpioInterrupt.h>

int main() {
  setupGPIOInterrupt();
  while(1) {
    delay(1000);
  }
}
