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
#include "md5sumADT.h"
#include "helpers.h"
#define MD5SUM "md5sum"

typedef struct md5sumCDT {
  int fdMd5Read; // File descriptor del pipe de lectura de md5sum
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
  } else if (!pid) { 
    dup2(pipe[PIPE_WRITE], STDOUT_FILENO);
    close(pipe[PIPE_READ]);
    close(pipe[PIPE_WRITE]);
    char * paramList[] = {MD5SUM, filename, NULL};
    execvp(MD5SUM, paramList); // Ejecutar md5sum sin usar path absoluto 
  }
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