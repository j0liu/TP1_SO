/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#ifndef MD5SUM_ADT_H
#define MD5SUM_ADT_H
#include <stdlib.h>

typedef struct md5sumCDT * md5sumADT;

/**
 * @brief Crea un nuevo md5sumADT
 * 
 * @return md5sumADT, o NULL si hubo un error
 */
md5sumADT createMD5sumADT();

/**
 * @brief Crea un proceso md5sum y lo conecta con el pipe
 * @note Limpia errno 
 * @param md5sum ADT del md5
 * @param filename Nombre del archivo a calcular el md5
 * @return int -1 si hubo un error, pid del proceso generado con fork si no 
 */
int sendFileName(md5sumADT md5sum, char * filename);

/**
 * @brief Lee un md5 del archivo proveniente del pipe 
 * @note Limpia errno 
 * @param md5sum ADT del md5
 * @param md5 Buffer donde se guardara el md5 junto con el nombre del archivo
 * @param resultLen Longitud del md5 + nombre del archivo
 * @return ssize_t Cantidad de bytes que se pudo leer, -1 si fallo
 */
ssize_t readMD5Result(md5sumADT md5sum, char * md5, int resultLen);

/**
 * @brief Libera el ADT de md5
 * 
 * @param md5sum ADT del md5
 */
void freeMD5sumADT(md5sumADT md5sum);

#endif //MD5SUMADT_H