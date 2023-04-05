#ifndef MD5SUM_ADT_H
#define MD5SUM_ADT_H
#include <stdlib.h>

typedef struct md5sumCDT * md5sumADT;

md5sumADT createMD5sumADT();
int sendFileName(md5sumADT md5sum, char * filename);
ssize_t readMD5Result(md5sumADT md5sum, char * md5, int resultLen);
void freeMD5sumADT(md5sumADT md5sum);

#endif //MD5SUMADT_H