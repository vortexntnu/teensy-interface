#include <ethernetModule.h>
#include <headerConfig.h>
#include <Arduino.h>

int main() {
  serialAndLEDSetup();
  ethernetSetup();

  while(1) {
    ethernetCheck();
  }
}
