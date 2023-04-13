#ifndef SLAVE_CONTROLLER_H
#define SLAVE_CONTROLLER_H

#include <stdlib.h>
#include <sys/types.h>

typedef struct slaveControllerCDT * slaveControllerADT;

slaveControllerADT createSlaveControllerADT(int qtySlaves);
int getFilesSent(slaveControllerADT slaveContr);
int getFilesReceived(slaveControllerADT slaveContr);
void incrementFilesSent(slaveControllerADT slaveContr);
void incrementFilesReceived(slaveControllerADT slaveContr);
char * getAvailableMD5Result(slaveControllerADT slaveContr, int * index);
ssize_t sendFile(slaveControllerADT slaveContr, int index, char * filename);
ssize_t sendFileIfIdle(slaveControllerADT slaveContr, int index, char * filename);
int getIdleSlaveIndex(slaveControllerADT slaveContr);
void freeSlaveControllerADT(slaveControllerADT slaveContr);

#endif