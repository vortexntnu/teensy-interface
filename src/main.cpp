#include <ethernetModule.h>
#include <Arduino.h>
#include "gpio.h"
#include "interrupts.h"

char defaultMessage[] = "No package received";
// This can be any wanted character array / string

static void send_on_interrupt(void) {
    uint16_t data = read_pins();
    #ifdef SERIAL_DEBUG
    Serial.printf("%u\n",data); 
    #endif
    eth::write(data); 
} 

int main() {

    #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial to connect
    }
    Serial.printf("Serial connected\r\n");
    #endif

  gpio_setup(); 

  eth::setup();

  timer::setUpPeriodic();
  timer::setUpPeriodicISR(send_on_interrupt);
  timer::startPeriodic();   

  while (1)
  {
    
  }
  

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
