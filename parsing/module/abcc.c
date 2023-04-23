#include "abcc.h"

int init_abcc(abcc *device) {
    char* IP = "192.168.1.150";
    device->IP_ADDRESS = strdup(IP);
    if(!device->IP_ADDRESS)
        return -1;

    return 0;

}

int set_IP_ADDRESS(abcc *device, char *IP) {
    if(!IP)
        return -1;
    device->IP_ADDRESS = strdup(IP);
    if(!device->IP_ADDRESS)
        return -1;

    return 0;

}