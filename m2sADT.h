#ifndef M2SADT_H
#define M2SADT_H
typedef struct masterToSlaveCDT * masterToSlaveADT;

masterToSlaveADT createMasterToSlaveADT();
void sendFileNames(masterToSlaveADT m2s, char * names[]);
void readMD5Result(masterToSlaveADT m2s, char * md5);
int isFree(masterToSlaveADT m2s);
void freeMasterToSlave(masterToSlaveADT m2s);
int getFdMSWrite(masterToSlaveADT m2s);
int getFdSMRead(masterToSlaveADT m2s);

#endif //M2SADT_H