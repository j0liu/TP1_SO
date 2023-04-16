// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#include <semaphore.h>
#include <fcntl.h>
#include "helpers.h"
#include "resultIOADT.h"
#include <errno.h>

#define SHM_FORMAT              "/shm_%d"
#define FILE_FORMAT             "./output/file_%d"
#define SEM_FORMAT              "/sem_%d"
#define panic(resultIO, msg)    do { \
                                    perror(msg);\
                                    freeResultIOADT(resultIO);\
                                    return NULL;\
                                } while (0)
#define ERROR_MSG_MALLOC_SHARED_MEMORY_IO       "malloc sharedMemoryIO"
#define ERROR_MSG_MALLOC_FILE_IO                "malloc fileIO"
#define ERROR_MSG_OPEN_IO                       "openIO"
#define ERROR_MSG_MALLOC_RESULTIO               "malloc resultIO"
#define ERROR_MSG_UNSUPPORTED_FILE_OPERATION    "Unsupported file operation\n"
#define ERROR_MSG_FTRUNCATE_SHARED_MEMORY_IO    "ftruncate sharedMemoryIO"
#define ERROR_MSG_MALLOC_SEM_PATH               "malloc semPath"
#define ERROR_MSG_SEM_OPEN                      "sem_open"
#define ERROR_MSG_MMAP                          "mmap resultIO"
#define ERROR_MSG_FTRUNCATE_FILE_IO             "ftruncate fileIO"
#define ERROR_MSG_SEM_POST                      "sem_post"
#define ERROR_MSG_SEM_WAIT                      "sem_wait"
#define ERROR_MSG_SEM_UNLINK                    "sem_unlink"
#define ERROR_MSG_SHM_UNLINK                    "shm_unlink"
#define ERROR_MSG_MUNMAP                        "munmap"



/**
 * @brief Abre el recurso del sistema donde se quiere trabajar (shared memory o file)
 * 
 * @param resultIO ADT de resultIO
 * @param pid ID del proceso master
 * @return int -1 si falla, 0 si no
 */

static int openIO(resultIOADT resultIO, int pid);
typedef struct resultIOCDT {
    int flags;                  // Flags del resultIO
    int fd;                     // File descriptor del resultIO
    int qtyFiles;               // Cantidad de archivos a procesar
    char * entryText;           // Puntero al inicio del texto
    char * entryTextPtr;        // Puntero a indice en el texto
    char * path;                // Path del resultIO
    char * semPath;             // Path del semaforo
    sem_t * sem;                // Semaforo para sincronizar lectura
} resultIOCDT;


static int openIO(resultIOADT resultIO, int pid) {
    char * msg, * format;
    if (IS_SHM(resultIO->flags)){
        msg = ERROR_MSG_MALLOC_SHARED_MEMORY_IO;
        format = SHM_FORMAT;
    } else {
        msg = ERROR_MSG_MALLOC_FILE_IO;
        format = FILE_FORMAT;
    }

    resultIO->path = safeMalloc(SHM_NAME_LENGTH, msg);
    if (resultIO->path == NULL) return -1; 
    sprintf(resultIO->path, format, pid);
    
    errno = 0;
    if (IS_READ(resultIO->flags)) {
        resultIO->fd = shm_open(resultIO->path, O_RDWR, 0);
    } else {
        resultIO->fd = IS_SHM(resultIO->flags) ? shm_open(resultIO->path, O_CREAT | O_RDWR, 0) : 
                                                 open(resultIO->path, O_CREAT | O_RDWR, 0);
    }

    if (resultIO->fd == -1) {
        perror(ERROR_MSG_OPEN_IO);
        return -1;
    }
    return 0;
}

