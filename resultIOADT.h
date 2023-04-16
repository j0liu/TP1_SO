/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#ifndef RESULT_IO_H
#define RESULT_IO_H
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define IO_WRITE        1                   // Flag para escritura
#define IO_READ         0                   // Flag para lectura
#define IO_SHM          2                   // Flag para shared memory 
#define IO_FILE         0                   // Flag para archivo
#define IO_SEM          4                   // Flag para semaforo habilitado
#define IO_SEM_DIS      0                   // Flag para semaforo dehabilitado

#define SHM_NAME_LENGTH         32          // Longitud maxima del nombre de la shared memory 
#define SEM_NAME_LENGTH         32          // Longitud maxima del nombre del semaforo

#define IS_WRITE(x) ((x) & IO_WRITE)        // Verifica si el flag es de escritura
#define IS_READ(x) (!IS_WRITE(x))           // Verifica si el flag es de lectura
#define IS_SHM(x) ((x) & IO_SHM)            // Verifica si el flag es de shared memory
#define IS_FILE(x) (!IS_SHM(x))             // Verifica si el flag es de archivo  
#define SEM_ENABLED(x) ((x) & IO_SEM)       // Verifica si el flag es de semaforo habilitado
#define SEM_DISABLED(x) (!SEM_ENABLED(x))   // Verifica si el flag es de semaforo deshabilitado

typedef struct resultIOCDT * resultIOADT;
/**
 * @brief Crea un nuevo createResultIOADT 
 * 
 * @param pid Id del proceso master
 * @param flags Flags de IO
 * @param qtyFiles Cantidad de archivos a procesar
 * @return resultIOADT, o NULL si hubo un error
 */
resultIOADT createResultIOADT(int pid, int flags, int qtyFiles);

/**
 * @brief Liberar memoria del resultIOADT
 * 
 * @param resultIO ADT del resultIO
 */
void freeResultIOADT(resultIOADT resultIO);

/**
 * @brief Escribe una entrada (puede ser tanto en la shared memory como en un archivo)
 * 
 * @note si el flag IO_SEM esta encendido, se llama a sem_post despues de realizar la escritura
 * @param resultIO ADT del resultIO
 * @param entry Entrada compuesta por el md5, el nombre del archivo y pid
 * @return int Cantidad de caracteres que se lograron escribir
 */
size_t writeEntry(resultIOADT resultIO, const char *entry);

/**
 * @brief Lee una entrada (puede ser tanto de la shared memory como de un archivo)
 * 
 * @note si el flag IO_SEM esta encendido, se llama a sem_wait antes de realizar la lectura
 * @param resultIO ADT del resultIO
 * @param entry Entrada compuesta por el md5, el nombre del archivo y pid
 * @return int Cantidad de caracteres que se lograron leer
 */
int readEntry(resultIOADT resultIO, char *entry);

#endif // RESULT_IO_H
