// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "m2sADT.h"
#include <string.h>
#include "helpers.h"

#define SLAVE "slave" // Nombre del archivo slave
#define ERROR_MSG_FORK "fork"
#define ERROR_MSG_MALLOC_M2SADT "malloc masterToSlave"
#define ERROR_MSG_MALLOC_READMD5 "malloc readMD5Result"

/**
 * @brief  Crea un proceso slave y lo vincula con master mediante los pipes recibidos
 *  
 * @param  pipeM2S Array de 2 enteros de los file descriptors correspondientes al pipe de master a slave
 * @param  pipeS2M Array de 2 enteros de los file descriptors correspondientes al pipe de slave a master
 * @return Pid del slave o -1 si hubo un error 
 */
static int createSlave(int pipeM2S[], int pipeS2M[]);

typedef struct masterToSlaveCDT {
  int pid;              // Pid del slave
  int fdMSWrite;        // File descriptor del extremo de escritura del pipeMS
  int fdSMRead;         // File descriptor del extremo de lecutra del pipeSM
  int remainingTasks;   // Cantidad de tareas restantes
} masterToSlaveCDT;


int getSMReadFd(masterToSlaveADT m2s) {
  if (m2s == NULL) return -1;
  return m2s->fdSMRead;
}

static int createSlave(int pipeM2S[], int pipeS2M[]) {
  errno = 0;  
  int slavePid = fork();
  if (slavePid == -1) {
      perror(ERROR_MSG_FORK);
      return -1;
  }
  else if (!slavePid) {
      dup2(pipeM2S[PIPE_READ], STDIN_FILENO);
      dup2(pipeS2M[PIPE_WRITE], STDOUT_FILENO);
      for (int fd = STDERR_FILENO + 1; fd <= pipeS2M[PIPE_WRITE]; fd++)
        close(fd);

    char *const paramList[] = {SLAVE, NULL};
    execve(SLAVE, paramList, 0);
    perror(SLAVE);
    exit(EXIT_FAILURE);
  } 
  close(pipeM2S[PIPE_READ]);
  close(pipeS2M[PIPE_WRITE]);
  return slavePid;
}

masterToSlaveADT createMasterToSlaveADT() {
  int pipeM2S[2]; // Pipe para enviar nombres a slave
  if (createPipe(pipeM2S) == -1) return NULL;

  int pipeS2M[2]; // Pipe para recibir md5 de slave
  if (createPipe(pipeS2M) == -1) return NULL;
  
  int slavePid;
  if ((slavePid = createSlave(pipeM2S, pipeS2M)) == -1)
    return NULL;

  masterToSlaveADT m2s = safeMalloc(sizeof(masterToSlaveADT), ERROR_MSG_MALLOC_M2SADT);
  if (m2s == NULL) return NULL;

  m2s->pid = slavePid;
  m2s->fdMSWrite = pipeM2S[PIPE_WRITE];
  m2s->fdSMRead = pipeS2M[PIPE_READ];
  m2s->remainingTasks = 0;
  
  return m2s; 
}

ssize_t sendFileName(masterToSlaveADT m2s, char * filename) {
  if (m2s == NULL) return -1;
  ssize_t result = write(m2s->fdMSWrite, filename, strlen(filename));
  write(m2s->fdMSWrite, "\n", 1);
  m2s->remainingTasks++;
  return result;
}

char * readMD5Result(masterToSlaveADT m2s) {
  if (m2s == NULL) return NULL;

  m2s->remainingTasks--;
  int totalLen;
  read(m2s->fdSMRead, &totalLen, sizeof(int));
  if (totalLen == EOF) return NULL; // El slave termino inesperadamente
  char * buff = safeMalloc(totalLen, ERROR_MSG_MALLOC_READMD5);
  if (buff == NULL) return NULL;
  read(m2s->fdSMRead, buff, totalLen);
  return buff;
}

int isIdle(masterToSlaveADT m2s) {
  if (m2s == NULL) return -1;
  return m2s->remainingTasks == 0;
}

void freeMasterToSlaveADT(masterToSlaveADT m2s) {
  if (m2s == NULL) return;
  close(m2s->fdMSWrite);
  close(m2s->fdSMRead);
  free(m2s);
}