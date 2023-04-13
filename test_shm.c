#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "resultIOADT.h"

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 

  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(1);
  }
  int pid = getpid();
  int qtyFiles = argc - 1;
  resultIOADT resultShmIO = createResultIOADT(pid, IO_WRITE | IO_SHM | IO_SEM, qtyFiles);
  if (resultShmIO == NULL) exit(1);
  printf("%d %d\n", pid, qtyFiles);
  sleep(2);

  resultIOADT resultFileIO = createResultIOADT(pid, IO_WRITE | IO_FILE | IO_SEM_DIS, qtyFiles);
  if (resultFileIO == NULL) exit(1);

  for (int i = 0; i < qtyFiles; i++){
    char *filename = argv[i + 1];
    if (writeEntry(resultShmIO, filename) == -1)
      exit(1);
    if (writeEntry(resultFileIO, filename) == -1)
      exit(1);
    sleep(1);
  }
  freeResultIOADT(resultShmIO);
  freeResultIOADT(resultFileIO);
  return 0;
}