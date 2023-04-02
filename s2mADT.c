#include "s2mADT.h"

typedef struct slaveToMasterCDT{
    int fdSMWrite;
    int fdMSRead;
} slaveToMasterCDT;

slaveToMasterADT createSlaveToMasterADT(int fdSMWrite, int fdMSRead);
void sendMD5Result(slaveToMasterADT s2m, char * md5result);
void readFileNames(slaveToMasterADT s2m, char ** filenames);