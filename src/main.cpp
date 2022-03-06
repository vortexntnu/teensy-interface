#include <ethernetModule.h>
#include <Arduino.h>

char defaultMessage[] = "No package received";
// This can be any wanted character array / string

int main() {
  eth::setup();

  while(1) {
    char* rec_message = eth::read();
    if(rec_message != nullptr) {
        eth::write(rec_message);
    }
    else {
        eth::write(defaultMessage);
    }
    delay(1000);
  }
}
