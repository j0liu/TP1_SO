#ifndef HELPERS_H
#define HELPERS_H
  #include <stdlib.h>
  #define PIPE_READ           0
  #define PIPE_WRITE          1
  #define MAX_PATH_LENGTH     4096
  #define MD5_LENGTH          32
  #define MAX_PID_DIGITS      5 
  int fileExists(const char *filename);
  int isDir(const char *filename);
  void * safeMalloc(size_t size, const char * msg);
  int createPipe(int * pipeFds);
  char * getlineFd(int fd);
#endif // HELPERS_H
