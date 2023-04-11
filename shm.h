#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <semaphore.h>

#define ROW_LEN 3000 // TODO: Poner bien y en otro lado
#define SHM_NAME_LENGTH 16
#define SEM_NAME_LENGTH 16

typedef struct shmbuf {
    char semName[SEM_NAME_LENGTH];
    size_t qtyFiles;
} shmbuf;

#endif

