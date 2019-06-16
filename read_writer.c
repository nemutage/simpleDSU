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


int set_data(pid_t pid, long int addr, void *val, int vlen){
  int i;
  long int addr0 = addr & ~7;
  int len = (((addr + vlen) - addr0) + 7)/8;
  long int *lv = malloc(len * sizeof(long int));

  for(i = 0; i < len; i++) {
    lv[i] = ptrace(PTRACE_PEEKDATA, pid, addr0 + i * sizeof(long int), NULL);
  }
  memcpy((char *)lv + (addr - addr0), val, vlen);
  for (i = 0; i < len; i++) {
    if(ptrace(PTRACE_POKEDATA, pid, addr0 + i * sizeof(long int), lv[i]) < 0) {
      perror("error : ptrace poke");
      return -1;
    }
  }
  return 0;
}


int get_data(pid_t pid, long int addr, void *val, int vlen){
  int i;
  long int addr0 = addr & ~7;
  int len = (((addr + vlen) - addr0) + 7)/8;
  long int *lv = malloc(len * sizeof(long int));

  for(i = 0; i < len; i++) {
    lv[i] = ptrace(PTRACE_PEEKDATA, pid, addr0 + i * sizeof(long int), NULL);
  }
  memcpy(val, (char *)lv + (addr - addr0), vlen);
  return 0;
}


void set_jump(pid_t pid, long int from_addr, long int to_addr){
  int i;
  char *ch;
  char jmp_cord[14] = {0xff, 0x25, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00};

  ch = (char *)&to_addr;
  for(i=0;i<8;i++){
    jmp_cord[i+6] = ch[i];
  }
  
  set_data(pid, from_addr, jmp_cord, 14); 
}
