#include <semaphore.h>
#include <fcntl.h>
#include "helpers.h"
#include "resultIOADT.h"

#define SHM_FORMAT              "/shm_%d"
#define FILE_FORMAT             "./output/file_%d"
#define SEM_FORMAT              "/sem_%d"
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
    char * msg, * format;
    if (IS_SHM(resultIO->flags)){
        msg = "malloc sharedMemoryIO";
        format = SHM_FORMAT;
    } else {
        msg = "malloc fileIO";
        format = FILE_FORMAT;
    }

    resultIO->path = safeMalloc(SHM_NAME_LENGTH, msg);
    if (resultIO->path == NULL) exit(1);
    sprintf(resultIO->path, format, pid);
    
    if (IS_READ(resultIO->flags)) {
        resultIO->fd = shm_open(resultIO->path, O_RDWR, 0);
    } else {
        resultIO->fd = IS_SHM(resultIO->flags) ? shm_open(resultIO->path, O_CREAT | O_RDWR, 0) : 
                                                 open(resultIO->path, O_CREAT | O_RDWR, 0);
    }

    if (resultIO->fd == -1) {
        perror("createIO");
        return -1;
    }
    return 0;
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
    
    if (!IS_WRITE(flags) && !IS_SHM(flags)) {
        fprintf(stderr, "Unsupported file operation");
        freeResultIOADT(resultIO);
        return NULL;
    } else {
        if (createIO(resultIO, pid) == -1) {
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
        sprintf(resultIO->semPath, SEM_FORMAT, pid);
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
        else {
            if (ftruncate(resultIO->fd, resultIO->entryTextPtr - resultIO->entryText) == -1)
                perror("ftruncate freeResultIOADT");
        }
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

int readEntry(resultIOADT resultIO, char *buffer) {
    char * endPos;
    sem_wait(resultIO->sem);
    if (*resultIO->entryTextPtr == EOF) return -1;
    endPos = strchr(resultIO->entryTextPtr, '\n');
    int entryLen = endPos - resultIO->entryTextPtr + 1;
    strncpy(buffer, resultIO->entryTextPtr, entryLen);
    resultIO->entryTextPtr = endPos + 1;
    return entryLen;
}