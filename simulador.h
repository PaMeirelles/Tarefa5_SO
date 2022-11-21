struct{
  unsigned int address;
  unsigned char r;
  unsigned char m;
  unsigned int acesso;
}typedef s_quadro;



void access_page(s_quadro * pages, unsigned int id, char mode, unsigned int time);
int contains(s_quadro * lista, unsigned int address, int len_lista);
unsigned int get_size(unsigned int address);
unsigned int num_bytes(unsigned int size);
unsigned int get_logical(unsigned int address);
void process_page(s_quadro * pages, unsigned int address, char mode, unsigned int time, int * len_lista, int max_len);
void set_page(s_quadro * pages, unsigned int id, char mode, unsigned int time, unsigned int address);
void add_page(s_quadro * pages, unsigned int address, char mode, unsigned int time, int * len_lista);