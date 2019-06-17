#include <unistd.h>

typedef struct{
  char name[64];
  long int addr;
  long int offset;
  int size;
}SEC_INFO;

typedef struct{
  char name[64];
  long int addr;
  char info;
  short int shndx;
  int size;
}SYM_INFO;

typedef struct{
  long int offset;
  long int info;
  long int addend;
}RELA_INFO;

typedef struct{
  SEC_INFO *sec_info;
  int sec_tab_size;
  int sec_num;
  SYM_INFO *sym_info;
  int sym_tab_size;
  int sym_num;
  SYM_INFO *dynsym_info;
  int dynsym_tab_size;
  int dynsym_num;  
  RELA_INFO *rela_text_info;
  int rela_text_tab_size;
  int rela_text_num;
  RELA_INFO *rela_plt_info;
  int rela_plt_tab_size;
  int rela_plt_num;
}ELF_INFO;

typedef struct{
  int size;
  int num;
  int exist;
  char **symname;
}PLT_GOT_INFO;


//read_writer
int set_data(pid_t,long int,void*,int); 
int get_data(pid_t,long int,void*,int);
void set_jump(pid_t,long int,long int); 

//mem_manager
long int alloc_mmap_area(pid_t,int);

//elf_reader                                                                   
int get_sec_addr(ELF_INFO*,char*,long int*);                                
int get_sec_offset(ELF_INFO*,char*,long int*);                              
int get_sec_size(ELF_INFO*,char*,int*);                                
int get_sym_addr(ELF_INFO*,char*,long int*);                               
int get_sym_shndx(ELF_INFO*,char*,short int*);                              
int get_sym_size(ELF_INFO*,char*,int*);                                     
ELF_INFO *make_elf_info(char*); 

//linker
PLT_GOT_INFO *make_plt_info(ELF_INFO*);
PLT_GOT_INFO *make_got_info(ELF_INFO*);
void relocate(ELF_INFO*,ELF_INFO*,char*,int,int,int,int,PLT_GOT_INFO*,PLT_GOT_INFO*); 
char *make_patch_obj(ELF_INFO*,char*,ELF_INFO*,int*); 

