#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "slave-controller.h"

#define PARAM_ERROR 1
#define FILE_ERROR  2
#define QUANTITY_SLAVES 5
#define FILE_PERCENTAGE 20

void skipDir(char * files[], int * filesSent, int * filesReceived);
void checkParams(int argc, char * argv[]);


int main(int argc, char *argv[]) {
  checkParams(argc, argv);
  char ** files = argv + 1;
  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return 1;

  int qtyFiles = argc - 1;

  int firstBatchFiles = qtyFiles * FILE_PERCENTAGE / 100.0;
  if (firstBatchFiles < QUANTITY_SLAVES)
    firstBatchFiles = QUANTITY_SLAVES < qtyFiles? QUANTITY_SLAVES : qtyFiles;

  int filesSent = 0;
  int filesReceived = 0;
  printf("Found %d files to process, first batch %d\n", qtyFiles, firstBatchFiles);
  for (; filesSent < firstBatchFiles; filesSent++) {
    skipDir(files, &filesSent, &filesReceived);
    sendFile(slaveContr, filesSent % QUANTITY_SLAVES, files[filesSent]);
  }

  while (filesReceived < qtyFiles) {
    int slaveIdx;
    char * md5Result = getAvailableMD5Result(slaveContr, &slaveIdx);
    if (md5Result != NULL && slaveIdx != -1) {
      printf("Test %d: %s", filesReceived + 1, md5Result);
      filesReceived++;
      free(md5Result);
      //mandar resultado al view

      if (filesSent < qtyFiles) {
        skipDir(files, &filesSent, &filesReceived);
        if (files[filesSent] != NULL && sendFileIfIdle(slaveContr, slaveIdx, files[filesSent]) != -1)
          filesSent++;
      }
    } else {
      perror("getAvailableMD5Result");
      break;
    } 
  }

  freeSlaveControllerADT(slaveContr);
  return 0;
}

void skipDir(char * files[], int * filesSent, int * filesReceived){
  while (files[*filesSent] != NULL && isDir(files[*filesSent])) {
    (*filesSent)++;
    (*filesReceived)++;
  }
}


void checkParams(int argc, char * argv[]){
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
}