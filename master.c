#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "slave-controller.h"

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

  int qtyFiles = argc - 1;

  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return 1;

  for (int i = 0; i < qtyFiles ; i++)
    sendFile(slaveContr, i, argv[i + 1]);
  
  freeSlaveControllerADT(slaveContr);

  return 0;
}