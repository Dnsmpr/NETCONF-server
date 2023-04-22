#include "abcc.h"

int init_abcc(abcc *device) {
    char* IP = "192.168.0.1";
    device->IP_ADDRESS = strdup(IP);
    if(!device->IP_ADDRESS)
        return -1;

    return 0;

}