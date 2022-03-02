#include <ethernetModule.h>
#include <Arduino.h>

char defaultMessage[] = "Default Message"; 
// This can be any wanted character array / string

int main() {
  eth::setup();

  while(1) {
    eth::read();
    eth::write(defaultMessage);
    delay(1000);
  }
}
