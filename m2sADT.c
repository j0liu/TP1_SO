#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "m2sADT.h"
#include <string.h>
#include "helpers.h"

#define SLAVE "slave"

typedef struct masterToSlaveCDT {
  int pid;
  int fdMSWrite;
  int fdSMRead;
  int remainingTasks;
} masterToSlaveCDT;

/*int getMSWriteFd(masterToSlaveADT m2s) {
  return m2s->fdMSWrite;
}*/

int getSMReadFd(masterToSlaveADT m2s) {
  return m2s->fdSMRead;
}

static int createSlave(int * pipeM2S, int * pipeS2M) {
  errno = 0;  
  int slavePid = fork();
  if (slavePid == -1) {
      perror("fork");
      return -1;
  }
  else if (!slavePid) { // slave
     dup2(pipeM2S[PIPE_READ], STDIN_FILENO); // TODO: Agregar proteccion?
     dup2(pipeS2M[PIPE_WRITE], STDOUT_FILENO);
     close(pipeM2S[PIPE_READ]);
     close(pipeM2S[PIPE_WRITE]);
     close(pipeS2M[PIPE_READ]);
     close(pipeS2M[PIPE_WRITE]);
     char * const paramList[] = {SLAVE, NULL};
     execve(SLAVE, paramList, 0); // TODO: Agregar proteccion?
  } // fin de slave
  close(pipeM2S[PIPE_READ]);
  close(pipeS2M[PIPE_WRITE]);
  return slavePid;
}

masterToSlaveADT createMasterToSlaveADT() {
  int pipeM2S[2]; // Pipe para enviar nombres a slave
  if (createPipe(pipeM2S) == -1)
    return NULL;

  int pipeS2M[2]; // Pipe para recibir md5 de slave
  if (createPipe(pipeS2M) == -1)
    return NULL;
  
  int slavePid;
  if ((slavePid = createSlave(pipeM2S, pipeS2M)) == -1)
    return NULL;

  masterToSlaveADT m2s;
  if ((m2s = malloc(sizeof(masterToSlaveCDT))) == NULL) {
    perror("malloc masterToSlaveADT");
    return NULL;
  }
  m2s->pid = slavePid;
  m2s->fdMSWrite = pipeM2S[PIPE_WRITE];
  m2s->fdSMRead = pipeS2M[PIPE_READ];
  m2s->remainingTasks = 0;
  
  return m2s; 
}

ssize_t sendFileName(masterToSlaveADT m2s, char * filename) {
  ssize_t result = write(m2s->fdMSWrite, filename, strlen(filename));
  write(m2s->fdMSWrite, "\n", 1);
  m2s->remainingTasks++;
  return result;
}

char * readMD5Result(masterToSlaveADT m2s) {
  m2s->remainingTasks--;
  int totalLen;
  read(m2s->fdSMRead, &totalLen, sizeof(int));
  char * buff = safeMalloc(totalLen, "malloc readMD5Result");
  if (buff == NULL) return NULL;
  read(m2s->fdSMRead, buff, totalLen);
  return buff;
}

int isIdle(masterToSlaveADT m2s) {
  return m2s->remainingTasks == 0;
}

void freeMasterToSlaveADT(masterToSlaveADT m2s) {
  free(m2s);
}