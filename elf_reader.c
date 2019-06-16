#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <elf.h>
#include "tool.h"


void set_sec_info(Elf64_Shdr *sec_header_table, Elf64_Ehdr *elf_header, char *buf, ELF_INFO *elf_info){
  int i;
  char *shstr_tab_addr;
  SEC_INFO *sec_info;

  sec_info = (SEC_INFO*)malloc(sizeof(SEC_INFO) * elf_header->e_shnum);
  shstr_tab_addr = &buf[(sec_header_table + elf_header->e_shstrndx)->sh_offset];    
  for(i = 0; i < elf_header->e_shnum; i++){
    strcpy((sec_info + i)->name, shstr_tab_addr + (sec_header_table + i)->sh_name);
    (sec_info + i)->addr = (sec_header_table + i)->sh_addr;
    (sec_info + i)->offset = (sec_header_table + i)->sh_offset;
    (sec_info + i)->size = (sec_header_table + i)->sh_size;
  }
  elf_info->sec_info = sec_info;
  elf_info->sec_tab_size = elf_header->e_shentsize * elf_header->e_shnum;
  elf_info->sec_num = elf_header->e_shnum;
}


void set_sym_info(Elf64_Shdr *sec_header_table, Elf64_Ehdr *elf_header, char *buf, ELF_INFO *elf_info){
  int i, j, k;
  int sym_num; 
  SYM_INFO *sym_info;
  Elf64_Sym *sym_tab_addr;
  char *str_tab_addr, *shstr_tab_addr;

  shstr_tab_addr = &buf[(sec_header_table + elf_header->e_shstrndx)->sh_offset];    
  for (i = 0; i < elf_header->e_shnum; i++){
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".strtab") == 0)j = i;    
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".symtab") == 0)k = i;    
  }
  
  str_tab_addr = &buf[(sec_header_table + j)->sh_offset];
  sym_tab_addr = (Elf64_Sym *)&buf[(sec_header_table + k)->sh_offset];

  sym_num = (sec_header_table + k)->sh_size / sizeof(Elf64_Sym);
  sym_info = (SYM_INFO*)malloc(sizeof(SYM_INFO) * sym_num);

  for (i = 0; i < sym_num; i++){
    strcpy((sym_info + i)->name, str_tab_addr + (sym_tab_addr + i)->st_name);
    (sym_info + i)->addr = (sym_tab_addr + i)->st_value;
    (sym_info + i)->info = (sym_tab_addr + i)->st_info;
    (sym_info + i)->shndx = (sym_tab_addr + i)->st_shndx;
    (sym_info + i)->size = (sym_tab_addr + i)->st_size;
  }
  elf_info->sym_info = sym_info;
  elf_info->sym_tab_size = (sec_header_table + k)->sh_size;
  elf_info->sym_num = sym_num;
}


void set_dynsym_info(Elf64_Shdr *sec_header_table, Elf64_Ehdr *elf_header, char *buf, ELF_INFO *elf_info){
  int i, j, k;
  int dynsym_num; 
  SYM_INFO *dynsym_info;
  Elf64_Sym *dynsym_tab_addr;
  char *dynstr_tab_addr, *shstr_tab_addr;

  shstr_tab_addr = &buf[(sec_header_table + elf_header->e_shstrndx)->sh_offset];    
  for (i = 0; i < elf_header->e_shnum; i++){
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".dynstr") == 0)j = i;    
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".dynsym") == 0)k = i;    
  }
  
  dynstr_tab_addr = &buf[(sec_header_table + j)->sh_offset];
  dynsym_tab_addr = (Elf64_Sym *)&buf[(sec_header_table + k)->sh_offset];

  dynsym_num = (sec_header_table + k)->sh_size / sizeof(Elf64_Sym);
  dynsym_info = (SYM_INFO*)malloc(sizeof(SYM_INFO) * dynsym_num);

  for (i = 0; i < dynsym_num; i++){
    strcpy((dynsym_info + i)->name, dynstr_tab_addr + (dynsym_tab_addr + i)->st_name);
    (dynsym_info + i)->addr = (dynsym_tab_addr + i)->st_value;
    (dynsym_info + i)->info = (dynsym_tab_addr + i)->st_info;
    (dynsym_info + i)->shndx = (dynsym_tab_addr + i)->st_shndx;
    (dynsym_info + i)->size = (dynsym_tab_addr + i)->st_size;
  }
  elf_info->dynsym_info = dynsym_info;
  elf_info->dynsym_tab_size = (sec_header_table + k)->sh_size;
  elf_info->dynsym_num = dynsym_num;
}


void set_rela_text_info(Elf64_Shdr *sec_header_table, Elf64_Ehdr *elf_header, char *buf, ELF_INFO *elf_info){
  int i, j, found=0;
  int rela_text_num; 
  RELA_INFO *rela_text_info;
  Elf64_Rela *rela_text_tab_addr;
  char *shstr_tab_addr;

  shstr_tab_addr = &buf[(sec_header_table + elf_header->e_shstrndx)->sh_offset];    
  for (i = 0; i < elf_header->e_shnum; i++){
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".rela.text") == 0){
      found = 1;
      j = i;
      break;
    }
  }
  if(found == 0)return;
  
  rela_text_tab_addr = (Elf64_Rela *)&buf[(sec_header_table + j)->sh_offset];

  rela_text_num = (sec_header_table + j)->sh_size / sizeof(Elf64_Rela);
  rela_text_info = (RELA_INFO*)malloc(sizeof(RELA_INFO) * rela_text_num);

  for (i = 0; i < rela_text_num; i++){
    (rela_text_info + i)->offset = (rela_text_tab_addr + i)->r_offset;
    (rela_text_info + i)->info = (rela_text_tab_addr + i)->r_info;
    (rela_text_info + i)->addend = (rela_text_tab_addr + i)->r_addend;    
  }
  elf_info->rela_text_info = rela_text_info;
  elf_info->rela_text_tab_size = (sec_header_table + j)->sh_size;
  elf_info->rela_text_num = rela_text_num;
}


