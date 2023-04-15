/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#ifndef M2SADT_H
#define M2SADT_H
#include <stdlib.h>
#include <sys/types.h>

typedef struct masterToSlaveCDT * masterToSlaveADT;


/**
 * @brief Crea un nuevo masterToSlaveADT
 *
 * @return masterToSlaveADT 
 */
masterToSlaveADT createMasterToSlaveADT();

/**
 * @brief 
 * 
 * @param m2s ADT del masterToSlave
 * @param filename Nombre del archivo a enviar
 * @return ssize_t Cantidad de bytes del archivo que se envio
 */
ssize_t sendFileName(masterToSlaveADT m2s, char * filename);

/**
 * @brief Lee un md5 del pipe
 * 
 * @param m2s ADT del masterToSlave
 * @return char* md5
 */
char * readMD5Result(masterToSlaveADT m2s);

/**
 * @brief Devuelve 1 si el slave esta ocupado, 0 si esta libre
 * 
 * @param m2s ADT del masterToSlave
 * @return int 1 si esta ocupado, 0 si esta libre
 */
int isIdle(masterToSlaveADT m2s);

/**
 * @brief Devuelve el file descriptor de lectura del pipe
 * 
 * @param m2s ADT del masterToSlave
 * @return int file descriptor
 */
int getSMReadFd(masterToSlaveADT m2s);

/**
 * @brief Libera la memoria de un masterToSlaveADT
 *
 * @param m2s ADT del masterToSlave
 */
void freeMasterToSlaveADT(masterToSlaveADT m2s);

#endif //M2SADT_H