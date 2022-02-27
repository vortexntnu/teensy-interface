#include <ethernetModule.h>
#include <headerConfig.h>
#include <Arduino.h>

int main() {
  eth::ethernetSetup();

  while(1) {
    eth::ethernetCheck();
    delay(1000);
  }
}
