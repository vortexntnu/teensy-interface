#include <headerConfig.h>

void serialAndLEDSetup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}