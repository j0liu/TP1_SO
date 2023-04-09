#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <semaphore.h>

#define ROW_LEN 3000 // TODO: Poner bien y en otro lado

typedef struct shmbuf {
    sem_t  sem;
    size_t qtyFiles;
    size_t entries; 
} shmbuf;

#endif

