#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <string.h>

/*
So far implemented, no clue if tested:



*/

namespace eth
{

    enum status
    {
        SUCCESS,
        FAILURE
    };

    status setup();

    // status check();

    status write(char *message);
    status write(uint16_t data);

    char *read();
};