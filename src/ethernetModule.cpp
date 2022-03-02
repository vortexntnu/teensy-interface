#include <ethernetModule.h>

EthernetUDP Udp;

//GLOBALS
byte macAddressTeensy[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ipAddressTeensy(10, 0, 0, 1);
//IPAddress switchAddress(129, 241, 187, 1);
IPAddress targetAddress(10, 0, 0, 2);

unsigned int localPort = 8888;
unsigned int targetPort = 8888;

char recievedString[UDP_TX_PACKET_MAX_SIZE];

namespace eth {

    status setup() { 
        Ethernet.begin(macAddressTeensy,ipAddressTeensy);
        if (Udp.begin(localPort)) {
            return SUCCESS;
        }
        else {
            return FAILURE;
        }
    }

    status write(char* message) {
        int udpStatus1 = Udp.beginPacket(targetAddress, targetPort);
        Udp.write(message);
        int udpStatus2 = Udp.endPacket();
        if (udpStatus1 && udpStatus2) {
            return SUCCESS;
        }
        else {
            return FAILURE;
        }   
    }
    
    char* read() {
        if (Udp.parsePacket() != 0) {                           //if part to confirm recieved UDP package
            Udp.read(recievedString, UDP_TX_PACKET_MAX_SIZE);
            return recievedString;
        }
        else {
            return nullptr;
        }
    }
};