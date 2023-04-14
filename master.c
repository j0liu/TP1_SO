#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "slave-controller.h"
#include "resultIOADT.h"

#define PARAM_ERROR 1
#define FILE_ERROR  2
#define QUANTITY_SLAVES 5
#define FILE_PERCENTAGE 20

void skipDir(char * files[], slaveControllerADT slaveContr);
void checkParams(int argc, char * argv[]);
void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr);
void mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr);


int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 
  checkParams(argc, argv);
  char ** files = argv + 1;
  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return 1;

  int qtyFiles = argc - 1;

  mainLoop(qtyFiles, files, slaveContr);
  freeSlaveControllerADT(slaveContr);
  return 0;
}

void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr) {
  int firstBatchFiles = qtyFiles * FILE_PERCENTAGE / 100.0;
  if (firstBatchFiles < QUANTITY_SLAVES)
    firstBatchFiles = QUANTITY_SLAVES < qtyFiles? QUANTITY_SLAVES : qtyFiles;
  // printf("Found %d files to process, first batch %d\n", qtyFiles, firstBatchFiles);
  for (; getFilesSent(slaveContr) < firstBatchFiles; incrementFilesSent(slaveContr)) {
    skipDir(files, slaveContr);
    sendFile(slaveContr, getFilesSent(slaveContr) % QUANTITY_SLAVES, files[getFilesSent(slaveContr)]);
  }
}

void mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr){
  int pid = getpid();
  resultIOADT resultShmIO = createResultIOADT(pid, IO_WRITE | IO_SHM | IO_SEM, qtyFiles);
  if (resultShmIO == NULL) exit(1);
  printf("%d %d\n", pid, qtyFiles);
  sleep(10);

  resultIOADT resultFileIO = createResultIOADT(pid, IO_WRITE | IO_FILE | IO_SEM_DIS, qtyFiles);
  if (resultFileIO == NULL) exit(1);

  distributeInitialLoad(qtyFiles, files, slaveContr);
  while (getFilesReceived(slaveContr) < qtyFiles) {
    int slaveIdx;
    char * md5Result = getAvailableMD5Result(slaveContr, &slaveIdx);
    if (md5Result != NULL && slaveIdx != -1) {
      //fprintf(stderr, "master: %s\n", md5Result);
      if (writeEntry(resultShmIO, md5Result) == -1)
        exit(1);
      if (writeEntry(resultFileIO, md5Result) == -1)
        exit(1);

      incrementFilesReceived(slaveContr);
      free(md5Result);
      if (getFilesSent(slaveContr) < qtyFiles) {
        skipDir(files, slaveContr);
        if (files[getFilesSent(slaveContr)] != NULL && sendFileIfIdle(slaveContr, slaveIdx, files[getFilesSent(slaveContr)]) != -1)
          incrementFilesSent(slaveContr);
      }
    } else {
      perror("getAvailableMD5Result");
      break;
    } 
  }

  freeResultIOADT(resultShmIO);
  freeResultIOADT(resultFileIO);
}

void skipDir(char * files[], slaveControllerADT slaveContr){
  while (files[getFilesSent(slaveContr)] != NULL && isDir(files[getFilesSent(slaveContr)])) {
    incrementFilesReceived(slaveContr);
    incrementFilesSent(slaveContr);
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