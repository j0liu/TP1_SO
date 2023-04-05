#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include "helpers.h"
#include "m2sADT.h"
#include "slave-controller.h"

typedef struct slaveControllerCDT {
    masterToSlaveADT * m2sList;
    int nfds;
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
  slaveContr->nfds = getSMReadFd(slaveContr->m2sList[qtySlaves-1]) + 1;

  return slaveContr;
}

static masterToSlaveADT getReadySlave(slaveControllerADT slaveContr) {
  fd_set fileDescSet;
  FD_ZERO(&fileDescSet);

  for (int i = 0; i < slaveContr->size; i++)
    FD_SET(getSMReadFd(slaveContr->m2sList[i]), &fileDescSet); // Agregar fd del pipe S->M a fileDescSet
  
  int retval = select(slaveContr->nfds, &fileDescSet, NULL, NULL, NULL);
  if (retval != -1) {
    for (int i = 0; i < slaveContr->size; i++)
      if (FD_ISSET(getSMReadFd(slaveContr->m2sList[i]), &fileDescSet))
        return slaveContr->m2sList[i];
  } else perror("select");
  return NULL;
}

char * getAvailableMD5Result(slaveControllerADT slaveContr, char * md5) {
  masterToSlaveADT m2s = getReadySlave(slaveContr);
  if (m2s == NULL) return NULL;
  return readMD5Result(m2s, md5);
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
    freeMasterToSlaveADT(slaveContr->m2sList[i]);
  free(slaveContr->m2sList);
  free(slaveContr);
}