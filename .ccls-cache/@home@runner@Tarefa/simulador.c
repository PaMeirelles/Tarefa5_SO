#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"

s_info_endereco * create_info(unsigned int key) {
    s_info_endereco * info = malloc (sizeof(s_info_endereco));
    info->key = key;
    info->num_usos = 0;
    // 8 é um número arbitrário, se ultrapassar realizo realloc
    info->proximos_usos = malloc(sizeof(int) * 8);
    return info;
}

s_hash_table * create_table(int size);
void free_item(s_info_endereco * item);
void free_table(s_hash_table * table);

// Caso haja colisões, precisam ser tratadas
// Uma função auxiliar para isso pode ser necessária
void insert(s_hash_table * table, unsigned int key);
void update(s_hash_table * table, unsigned int key, int linha_novo_uso);

// Caso a chave já exista, apenas chama update
// Caso não exista, chama insert e então update
// Uma função auxiliar para procurar na hashtable pode ser necessária
void novo_uso(s_hash_table * table, unsigned int key, int linha_novo_uso);
void delete(s_hash_table * table, unsigned int key);


s_hash_table * fill_table(FILE * f){
  s_hash_table * table = create_table(10000);
  int id;
  char mode;
  int i = 0;
  while(fscanf(f, "%x %c", &id, &mode) == 2){
    update(table, id, i);
    i++;
  }
  return table;
}
// Não referenciada, não modificada = id0
// Não referenciada, modificada = id1
// Referenciada, não modificada = id2
// Referenciada, modificada = id 3
void update_nru_info(s_nru * info, s_quadro * pages, int len_pages){
  s_quadro page;
  for(int i=0; i < len_pages; i++){
    page = pages[i];
      if(page.r == 1){
    if(page.m == 1){
      if(info->tiers[3] == -1){
        info->tiers[3] = i;
      }
    }
    else{
      if(info->tiers[2] == -1){
        info->tiers[2] = i;
      }
    }
  }
  else{
    if(page.m == 1){
      if(info->tiers[1] == -1){
        info->tiers[1] = i;
      }
    }
    else{
      if(info->tiers[0] == -1){
        info->tiers[0] = i;
      }
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
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len, s_nru * info, int page_size, int * page_fault, int * escrita, int algo){
  unsigned int processed_address = get_logical(raw_address, page_size);
  int c = contains(pages, processed_address, *len_lista, info);

  if(c != -1){
    set_page(pages, c, mode, time, processed_address);  }
  else{
    if(*len_lista == max_len){
      *page_fault += 1;
      int id;
      if(algo == 0){
        update_nru_info(info, pages, *len_lista);
      int i = get_nru_index(info);
      if(i == 3 || i == 1){
        *escrita += 1;
      }
              id = info->tiers[i];

      }
      else{
        id = get_lru(pages, *len_lista);
        if(pages[id].m){
          *escrita += 1;
        }
      }
      
      set_page(pages, id, mode, time, processed_address);
      info->tiers[id] = -1;
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
  pages[id].acesso = time;
  if(mode == 'W'){
    pages[id].m = 1;
  }
  else{
    pages[id].r = 1;
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
int get_lru(s_quadro * pages, int len_pages){
  int id = 0;
  int menor = pages[0].acesso;
  for(int i=1; i < len_pages; i++){
    if(pages[i].acesso < menor){
      menor = pages[i].acesso;
      id = i;
    }
  }
  return id;
}
void processa(FILE * f, int page_size, int memmory_size, int algo){
  int tempo = 0;
  int len_lista = 0;
  int page_fault = 0;
  int escrita = 0;
  
  s_quadro * pages = malloc(sizeof(s_quadro) * memmory_size / page_size);
  s_nru * info = get_nru();
  
  int id;
  char mode;
  while(fscanf(f, "%x %c", &id, &mode) == 2){
    process_page(pages, id, mode, tempo, &len_lista, memmory_size / page_size, info, page_size, &page_fault, &escrita, algo);
    tempo++;
  }
  printf("Número de faltas de página: %d\n", page_fault);
  printf("Número de Páginas escritas: %d\n", escrita);
  free(pages);
}
int main(int argc, char * argv[]) {
  int algo, pag, mem;
  if(argc != 5){
    printf("4 argumentos são necessários\n");
    exit(-1);
  }
  if(strcmp(argv[1], "NRU") == 0){
    algo = 0;
  }
  else if(strcmp(argv[1], "LRU") == 0){
    algo = 1;
  }
  else{
    printf("Algoritmo inválido. Apenas NRU e LRU são parâmetros aceitos");
    exit(-2);
  }
  FILE * f = fopen(argv[2], "r");
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(-3);
  }
  
  pag = atoi(argv[3]);
  if(pag < 8 || pag > 32){
    printf("Tamanho da página deve estar entre 8kB e 32 kB\n");
    exit(-4);
  }
  
  mem = atoi(argv[4]) * 1000;
  if(mem < 1000 || mem > 16000){
    printf("Tamanho da memória deve estar entre 1mB e 16mB\n");
    exit(-5);
  }
  printf("Executando o simulador...\n");
  printf("Arquivo de entrada: %s\n", argv[2]);
  printf("Tamanho da memória física: %dmB\n", mem / 1000);
  printf("Tamanho da página: %dkB\n", pag);
  printf("Algoritmo de substituição utilizado: %s\n", argv[1]);
  processa(f, pag, mem, algo);
  free(f);
return 0;
}

