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
  setvbuf(stdout, NULL, _IONBF, 0); 
  
  char *shmpath;
  char input[SHM_NAME_LENGTH]; 
  if (argc > 2) {
    fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
    exit(EXIT_FAILURE);
  } else if (argc == 2) {
    shmpath = argv[1];
  } else {
    scanf("%s", input);
    shmpath = input;
  }
  
  int fd = shm_open(shmpath, O_RDWR, 0);
  
  if (fd == -1)
    errExit("shm_open");

  //if (ftruncate(fd, sizeof(shmbuf)) == -1)
    //errExit("ftruncate");

  shmbuf *shmp = mmap(NULL, sizeof(shmbuf) + 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  if (shmp == MAP_FAILED)
    errExit("mmap shmbuf");

  if (ftruncate(fd, sizeof(shmbuf) + shmp->qtyFiles * ROW_LEN) == -1)
    errExit("ftruncate");

  int qtyFiles = shmp->qtyFiles;

  munmap(shmp, sizeof(shmbuf));

  shmp = mmap(NULL, sizeof(shmbuf) + qtyFiles * ROW_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED)
    errExit("mmap entryText");

  sem_t * sem = sem_open(shmp->semName, O_RDWR);

  char * entryText = (char *) shmp + sizeof(shmbuf);
  
  char * startPos = entryText;
  char * endPos;

  printf("Vista B)\n");
  do {
    sem_wait(sem);
    endPos = strchr(startPos, '\n');
    write(STDOUT_FILENO, startPos, endPos - startPos + 1);
    startPos = endPos + 1;
  } while (*startPos != EOF);

  return 0;
}