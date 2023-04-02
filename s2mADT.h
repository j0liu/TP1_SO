#ifndef S2MADT_H
#define S2MADT_H
typedef struct slaveToMasterCDT * slaveToMasterADT;

slaveToMasterADT createSlaveToMasterADT(int fdSMWrite, int fdMSRead);
void sendMD5Result(slaveToMasterADT s2m, char * md5result);
void readFileNames(slaveToMasterADT s2m, char ** filenames);

#endif //S2MADT_H