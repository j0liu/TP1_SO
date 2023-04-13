#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"
#include "resultIOADT.h"

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 

  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(1);
  }
  int pid = getpid();
  int qtyFiles = argc - 1;
  resultIOADT resultIO = createResultIOADT(pid, IO_WRITE | IO_SHM | IO_SEM, qtyFiles);
  if (resultIO == NULL) exit(1);
  printf("%d %d\n", pid, qtyFiles);
  sleep(2);

  for (int i = 0; i < qtyFiles; i++){
    char *filename = argv[i + 1];
    if (writeEntry(resultIO, filename) == -1)
      exit(1);
    sleep(1);
  }
  freeResultIOADT(resultIO);
  return 0;
}