#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#define PARAM_ERROR 1
#define FILE_ERROR  2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(PARAM_ERROR);
  }
  
  for (int i = 1; i < argc; i++){
    if (!file_exists(argv[i])) {
      perror("file_exists");
      fprintf(stderr, "Invalid file %s\n", argv[i]);
      exit(FILE_ERROR);
    }
  }
  
  

  return 0;
}