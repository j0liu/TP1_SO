/*
 *  Fecha: 17 de abril de 2023
 *  Autores: Liu, Jonathan Daniel
 *           Vilamowski, Abril
 *           Wischñevsky, David
 *  Version: 1.0
 */

#ifndef SLAVE_CONTROLLER_H
#define SLAVE_CONTROLLER_H

#include <stdlib.h>
#include <sys/types.h>

typedef struct slaveControllerCDT * slaveControllerADT;

/**
 * @brief  Crea un nuevo slaveControllerADT
 * @param  qtySlaves Cantidad de esclavos 
 * @return slaveControllerADT 
 */
slaveControllerADT createSlaveControllerADT(int qtySlaves);

/**
 * @brief Retorna la cantidad de archivos enviados 
 * 
 * @param slaveContr ADT del slaveController
 * @return int cantidad de archivos enviados
 */
int getFilesSent(slaveControllerADT slaveContr);

/**
 * @brief Retorna la cantidad de archivos recibidos
 * 
 * @param slaveContr ADT del slaveController
 * @return int cantidad de archivos recibidos
 */
int getFilesReceived(slaveControllerADT slaveContr);

/**
 * @brief Incrementa en uno la cantidad de archivos enviados 
 * 
 * @param slaveContr ADT del slaveController
 */
void incrementFilesSent(slaveControllerADT slaveContr);

/**
 * @brief Incrementa un uno la cantidad de archivos recibidos
 * 
 * @param slaveContr ADT del slaveController
 */
void incrementFilesReceived(slaveControllerADT slaveContr);

/**
 * @brief Retorna un el ultimo md5 procesado por un esclavo
 * @note El md5 debe ser liberado por el usuario
 * @param slaveContr ADT del slaveController
 * @param index Indice del esclavo al que se le pide el md5 
 * @return char* md5 obtenido
 */
char * getAvailableMD5Result(slaveControllerADT slaveContr, int * index);

/**
 * @brief Envia un archivo a un esclavo
 * 
 * @param slaveContr ADT del slaveController
 * @param index Index del eclavo
 * @param filename Archivo para procesar
 * @return ssize_t Cantidad de bytes del nombre del archivo enviado, -1 si fallo
 */
ssize_t sendFile(slaveControllerADT slaveContr, int index, char * filename);

/**
 * @brief En caso que un esclavo no tenga mas archivos por procesar, le envia uno
 * 
 * @param slaveContr ADT del slaveController
 * @param index Index del eclavo
 * @param filename Archivo para procesar
 * @return ssize_t Cantidad de bytes del nombre del archivo enviado, -1 si fallo
 */
ssize_t sendFileIfIdle(slaveControllerADT slaveContr, int index, char * filename);

/**
 * @brief Obtiene el index de un esclavo que no tiene mas archivos por procesar
 * 
 * @param slaveContr ADT del slaveController
 * @return int Index del esclavo
 */
int getIdleSlaveIndex(slaveControllerADT slaveContr);

/**
 * @brief Libera la memoria de un slaveControllerADT
 * 
 * @param slaveContr ADT del slaveController
 */
void freeSlaveControllerADT(slaveControllerADT slaveContr);

#endif