#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <elf.h>
#include "tool.h"


PLT_GOT_INFO *make_plt_info(ELF_INFO *patch_elf_info){
  int i, j, k=0;
  int sym1, type1, sym2, type2, num=0, found=0;
  short int shndx;
  PLT_GOT_INFO *plt_info;
  char **symname;
  
  plt_info = (PLT_GOT_INFO*)malloc(sizeof(PLT_GOT_INFO));
  symname = (char**)malloc(sizeof(char*) * patch_elf_info->rela_text_num);
  
  for(i = 0; i < patch_elf_info->rela_text_num; i++){
    sym1 = ELF64_R_SYM(((patch_elf_info->rela_text_info) + i)->info);
    type1 = ELF64_R_TYPE(((patch_elf_info->rela_text_info) + i)->info);
    
    if(type1 == R_X86_64_PLT32){
      for(j = 0; j < i; j++){
        sym2 = ELF64_R_SYM(((patch_elf_info->rela_text_info) + j)->info);
        type2 = ELF64_R_TYPE(((patch_elf_info->rela_text_info) + j)->info);
        if((sym2 == sym1) && (type2 == R_X86_64_PLT32)){
           found = 1;
           break;
        }
      }
      if(found == 0){
        num++;
        symname[k] = ((patch_elf_info->sym_info) + sym1)->name;
        k++;
      }
      else found = 0;
    } 
  }
  plt_info->size = num * 6;
  plt_info->num = num;
  if(num > 0)plt_info->exist = 1;
  else plt_info->exist = 0;
  plt_info->symname = symname;
  
  return plt_info;
}


PLT_GOT_INFO *make_got_info(ELF_INFO *patch_elf_info){
  int i, j, k=0;
  int sym1, type1, sym2, type2, num=0, found=0;
  short int shndx;
  PLT_GOT_INFO *got_info;
  char **symname;

  got_info = (PLT_GOT_INFO*)malloc(sizeof(PLT_GOT_INFO));
  symname = (char**)malloc(sizeof(char*) * patch_elf_info->rela_text_num);  
 
  for(i = 0; i < patch_elf_info->rela_text_num; i++){
    sym1 = ELF64_R_SYM(((patch_elf_info->rela_text_info) + i)->info);
    type1 = ELF64_R_TYPE(((patch_elf_info->rela_text_info) + i)->info);
    
    if(type1 == R_X86_64_PLT32 || type1 == 0x2a){      //0x2a : R_X86_64_REX_GOTP
      for(j = 0; j < i; j++){
        sym2 = ELF64_R_SYM(((patch_elf_info->rela_text_info) + j)->info);
        type2 = ELF64_R_TYPE(((patch_elf_info->rela_text_info) + j)->info);
        if((sym2 == sym1) && (type2 == R_X86_64_PLT32 || type2 == 0x2a)){
          found = 1;
          break;
        }
      }
      if(found == 0){
        num++;
        symname[k] = ((patch_elf_info->sym_info) + sym1)->name;
        k++;
      }
      else found = 0;
    } 
  } 
  got_info->size = num * 8;
  got_info->num = num;
  if(num > 0)got_info->exist = 1;
  else got_info->exist = 0;
  got_info->symname = symname;
  
  return got_info;

}


