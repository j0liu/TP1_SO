#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                   } while (0)

#define BUF_SIZE 1024   


int main(int argc, char *argv[]) {
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *shmpath = argv[1];

  int fd = shm_open(shmpath, O_RDWR, 0);
  if (fd == -1)
    errExit("shm_open");

  if (ftruncate(fd, sizeof(shmbuf) + 1000) == -1)
    errExit("ftruncate");

  shmbuf *shmp = mmap(NULL, sizeof(shmbuf) + 1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED)
    errExit("mmap shmbuf");

  printf("qtyFiles: %d\n", shmp->qtyFiles);

  //if (ftruncate(fd, sizeof(shmbuf) + shmp->qtyFiles * ROW_LEN) == -1)
    //errExit("ftruncate");
  
  //off_t offset = 10;
  //char * entryText = (char *) mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
  //shmp = mmap(NULL, sizeof(shmbuf) + shmp->qtyFiles * ROW_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED)
    errExit("mmap entryText");

  char * entryText = (char *) shmp + sizeof(shmbuf);
  
  /*for (int i = 0; entryText[i] != EOF; i++) {
    putchar(entryText[i]);
  }*/

  char * startPos = entryText;
  char * endPos;
  do {
    endPos = strchr(startPos, '\n');
    write(STDOUT_FILENO, startPos, endPos - startPos + 1);
    startPos = endPos + 1;
  } while (*startPos != EOF);

  //shm_unlink(shmpath);

}