resultIOADT createResultIOADT(int pid, int flags, int qtyFiles) {
    resultIOADT resultIO;
    resultIO = safeMalloc(sizeof(resultIOCDT), ERROR_MSG_MALLOC_RESULTIO);
    if (resultIO == NULL) return NULL;
    resultIO->flags = flags;
    resultIO->qtyFiles = qtyFiles;
    resultIO->fd = -1;
    resultIO->sem = NULL;
    resultIO->semPath = NULL;
    resultIO->entryText = NULL;
    resultIO->entryTextPtr = NULL;
    
    if (!IS_WRITE(flags) && !IS_SHM(flags)) { // La consigna no pide leer de un archivo
        fprintf(stderr, ERROR_MSG_UNSUPPORTED_FILE_OPERATION);
        freeResultIOADT(resultIO);
        return NULL;
    } else if (openIO(resultIO, pid) == -1) {
        freeResultIOADT(resultIO);
        return NULL;
    }      
    errno = 0;
    if (ftruncate(resultIO->fd, qtyFiles * ROW_LEN) == -1) 
        panic(resultIO, ERROR_MSG_FTRUNCATE_SHARED_MEMORY_IO);

    resultIO->entryText = (char *) mmap(NULL, qtyFiles * ROW_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, resultIO->fd, 0);
    if (resultIO->entryText == MAP_FAILED)
        panic(resultIO, ERROR_MSG_MMAP);
    resultIO->entryTextPtr = resultIO->entryText;

    if (SEM_ENABLED(flags)) {
        resultIO->semPath = safeMalloc(SHM_NAME_LENGTH, ERROR_MSG_MALLOC_SEM_PATH);
        if (resultIO->semPath == NULL) return NULL;
        sprintf(resultIO->semPath, SEM_FORMAT, pid);
        resultIO->sem = IS_WRITE(flags)? sem_open(resultIO->semPath, O_CREAT | O_RDWR, 0, 0) : 
                                         sem_open(resultIO->semPath, O_RDWR);
        if (resultIO->sem == SEM_FAILED)
            panic(resultIO, ERROR_MSG_SEM_OPEN);
    }

    return resultIO;
}

void freeResultIOADT(resultIOADT resultIO) {
    if (resultIO == NULL) return;

    if (IS_WRITE(resultIO->flags)) {
        if (SEM_ENABLED(resultIO->flags) && resultIO->sem != NULL){
            if (resultIO->entryTextPtr != NULL)
                *resultIO->entryTextPtr = EOF;
            errno = 0;
            if (sem_post(resultIO->sem) == -1) perror(ERROR_MSG_SEM_POST);
            errno = 0;
            if (sem_unlink(resultIO->semPath) == -1) perror(ERROR_MSG_SEM_UNLINK);
        }
        if(IS_SHM(resultIO->flags) && resultIO->entryText != NULL) {
            errno = 0;
            if (shm_unlink(resultIO->path) == -1) perror(ERROR_MSG_SHM_UNLINK);
        } else if (resultIO->entryText != NULL) {
            errno = 0;
            if (ftruncate(resultIO->fd, resultIO->entryTextPtr - resultIO->entryText) == -1)
                perror(ERROR_MSG_FTRUNCATE_FILE_IO);
        }
    } 

    if (resultIO->entryTextPtr != NULL) {
        errno = 0;
        if(munmap(resultIO->entryText, resultIO->qtyFiles * ROW_LEN) == -1)
            perror(ERROR_MSG_MUNMAP);
    }

    if (resultIO->fd != -1)
        close(resultIO->fd);

    free(resultIO->path);
    free(resultIO->semPath);
    free(resultIO);
}

size_t writeEntry(resultIOADT resultIO, const char * entry) {
    if (resultIO == NULL || resultIO->entryTextPtr == NULL) return -1;

    size_t entryLen = strlen(entry);
    memcpy(resultIO->entryTextPtr, entry, entryLen);
    resultIO->entryTextPtr += entryLen;
    *resultIO->entryTextPtr = '\n';
    resultIO->entryTextPtr++;

    if (SEM_ENABLED(resultIO->flags)) {
        errno = 0;
        if (sem_post(resultIO->sem) == -1) {
            perror(ERROR_MSG_SEM_POST);
            return -1;
        }
    }
    return entryLen; 
}

int readEntry(resultIOADT resultIO, char *buffer) {
    if (resultIO == NULL || resultIO->entryTextPtr == NULL) return -1;
    
    char * endPos;
    if(SEM_ENABLED(resultIO->flags)) {
        errno = 0;
        if (sem_wait(resultIO->sem) == -1) {
            perror(ERROR_MSG_SEM_WAIT);
            return -1;
        }
    }

    if ((int) *resultIO->entryTextPtr == EOF) return -1;
    endPos = strchr(resultIO->entryTextPtr, '\n');
    if (endPos == NULL) return -1;

    int entryLen = endPos - resultIO->entryTextPtr + 1;
    strncpy(buffer, resultIO->entryTextPtr, entryLen);
    resultIO->entryTextPtr = endPos + 1;
    return entryLen;
}