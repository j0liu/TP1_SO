#include <sys/stat.h>

int fileExists (char * path) {
    struct stat sb;
    return !stat(path, &sb);
}