#pragma once

#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace eth {

    enum status {
        success, failure
    };

    status ethernetSetup();

    status ethernetCheck();
};