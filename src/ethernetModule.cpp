#include <ethernetModule.h>

EthernetUDP Udp;


namespace eth {


    byte macAddressTeensy[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ipAddressTeensy(10, 0, 0, 1);
    //IPAddress switchAddress(129, 241, 187, 1);
    IPAddress targetAddress(10, 0, 0, 2);

    unsigned int localPort = 8888;
    unsigned int targetPort = 8888;

    char stringSent[] = "No packets recieved";
    //char recieveConfirmed[] = "Packet recieved";
    char recievedString[UDP_TX_PACKET_MAX_SIZE];

    status ethernetSetup() { 
        Ethernet.begin(macAddressTeensy,ipAddressTeensy);
        int udpStatus = Udp.begin(localPort);
        if (udpStatus == 1) {
            return success;
        }
        else {
            return failure;
        }
    }

    status ethernetCheck() {
        int packetSize = Udp.parsePacket();
        if (packetSize != 0) {                           //if part to confirm recieved UDP package
            int udpStatus1 = Udp.read(recievedString, UDP_TX_PACKET_MAX_SIZE);
            int udpStatus2 = Udp.beginPacket(targetAddress, targetPort);
            Udp.write("Recieved UDP packet containing: ");
            Udp.write(recievedString);
            int udpStatus3 = Udp.endPacket();
            if (udpStatus1 && udpStatus2 && udpStatus3) {
                return success;
            }
            else {
                return failure;
            }
        }
        else {
            int udpStatus1 = Udp.beginPacket(targetAddress, targetPort);
            Udp.write(stringSent);
            int udpStatus2 = Udp.endPacket();
            if (udpStatus1 && udpStatus2) {
                return success;
            }
            else {
                return failure;
            }
        }
    }
};