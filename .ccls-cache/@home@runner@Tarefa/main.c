#include <stdio.h>

struct{
  unsigned int address;
  unsigned char r;
  unsigned char w;
  unsigned int acesso;
}typedef s_quadro;

unsigned int get_size(unsigned int address){
  unsigned int size = 0;
  while(address > 1){
    size++;
    address /= 10;
  }
  return size;
}


unsigned int num_bytes(unsigned int size){
  unsigned int i = 0;
  while(size > 1){
    i++;
    size /= 2;
  }
  return 10 + i;
}

unsigned int get_logical(unsigned int address){
  unsigned int size = num_bytes(get_size(address));
  return address >> size;
}

int main(void) {
  unsigned int ad = 0x0044e4f8;
  printf("%d\n", get_logical(ad));
  return 0;
}