#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                   } while (0)

#define ROW_LEN 20

int main(int argc, char *argv[]) {
if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(1);
  }
  char *shmpath = "FLDSMDFR";

  /* Open the existing shared memory object and map it
    into the caller's address space. */

  int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0);
  if (fd == -1)
    errExit("shm_open");

  int qtyFiles = argc-1;

  struct shmbuf *shmp = mmap(NULL, sizeof(*shmp) + qtyFiles*ROW_LEN,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED)
    errExit("mmap");

  /* Copy data into the shared memory object. */
  char * buffer = shmp + sizeof(*shmp);

  shmp->qtyFiles = qtyFiles;
  shmp->entries = 0;

  char * buffAux = buffer;
  for (int i = 0; i < qtyFiles; i++) {
    char * filename = argv[i+1];
    int nameLen = strlen(filename);
    memcpy(buffAux, &nameLen, sizeof(int));
    buffAux += sizeof(int);
    memcpy(buffAux, filename, nameLen);
    buffAux = ROW_LEN*i;
  }
    
/*
  if (sem_post(&shmp->sem1) == -1)
    errExit("sem_post");

  if (sem_wait(&shmp->sem2) == -1)
    errExit("sem_wait");

*/

}