#ifndef ABCC_H
#define ABCC_H
typedef struct abcc
{
    char *IP_ADDRESS;
}abcc;

int init_abcc(abcc *device);
#endif