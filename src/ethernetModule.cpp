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

void ethernetSetup() { 
    Ethernet.begin(macAddressTeensy,ipAddressTeensy);
    Udp.begin(localPort);
}

void ethernetCheck() {
    int packetSize = Udp.parsePacket();
    if (packetSize != 0) {                           //if part to confirm recieved UDP package
        Udp.read(recievedString, UDP_TX_PACKET_MAX_SIZE);
        Udp.beginPacket(targetAddress, targetPort);
        Udp.write("Recieved UDP packet containing: ");
        Udp.write(recievedString);
        Udp.endPacket();
    }
    else {
        Udp.beginPacket(targetAddress, targetPort);
        Udp.write(stringSent);
        Udp.endPacket();
  }
}