void set_rela_plt_info(Elf64_Shdr *sec_header_table, Elf64_Ehdr *elf_header, char *buf, ELF_INFO *elf_info){
  int i, j, found=0;
  int rela_plt_num; 
  RELA_INFO *rela_plt_info;
  Elf64_Rela *rela_plt_tab_addr;
  char *shstr_tab_addr;

  shstr_tab_addr = &buf[(sec_header_table + elf_header->e_shstrndx)->sh_offset];    
  for (i = 0; i < elf_header->e_shnum; i++){
    if(strcmp(shstr_tab_addr + (sec_header_table + i)->sh_name, ".rela.plt") == 0){
      found = 1;
      j = i;
      break;
    }
  }
  if(found == 0)return;
  
  rela_plt_tab_addr = (Elf64_Rela *)&buf[(sec_header_table + j)->sh_offset];

  rela_plt_num = (sec_header_table + j)->sh_size / sizeof(Elf64_Rela);
  rela_plt_info = (RELA_INFO*)malloc(sizeof(RELA_INFO) * rela_plt_num);

  for (i = 0; i < rela_plt_num; i++){
    (rela_plt_info + i)->offset = (rela_plt_tab_addr + i)->r_offset;
    (rela_plt_info + i)->info = (rela_plt_tab_addr + i)->r_info;
    (rela_plt_info + i)->addend = (rela_plt_tab_addr + i)->r_addend;    
  }
  elf_info->rela_plt_info = rela_plt_info;
  elf_info->rela_plt_tab_size = (sec_header_table + j)->sh_size;
  elf_info->rela_plt_num = rela_plt_num;
}


int get_sec_addr(ELF_INFO *elf_info, char *secname, long int *addr){
  int i, found=0;

  for(i = 0; i < elf_info->sec_num; i++){
    if(strcmp(((elf_info->sec_info) + i)->name, secname)==0){
      *addr = ((elf_info->sec_info) + i)->addr;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}


int get_sec_offset(ELF_INFO *elf_info, char *secname, long int *offset){
  int i, found=0;

  for(i = 0; i < elf_info->sec_num; i++){
    if(strcmp(((elf_info->sec_info) + i)->name, secname)==0){
      *offset = ((elf_info->sec_info) + i)->offset;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}


int get_sec_size(ELF_INFO *elf_info, char *secname, int *size){
  int i, found=0;

  for(i = 0; i < elf_info->sec_num; i++){
    if(strcmp(((elf_info->sec_info) + i)->name, secname)==0){
      *size = ((elf_info->sec_info) + i)->size;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}



int get_sym_addr(ELF_INFO *elf_info, char *symname, long int *addr){
  int i, found=0;

  for(i = 0; i < elf_info->sym_num; i++){
    if(strcmp(((elf_info->sym_info) + i)->name, symname)==0){
      *addr = ((elf_info->sym_info) + i)->addr;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}


int get_sym_shndx(ELF_INFO *elf_info, char *symname, short int *shndx){
  int i, found=0;

  for(i = 0; i < elf_info->sym_num; i++){
    if(strcmp(((elf_info->sym_info) + i)->name, symname)==0){
      *shndx = ((elf_info->sym_info) + i)->shndx;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}



int get_sym_size(ELF_INFO *elf_info, char *symname, int *size){
  int i, found=0;

  for(i = 0; i < elf_info->sym_num; i++){
    if(strcmp(((elf_info->sym_info) + i)->name, symname)==0){
      *size = ((elf_info->sym_info) + i)->size;
      found = 1;
      break;
    }
  }
  if(found == 1)return 1;
  else return 0;
}


ELF_INFO *make_elf_info(char *filename){
    Elf64_Ehdr *elf_header;
    Elf64_Shdr *sec_header_table;
    ELF_INFO *elf_info;
    char *buf;
    int fd;
    FILE *fp;
    struct stat stbuf;

    fd = open(filename, O_RDONLY);
    fp = fdopen(fd, "rb");
    fstat(fd, &stbuf);
    buf = (char*)malloc(stbuf.st_size);
    fread(buf, 1, stbuf.st_size, fp);
    fclose(fp);

    elf_header = (Elf64_Ehdr *)buf; 
    sec_header_table = (Elf64_Shdr *)(&buf[elf_header->e_shoff]);
   
    elf_info = (ELF_INFO*)malloc(sizeof(ELF_INFO));
    set_sec_info(sec_header_table, elf_header, buf, elf_info);
    set_sym_info(sec_header_table, elf_header, buf, elf_info);
    set_dynsym_info(sec_header_table, elf_header, buf, elf_info);
    set_rela_text_info(sec_header_table, elf_header, buf, elf_info);
    set_rela_plt_info(sec_header_table, elf_header, buf, elf_info);
    
    return elf_info;
}
