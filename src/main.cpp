#include <ethernetModule.h>

void setup() {
  basicSetup();
  ethernetSetup();
}

void loop() {
  ethernetLoop();
}
