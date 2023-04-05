#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "md5sumADT.h"
#include "helpers.h"
#define MD5SUM "/usr/bin/md5sum"

typedef struct md5sumCDT {
  int fdMd5Read;
} md5sumCDT;

md5sumADT createMD5sumADT(){
  md5sumADT md5sum = safeMalloc(sizeof(md5sumADT), "malloc md5sumADT");
  if (md5sum == NULL) return NULL;
  md5sum->fdMd5Read = -1;
  return md5sum;
}

int sendFileName(md5sumADT md5sum, char * filename) {
  if (md5sum == NULL || md5sum->fdMd5Read != -1) return -1;

  int pipe[2];
  if (createPipe(pipe) == -1)
    return -1;

  errno = 0;
  int pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  }
  else if (!pid) { // md5sum
    dup2(pipe[PIPE_WRITE], STDOUT_FILENO);
    close(pipe[PIPE_READ]);
    close(pipe[PIPE_WRITE]);
    char * paramList[] = {MD5SUM, filename, NULL};
    execve(MD5SUM, paramList, 0); // TODO: Agregar proteccion?
  } // fin md5sum 
  close(pipe[PIPE_WRITE]);
  md5sum->fdMd5Read = pipe[PIPE_READ];
  return pid;
}

ssize_t readMD5Result(md5sumADT md5sum, char * md5, int resultLen){
  if (md5sum == NULL || md5sum->fdMd5Read == -1) return -1;
  int result = read(md5sum->fdMd5Read, md5, resultLen);
  md5[result] = '\0';
  md5sum->fdMd5Read = -1;
  return result;
}


void freeMD5sumADT(md5sumADT md5sum){
    free(md5sum);
}