#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "resultIOADT.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                   } while (0)

#define BUF_SIZE 1024   

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 
  
  int pid = 0;
  int qtyFiles = 0;
  if (argc != 3 && argc != 1) {
    fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
    exit(EXIT_FAILURE);
  } else if (argc == 3) {
    pid = atoi(argv[1]);
    qtyFiles = atoi(argv[2]);
  } else {
    scanf("%d %d", &pid, &qtyFiles);
  }

  resultIOADT resultIO = createResultIOADT(pid, IO_READ | IO_SHM | IO_SEM, qtyFiles);
  if (resultIO == NULL)
    exit(1);
  char buffer[ROW_LEN] = {0};
  int entryLen = 0;
  do {
    entryLen = readEntry(resultIO, buffer);
    buffer[entryLen-1]  = '\0';
    puts(buffer);
  } while (entryLen > 0);
  freeResultIOADT(resultIO);
  return 0;
}