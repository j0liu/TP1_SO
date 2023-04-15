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

#define PARAM_ERROR     1   // Codigo de error para cantidad de parametros incorrecta
#define FILE_ERROR      2   // Codigo de error para archivo invalido
#define QUANTITY_SLAVES 5   // Cantidad de procesos slave
#define FILE_PERCENTAGE 20  // Porcentaje de archivos que se envian en la carga inicial


/**
 * @brief Loop principal del programa. Se encarga de enviar los archivos a los slaves, y de recibir y procesar los resultados. 
 * 
 * @param qtyFiles Cantidad de archivos
 * @param files Vector de archivos 
 * @param slaveContr Controlador de procesos slaves
 */
static void mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr);

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
 * @param slaveContr Controlador de procesos slaves
 */
static void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr);

/**
 * @brief  Saltea los directorios de la lista de archivos 
 * 
 * @param files Vector de archivos
 * @param slaveContr Controlador de procesos slaves
 */
static void skipDir(char * files[], slaveControllerADT slaveContr);


int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 
  checkParams(argc, argv);
  char ** files = argv + 1; // Saltear el nombre del ejecutable
  slaveControllerADT slaveContr = createSlaveControllerADT(QUANTITY_SLAVES);
  if (slaveContr == NULL) return 1;

  int qtyFiles = argc - 1;

  mainLoop(qtyFiles, files, slaveContr);
  return 0;
}

static void distributeInitialLoad(int qtyFiles, char ** files, slaveControllerADT slaveContr) {
  int firstBatchFiles = qtyFiles * FILE_PERCENTAGE / 100.0;
  if (firstBatchFiles < QUANTITY_SLAVES)
    firstBatchFiles = QUANTITY_SLAVES < qtyFiles? QUANTITY_SLAVES : qtyFiles;
  for (; getFilesSent(slaveContr) < firstBatchFiles; incrementFilesSent(slaveContr)) {
    skipDir(files, slaveContr);
    sendFile(slaveContr, getFilesSent(slaveContr) % QUANTITY_SLAVES, files[getFilesSent(slaveContr)]);
  }
}

static void mainLoop(int qtyFiles, char ** files, slaveControllerADT slaveContr){
  int pid = getpid();
  resultIOADT resultShmIO = createResultIOADT(pid, IO_WRITE | IO_SHM | IO_SEM, qtyFiles);
  if (resultShmIO == NULL) exit(1);
  printf("%d %d\n", pid, qtyFiles);
  sleep(2);

  resultIOADT resultFileIO = createResultIOADT(pid, IO_WRITE | IO_FILE | IO_SEM_DIS, qtyFiles);
  if (resultFileIO == NULL) exit(1);

  distributeInitialLoad(qtyFiles, files, slaveContr);
  while (getFilesReceived(slaveContr) < qtyFiles) {
    int slaveIdx;
    char * md5Result = getAvailableMD5Result(slaveContr, &slaveIdx);
    if (md5Result != NULL && slaveIdx != -1) {
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
  freeSlaveControllerADT(slaveContr);
}

static void skipDir(char * files[], slaveControllerADT slaveContr){
  while (files[getFilesSent(slaveContr)] != NULL && isDir(files[getFilesSent(slaveContr)])) {
    incrementFilesReceived(slaveContr);
    incrementFilesSent(slaveContr);
  }
}

static void checkParams(int argc, char * argv[]){
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