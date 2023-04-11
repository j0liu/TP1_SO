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

#define SHM_NAME "/myshm"

int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0); 

  if (argc < 2) {
    fprintf(stderr, "Expected files as input\n");
    exit(1);
  }
  char shmpath[SHM_NAME_LENGTH];
  sprintf(shmpath, "%s%d", SHM_NAME, getpid());

  int fd = shm_open(shmpath, O_CREAT | O_RDWR, 0);
  if (fd == -1) errExit("shm_open");


  int qtyFiles = argc - 1;
  
  if (ftruncate(fd, sizeof(shmbuf) + qtyFiles * ROW_LEN) == -1) errExit("ftruncate");

  shmbuf *shmp = mmap(NULL, sizeof(shmbuf) + qtyFiles * ROW_LEN,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED)
    errExit("mmap");


  strcpy(shmp->semName, "/semfiles");
  sem_unlink(shmp->semName);
  sem_t * sem = sem_open(shmp->semName, O_CREAT | O_RDWR, 0, 0);

  /* Copy data into the shared memory object. */
  char *entryList = (char *) shmp + sizeof(shmbuf);

  shmp->qtyFiles = qtyFiles;

  printf("%s\n", shmpath);
  sleep(2);

  char *entryListPtr = entryList;
  for (int i = 0; i < qtyFiles; i++){
    char *filename = argv[i + 1];
    int nameLen = strlen(filename);
    memcpy(entryListPtr, filename, nameLen);
    entryListPtr += nameLen;
    *entryListPtr = '\n';
    entryListPtr++;
    sleep(1);

    if (i == qtyFiles - 1) *entryListPtr = EOF;
    sem_post(sem);
  }

  sem_unlink(shmp->semName);
  shm_unlink(shmpath);
  return 0;
}