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

char * getlineFd(int fd) {
  FILE * file = fdopen(fd, "r");
  char * line = NULL;
  size_t n = 0;
  size_t len = getline(&line, &n, file); 
  line[len-1] = '\0';
  return line;
}