#include "m2sADT.h"
#include "slave-controller.h"
#include <stdlib.h>
#include <stdio.h>
#include "helpers.h"

typedef struct slaveControllerCDT {
    masterToSlaveADT * m2sList;
    int size;
} slaveControllerCDT;

slaveControllerADT createSlaveControllerADT(int qtySlaves) {
  slaveControllerADT slaveContr;
  slaveContr = safeMalloc(sizeof(slaveControllerCDT), "malloc slaveController");
  if (slaveContr == NULL) return NULL;

  slaveContr->m2sList = safeMalloc(sizeof(masterToSlaveADT) * qtySlaves, "malloc masterToSlavelist");
  if (slaveContr->m2sList == NULL) return NULL;
  
  slaveContr->size = qtySlaves;
  for (int i = 0; i < qtySlaves; i++)
    slaveContr->m2sList[i] = createMasterToSlaveADT();
  return slaveContr;
}

char * getAvailableMD5Result(slaveControllerADT slaveContr) {
  return NULL;
}

int sendFile(slaveControllerADT slaveContr, int index, char * filename) {
  if (index < 0 || index >= slaveContr->size) return -1;
  return sendFileName(slaveContr->m2sList[index], filename);
}

int getIdleSlaveIndex(slaveControllerADT slaveContr){
    return 0;
}

void freeSlaveControllerADT(slaveControllerADT slaveContr) {
  for(int i = 0; i < slaveContr->size; i++) 
    free(slaveContr->m2sList[i]);
  free(slaveContr->m2sList);
  free(slaveContr);
}