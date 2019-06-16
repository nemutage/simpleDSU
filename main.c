#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tool.h"


//[1]pid, [2]target_filename, [3]patch_filename [4]target_symbol
int main(int argc, char** argv){
    int i;
    pid_t pid;
    int patch_size;
    char *patch_obj;
    char *target_filename, *patch_filename;
    char *target_symbol;
    long int target_sym_addr;
    long int mmap_addr;
    ELF_INFO *target_elf_info, *patch_elf_info;


    pid = atoi(argv[1]);
    target_filename = argv[2];
    patch_filename = argv[3];
    target_symbol = argv[4];


    target_elf_info = make_elf_info(target_filename);
    patch_elf_info = make_elf_info(patch_filename);

    
    get_sym_addr(target_elf_info, target_symbol, &target_sym_addr);  
    patch_obj = make_patch_obj(target_elf_info, patch_filename, patch_elf_info, &patch_size);

   

    if(ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0){
      perror("attach error\n");
    }

    mmap_addr = alloc_mmap_area(pid, patch_size);
    set_data(pid, mmap_addr, patch_obj, patch_size);
    set_jump(pid, target_sym_addr, mmap_addr);

    if(ptrace(PTRACE_DETACH, pid, NULL, NULL) < 0){
      perror("detach error\n");
    }
    
    

    for(i = 0; i < patch_size; i++){
      printf("%02x ", patch_obj[i] & 0x000000ff);
      if((i+1)%7==0)printf("\n");
    }
    printf("\n\n%llx\n\n", mmap_addr);
    
    
  return 0;  
}
