#pragma once

#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

byte macAddressTeensy[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ipAddressTeensy(10, 0, 0, 1);
IPAddress switchAddress(129, 241, 187, 1);
IPAddress targetAddress(10, 0, 0, 2);

unsigned int localPort = 8888;
unsigned int targetPort = 8888;

void basicSetup();

void ethernetSetup();

void ethernetLoop();