#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "helpers.h"
#include "md5sumADT.h"
#include <math.h>

int main() {
  setvbuf(stdout, NULL, _IONBF, 0);
  char filename[MAX_PATH_LENGTH] = {0};

  md5sumADT md5sum = createMD5sumADT();
  while (scanf("%s", filename) != EOF) {
    sendFileName(md5sum, filename);
    int resultLen = strlen(filename) + MD5_LENGTH + 2; // Preguntar si esta bien
    char md5Result[resultLen];
    readMD5Result(md5sum, md5Result, resultLen);
    
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