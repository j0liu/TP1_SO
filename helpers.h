/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#define PIPE_READ           0
#define PIPE_WRITE          1
#define MAX_PATH_LENGTH     4096                                                  // 4KB es el maximo que puede tener un path en linux
#define MD5_LENGTH          32                                                    // Longitud del md5
#define MAX_PID_DIGITS      5                                                     // Cantidad maxima de digitos del pid
#define ROW_LEN             (MAX_PATH_LENGTH + MD5_LENGTH + MAX_PID_DIGITS + 4)   // Longitud maxima de una linea de un archivo

/**
 * @brief Determina si un archivo existe o no
 * 
 * @param filename Nombre del archivo
 * @return int 1 si existe, 0 sino
 */
int fileExists(const char *filename);

/**
 * @brief Determina si un path corresponde a un archivo o no
 * @note Limpia errno 
 * @param filename Path
 * @return int int 1 si es un archivo, 0 si no, -1 si hubo un error
 */
int isFile(const char *filename);

/**
 * @brief Crea un espacio de memoria en el heap, mostrando un mensaje de error en caso de que no se pueda 
 * @note Limpia errno 
 * @param size Cantidad de bytes que se quiere reservar de memoria en heap
 * @param msg Mensaje en caso de error
 * @return void* Puntero a la memoria reservada, o NULL en caso de error
 */
void * safeMalloc(size_t size, const char * msg);

/**
 * @brief Arma un pipe y devuelve los file descriptors de sus extremos
 * @note Limpia errno 
 * @param pipeFds Array de 2 enteros donde se guardaran los file descriptors 
 * @return int 0 si se creo correctamente, -1 si hubo un error
 */
int createPipe(int pipeFds[]);


#endif // HELPERS_H
