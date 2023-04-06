#ifndef M2SADT_H
#define M2SADT_H
#include <stdlib.h>
#include <sys/types.h>

typedef struct masterToSlaveCDT * masterToSlaveADT;

masterToSlaveADT createMasterToSlaveADT();
ssize_t sendFileName(masterToSlaveADT m2s, char * filename);
char * readMD5Result(masterToSlaveADT m2s);
int isIdle(masterToSlaveADT m2s);
int getMSWriteFd(masterToSlaveADT m2s);
int getSMReadFd(masterToSlaveADT m2s);
void freeMasterToSlaveADT(masterToSlaveADT m2s);

#endif //M2SADT_H