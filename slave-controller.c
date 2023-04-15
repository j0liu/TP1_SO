// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include "helpers.h"
#include "m2sADT.h"
#include "slave-controller.h"

/**
 * @brief Devuelve el menor indice de un slave listo para leer 
 * 
 * @param slaveContr ADT del slaveController 
 * @return int Indice del slave listo para leer o -1 si hay un error
 */
static int getReadySlaveIndex(slaveControllerADT slaveContr);

typedef struct slaveControllerCDT {
    masterToSlaveADT * m2sList;   // Lista de m2sADT 
    int nfds;                     // File descriptor maximo + 1
    int size;                     // Cantidad de slaves
    int filesSent;                // Cantidad de archivos enviados
    int filesReceived;            // Cantidad de archivos recibidos
} slaveControllerCDT;

slaveControllerADT createSlaveControllerADT(int qtySlaves) {
  slaveControllerADT slaveContr;
  slaveContr = safeMalloc(sizeof(slaveControllerCDT), "malloc slaveController");
  if (slaveContr == NULL) return NULL;

  slaveContr->m2sList = safeMalloc(sizeof(masterToSlaveADT) * qtySlaves, "malloc masterToSlavelist");
  if (slaveContr->m2sList == NULL) return NULL;
  
  slaveContr->filesReceived = 0;
  slaveContr->filesSent = 0;
  
  slaveContr->size = qtySlaves;
  for (int i = 0; i < qtySlaves; i++)
    slaveContr->m2sList[i] = createMasterToSlaveADT();
  slaveContr->nfds = getSMReadFd(slaveContr->m2sList[qtySlaves-1]) + 1;

  return slaveContr;
}

int getFilesSent(slaveControllerADT slaveContr) {
  return slaveContr->filesSent;
}

int getFilesReceived(slaveControllerADT slaveContr){
  return slaveContr->filesReceived;
}

void incrementFilesSent(slaveControllerADT slaveContr) {
  slaveContr->filesSent++;
}

void incrementFilesReceived(slaveControllerADT slaveContr) {
  slaveContr->filesReceived++;
}

static int getReadySlaveIndex(slaveControllerADT slaveContr) {
  fd_set fileDescSet;
  FD_ZERO(&fileDescSet);

  for (int i = 0; i < slaveContr->size; i++)
    FD_SET(getSMReadFd(slaveContr->m2sList[i]), &fileDescSet); // Agregar fd del pipe S->M a fileDescSet
  
  int retval = select(slaveContr->nfds, &fileDescSet, NULL, NULL, NULL);
  if (retval != -1) {
    for (int i = 0; i < slaveContr->size; i++)
      if (FD_ISSET(getSMReadFd(slaveContr->m2sList[i]), &fileDescSet))
        return i;
  } else perror("select");
  return -1;
}

char * getAvailableMD5Result(slaveControllerADT slaveContr, int * index) {
  *index = getReadySlaveIndex(slaveContr);
  if (*index == -1) return NULL; 
  return readMD5Result(slaveContr->m2sList[*index]);
}

ssize_t sendFile(slaveControllerADT slaveContr, int index, char * filename) {
  if (index < 0 || index >= slaveContr->size) return -1;
  return sendFileName(slaveContr->m2sList[index], filename);
}

ssize_t sendFileIfIdle(slaveControllerADT slaveContr, int index, char * filename) {
  if (slaveContr == NULL || index < 0 || index >= slaveContr->size) return -1;
  if (isIdle(slaveContr->m2sList[index]))
    return sendFile(slaveContr, index, filename);
  return -1;
}

void freeSlaveControllerADT(slaveControllerADT slaveContr) {
  for(int i = 0; i < slaveContr->size; i++) 
    freeMasterToSlaveADT(slaveContr->m2sList[i]);
  free(slaveContr->m2sList);
  free(slaveContr);
}