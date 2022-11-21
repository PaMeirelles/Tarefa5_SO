#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"


void update_nru_info(s_nru * info, s_quadro page, int index){
  if(page.r == 1){
    if(page.m == 1){
      if(info->tiers[3] == -1){
        info->tiers[3] = index;
      }
    }
    else{
      if(info->tiers[2] == -1){
        info->tiers[2] = index;
      }
    }
  }
  else{
    if(page.m == 1){
      if(info->tiers[1] == -1){
        info->tiers[1] = index;
      }
    }
    else{
      if(info->tiers[0] == -1){
        info->tiers[0] = index;
      }
    }
  }
}

int get_nru_index(s_nru * info){
  for(int i=0; i < 4; i ++){
    if(info->tiers[i] != -1){
      return i;
    }
  }
  return -1;
}
int contains(s_quadro * lista, unsigned int address, int len_lista, s_nru * info_nru){
  for(int i=0; i < len_lista; i++){
    update_nru_info(info_nru, lista[i], i);
    if(lista[i].address == address){
      return i;
    }
  }
  return -1;
}

unsigned int num_bytes(unsigned int size){
  unsigned int i = 0;
  while(size > 1){
    i++;
    size /= 2;
  }
  return 10 + i;
}
unsigned int get_logical(unsigned int address, int page_size){
  return address >> page_size;
}
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len, s_nru * info, int page_size){
  unsigned int processed_address = get_logical(raw_address, page_size);
  int c = contains(pages, processed_address, *len_lista, info);

  if(c != -1){
    set_page(pages, c, mode, time, processed_address);
  }
  else{
    if(*len_lista == max_len){
      int i = get_nru_index(info);
      int id = info->tiers[i];
      set_page(pages, id, mode, time, processed_address);
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

s_nru * get_nru(){
  s_nru * info = malloc(sizeof(s_nru));
  info->tiers = malloc(sizeof(int) * 4);
  for(int i=0; i < 4; i++){
    info->tiers[i] = -1;
  }
  return info;
}

int main(void) {
  FILE * f = fopen("matriz.log", "r");
  int page_size = 8;
  int memmory_size = 16;
  int tempo = 0;
  int len_lista = 0;
  s_quadro * pages = malloc(sizeof(s_quadro) * memmory_size * 1000);
  s_nru * info = get_nru();
  int id;
  char mode;
  while(fscanf(f, "%x %c", &id, &mode) == 2){
    process_page(pages, id, mode, tempo, &len_lista, memmory_size * 1000, info, page_size);
    tempo++;
  }
  
  return 0;
}

