#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"

#define table_size 100

s_info_endereco * create_info(unsigned int key) {
    s_info_endereco * info = malloc (sizeof(s_info_endereco));
    info->key = key;
    info->ultimo_uso = 0;
    return info;
}
void handle_collision(s_hash_table * table, unsigned int key, s_info_endereco * item){};
s_hash_table * create_table(){
    // Creates a new HashTable
    s_hash_table* table = (s_hash_table*) malloc (sizeof(s_hash_table));
    table->size = table_size;
    table->count = 0;
    table->itens = (s_info_endereco**) calloc (table->size, sizeof(s_info_endereco*));
    for (int i=0; i<table->size; i++)
        table->itens[i] = NULL;
    return table;
}
void free_item(s_info_endereco * item){
    // Frees an item
    free(item);
}
void free_table(s_hash_table * table){
    // Frees the table
    for (int i=0; i<table->size; i++) {
        s_info_endereco* item = table->itens[i];
        if (item != NULL)
            free_item(item);
    }

    free(table->itens);
    free(table);
}
// Caso haja colisões, precisam ser tratadas
// Uma função auxiliar para isso pode ser necessária
void insert(s_hash_table * table, unsigned int key, int time){
    // Create the item
    s_info_endereco* item = create_info(key);

    // Compute the index
    unsigned long index = hash_function(key);
    //printf("%x %ld %d\n", key, index, time);
   s_info_endereco* current_item = table->itens[index];
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }
        
        // Insert directly
        table->itens[index] = item; 
        table->count++;
    }

    else {

            // Scenario 1: We only need to update value
            if (current_item->key == key) {
                current_item->ultimo_uso = time;
                return;
            }
    
        else {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return;
        }
    }
  
}

// Caso a chave já exista, apenas chama update
// Caso não exista, chama insert e então update
// Uma função auxiliar para procurar na hashtable pode ser necessária
void delete(s_hash_table * table, unsigned int key){
table->count -= 1;
free_item(table->itens[hash_function(key)]);
table->itens[hash_function(key)] = NULL;
};
unsigned int hash_function(unsigned int index){
  return index % table_size;
}

s_info_endereco * acessa_elemento(s_hash_table * table, unsigned int key){
    int index = hash_function(key);
    s_info_endereco * item = table->itens[index];
    // Ensure that we move to a non NULL item
    if (item != NULL) {
        if (item->key == key){
            return item;
        }
    }
    return NULL;
}

s_hash_table * fill_table(FILE * f){
  s_hash_table * table = create_table(table_size);
  int id;
  char mode;
  int i = 0;
  
  while(fscanf(f, "%x %c", &id, &mode) == 2){
    insert(table, id, i);
    i++;
  }
  
  return table;
}

void trata_pagina(unsigned int key, int tempo, s_hash_table * table){
  s_info_endereco * i = acessa_elemento(table, key);
  if(tempo >= i->ultimo_uso){
    delete(table, key);
  }
}

unsigned int algo_otimo(int tempo, s_hash_table * table, s_quadro * pages, int len){
  unsigned int key = pages[0].address;
  s_info_endereco * melhor_elemento = acessa_elemento(table, key);
  int melhor_valor = melhor_elemento->ultimo_uso;

  s_info_endereco * elemento_atual;
  int valor_atual;
   for(int i=1; i < len; i++){
    key = pages[i].address;
    elemento_atual = acessa_elemento(table, key);
    valor_atual = elemento_atual->ultimo_uso;
    if(valor_atual > melhor_valor){
      melhor_valor = valor_atual;
      melhor_elemento = elemento_atual;
    }
  }
  return melhor_elemento->key;
}
// Não referenciada, não modificada = id0
// Não referenciada, modificada = id1
// Referenciada, não modificada = id2
// Referenciada, modificada = id 3
unsigned int num_bytes(unsigned int size){
  unsigned int i = 0;
  while(size > 1){
    i++;
    size /= 2;
  }
  return 10 + i;
}
int contains(s_quadro * lista, unsigned int address, int len_lista){
  for(int i=0; i < len_lista; i++){
    if(lista[i].address == address){
      return i;
    }
  }
  return -1;
}
unsigned int get_logical(unsigned int address, int page_size){
  return address >> page_size;
}
void process_page(s_quadro * pages, unsigned int raw_address, unsigned int time, int page_size, int * page_fault, int * escrita, char mode, s_hash_table * table, int * len_lista, int max_len){
  unsigned int processed_address = get_logical(raw_address, page_size);
  int c = contains(pages, processed_address, *len_lista);

  if(c != -1){
    set_page(pages, c, mode, time, processed_address);  
    trata_pagina(c, time, table);
    }
  else{
    if(*len_lista == max_len){
      *page_fault += 1;
      unsigned int id = algo_otimo(time, table, pages, *len_lista);
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
  pages[id].acesso = time;
  if(mode == 'W'){
    pages[id].m = 1;
  }
  else{
    pages[id].r = 1;
  }
  pages[id].address = address;
}
void processa(FILE * f, int page_size, int memmory_size){
  int tempo = 0;
  int len_lista = 0;
  int page_fault = 0;
  int escrita = 0;
  
  s_quadro * pages = malloc(sizeof(s_quadro) * memmory_size / page_size);
  
  s_hash_table * table = fill_table(f); 
  int id;
  char mode;
  while(fscanf(f, "%x %c", &id, &mode) == 2){
    process_page(pages, id, tempo, page_size, &page_fault, &escrita, mode, table, &len_lista, memmory_size / page_size);
    tempo++;
  }
  printf("Número de faltas de página: %d\n", page_fault);
  printf("Número de Páginas escritas: %d\n", escrita);
  free(pages);
  free_table(table);
  
}
int main(int argc, char * argv[]) {
  int pag, mem;
  if(argc != 4){
    printf("3 argumentos são necessários\n");
    exit(-1);
  }
  FILE * f = fopen(argv[1], "r");
  if(f == NULL){
    printf("Arquivo não encontrado\n");
    exit(-3);
  }
  
  pag = atoi(argv[2]);
  if(pag < 8 || pag > 32){
    printf("Tamanho da página deve estar entre 8kB e 32 kB\n");
    exit(-4);
  }
  
  mem = atoi(argv[3]) * 1000;
  if(mem < 1000 || mem > 16000){
    printf("Tamanho da memória deve estar entre 1mB e 16mB\n");
    exit(-5);
  }
  printf("Executando o simulador...\n");
  printf("Arquivo de entrada: %s\n", argv[1]);
  printf("Tamanho da memória física: %dmB\n", mem / 1000);
  printf("Tamanho da página: %dkB\n", pag);
  processa(f, pag, mem);
  free(f);
return 0;
}

