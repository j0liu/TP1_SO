#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "resultIOADT.h"
#include "helpers.h"

#define USAGE "Usage: %s [md5 pid] [qtyFiles]\n"
#define INVALID_ARGUMENTS "Invalid arguments: %s %s\n"
int initNamedPipe(int pid);

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 
  
  int pid = 0;
  int qtyFiles = 0;
  if (argc != 3 && argc != 1) {
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  } else if (argc == 3) {
    pid = atoi(argv[1]);
    qtyFiles = atoi(argv[2]);
    if (pid <= 0 || qtyFiles <= 0) {
      fprintf(stderr, INVALID_ARGUMENTS, argv[1], argv[2]);
      exit(EXIT_FAILURE);
    }
  } else {
    scanf("%d %d", &pid, &qtyFiles);
  }

  //resultIOADT resultIO = createResultIOADT(pid, IO_READ | IO_SHM | IO_SEM, qtyFiles);
  //if (resultIO == NULL) exit(EXIT_FAILURE);
  char buffer[ROW_LEN] = {0};
  //int entryLen = 0;
  int received = 0;
  int fdNamedPipe = initNamedPipe(pid);
  do {
    //entryLen = readEntry(resultIO, buffer);
    read(fdNamedPipe, buffer, 1000);
    printf("y el resultado es %s\n", buffer);
  } while (received++ < qtyFiles);
  close(fdNamedPipe);
  //freeResultIOADT(resultIO);
  return 0;
}

int initNamedPipe(int pid) {
  char pipeName[1000] = {0};
  sprintf(pipeName, "/pipe%d", pid);
  int fd = open(pipeName, O_RDONLY);
  return fd;
}