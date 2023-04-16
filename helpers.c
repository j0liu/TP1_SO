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
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define ERROR_MSG_IS_FILE_STAT "isFile stat"
#define ERROR_MSG_PIPE "pipe"

int fileExists (const char * path) {
  struct stat sb;
  return !stat(path, &sb);
}

int isFile(const char * path) {
  errno = 0;
  struct stat sb;
  if (stat(path, &sb) == -1){
    perror(ERROR_MSG_IS_FILE_STAT);
    return -1;
  }
  return S_ISREG(sb.st_mode);
}

void * safeMalloc(size_t size, const char * msg){
  errno = 0;
  void *aux = malloc(size);
  if (aux == NULL) perror(msg);
  return aux;
}

int createPipe(int pipeFds[]) {
  errno = 0;  
  if (pipe(pipeFds) < 0) {
    perror(ERROR_MSG_PIPE);
    return -1;
  }
  return 0;
}