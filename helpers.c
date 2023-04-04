#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

int fileExists (char * path) {
    struct stat sb;
    return !stat(path, &sb);
}

void * safeMalloc(size_t size, const char * msg){
    void * aux = malloc(size);
    if (aux == NULL) perror(msg);
    return aux;
}
