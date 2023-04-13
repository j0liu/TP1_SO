#ifndef RESULT_IO_H
#define RESULT_IO_H
#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define IO_WRITE        1
#define IO_READ         0
#define IO_SHM          2
#define IO_FILE         0
#define IO_SEM          4
#define IO_SEM_DIS      0

#define IS_WRITE(x) ((x) & IO_WRITE)
#define IS_READ(x) (!CAN_WRITE(x))
#define IS_SHM(x) ((x) & IO_SHM)
#define IS_FILE(x) (!IS_SHM(x))
#define SEM_ENABLED(x) ((x) & IO_SEM)
#define SEM_DISABLED(x) (!SEM_ENABLED(x))

typedef struct resultIOCDT * resultIOADT;

resultIOADT createResultIOADT(int pid, int flags, int qtyFiles);
void freeResultIOADT(resultIOADT resultIO);
int writeEntry(resultIOADT resultIO, const char *entry);
int readEntry(resultIOADT resultIO, char *entry);

#endif // RESULT_IO_H
