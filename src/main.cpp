#include <SPI.h>        
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>



// Enter a MAC address and IP address for your controller below.

// The IP address will be dependent on your local network:

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192, 168, 1, 177);

unsigned int localPort = 8888;      // local port to listen on

char ReplyBuffer[] = "messageSent";

// An EthernetUDP instance to let us send and receive packets over UDP

EthernetUDP Udp;

void setup() {

  // start the Ethernet and UDP:
  Serial.begin(9600); 

  Ethernet.begin(mac,ip);

  Udp.begin(localPort);

  pinMode(LED_BUILTIN, OUTPUT);

}



void loop() {

  IPAddress remote = Udp.remoteIP(); 
  Udp.beginPacket(remote, Udp.remotePort());

    Udp.write(ReplyBuffer);
    Serial.print(remote); 

    int status = Udp.endPacket();
    
    if (status == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      digitalToggle(LED_BUILTIN);
    }
<<<<<<< HEAD

    delay(100);
}
=======
    delay(1000);
}
>>>>>>> c5dec2e (added python udp listening program)
