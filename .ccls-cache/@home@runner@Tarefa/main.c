#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"

int contains(s_quadro * lista, unsigned int address, int len_lista){
  for(int i=0; i < len_lista; i++){
    if(lista[i].address == address){
      return i;
    }
  }
  return -1;
}
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
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len){
  unsigned int processed_address = get_logical(raw_address);
  int c = contains(pages, processed_address, *len_lista);

  if(c != -1){
    set_page(pages, c, mode, time);
  }
  else{
    if(*len_lista == max_len){
      // Substituição
    }
    else{
      add_page(pages, processed_address, mode, time, len_lista);
    }
  }
}
void add_page(s_quadro * pages, unsigned int address, char mode, unsigned int time, int * len_lista){
  s_quadro * page = malloc(sizeof(s_quadro));
  page->address = address;
  page->r = 0;
  page->m = 0;
  page->acesso = time;
  pages[*len_lista] = *page;

  *len_lista += 1;
}
void set_page(s_quadro * pages, unsigned int id, char mode, unsigned int time, unsigned int address){
  pages[id].r = 0;
  pages[id].acesso = time;
  if(mode == 'W'){
    pages[id].m = 1;
  }
  pages[id].address = address;
}

int main(void) {
  unsigned int ad = 0x0044e4f8;
  printf("%d\n", get_logical(ad));
  return 0;
}