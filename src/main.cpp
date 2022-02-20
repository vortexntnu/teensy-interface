#include <SPI.h>        
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

constexpr int SUCCESSFUL = 1;
constexpr int FAILIURE = 0;

byte macAddressTeensy[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ipAddressTeensy(10, 0, 0, 1);
IPAddress switchAddress(129, 241, 187, 1);
IPAddress targetAddress(10, 0, 0, 2);

unsigned int localPort = 8888;
unsigned int targetPort = 8888;

char stringSent[] = "No packets recieved, 5 second cooldown set";
char recieveConfirmed[] = "PacketRecieved";
char recievedString[UDP_TX_PACKET_MAX_SIZE];

// An EthernetUDP instance to let us send and receive packets over UDP

EthernetUDP Udp;

void setup() {

  // start the Ethernet and UDP:
  Serial.begin(9600); 
  Ethernet.begin(macAddressTeensy,ipAddressTeensy);
  Udp.begin(localPort);

  
  Serial.println("Ethernet and UDP is set up!");

  pinMode(LED_BUILTIN, OUTPUT);

}



void loop() {

  int packetSize = Udp.parsePacket();
  int status;
  if (packetSize != 0) {                           //if part to confirm recieved UDP package
    Serial.print("Received packet of size ");      //by serial printing address recieved from
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.println("\n");

    Udp.read(recievedString, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(recievedString);

    Udp.beginPacket(targetAddress,targetPort);
    Udp.write("Recieved UDP packet containing: ");
    Udp.write(recievedString);
    status = Udp.endPacket();
    delay(1000);
  }
  else {
    Udp.beginPacket(targetAddress, targetPort);
    Udp.write(stringSent);
    status = Udp.endPacket();
    delay(5000);                                  //5sec delay to not overload the recieving end terminal
  }
  
  if (status == SUCCESSFUL) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalToggle(LED_BUILTIN);
  }

  delay(1000);
}
