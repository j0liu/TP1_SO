#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  char hola[100] = {0};
  scanf("%s", hola);
  perror(hola);
  return 0;
}