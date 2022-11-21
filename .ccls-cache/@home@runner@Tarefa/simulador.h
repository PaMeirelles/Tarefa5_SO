struct{
  unsigned int address;
  unsigned char r;
  unsigned char m;
  unsigned int acesso;
}typedef s_quadro;

struct{
  int tier1;
  int tier2;
  int tier3;
  int tier4;
} typedef s_nru;

void access_page(s_quadro * pages, unsigned int id, char mode, unsigned int time);
int contains(s_quadro * lista, unsigned int address, int len_lista, s_nru * info_nru);
unsigned int get_size(unsigned int address);
unsigned int num_bytes(unsigned int size);
unsigned int get_logical(unsigned int address);
void process_page(s_quadro * pages, unsigned int raw_address, char mode, unsigned int time, int * len_lista, int max_len, s_nru * info);
void set_page(s_quadro * pages, unsigned int id, char mode, unsigned int time, unsigned int address);
void add_page(s_quadro * pages, unsigned int address, char mode, unsigned int time, int * len_lista);
int get_nru_index(s_nru * info);
void update_nru_info(s_nru * info, s_quadro page, int index);