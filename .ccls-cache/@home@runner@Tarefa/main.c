#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"


void update_nru_info(s_nru * info, s_quadro page, int index){
  if(page.r == 1){
    if(page.m == 1){
      if(info->tier4 == -1){
        info->tier4 = index;
      }
    }
    else{
      if(info->tier3 == -1){
        info->tier3 = index;
      }
    }
  }
  else{
    if(page.m == 1){
      if(info->tier2 == -1){
        info->tier2 = index;
      }
    }
    else{
      if(info->tier1 == -1){
        info->tier1 = index;
      }
    }
  }
}

int get_nru_index(s_nru * info){
  if(info->tier1 != -1){
    info->tier1 = -1;
    return info->tier1;
  }
  if(info->tier2 != -1){
    info->tier2 = -1;
    return info->tier2;
  }
  if(info->tier3 != -1){
    info->tier3 = -1;
    return info->tier3;
  }
  if(info->tier4 != -1){
    info->tier4 = -1;
    return info->tier4;
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
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len, s_nru * info){
  unsigned int processed_address = get_logical(raw_address);
  int c = contains(pages, processed_address, *len_lista, info);

  if(c != -1){
    set_page(pages, c, mode, time, processed_address);
  }
  else{
    if(*len_lista == max_len){
      int i = get_nru_index(info);
      set_page(pages, i, mode, time, processed_address);
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

