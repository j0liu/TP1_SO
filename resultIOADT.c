#include <semaphore.h>
#include <fcntl.h>
#include "helpers.h"
#include "resultIOADT.h"

#define SHM_NAME_LENGTH         16
#define SEM_NAME_LENGTH         16
#define ROW_LEN                 3000 // TODO: Poner bien 
#define SHM_PREFIX              "shm"
#define FILE_PREFIX             "file"
#define SEM_PREFIX              "sem"
#define panic(resultIO, msg)    do { \
                                    perror(msg);\
                                    freeResultIOADT(resultIO);\
                                    return NULL;\
                                } while (0)


typedef struct resultIOCDT {
    int flags;
    int fd;
    int qtyFiles;
    char * entryText;
    char * entryTextPtr;
    char * path;
    char * semPath;
    sem_t * sem;
} resultIOCDT;


static int createIO(resultIOADT resultIO, int pid) {
    char * msg, * prefix;
    if (IS_SHM(resultIO->flags)){
        msg = "malloc sharedMemoryIO";
        prefix = SHM_PREFIX;
    } else {
        msg = "malloc fileIO";
        prefix = FILE_PREFIX;
    }

    resultIO->path = safeMalloc(SHM_NAME_LENGTH, msg);
    sprintf(resultIO->path, "/%s_%d", prefix, pid);
    
    resultIO->fd = IS_SHM(resultIO->flags) ? shm_open(resultIO->path, O_CREAT | O_RDWR, 0) :
                                             open(resultIO->path, O_CREAT | O_RDWR, 0);
    if (resultIO->fd == -1) {
        perror(msg);
        return -1;
    }
    return 0;
}

static void openIO(resultIOADT resultIO){

}


resultIOADT createResultIOADT(int pid, int flags, int qtyFiles) {
    resultIOADT resultIO;
    resultIO = safeMalloc(sizeof(resultIOCDT), "malloc resultIO");
    if (resultIO == NULL) return NULL;
    resultIO->flags = flags;
    resultIO->qtyFiles = qtyFiles;
    resultIO->fd = -1;
    resultIO->sem = NULL;
    resultIO->semPath = NULL;
    resultIO->entryText = NULL;
    resultIO->entryTextPtr = NULL;
    
    if (IS_WRITE(flags)) createIO(resultIO, pid);
    else {
        if (IS_SHM(flags)) openIO(resultIO);
        else {
            fprintf(stderr, "Unsupported file operation");
            freeResultIOADT(resultIO);
            return NULL;
        }
    }

    if (ftruncate(resultIO->fd, qtyFiles * ROW_LEN) == -1) 
        panic(resultIO, "ftruncate resultIO");

    resultIO->entryText = (char *) mmap(NULL, qtyFiles * ROW_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, resultIO->fd, 0);
    resultIO->entryTextPtr = resultIO->entryText;
    if (resultIO->entryText == MAP_FAILED)
        panic(resultIO, "mmap resultIO");

    if (SEM_ENABLED(flags)) {
        resultIO->semPath = safeMalloc(SHM_NAME_LENGTH, "malloc semPath");
        if (resultIO->semPath == NULL) return NULL;
        sprintf(resultIO->semPath, "/%s_%d", SEM_PREFIX, pid);
        resultIO->sem = IS_WRITE(flags)? sem_open(resultIO->semPath, O_CREAT | O_RDWR, 0, 0) : 
                                         sem_open(resultIO->semPath, O_RDWR);
        if (resultIO->sem == SEM_FAILED)
            panic(resultIO, "sem");
    }

    return resultIO;
}

void freeResultIOADT(resultIOADT resultIO) {
    
    if (IS_WRITE(resultIO->flags)) {
        if (SEM_ENABLED(resultIO->flags) && resultIO->sem != NULL){
            if (resultIO->entryTextPtr != NULL)
                *resultIO->entryTextPtr = EOF;
            sem_post(resultIO->sem);
            sem_unlink(resultIO->semPath);
        }
        if(IS_SHM(resultIO->flags) && resultIO->entryText != NULL)
            shm_unlink(resultIO->path);
    }

    if (resultIO->entryTextPtr != NULL)
        munmap(resultIO->entryText, resultIO->qtyFiles * ROW_LEN);

    free(resultIO->path);
    free(resultIO->semPath);
    free(resultIO);
}

int writeEntry(resultIOADT resultIO, const char * entry) {
    int entryLen = strlen(entry);
    memcpy(resultIO->entryTextPtr, entry, entryLen);
    resultIO->entryTextPtr += entryLen;
    *resultIO->entryTextPtr = '\n';
    resultIO->entryTextPtr++;
    if(SEM_ENABLED(resultIO->flags))
        sem_post(resultIO->sem);
    return 0; // TODO: Handle errors
}

int readEntry(resultIOADT resultIO, char *result) {
    return 0;
}