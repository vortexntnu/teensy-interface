#pragma once

#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <string.h>

namespace eth {

    enum status {
        SUCCESS, FAILURE
    };

    status setup();

    //status check();

    status write(char* message);

    char* read();
};