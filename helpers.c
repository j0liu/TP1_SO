// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int fileExists (const char * path) {
  struct stat sb;
  return !stat(path, &sb);
}

int isDir(const char * path) {
  struct stat sb;
  stat(path, &sb);
  return S_ISDIR(sb.st_mode);
}

void * safeMalloc(size_t size, const char * msg){
  void *aux = malloc(size);
  if (aux == NULL) perror(msg);
  return aux;
}

int createPipe(int * pipeFds) {
  errno = 0;  
  if (pipe(pipeFds) < 0) {
    perror("pipe");
    return -1;
  }
  return 0;
}