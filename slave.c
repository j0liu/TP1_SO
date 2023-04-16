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
#include <unistd.h>
#include <string.h>
#include "helpers.h"
#include "md5sumADT.h"

int main() {
  setvbuf(stdout, NULL, _IONBF, 0);
  char filename[MAX_PATH_LENGTH] = {0};

  md5sumADT md5sum = createMD5sumADT();
  if (md5sum == NULL) return 1;

  while (scanf("%s", filename) != EOF) {
    if (sendFileName(md5sum, filename) == -1) break;
    int resultLen = strlen(filename) + MD5_LENGTH + 3;  // 3 = 2 espacios + 1 \0 
    char md5Result[resultLen];
    ssize_t readLen = readMD5Result(md5sum, md5Result, resultLen - 1); // -1 para no contar el \0
    if (readLen <= 1) break;
    
    int pid = getpid();
    int finalResultLen = resultLen + MAX_PID_DIGITS;
    char finalResult[finalResultLen];
    int len = sprintf(finalResult, "%s %d", md5Result, pid) + 1;
    write(STDOUT_FILENO, &len, sizeof(int));
    write(STDOUT_FILENO, finalResult, len);
  }
  freeMD5sumADT(md5sum);
  return 0;
}