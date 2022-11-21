#include <stdio.h>

struct{
  unsigned int address;
  unsigned char r;
  unsigned char w;
  unsigned int acesso;
}typedef s_quadro;


unsigned int num_bytes(unsigned int size){
  unsigned int i = 0;
  while(size > 1){
    i++;
    size /= 2;
  }
  return 10 + i;
}


int main(void) {
  printf("%d\n", num_bytes(8));
  printf("Hello World\n");
  return 0;
}