#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "m2sADT.h"
#include "s2mADT.h"

#define PARAM_ERROR 1
#define FILE_ERROR  2
#define QUANTITY_SLAVES 5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(PARAM_ERROR);
  }
  
  for (int i = 1; i < argc; i++){
    if (!fileExists(argv[i])) {
      perror("file_exists");
      fprintf(stderr, "Invalid file %s\n", argv[i]);
      exit(FILE_ERROR);
    }
  }
  //for (int i = 1; i < argc; i++) {
    masterToSlaveADT m2s = createMasterToSlaveADT();
    printf(getFdMSWrite(m2s), argv[1]);
   //}

  return 0;
}