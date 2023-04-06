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
  while (scanf("%s", filename) != EOF) { 
    sendFileName(md5sum, filename);
    int resultLen = strlen(filename) + MD5_LENGTH + 2; // Preguntar si esta bien
    char md5Result[resultLen];
    readMD5Result(md5sum, md5Result, resultLen);
    printf("%s\n", md5Result);
  }
  freeMD5sumADT(md5sum);
  return 0;
}