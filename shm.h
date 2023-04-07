#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <semaphore.h>
typedef struct shmbuf {
    sem_t  sem;
    size_t qtyFiles;
    size_t entries; 
} shmbuf;

#endif

