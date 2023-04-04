#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "slave-controller.h"

#define PARAM_ERROR 1
#define FILE_ERROR  2
#define QUANTITY_SLAVES 5
#define FILE_PERCENTAGE 20

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

  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return 1;


  int qtyFiles = argc - 1;
  printf("Found %d files to process", qtyFiles);

  int firstBatchFiles = qtyFiles * FILE_PERCENTAGE / 100.0;
  int fileNumber = 0;

  for (; fileNumber < firstBatchFiles; fileNumber++)
    sendFile(slaveContr, fileNumber % QUANTITY_SLAVES, argv[fileNumber + 1]);


  char md5Result[17] = {0};
  while (fileNumber < qtyFiles) {
    getAvailableMD5Result(slaveContr, md5Result);
    printf("Test: %s\n", md5Result);
    fileNumber++;
  }
  
  freeSlaveControllerADT(slaveContr);

  return 0;
}