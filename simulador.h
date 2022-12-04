struct{
  unsigned int address;
  unsigned char r;
  unsigned char m;
  unsigned int acesso;
}typedef s_quadro;

struct{
  int * tiers;
} typedef s_nru;

// Estrutura que vai na hash table
struct{
  int num_usos;
  int * proximos_usos;
  unsigned int key;
} typedef s_info_endereco;

struct{
    s_info_endereco ** itens;
    int size;
    int count;
}typedef s_hash_table;
void access_page(s_quadro * pages, unsigned int id, char mode, unsigned int time);
int contains(s_quadro * lista, unsigned int address, int len_lista, s_nru * info_nru);
unsigned int get_size(unsigned int address);
unsigned int get_logical(unsigned int address, int page_size);
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len, s_nru * info, int page_size, int * page_fault, int * escrita, int algo);
void set_page(s_quadro * pages, unsigned int id, char mode, unsigned int time, unsigned int address);
void add_page(s_quadro * pages, unsigned int address, char mode, unsigned int time, int * len_lista);
int get_nru_index(s_nru * info);
void update_nru_info(s_nru * info, s_quadro * pages, int len_pages);
int get_lru(s_quadro * pages, int len_pages);