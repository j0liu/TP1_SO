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
  
  char shmpath[SHM_NAME_LENGTH]; 
  char sempath[SEM_NAME_LENGTH]; 
  int pid = 0;
  int qtyFiles = 0;
  if (argc != 3 && argc != 1) {
    fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
    exit(EXIT_FAILURE);
  } else if (argc == 3) {
    pid = atoi(argv[1]);
    qtyFiles = atoi(argv[2]);
  } else {
    scanf("%d %d", &pid, &qtyFiles);
  }
  printf("recibio %d %d\n", pid, qtyFiles);
  sprintf(shmpath, "/%s_%d", SHM_NAME, pid);
  sprintf(sempath, "/%s_%d", SEM_NAME, pid);
  
  int fd = shm_open(shmpath, O_RDWR, 0);
  
  if (fd == -1)
    errExit("shm_open");

  //if (ftruncate(fd, sizeof(shmbuf)) == -1)
    //errExit("ftruncate");

  char * entryText = (char *) mmap(NULL, qtyFiles * ROW_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (entryText == MAP_FAILED)
    errExit("mmap entryText");

  sem_t * sem = sem_open(sempath, O_RDWR);
  
  char * startPos = entryText;
  char * endPos;

  printf("Vista B)\n");
  do {
    sem_wait(sem);
    if (*startPos == EOF) break;
    endPos = strchr(startPos, '\n');
    write(STDOUT_FILENO, startPos, endPos - startPos + 1);
    startPos = endPos + 1;
  } while (*startPos != EOF);

  return 0;
}