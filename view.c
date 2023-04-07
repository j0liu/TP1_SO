#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                   } while (0)

#define BUF_SIZE 1024   /* Maximum size for exchanged string */

/* Define a structure that will be imposed on the shared
  memory object */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *shmpath = argv[1];

  /* Create shared memory object and set its size to the size
     of our structure. */

  int fd = shm_open(shmpath, O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1)
    errExit("shm_open");

  if (ftruncate(fd, sizeof(struct shmbuf)) == -1)
    errExit("ftruncate");

  /* Map the object into the caller's address space. */

  struct shmbuf *shmp = mmap(NULL, sizeof(*shmp),
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED)
    errExit("mmap");

  /* Initialize semaphores as process-shared, with value 0. */

  if (sem_init(&shmp->sem1, 1, 0) == -1)
    errExit("sem_init-sem1");
  if (sem_init(&shmp->sem2, 1, 0) == -1)
    errExit("sem_init-sem2");

  /* Wait for 'sem1' to be posted by peer before touching
     shared memory. */

  if (sem_wait(&shmp->sem1) == -1)
    errExit("sem_wait");

  /* Convert data in shared memory into upper case. */

  for (int j = 0; j < shmp->cnt; j++)
    shmp->buf[j] = toupper((unsigned char)shmp->buf[j]);

  /* Post 'sem2' to tell the peer that it can now
     access the modified data in shared memory. */

  if (sem_post(&shmp->sem2) == -1)
    errExit("sem_post");

  /* Unlink the shared memory object. Even if the peer process
     is still using the object, this is okay. The object will
     be removed only after all open references are closed. */

  shm_unlink(shmpath);

  exit(0);
}