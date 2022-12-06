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

s_hash_table * create_table(int size){
    // Creates a new HashTable
    s_hash_table* table = (s_hash_table*) malloc (sizeof(s_hash_table));
    table->size = size;
    table->count = 0;
    table->itens = (s_info_endereco**) calloc (table->size, sizeof(s_info_endereco*));
    for (int i=0; i<table->size; i++)
        table->itens[i] = NULL;

    return table;
}
void free_item(s_info_endereco * item){
    // Frees an item
    free(item->key);
    free(item->num_usos);
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
void insert(s_hash_table * table, unsigned int key){
    // Create the item
    s_info_endereco* item = create_info(key);

    // Compute the index
    unsigned long index = hash_function(key);

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
            if (strcmp(current_item->key, key) == 0) {
                strcpy(table->itens[index]->num_usos, value);
                return;
            }
    
        else {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return;
        }
    }
}

void handle_collision(s_hash_table* table, unsigned long index, s_info_endereco* item) {
    LinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
 }

// Caso a chave já exista, apenas chama update
// Caso não exista, chama insert e então update
// Uma função auxiliar para procurar na hashtable pode ser necessária
void novo_uso(s_hash_table * table, unsigned int key, int linha_novo_uso);


void delete(s_hash_table * table, unsigned int key){
    // Deletes an item from the table
    int index = hash_function(key);
    Ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];

    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && strcmp(item->key, key) == 0) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->itens[index] = NULL;
            free_item(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (strcmp(item->key, key) == 0) {
                // Remove this item and set the head of the list
                // as the new item
                
                free_item(item);
                LinkedList* node = head;
                head = head->next;
                node->next = NULL;
                table->itens[index] = create_item(node->item->key, node->item->value);
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            LinkedList* curr = head;
            LinkedList* prev = NULL;
            
            while (curr) {
                if (strcmp(curr->item->key, key) == 0) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }

        }
    }
}
s_info_endereco * acessa_elemento(s_hash_table * table, unsigned int key);

s_hash_table * fill_table(FILE * f, int size){
  s_hash_table * table = create_table(size);
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
  if(tempo >= i->proximos_usos[i->num_usos]){
    delete(table, key);
  }
  i->num_usos += 1;
}

unsigned int algo_otimo(unsigned key, int tempo, s_hash_table * table, s_quadro * pages, int len){
  unsigned int key = pages[0].address;
  s_info_endereco * melhor_elemento = acessa_elemento(table, key);
  int melhor_valor = melhor_elemento->proximos_usos[melhor_elemento->num_usos];

  s_info_endereco * elemento_atual;
  int valor_atual;
   for(int i=1; i < len; i++){
    key = pages[i].address;
    elemento_atual = acessa_elemento(table, key);
    valor_atual = elemento_atual->proximos_usos[elemento_atual->num_usos];
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
void process_page(s_quadro * pages, unsigned int raw_address, unsigned int time, int page_size, int * page_fault, int * escrita, char mode, s_hash_table * table, int * len_lista, int max_len){
  unsigned int processed_address = get_logical(raw_address, page_size);
  int c = contains(pages, processed_address, *len_lista, info);

  if(c != -1){
    set_page(pages, c, mode, time, processed_address);  }
  else{
    if(*len_lista == max_len){
      *page_fault += 1;
      int id;
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

