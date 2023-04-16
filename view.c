// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

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

  resultIOADT resultIO = createResultIOADT(pid, IO_READ | IO_SHM | IO_SEM, qtyFiles);
  if (resultIO == NULL) exit(EXIT_FAILURE);
  char buffer[ROW_LEN] = {0};
  int entryLen = 0;
  int received = 0;
  do {
    entryLen = readEntry(resultIO, buffer);
    if (entryLen > 0)  {
      buffer[entryLen-1]  = '\0';
      puts(buffer);
    }
  } while (entryLen > 0 && received++ < qtyFiles);
  freeResultIOADT(resultIO);
  return 0;
}