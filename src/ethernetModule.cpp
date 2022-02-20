#include <ethernetModule.h>

/*
extern byte macAddressTeensy[];
extern IPAddress ipAddressTeensy;
extern IPAddress switchAddress;
extern IPAddress targetAddress;
*/

EthernetUDP Udp;
/*
extern unsigned int localPort;
extern unsigned int targetPort;
*/

char stringSent[] = "No packets recieved, 5 second cooldown set";
char recieveConfirmed[] = "PacketRecieved";
char recievedString[UDP_TX_PACKET_MAX_SIZE];

void basicSetup() {
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

void ethernetLoop() {
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
        delay(1000);
    }
    else {
        Udp.beginPacket(targetAddress, targetPort);
        Udp.write(stringSent);
        Udp.endPacket();
        delay(5000);                                  //5sec delay to not overload the recieving end terminal
  }
}