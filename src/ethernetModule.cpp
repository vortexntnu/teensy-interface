#include <ethernetModule.h>

byte macAddressTeensy[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ipAddressTeensy(10, 0, 0, 1);
IPAddress switchAddress(129, 241, 187, 1);
IPAddress targetAddress(10, 0, 0, 2);

unsigned int localPort = 8888;
unsigned int targetPort = 8888;

EthernetUDP Udp;


char stringSent[] = "No packets recieved, 5 second cooldown set";
char recieveConfirmed[] = "PacketRecieved";
char recievedString[UDP_TX_PACKET_MAX_SIZE];

void serialAndLEDSetup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void ethernetSetup() { 
    Ethernet.begin(macAddressTeensy,ipAddressTeensy);
    int udpStatus = Udp.begin(localPort);

    if (udpStatus == 1) {
        Serial.print("UDP setup successful!");
    }
    else {
        Serial.print("UDP setup failed!");
    }
}

void ethernetCheck() {
    int packetSize = Udp.parsePacket();
    if (packetSize != 0) {                           //if part to confirm recieved UDP package
        Serial.print("Received packet of size ");      //by serial printing address recieved from
        Serial.println(packetSize);
        Serial.print("From ");
        for (int i=0; i < 4; i++) {
            Serial.print(targetAddress[i], DEC);
            if (i < 3) {
                Serial.print(".");
            }
        }
        Serial.println("\n");

        Udp.read(recievedString, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");
        Serial.println(recievedString);

        Udp.beginPacket(targetAddress, targetPort);
        Udp.write("Recieved UDP packet containing: ");
        Udp.write(recievedString);
        Udp.endPacket();
        digitalWrite(LED_BUILTIN, 1);
        delay(1000);
    }
    else {
        Udp.beginPacket(targetAddress, targetPort);
        Udp.write(stringSent);
        Udp.endPacket();
        digitalWrite(LED_BUILTIN, 0);
        delay(5000);                                  //5sec delay to not overload the recieving end terminal
  }
}
