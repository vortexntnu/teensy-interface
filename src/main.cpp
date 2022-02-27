#include <ethernetModule.h>
#include <Arduino.h>

void setup() {
  serialAndLEDSetup();
  ethernetSetup();
}

void loop() {
  ethernetCheck();
}
