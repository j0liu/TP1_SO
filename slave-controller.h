#ifndef SLAVE_CONTROLLER_H
#define SLAVE_CONTROLLER_H

#include <stdlib.h>

typedef struct slaveControllerCDT * slaveControllerADT;

slaveControllerADT createSlaveControllerADT(int qtySlaves);
char * getAvailableMD5Result(slaveControllerADT slaveContr, char * md5);
int sendFile(slaveControllerADT slaveContr, int index, char * filename);
int getIdleSlaveIndex(slaveControllerADT slaveContr);
void freeSlaveControllerADT(slaveControllerADT slaveContr);

#endif