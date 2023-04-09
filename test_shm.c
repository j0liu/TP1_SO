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


int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(1);
  }
  char *shmpath = "FLDSMDFR";

  int fd = shm_open(shmpath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

  if (fd == -1)
    errExit("shm_open");

  int qtyFiles = argc - 1;

  shmbuf *shmp = mmap(NULL, sizeof(shmbuf) + qtyFiles * ROW_LEN,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED)
    errExit("mmap");

  /* Copy data into the shared memory object. */
  char *buffer = (char *) shmp + sizeof(shmbuf);

  shmp->qtyFiles = qtyFiles;
  shmp->entries = 0;

  char *buffAux = buffer;
  for (; shmp->entries < qtyFiles; shmp->entries++){
    char *filename = argv[shmp->entries + 1];
    int nameLen = strlen(filename);
    memcpy(buffAux, filename, nameLen);
    buffAux += nameLen;
    (*buffAux) = '\n';
    buffAux++;
  }
  printf("%s\n",shmpath);
  // ------------------------------
  int xd;
  scanf("%d", &xd);
  /*
    if (sem_post(&shmp->sem1) == -1)
      errExit("sem_post");

    if (sem_wait(&shmp->sem2) == -1)
      errExit("sem_wait");

  */
  shm_unlink(shmpath);
  return 0;
}