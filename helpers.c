#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int fileExists (char * path) {
    struct stat sb;
    return !stat(path, &sb);
}

void * safeMalloc(size_t size, const char * msg){
    void * aux = malloc(size);
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