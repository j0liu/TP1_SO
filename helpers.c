#include <sys/stat.h>

int file_exists (char * path) {
    struct stat sb;
    return !stat(path, &sb);
}