void relocate(ELF_INFO *target_elf_info, ELF_INFO *patch_elf_info, char *buf, int text_index, int rodata_index, int plt_index, int got_index, PLT_GOT_INFO *plt_info, PLT_GOT_INFO *got_info){
  int i, j, operand;
  long int offset, sym, type, addend;
  short int shndx;
  char *name;
  long int target_sym_addr;
  short int target_sym_shndx;
  char jmp_cord[6] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00};

  /// text relocation ///
  for(i = 0; i < patch_elf_info->rela_text_num; i++){
    offset = ((patch_elf_info->rela_text_info) + i)->offset;
    sym = ELF64_R_SYM(((patch_elf_info->rela_text_info) + i)->info);
    type = ELF64_R_TYPE(((patch_elf_info->rela_text_info) + i)->info);
    addend = ((patch_elf_info->rela_text_info) + i)->addend;
    
    if(ELF64_ST_TYPE(((patch_elf_info->sym_info) + sym)->info) == STT_SECTION){
      shndx = ((patch_elf_info->sym_info) + sym)->shndx;
      name = ((patch_elf_info->sec_info) + shndx)->name;
    }
    else{
      name = ((patch_elf_info->sym_info) + sym)->name;
    }
    
    if(type == R_X86_64_PC32){
      if(strcmp(name, ".rodata") == 0){
        operand = rodata_index - (text_index + offset) + addend;
        memcpy(&buf[text_index + offset], &operand, 4);
      }
    }
    if(type == R_X86_64_PLT32){
      for(j = 0; j < plt_info->num; j++){
        if(strcmp(name, plt_info->symname[j]) == 0)break;
      }
      operand = (plt_index + 6 * j) - (text_index + offset) + addend;
      memcpy(&buf[text_index + offset], &operand, 4);
    }
    if(type == 0x2a){  // 0x2a : R_X86_64_REX_GOTP 
      for(j = 0; j < got_info->num; j++){
        if(strcmp(name, got_info->symname[j]) == 0)break;
      }
      operand = (got_index + 8 * j) - (text_index + offset) + addend;
      memcpy(&buf[text_index + offset], &operand, 4);
    }     
  }

  
  /// plt relocation ///
  for(i = 0; i < plt_info->num; i++){
    for(j = 0; j < got_info->num; j++){
      if(strcmp(plt_info->symname[i], got_info->symname[j]) == 0)break;
    }
    operand = (got_index + 8 * j) - (plt_index + 6 * i) - 6;
    memcpy(&jmp_cord[2], &operand, 4);
    memcpy(&buf[plt_index + 6 * i], jmp_cord, 6);
  }

  
  /// got relocation ///
  for(i = 0; i < got_info->num; i++){
    if(get_sym_addr(target_elf_info, got_info->symname[i], &target_sym_addr)){
      memcpy(&buf[got_index + 8 * i], &target_sym_addr, 8);
    }
  }
  
}


char *make_patch_obj(ELF_INFO *target_elf_info, char *patch_filename, ELF_INFO *patch_elf_info, int *size){
  long int text_offset, rodata_offset;
  int text_size, rodata_size, plt_size, got_size;
  int text_exist, rodata_exist, plt_exist, got_exist;
  int text_index, rodata_index, plt_index, got_index;
  char *buf;
  int all_size=0, temp=0, i;
  int patch_fd;
  PLT_GOT_INFO *plt_info, *got_info;
  FILE *patch_fp;

  patch_fd = open(patch_filename, O_RDONLY);
  patch_fp = fdopen(patch_fd, "rb");

  if(text_exist = get_sec_offset(patch_elf_info, ".text", &text_offset)){
    get_sec_size(patch_elf_info, ".text", &text_size);
    all_size += text_size;
  }
  if(rodata_exist = get_sec_offset(patch_elf_info, ".rodata", &rodata_offset)){
    get_sec_size(patch_elf_info, ".rodata", &rodata_size);
    all_size += rodata_size;
  }
  plt_info = make_plt_info(patch_elf_info);
  if(plt_exist = plt_info->exist){
    plt_size = plt_info->size;
    all_size += plt_size;
  }
  got_info = make_got_info(patch_elf_info);
  if(got_exist = got_info->exist){
    got_size = got_info->size;
    all_size += got_size;
  }
     
  *size = all_size;
  buf = (char*)malloc(all_size);
  
  if(text_exist){
    fseek(patch_fp, text_offset, SEEK_SET);
    fread(&buf[temp], 1, text_size, patch_fp);
    text_index = temp;
    temp += text_size;
  }
  if(rodata_exist){
    fseek(patch_fp, rodata_offset, SEEK_SET);
    fread(&buf[temp], 1, rodata_size, patch_fp);
    rodata_index = temp;
    temp += rodata_size;
  }
  if(plt_exist){
    memset(&buf[temp], 0, plt_size);
    plt_index = temp;
    temp += plt_size;
  }
  if(got_exist){
    memset(&buf[temp], 0, got_size);
    got_index = temp;
    temp += got_size;
  }
  

  relocate(target_elf_info, patch_elf_info, buf, text_index, rodata_index, plt_index, got_index, plt_info, got_info); 

  
  fclose(patch_fp);

  return buf;
}


