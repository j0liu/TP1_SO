// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "slave-controller.h"
#include "resultIOADT.h"

#define START_SLEEP_TIME          2    // Cantidad de segundos que la aplicacion espera antes de comenzar a enviar archivos
#define QUANTITY_SLAVES           10   // Cantidad de procesos slave
#define FILE_PERCENTAGE           20   // Porcentaje de archivos que se envian en la carga inicial
#define ERROR_MSG_FILE_EXISTS     "file_exists"
#define ERROR_MSG_EXPECTED_FILES  "Expected files as input\n"
#define ERROR_MSG_INVALID_FILE    "Invalid file %s\n"
#define ERROR_MSG_GET_AVAILABLE_MD5_RESULT "getAvailableMD5Result"
#define ERROR_MSG_COULD_NOT_GET_MD5_RESULT "Error, could not get available MD5 result"

/**
 * @brief Loop principal del programa. Se encarga de enviar los archivos a los slaves, y de recibir y procesar los resultados. 
 * 
 * @param qtyFiles Cantidad de archivos
 * @param files Vector de archivos 
 * @param slaveContr ADT del slaveController
 * @return int 0 si no hubo errores, 1 en caso contrario
 */
static int mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr, resultIOADT resultShmIO, resultIOADT resultFileIO);

/**
 * @brief Chequea si la cantidad de paramentros es correcta. En caso de ser menor a 2 aborta el programa.
 * 
 * @param argc Cantidad de parametros
 * @param argv En el primer argumento, el nombre del ejecutable. El resto, los archivos que recibe.
 */

static void checkParams(int argc, char * argv[]);
/**
 * @brief Distribuye la carga de trabajo inicial como porcentaje de la cantidad de archivos, y se las asigna a los slaves
 * 
 * @note utiliza la constante FILE_PERCENTAGE para calcular el porcentaje de la carga inicial
 * @param qtyFiles Cantidad total de archivos
 * @param files Vector de archivos
 * @param slaveContr ADT del slaveController
 */
static void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr);

/**
 * @brief  Saltea los directorios de la lista de archivos 
 * 
 * @param files Vector de archivos
 * @param slaveContr ADT del slaveController
 */
static void skipDir(char * files[], slaveControllerADT slaveContr);

/**
 * @brief Libera todos los ADT 
 * 
 * @param slaveContr ADT del slaveController
 * @param resultShmIO ADT para manejo de shared memory 
 * @param resultFileIO ADT para manejo del archivo resultado

 */
static void freeAll(slaveControllerADT slaveContr, resultIOADT resultShmIO, resultIOADT resultFileIO);

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 
  checkParams(argc, argv);
  char ** files = argv + 1; // Saltear el nombre del ejecutable
  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return EXIT_FAILURE;

  int qtyFiles = argc - 1;
  int pid = getpid();
  resultIOADT resultShmIO = createResultIOADT(pid, IO_WRITE | IO_SHM | IO_SEM, qtyFiles);
  if (resultShmIO == NULL) {
    freeSlaveControllerADT(slaveContr);
    exit(EXIT_FAILURE);
  } 
  printf("%d %d\n", pid, qtyFiles);
  sleep(START_SLEEP_TIME);

  resultIOADT resultFileIO = createResultIOADT(pid, IO_WRITE | IO_FILE | IO_SEM_DIS, qtyFiles);
  if (resultFileIO == NULL) {
    freeAll(slaveContr, resultShmIO, NULL);
    exit(EXIT_FAILURE);
  } 

  distributeInitialLoad(qtyFiles, files, slaveContr);
  int result = mainLoop(qtyFiles, files, slaveContr, resultShmIO, resultFileIO);

  freeAll(slaveContr, resultShmIO, resultFileIO);
  return result;
}

static void freeAll(slaveControllerADT slaveContr, resultIOADT resultShmIO, resultIOADT resultFileIO) {
  freeResultIOADT(resultShmIO);
  freeResultIOADT(resultFileIO);
  freeSlaveControllerADT(slaveContr);
}

static void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr) {
  int firstBatchFiles = qtyFiles * FILE_PERCENTAGE / 100.0;
  if (firstBatchFiles < QUANTITY_SLAVES)
    firstBatchFiles = QUANTITY_SLAVES < qtyFiles? QUANTITY_SLAVES : qtyFiles;

  for (; getFilesSent(slaveContr) < firstBatchFiles; incrementFilesSent(slaveContr)) {
    skipDir(files, slaveContr);
    if(files[getFilesSent(slaveContr)] != NULL)
      sendFile(slaveContr, getFilesSent(slaveContr) % QUANTITY_SLAVES, files[getFilesSent(slaveContr)]);
  }
}

static int mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr, resultIOADT resultShmIO, resultIOADT resultFileIO) {
  while (getFilesReceived(slaveContr) < qtyFiles) {
    int slaveIdx;
    char * md5Result = getAvailableMD5Result(slaveContr, &slaveIdx);
    if (md5Result != NULL && slaveIdx != -1) {
      if (writeEntry(resultShmIO, md5Result) == -1) return EXIT_FAILURE; 
      if (writeEntry(resultFileIO, md5Result) == -1) return EXIT_FAILURE; 

      incrementFilesReceived(slaveContr);
      free(md5Result);
      if (getFilesSent(slaveContr) < qtyFiles) {
        skipDir(files, slaveContr);
        if (files[getFilesSent(slaveContr)] != NULL && sendFileIfIdle(slaveContr, slaveIdx, files[getFilesSent(slaveContr)]) != -1)
          incrementFilesSent(slaveContr);
      }
    } else {
      perror(ERROR_MSG_GET_AVAILABLE_MD5_RESULT);
      fprintf(stderr, ERROR_MSG_COULD_NOT_GET_MD5_RESULT);
      return EXIT_FAILURE;
    } 
  }
  return EXIT_SUCCESS;
}

static void skipDir(char * files[], slaveControllerADT slaveContr){
  while (files[getFilesSent(slaveContr)] != NULL && !isFile(files[getFilesSent(slaveContr)])) {
    incrementFilesReceived(slaveContr);
    incrementFilesSent(slaveContr);
  }
}

static void checkParams(int argc, char * argv[]){
  if (argc < 2) {
    fprintf(stderr, ERROR_MSG_EXPECTED_FILES);
    exit(EXIT_FAILURE);
  }
  
  for (int i = 1; i < argc; i++){
    if (!fileExists(argv[i])) {
      perror(ERROR_MSG_FILE_EXISTS);
      fprintf(stderr, ERROR_MSG_INVALID_FILE, argv[i]);
      exit(EXIT_FAILURE);
    }
  }
}